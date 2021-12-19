#include "os.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char **argv) {
  char words[] = "Hello how are you";

  const char *token = strtok(words, " ");

  while (token) {
    printf("%s\n", token);
    token = strtok(NULL, " ");
  }

  while (1) {
  }

  return 0;
}
