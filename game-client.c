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


int main(void) { 
    char *buf;

    if (!(buf = (char *)malloc(sizeof(char) * 1024))) {
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
    int num_read;

    while (1) {
        FD_SET(sock_fd, &fdset);
        FD_SET(fileno(stdin), &fdset);
        if (select(sock_fd + 1, &fdset, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }
        if (FD_ISSET(fileno(stdin), &fdset)) {
            num_read = read(STDIN_FILENO, buf, BUF_SIZE - 2);
            if (num_read == 0) {
                break;
            }
            if (buf[num_read-1] == '\n') {
                num_read -=1;
            }
            buf[num_read] = '\r';
            buf[num_read + 1] = '\n';
            num_read +=2;

            write_socket(sock_fd, buf, num_read);
        }

        if (FD_ISSET(sock_fd, &fdset)) {
            num_read = 0;
            int client_closed = read_socket(sock_fd, &buf, &num_read);
            if (client_closed < 0) {
                break;
            } else {
                printf("Echoing message from server\n");
                printf("%s\n", buf);
            }
        } 
    }

    close(sock_fd);
    free(buf);
    return 0;
}
