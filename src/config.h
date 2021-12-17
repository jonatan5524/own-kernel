#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define TOTAL_INTERRUPTS 512

// 100MB heap size
#define HEAP_SIZE_BYTES 104857600
#define HEAP_BLOCK_SIZE 4096
#define HEAP_ADDRESS  0x01000000
#define HEAP_TABLE_ADDRESS  0x00007E00

#define SECTOR_SIZE 512

#define OS_MAX_FILESYSTEMS 20
#define OS_MAX_FILE_DESCRIPTORS 512

#define OS_MAX_PATH 108

#define TOTAL_GDT_SEGMENTS 6

#define OS_PROGRAM_VIRTUAL_ADDRESS 0x400000
#define OS_USER_PROGRAM_VIRTUAL_STACK_SIZE 1024 * 16
#define OS_USER_PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define OS_USER_PROGRAM_VIRTUAL_STACK_ADDRESS_END OS_USER_PROGRAM_VIRTUAL_STACK_ADDRESS_START - OS_USER_PROGRAM_VIRTUAL_STACK_SIZE 

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

#define OS_MAX_PROGRAM_ALLOCARTIONS 1024
#define OS_MAX_PROCESSES 12

#define OS_MAX_ISR80H_COMMANDS 1024

#define OS_KEYBOARD_BUFFER_SIZE 1024

#endif
