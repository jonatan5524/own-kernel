; vim: set ft=nasm :
[BITS 32]
section .asm

global _start 

extern c_start
extern os_exit

_start:
  call c_start
  call os_exit
  ret
