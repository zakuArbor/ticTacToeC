#ifndef SOCKET_H

#define SOCKET_H

#ifndef PORT
  #define PORT 30002
#endif
#define BUF_SIZE 1024

#define MAX_BACKLOG 2
#define MAX_CONNECTIONS 2

int read_socket(int fd, char **buf, int *buf_len);
int write_socket(int fd, char *buf, int buf_len);
#endif