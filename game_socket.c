#include "game_socket.h"
#include <ctype.h>
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
        write_move(num_moves, moves, player_num, move);
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

void set_terminate_pkt(message_t *pkt) {
    strncpy(pkt->username, ADMIN_USERNAME, strlen(ADMIN_USERNAME));
    pkt->username[strlen(ADMIN_USERNAME)] = '\0';
    pkt->msg_type = GAME_TERMINATE;
}

void send_winner_loser_msg(player **players, message_t *pkt, char **send_buf, int winner) {
    set_terminate_pkt(pkt);

    snprintf(pkt->msg, 9, "You won!");
    packet_to_string(pkt, send_buf);
    packet_to_string(pkt, send_buf);
    write_socket(players[winner]->fd, *send_buf, strlen(*send_buf));

    snprintf(pkt->msg, 10, "You lose!");
    packet_to_string(pkt, send_buf);
    write_socket(players[winner^1]->fd, *send_buf, strlen(*send_buf));
}

void send_tied_msg(player **players, message_t *pkt, char **send_buf) {
    set_terminate_pkt(pkt);
    snprintf(pkt->msg, 10, "Game Tied");
    packet_to_string(pkt, send_buf);
    broadcast_socket(*send_buf, strlen(*send_buf), (void **)players, 2, PLAYER);
}

/*
* Returns 1 to indicate termination of the program
*/
int packet_handler(message_t *pkt, char **send_buf, player **players, int *num_moves, int *moves, int *player_num, int fd) {
    char buf[BUF_SIZE];
    switch(pkt->msg_type) {
        case GAME_MESSAGE:
            #ifdef CLIENT_MACRO
                printf("\n%s", pkt->msg);
                fflush(stdout);
            #endif
            break;
        case GAME_CONTROL:
            #ifdef CLIENT_MACRO
                if (strncmp(pkt->msg, ASSIGN_CONTROL, strlen(ASSIGN_CONTROL)) == 0) {
                    int player_id = strtol(pkt->username, NULL, 10);
                    set_players(players, player_num, player_id);
                }
            #endif
            break;
        case PLAYER_ACTION:
            #ifdef SERVER_MACRO
            if (players[*player_num]->fd != fd) {
                printf("Not %d turn\n", fd);
                fflush(stdout);
                break;
            }
            #endif
            if (move_player(players, num_moves, moves, player_num, pkt->msg)  >= 0) {                
                #ifdef SERVER_MACRO
                    strip_network_newline(pkt->msg, strlen(pkt->msg));
                    packet_to_string(pkt, send_buf);
                    broadcast_socket(*send_buf, strlen(*send_buf), (void **)players, 2, PLAYER);
                    if (*num_moves == 9 || isWin(moves) != -1) {
                        if (isWin(moves) != -1) {
                            send_winner_loser_msg(players, pkt, send_buf, isWin(moves));
                        }
                        else {
                            send_tied_msg(players, pkt, send_buf);
                        }
                        return 1;
                    }
                    else {
                        notify_move(players, pkt, send_buf, ADMIN_USERNAME, *player_num);
                    }
                #endif
                drawBoard(players, *num_moves, moves);
            }
            break;
        case GAME_TERMINATE:
            #ifdef CLIENT_MACRO
                printf("\n%s", pkt->msg);
                return 1;
            #endif
            break;
        default:
            return 1;
    }
    return 0;
}

int is_number(char *num_str) {
    if (!num_str) {
        return 0;
    }
    for (int i = 0; i < strlen(num_str); i++) {
        if (!isdigit(num_str[i])) {
            return 0;
        }
    }
}

int get_port(char *port_str) {
    int port;

    if (!port_str) {
        return 0;
    }
    if (!is_number(port_str)) {
        return 0;
    }
    
    port = strtol(port_str, NULL, 10);
    if (port >= 1024 && port <= 65535) {
        return port;
    }
    return 0;
}