#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

/* shared state — defined in keyboard.c, read by lab3.c */
extern uint8_t scancode_byte;
extern bool    kbc_error;
extern uint8_t scancode_bytes[2];
extern uint8_t scancode_size;
extern bool    scancode_ready;

int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)(void);

int (kbc_write_command)(uint8_t cmd);
int (kbc_write_argument)(uint8_t arg);
int (kbc_read_cmd_byte)(uint8_t *cmd_byte);
int (kbc_write_cmd_byte)(uint8_t cmd_byte);
int (kbd_enable_interrupts)(void);

/* interrupt handler — called from lab3.c on every keyboard IRQ */
void (kbc_ih)(void);

/* polling read — returns 0 when a valid byte is in scancode_byte */
int (kbd_poll)(void);

/* sys_inb counter — managed inside keyboard.c by kbd_poll() */
uint32_t (kbd_get_sys_inb_count)(void);
void     (kbd_reset_sys_inb_count)(void);

#endif
