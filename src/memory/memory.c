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

int memcmp(void* s1, void* s2, size_t size)
{
  char* c1 = s1;
  char* c2 = s2;

  while(size--)
  {
    if(*c1++ != *c2++)
    {
      return c1[-1] < c2[-1] ? -1 : 1;
    }
  }

  return 0;
}
