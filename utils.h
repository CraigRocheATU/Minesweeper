#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

//Declaration of functions
int read_int_in_range(const char* prompt, int min, int max);
void read_string(const char* prompt, char* buf, int buflen);
bool yes_no(const char* prompt);
void clear_input_buffer(void);
void trim_newline(char* s);

#endif // UTILS_H
