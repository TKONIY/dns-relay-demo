#ifndef	_CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "control.h"

static const char emptyBuffer[MAX_BUFSIZE] = { '\0' }; /*空buffer*/

/*extern const char* gDefaultDBtxt = "dnsrelay.txt";*/	
/*extern const char* gDefaultDBsqlite = "dnsrelay.db";*/
char gDBtxt[100] = "dnsrelay.txt";
char gDBsqlite[100] = "dnsrelay.db";
char addrDNSserv[16] = "202.106.0.20"; 
char addrDNSclie[16] = "127.0.0.1";
int gDebugLevel = 0;


event_type WaitForEvent() {
	UpdateCache(); /*更新cache*/
	return dgram_arrival;
}


void DebugBuffer(const unsigned char* buf, int bufSize) {
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
		memset(buf, 0, bufSize);
		/*memcpy(buf,emptyBuffer, bufSize);*/
}

extern int dealOpts(int argc, char* argv[]) {
	int i = 1;/*从第1个参数开始*/

	if (i >= argc)return 1;	/*识别结束,正确*/

	/*获取调试级别*/
	if (!strcmp(argv[i], "-dd")) {
		/*printf("使用调试级别2\n");*/
		gDebugLevel = 2;
		++i;
	} else if (!strcmp(argv[i], "-d")) {
		/*printf("使用调试级别1\n");*/
		gDebugLevel = 1;
		++i;
	}

	if (i >= argc)return 1;

	/*获取DNS服务器的IP地址*/
	int ipInt[4];
	if ((4 == sscanf(argv[i], "%d.%d.%d.%d", ipInt + 0, ipInt + 1, ipInt + 2, ipInt + 3))
		&& ipInt[0] < 256 && ipInt[1] < 256 && ipInt[2] < 256 && ipInt[3] < 256) { /*简单检查一下ip地址*/
		/*将他们存为字符串,遍历4个数字*/
		int stri = 0;
		for (int ipInti = 0; ipInti < 4; ipInti++) {
			if (stri) { addrDNSserv[stri++] = '.'; }/*除了开头都加点*/
			/*遍历每个数字*/
			int zeroValid = 0;						/*表示后续的0是否要写进去*/
			for (int div = 100; div > 0; div /= 10) {
				addrDNSserv[stri] = ipInt[ipInti] / div; 	/*计算当前位的数值*/
				if (zeroValid || addrDNSserv[stri]) {		/*如果当前值是有效0或不为0*/
					addrDNSserv[stri++] += 48;				/*转换为字符*/
					ipInt[ipInti] %= div;			/*去掉最高位*/
					zeroValid = 1;					/*后续的0都有效，需要写入字符串*/
				} /*否则不管,等下一位*/
			}
			if (!zeroValid) { addrDNSserv[stri++] = '0'; }	/*对于0的数，补充一个0*/
		}
		addrDNSserv[stri] = '\0';
		/*printf("DNS server ip: %s\n", addrDNSserv);*/
		++i; /*下一个命令行参数*/
	}

	if (i >= argc)return 1;

	/*获取数据库txt文件的名字*/
	if (!strcmp(argv[i] + strlen(argv[i]) - 4, ".txt")) {
		strcpy(gDBtxt, argv[i]);
		/*printf("fileName: %s\n", argv[i]);*/
		++i;
	}

	if (i >= argc)return 1; /*处理完了所有参数,返回成功*/
	else return 0;			/*没处理完,返回失败*/

}