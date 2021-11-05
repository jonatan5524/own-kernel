#include "kernelHeap.h"
#include "heap.h"
#include "../../config.h"
#include "../../kernel.h"
#include "../memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kernel_heap_init()
{
  int total_table_entries = HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;
  
  kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) (HEAP_TABLE_ADDRESS);
  kernel_heap_table.total = total_table_entries;

  void* end = (void*) (HEAP_ADDRESS + HEAP_SIZE_BYTES);

  int res = heap_create(&kernel_heap, (void*) HEAP_ADDRESS, end, &kernel_heap_table);
 
  if (res < 0)
  {
    print("Failed to create heap\n");
  }
}

void* kernel_malloc(size_t size)
{
  return heap_malloc(&kernel_heap, size);
}

void* kernel_zalloc(size_t size)
{
  void* ptr = kernel_malloc(size);
  
  if (!ptr)
  {
    return ptr;
  }

  memset(ptr, 0x00, size);

  return ptr;
}

void kernel_free(void* ptr)
{
  heap_free(&kernel_heap, ptr);
}
