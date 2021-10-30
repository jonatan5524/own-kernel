section .asm

global idt_load

idt_load:
  push ebp
  mov ebp, esp

  ; take the first arg to function
  mov ebx, [ebp + 8]
  ; set the interrupt table
  lidt [ebx]

  pop ebp
  ret 
