#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "readnwrite.c"

#define BUF_SIZE	128

int main(int argc, char* argv[])
{
	int clnt_sock;
	struct sockaddr_in clnt_addr;

	char IPAddr[] = "127.0.0.1";
	char buf[BUF_SIZE+1];
	char* testdata[] = {"Hello", "I'm topcue", "Nice to meet you", "What do you wnat for me", "So do I"};
	int n, cnt_i;
	int dataLen;

	if(argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	// socket()
	clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(clnt_sock == -1) {
		err("socket() eror");
	}

	// set address
	memset(&clnt_addr, 0, sizeof(clnt_addr));
	clnt_addr.sin_family = AF_INET;
	clnt_addr.sin_addr.s_addr = inet_addr(IPAddr);
	clnt_addr.sin_port = htons(atoi(argv[1]));

	// connect()
	if(connect(clnt_sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) == -1) {
		err("connect() error");
	}
	else {
		puts("\n[*]Connected\n");
	}

	for(cnt_i = 0; cnt_i < sizeof(testdata)/sizeof(char*); cnt_i++) {
		memset(buf, '#', sizeof(buf));
		// send
		dataLen = strnlen(testdata[cnt_i], sizeof(buf));
		strncpy(buf, testdata[cnt_i], dataLen);
		// fixed BUF_SIZE
		n = writen(clnt_sock, buf, BUF_SIZE);
		if(n == -1) {
			err("write() error");
		}
		printf("[TCP Client] %d byte sent\n", n);
	}

	// close()
	close(clnt_sock);

	return 0;
}

// EOF


