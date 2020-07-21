#pragma once
#include <string.h>
#include <winsock2.h>
#include<WS2tcpip.h>
#include"database.h"
#include"control.h"

/*����ʹ��database�����нӿ�,�Լ�control�ж���ĺ�*/
extern char addrDNSserv[];/*�ⲿ����,��ӦΪchar*,���޸�*/


/*
	Discription:	DNSͷ���ṹ��,12�ֽ�
	Attributes:		��ʱ��˵ÿ���������ֽ�,
*/
typedef struct dnsheader{
	unsigned short ID;		
	unsigned short FLAGS;	/*2�ֽڵĸ���flag*/
	unsigned short QDCOUNT;	
	unsigned short ANCOUNT;
	unsigned short NSCOUNT;
	unsigned short ARCOUNT;
}DNSHeader;

/*
	Discription:	������յ�query�������
	params:			
		recvBuf:	���ջ���(�������)
		sendBuf:	���ͻ���(��������)
		recvByte:	���յ����ֽ���(�������)
		addrCli:	&�µ�Ŀ���ַ(��������)
	return:			int	---- ����д�뵽sendBuf���ֽ���
					
*/
extern int ResolveQuery(const unsigned char* recvBuf, unsigned char* sendBuf,  int recvByte, SOCKADDR_IN* addrCli);

/*
	Discription:	������յ�Response�������
	params:
		recvBuf:	���ջ���
		sendBuf:	���ͻ���
		recvByte:	���յ����ֽ���
		addrCli:	&�µ�Ŀ���ַ
	return:			int	---- ����д�뵽sendBuf���ֽ���
*/
extern int ResolveResponse(const unsigned char* recvBuf, unsigned char* sendBuf, int recvByte,SOCKADDR_IN*addrCli);
