#include "game.h"
#include "socket.h"

#ifndef GAME_SOCKET_H

#define GAME_SOCKET_H

int move_player(player **players, int *num_moves, int *moves, int *player_num, char *move_str);
int packet_handler(message_t *pkt, char **send_buf, player **players, int *num_moves, int *moves, int *player_num, int fd);
void notify_move(player **players, message_t *pkt, char **send_buf, char *username, int player_num);
int is_number(char *num_str);
int get_port(char *port_str);

#ifdef CLIENT_MACRO
	extern void set_players(player **players, int *player_num, int first_player_id);
#endif

#endif