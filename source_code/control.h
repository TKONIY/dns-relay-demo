#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "database.h"

/* ȫ������ */

#define MAX_BUFSIZE 512				/*UDP����غ�*/
#define MAX_IP_BUFSIZE 16			/*IP����󳤶�*/
#define MAX_DOMAINNAME 100			/*��������󳤶�*/
#define TTL 120						/*TTL�ĳ�ʼֵ*/
typedef enum { dgram_arrival, timeout } event_type; /*�¼�����*/
/*extern const char* gDefaultDBtxt;*/	/*Ĭ��txt��ʽ�����ݿ��ļ�*/
/*extern const char* gDefaultDBsqlite;*//*Ĭ��sqlite3���ݿ��ļ�*/
extern char gDBtxt[100];			/*�û�ָ����txt���ݿ��ļ���*/
extern char addrDNSserv[16];		/*DNS��������ַ*/
extern char addrDNSclie[16];		/*127.0.0.1*/
extern char gDBsqlite[100];			/*sqlite���ݿ�����*/
int gDebugLevel;					/*���Եȼ�*/


/*
	�������.
	DNSDebug();  //��ӡ����־��
	CmdResolve();//��ͬģʽ�ò�ͬ���ʾ
	.....
*/



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
extern void ClearBuffer(unsigned char* buf, int bufSize);


/*	Discription:	���������в���
	Params:
		@argc	
		@argv
	return:
		1			���ϸ�ʽ
		0			�����ϸ�ʽ
*/
extern int dealOpts(int argc, char* argv[]);