#ifndef SOCKET_H

#define SOCKET_H

#ifndef PORT
  #define PORT 30002
#endif
#define BUF_SIZE 1024

#define MAX_BACKLOG 2
#define MAX_CONNECTIONS 2

enum client_type_t {
	PLAYER
};

int read_socket(int fd, char **buf, int *buf_len);
int write_socket(int fd, char *buf, int buf_len);
int broadcast_socket(char *buf, int buf_len, void **clients, int clients_len, enum client_type_t client_type);
#endif