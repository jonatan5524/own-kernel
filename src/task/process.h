#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "../config.h"

struct process
{
  // The process id 
  uint16_t id;

  char filename[OS_MAX_PATH];

  // The main process task
  struct task* task;

  // The memory (malloc) allocations of the process
  void* allocations[OS_MAX_PROGRAM_ALLOCARTIONS];

  // The physical pointer to the process memory
  void* ptr;
 
  // The physical pointer to the stack memory
  void* stack;

  // The size of the data pointed to by ptr 
  uint32_t size;

  struct keyboard_buffer
  {
    char buffer[OS_KEYBOARD_BUFFER_SIZE];
    int tail;
    int head;
  } keyboard;
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load_switch(const char* filename, struct process** process);
int process_switch(struct process* process);
int process_load(const char* filename, struct process** process);
struct process* process_current();
struct process* process_get(int process_id);

#endif 
