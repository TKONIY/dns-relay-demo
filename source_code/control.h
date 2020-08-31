#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>


/* 全局配置 */
#define MAX_QUERIES 500			/*最大支持的同时等待的用户query数*/
#define MAX_BUFSIZE 512				/*UDP最大载荷*/
typedef enum { dgram_arrival, timeout } event_type; /*事件类型*/
/*extern const char* gDefaultDBtxt;*/	/*默认txt格式的数据库文件*/
/*extern const char* gDefaultDBsqlite;*//*默认sqlite3数据库文件*/
extern char gDBtxt[100];			/*用户指定的txt数据库文件名*/
extern char addrDNSserv[16];		/*DNS服务器地址*/
extern char addrDNSclie[16];		/*127.0.0.1*/
extern char gDBsqlite[100];			/*sqlite数据库名字*/
int gDebugLevel;					/*调试等级*/



/*
	核心组件.
	WaitForEvent();
	DNSDebug();  //打印到日志中
	CmdResolve();//不同模式用不同码表示
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
	Discription:	以16进制打印内存区
	Params:
		@buf		内存
		@bufSize	要打印的内存区大小
	Return:			void
*/
extern void DebugBuffer(const unsigned char * buf, int bufSize);

/*
	Discription:	清空缓冲区
	Params:
		@buf		内存
		@bufSize	要清空的内存区大小
	return:			void
*/
extern void ClearBuffer(unsigned char* buf, int bufSize);


/*	Discription:	处理命令行参数
	Params:
		@argc	
		@argv
	return:
		1			符合格式
		0			不符合格式
*/
extern char dealOpts(int argc, char* argv[]);