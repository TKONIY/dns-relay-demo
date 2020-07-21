#include "resolve.h"

const char addrDNSserv[] = "202.106.0.20"; /*����*/

/*
	Description:	�ֶ�������(n)�����ݿ�������(p)���໥ת��(�ο�������->�����������)
	params:
		nName		�����ϵ����� (����)
		pName		�����ϵ����� (����)
	return :
		0			ʧ��
		1			�ɹ�
*/
static char domainName_ntop(const unsigned char* nName, unsigned char* pName) {
	/*��һ���ַ���Ҫ*/
	
	memcpy(pName, nName+1, strlen((char*)nName));/*��������Ҳ��\0����,����ʹ��strlen*/
	int i = nName[0];/*��һ�����λ��*/
	while (pName[i]) {						/*������0ʱ����*/
		int offset = pName[i];				/*��һ�εĳ���*/
		pName[i] = '.';						/*�ָ�*/
		printf("i=%d\n", i);
		i += offset+1;						/*������һ���ָ���*/
	}
	return 1;
}

extern int ResolveQuery(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	char ip[16] = { '\0' };						/*���ip*/
	char domainName[100] = { '\0' };			/*�������*/
	domainName_ntop(recvBuf + 12, domainName);	/*��ȡ����*/

	if (FindInDNSDatabase(domainName, ip)) {	/*����ҵ���¼*/

		memcpy(sendBuf, recvBuf, recvByte);		/*���������ݸ��Ƶ����ͻ���*/
		sendBuf[2] |= 0x80;						/*QR=response*/
		sendBuf[2] |= 0x05;						/*Authority=1*/
		sendBuf[3] |= 0x80;						/*RA=1*/
		sendBuf[7] |= 0x01;						/*Answer count = 1*/
		unsigned char answer[16] = { 0 };		/*���answer����Ϊ0*/
		answer[0] = 0xc0;						/*ʹ��ָ���ʾ����*/
		answer[1] = 0x0c;						/*�����ĵ�ַΪ��12�ֽ�*/
		answer[3] = 0x01;						/*TYPE = 1*/
		answer[5] = 0x01;						/*CLASS = 1*/
		answer[9] = 0x78;						/*TTL = 120s*/
		answer[11] = 0x04;						/*DATA LENGTH = 4*/
		inet_pton(AF_INET, ip, answer + 12);	/*IP��ַ*/
		memcpy(sendBuf + recvByte, answer, 16);	/*��answer���ӵ�query����*/
		return recvByte + 16;					/*sendBuf�Ĵ�С*/

	} else {

		/*��������ݿ����Ҳ���,��ֱ��ת��������DNS������---*/
		DNSHeader* header = (DNSHeader*)recvBuf;
		printf("�յ�IDΪ%x��������\n", ntohs(header->ID));
		DNSID newID = ntohs(header->ID);
		if (PushCRecord((SOCKADDR*)addrCli,&newID)) {/*����ɹ��������*/
			/*��д���ͻ���*/
			memcpy(sendBuf, recvBuf, recvByte);
			/*����ת��ID*/
			header = (DNSHeader*)sendBuf;
			header->ID = htons(newID);
			/*����Ŀ���ַ*/
			addrCli->sin_family = AF_INET;
			addrCli->sin_port = htons(53);
			inet_pton(AF_INET, addrDNSserv, &addrCli->sin_addr);
			return recvByte;
		} else { /*����������ʧ��*/
			return -1;
		}
	} 
}

extern int ResolveResponse(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	return recvByte;
}