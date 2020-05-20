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
	printf("������󻺴�%d ��ʹ��%d\n", MAX_QUERIES, used);
}

char PushCRecord(SOCKADDR*pAddr, DNSID *pId) {
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
		&& (id<clientTable.rear && id>=clientTable.front)
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

