#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include "readnwrite.c"

#define BUF_SIZE 128

int main(int argc, char* argv[]) {
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char msg[BUF_SIZE+1];
	char* buf;
	char* chatid;
	int len, lenOfchatid;
	int n;

	if(argc != 3) {
		fprintf(stderr, "%s <port> <id>\n", argv[0]);
		exit(1);
	}

	chatid = argv[2];

	// socket()
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1) {
		err("socket() error");
	}

	// set address
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	// bind()
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		err("bind() error");
	}

	// listen()
	if(listen(serv_sock, 5) == -1) {
		err("listen() error");
	}

	sprintf(msg, "[%s] ", chatid);
	lenOfchatid = strlen(msg);
	buf = msg + lenOfchatid;

	// here
	while(1) {
		// accept()
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1) {
			err("accept() error");
		}
		printf("\n[TCP Server] Client connected: IP=%s, port=%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));


		while(1) {
			// readn()
			n = readn(clnt_sock, buf, BUF_SIZE);
			if(n == -1) {
				err("readn() error");
			}
			else if (n == 0) {
				// get EOF
				break;
			}
			buf[n] = '\0';
			printf("%s\n", buf);
	
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
		}
		// close()
		close(clnt_sock);
		printf("[TCP Server] Client closed : Ip=%s, port=%d", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
	}

	// close()
	close(clnt_sock);
	close(serv_sock);

	return 0;
}

// EOF
