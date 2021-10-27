#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include "readnwrite.c"

// different from client
#define BUF_SIZE 128

int  _recv_ahead(int sock, char* p) {
	// static -> DATA SECTION
	static int nbytes = 0;
	static char buf[1024] = {0x00, };
	static char* ptr;

	if(nbytes == 0 || nbytes == -1) {
		nbytes = read(sock, buf, sizeof(buf));

		if(nbytes == -1) {
			return -1;
		}
		else if(nbytes == 0) {
			return 0;
		}
		ptr = buf;
	}
	--nbytes;
	*p = *ptr++;

	return 1;
}

int readline(int sock, char* buf, int maxlen) {
	int n, nbytes;
	char c;
	char* ptr = buf;

	for(n = 1; n < maxlen; n++) {
		// nbytes = read(sock, &c, 1);
		nbytes = _recv_ahead(sock, &c);

		if(nbytes == 1) {
			*ptr++ = c;
			if(c == '\n') {
				break;
			}
		}
		else if(nbytes == 0) {
			*ptr = '\0';
			return (n-1);
		}
		else {
			// error
			return -1;
		}
	}
	*ptr = '\0';
	return n;
}


int main(int argc, char* argv[]) {
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char buf[BUF_SIZE+1];
	int dataLen;

	if(argc != 2) {
		fprintf(stderr, "%s <port>\n", argv[0]);
		exit(1);
	}

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
			// changed here
			// readline()
			dataLen = readline(clnt_sock, buf, BUF_SIZE+1);
			if(dataLen == -1) {
				err("readline() error");
			}
			else if(dataLen == 0) {
				break;
			}

			printf("[TCP/%s:%d] %s\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port), buf);
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
