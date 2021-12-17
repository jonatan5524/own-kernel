; vim: set ft=nasm :
section .asm

global idt_load
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

extern int21h_handler
extern isr80h_handler
extern interrupt_handler

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

%macro interrupt 1
  global int%1
  int%1:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US BY PROCESSOR UPON ENTRY TO THIS INTERRUPT
    ; uint32_t ip
    ; uint32_t cs
    ; uint32_t flags
    ; uint32_t sp
    ; uint32_t ss
    ; Pushes the general purpose registers to the stack 
    pushad

    ; Interrupt frame end
    push esp
    push dword %1
    call interrupt_handler

    add esp, 8
    popad
    iret 
%endmacro

%assign i 0
%rep 512
  interrupt i
%assign i i + 1
%endrep

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

%macro interrupt_array_entry 1
  dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
  interrupt_array_entry i
%assign i i + 1
%endrep
