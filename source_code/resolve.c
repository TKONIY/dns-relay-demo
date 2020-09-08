#include "resolve.h"


/*
	Description:	�ֶ�������(n)�����ݿ�������(p)���໥ת��(�ο�������->�����������)
	params:
		nName		�����ϵ����� (����)
		pName		�����ϵ����� (����)
	return :
		0			ʧ��
		1			�ɹ�
*/
static int domainName_ntop(const unsigned char* nName, unsigned char* pName) {
	/*��һ���ַ���Ҫ*/

	memcpy(pName, nName + 1, strlen((char*)nName));/*��������Ҳ��\0����,����ʹ��strlen*/
	int length;
	length = strlen((char*)nName) + 1;
	int i = nName[0];/*��һ�����λ��*/
	while (pName[i]) {						/*������0ʱ����*/
		int offset = pName[i];				/*��һ�εĳ���*/
		pName[i] = '.';						/*�ָ�*/
		/*printf("i=%d\n", i);*/			/*������*/
		i += offset + 1;						/*������һ���ָ���*/
	}
	return length;
}

extern int ResolveQuery(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	char ip[16] = { '\0' };						/*���ip*/
	char domainName[MAX_DOMAINNAME] = { '\0' };			/*�������*/
	domainName_ntop(recvBuf + 12, domainName);	/*��ȡ����*/
	if (FindInDNSDatabase(domainName, ip)) {	/*����ҵ���¼*/
		debugPrintf("�����ݿ����ҵ��˼�¼ %s : %s\n", domainName, ip);
		memcpy(sendBuf, recvBuf, recvByte);		/*���������ݸ��Ƶ����ͻ���*/
		sendBuf[2] |= 0x80;						/*QR=response*/
		sendBuf[2] |= 0x05;						/*Authority=1*/
		sendBuf[3] |= 0x80;						/*RA=1*/

		if (!strcmp(ip, "0.0.0.0")) {			/*������Ч*/
			sendBuf[7] |= 0x00;					/*Answer count = 0*/
			sendBuf[3] |= 0x03;					/*����������*/
			return recvByte;					/*���ĳ���Ϊ�ײ�����*/
		}

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
		debugPrintf("û�������ݿ����ҵ���¼: %s\n", domainName);
		/*��������ݿ����Ҳ���,��ֱ��ת��������DNS������---*/
		DNSHeader* header = (DNSHeader*)recvBuf;
		debugPrintf("�յ�IDΪ%x��������\n", ntohs(header->ID));
		DNSID newID = ntohs(header->ID);
		if (PushCRecord(addrCli, &newID)) {/*����ɹ��������*/
			//SetTime();
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
	//char temttl[16] = { '\0' };
	char ip[16] = { '\0' };								/*���ip*/
	char domainName[MAX_DOMAINNAME] = { '\0' };			/*�������*/
	int domainlen;

	domainlen = domainName_ntop(recvBuf + 12, domainName);	/*��ȡ����*/
	inet_ntop(AF_INET, recvBuf + (12 + domainlen + 16), ip, 16);
	int ttl = ntohl(*((int*)(recvBuf + (12 + domainlen + 10))));
	//int* pttl = recvBuf + (12 + domainlen + 10);
	if (!recvBuf[3]) { /*Z�ֶ���ԶΪ0, RCODEΪ0��ʾ����*/
		InsertIntoDNSCache(domainName, ip, ttl);
	}

	//printf("*pttl: %d\n", *pttl);
	//printf("char2ttl: %d\n", recvBuf[12 + domainlen + 13]);

	//DebugBuffer(recvBuf + (12 + domainlen + 10), 8);
	debugPrintf("ttl�ڴ�:%d\n", ttl);
	//memcpy(ip, tempBuf + (recvByte - 4), 4);
	//int ttl;
	//unsigned char tempBuf[MAX_BUFSIZE] = { '\0' };/*FindCRecord����ò�ƻ�ı�recvBuf,tempBuf��¼�ı�ǰ��*/
	/*FindCRecordҪ�õ�����������*/
	//memcpy(tempBuf, recvBuf, recvByte);

	//memcpy(ip, tempBuf + (recvByte - 4), 4);
	//memcpy(temttl, tempBuf + (recvByte - 10), 4);
	//DebugBuffer(temttl, 4);
	//DebugBuffer(ip,4);
	//CRecord* pRecord = (CRecord*)tempBuf;	/*TODO!!! ʲô��, ��ĵ�,*/
	const DNSHeader* recvHeader = (DNSHeader*)recvBuf;
	debugPrintf("�յ�IDΪ%x����Ӧ����\n", ntohs(recvHeader->ID));
	/*�ⲿDNS������ID��newID��FindCRecordǰ��¼Buf*/
	DNSID newID = ntohs(recvHeader->ID);

	/*TODO*/
	/*
		�Ѳ�ѯ���Ľ�����µ�cache, ��Ҫ��DNS�����л�ȡTTL, ����, IP;
	*/

	CRecord record = { 0 };
	CRecord* pRecord = &record;


	if (FindCRecord((DNSID)newID, (CRecord*)pRecord) == 1) {/*�����clientTable���ҵ�newID��¼*/
		if (pRecord->r == 0) {
			SetCRecordR(newID); /*δ�ظ���ظ�����r��Ϊ1*/
			/*printf("IDΪ %hu �ı����Ѿ��ظ�\n", newID);*/
		} else {
			return -1; /*�ظ����Ͳ�������*/
		}
		memcpy(sendBuf, recvBuf, recvByte);			/*�������ݸ��Ƶ����ͻ���*/
		/*��newID����originID*/
		DNSHeader* sendHeader = (DNSHeader*)sendBuf;	/*header��Ϊ*/
		sendHeader->ID = htons(pRecord->originId);
		/*printf("orignID is %x\n", htons(pRecord->originId)); */
		/*���͵�ַ��IPv4,��ַ���˿�:��CRecord�л�ȡ��ǰID��Ӧ��Դ��ַ���˿�*/
		/*addrCli->sin_family = AF_INET;
		addrCli->sin_port = pRecord->addr.sin_port;
		addrCli->sin_addr = pRecord->addr.sin_addr;*/
		*addrCli = pRecord->addr;
		//inet_pton(AF_INET, pRecord->addr., &addrCli->sin_addr);
		/*printf("address: %x\n", addrCli->sin_addr);*/
		return recvByte;
	} else {
		return -1;
	}
}

