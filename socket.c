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
* Return the index of the first occurance of "\r\n" stored in the buffer. Else return -1
*/
int find_network_newline(char **buf, int buf_len) {
	//find the first occurance of \r\n
	for (int i = 0; i < buf_len; i++) {
		if (i + 1 < buf_len && (*buf)[i] == '\r' && (*buf)[i + 1] == '\n') {
			return i;
		}		
	}
	return -1;
}

void strip_network_newline(char *buf, int buf_len) {
	if (buf[buf_len-2] == '\r') {
		buf[buf_len-2] = '\0';
	}
}

void print(char *buf, int buf_len) {
	for(int i = 0; i < buf_len; i++) {
		if (buf[i] == '\r' || buf[i] == '\n') {
			printf("*");
		}
		else {
			printf("|%c|", buf[i]);
		}
	}
	printf("\n");
}

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
	printf("on read socket\n");
	printf("before buf_len: %d\n", *buf_len);


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

	print(*buf, *buf_len);

	if ((*buf)[*buf_len-2] == '\r' && (*buf)[*buf_len - 1] == '\n') {
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

/*
* Parse message (called a packet) sent either by the server or player
*
* NOTE: Buffer gets shifted after being parsed to anything after the network newline
*
* @param buf: the buffer to parse
* @param buf_len: the length of the buffer to parse
* @param pkt: the packet that is sent (contains the message and the user)
* @return: a non-zero if an error occurs
*/
int parse_packet(char **buf, int *buf_len, message_t *pkt) {
	if (!buf || !pkt || buf_len <= 0) {
		return 1;
	}
	if (*buf_len > BUF_SIZE) {
		*buf_len = BUF_SIZE;
	}

	printf("parsing message\n");

	/*
	* STATES:
	* 0 - get message type
	* 2 - get username
	* 4 - get message till "\r\n"
	*/
	int state = 0;
	int name_len = 0, msg_len = 0, start_index;
	char temp_buf[BUF_SIZE];

	for(int i = 0; i < *buf_len; i++) {
		//printf("state: %d\n", state);
		switch(state) {
			case 0:
				if (isdigit((*buf)[i]) == 0) {
					return 1;
				}
				pkt->msg_type = (*buf)[i] - '0';
				state = 1;
				break;
			case 1:
				if ((*buf)[i] == ' ') {
					state++;
					start_index = i + 1;
				}
				break;
			case 2:
				if ((*buf)[i] == ' ' || name_len >= USERNAME_SIZE) {
					state = 3;
					
					strncpy(pkt->username, *buf+start_index, name_len);
					pkt->username[name_len] = '\0';
					start_index = i + 1;
				}
				else if (name_len < USERNAME_SIZE - 1) { 
					name_len++;
				}
				break;
			case 3:
				//msg_type(1B) + ' '(1B) + username(name_len B) + ' '(1B)
				printf("msg: |%c| ", (*buf)[i]);
				if ((*buf)[i] != '\r' && msg_len < MSG_SIZE - 1 - 1 - name_len - 1) {
					msg_len++;
				}
				else {
					strncpy(pkt->msg, *buf+start_index, msg_len);
					pkt->msg[msg_len] = '\0';

					//buf + start_index + msg_len + 2(network newline)
					printf("before\n");
					print(*buf, *buf_len);
					int offset = start_index + msg_len + 2;
					(*buf_len) -= offset;
					memmove((*buf), *buf+offset, *buf_len);
					printf("\t new buf len: %d\n", *buf_len);
					printf("\t new buf: ");
					print(*buf, *buf_len);
					
					return 0;
				}
				break;
		}
	}
	return 1;
}

/*
* Format paramters to a packet
*
* @param buf: the message to send
* @param buf_len: the length of the buffer to send
* @param username: the username who is sending the message
* @param msg_type: the type of message to send
* @param pkt: the packet to store all the parameters
* @return: a non-zero if an error occurs
*/
int format_packet(char *buf, int buf_len, char *username, int msg_type, message_t *pkt) {
	printf("on format packet\n");
	if (!username || strlen(username) >= USERNAME_SIZE || !buf || buf_len <= 0 || !pkt) {	
		return 1;
	}
	if (buf_len > MSG_SIZE) {
		buf_len = MSG_SIZE;
	}
	
	pkt->msg_type = msg_type;

	strncpy(pkt->username, username, strlen(username));
	pkt->username[strlen(username)] = '\0';

	strncpy(pkt->msg, buf, buf_len);
	pkt->msg[buf_len] = '\0';
	printf("packet format completed\n");
	return 0;
}

/*
* Create a message to send following the protocol stated under protocol.md
*
* @param pkt: contains all the information we wish to send (i.e. username, msg_type, and the message itself)
* @param send_buf: the message that will store the formatted message to send 
* @return: a non-zero if an error occurs
*/
int packet_to_string(message_t *pkt, char **send_buf) {
	if (!send_buf || !pkt) {
		printf("should not be in here\n");
		return 1;
	}

	snprintf(*send_buf, 3, "%d ", pkt->msg_type);
	strncat(*send_buf, pkt->username, strlen(pkt->username));
	strncat(*send_buf, " ", 2);
	strncat(*send_buf, pkt->msg, strlen(pkt->msg));
	strncat(*send_buf, "\r\n", 3);
	//printf("to send: %s\n", *send_buf);
	print(*send_buf, strlen(*send_buf));
	return 0;
}