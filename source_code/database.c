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
	Discription:	向ClientTable添加新记录
*/
void AddToClientTable() {
	clientTable.a++;
}