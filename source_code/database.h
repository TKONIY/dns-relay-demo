#pragma once
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "control.h"

#define MAX_QUERIES 25			/*最大支持的同时等待的用户query数*/
#define MAX_CACHE_SIZE 100		/*dns缓存的容量*/

typedef unsigned short DNSID; /*适用于DNS报文的ID类型*/

/*
	Discription:			存放用户query记录的结构
	Attributes:
		SOCKADDR_IN addr	socket地址
		DNSID originId		用户发送的DNS报文的ID
		unsigned char r;	表示这个是否被reply了
		expireTime			超时时刻
*/

typedef struct {
	SOCKADDR_IN addr;
	DNSID originId;
	unsigned char r;
	int expireTime;

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
	Discription:	返回ClientTable已经使用的空间
	Return:			队列长度
*/
int CTableUsage();

/*
	Discription:		向ClientTable的队尾添加记录
	Params:			
		SOCKADDR* pAddr	地址(传入)
		DNSID id		原id(传入传出)
	Return:				
		int			0/1表示是否成功
	Remarks:			自动计算超时时刻
*/
extern int PushCRecord(const SOCKADDR_IN* pAddr, DNSID *pId);

/*
	Discription:		将队首记录弹出
	Params:
		DNSID id		指定的DNS报文
	Return:
		int			0/1表示是否成功
*/
extern int PopCRecord();


/*
	Discription:		将指定记录修改为已回复
	Params:
		DNSID id		指定的DNS报文
	Return:
		int			0/1表示是否成功
*/
extern int SetCRecordR(DNSID id);

/*
	Discription:		根据转发ID查找原ID和地址
	Params:
		DNSID id;			id
		CRecord* pRecord	存放record的地址 传出参数
	Return
		int				0/1表示是否成功。
*/
extern int FindCRecord(DNSID id, CRecord* pRecord);

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

///*
//	Discription:		队首元素是否回复
//	Params:
//	Return:
//		int				0/1表示是否回复过
//*/
//extern int GetCTableFrontIndex_r();

///*
//	Discription:		队首元素是否超时
//	Params:
//	Return:
//		int				0/1表示是否超时
//*/
//extern int CheckExpired();

/*
	Discription:		给当前队首元素设置新超时时间
	Params:
	Return:
		int				默认3秒
*/
/*extern int SetTime();*/

/******************DNSTable系列方法***********************/

/*
	Discription:	从ascii文件中构建DNS记录表
	Params:
	Return:			
		char 0 失败
		char 1 成功
*/
extern int BuildDNSDatabase();

/*
	Discription:	在DNS中查找域名
	Params:
		domainName	域名
		ip			ip地址，返回值,请保证至少有16字节的空间(15个字符+\0)
		
	Return: 
		0: 没找到
		1: 找到了
*/
extern int FindInDNSDatabase(const char* domainName, char*ip);


/******************DNSCache系列方法***********************/

/*
	Discription:	添加记录到DNSCache
	Params:
		domainName	域名
		ip			ip地址，返回值,请保证至少有16字节的空间(15个字符+\0)
		ttl			当前的ttl

	Return:
		0: 没找到
		1: 找到了
*/
extern int InsertIntoDNSCache(const char* domainName, const char* ip, int ttl);

/*
	Discription:	更新DNS cache里面的TTL和删除过期的DNS记录
	Params:
*/
extern void UpdateCache();