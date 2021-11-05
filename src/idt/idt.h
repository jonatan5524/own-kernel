#ifndef IDT_H
#define IDT_H
#include <stdint.h>

// interrupt descriptor table

struct idt_desc
{
  // offset bits 0 - 15
  uint16_t offset_1;
  // selector thats in our global descriptor table
  uint16_t selector;
  // Does nothing, unused set to zero
  uint8_t zero;
  // Descriptor type and attributes
  uint8_t type_attr;
  // oofset bits 16-32
  uint16_t offset_2;
} __attribute__((packed));

struct idtr_desc
{
  // size of the descriptor table minus one
  uint16_t limit;
  // abse address of the start of the interrupt descriptor table
  uint32_t base;
} __attribute__((packed));

void idt_init();
void enable_interrupts();
void disable_interrupts();
#endif
