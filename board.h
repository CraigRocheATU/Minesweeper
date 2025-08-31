#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

//The Rows and collumns of the board and the number of mines
#define ROWS 8
#define COLS 8
#define MINES 10

//Board Sturcture
typedef struct {
    bool mines[ROWS][COLS];
    int  numbers[ROWS][COLS];
    bool revealed[ROWS][COLS];
} Board;

//Declaration of functions
void board_init(Board* b);
void board_place_mines(Board* b);
void board_compute_numbers(Board* b);
int  count_adjacent_mines(const Board* b, int r, int c);
int  board_total_safe_cells(void);
bool board_is_cleared(const Board* b);
int  board_reveal(Board* b, int r, int c, int* sum_nonzero_out, int* count_revealed_out);
void board_print(const Board* b, bool reveal_all);

#endif // BOARD_H
