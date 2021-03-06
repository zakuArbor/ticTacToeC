#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "socket.h"
#include "game.h"
#include "game_socket.h"

extern int move_player(player **players, int *num_moves, int *moves, int *player_num, char *move_str);
extern void set_players(player **players, int *num_moves, int*moves, int *player_num);


void makeMove(player **players, int *num_moves, int *moves, int *player_num) {
    //NOT IMPLEMENTED
    return; 
}

int get_index(player **players) {
	int player_index = 0;
    while (player_index < MAX_CONNECTIONS && players[player_index]->fd != -1) {
        player_index++;
    }
    return(player_index);
}

int get_player_index(player ** players, int fd) {
	int i;
	for (i = 0; i < 2; i++) {
		if (players[i]->fd == fd) {
			return(i);
		}
	}
	return(-1);
}

int accept_connection(int fd, player **players) {
    int player_index = get_index(players);

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    if (player_index == MAX_CONNECTIONS) {
        fprintf(stderr, "server: max concurrent connections\n");
        close(client_fd);
        return -1;
    }

    players[player_index]->fd = client_fd;
    printf("accepted connection: %d\n", client_fd);

    return client_fd;
}

/* Read a message from client_index and echo it back to them.
 * Return the fd if it has been closed or 0 otherwise.
 */
int read_from(int client_index, player **players, int player_1, int connections) { 
    char buf[BUF_SIZE + 1];
    int num_read;
    int fd = players[client_index]->fd;
    printf("on read_from\n");    
    num_read = read(fd, &buf, BUF_SIZE);
    buf[num_read] = '\0';

    printf("read: %s\n", buf);

    if (num_read == 0) {
        players[client_index]->fd = -1;
        return fd;
    }

    if (connections == 0 && player_1 == client_index) { //reminder: pass in variable
    	connections++;
    	printf("Assigning pieces\n");
    	if (buf[0] == 'y') { //only look at first character and ignore the rest
    		players[client_index]->piece = 'x';
    		players[client_index ^ 1]->piece = 'o';
    	}
    	else {
    		players[client_index]->piece = 'o';
    		players[client_index ^ 1]->piece = 'x';
    	}
    }
            
    return(0);
}

