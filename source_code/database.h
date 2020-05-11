#pragma once
/*
	Discription:	ClientTable结构体
	Attributes:		
*/	
typedef struct {
	int a;
}CTable;

/********************ClientTable系列方法******************/

/*
	Discription:	初始化ClientTable
	Params:			void
	Return:			void
*/
extern void InitClientTable(); 

/*
	Discription:	打印ClientTable(没啥用
	Params:			void
	Return:			void
*/
extern void DebugClientTable();

/*	
	Discription:	向ClientTable中添加记录
	Params:			void
	Return:			void
*/
extern void AddToClientTable();


/******************DNSTable系列方法***********************/

/*
	Discription:	从ascii文件中构建DNS记录表
	Params:
	Return:			void
*/
extern void BuildDNSTable();

/*
	Discription:	在DNS中查找域名
	Params:
	Return:
*/
extern int FindInDNSTable();


/******************DNSCache系列方法***********************/
