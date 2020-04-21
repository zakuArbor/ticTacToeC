#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "socket.h"

/*
* Read from socket and store to buffer
*
* @param fd: file descriptor to read from
* @param buf: the buffer to store input
* @param buf_len: the number of bytes stored in the buffer
* @return:
* 	0: read to buffer with no issues
*	1: still bytes to read from buffer
*	-1: error in read or no bytes read
*/
int read_socket(int fd, char **buf, int *buf_len) {
	printf("%d - in read socket: %d\n", fd, *buf_len);
	if (*buf_len > BUF_SIZE) {
		return 1;
	}
	else if (*buf_len < 0) { //should never happen
		*buf_len = 0;
	}
	printf("position: %p\n", *buf + *buf_len);
	int bytes_read = 0;
	char temp[1024];
	if ((bytes_read = read(fd, *buf + *buf_len, BUF_SIZE - *buf_len)) < 0) {
		perror("read");
		return -1;
	}
	else if (bytes_read == 0) {
		return -1;
	}
	
	*buf_len += bytes_read;

	printf("bytes_read: %d\n", bytes_read);

	for (int i = 0; i < *buf_len; i++) {
		printf("|%c|\n", (*buf)[i]);
		
	}

	if ((*buf)[*buf_len - 2] == '\r' && (*buf)[*buf_len -1] == '\n') {
		(*buf)[*buf_len -2] = '\0';
		*buf_len -= 2;
		printf("read: %s\n", *buf);
		return 0;
	}
	return 1;
}

/*
* Write the contents of buf to the sockets
*
* @param fd: file descriptor to read from
* @param buf: the buffer to send
* @param buf_len: the number of bytes stored in the buffer
* @return: 0 iff there is nothing wrong else return 1
*/
int write_socket(int fd, char *buf, int buf_len) {
	return 0;
}
