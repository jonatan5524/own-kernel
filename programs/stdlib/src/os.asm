; vim: set ft=nasm :
[BITS 32]

section .asm

global print:function
global os_getkey:function
global os_putchar:function
global os_malloc:function
global os_free:function
global os_process_load_start:function
global os_process_get_arguments:function
global os_system:function
global os_exit:function

; void os_exit()
os_exit:
  push ebp
  mov ebp, esp

  mov eax, 0 ; command exit
  int 0x80

  pop ebp
  ret

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
os_getkey:
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

; void os_process_load_start(const char* filename)
os_process_load_start:
  push ebp
  mov ebp, esp

  mov eax, 6 ; command process_load_start
  push dword[ebp + 8]
  int 0x80
  add esp, 4

  pop ebp
  ret 

; int os_system(struct command_argument* arguments)
os_system:
  push ebp
  mov ebp, esp

  mov eax, 7 ; command process system (runs a system command based on arguments)
  push dword[ebp + 8]
  int 0x80
  add esp, 4

  pop ebp
  ret 

; void os_process_get_arguments(struct process_arguments* arguments);
os_process_get_arguments:
  push ebp
  mov ebp, esp
  
  mov eax, 8 ; command get process arguments
  push dword[ebp + 8] ; variable arguments
  int 0x80
  add esp, 4

  pop ebp
  ret

