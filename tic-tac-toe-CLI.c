#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	char name[10];
	char piece; //either x or o
} player;

extern int initGame(player **players, int *moves);
extern void drawBoard(player **players, int num_moves, int *moves);
extern void makeMove(player **players, int *num_moves, int *moves, int *player_num);

int main (int argc, char **argv) {
	player **players;

	int player_num = 0;
	int quit = 0;
	int num_moves = 0;
	int moves[9]; //-1 for empty slot, 0 for player1 and 1 for player2

	if ((players = malloc(sizeof(player*) * 2)) == NULL) {
		perror("malloc");
		return(1);
	}

	printf("Hello World\n");
	
	if (initGame(players, moves) != 0) {
		return(1);
	}

	while (!quit) {
		drawBoard(players, num_moves, moves);
		makeMove(players, &num_moves, moves, &player_num);
		if (num_moves == 9) {
			printf("check win\n");
		}
	}

	return(0);
}

/*
* Initializes the players and the board
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
*/
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
		moves[i] = -1;
	}

	return(0);
}

/*
* Draws the game board
*
* @param players: an array of 2 player struct
* @param num_moves: number of moves that has been played
* @param moves: a reference to a list of player's moves
*/
void drawBoard(player **players, int num_moves, int *moves) {
	int row, col;
	for (row = 0; row < 3; row++) {
		for (col = 0; col < 5; col++) {
			if (col % 2 == 0) { //a slot
				if (num_moves > 0 && moves[(row*3) + (col/2)] != -1) { //if there is a piece placed
					printf("%c", players[moves[(row*3) + (col/2)]]->piece);
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

/*
* Allow player to make a move
* @param players: an array of 2 player struct
* @param num_moves: a reference to the number of moves that has been played
* @param moves: a reference to a list of moves that has been made so far
* @param player_num: a reference to a variable that keeps track whose turn it is
*/
void makeMove(player **players, int *num_moves, int *moves, int *player_num) {
	int move;
	scanf("%d", &move);
	if (move >= 0 && move < 9 && moves[move] == -1) {
		moves[move] = *player_num;
		*player_num = *player_num ^ 1;
		*num_moves += 1;
	}
}