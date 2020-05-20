#include "database.h"

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

char PushCRecord(SOCKADDR*pAddr, DNSID *pId) {
	if ((clientTable.rear + 1) % MAX_QUERIES == clientTable.front) {
		printf("队列已满,丢弃报文\n.");
		return 0;
	} else {
		clientTable.base[clientTable.rear].addr = *pAddr;
		clientTable.base[clientTable.rear].originId = *pId;
		clientTable.base[clientTable.rear].r = 0;
		*pId = clientTable.rear;/*获取新的ID*/
		clientTable.rear = (clientTable.rear + 1) % MAX_QUERIES;
		return 1;
	}
}

char PopCRecord() {
	if (clientTable.rear == clientTable.front) {
		printf("队列为空，PopCRecord()失败。\n");
		return 0;
	} else {
		++clientTable.front;
		return 1;
	}
}

char SetCRecordR(DNSID id) {
	if ((
		clientTable.front <= clientTable.rear 
		&& (id<clientTable.rear && id>=clientTable.front)
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

