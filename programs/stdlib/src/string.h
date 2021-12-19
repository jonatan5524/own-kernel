#ifndef OS_STRING_H
#define OS_STRINGH

#include <stdbool.h>
#include <stddef.h>

size_t strlen(const char *str);
size_t strnlen(const char *str, int max);
int to_numeric_digit(char c);
bool is_digit(char c);
char *strcpy(char *dest, const char *src);
char tolower(char s1);
int istrncmp(const char *str1, const char *str2, int n);
int strncmp(const char *str1, const char *str2, int n);
int strnlen_terminator(const char *str, int max, char terminator);
char *strncpy(char *dest, const char *src, int count);
char *strtok(char *str, const char *delim);

#endif