#include "string.h"

size_t strlen(const char* str) 
{
  size_t len = 0;

  while (str[len++]);

  return len;
}

size_t strnlen(const char* str, int max) 
{
  size_t len = 0;

  while (str[len++] && max--);

  return len;
}


bool is_digit(char c)
{
  return c >= 48 && c <= 57;
}

int to_numeric_digit(char c)
{
  return c - 48;
}

char* strcpy(char* dest, const char* src)
{
  char* res = dest;

  while(*src)
  {
    *dest = *src;
    src++;
    dest++;
  }

  *dest = 0x00;

  return res;
}
