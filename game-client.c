#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"
#include "game.h"
#include "game_socket.h"

/*
* Display menu and return:
*   0: If multiplayer
*   1: If single player and player goes first
*   2: If single player and ai goes first
*/
void menu(int sock_fd) {
    int option;
    char s[2];
    char c = 'i'; //some default value that is not 'y' nor 'n'
    int response = 0; //default: no (0)

    char msg[] = "Do you wish to go first?[Y/n]: ";

    while (tolower(c) != 'y' && tolower(c) != 'n') {
        printf("%s", msg);
        //write(players[index], msg, strlen(msg));
        scanf(" %c", &c); //extra space in format specifier allows input to ignore leading whitespace chars (including newline)
    }

    write(sock_fd, s, strlen(s));
}

void makeMove(player **players, int *num_moves, int *moves, int *player_num) {
    //TO BE IMPLEMENTED
}

/*
* Set players to their respective role based on game option
*
* @param players: an array of 2 player struct
*/
void set_players(player **players, int *player_num, int first_player_id) {
    if (first_player_id == 0) {
        players[0]->piece = 'x';
        players[1]->piece = 'y';
        *player_num = 0;
    } 
    else {
        players[0]->piece = 'y';
        players[1]->piece = 'x';        
        *player_num = 1;
    }
}


int main(void) { 
    char *buf, *buf_stdin;
    char *send_buf;
    message_t pkt;
    player **players;
    int num_moves;
    int player_num = -1;    //default -1: no players in the game
    int quit = 0;
    int moves[9];           //-1 for empty slot, 0 for player1 and 1 for player2
    int connections = 0;    //keeps track of number of people connected to the server
                            // 0 <= connections <= 2
    int player_1 = -1;      //keeps track who is the first player to choose their piece

    int num_read = 0, buf_len = 0;

    if (!(players = initGame(moves, &num_moves, &player_num))) {
        return(1);
    }

    if (!(buf_stdin = (char *)malloc(sizeof(char) * BUF_SIZE))) {
        return 1;
    }

    if (!(buf = (char *)malloc(sizeof(char) * BUF_SIZE))) {
        return 1;
    }

    if (!(send_buf = (char *)malloc(sizeof(char) * PKT_STR_SIZE))) {
        return 1;
    }

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 1) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        exit(1);
    }
    else {
        printf("Connected to game server\n");
        fflush(stdout);
    }    

    fd_set fdset;
    FD_ZERO(&fdset);

    while (1) {
        FD_SET(sock_fd, &fdset);
        FD_SET(fileno(stdin), &fdset);
        if (select(sock_fd + 1, &fdset, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }
        if (FD_ISSET(fileno(stdin), &fdset)) {
            num_read = read(STDIN_FILENO, buf_stdin, BUF_SIZE - 2);
            if (num_read == 0) {
                break;
            }
            if (buf_stdin[num_read-1] == '\n') {
                num_read -=1;
            }
            /*buf_stdin[num_read] = '\r';
            buf_stdin[num_read + 1] = '\n';
            num_read +=2;*/

            
            format_packet(buf_stdin, num_read, "Player1", PLAYER_ACTION, &pkt);
            packet_to_string(&pkt, &send_buf);
            write_socket(sock_fd, send_buf, strlen(send_buf));
        }

        if (FD_ISSET(sock_fd, &fdset)) {
            int client_closed = read_socket(sock_fd, &buf, &buf_len);
            if (client_closed < 0) {
                break;
            } 
            else if (client_closed == 0) {    
                while (find_network_newline(&buf, buf_len) != -1) {
                    parse_packet(&buf, &buf_len, &pkt);
                    printf("buf_len: %d\n", buf_len);
                    packet_handler(&pkt, &send_buf, players, &num_moves, moves, &player_num, 0);

                }
            }
        } 
    }
                                                                 
    close(sock_fd);
    if (buf) {
        free(buf);
    }
    if (send_buf) {
        free(send_buf);
    }
    if (players) {
        free_players(players);
    }
    return 0;
}