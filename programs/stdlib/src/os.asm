; vim: set ft=nasm :
[BITS 32]

global print:function

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
