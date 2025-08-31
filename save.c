#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "save.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define MKDIR(p) _mkdir(p)
#else
#include <dirent.h>
#include <unistd.h>
#define MKDIR(p) mkdir(p, 0777)
#endif

//Ensures save directory exists
bool ensure_saves_dir(void) {
    struct stat st;
#ifdef _WIN32
    if (_stat("saves", &st) == 0 && (st.st_mode & _S_IFDIR)) return true;
#else
    if (stat("saves", &st) == 0 && S_ISDIR(st.st_mode)) return true;
#endif
    return MKDIR("saves") == 0;
}

//Lists all the save files in the save directory
bool list_save_files(char files[][256], int max_files, int* out_count) {
    *out_count = 0;
#ifdef _WIN32
    intptr_t h;
    struct _finddata_t fd;
    h = _findfirst("saves\\*.msave", &fd);
    if (h == -1L) return true; // no files
    do {
        if (*out_count < max_files) { snprintf(files[*out_count], 256, "saves/%s", fd.name); (*out_count)++; }
    } while (_findnext(h, &fd) == 0);
    _findclose(h);
    return true;
#else
    DIR* d = opendir("saves");
    if (!d) return false;
    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        const char* name = ent->d_name;
        if (name[0] == '.') continue;
        const char* ext = strrchr(name, '.');
        if (!ext || strcmp(ext, ".msave") != 0) continue;
        if (*out_count < max_files) { snprintf(files[*out_count], 256, "saves/%s", name); (*out_count)++; }
    }
    closedir(d);
    return true;
#endif
}


//The following are used for writing and reading matrices to a file, specifically 2D grid arrys used for the board
static void write_matrix_bool(FILE* f, bool m[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) fprintf(f, "%d ", m[r][c] ? 1 : 0);
        fprintf(f, "\n");
    }
}
static void write_matrix_int(FILE* f, int m[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) fprintf(f, "%d ", m[r][c]);
        fprintf(f, "\n");
    }
}
static void read_matrix_bool(FILE* f, bool m[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) {
            int x;
            fscanf(f, "%d", &x);
            m[r][c] = x ? true : false; 
        }
}
static void read_matrix_int(FILE* f, int m[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) fscanf(f, "%d", &m[r][c]);
}

//Saves the current game state to a file
bool save_game_to_file(const char* filepath, const GameState* gs) {
    FILE* f = fopen(filepath, "w");
    if (!f) return false;
    fprintf(f, "MSAVEv1\n");
    time_t now = time(NULL);
    fprintf(f, "timestamp %lld\n", (long long)now);
    fprintf(f, "players %d\n", gs->num_players);
    fprintf(f, "user0 %s\n", gs->usernames[0]);
    fprintf(f, "name0 %s\n", gs->display_names[0]);
    fprintf(f, "score0 %d\n", gs->scores[0]);
    fprintf(f, "user1 %s\n", gs->usernames[1]);
    fprintf(f, "name1 %s\n", gs->display_names[1]);
    fprintf(f, "score1 %d\n", gs->scores[1]);
    fprintf(f, "current %d\n", gs->current_player);
    fprintf(f, "gameover %d\n", gs->game_over ? 1 : 0);
    fprintf(f, "winner %d\n", gs->winner);

    fprintf(f, "mines\n");   write_matrix_bool(f, gs->board.mines);
    fprintf(f, "numbers\n"); write_matrix_int(f, gs->board.numbers);
    fprintf(f, "revealed\n"); write_matrix_bool(f, gs->board.revealed);

    fclose(f);
    return true;
}

//Loads an existing game state from a file
bool load_game_from_file(const char* filepath, GameState* gs) {
    FILE* f = fopen(filepath, "r");
    if (!f) return false;
    char header[32];
    if (fscanf(f, "%31s", header) != 1 || strcmp(header, "MSAVEv1") != 0) { fclose(f); return false; }

    char key[32];
    long long tstamp; int tmp;
    fscanf(f, "%31s %lld", key, &tstamp);
    fscanf(f, "%31s %d", key, &gs->num_players);
    fscanf(f, "%31s %63s", key, gs->usernames[0]);
    fscanf(f, "%31s %63s", key, gs->display_names[0]);
    fscanf(f, "%31s %d", key, &gs->scores[0]);
    fscanf(f, "%31s %63s", key, gs->usernames[1]);
    fscanf(f, "%31s %63s", key, gs->display_names[1]);
    fscanf(f, "%31s %d", key, &gs->scores[1]);
    fscanf(f, "%31s %d", key, &gs->current_player);
    fscanf(f, "%31s %d", key, &tmp); gs->game_over = tmp ? true : false;
    fscanf(f, "%31s %d", key, &gs->winner);

    fscanf(f, "%31s", key);
    read_matrix_bool(f, gs->board.mines);
    fscanf(f, "%31s", key);
    read_matrix_int(f, gs->board.numbers);
    fscanf(f, "%31s", key);
    read_matrix_bool(f, gs->board.revealed);

    fclose(f);
    return true;
}

//Prints a summary of the saved game to the console
void print_save_summary(const GameState* gs) {
    printf("Players: %d | ", gs->num_players);
    printf("%s(%d) vs %s(%d) | ",
        gs->display_names[0], gs->scores[0],
        gs->display_names[1], gs->scores[1]);
    printf("Turn: %s | ",
        gs->current_player == 0 ? gs->display_names[0] : gs->display_names[1]);
    printf("GameOver: %s\n", gs->game_over ? "yes" : "no");
}