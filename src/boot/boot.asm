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
  jmp $

; GDT - Global Descriptor Table
; Base - linear address of the segment
; Limit - Size of the segment 
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

[BITS 32]
load32:
  ; sector to read from
  mov eax, 1
  ; number of sector to read 
  mov ecx, 100
  ; address to load into 
  mov edi, 0x0100000
  call ata_lba_read
  jmp CODE_SEG:0x0100000

ata_lba_read:
  ; backup the lba
  mov ebx, eax
  ; send the highest 8 bits of the lba to hard disk controller
  shr eax, 24
  ; select the master drive
  or eax, 0xE0
  ; port to write the 8 bits to 
  mov dx, 0x1F6
  out dx, al
  ; finished sending highest 8 bits of the lba

  ; send the total sector to read 
  mov eax, ecx
  mov dx, 0x1F2
  out dx, al 
  
  ; send more bits to the lba
  ; retore the backyp lba 
  mov eax, ebx
  mov dx, 0x1F3
  out dx, al

  ; send more bits of the lba
  mov dx, 0x1F4
  ; restore the backup lba
  mov eax, ebx
  shr eax, 8
  out dx, al

  ; send upper 16 bits of the lba
  mov dx, 0x1F5
  ; restore the backup lba
  mov eax, ebx 
  shr eax, 16 
  out dx, al

  mov dx, 0x1F7
  mov al, 0x20
  out dx, al 

  ; read all sectors into memory
.next_sector:
  push ecx

; checking if we need to read
.try_again:
  mov dx, 0x1F7
  in al,dx
  test al, 8
  jz .try_again

; read 256 words at a time
  mov ecx, 256
  mov dx, 0x1F0
  rep insw
  pop ecx
  loop .next_sector
  ret

; fill until 510 bytes of sector with 0
times 510-($ - $$) db 0
; add the bootloader signiture
dw 0xAA55
