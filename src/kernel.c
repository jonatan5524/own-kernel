#include "kernel.h"
#include "./fileSystem/file.h"
#include "config.h"
#include "disk/disk.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "io/io.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"
#include "memory/heap/kernelHeap.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "string/string.h"
#include "task/process.h"
#include "task/task.h"
#include "task/tss.h"
#include <stddef.h>
#include <stdint.h>

uint16_t *video_memory = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color) { return (color << 8) | c; }

void terminal_putchar(int x, int y, char c, char color) {
  video_memory[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_backspace() {
  if (terminal_row == 0 && terminal_col == 0) {
    return;
  }

  if (terminal_col == 0) {
    terminal_row -= 1;
    terminal_col = VGA_WIDTH;
  }

  terminal_col -= 1;
  terminal_writechar(' ', 15);
  terminal_col -= 1;
}

void terminal_writechar(char c, char color) {
  if (c == '\n') {
    terminal_row++;
    terminal_col = 0;

    return;
  }

  // backspace ascii
  if (c == 0x08) {
    terminal_backspace();
    return;
  }

  terminal_putchar(terminal_col, terminal_row, c, color);
  terminal_col++;

  if (terminal_col == VGA_WIDTH) {
    terminal_col = 0;
    terminal_row++;
  }
}

void terminal_initalize() {
  video_memory = (uint16_t *)(0xB8000);
  terminal_row = 0;
  terminal_col = 0;

  for (int row = 0; row < VGA_HEIGHT; row++) {
    for (int col = 0; col < VGA_WIDTH; col++) {
      terminal_putchar(col, row, ' ', 0);
    }
  }
}

void print(const char *str) {
  size_t len = strlen(str);

  for (int index = 0; index < len; index++) {
    terminal_writechar(str[index], 15);
  }
}

static struct paging_4gb_chunk *kernel_chunk = 0;

void panic(const char *msg) {
  print(msg);

  while (1) {
  }
}

void kernel_page() {
  kernel_registers();
  paging_switch(kernel_chunk);
}

struct tss tss;
struct gdt gdt_real[TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[TOTAL_GDT_SEGMENTS] = {
    // NULL Segment
    {.base = 0x00, .limit = 0x00, .type = 0x00},
    // Kernel Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},
    // Kernel Data Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},
    // User Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF8},
    // User Data Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF2},
    // TSS Segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9}};

void kernel_main() {
  terminal_initalize();

  memset(gdt_real, 0x00, sizeof(gdt_real));
  gdt_structured_to_gdt(gdt_real, gdt_structured, TOTAL_GDT_SEGMENTS);
  gdt_load(gdt_real, sizeof(gdt_real));

  // initialize the heap
  kernel_heap_init();

  // Initizalize fle systems
  fs_init();

  // search and initialize the disks
  disk_search_and_init();

  // initialize the interrupt descriptor table
  idt_init();

  // Setup TSS
  memset(&tss, 0x00, sizeof(tss));
  tss.esp0 = 0x600000;
  tss.ss0 = KERNEL_DATA_SELECTOR;

  // Load TSS
  tss_load(0x28);

  // setup paging
  kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT |
                                PAGING_ACCESS_FROM_ALL);

  // switch to kernel paging chunk
  paging_switch(kernel_chunk);

  // enable paging
  enable_paging();

  // registers the kernel commands
  isr80h_register_commands();

  // Initialize all the system keyboards
  keyboard_init();

  struct process *process = 0;
  int res = process_load_switch("0:/shell.elf", &process);

  if (res != ALL_OK) {
    panic("failed to load blank.elf");
  }

  task_run_first_ever_task();

  while (1) {
  }
}
