#include "idt.h"
#include "../status.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../kernel.h"
#include "../task/task.h"
#include "../io/io.h"
#include <stdint.h>

struct idt_desc idt_descriptors[TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;
 
extern void* interrupt_pointer_table[TOTAL_INTERRUPTS]; 

static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[TOTAL_INTERRUPTS];
static ISR80H_COMMAND isr80h_commands[OS_MAX_ISR80H_COMMANDS];

extern void idt_load(struct idtr_desc* ptr);
extern void no_interrupt();
extern void isr80h_wrapper();

void interrupt_handler(int interrupt, struct interrupt_frame* frame)
{
  kernel_page();

  if (interrupt_callbacks[interrupt] != 0)
  {
    task_current_save_state(frame);
    interrupt_callbacks[interrupt](frame);
  }

  task_page();
  outb(0x20, 0x20);
}

void idt_set(int interrupt_number, void* address)
{
  struct idt_desc* descriptor = &idt_descriptors[interrupt_number];

  descriptor->offset_1 = (uint32_t) address & 0x0000FFFF;
  descriptor->selector = KERNEL_CODE_SELECTOR;
  descriptor->zero = 0x00;

  // set gate type to interrupt gate
  // set 0 for interrupt and trap gate
  // set descriptor privilege level to 3 (user space ring)
  // set 1 for present - used interrupt
  descriptor->type_attr = 0xEE;
  descriptor->offset_2 = (uint32_t) address >> 16;
}

void idt_init()
{
  memset(idt_descriptors, 0, sizeof(idt_descriptors));
  
  for (int index = 0; index < TOTAL_INTERRUPTS; index++) 
  {
    idt_set(index, interrupt_pointer_table[index]);
  }

  idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
  idtr_descriptor.base = (uint32_t) idt_descriptors;

  idt_set(0x80, isr80h_wrapper);

  idt_load(&idtr_descriptor);
}

int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback)
{
  if (interrupt < 0 || interrupt >= TOTAL_INTERRUPTS)
  {
    return -INVALID_ARGUMENT_ERROR;
  }

  interrupt_callbacks[interrupt] = interrupt_callback;

  return 0;
}

void isr80h_register_command(int command, ISR80H_COMMAND command_func)
{
  if (command < 0 || command >= OS_MAX_ISR80H_COMMANDS)
  {
    panic("The command is out of bounds\n");
  }

  if (isr80h_commands[command])
  {
    panic("You're ettermpting to overrite an exitsting command\n");
  }

  isr80h_commands[command] = command_func;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame)
{
  if (command < 0 || command >= OS_MAX_ISR80H_COMMANDS)
  {
    return 0;
  }

  ISR80H_COMMAND command_func = isr80h_commands[command];

  if (!command_func)
  {
    return 0;
  }

  return command_func(frame);
}

void* isr80h_handler(int command, struct interrupt_frame* frame)
{
  void* res = 0;

  kernel_page();
  task_current_save_state(frame);

  res = isr80h_handle_command(command, frame);

  task_page();

  return res;
}
