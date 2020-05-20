#include "resolve.h"

extern char addrDNSserv[] = "192.168.0.1";

extern int ResolveQuery(unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	if (1) {
		/*如果在数据库中找不到,则直接转发给本地DNS服务器---*/



		DNSHEADER* header = (DNSHEADER*)recvBuf;
		printf("收到ID为%x的请求报文\n", ntohs(header->ID));
		DNSID newID = ntohs(header->ID);
		if (PushCRecord(addrCli,&newID)) {/*如果成功加入队列*/
			/*填写发送缓冲*/
			memcpy(sendBuf, recvBuf, recvByte);
			/*更新转发ID*/
			header = (DNSHEADER*)sendBuf;
			header->ID = htons(newID);
			/*更新目标地址*/
			addrCli->sin_family = AF_INET;
			addrCli->sin_port = htons(53);
			inet_pton(AF_INET, addrDNSserv, &addrCli->sin_addr);

			return recvByte;
		} else {
			return -1;
		}


	} else {
		/*sendBuf[2] |= 0x80;*//*该字节与1000 0000进行按位或运算,即将最高位置为1*/
	}
}

extern int ResolveResponse(unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	return recvByte;
}