/*
* Set-up the server
*
* @param sock_fd: to store the server's file descriptor
* @param server: the socket information to set
* @param the port to bind the application to
* @return: return non-zero if there is an issue
*/
int server_setup(int *sock_fd, struct sockaddr_in *server, int port) {
    *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    (server->sin_addr).s_addr = INADDR_ANY;
    
    //initialize to zero to avoid any possible issues
    memset(&(server->sin_zero), 0, 8);

    // Bind the selected port to the socket.
    if (bind(*sock_fd, (struct sockaddr *)server, sizeof(*server)) < 0) {
        perror("server: bind");
        return 1;
    }

    if (listen(*sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        return 1;
    }
}

void display_usage(char *prog) {
    printf("usage: %s <PORT>\n", prog);
    printf("\tPORT: Range must be within 1024 to 65535\n");
}

int main(int argc, char **argv) {	
	player **players;
    struct sockaddr_in server;
    int sock_fd = -1;
    int max_fd;
    fd_set all_fds, listen_fds;
    char buf[BUF_SIZE];
    char *send_buf;
    message_t pkt;

	int num_moves;
	int player_num = -1;    //default -1: no players in the game
	int quit = 0;
	int moves[9];           //-1 for empty slot, 0 for player1 and 1 for player2
	int connections = 0;    //keeps track of number of people connected to the server
                            // 0 <= connections <= 2
	int player_1 = -1;      //keeps track who is the first player to choose their piece
    int state = START_STATE;//the state of the game
    int port = 30002;

    if (argc < 2) {
        display_usage(argv[0]);
        return 1;
    }

    if (!(port = get_port(argv[1]))) {
        fprintf(stderr, "Error: Invalid Port Number\n");
        display_usage(argv[0]);
        return 1;
    }
    printf("port: %d\n", port);

	if (!(players = initGame(moves, &num_moves, &player_num))) {
		return(1);
	}

    if (!(send_buf = malloc(sizeof(char) * MSG_SIZE))) {
        goto terminate;
    }
    
    if (server_setup(&sock_fd, &server, port) != 0) {
        goto terminate;
    }

    max_fd = sock_fd;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        if (state == PLAYER_STATE) {
            set_players(players, &num_moves, moves, &player_num);
            state = GAME_STATE;

            player_num = 0;

            if (players[0]->piece != 'x') {
                player_num = 1;
            }

            snprintf(buf, 22, "Player %d goes first\n", players[player_num]->fd);
            char username[USERNAME_SIZE];
            strncpy(username, ADMIN_USERNAME, strlen(ADMIN_USERNAME));
            username[strlen(ADMIN_USERNAME)] = '\0';

            format_packet(buf, strlen(buf), username, GAME_MESSAGE, &pkt);
            packet_to_string(&pkt, &send_buf);      
            broadcast_socket(send_buf, strlen(send_buf), (void **)players, 2, PLAYER);  

            char username_assign[2];
            snprintf(username_assign, 2, "%d", player_num);
            strncpy(buf, ASSIGN_CONTROL, strlen(ASSIGN_CONTROL));
            buf[strlen(ASSIGN_CONTROL)] = '\0';

            format_packet(buf, strlen(buf), username_assign, GAME_CONTROL, &pkt);
            packet_to_string(&pkt, &send_buf);
            broadcast_socket(send_buf, strlen(send_buf), (void **)players, 2, PLAYER); 

            notify_move(players, &pkt, &send_buf, username, player_num);

            state = GAME_STATE;
        }

        // select updates the fd_set it receives, so we always use a copy and retain the original.
        listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            goto terminate;
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, players);

            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            if (client_fd > 0) {
                FD_SET(client_fd, &all_fds);
                char username[USERNAME_SIZE + 1];
                snprintf(username, 9,"Player%d", client_fd);
                snprintf(buf, strlen(username) + 23, "Player %d has entered the room\n", client_fd); //29 chars

                format_packet(buf, strlen(buf), username, GAME_MESSAGE, &pkt);
                packet_to_string(&pkt, &send_buf);
                broadcast_socket(send_buf, strlen(send_buf), (void **)players, 2, PLAYER);

                connections++;
                if (connections >= 2) {
                    state = PLAYER_STATE;
                }
            }
        }

        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (players[index]->fd > -1 && FD_ISSET(players[index]->fd, &listen_fds)) {
                //int client_closed = read_from(index, players, player_1, connections);
                int client_closed = read_socket(players[index]->fd, &(players[index]->buf), &(players[index]->buf_len));
                if (client_closed < 0) {
                    FD_CLR(players[index]->fd, &all_fds);
                    printf("Client %d disconnected\n", client_closed);
                    connections--;
                } else if (client_closed == 0) {
                    printf("Echoing message from client %d\n", players[index]->fd);
                    parse_packet(&(players[index]->buf), &(players[index]->buf_len), &pkt);
                    if (state == GAME_STATE) {
                        packet_handler(&pkt, &send_buf, players, &num_moves, moves, &player_num, players[index]->fd);
                    }
                }
            }
        }
    }

terminate:
    if (players) {
        free_players(players);
    }
    if (send_buf) {
        free(send_buf);
    }
    if (sock_fd > 0) {
        close(sock_fd);
    }

    // Should never get here.
    return 1;


}

/*
* Set players to their respective role based on game option
*
* Player 1 goes first iff random number is even
*
* @param players: an array of 2 player struct
*/
void set_players(player **players, int *num_moves, int*moves, int *player_num) {
    srand(time(0));
    if (rand() % 100 == 0) {
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