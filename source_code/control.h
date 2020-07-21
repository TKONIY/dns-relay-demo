#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "database.h"

/* 全局配置 */
#define MAX_BUFSIZE 512
typedef enum { dgram_arrival, timeout } event_type;

static char emptyBuffer[MAX_BUFSIZE] = { '\0' }; //空buffer



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
extern void ClearBuffer(const unsigned char* buf, int bufSize);