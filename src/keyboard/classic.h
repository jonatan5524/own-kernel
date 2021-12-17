#ifndef CLASS_KEYBOARD_H
#define CLASS_KEYBOARD_H

#include <stdint.h>

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

#define CLASSIC_KEYBOARD_KEY_RELEASED 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

struct keyboard* classic_init();
uint8_t class_keyboard_scancode_to_char(uint8_t scancode);

#endif
