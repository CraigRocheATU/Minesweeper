#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "app.h"
#include "utils.h"
#include "save.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

//Title
void show_title(void) {
    printf("\n========================\n");
    printf("     MIND SWEEPER\n");
    printf("========================\n");
}

//Main Menu
void show_main_menu(void) {
    printf("\nMain Menu:\n");
    printf(" 1) New Game\n");
    printf(" 2) Load Saved Game\n");
    printf(" 3) Quit\n");
}

//Handles player logins
//Validates player login against linked list of players
int login_players(Player* db, int* out_num_players, char user[2][64], char disp[2][64]) {
    printf("\nLogin:\n");
    int n = read_int_in_range("Number of players (1 or 2): ", 1, 2);
    *out_num_players = n;
    for (int i = 0; i < n; ++i) {
        char u[64], p[64], d[64];
        for (;;) {
            read_string(i == 0 ? "Player 1 username: " : "Player 2 username: ", u, sizeof(u));
            read_string("Password: ", p, sizeof(p));
            if (players_validate(db, u, p, d, sizeof(d))) {
                printf("Welcome, %s!\n", d);
                strncpy(user[i], u, 63); user[i][63] = '\0';
                strncpy(disp[i], d, 63); disp[i][63] = '\0';
                break;
            }
            else {
                printf("Invalid credentials. Try again.\n");
                if (!yes_no("Try again?")) return 0;
            }
        }
    }
    //When playing single player, fills in placeholders for a second player
    if (n == 1) { strcpy(user[1], "-"); strcpy(disp[1], "-"); }
    return 1;
}

//Functionality for loading existing saves
void load_game_menu(Game* g) {
    static char paths[128][256];
    int count = 0;
    //Gets llist of existing saves
    if (!list_save_files(paths, 128, &count) || count == 0) { 
        printf("No save files found.\n"); 
        return; 
    }
    printf("\nSaved Games:\n");
    for (int i = 0; i < count; ++i) {
        GameState tmp;
        if (load_game_from_file(paths[i], &tmp)) {
            printf(" %2d) %s | ", i + 1, paths[i]); print_save_summary(&tmp);
        }
        else {
            printf(" %2d) %s (unreadable)\n", i + 1, paths[i]);
        }
    }
    //Prompts the user to select an existing save file
    int choice = read_int_in_range("Select a save to load (number): ", 1, count);
    if (game_load(g, paths[choice - 1])) {
        printf("Loaded: %s\n", paths[choice - 1]);
        game_print_board(g, false);
        if (!yes_no("Start playing this loaded game now?")) return;
        game_loop(g);
    }
    else {
        printf("Failed to load that save.\n");
    }
}

int app_main(void) {
    // ensure sample login file exists, if it doesn't creates one with sample users
    FILE* lf = fopen("login.txt", "r");
    if (!lf) {
        lf = fopen("login.txt", "w");
        if (lf) {
            fprintf(lf, "shane,shane123,Shane\nrick,rick123,Rick\ncarl,carl123,Carl\n");
            fclose(lf);
        }
    }
    else fclose(lf);
    //loads players from login.txt
    Player* db = players_load_from_file("login.txt");
    show_title();
    //Main menu loop
    for (;;) {
        show_main_menu();
        int choice = read_int_in_range("Enter choice (1-3): ", 1, 3);
        //New Game
        if (choice == 1) {
            char user[2][64], disp[2][64]; int np = 0;
            if (!db) {
                printf("Warning: No login database loaded. Guest mode.\n");
                strcpy(user[0], "guest1"); strcpy(disp[0], "Guest 1");
                strcpy(user[1], "guest2"); strcpy(disp[1], "Guest 2");
                np = read_int_in_range("Number of players (1 or 2): ", 1, 2);
                if (np == 1) strcpy(disp[1], "-");
            }
            else {
                if (!login_players(db, &np, user, disp)) { 
                    printf("Login canceled.\n"); 
                    continue; 
                }
            }
            Game g; game_new(&g, np, user[0], disp[0], user[1], disp[1]); game_loop(&g);
        }
        //Loads existing save file
        else if (choice == 2) {
            Game g; load_game_menu(&g);
        }
        //Exit program
        else if (choice == 3) {
            printf("Goodbye!\n"); break;
        }
    }
    players_free(db);
    return 0;
}
