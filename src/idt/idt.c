#include "idt.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../kernel.h"
#include <stdint.h>

struct idt_desc idt_descriptors[TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);

void idt_zero()
{
  print("Divide by zero error\n");
}

void idt_set(int interrupt_number, void* address)
{
  struct idt_desc* descriptor = &idt_descriptors[interrupt_number];

  descriptor->offset_1 = (uint32_t) address & 0x000FFFF;
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
  
  idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
  idtr_descriptor.base = (uint32_t) idt_descriptors;

  idt_set(32, idt_zero);

  idt_load(&idtr_descriptor);
}
