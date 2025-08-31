#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//Used to clear the input buffer
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

//Trims and trailing newlines
void trim_newline(char* s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[n - 1] = '\0';
    n = strlen(s);
    if (n > 0 && s[n - 1] == '\r') s[n - 1] = '\0';
}

//Prompts the user for an integer input within a specified range
int read_int_in_range(const char* prompt, int min, int max) {
    int value;
    char line[128];
    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) { clearerr(stdin); continue; }
        char extra;
        if (sscanf(line, "%d %c", &value, &extra) == 1) {
            if (value >= min && value <= max) return value;
        }
        printf("Invalid input. Please enter a number between %d and %d.\n", min, max);
    }
}

//Reads a string
void read_string(const char* prompt, char* buf, int buflen) {
    for (;;) {
        printf("%s", prompt);
        if (!fgets(buf, buflen, stdin)) { clearerr(stdin); continue; }
        trim_newline(buf);
        if ((int)strlen(buf) > 0) return;
        printf("Input cannot be empty. Try again.\n");
    }
}

//Used for y/n functionality
bool yes_no(const char* prompt) {
    char line[16];
    for (;;) {
        printf("%s (y/n): ", prompt);
        if (!fgets(line, sizeof(line), stdin)) { clearerr(stdin); continue; }
        if (line[0] == 'y' || line[0] == 'Y') return true;
        if (line[0] == 'n' || line[0] == 'N') return false;
        printf("Please answer y or n.\n");
    }
}
