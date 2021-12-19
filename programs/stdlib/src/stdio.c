#include "stdio.h"
#include "os.h"

int putchar(int c) {
  os_putchar((char)c);

  return 0;
}