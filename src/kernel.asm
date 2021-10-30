; set 32 bit code 
[BITS 32]
global _start

; set code and data offset addresses
CODE_SEQ equ 0x08
DATA_SEG equ 0x10

_start:
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

  jmp $

times 512-($ - $$) db 0
