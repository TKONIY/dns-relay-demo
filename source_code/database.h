#pragma once
/*
	Discription:	ClientTable�ṹ��
	Attributes:		
*/	
typedef struct {
	int a;
}CTable;

/********************ClientTableϵ�з���******************/

/*
	Discription:	��ʼ��ClientTable
	Params:			void
	Return:			void
*/
extern void InitClientTable(); 

/*
	Discription:	��ӡClientTable(ûɶ��
	Params:			void
	Return:			void
*/
extern void DebugClientTable();

/*	
	Discription:	��ClientTable����Ӽ�¼
	Params:			void
	Return:			void
*/
extern void AddToClientTable();


/******************DNSTableϵ�з���***********************/

/*
	Discription:	��ascii�ļ��й���DNS��¼��
	Params:
	Return:			void
*/
extern void BuildDNSTable();

/*
	Discription:	��DNS�в�������
	Params:
	Return:
*/
extern int FindInDNSTable();


/******************DNSCacheϵ�з���***********************/
