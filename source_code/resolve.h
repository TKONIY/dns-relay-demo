#pragma once
#include <string.h>
#include <winsock2.h>
#include<WS2tcpip.h>
#include"database.h"
#include"control.h"

/*可以使用database的所有接口,以及control中定义的宏*/
extern char addrDNSserv[];/*外部变量,理应为char*,可修改*/


/*
	Discription:	DNS头部结构体,12字节
	Attributes:		暂时来说每个属性两字节,
*/
typedef struct dnsheader{
	unsigned short ID;		
	unsigned short FLAGS;	/*2字节的各种flag*/
	unsigned short QDCOUNT;	
	unsigned short ANCOUNT;
	unsigned short NSCOUNT;
	unsigned short ARCOUNT;
}DNSHeader;

/*
	Discription:	处理接收到query包的情况
	params:			
		recvBuf:	接收缓存(传入参数)
		sendBuf:	发送缓存(传出参数)
		recvByte:	接收到的字节数(传入参数)
		addrCli:	&新的目标地址(传出参数)
	return:			int	---- 最终写入到sendBuf的字节数
					
*/
extern int ResolveQuery(const unsigned char* recvBuf, unsigned char* sendBuf,  int recvByte, SOCKADDR_IN* addrCli);

/*
	Discription:	处理接收到Response包的情况
	params:
		recvBuf:	接收缓存
		sendBuf:	发送缓存
		recvByte:	接收到的字节数
		addrCli:	&新的目标地址
	return:			int	---- 最终写入到sendBuf的字节数
*/
extern int ResolveResponse(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte,SOCKADDR_IN*addrCli);
