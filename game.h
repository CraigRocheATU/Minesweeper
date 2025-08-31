#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "save.h"

//Game Structure
typedef struct {
    GameState state;
} Game;

//Declaration of functions
void game_new(Game* g, int num_players, const char* u0, const char* n0, const char* u1, const char* n1);
bool game_load(Game* g, const char* filepath);
bool game_save(const Game* g, const char* filepath);
void game_print_board(const Game* g, bool reveal_all);
void game_loop(Game* g);

#endif // GAME_H