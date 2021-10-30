; set 32 bit code 
[BITS 32]

global _start

extern kernel_main

; set code and data offset addresses
CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
  ; init data and stack segment
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov ebp, 0x00200000
  mov esp, ebp

  ; Enable the A20 line
  ; in, out - read and write to processer bus
  in al, 0x92
  or al, 2 
  out 0x92, al 

  ; remap the master PIC - programmable interrupt controller
  ; put the pic in initalize mode
  mov al, 00010001b
  ; tell the master PIC
  out 0x20, al

  ; interrupt 0x20 is where master ISR - Interrupt Service Routines should start
  mov al, 0x20
  out 0x21, al

  mov al, 00000001b
  out 0x21, al

  ; enable interrupts
  sti

  call kernel_main

  jmp $

times 512-($ - $$) db 0
