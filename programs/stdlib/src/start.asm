; vim: set ft=nasm :
[BITS 32]
section .asm

global _start 

extern c_start

_start:
  call c_start
  ret
