#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED   0b00010000
#define PAGING_WRITE_THROUGH    0b00001000
#define PAGING_ACCESS_FROM_ALL  0b00000100
#define PAGING_IS_WRITEABLE     0b00000010
#define PAGING_IS_PRESENT       0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

struct paging_4gb_chunk
{
  uint32_t* directory_entry;
};

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void paging_switch(struct paging_4gb_chunk* directory);
void enable_paging();

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);
void paging_free_4gb(struct paging_4gb_chunk* chunk);

int paging_set_physical_address(uint32_t* directory, void* virtual_address, uint32_t val);
bool paging_is_aligned(void* address);

int paging_map_to(struct paging_4gb_chunk* directory, void* virtual_address, void* physical_address, void* physical_end_address, int flags);
int paging_map_range(struct paging_4gb_chunk* directory, void* virtual_address, void* physical_address, int total_pages, int flags);
int paging_map(struct paging_4gb_chunk* directory, void* virtual_address, void* physical_address, int flags);
void* paging_align_address(void* ptr);
uint32_t paging_get(uint32_t* directory, void* virtual_address);

#endif
