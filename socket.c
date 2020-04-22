#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "socket.h"
#include "game.h"

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
	int bytes_read = 0;
	if (fd < 0 || !buf) {
		return -1;
	}

	
	if (*buf_len > BUF_SIZE) {
		return 1;
	}
	else if (*buf_len < 0) { //should never happen
		*buf_len = 0;
	}
	
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
	int bytes_written = 0;

	if (fd < 0 || !buf || buf_len < 0) {
		return 1;
	}

	if (buf_len > BUF_SIZE) { //should never occur
		buf_len = BUF_SIZE;
	}

	if ((bytes_written = write(fd, buf, buf_len)) < buf_len) {
		perror("write");
		return 1;
	} 

	return 0;
}

/*
* Broadcast to all clients
*
* @param buf: the buffer to send
* @param buf_len: the length of the buffer to send
* @param clients: client array which must contain a fd member
* @param clients_len: the length of the clients array
* @param type: the data type of the clients array
* @return: return non-zero on failure
*/
int broadcast_socket(char *buf, int buf_len, void **clients, int clients_len, enum client_type_t client_type) {
	int status = 0;

	if (!buf || buf_len < 0 || !clients) {
		return 1;
	}

	for (int i = 0; i < clients_len; i++) {
		int fd;
		if (client_type == PLAYER) {
			fd = (((player **) clients)[i])->fd;
			if (write_socket(fd, buf, buf_len) != 0) {
				status = 1;
			}
		}
	}
	return status;
}