; vim: set ft=nasm :
[BITS 32]

section .asm

global print:function
global getkey:function
global os_putchar:function
global os_malloc:function
global os_free:function

; void print(const char* massage)
print:
  push ebp
  mov ebp, esp

  push dword[ebp + 8] ; variable massage
  mov eax, 1 ; command print
  int 0x80 
  add esp, 4

  pop ebp 
  ret 

; int getkey()
getkey:
  push ebp
  mov ebp, esp

  mov eax, 2 ; command getkey
  int 0x80

  pop ebp
  ret 

; void putchar(char c)
os_putchar:
  push ebp
  mov ebp, esp

  mov eax, 3 ; command putchar
  push dword[ebp + 8] ; variable c
  int 0x80
  add esp, 4

  pop ebp
  ret

; void* os_malloc(size_t size)
os_malloc:
  push ebp
  mov ebp, esp

  mov eax, 4 ; command malloc
  push dword[ebp + 8] ; variable size
  int 0x80
  add esp, 4

  pop ebp
  ret

; void os_free(void* ptr)
os_free:
  push ebp
  mov ebp, esp

  mov eax, 5 ; command free
  push dword[ebp + 8]
  int 0x80
  add esp, 4

  pop ebp
  ret