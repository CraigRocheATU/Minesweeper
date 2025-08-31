#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#define ROWS 8
#define COLS 8
#define MINES 10

typedef struct {
	bool mines[ROWS][COLS];
	int numbers[ROWS][COLS];
	bool revealed[ROWS][COLS];
} Board;

void board_init(Board* b);
void board_place_mines(Board* b);
void board_compute_mines(Board* b);
void board_reveal_all(Board* b);
int board_reveal(Board* b, int r, int c, int* newly_revealed_nonzero_sum, int* newly_revelaed_count);
int count_adjacent_mines(const Board* b, int r, int c);
bool board_is_cleared(const Board* b);
int board_safe_cells(void);
static void safe_cell_reveal(Board* b, int r, int c, int* zero_nonzero, int* count_revealed);
void board_print(const Board* b, bool reveal_all);

#endif // BOARD_H
