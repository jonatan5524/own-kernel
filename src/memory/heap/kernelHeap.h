#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

void kernel_heap_init();
void kernel_free(void* ptr);
void* kernel_malloc(size_t size);
void* kernel_zalloc(size_t size);

#endif
