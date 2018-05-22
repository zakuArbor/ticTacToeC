#include "game.h"

#ifndef CLI_H

/*
* Initializes the players and the board
*
* @param players: an array of size 2 that contains the player information
* @param moves: the moves made on the board
* @param num_moves: a reference to the number of moves made in the game
* @param player_num: a reference to the current player's number (which player's turn is it) 
*/
int initGame(player **players, int *moves, int *num_moves, int *player_num) {
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
	
	resetGame(players, moves, num_moves, player_num);

	return(0);
}

#endif