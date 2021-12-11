; vim: set ft=nasm :
[BITS 32]
section .asm

global _start 

_start:
  
  push massage
  mov eax, 1 ; Command print
  int 0x80
  add esp, 4

  jmp $

section .data
massage: db 'Hello World From task!', 0
