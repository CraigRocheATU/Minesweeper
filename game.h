#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "board.h"
#include "save.h"

typedef struct {
	GameState state;
} Game;

#endif // GAME_H

void game_new(Game* g, int num_players, const char* u0, const char* n0, const char* u1, const char* n1);

