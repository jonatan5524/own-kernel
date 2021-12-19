#include "os.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char **argv) {
  printf("My age is: %i\n", 21);

  print("Hello how are you?\n");

  putchar('a');

  print(itoa(8762));

  void *ptr = malloc(512);

  if (ptr) {
    free(ptr);
  }

  while (1) {
    if (getkey() != 0) {
      print("key was pressed!\n");
    }
  }
  return 0;
}
