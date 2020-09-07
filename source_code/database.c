#ifndef	_CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "database.h"
#include "sqlite3.h" /*仅该模块操作sqlite*/

static CQueue clientTable;

void InitCTable() {
	clientTable.front = 0;
	clientTable.rear = 0;
	printf("ClientTable init.\n");
}

void DebugCTable() {
	int used = clientTable.rear - clientTable.front;
	used = used < 0 ? used + MAX_QUERIES : used;
	printf("队列最大缓存%d 已使用%d\n", MAX_QUERIES, used);
}

int PushCRecord(const SOCKADDR_IN* pAddr, DNSID* pId) {
	if ((clientTable.rear + 1) % MAX_QUERIES == clientTable.front) {
		printf("队列已满,丢弃报文\n.");
		return 0;
	} else {
		clientTable.base[clientTable.rear].addr = *pAddr;
		clientTable.base[clientTable.rear].originId = *pId;
		clientTable.base[clientTable.rear].r = 0;
		clientTable.base[clientTable.rear].expireTime = (int)time(NULL) + 3;//TODO
		//clientTable.base[clientTable.rear].addrReq = *rAddr; /*获取发出请求的客户端地址*/
		*pId = clientTable.rear;/*获取新的ID*/
		clientTable.rear = (clientTable.rear + 1) % MAX_QUERIES;
		return 1;
	}
}

int PopCRecord() {
	if (clientTable.rear == clientTable.front) {
		printf("队列为空，PopCRecord()失败。\n");
		return 0;
	} else {
		++clientTable.front;
		return 1;
	}
}

int SetCRecordR(DNSID id) {
	if ((
		clientTable.front <= clientTable.rear
		&& (id < clientTable.rear && id >= clientTable.front)
		) || (
			clientTable.front > clientTable.rear
			&& (id < clientTable.rear || id >= clientTable.front))
		) {
		clientTable.base[id].r = 1;
		return 1;
	} else {
		printf("要修改的记录不存在,索引越界,SetCRecordR()失败\n");
		return 0;
	}
}

int FindCRecord(DNSID id, CRecord* pRecord) {
	if ((
		clientTable.front <= clientTable.rear
		&& (id < clientTable.rear && id >= clientTable.front)
		) || (
			clientTable.front > clientTable.rear
			&& (id < clientTable.rear || id >= clientTable.front))
		) {
		*pRecord = clientTable.base[id];
		return 1;
	} else {
		printf("要查找的记录不存在,索引越界,SetCRecordR()失败\n");
		return 0;
	}
}

int GetCTableRearIndex() {
	/*返回rear-1*/
	return ((clientTable.rear + MAX_QUERIES - 1) % MAX_QUERIES);
}

int GetCTableFrontIndex() {
	return clientTable.front;
}

int GetCTableFrontIndex_r() {
	return clientTable.base[clientTable.front].r;
}

int CheckExpired() {
	/*判断是否超时*/
	return clientTable.base[clientTable.front].expireTime > 0 &&
		time(NULL) > clientTable.base[clientTable.front].expireTime;
}

//int SetTime() {
//	clientTable.base[clientTable.rear].expireTime = time(NULL) + 3;
//}
/*******************SQLite封装*******************************/

//static sqlite3* db = NULL;  /*数据库对象*/

/*封装INSERT接口*/
static int DNSInsert(sqlite3* db, const char* ip, const char* name) {
	/*char sql_insert[512] = "INSERT INTO DNS_record (IP, Name) VALUES('";
	strcat(sql_insert, ip);
	strcat(sql_insert, "', '");
	strcat(sql_insert, name);
	strcat(sql_insert, "');");*/

	char sql_insert[512];
	sprintf(sql_insert,
		"INSERT INTO DNS_record (IP, Name) "
		"VALUES('%s', '%s');",
		ip, name);
	/*"INSERT INTO DNS_record (IP, Name)"
	"VALUES('49.234.121.191', 'canteencloud.com');";*/
	return sqlite3_exec(db, sql_insert, NULL, NULL, NULL);
}

/*封装SELECT接口*/
static int DNSSelect(sqlite3* db, const char* name, char* ip) {
	/*char sql_select[512] = "SELECT IP FROM DNS_record WHERE Name = '";
	strcat(sql_select, name);
	strcat(sql_select, "';");*/

	char sql_select[512];
	sprintf(sql_select,
		"SELECT IP "
		"FROM DNS_record "
		"WHERE Name = '%s';",
		name);
	printf("%s\n", sql_select);
	/*"SELECT IP"
	"FROM DNS_record"
	"WHERE Name = 'canteencloud.com';";*/
	char** result = NULL;/* 查询结果的指针*/
	int nRow;	/*行数*/
	int nCol;	/*列数*/
	int rc = sqlite3_get_table(db, sql_select, &result, &nRow, &nCol, NULL);

	if (SQLITE_OK == rc && nRow > 0) {
		/*
			result表格的构造:
			将 name, ip,
			   n1,	0.0.0.0,
			   n2,  0.0.0.0, ...
			变成一维数组
			name, ip, n1, 0.0.0.0, n2, 0.0.0.0, n3, 0.0.0.0

			我们只搜IP,那么搜出来的表格就是
			'IP', ip1, ip2, ip3;
		*/
		strcpy(ip, result[1]);
		sqlite3_free_table(result);
		return SQLITE_OK;
	} else {
		sqlite3_free_table(result);
		return SQLITE_FAIL;
	}
}

