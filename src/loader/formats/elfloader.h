#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "../../config.h"

struct elf_file
{
  char filename[OS_MAX_PATH];

  int in_memory_size;

  // The physical address that this elf file is loaded at
  void* elf_memory;

  // The virtual base address of this binary
  void* virtual_base_address;

  // The ending virtual address
  void* virtual_end_address;

  // The physical base address
  void* physical_base_address;

  // The physical end address
  void* physical_end_address;
};

int elf_load(const char* filename, struct elf_file** file_out);
void elf_close(struct elf_file* file);

#endif 
