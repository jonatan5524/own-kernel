#include "io.h"
#include "../task/task.h"
#include "../kernel.h"

void* isr80h_command1_print(struct interrupt_frame* frame)
{
  void* user_space_massage_buffer = task_get_stack_item(task_current(), 0);
  char buffer[1024];

  copy_string_from_task(task_current(), user_space_massage_buffer, buffer, sizeof(buffer));
  
  print(buffer);

  return 0;
}
