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

char PushCRecord(SOCKADDR* pAddr, DNSID* pId) {
	if ((clientTable.rear + 1) % MAX_QUERIES == clientTable.front) {
		printf("��������,��������\n.");
		return 0;
	} else {
		clientTable.base[clientTable.rear].addr = *pAddr;
		clientTable.base[clientTable.rear].originId = *pId;
		clientTable.base[clientTable.rear].r = 0;
		*pId = clientTable.rear;/*��ȡ�µ�ID*/
		clientTable.rear = (clientTable.rear + 1) % MAX_QUERIES;
		return 1;
	}
}

char PopCRecord() {
	if (clientTable.rear == clientTable.front) {
		printf("����Ϊ�գ�PopCRecord()ʧ�ܡ�\n");
		return 0;
	} else {
		++clientTable.front;
		return 1;
	}
}

char SetCRecordR(DNSID id) {
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

char FindCRecord(DNSID id, CRecord* pRecord) {
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

/*******************SQLite��װ*******************************/

static sqlite3* db = NULL;  /*���ݿ����*/

/*��װINSERT�ӿ�*/
int DNSInsert(sqlite3* db, const char* ip, const char* name) {
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
int DNSSelect(sqlite3* db, const char* name, char* ip) {
	/*char sql_select[512] = "SELECT IP FROM DNS_record WHERE Name = '";
	strcat(sql_select, name);
	strcat(sql_select, "';");*/

	char sql_select[512];
	sprintf(sql_select,
		"SELECT IP "
		"FROM DNS_record "
		"WHERE Name = '%s';",
		name);
	printf("%s", sql_select);
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
int DNSImport(sqlite3* db, const char* fname) {
	FILE* fp = fopen(fname, "r");
	if (!fp) { //��
		printf("Can't open dnsrealay.txt %s\n", fname);
		return SQLITE_FAIL;
	} else {
		char ip[15] = { '\0' };
		char name[100] = { '\0' };
		while (!feof(fp))
		{
			fscanf(fp, "%s", ip);
			fscanf(fp, "%s", name);

			/*insert to db, ���жϽ����*/
			DNSInsert(db, ip, name);
			printf("imported %s %s\n", ip, name);
		}
		fclose(fp);
		return SQLITE_OK;
	}

}


char BuildDNSDatabase()
{
	/*�������ݿ�*/
	if (SQLITE_OK == sqlite3_open("dnsrelay.db", &db)) {
		printf("Opened.\n");
	} else {
		printf("Failed to open .\n");
		return 0;
	}
	/*ɾ����*/
	const char* sql_drop = "DROP TABLE DNS_record;";
	sqlite3_exec(db, sql_drop, NULL, NULL,NULL);
	/*������*/
	const char* sql_create = "CREATE TABLE DNS_record("
		"IP   varchar(16)  NOT NULL,"
		"Name varchar(100) NOT NULL);";
	if (SQLITE_OK == sqlite3_exec(db, sql_create, NULL, NULL, NULL)) {
		printf("successfully created.\n");
	} else {
		printf("falied to created\n");
		return 0;
	}

	/*�����*/
	if (SQLITE_OK == DNSImport(db, "dnsrelay.txt")) {
		printf("successfully read\n");
	} else {
		printf("failed to read.");
		return 0;
	}
	return 1;
}

char FindInDNSDatabase()
{
	return 0;
}




