#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

size_t strlen(const char* str);
size_t strnlen(const char* str, int max);
int to_numeric_digit(char c);
bool is_digit(char c);

#endif 
