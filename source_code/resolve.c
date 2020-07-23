#include "resolve.h"


/*
	Description:	字段中域名(n)与数据库中域名(p)的相互转换(参考网络序->主机序的命名)
	params:
		nName		网络上的域名 (传入)
		pName		主机上的域名 (传出)
	return :
		0			失败
		1			成功
*/
static char domainName_ntop(const unsigned char* nName, unsigned char* pName) {
	/*第一个字符不要*/
	
	memcpy(pName, nName+1, strlen((char*)nName));/*网络域名也以\0结束,可以使用strlen*/
	int i = nName[0];/*第一个点的位置*/
	while (pName[i]) {						/*遍历到0时结束*/
		int offset = pName[i];				/*下一段的长度*/
		pName[i] = '.';						/*分隔*/
		//printf("i=%d\n", i);				/*调试用*/
		i += offset+1;						/*跳到下一个分隔处*/
	}
	return 1;
}

extern int ResolveQuery(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	char ip[16] = { '\0' };						/*存放ip*/
	char domainName[100] = { '\0' };			/*存放域名*/
	domainName_ntop(recvBuf + 12, domainName);	/*获取域名*/
	if (FindInDNSDatabase(domainName, ip)) {	/*如果找到记录*/

		memcpy(sendBuf, recvBuf, recvByte);		/*将接收内容复制到发送缓冲*/
		sendBuf[2] |= 0x80;						/*QR=response*/
		sendBuf[2] |= 0x05;						/*Authority=1*/
		sendBuf[3] |= 0x80;						/*RA=1*/
		sendBuf[7] |= 0x01;						/*Answer count = 1*/
		unsigned char answer[16] = { 0 };		/*填充answer区域为0*/
		answer[0] = 0xc0;						/*使用指针表示域名*/
		answer[1] = 0x0c;						/*域名的地址为第12字节*/
		answer[3] = 0x01;						/*TYPE = 1*/
		answer[5] = 0x01;						/*CLASS = 1*/
		answer[9] = 0x78;						/*TTL = 120s*/
		answer[11] = 0x04;						/*DATA LENGTH = 4*/
		inet_pton(AF_INET, ip, answer + 12);	/*IP地址*/
		memcpy(sendBuf + recvByte, answer, 16);	/*将answer附加到query后面*/
		return recvByte + 16;					/*sendBuf的大小*/

	} else {

		/*如果在数据库中找不到,则直接转发给本地DNS服务器---*/
		DNSHeader* header = (DNSHeader*)recvBuf;
		printf("收到ID为%x的请求报文\n", ntohs(header->ID));
		DNSID newID = ntohs(header->ID);
		if (PushCRecord(addrCli, &newID)) {/*如果成功加入队列*/
			/*填写发送缓冲*/
			memcpy(sendBuf, recvBuf, recvByte);
			/*更新转发ID*/
			header = (DNSHeader*)sendBuf;
			header->ID = htons(newID);
			/*更新目标地址*/
			addrCli->sin_family = AF_INET;
			addrCli->sin_port = htons(53);
			inet_pton(AF_INET, addrDNSserv, &addrCli->sin_addr);
			return recvByte;
		} else { /*队列已满，失败*/
			return -1;
		}
	} 
}

extern int ResolveResponse(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	unsigned char tempBuf[MAX_BUFSIZE] = { '\0' };/*FindCRecord函数貌似会改变recvBuf,tempBuf记录改变前的*/
	/*FindCRecord要用到的两个参数*/
	DNSHeader* header = (DNSHeader*)recvBuf;
	CRecord* pRecord = (CRecord*)recvBuf;
	printf("收到ID为%x的响应报文\n", ntohs(header->ID));
	/*外部DNS给出的ID是newID，FindCRecord前记录Buf*/
	DNSID newID = ntohs(header->ID);
	memcpy(tempBuf, recvBuf, recvByte);

	if(FindCRecord((DNSID) newID, (CRecord*) pRecord)==1) {/*如果在clientTable中找到newID记录*/
		memcpy(sendBuf, tempBuf, recvByte);       /*接受内容复制到发送缓存*/
		/*将newID换成originID*/
		header = (DNSHeader*)sendBuf;			  
		header->ID = htons(pRecord->originId);
		//printf("orignID is %x\n", htons(pRecord->originId)); Debug语句便于区分我用//注释了
		/*发送地址族IPv4,地址及端口:从CRecord中获取当前ID对应的源地址及端口*/
		addrCli->sin_family = AF_INET;
		addrCli->sin_port = pRecord->addr.sin_port;
		inet_pton(AF_INET, addrDNSclie, &addrCli->sin_addr);
		//printf("address: %x\n", addrCli->sin_addr);
		return recvByte;
	}
	else {
		return -1;
	}
}