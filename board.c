#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Direction Arrays
static int dr[8] = { -1,-1,-1, 0, 0, 1, 1, 1 };
static int dc[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };

//Verifies coordinates are within bounds
static int in_bounds(int r, int c) {
    return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

//Declaration of flood_reveal function (explained later)
static void flood_reveal(Board* b, int r, int c, int* sum_nonzero_out, int* count_revealed_out);

//Places mines randomly on board
void board_place_mines(Board* b) {
    int placed = 0;
    while (placed < MINES) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        //ensures mines are not placed in same position
        if (!b->mines[r][c]) {
            b->mines[r][c] = true;
            placed++; 
        }
    }
}

//Counts mines adjacent to cell
int count_adjacent_mines(const Board* b, int r, int c) {
    int cnt = 0;
    for (int k = 0; k < 8; ++k) {
        int nr = r + dr[k], nc = c + dc[k];
        if (in_bounds(nr, nc) && b->mines[nr][nc]) cnt++;
    }
    return cnt;
}

//Calculates numbers array based on count_adjacent_mines function
void board_compute_numbers(Board* b) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (b->mines[r][c])
                //marks mines as -1
                b->numbers[r][c] = -1;
            else
                b->numbers[r][c] = count_adjacent_mines(b, r, c);
        }
    }
}

//Initialises board
void board_init(Board* b) {
    for (int r = 0; r < ROWS; ++r) for (int c = 0; c < COLS; ++c) {
        b->mines[r][c] = false; b->numbers[r][c] = 0; b->revealed[r][c] = false;
    }
    board_place_mines(b);
    board_compute_numbers(b);
}

//Flood fill type reveal for when cells adjacent mine count is 0
static void flood_reveal(Board* b, int r, int c, int* sum_nonzero_out, int* count_revealed_out) {
    if (!in_bounds(r, c)) return;
    if (b->revealed[r][c] || b->mines[r][c]) return;
    b->revealed[r][c] = true;
    (*count_revealed_out)++;
    int n = b->numbers[r][c];
    if (n > 0) { *sum_nonzero_out += n; return; }
    for (int k = 0; k < 8; ++k) flood_reveal(b, r + dr[k], c + dc[k], sum_nonzero_out, count_revealed_out);
}

//Used when players reveal a cell containing a mine or is safe
int board_reveal(Board* b, int r, int c, int* sum_nonzero_out, int* count_revealed_out) {
    *sum_nonzero_out = 0; *count_revealed_out = 0;
    if (!in_bounds(r, c)) return -2;
    if (b->revealed[r][c]) return -3;
    if (b->mines[r][c]) { b->revealed[r][c] = true; return -1; }
    if (b->numbers[r][c] > 0) {
        b->revealed[r][c] = true;
        *sum_nonzero_out = b->numbers[r][c];
        *count_revealed_out = 1;
        return b->numbers[r][c];
    }
    //when a 0 cell is revealed calls on flood_reveal function
    flood_reveal(b, r, c, sum_nonzero_out, count_revealed_out);
    return 0;
}

//Checks that all cells not containing mines are revealed
bool board_is_cleared(const Board* b) {
    int safe = 0;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (!b->mines[r][c] && b->revealed[r][c]) safe++;
    return safe == board_total_safe_cells();
}

//Calculates the number of safe cells (cells not containing mines) on the board
int board_total_safe_cells(void) { 
    return ROWS * COLS - MINES; 
}

//Prints the board to the console
void board_print(const Board* b, bool reveal_all) {
    printf("    ");
    for (int c = 0; c < COLS; ++c) printf("%d ", c + 1);
    printf("\n   +-");
    for (int c = 0; c < COLS; ++c) printf("--");
    printf("+\n");
    for (int r = 0; r < ROWS; ++r) {
        printf("%2d | ", r + 1);
        for (int c = 0; c < COLS; ++c) {
            bool shown = b->revealed[r][c] || reveal_all;
            if (!shown) printf("? ");
            else if (b->mines[r][c]) printf("* ");
            else {
                int n = b->numbers[r][c];
                if (n == 0) printf("  ");
                else printf("%d ", n);
            }
        }
        printf("|\n");
    }
    printf("   +-");
    for (int c = 0; c < COLS; ++c) printf("--");
    printf("+\n");
}
