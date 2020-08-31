#include<stdio.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<conio.h>
#include "control.h"
#include "database.h"
#include "resolve.h"
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char* argv[]) {

	if (dealOpts(argc, argv)) { /*�ɹ���ȡ����*/
		printf("Debug level %d\n", gDebugLevel);
		printf("Name server %s:53\n", addrDNSserv);
		printf("Database using %s\n", gDBtxt);
	} else {					/*��ȡ����ʧ��*/
		printf("Please use the following format:\n"
			"dnsrelay [-d|-dd] [dns-server-ipaddr] [filename]");
		return 0;
	}

	if (!BuildDNSDatabase()) {
		printf("Failed to build the database.\n");
		return 0;
	}

	WSADATA wsaData;								/*Э��汾��Ϣ*/
	SOCKADDR_IN addrSrv;							/*�����(dnsrelay)��ַ*/
	SOCKADDR_IN addrCli;							/*�ͻ��˵�ַ*/
	int addrCliSize = sizeof(addrCli);				/*�ͻ��˵�ַ�Ĵ�С*/
	int Port = 53;									/*socket�󶨶˿�*/
	char ipStrBuf[20] = { '\0' };					/*���IP��ַ��ջ�ڴ�*/
	event_type event;								/*�����¼�*/
	unsigned char recvBuf[MAX_BUFSIZE] = { '\0' };	/*���ջ���*/
	unsigned char sendBuf[MAX_BUFSIZE] = { '\0' };	/*���ͻ���*/
	int recvByte = 0;								/*recvBuf��ŵı��Ĵ�С*/
	int sendByte = 0;								/*sendBuf��ŵı��Ĵ�С*/
	int front;                                       /*ÿ���жϳ�ʱʱ�洢��ͷ*/

	/* ��ȡsocket�汾2.2 */
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Server: WSAStartup failed with error %ld\n", WSAGetLastError());
		return 0;
	}

	/*
		����socket:
		��ַ��:	AF_INET		-- IPv4
		����:	SOCK_DGRAM	-- udp���ݱ�
		Э��:	UDP			-- udp
	*/
	SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockSrv == INVALID_SOCKET) {
		printf("Invalid socket error %ld\n", WSAGetLastError());
		WSACleanup();/* clean up */
		return 0;
	} else printf("Socket() is OK!\n");

	/* �����ַ */
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(Port);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/* ��������IP���������� */

	/* ��socket */
	if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(addrSrv)) == SOCKET_ERROR) {
		/* �󶨴��� */
		printf("Failed to bind() with error %ld\n", WSAGetLastError());
		closesocket(sockSrv);	/* �ر�socket */
		WSACleanup();			/* clean up */
		return 0;
	} else printf("Bind() is OK!\n");

	/* ��ӡ socket ��Ϣ */
	/*getsockname(sockSrv, (SOCKADDR*)&addrSrv, (int*)sizeof(addrSrv));*/
	/*printf("Server: Receiving IP(s) used : % s\n", inet_ntop(AF_INET, (void*)&addrSrv.sin_addr, ipStrBuf, 16));*/
	/*printf("Server: Receiving port used : % d\n", htons(addrSrv.sin_port));*/
	/*printf("Server: I\'m ready to receive a datagram...\n");*/

	InitCTable();/*��ʼ��clientTableΪ�ն���*/

	/* ��ʼ�޾���ѭ��, ���� Esc �˳�ѭ�� */
	while (!(_kbhit() && _getch() == 27)) {


		event = WaitForEvent();
		switch (event)
		{
		case timeout:
			printf("Timeout.\n");
			break;

		case dgram_arrival:

			/*���buffer*/
			ClearBuffer(recvBuf, recvByte);
			ClearBuffer(sendBuf, sendByte);

			/*���ձ���*/
			printf("datagram arrived. Recieving...\n");
			recvByte = recvfrom(sockSrv, (char*)recvBuf, MAX_BUFSIZE, 0, (SOCKADDR*)&addrCli, &addrCliSize);
			if (recvByte <= 0) {
				printf("recvfrom() failed with error %ld\n", WSAGetLastError());
				recvByte = 0; /*����*/
				break;
			} else
				printf("datagram received. %d Bytes in all.\n", recvByte);

			/*�жϽ��յ���response����query,���±���sendBuf,���·���Ŀ��addrCli(��һ���ǿͻ���,Ҳ������DNS������*/
			if ((recvBuf[2] & 0x80) >> 7 == 0) {
				printf("�յ�һ�������ġ���������:\n");
				DebugBuffer(recvBuf, recvByte);	/*��ӡbuffer--debug*/
				sendByte = ResolveQuery(recvBuf, sendBuf, recvByte, &addrCli);
				if (sendByte < 0) { /*����queryʧ��*/
					printf("failed to solved query.\n");
					break;
				}
			} else {
				printf("�յ�һ����Ӧ���ġ���������:\n");
				DebugBuffer(recvBuf, recvByte);/*��ӡbuffer--debug*/
				sendByte = ResolveResponse(recvBuf, sendBuf, recvByte, &addrCli);
				if (sendByte < 0) { /*����responseʧ��*/
					printf("failed to solved response.\n");
					break;
				}
			}

			/*���ͱ���*/
			printf("datagram sending to %s:%d\n",
				inet_ntop(AF_INET, (void*)&addrCli.sin_addr, ipStrBuf, 16),
				htons(addrCli.sin_port)
			);
			addrCliSize = sizeof(addrCli);
			sendto(sockSrv, (char*)sendBuf, sendByte, 0, (SOCKADDR*)&addrCli, addrCliSize);
			printf("datagram sending succeed %d Bytes in all.\n", sendByte);

			/*��ӡ���͵�����--debug*/
			if ((sendBuf[2] & 0x80) >> 7 == 0) {
				printf("����һ�������ġ���������:\n");
				DebugBuffer(sendBuf, sendByte);/*��ӡbuffer*/
			} else {
				printf("����һ����Ӧ���ġ���������:\n");
				DebugBuffer(sendBuf, sendByte);/*��ӡbuffer*/
			}

			/*if sendByte == sendto() ����ֽ���*/

			break;

		default:
			break;
		}
	}





	/* ���н���, socket�ر� */
	if (closesocket(sockSrv) != 0)
		printf("close socket failed with error %ld\n", GetLastError());
	else
		printf("socket closed.\n");

	/* ���н���, ������ */
	if (WSACleanup() != 0)
		printf("WSA clean up failed with error %ld\n", GetLastError());
	else
		printf("clean up is OK.\n");


	return 0;
}