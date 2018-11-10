#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "game.h"


#ifndef PORT
  #define PORT 30000
#endif
#define BUF_SIZE 128

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
    // Read input from the user, send it to the server, and then accept the
    // echo that returns. Exit when stdin is closed.
    char buf[BUF_SIZE + 1];
    

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
            num_read = read(STDIN_FILENO, buf, BUF_SIZE);
            if (num_read == 0) {
                break;
            }
            buf[num_read] = '\0';         // Just because I'm paranoid

            int num_written = write(sock_fd, buf, num_read);
            if (num_written != num_read) {
                perror("client: write");
                close(sock_fd);
                exit(1);
            }
        }
        if (FD_ISSET(sock_fd, &fdset)) {
            num_read = read(sock_fd, buf, BUF_SIZE);
            if (num_read == 0) {
                break;
            }
            buf[num_read] = '\0';
            printf("%s\n", buf);
        } 
    }

    close(sock_fd);
    return 0;
}
