#include "memory.h"

void* memset(void* ptr, int c, size_t size)
{
  char* c_ptr = (char*) ptr;

  for (int index = 0; index < size; index++)
  {
    c_ptr[index] = (char) c;
  }

  return ptr;
}
