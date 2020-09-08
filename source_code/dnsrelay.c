#include<stdio.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<conio.h>
#include<time.h>
#include "control.h"
#include "database.h"
#include "resolve.h"
#pragma comment(lib,"ws2_32.lib")

event_type WaitForEvent(SOCKET fd) {
	/*
		TODO, �������ڲ��÷����¼����ͻ��Ƶ�ԭ��
	*/

	UpdateCache(); /*����cache*/

	while (1)
	{
		//DebugCTable(); /*�鿴Client���пռ�*/


		CRecord record = { 0 };
		if (CTableUsage()
			&& FindCRecord(GetCTableFrontIndex(), &record)
			&& record.r) {
			PopCRecord();
			continue;
		}
		//FindCRecord(GetCTableFrontIndex(), &record);
		// debugPrintf("%d, %d\n", record.expireTime, record.originId);
		if (CTableUsage()
			&& FindCRecord(GetCTableFrontIndex(), &record)
			&& record.expireTime
			&& time(NULL) > record.expireTime) {
			/*
				��ʱ����, ֱ�Ӵӻ�����ɾ��, ��������:
				1. ������select()ģ�͵ȴ���ʱ��Ϊ1s, ��nslookup������ش������2s��
				   �м�����鵽֮�����ش�û�����塣
			*/
			debugPrintf("ɾ��\n");
			PopCRecord();
			continue;
		}

		else
			break;
	}

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	struct timeval t = { 1,0 }; /*����һ���ӵ�����ʱ��*/
	select(0, &readfds, 0, 0, &t);

	if (FD_ISSET(fd, &readfds)) {	/*�ɶ�*/
		return dgram_arrival;
	}

	return nothing;
}

int main(int argc, char* argv[]) {

	if (dealOpts(argc, argv)) { /*�ɹ���ȡ����*/
		debugPrintf("Debug level %d\n", gDebugLevel);
		debugPrintf("Name server %s:53\n", addrDNSserv);
		debugPrintf("Database using %s\n", gDBtxt);
	} else {					/*��ȡ����ʧ��*/
		debugPrintf("Please use the following format:\n"
			"dnsrelay [-d|-dd] [dns-server-ipaddr] [filename]");
		return 0;
	}

	if (!BuildDNSDatabase()) {
		debugPrintf("Failed to build the database.\n");
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
	//int front;                                    /*ÿ���жϳ�ʱʱ�洢��ͷ*/

	/* ��ȡsocket�汾2.2 */
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		debugPrintf("Server: WSAStartup failed with error %ld\n", WSAGetLastError());
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
		debugPrintf("Invalid socket error %ld\n", WSAGetLastError());
		WSACleanup();/* clean up */
		return 0;
	} else debugPrintf("Socket() is OK!\n");

	/* �����ַ */
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(Port);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/* ��������IP���������� */

	/* ��socket */
	if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(addrSrv)) == SOCKET_ERROR) {
		/* �󶨴��� */
		debugPrintf("Failed to bind() with error %ld\n", WSAGetLastError());
		closesocket(sockSrv);	/* �ر�socket */
		WSACleanup();			/* clean up */
		return 0;
	} else debugPrintf("Bind() is OK!\n");

	/* ��ӡ socket ��Ϣ */
	/*getsockname(sockSrv, (SOCKADDR*)&addrSrv, (int*)sizeof(addrSrv));*/
	/*printf("Server: Receiving IP(s) used : % s\n", inet_ntop(AF_INET, (void*)&addrSrv.sin_addr, ipStrBuf, 16));*/
	/*printf("Server: Receiving port used : % d\n", htons(addrSrv.sin_port));*/
	/*printf("Server: I\'m ready to receive a datagram...\n");*/

	InitCTable();/*��ʼ��clientTableΪ�ն���*/

	/* ��ʼ�޾���ѭ��, ���� Esc �˳�ѭ�� */
	while (!(_kbhit() && _getch() == 27)) {

		event = WaitForEvent(sockSrv);
		switch (event)
		{
		case timeout:
			debugPrintf("Timeout.\n");
			break;

		case dgram_arrival:

			/*���buffer*/
			ClearBuffer(recvBuf, recvByte);
			ClearBuffer(sendBuf, sendByte);

			/*���ձ���*/
			debugPrintf("datagram arrived. Recieving...\n");
			recvByte = recvfrom(sockSrv, (char*)recvBuf, MAX_BUFSIZE, 0, (SOCKADDR*)&addrCli, &addrCliSize);
			if (recvByte <= 0) {
				debugPrintf("recvfrom() failed with error %ld\n", WSAGetLastError());
				recvByte = 0; /*����*/
				break;
			} else
				debugPrintf("datagram received. %d Bytes in all.\n", recvByte);

			/*�жϽ��յ���response����query,���±���sendBuf,���·���Ŀ��addrCli(��һ���ǿͻ���,Ҳ������DNS������*/
			if ((recvBuf[2] & 0x80) >> 7 == 0) {
				debugPrintf("�յ�һ�������ġ���������:\n");
				//DebugBuffer(recvBuf, recvByte);	/*��ӡbuffer--debug*/
				sendByte = ResolveQuery(recvBuf, sendBuf, recvByte, &addrCli);
				if (sendByte < 0) { /*����queryʧ��*/
					debugPrintf("failed to solved query.\n");
					break;
				}
			} else {
				debugPrintf("�յ�һ����Ӧ���ġ���������:\n");
				//DebugBuffer(recvBuf, recvByte);/*��ӡbuffer--debug*/
				sendByte = ResolveResponse(recvBuf, sendBuf, recvByte, &addrCli);
				if (sendByte < 0) { /*����responseʧ��*/
					debugPrintf("failed to solved response.\n");
					break;
				}
			}

			/*���ͱ���*/
			debugPrintf("datagram sending to %s:%d\n",
				inet_ntop(AF_INET, (void*)&addrCli.sin_addr, ipStrBuf, 16),
				htons(addrCli.sin_port)
			);
			addrCliSize = sizeof(addrCli);
			sendto(sockSrv, (char*)sendBuf, sendByte, 0, (SOCKADDR*)&addrCli, addrCliSize);
			debugPrintf("datagram sending succeed %d Bytes in all.\n", sendByte);

			/*��ӡ���͵�����--debug*/
			if ((sendBuf[2] & 0x80) >> 7 == 0) {
				debugPrintf("����һ�������ġ���������:\n");
				//DebugBuffer(sendBuf, sendByte);/*��ӡbuffer*/
			} else {
				debugPrintf("����һ����Ӧ���ġ���������:\n");
				//DebugBuffer(sendBuf, sendByte);/*��ӡbuffer*/
			}

			/*if sendByte == sendto() ����ֽ���*/

			break;

		default:
			break;
		}
	}

	/* ���н���, socket�ر� */
	if (closesocket(sockSrv) != 0)
		debugPrintf("close socket failed with error %ld\n", GetLastError());
	else
		debugPrintf("socket closed.\n");

	/* ���н���, ������ */
	if (WSACleanup() != 0)
		debugPrintf("WSA clean up failed with error %ld\n", GetLastError());
	else
		debugPrintf("clean up is OK.\n");


	return 0;
}