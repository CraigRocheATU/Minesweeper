#include "game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void game_new(Game* g, int num_players, const char* u0, const char* n0, const char* u1, const char* n1) {
	g->state.num_players = num_players;
	strncpy(g->state.usernames[0], u0 ? u0 : "player1", sizeof(g->state.usernames[0]) - 1);
	strncpy(g->state.display_names[0], n0 ? n0 : "Player 1", sizeof(g->state.display_names[0]) - 1);
	strncpy(g->state.usernames[1], u1 ? u1 : "player2", sizeof(g->state.username[1]) - 1);
	strncpy(g->state.display_names[1], n1 ? n1 : "Player 2", sizeof(g->state.display_names[1]) - 1);
	g->state.score[0] = 0;
	g->state.scores[1] = 0;
	g->state.current_player = 0;
	g->state.game_over = false;
	g->state.game_winner = -1;

	srand((unsigned)time(NULL));
	board_init(&g->state.board);
}
