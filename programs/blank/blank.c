#include "os.h"
#include "stdlib.h"

int main(int argc, char **argv) {
  print("Hello how are you?\n");

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
