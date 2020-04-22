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


extern void makeMove(player **players, int *num_moves, int *moves, int *player_num);
extern void set_players(int index, player **players);

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
* @return: return non-zero if there is an issue
*/
int server_setup(int *sock_fd, struct sockaddr_in *server) {
    *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
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


int main(void) {	
	player **players;
    struct sockaddr_in server;
    int sock_fd = -1;
    int max_fd;
    fd_set all_fds, listen_fds;

	int num_moves;
	int player_num = -1; //default -1: no players in the game
	int quit = 0;
	int moves[9]; //-1 for empty slot, 0 for player1 and 1 for player2
	int connections = 0;	//keeps track of number of people connected to the server
							// 0 <= connections <= 2
	int player_1 = -1; //keeps track who is the first player to choose their piece

	if (!(players = initGame(moves, &num_moves, &player_num))) {
		return(1);
	}

    
    if (server_setup(&sock_fd, &server) != 0) {
        goto terminate;
    }

    max_fd = sock_fd;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
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
            FD_SET(client_fd, &all_fds);
        }

        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (players[index]->fd > -1 && FD_ISSET(players[index]->fd, &listen_fds)) {
                //int client_closed = read_from(index, players, player_1, connections);
                printf("send: %p\n", players[index]->buf);
                int client_closed = read_socket(players[index]->fd, &(players[index]->buf), &(players[index]->buf_len));
                if (client_closed < 0) {
                    FD_CLR(players[index]->fd, &all_fds);
                    printf("Client %d disconnected\n", client_closed);
                } else {
                    printf("Echoing message from client %d\n", players[index]->fd);
                    broadcast_socket(players[index]->buf, players[index]->buf_len, (void **)players, 2, PLAYER);
                }
            }
        }
    }

terminate:
    if (players) {
        free_players(players);
    }
    if (sock_fd > 0) {
        close(sock_fd);
    }

    // Should never get here.
    return 1;


}

/*
* Allow player to make a move
*
* @param players: an array of 2 player struct
* @param num_moves: a reference to the number of moves that has been played
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
*/
void makeMove(player **players, int *num_moves, int *moves, int *player_num) {
	int move;
	char s[2];

	printf("Player %d Turn: ", *player_num + 1);
	scanf("%s", s);
	move = strtol(s, NULL, 10);

	if (move >= 0 && move < 9 && moves[move] == -1) {
		write_move(num_moves, moves, player_num, move);
	}
}
