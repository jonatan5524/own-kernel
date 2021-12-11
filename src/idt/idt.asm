; vim: set ft=nasm :
section .asm

global idt_load
global int21h
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler

enable_interrupts:
  sti
  ret

disable_interrupts:
  cli
  ret

idt_load:
  push ebp
  mov ebp, esp

  ; take the first arg to function
  mov ebx, [ebp + 8]
  ; set the interrupt table
  lidt [ebx]

  pop ebp
  ret 

int21h:
  pushad
  
  call int21h_handler

  popad
  iret

no_interrupt:
  pushad
  
  call no_interrupt_handler

  popad
  iret

isr80h_wrapper:
  ; INTERRUPT FRAME START
  ; ALREADY PUSHED TO US BY PROCESSOR UPON ENTRY TO THIS INTERRUPT
  ; uint32_t ip
  ; uint32_t cs
  ; uint32_t flags
  ; uint32_t sp
  ; uint32_t ss
  ; Pushes the general purpose registers to the stack 
  pushad

  ; INTERRUPT FRAME END

  ; Push the stack pointer so that we are pointing to the interrupt frame
  push esp
  
  ; EAX holds our command
  push eax
  call isr80h_handler
  mov dword[tmp_res], eax
  add esp, 8

  ; Restore general purpose registers for user land
  popad
  mov eax, [tmp_res]

  iretd

section .data
; Stored the return result from isr80h_handler 
tmp_res: dd 0 
