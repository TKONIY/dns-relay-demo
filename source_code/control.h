#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "database.h"

/* ȫ������ */
#define MAX_BUFSIZE 512
typedef enum { dgram_arrival, timeout } event_type;

static char emptyBuffer[MAX_BUFSIZE] = { '\0' }; //��buffer



/*
	�������.
	WaitForEvent();
	DNSDebug();  //��ӡ����־��
	CmdResolve();//��ͬģʽ�ò�ͬ���ʾ
	.....
*/

/*
	Discription:
	Params:
	Return:
*/
extern event_type WaitForEvent();


/********************** Buffer API **********************/

/* 
	Discription:	��16���ƴ�ӡ�ڴ���
	Params:
		@buf		�ڴ�
		@bufSize	Ҫ��ӡ���ڴ�����С
	Return:			void
*/
extern void DebugBuffer(const unsigned char * buf, int bufSize);

/*
	Discription:	��ջ�����
	Params:
		@buf		�ڴ�
		@bufSize	Ҫ��յ��ڴ�����С
	return:			void
*/
extern void ClearBuffer(const unsigned char* buf, int bufSize);