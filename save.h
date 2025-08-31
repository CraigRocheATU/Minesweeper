#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include "board.h"

//GameState Structure
typedef struct {
    int num_players;
    char usernames[2][64];
    char display_names[2][64];
    int scores[2];
    int current_player;
    Board board;
    bool game_over;
    int winner;
} GameState;

//Declaration of functions
bool ensure_saves_dir(void);
bool list_save_files(char files[][256], int max_files, int* out_count);
bool save_game_to_file(const char* filepath, const GameState* gs);
bool load_game_from_file(const char* filepath, GameState* gs);
void print_save_summary(const GameState* gs);

#endif // SAVE_H
