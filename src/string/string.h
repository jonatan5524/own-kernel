#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

size_t strlen(const char* str);
size_t strnlen(const char* str, int max);
int to_numeric_digit(char c);
bool is_digit(char c);
char* strcpy(char* dest, const char* src);
char tolower(char s1);
int istrncmp(const char* str1, const char* str2, int n);
int strncmp(const char* str1, const char* str2, int n);
int strnlen_terminator(const char* str, int max, char terminator);
char* strncpy(char* dest, const char* src, int count);

#endif 
