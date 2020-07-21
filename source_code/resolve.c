#include "resolve.h"

extern const char addrDNSserv[] = "192.168.0.1"; /*����*/

extern int ResolveQuery(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	if (1) {
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
		} else {
			return -1;
		}
	} else { //�ҵõ���ֱ�ӷ��ͽ��
		/*sendBuf[2] |= 0x80;*//*���ֽ���1000 0000���а�λ������,�������λ��Ϊ1*/
	}
}

extern int ResolveResponse(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	return recvByte;
}