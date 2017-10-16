#include <stdio.h>
#include <stdlib.h>

struct player 
{
	char name[10];
	char piece; //either x or o
};

extern int initGame(struct player *player1, struct player *player2, int *num_moves, int *moves);

int main (int argc, char **argv) {
	struct player *player1, *player2;
	player1 = NULL;
	player2 = NULL;
	int num_moves = 0;
	int moves[9];

	printf("Hello World");
	if (initGame(player1, player2, &num_moves, moves) != 0) {
		return(1);
	}

	return(0);
}


int initGame(struct player *player1, struct player *player2, int *num_moves, int *moves) {
	int i;

	if ((player1 = malloc(sizeof(struct player))) == NULL) {
		perror("malloc");
		return(1);
	}

	if ((player2 = malloc(sizeof(struct player))) == NULL) {
		perror("malloc");
		return(1);
	}	

	*num_moves = 0;

	for (i = 0; i < 9; i++) {
		moves[i] = 0;
	}
	return(0);
}