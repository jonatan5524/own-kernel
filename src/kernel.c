#include "kernel.h"
#include "idt/idt.h"
#include <stddef.h>
#include <stdint.h>
#include "./fileSystem/file.h"
#include "io/io.h"
#include "memory/paging/paging.h"
#include "memory/heap/kernelHeap.h"
#include "string/string.h"
#include "disk/disk.h"

uint16_t* video_memory = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color)
{
  return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
  video_memory[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color)
{
  if (c == '\n')
  {
    terminal_row++;
    terminal_col = 0;

    return;
  }

  terminal_putchar(terminal_col, terminal_row, c, color);
  terminal_col++;

  if (terminal_col == VGA_WIDTH) 
  {
    terminal_col = 0;
    terminal_row++;
  }
}

void terminal_initalize()
{
  video_memory = (uint16_t*)(0xB8000);
  terminal_row = 0;
  terminal_col = 0;

  for(int row = 0; row < VGA_HEIGHT; row++) 
  {
    for(int col = 0; col < VGA_WIDTH; col++)
    {
        terminal_putchar(col, row, ' ', 0);
    }      
  }
}

void print(const char* str)
{
  size_t len = strlen(str);

  for(int index = 0; index < len; index++)
  {
    terminal_writechar(str[index], 15);
  }
}

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main()
{
  terminal_initalize();
  print("Hello World!\nstrs");

  // initialize the heap 
  kernel_heap_init();

  // Initizalize fle systems
  fs_init();

  // search and initialize the disks
  disk_search_and_init();

  // initialize the interrupt descriptor table
  idt_init();

  // setup paging 
  kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  // switch to kernel paging chunk
  paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

  // enable paging
  enable_paging();

  // enable interrupts
  enable_interrupts();

  int fd = fopen("0:/hello.txt", "r");

  if (fd)
  {
    print("we opened hello.txt\n");
  }

  while(1) {}
}
