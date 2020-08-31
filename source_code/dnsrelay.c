#include<stdio.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<conio.h>
#include "control.h"
#include "database.h"
#include "resolve.h"
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char* argv[]) {

	if (dealOpts(argc, argv)) { /*成功获取参数*/
		printf("Debug level %d\n", gDebugLevel);
		printf("Name server %s:53\n", addrDNSserv);
		printf("Database using %s\n", gDBtxt);
	} else {					/*获取参数失败*/
		printf("Please use the following format:\n"
			"dnsrelay [-d|-dd] [dns-server-ipaddr] [filename]");
		return 0;
	}

	if (!BuildDNSDatabase()) {
		printf("Failed to build the database.\n");
		return 0;
	}

	WSADATA wsaData;								/*协议版本信息*/
	SOCKADDR_IN addrSrv;							/*服务端(dnsrelay)地址*/
	SOCKADDR_IN addrCli;							/*客户端地址*/
	int addrCliSize = sizeof(addrCli);				/*客户端地址的大小*/
	int Port = 53;									/*socket绑定端口*/
	char ipStrBuf[20] = { '\0' };					/*存放IP地址的栈内存*/
	event_type event;								/*接收事件*/
	unsigned char recvBuf[MAX_BUFSIZE] = { '\0' };	/*接收缓冲*/
	unsigned char sendBuf[MAX_BUFSIZE] = { '\0' };	/*发送缓冲*/
	int recvByte = 0;								/*recvBuf存放的报文大小*/
	int sendByte = 0;								/*sendBuf存放的报文大小*/
	int front;                                       /*每次判断超时时存储队头*/

	/* 获取socket版本2.2 */
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Server: WSAStartup failed with error %ld\n", WSAGetLastError());
		return 0;
	}

	/*
		创建socket:
		地址族:	AF_INET		-- IPv4
		类型:	SOCK_DGRAM	-- udp数据报
		协议:	UDP			-- udp
	*/
	SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockSrv == INVALID_SOCKET) {
		printf("Invalid socket error %ld\n", WSAGetLastError());
		WSACleanup();/* clean up */
		return 0;
	} else printf("Socket() is OK!\n");

	/* 构造地址 */
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(Port);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/* 本机所有IP都可以连接 */

	/* 绑定socket */
	if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(addrSrv)) == SOCKET_ERROR) {
		/* 绑定错误 */
		printf("Failed to bind() with error %ld\n", WSAGetLastError());
		closesocket(sockSrv);	/* 关闭socket */
		WSACleanup();			/* clean up */
		return 0;
	} else printf("Bind() is OK!\n");

	/* 打印 socket 信息 */
	/*getsockname(sockSrv, (SOCKADDR*)&addrSrv, (int*)sizeof(addrSrv));*/
	/*printf("Server: Receiving IP(s) used : % s\n", inet_ntop(AF_INET, (void*)&addrSrv.sin_addr, ipStrBuf, 16));*/
	/*printf("Server: Receiving port used : % d\n", htons(addrSrv.sin_port));*/
	/*printf("Server: I\'m ready to receive a datagram...\n");*/

	InitCTable();/*初始化clientTable为空队列*/

	/* 开始无尽的循环, 按下 Esc 退出循环 */
	while (!(_kbhit() && _getch() == 27)) {


		event = WaitForEvent();
		switch (event)
		{
		case timeout:
			printf("Timeout.\n");
			break;

		case dgram_arrival:

			/*清空buffer*/
			ClearBuffer(recvBuf, recvByte);
			ClearBuffer(sendBuf, sendByte);

			/*接收报文*/
			printf("datagram arrived. Recieving...\n");
			recvByte = recvfrom(sockSrv, (char*)recvBuf, MAX_BUFSIZE, 0, (SOCKADDR*)&addrCli, &addrCliSize);
			if (recvByte <= 0) {
				printf("recvfrom() failed with error %ld\n", WSAGetLastError());
				recvByte = 0; /*置零*/
				break;
			} else
				printf("datagram received. %d Bytes in all.\n", recvByte);

			/*判断接收的是response还是query,更新报文sendBuf,更新发送目标addrCli(不一定是客户端,也可能是DNS服务器*/
			if ((recvBuf[2] & 0x80) >> 7 == 0) {
				printf("收到一个请求报文。内容如下:\n");
				DebugBuffer(recvBuf, recvByte);	/*打印buffer--debug*/
				sendByte = ResolveQuery(recvBuf, sendBuf, recvByte, &addrCli);
				if (sendByte < 0) { /*处理query失败*/
					printf("failed to solved query.\n");
					break;
				}
			} else {
				printf("收到一个响应报文。内容如下:\n");
				DebugBuffer(recvBuf, recvByte);/*打印buffer--debug*/
				sendByte = ResolveResponse(recvBuf, sendBuf, recvByte, &addrCli);
				if (sendByte < 0) { /*处理response失败*/
					printf("failed to solved response.\n");
					break;
				}
			}

			/*发送报文*/
			printf("datagram sending to %s:%d\n",
				inet_ntop(AF_INET, (void*)&addrCli.sin_addr, ipStrBuf, 16),
				htons(addrCli.sin_port)
			);
			addrCliSize = sizeof(addrCli);
			sendto(sockSrv, (char*)sendBuf, sendByte, 0, (SOCKADDR*)&addrCli, addrCliSize);
			printf("datagram sending succeed %d Bytes in all.\n", sendByte);

			/*打印发送的内容--debug*/
			if ((sendBuf[2] & 0x80) >> 7 == 0) {
				printf("发送一个请求报文。内容如下:\n");
				DebugBuffer(sendBuf, sendByte);/*打印buffer*/
			} else {
				printf("发送一个响应报文。内容如下:\n");
				DebugBuffer(sendBuf, sendByte);/*打印buffer*/
			}

			/*if sendByte == sendto() 检查字节数*/

			break;

		default:
			break;
		}
	}





	/* 运行结束, socket关闭 */
	if (closesocket(sockSrv) != 0)
		printf("close socket failed with error %ld\n", GetLastError());
	else
		printf("socket closed.\n");

	/* 运行结束, 清理缓存 */
	if (WSACleanup() != 0)
		printf("WSA clean up failed with error %ld\n", GetLastError());
	else
		printf("clean up is OK.\n");


	return 0;
}