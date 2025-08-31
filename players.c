#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "players.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Creates a player node from linked list
static Player* node_new(const char* u, const char* p, const char* d) {
    Player* n = (Player*)malloc(sizeof(Player));
    if (!n) return NULL;
    strncpy(n->username, u, sizeof(n->username) - 1);
    n->username[sizeof(n->username) - 1] = '\0';
    strncpy(n->password, p, sizeof(n->password) - 1);
    n->password[sizeof(n->password) - 1] = '\0';
    strncpy(n->display_name, d, sizeof(n->display_name) - 1);
    n->display_name[sizeof(n->display_name) - 1] = '\0';
    n->wins = 0;
    n->losses = 0;
    n->next = NULL;
    return n;
}

//Loads players from the login.txt
Player* players_load_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Warning: could not open %s; no users loaded.\n", filename);
        return NULL;
    }
    Player* head = NULL, * tail = NULL;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char* u = strtok(line, ",");
        char* p = u ? strtok(NULL, ",") : NULL;
        char* d = p ? strtok(NULL, ",") : NULL;
        if (!u || !p || !d) continue;
        size_t nd = strlen(d);
        if (nd && (d[nd - 1] == '\n' || d[nd - 1] == '\r')) d[--nd] = '\0';
        if (nd && d[nd - 1] == '\r') d[--nd] = '\0';
        Player* n = node_new(u, p, d);
        if (!n) break;
        if (!head) head = tail = n;
        else { 
            tail->next = n; tail = n; 
        }
    }
    fclose(f);
    return head;
}

//Frees the linked lilst
void players_free(Player* head) {
    while (head) { Player* n = head->next;
    free(head); head = n;
    }
}

//Finds specific player using username
Player* players_find(Player* head, const char* username) {
    for (Player* p = head;
        p; p = p->next) if (strcmp(p->username, username) == 0) return p;
    return NULL;
}

//Used to validate player logins ensuring username & password match
bool players_validate(Player* head, const char* username, const char* password, char* out_display_name, int out_len) {
    Player* p = players_find(head, username);
    if (!p) return false;
    if (strcmp(p->password, password) != 0) return false;
    if (out_display_name && out_len > 0) { strncpy(out_display_name, p->display_name, out_len - 1); out_display_name[out_len - 1] = '\0'; }
    return true;
}
