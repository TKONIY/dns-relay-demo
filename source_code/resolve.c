#include "resolve.h"

extern int ResolveQuery(unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	memcpy(sendBuf, recvBuf, recvByte);
	return recvByte;
}

extern int ResolveResponse(unsigned char* recvBuf, unsigned char* sendBuf, int recvByte, SOCKADDR_IN* addrCli) {
	return recvByte;
}