#ifndef GAME_H

typedef int bool;
#define true 1
#define false 0

typedef struct 
{
	char name[10];
	char piece; //either x or o
	int isHuman; //true(1) for human player, false(0) for computer player
	int fd;
} player;

int isWin(int *moves);
int initGame(player **players, int *moves, int *num_moves, int *player_num);
void resetGame(player **players, int *moves, int *num_moves, int *player_num);
void makeMove(player **players, int *num_moves, int *moves, int *player_num);
void write_move(int *num_moves, int *moves, int *player_num, int move);
int move_to_win(int *moves, int player_num);
void ai_move(player **players, int *moves, int *player_num, int *num_moves);
void player_move(player **players, int *moves, int *player_num, int *num_moves);
player **initGame(int *moves, int *num_moves, int *player_num);

#endif