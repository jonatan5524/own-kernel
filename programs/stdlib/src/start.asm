; vim: set ft=nasm :
[BITS 32]
section .asm

global _start 

extern main

_start:
  call main
  ret
