#include "game.h"

#ifndef SOCKET_H

#define SOCKET_H

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
	GAME_CONTROL,
	PLAYER_ACTION,
	PLAYER_MESSAGE,
	GAME_TERMINATE
};

#define ASSIGN_CONTROL "assign"

int read_socket(int fd, char **buf, int *buf_len);
int write_socket(int fd, char *buf, int buf_len);
int broadcast_socket(char *buf, int buf_len, void **clients, int clients_len, enum client_type_t client_type);
int parse_packet(char **buf, int *buf_len, message_t *pkt);
int format_packet(char *buf, int buf_len, char *username, int msg_type, message_t *pkt);
int packet_to_string(message_t *pkt, char **send_buf);
int find_network_newline(char **buf, int buf_len);
void strip_network_newline(char *buf, int buf_len);
void print(char *buf, int buf_len);
#endif