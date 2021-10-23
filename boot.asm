; BIOS load bootloader address
ORG 0x7c00
; boot processer mode - 16bit mode
BITS 16

start:
  mov si, massage
  call print

  jmp $ ; jump to intself, make the program halt

print:
  mov bx, 0 

.loop:
  lodsb
  cmp al, 0
  je .done
  call print_char
  jmp .loop

.done:
  ret

print_char:
  mov ah, 0eh
  int 0x10
  ret

massage: db 'Hello World!', 0

; fill until 510 bytes of sector with 0
times 510-($ - $$) db 0
; add the bootloader signiture
dw 0xAA55
