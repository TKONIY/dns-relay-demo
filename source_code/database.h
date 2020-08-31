#pragma once
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <control.h>
#include "control.h"


typedef unsigned short DNSID; /*������DNS���ĵ�ID����*/
/*
	Discription:			����û�query��¼�Ľṹ
	Attributes:
		SOCKADDR_IN addr	socket��ַ
		DNSID originId		�û����͵�DNS���ĵ�ID
		unsigned char r;	��ʾ����Ƿ�reply��
*/

typedef struct clientRecord{
	SOCKADDR_IN addr;
	DNSID originId;
	unsigned char r;
	int expireTime;
}CRecord;

/*
	Discription:	ClientTable�ṹ��
	Attributes:
		CRecord base[MAX_QUERIES];	�ȴ�����
		int front					ͷָ��
		int rear					βָ��
*/
typedef struct {
	CRecord base[MAX_QUERIES];
	int front;
	int rear;
}CQueue;


/********************ClientTableϵ�з���******************/

/*
	Discription:	��ʼ��ClientTable
	Params:			void
	Return:			void
*/
extern void InitCTable();

/*
	Discription:	��ӡClientTable״̬
	Params:			void
	Return:			void
*/
extern void DebugCTable();

/*
	Discription:		��ClientTable�Ķ�β��Ӽ�¼
	Params:			
		SOCKADDR* pAddr	��ַ(����)
		DNSID id		ԭid
	Return:				
		char			0/1��ʾ�Ƿ�ɹ�
*/
extern char PushCRecord(const SOCKADDR_IN* pAddr, DNSID *pId);

/*
	Discription:		�����׼�¼����
	Params:
		DNSID id		ָ����DNS����
	Return:
		char			0/1��ʾ�Ƿ�ɹ�
*/
extern char PopCRecord();


/*
	Discription:		��ָ����¼�޸�Ϊ�ѻظ�
	Params:
		DNSID id		ָ����DNS����
	Return:
		char			0/1��ʾ�Ƿ�ɹ�
*/
extern char SetCRecordR(DNSID id);

/*
	Discription:		����ת��ID����ԭID�͵�ַ
	Params:
		DNSID id;			id
		CRecord* pRecord	���record�ĵ�ַ
	Return
		char				0/1��ʾ�Ƿ�ɹ���
*/
extern char FindCRecord(DNSID id, CRecord* pRecord);

/*
	Discription:		��ȡ��βԪ�ص����
	Params:
	Return:
		int				���
						
*/
extern int GetCTableRearIndex();

/*
	Discription:		��ȡ����Ԫ�����
	Params:			
	Return:				
		int				���	
*/
extern int GetCTableFrontIndex();

/*
	Discription:		����Ԫ���Ƿ�ظ�
	Params:
	Return:
		int				0/1��ʾ�Ƿ�ظ���
*/
extern int GetCTableFrontIndex_r();

/*
	Discription:		����Ԫ���Ƿ�ʱ
	Params:
	Return:
		int				0/1��ʾ�Ƿ�ʱ
*/
extern int CheckExpired();

/*
	Discription:		����ǰ����Ԫ�������³�ʱʱ��
	Params:
	Return:
		int				Ĭ��3��
*/
extern int SetTime();

/******************DNSTableϵ�з���***********************/

/*
	Discription:	��ascii�ļ��й���DNS��¼��
	Params:
	Return:			
		char 0 ʧ��
		char 1 �ɹ�
*/
extern char BuildDNSDatabase();

/*
	Discription:	��DNS�в�������
	Params:
		domainName	����
		ip			ip��ַ������ֵ,�뱣֤������16�ֽڵĿռ�(15���ַ�+\0)
		
	Return: 
		0: û�ҵ�
		1: �ҵ���
*/
extern char FindInDNSDatabase(const char* domainName, char*ip);


/******************DNSCacheϵ�з���***********************/
