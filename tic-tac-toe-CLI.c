#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	char name[10];
	char piece; //either x or o
} player;

extern int initGame(player **players, int *moves);
extern void drawBoard(player **players, int num_moves, int *moves);

int main (int argc, char **argv) {
	player **players;
	if ((players = malloc(sizeof(player*) * 2)) == NULL) {
		perror("malloc");
		return(1);
	}

	int num_moves = 0;
	int moves[9];

	printf("Hello World\n");
	
	if (initGame(players, moves) != 0) {
		return(1);
	}

	drawBoard(players, num_moves, moves);

	return(0);
}

int initGame(player **players, int *moves) {
	int i;

	if (!(players[0] = malloc(sizeof(player)))) {
		perror("malloc");
		return(1);
	}


	if (!(players[1] = malloc(sizeof(player)))) {
		perror("malloc");
		return(1);
	}

	players[0]->piece = 'x';
	players[1]->piece = 'o';
	

	for (i = 0; i < 9; i++) {
		moves[i] = 0;
	}

	return(0);
}

void drawBoard(player **players, int num_moves, int *moves) {
	int row, col;
	for (row = 0; row < 3; row++) {
		for (col = 0; col < 5; col++) {
			if (col % 2 == 0) { //a slot
				if (num_moves > 0 && moves[(row*3) + (col/2)] != 0) { //if there is a piece placed
					printf("%c", moves[(row*3) + (col/2)]);
				}
				else {
					printf(" ");
				}
			} 
			else { //a border
					printf("|");
			}
		}
		if (row < 2) printf("\n-----\n");
	}
	printf("\n");
}