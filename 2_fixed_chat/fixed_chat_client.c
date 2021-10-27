#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "readnwrite.c"

#define BUF_SIZE 128

int main(int argc, char* argv[]) {
	int clnt_sock;
	struct sockaddr_in clnt_addr;

	char IPAddr[] = "127.0.0.1";
	char msg[BUF_SIZE+1];
	char* buf;
	char* chatid;
	int n, len;
	int lenOfchatid;

	if(argc != 3) {
		printf("Usage : %s <port> <id>\n", argv[0]);
		exit(1);
	}

	chatid = argv[2];

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

	sprintf(msg, "[%s] ", chatid);
	lenOfchatid = strlen(msg);
	buf = msg + lenOfchatid;

	while(1) {
		printf("[%s] ", chatid);
		if(fgets(buf, (BUF_SIZE+1)-lenOfchatid, stdin) == NULL) {
			break;
		}
		len = strlen(buf);
		if(buf[len-1] == '\n') {
			// fgets()는 enter 시 개행문자가 들어감
			buf[len-1] = '\0';
		}
		if(strlen(buf) == 0) {
			break;
		}

		// writen()
		n = writen(clnt_sock, msg, BUF_SIZE);
		if(n == -1) {
			err("writen() error");
		}

		// readn()
		n = readn(clnt_sock, buf, BUF_SIZE);
		if(n == -1) {
			err("readn() error");
		}
		else if (n == 0) {
			break;
		}
		buf[n] = '\0';
		printf("%s\n", buf);
	}

	// close()
	close(clnt_sock);

	return 0;
}

// EOF
