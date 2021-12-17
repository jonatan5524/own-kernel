#ifndef CLASS_KEYBOARD_H
#define CLASS_KEYBOARD_H

#include <stdint.h>

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

struct keyboard* classic_init();
uint8_t class_keyboard_scancode_to_char(uint8_t scancode);

#endif
