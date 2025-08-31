#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "game.h"
#include "board.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void game_new(Game* g, int num_players, const char* u0, const char* n0, const char* u1, const char* n1) {
    g->state.num_players = num_players;
    strncpy(g->state.usernames[0], u0 ? u0 : "player1", 63);
    strncpy(g->state.display_names[0], n0 ? n0 : "Player 1", 63);
    strncpy(g->state.usernames[1], u1 ? u1 : "player2", 63);
    strncpy(g->state.display_names[1], n1 ? n1 : (num_players == 2 ? "Player 2" : "-"), 63);
    g->state.scores[0] = g->state.scores[1] = 0;
    g->state.current_player = 0;
    g->state.game_over = false;
    g->state.winner = -1;
    srand((unsigned)time(NULL));
    board_init(&g->state.board);
}

bool game_load(Game* g, const char* filepath) { return load_game_from_file(filepath, &g->state); }
bool game_save(const Game* g, const char* filepath) { return save_game_to_file(filepath, &g->state); }

void game_print_board(const Game* g, bool reveal_all) {
    printf("\nScores: %s=%d | %s=%d | Turn: %s\n",
        g->state.display_names[0], g->state.scores[0],
        g->state.display_names[1], g->state.scores[1],
        g->state.display_names[g->state.current_player]);
    board_print(&g->state.board, reveal_all);
}

static void end_game(Game* g, int winner) {
    g->state.game_over = true;
    g->state.winner = winner;
    printf("\n=== GAME OVER ===\n");
    if (winner >= 0) printf("Winner: %s\n", g->state.display_names[winner]);
    else printf("No winner determined.\n");
    printf("Final scores: %s=%d | %s=%d\n",
        g->state.display_names[0], g->state.scores[0],
        g->state.display_names[1], g->state.scores[1]);
    board_print(&g->state.board, true);
}

void game_loop(Game* g) {
    while (!g->state.game_over) {
        if (board_is_cleared(&g->state.board)) {
            int winner = (g->state.scores[0] >= g->state.scores[1]) ? 0 : 1;
            end_game(g, winner);
            break;
        }

        game_print_board(g, false);
        printf("\nEnter a move for %s:\n", g->state.display_names[g->state.current_player]);
        printf(" - Reveal: r ROW COL  (1-8)\n");
        printf(" - Save:   s\n");
        printf(" - Quit:   q\n");
        printf("> ");

        char cmd[8]; int row, col; char line[128];
        if (!fgets(line, sizeof(line), stdin)) { clearerr(stdin); continue; }
        if (sscanf(line, "r %d %d", &row, &col) == 2) {
            if (row < 1 || row > ROWS || col < 1 || col > COLS) { printf("Invalid coordinates. Please enter numbers 1-8.\n"); continue; }
            int sum_nonzero = 0, count_rev = 0;
            int result = board_reveal(&g->state.board, row - 1, col - 1, &sum_nonzero, &count_rev);
            if (result == -1) {
                printf("Boom! %s hit a mine at (%d,%d).\n", g->state.display_names[g->state.current_player], row, col);
                int winner = (g->state.num_players == 2) ? (1 - g->state.current_player) : -1;
                end_game(g, winner);
                break;
            }
            else if (result == -2) { printf("Out of bounds.\n"); }
            else if (result == -3) { printf("That cell is already revealed.\n"); }
            else {
                g->state.scores[g->state.current_player] += sum_nonzero;
                printf("Revealed %d cell(s). Points this move: %d\n", count_rev, sum_nonzero);
                if (g->state.num_players == 2) g->state.current_player = 1 - g->state.current_player;
            }
        }
        else if (sscanf(line, "%7s", cmd) == 1) {
            if (cmd[0] == 's' || cmd[0] == 'S') {
                if (!ensure_saves_dir()) { printf("Error: could not ensure saves directory.\n"); continue; }
                time_t now = time(NULL); struct tm* tmv = localtime(&now);
                char fname[256];
                snprintf(fname, sizeof(fname), "saves/%04d%02d%02d_%02d%02d%02d_%s.msave",
                    tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
                    tmv->tm_hour, tmv->tm_min, tmv->tm_sec,
                    g->state.display_names[g->state.current_player]);
                if (game_save(g, fname)) printf("Game saved to %s\n", fname);
                else printf("Failed to save game.\n");
            }
            else if (cmd[0] == 'q' || cmd[0] == 'Q') {
                if (yes_no("Quit without saving?")) { printf("Quitting game.\n"); break; }
            }
            else { printf("Unknown command. Try again.\n"); }
        }
    }
}