/*封装从文件中读取接口*/
static int DNSImport(sqlite3* db, const char* fname) {
	FILE* fp = fopen(fname, "r");
	if (!fp) { //空
		printf("Can't open dnsrealay.txt %s\n", fname);
		return SQLITE_FAIL;
	} else {
		char ip[MAX_IP_BUFSIZE] = { '\0' };
		char name[MAX_DOMAINNAME] = { '\0' };
		while (!feof(fp))
		{
			fscanf(fp, "%s", ip);
			fscanf(fp, "%s", name);

			/*insert to db, 不判断结果了*/
			DNSInsert(db, ip, name);
			//printf("imported %s %s\n", ip, name);
		}
		fclose(fp);
		return SQLITE_OK;
	}
}

/*******************dnsrelay.txt文本数据库封装****************/
static FILE* dbTXT = NULL;		/*文本数据库对象*/
static fpos_t dbHome = 0;		/*避免每次重新打开文件, */

/*封装从文本中查询IP接口*/
static int FindIPByDNSinTXT(FILE* dbTXT, const char* name, char* ip) {
	char retName[MAX_DOMAINNAME] = { '\0' };
	fsetpos(dbTXT, &dbHome);	/*设置到起始位置*/
	while (!feof(dbTXT)) {
		fscanf(dbTXT, "%s %s", ip, retName);
		if (!strcmp(retName, name))break;
	}
	if (feof(dbTXT))return 0;/*文件结束,未找到*/
	return 1;
}


/***********************cache接口*************************/

/*不需要对外开放*/
typedef struct {
	char domainName[MAX_DOMAINNAME];/*域名*/
	char ip[MAX_IP_BUFSIZE];		/*ip*/
	int ttl;						/*ttl*/
}DNScache;

/*cache数组, 有位置就插, 就这么随便实现一下吧, 最低效的cache*/
static DNScache cache[MAX_CACHE_SIZE] = { 0 };

static time_t cacheLastCheckTime = 0; /*上一次检查的时间, 初始化为0, 非零的时候才检查*/

/*
	Discription:	在DNScache中查找域名
	Params:
		domainName	域名
		ip			ip地址，返回值,请保证至少有16字节的空间(15个字符+\0)

	Return:
		0: 没找到
		1: 找到了
*/

static int FindInDNSCache(const char* domainName, char* ip) {
	for (int i = 0; i < MAX_CACHE_SIZE; i++) {
		if (cache[i].ttl > 0 && !strcmp(cache[i].domainName, domainName)) {
			sprintf(ip, "%s", cache[i].ip);
			return 1;
		}
	}
	return 0;
}

int InsertIntoDNSCache(const char* domainName, const char* ip, int ttl) {
	for (int i = 0; i < MAX_CACHE_SIZE; i++) {
		if (cache[i].ttl <= 0) {
			sprintf(cache[i].domainName, "%s", domainName);
			sprintf(cache[i].ip, "%s", ip);
			cache[i].ttl = ttl;
			return 1;
		}
	}
	return 0;
}

void UpdateCache() {
	time_t newTime = time(0);
	time_t diff = newTime - cacheLastCheckTime;
	printf("离上一次检查过去了%lld秒\n", diff);
	if (diff) { //时间变了
		cacheLastCheckTime = newTime;
		for (int i = 0; i < MAX_CACHE_SIZE; i++) {
			if (cache[i].ttl > 0)
			{
				cache[i].ttl -= (int)diff;
				printf("%s : %s  TTL= %d\n", cache[i].domainName, cache[i].ip, cache[i].ttl);
			}
		}
	}
}

/************************统一的数据库接口**********************/


int BuildDNSDatabase()
{
	/*打开文本文件*/
	dbTXT = fopen("dnsrelay.txt", "r");
	fgetpos(dbTXT, &dbHome);
	if (!dbTXT)return 0;
	return 1;

	///*创建数据库*/
	//if (SQLITE_OK == sqlite3_open(gDBsqlite, &db)) {
	//	printf("Database %s opened.\n", gDBsqlite);
	//} else {
	//	printf("Failed to open %s.\n",gDBsqlite);
	//	return 0;
	//}
	///*删除表*/
	//const char* sql_drop = "DROP TABLE DNS_record;";
	//sqlite3_exec(db, sql_drop, NULL, NULL,NULL);
	///*创建表*/
	//const char* sql_create = "CREATE TABLE DNS_record("
	//	"IP   varchar(16)  NOT NULL,"
	//	"Name varchar(100) NOT NULL);";
	//if (SQLITE_OK == sqlite3_exec(db, sql_create, NULL, NULL, NULL)) {
	//	printf("successfully created %s.\n", gDBsqlite);
	//} else {
	//	printf("falied to created sqlite database.\n");
	//	return 0;
	//}

	///*导入表*/
	//printf("importing records from %s to %s\n", gDBtxt, gDBsqlite);
	//if (SQLITE_OK == DNSImport(db, "dnsrelay.txt")) {
	//	printf("successfully import\n");
	//} else {
	//	printf("failed to import.\n");
	//	return 0;
	//}
	//return 1;
}

int FindInDNSDatabase(const char* domainName, char* ip) {
	//if (db && SQLITE_OK == DNSSelect(db, domainName, ip)) { 
	//	/*db不为空且找到了对应记录*/
	//	return 1;
	//} else {
	//	return 0;
	//}

	/*先在cache里面找*/
	if (FindInDNSCache(domainName, ip)) {
		return 1;
	}

	if (dbTXT && FindIPByDNSinTXT(dbTXT, domainName, ip)) {
		/*记录插入到cache中*/
		InsertIntoDNSCache(domainName, ip, TTL);
		return 1; /*db存在且找到了对应记录*/
	} else {
		return 0;
	}

}