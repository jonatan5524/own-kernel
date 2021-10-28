ORG 0x7c00
; boot processer mode - 16bit mode
BITS 16

; set code and data offset addresses
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
  jmp short start
  nop

; fill bios parameter block with 0
times 33 db 0

start:
  ; set code segment 
  jmp 0:start_boot

start_boot:
  ; clear interrupts
  cli

  ; set data segment and extra segment
  mov ax, 0x0
  mov ds, ax
  mov es, ax

  ; set stack segment
  mov ss, ax
  mov sp, 0x7c00 

  ; enables interrupts 
  sti
 
.load_protected:
  cli
  ; loads gdt
  lgdt[gdt_descriptor]
  ; set PE (Protection Enable) bit in CR0 (Control Register 0)
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax
  ; switch to code selector and jmp to load32 address
  jmp CODE_SEG:load32 

; GDT - Global Descriptor Table
gdt_start:
gdt_null:
  dd 0x0
  dd 0x0 

; offset 0x8 - default code segment gdt
; CS should point to this
gdt_code:
  ; segment limit first 0-15 bits
  dw 0xffff
  ; Base 0-15 bits
  dw 0
  ; Base 16-23 bits
  db 0 
  ; Access byte
  db 0x9a
  ; high 4 bit flags and low 4 bit flags
  db 11001111b
  ; base 24-31 bits
  db 0 

; offset 0x10 - default data segment gdt
; DS, SS, ES, FS, GS sgould point to this
gdt_data:
  ; segment limit first 0-15 bits
  dw 0xffff
  ; Base 0-15 bits
  dw 0
  ; Base 16-23 bits
  db 0 
  ; Access byte
  db 0x92
  ; high 4 bit flags and low 4 bit flags
  db 11001111b
  ; base 24-31 bits
  db 0 

gdt_end:

gdt_descriptor:
  ; gdt size
  dw gdt_end - gdt_start-1
  ; gdt start address
  dd gdt_start 

; set 32 bit code 
[BITS 32]
load32:
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov ebp, 0x00200000
  mov esp, ebp
  jmp $

; fill until 510 bytes of sector with 0
times 510-($ - $$) db 0
; add the bootloader signiture
dw 0xAA55
