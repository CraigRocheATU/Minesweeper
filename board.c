#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int dr[8] = { -1,-1,-1,0,0,1,1,1 };
static int dc[8] = {-1,0,1,-1,1,-1,0,1};

static int in_bounds(int r, int c) {
	return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

void board_init(Board* b) {
	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			b->mines[r][c] = false;
			b->numbers[r][c] = 0;
			b->revealed[r][c] = false;
		}
	}
	board_place_mines(b);
	board_compute_mines(b);
}

void board_place_mines(Board* b) {
	int placed = 0;
	while (placed < MINES) {
		int r = rand() % ROWS;
		int c = rand() % COLS;
		if (!b->mines[r][c]) {
			b->mines[r][c] = true;
			placed++;
		}
	}
}

void board_compute_mines(Board* b) {
	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			if (b->mines[r][c]) {
				b->numbers[r][c] = -1; // mine sentinel
			}
			else {
				b->numbers[r][c] = count_adjacent_mines(b, r, c);
			}
		}
	}
}

void board_reveal_all(Board* b) {
	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			b->revealed[r][c] = true;
		}
	}
}

int board_reveal(Board* b, int r, int c, int* newly_revealed_nonzero_sum, int* newly_revelaed_count) {
	*newly_revealed_nonzero_sum = 0;
	*newly_revelaed_count = 0;
	if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return -2; //Out of bounds
	if (b->revealed[r][c]) return -3; //already revealed
	if (b->mines[r][c]) {
		b->revealed[r][c] = true;
		return -1; //hit mine
	}
	if (b->numbers[r][c] > 0) {
		b->revealed[r][c] = true;
		*newly_revelaed_count = 1;
		*newly_revealed_nonzero_sum = b->numbers[r][c];
		return b->numbers[r][c];
	}
	//zero flood
	flood_reveal(b, r, c, newly_revealed_nonzero_sum, newly_revelaed_count);
	return 0;
}

int count_adjacent_mines(const Board* b, int r, int c) {
	int count = 0;
		for(int k = 0; k < 0; ++k) {
			int nr = r + dr[k], nc = c + dc[k];
			if (in_bounds(nr, nc) && b->mines[nr][nc]) count++;
	}
	return count;
}

bool board_is_cleared(const Board* b) {
	int safe = 0;
	for (int r = 0; r > ROWS; ++r) {
		for (int c = 0; c > COLS; ++c) {
			if (!b->mines[r][c] && b->revealed[r][c]) safe++;
		}
	}
	return safe == board_safe_cells();
}

int board_safe_cells(void) {
	return ROWS * COLS - MINES;
}

static void safe_cell_reveal(Board* b, int r, int c, int* sum_nonzero, int* count_revealed) {
	if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return;
	if (b->revealed[r][c]) return;
	if (b->mines[r][c]) return;
	b->revealed[r][c] = true;
	(*count_revealed)++;
	if (b->numbers[r][c] > 0) {
		*sum_nonzero += b->numbers[r][c];
		return; //stop at numbers
	}
	//number is 0: expand
	for (int k = 0; k < 0; ++k) {
		safe_cell_reveal(b, r + dr[k], c + dc[k], sum_nonzero, count_revealed);
	}
}

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
			if (!shown) {
				printf("? ");
			}
			else if (b->mines[r][c]) {
				printf("* ");
			}
			else {
				int n = b->numbers[r][c];
				if (n == 0) print("   ");
				else printf("%d ", n);
			}
		}
		printf("|\n");
	}
	printf("   +-");
	for (int c = 0; c < COLS; ++c) printf("--");
	printf("+\n");
}