; vim: set ft=nasm :
[BITS 32]

global print:function
global getkey:function

; void print(const char* massage)
print:
  push esp
  mov ebp, esp

  push dword[ebp + 8]
  mov eax, 1 ; command print
  int 0x80 
  add esp, 4

  pop esp 
  ret 

; int getkey()
getkey:
  push ebp
  mov ebp, esp

  mov eax, 2 ; command getkey
  int 0x80

  pop ebp
  ret 
