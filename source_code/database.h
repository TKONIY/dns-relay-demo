#pragma once
#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#define MAX_QUERIES 1000 /*最大支持的同时等待的用户query数*/

typedef unsigned short DNSID; /*适用于DNS报文的ID类型*/
/*
	Discription:			存放用户query记录的结构
	Attributes:
		SOCKADDR_IN addr	socket地址
		DNSID originId		用户发送的DNS报文的ID
		unsigned char r;	表示这个是否被reply了
*/

typedef struct clientRecord{
	SOCKADDR addr;
	DNSID originId;
	unsigned char r;
}CRecord;

/*
	Discription:	ClientTable结构体
	Attributes:
		CRecord base[MAX_QUERIES];	等待窗口
		int front					头指针
		int rear					尾指针
*/
typedef struct {
	CRecord base[MAX_QUERIES];
	int front;
	int rear;
}CQueue;


/********************ClientTable系列方法******************/

/*
	Discription:	初始化ClientTable
	Params:			void
	Return:			void
*/
extern void InitCTable();

/*
	Discription:	打印ClientTable状态
	Params:			void
	Return:			void
*/
extern void DebugCTable();

/*
	Discription:		向ClientTable的队尾添加记录
	Params:			
		SOCKADDR* pAddr	地址(传入)
		DNSID id		原id
	Return:				
		char			0/1表示是否成功
*/
extern char PushCRecord(const SOCKADDR* pAddr, DNSID *pId);

/*
	Discription:		将队首记录弹出
	Params:
		DNSID id		指定的DNS报文
	Return:
		char			0/1表示是否成功
*/
extern char PopCRecord();


/*
	Discription:		将指定记录修改为已回复
	Params:
		DNSID id		指定的DNS报文
	Return:
		char			0/1表示是否成功
*/
extern char SetCRecordR(DNSID id);

/*
	Discription:		根据转发ID查找原ID和地址
	Params:
		DNSID id;			id
		CRecord* pRecord	存放record的地址
	Return
		char				0/1表示是否成功。
*/
extern char FindCRecord(DNSID id, CRecord* pRecord);

/*
	Discription:		获取队尾元素的序号
	Params:
	Return:
		int				序号
						
*/
extern int GetCTableRearIndex();

/*
	Discription:		获取队首元素序号
	Params:			
	Return:				
		int				序号	
*/
extern int GetCTableFrontIndex();


/******************DNSTable系列方法***********************/

/*
	Discription:	从ascii文件中构建DNS记录表
	Params:
	Return:			
		char 0 失败
		char 1 成功
*/
extern char BuildDNSDatabase();

/*
	Discription:	在DNS中查找域名
	Params:
		domainName	域名
		ip			ip地址，返回值,请保证至少有16字节的空间(15个字符+\0)
		
	Return: 
		0: 没找到
		1: 找到了
*/
extern char FindInDNSDatabase(const char* domainName, char*ip);


/******************DNSCache系列方法***********************/
