#include "database.h"
#include <stdio.h>
static CTable clientTable;

void InitClientTable() {
	clientTable.a = 1;
	printf("ClientTable init.\n");
}


void DebugClientTable() {
	printf("print ctable: %d\n", clientTable.a);
}

/*
	Discription:	��ClientTable����¼�¼
*/
void AddToClientTable() {
	clientTable.a++;
}