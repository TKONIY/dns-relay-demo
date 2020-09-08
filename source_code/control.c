#ifndef	_CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "control.h"

static const char emptyBuffer[MAX_BUFSIZE] = { '\0' }; /*��buffer*/

/*extern const char* gDefaultDBtxt = "dnsrelay.txt";*/
/*extern const char* gDefaultDBsqlite = "dnsrelay.db";*/
char gDBtxt[100] = "dnsrelay.txt";
char gDBsqlite[100] = "dnsrelay.db";
char addrDNSserv[16] = "202.106.0.20";
char addrDNSlocalhost[16] = "127.0.0.1";
int gDebugLevel = 0;


void DebugBuffer(const unsigned char* buf, int bufSize) {
	if (gDebugLevel < 2)return; /*���Լ���2���ϲ����buffer*/
	char isEnd = 0;
	if (bufSize > MAX_BUFSIZE)
		debugPrintf("DebugBuffer() failed, bufSize too big: %d>%d", bufSize, MAX_BUFSIZE);
	else {
		/*bufSize = ((bufSize - 1) / 16 + 1) * 16;*/ /*debugʱ��ӡ�������ڴ�*/
		for (int i = 0; i < bufSize; ++i) {
			debugPrintf("%02x ", buf[i]);
			isEnd = 0;
			if (i % 16 == 15) {
				debugPrintf("\n");
				isEnd = 1;
			}
		}
		if (!isEnd)
			debugPrintf("\n");
	}
}

void ClearBuffer(unsigned char* buf, int bufSize) {
	if (bufSize > MAX_BUFSIZE)
		debugPrintf("ClearBuffer() failed, bufSize too big: %d>%d\n", bufSize, MAX_BUFSIZE);
	else if (bufSize < 0)
		debugPrintf("ClearBuffer() failed, bufSize error: %d\n", bufSize);
	else
		memset(buf, 0, bufSize);
	/*memcpy(buf,emptyBuffer, bufSize);*/
}

extern int dealOpts(int argc, char* argv[]) {
	int i = 1;/*�ӵ�1��������ʼ*/

	if (i >= argc)return 1;	/*ʶ�����,��ȷ*/

	/*��ȡ���Լ���*/
	if (!strcmp(argv[i], "-dd")) {
		/*printf("ʹ�õ��Լ���2\n");*/
		gDebugLevel = 2;
		++i;
	} else if (!strcmp(argv[i], "-d")) {
		/*printf("ʹ�õ��Լ���1\n");*/
		gDebugLevel = 1;
		++i;
	}

	if (i >= argc)return 1;

	/*��ȡDNS��������IP��ַ*/
	int ipInt[4];
	if ((4 == sscanf(argv[i], "%d.%d.%d.%d", ipInt + 0, ipInt + 1, ipInt + 2, ipInt + 3))
		&& ipInt[0] < 256 && ipInt[1] < 256 && ipInt[2] < 256 && ipInt[3] < 256) { /*�򵥼��һ��ip��ַ*/
		/*�����Ǵ�Ϊ�ַ���,����4������*/
		int stri = 0;
		for (int ipInti = 0; ipInti < 4; ipInti++) {
			if (stri) { addrDNSserv[stri++] = '.'; }/*���˿�ͷ���ӵ�*/
			/*����ÿ������*/
			int zeroValid = 0;						/*��ʾ������0�Ƿ�Ҫд��ȥ*/
			for (int div = 100; div > 0; div /= 10) {
				addrDNSserv[stri] = ipInt[ipInti] / div; 	/*���㵱ǰλ����ֵ*/
				if (zeroValid || addrDNSserv[stri]) {		/*�����ǰֵ����Ч0��Ϊ0*/
					addrDNSserv[stri++] += 48;				/*ת��Ϊ�ַ�*/
					ipInt[ipInti] %= div;			/*ȥ�����λ*/
					zeroValid = 1;					/*������0����Ч����Ҫд���ַ���*/
				} /*���򲻹�,����һλ*/
			}
			if (!zeroValid) { addrDNSserv[stri++] = '0'; }	/*����0����������һ��0*/
		}
		addrDNSserv[stri] = '\0';
		/*printf("DNS server ip: %s\n", addrDNSserv);*/
		++i; /*��һ�������в���*/
	}

	if (i >= argc)return 1;

	/*��ȡ���ݿ�txt�ļ�������*/
	if (!strcmp(argv[i] + strlen(argv[i]) - 4, ".txt")) {
		strcpy(gDBtxt, argv[i]);
		/*printf("fileName: %s\n", argv[i]);*/
		++i;
	}

	if (i >= argc)return 1; /*�����������в���,���سɹ�*/
	else return 0;			/*û������,����ʧ��*/

}

void debugPrintf(const char* cmd, ...) {
	if (gDebugLevel < 1) return; /*���Լ���>=1��ִ��*/
	va_list args;
	va_start(args, cmd);
	vprintf(cmd, args);
	va_end(args);
}

