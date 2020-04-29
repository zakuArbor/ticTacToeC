#include "game_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Allow player to make a move
*
* @param players: an array of 2 player struct
* @param num_moves: a reference to the number of moves that has been played
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
* @param move_str: player's move stored in a string
* @return: return a non-negative number (0-9) if move is valid
*/
int move_player(player **players, int *num_moves, int *moves, int *player_num, char *move_str) {
	int move;
    move = strtol(move_str, NULL, 10);

    printf("move player: %d - piece: %d\n", move, moves[move]);

	if (move >= 0 && move < 9 && moves[move] == -1) {
        printf("pika move\n");
		write_move(num_moves, moves, player_num, move);
        printf("end pika move\n");
        return move;
	}
    return -1;
}

void notify_move(player **players, message_t *pkt, char **send_buf, char *username, int player_num) {
    char buf[BUF_SIZE];
    snprintf(buf, 21, "Please make a move: ");
            
    format_packet(buf, strlen(buf), username, GAME_MESSAGE, pkt);
    packet_to_string(pkt, send_buf);
    write_socket(players[player_num]->fd, *send_buf, strlen(*send_buf));
}

int packet_handler(message_t *pkt, char **send_buf, player **players, int *num_moves, int *moves, int *player_num, int fd) {
    char buf[BUF_SIZE];
    printf("on pkt handler: %d\n", pkt->msg_type);
    printf("%s", pkt->msg);
    printf("\n===============\n");
    for (int i = 0; i < 9; i++) { printf("%d \t", moves[i]); }
    switch(pkt->msg_type) {
        case GAME_MESSAGE:
            #ifdef CLIENT_MACRO
                printf("\n%s", pkt->msg);
            #endif
            break;
        case GAME_CONTROL:
            #ifdef CLIENT_MACRO
                printf("\non game-control\n");
                if (strncmp(pkt->msg, ASSIGN_CONTROL, strlen(ASSIGN_CONTROL)) == 0) {
                    printf("username: %s\n", pkt->username);
                    int player_id = strtol(pkt->username, NULL, 10);
                    set_players(players, player_num, player_id);
                }
            #endif
            break;
        case PLAYER_ACTION:
            #ifdef SERVER_MACRO
            if (players[*player_num]->fd != fd) {
                printf("Not %d turn\n", fd);
                break;
            }
            #endif
            if (move_player(players, num_moves, moves, player_num, pkt->msg)  >= 0) {                
                #ifdef SERVER_MACRO
                    strip_network_newline(pkt->msg, strlen(pkt->msg));
                    packet_to_string(pkt, send_buf);
                    broadcast_socket(*send_buf, strlen(*send_buf), (void **)players, 2, PLAYER);
                    notify_move(players, pkt, send_buf, ADMIN_USERNAME, *player_num);
                #endif
                drawBoard(players, *num_moves, moves);
            }
            else {

            }
            break;
        default:
            return 1;
    }
    return 0;
}