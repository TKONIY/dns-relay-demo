#ifndef	_CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "database.h"
#include "sqlite3.h" /*����ģ�����sqlite*/

static CQueue clientTable;

void InitCTable() {
	clientTable.front = 0;
	clientTable.rear = 0;
	printf("ClientTable init.\n");
}

void DebugCTable() {
	int used = clientTable.rear - clientTable.front;
	used = used < 0 ? used + MAX_QUERIES : used;
	printf("������󻺴�%d ��ʹ��%d\n", MAX_QUERIES, used);
}

int PushCRecord(const SOCKADDR_IN* pAddr, DNSID* pId) {
	if ((clientTable.rear + 1) % MAX_QUERIES == clientTable.front) {
		printf("��������,��������\n.");
		return 0;
	} else {
		clientTable.base[clientTable.rear].addr = *pAddr;
		clientTable.base[clientTable.rear].originId = *pId;
		clientTable.base[clientTable.rear].r = 0;
		clientTable.base[clientTable.rear].expireTime = (int)time(NULL) + 3;//TODO
		//clientTable.base[clientTable.rear].addrReq = *rAddr; /*��ȡ��������Ŀͻ��˵�ַ*/
		*pId = clientTable.rear;/*��ȡ�µ�ID*/
		clientTable.rear = (clientTable.rear + 1) % MAX_QUERIES;
		return 1;
	}
}

int PopCRecord() {
	if (clientTable.rear == clientTable.front) {
		printf("����Ϊ�գ�PopCRecord()ʧ�ܡ�\n");
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
		printf("Ҫ�޸ĵļ�¼������,����Խ��,SetCRecordR()ʧ��\n");
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
		printf("Ҫ���ҵļ�¼������,����Խ��,SetCRecordR()ʧ��\n");
		return 0;
	}
}

int GetCTableRearIndex() {
	/*����rear-1*/
	return ((clientTable.rear + MAX_QUERIES - 1) % MAX_QUERIES);
}

int GetCTableFrontIndex() {
	return clientTable.front;
}

int GetCTableFrontIndex_r() {
	return clientTable.base[clientTable.front].r;
}

int CheckExpired() {
	/*�ж��Ƿ�ʱ*/
	return clientTable.base[clientTable.front].expireTime > 0 &&
		time(NULL) > clientTable.base[clientTable.front].expireTime;
}

//int SetTime() {
//	clientTable.base[clientTable.rear].expireTime = time(NULL) + 3;
//}
/*******************SQLite��װ*******************************/

//static sqlite3* db = NULL;  /*���ݿ����*/

/*��װINSERT�ӿ�*/
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

/*��װSELECT�ӿ�*/
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
	char** result = NULL;/* ��ѯ�����ָ��*/
	int nRow;	/*����*/
	int nCol;	/*����*/
	int rc = sqlite3_get_table(db, sql_select, &result, &nRow, &nCol, NULL);

	if (SQLITE_OK == rc && nRow > 0) {
		/*
			result���Ĺ���:
			�� name, ip,
			   n1,	0.0.0.0,
			   n2,  0.0.0.0, ...
			���һά����
			name, ip, n1, 0.0.0.0, n2, 0.0.0.0, n3, 0.0.0.0

			����ֻ��IP,��ô�ѳ����ı�����
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

/*��װ���ļ��ж�ȡ�ӿ�*/
static int DNSImport(sqlite3* db, const char* fname) {
	FILE* fp = fopen(fname, "r");
	if (!fp) { //��
		printf("Can't open dnsrealay.txt %s\n", fname);
		return SQLITE_FAIL;
	} else {
		char ip[MAX_IP_BUFSIZE] = { '\0' };
		char name[MAX_DOMAINNAME] = { '\0' };
		while (!feof(fp))
		{
			fscanf(fp, "%s", ip);
			fscanf(fp, "%s", name);

			/*insert to db, ���жϽ����*/
			DNSInsert(db, ip, name);
			//printf("imported %s %s\n", ip, name);
		}
		fclose(fp);
		return SQLITE_OK;
	}
}

/*******************dnsrelay.txt�ı����ݿ��װ****************/
static FILE* dbTXT = NULL;		/*�ı����ݿ����*/
static fpos_t dbHome = 0;		/*����ÿ�����´��ļ�, */

/*��װ���ı��в�ѯIP�ӿ�*/
static int FindIPByDNSinTXT(FILE* dbTXT, const char* name, char* ip) {
	char retName[MAX_DOMAINNAME] = { '\0' };
	fsetpos(dbTXT, &dbHome);	/*���õ���ʼλ��*/
	while (!feof(dbTXT)) {
		fscanf(dbTXT, "%s %s", ip, retName);
		if (!strcmp(retName, name))break;
	}
	if (feof(dbTXT))return 0;/*�ļ�����,δ�ҵ�*/
	return 1;
}


/***********************cache�ӿ�*************************/

/*����Ҫ���⿪��*/
typedef struct {
	char domainName[MAX_DOMAINNAME];/*����*/
	char ip[MAX_IP_BUFSIZE];		/*ip*/
	int ttl;						/*ttl*/
}DNScache;

/*cache����, ��λ�þͲ�, ����ô���ʵ��һ�°�, ���Ч��cache*/
static DNScache cache[MAX_CACHE_SIZE] = { 0 };

static time_t cacheLastCheckTime = 0; /*��һ�μ���ʱ��, ��ʼ��Ϊ0, �����ʱ��ż��*/

/*
	Discription:	��DNScache�в�������
	Params:
		domainName	����
		ip			ip��ַ������ֵ,�뱣֤������16�ֽڵĿռ�(15���ַ�+\0)

	Return:
		0: û�ҵ�
		1: �ҵ���
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
	printf("����һ�μ���ȥ��%lld��\n", diff);
	if (diff) { //ʱ�����
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

/************************ͳһ�����ݿ�ӿ�**********************/


int BuildDNSDatabase()
{
	/*���ı��ļ�*/
	dbTXT = fopen("dnsrelay.txt", "r");
	fgetpos(dbTXT, &dbHome);
	if (!dbTXT)return 0;
	return 1;

	///*�������ݿ�*/
	//if (SQLITE_OK == sqlite3_open(gDBsqlite, &db)) {
	//	printf("Database %s opened.\n", gDBsqlite);
	//} else {
	//	printf("Failed to open %s.\n",gDBsqlite);
	//	return 0;
	//}
	///*ɾ����*/
	//const char* sql_drop = "DROP TABLE DNS_record;";
	//sqlite3_exec(db, sql_drop, NULL, NULL,NULL);
	///*������*/
	//const char* sql_create = "CREATE TABLE DNS_record("
	//	"IP   varchar(16)  NOT NULL,"
	//	"Name varchar(100) NOT NULL);";
	//if (SQLITE_OK == sqlite3_exec(db, sql_create, NULL, NULL, NULL)) {
	//	printf("successfully created %s.\n", gDBsqlite);
	//} else {
	//	printf("falied to created sqlite database.\n");
	//	return 0;
	//}

	///*�����*/
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
	//	/*db��Ϊ�����ҵ��˶�Ӧ��¼*/
	//	return 1;
	//} else {
	//	return 0;
	//}

	/*����cache������*/
	if (FindInDNSCache(domainName, ip)) {
		return 1;
	}

	if (dbTXT && FindIPByDNSinTXT(dbTXT, domainName, ip)) {
		/*��¼���뵽cache��*/
		InsertIntoDNSCache(domainName, ip, TTL);
		return 1; /*db�������ҵ��˶�Ӧ��¼*/
	} else {
		return 0;
	}

}