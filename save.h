#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include "board.h"

typedef struct {
	int num_players;
	char usernames[2][64];
	char display_names[2][64];
	int scores[2];
	int current_player;
	Board board;
	bool game_over;
	int game_winner;
} GameState;

#endif // SAVE_H
