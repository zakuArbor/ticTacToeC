#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "game.h"
#include "socket.h"

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


int main(void) {	
	player **players;

	int num_moves;
	int player_num = -1; //default -1: no players in the game
	int quit = 0;
	int moves[9]; //-1 for empty slot, 0 for player1 and 1 for player2
	int connections = 0;	//keeps track of number of people connected to the server
							// 0 <= connections <= 2
	int player_1 = -1; //keeps track who is the first player to choose their piece

	printf("Hello World\n");

	if ((players = malloc(sizeof(player*) * 2)) == NULL) {
		perror("malloc");
		return(1);
	}

	if (initGame(players, moves, &num_moves, &player_num) != 0) {
		return(1);
	}

    for (int index = 0; index < MAX_CONNECTIONS; index++) {
        players[index]->fd = -1;
        players[index]->isHuman = true;
    }

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds, listen_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }
        printf("nready: %d\n", nready);

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, players);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            printf("Accepted connection\n");

            printf("returned client_fd %d\n", client_fd);
            printf("fd: %d\n", players[0]->fd);

            //instead of writing an integer to the client by first converting the int using htonl, I'll just convert the integer to a string
            char *num_str;
            sprintf(num_str, "%d",connections);
            printf("num: %s\n", num_str);

            write(client_fd, num_str, strlen(num_str) + 1);
            printf("write\n");

            if (!connections) {
            	player_1 = get_player_index(players, client_fd);
            }

            /*write(client_fd, "hello world", strlen("hello world"));
            if (connections == 0) {
            	//set player
            	set_players(get_index(players), players);
            }*/
        }

        // Next, check the clients.
        // NOTE: We could do some tricks with nready to terminate this loop early.
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (players[index]->fd > -1 && FD_ISSET(players[index]->fd, &listen_fds)) {
                // Note: never reduces max_fd
                int client_closed = read_from(index, players, player_1, connections);
                if (client_closed > 0) {
                    FD_CLR(client_closed, &all_fds);
                    printf("Client %d disconnected\n", client_closed);
                } else {
                    printf("Echoing message from client %d\n", players[index]->fd);
                    //write(usernames[index].sock_fd, &);
                    //printf("%s: %");
                }
            }
        }
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
