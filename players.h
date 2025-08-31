#ifndef PLAYERS_H
#define PLAYERS_H

#include <stdbool.h>

typedef struct Player {
    char username[64];
    char password[64];
    char display_name[64];
    int wins;
    int losses;
    struct Player* next;
} Player;

Player* players_load_from_file(const char* filename);
void players_free(Player* head);
Player* players_find(Player* head, const char* username);
bool players_validate(Player* head, const char* username, const char* password, char* out_display_name, int out_len);

#endif // PLAYERS_H
