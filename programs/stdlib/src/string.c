#include "string.h"

size_t strlen(const char *str) {
  size_t len = 0;

  while (str[len++])
    ;

  return len;
}

size_t strnlen(const char *str, int max) {
  size_t len = 0;

  while (str[len++] && max--)
    ;

  return len;
}

bool is_digit(char c) { return c >= 48 && c <= 57; }

int to_numeric_digit(char c) { return c - 48; }

char *strcpy(char *dest, const char *src) {
  char *res = dest;

  while (*src) {
    *dest = *src;
    src++;
    dest++;
  }

  *dest = 0x00;

  return res;
}

char *strncpy(char *dest, const char *src, int count) {
  int i = 0;

  for (i = 0; i < count - 1; i++) {
    if (src[i] == 0x00) {
      break;
    }

    dest[i] = src[i];
  }

  dest[i] = 0x00;

  return dest;
}

int strnlen_terminator(const char *str, int max, char terminator) {
  int i = 0;

  for (i = 0; i < max; i++) {
    if (str[i] == 0 || str[i] == terminator) {
      break;
    }
  }

  return i;
}

char tolower(char s1) {
  if (s1 >= 64 && s1 <= 90) {
    s1 += 32;
  }

  return s1;
}

int istrncmp(const char *str1, const char *str2, int n) {
  unsigned char u1, u2;

  while (n--) {
    u1 = (unsigned char)*str1++;
    u2 = (unsigned char)*str2++;

    if (u1 != u2 && tolower(u1) != tolower(u2)) {
      return u1 - u2;
    }

    if (u1 == 0) {
      return 0;
    }
  }

  return 0;
}

int strncmp(const char *str1, const char *str2, int n) {
  unsigned char u1, u2;

  while (n--) {
    u1 = (unsigned char)*str1++;
    u2 = (unsigned char)*str2++;

    if (u1 != u2) {
      return u1 - u2;
    }

    if (u1 == 0) {
      return 0;
    }
  }

  return 0;
}

unsigned int is_delim(char c, const char *delim) {
  while (*delim != '\0') {
    if (c == *delim)
      return 1;
    delim++;
  }
  return 0;
}

char *strtok(char *srcString, const char *delim) {
  static char *backup_string; // start of the next search

  if (!srcString) {
    srcString = backup_string;
  }
  if (!srcString) {
    // user is bad user
    return NULL;
  }
  // handle beginning of the string containing delims
  while (1) {
    if (is_delim(*srcString, delim)) {
      srcString++;
      continue;
    }
    if (*srcString == '\0') {
      // we've reached the end of the string
      return NULL;
    }
    break;
  }
  char *ret = srcString;
  while (1) {
    if (*srcString == '\0') {
      /*end of the input string and
      next exec will return NULL*/
      backup_string = srcString;
      return ret;
    }
    if (is_delim(*srcString, delim)) {
      *srcString = '\0';
      backup_string = srcString + 1;
      return ret;
    }
    srcString++;
  }
}