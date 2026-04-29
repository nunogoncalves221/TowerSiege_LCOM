#ifndef UTILS_H
#define UTILS_H

#include <lcom/lcf.h>
#include <stdint.h>

// Function prototypes (the "menu")
int (util_get_LSB)(uint16_t val, uint8_t *lsb);
int (util_get_MSB)(uint16_t val, uint8_t *msb);
int (util_sys_inb)(int port, uint8_t *value);


uint32_t (kbd_get_sys_inb_count)(void);
void (kbd_reset_sys_inb_count)(void);

#endif
