#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <lcom/lcf.h>
#include <stdint.h>

#define KBD_IRQ 1

#define KBC_ST_REG  0x64
#define KBC_OUT_BUF 0x60

#define OBF     BIT(0)
#define AUX     BIT(5)
#define TO_ERR  BIT(6)
#define PAR_ERR BIT(7)

int (keyboard_subscribe_int)(uint8_t *bit_no);

int (keyboard_unsubscribe_int)();

#endif
