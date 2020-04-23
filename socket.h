#ifndef SOCKET_H

#define SOCKET_H

#ifndef PORT
  #define PORT 3001
#endif
#define BUF_SIZE 1024
#define MSG_SIZE 500
#define USERNAME_SIZE 20
#define PKT_STR_SIZE MSG_SIZE+USERNAME_SIZE

#define MAX_BACKLOG 2
#define MAX_CONNECTIONS 2

typedef struct {
	int msg_type;
	char username[USERNAME_SIZE + 1];
	char msg[BUF_SIZE + 2 + 1]; //+2 for "\r\n"
} message_t;

enum client_type_t {
	PLAYER
};

enum message_type_t {
	GAME_MESSAGE,
	PLAYER_MESSAGE
};

int read_socket(int fd, char **buf, int *buf_len);
int write_socket(int fd, char *buf, int buf_len);
int broadcast_socket(char *buf, int buf_len, void **clients, int clients_len, enum client_type_t client_type);
int parse_packet(char *buf, int buf_len, message_t *pkt);
int format_packet_string(char *buf, int buf_len, char *username, int message, char **send_buf);
#endif