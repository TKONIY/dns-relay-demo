#pragma once
#include <string.h>
#include <winsock2.h>
#include"database.h"
#include"control.h"

/*����ʹ��database�����нӿ�,�Լ�control�ж���ĺ�*/

/*
	Discription:	������յ�query�������
	params:			
		recvBuf:	���ջ���
		sendBuf:	���ͻ���
		recvByte:	���յ����ֽ���
		addrCli:	&�µ�Ŀ���ַ
	return:			int	---- ����д�뵽sendBuf���ֽ���
*/
extern int ResolveQuery(unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli);

/*
	Discription:	������յ�Response�������
	params:
		recvBuf:	���ջ���
		sendBuf:	���ͻ���
		recvByte:	���յ����ֽ���
		addrCli:	&�µ�Ŀ���ַ
	return:			int	---- ����д�뵽sendBuf���ֽ���
*/
extern int ResolveResponse(unsigned char* recvBuf, unsigned char* sendBuf, int recvByte,SOCKADDR_IN*addrCli);
