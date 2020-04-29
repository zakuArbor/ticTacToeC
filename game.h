#ifndef GAME_H

#define GAME_H

typedef int bool;
#define true 1
#define false 0

#define ADMIN_USERNAME "admin"

typedef struct 
{
	char name[10];
	char piece; //either x or o
	int isHuman; //true(1) for human player, false(0) for computer player

	#ifdef SOCKET_V
	int fd;
	char *buf;
	int buf_len;
	#endif
} player;

enum game_state_t { START_STATE, PLAYER_STATE, GAME_STATE, END_STATE };

int isWin(int *moves);
void resetGame(player **players, int *moves, int *num_moves, int *player_num);
void makeMove(player **players, int *num_moves, int *moves, int *player_num);
void write_move(int *num_moves, int *moves, int *player_num, int move);
int move_to_win(int *moves, int player_num);
void ai_move(player **players, int *moves, int *player_num, int *num_moves);
void player_move(player **players, int *moves, int *player_num, int *num_moves);
player **initGame(int *moves, int *num_moves, int *player_num);
void free_players(player **players);
void drawBoard(player **players, int num_moves, int *moves);

#endif