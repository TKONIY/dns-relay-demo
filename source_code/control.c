#include "control.h"


event_type WaitForEvent() {

	return dgram_arrival;
}

void DebugBuffer(unsigned char* buf, int bufSize) {
	char isEnd = 0;
	if (bufSize > MAX_BUFSIZE)
		printf("DebugBuffer() failed, bufSize too big: %d>%d", bufSize, MAX_BUFSIZE);
	else {
		/*bufSize = ((bufSize - 1) / 16 + 1) * 16;*/ /*debug时打印完整行内存*/
		for (int i = 0; i < bufSize; ++i) {
			printf("%02x ", buf[i]);
			isEnd = 0;
			if (i % 16 == 15) {
				printf("\n");
				isEnd = 1;
			}
		}
		if (!isEnd)
			printf("\n");
	}
}

void ClearBuffer(unsigned char* buf, int bufSize) {
	if (bufSize > MAX_BUFSIZE)
		printf("ClearBuffer() failed, bufSize too big: %d>%d\n", bufSize, MAX_BUFSIZE);
	else if (bufSize < 0)
		printf("ClearBuffer() failed, bufSize error: %d\n", bufSize);
	else 
		memcpy(buf, emptyBuffer, bufSize);
}