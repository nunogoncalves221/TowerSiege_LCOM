#include <lcom/lcf.h>
#include <stdint.h>
#include "utils.h"

uint32_t sys_inb_count = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;

  *lsb = (uint8_t)(val & 0x00FF);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;

  *msb = (uint8_t)((val >> 8) & 0x00FF);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t temp;

  if (sys_inb(port, &temp) != 0) return 1;

  *value = (uint8_t) temp;

  sys_inb_count++;

  return 0;
}

uint32_t (kbd_get_sys_inb_count)(void) { return sys_inb_count; } // return number of sys_inb calls
void (kbd_reset_sys_inb_count)(void) { sys_inb_count = 0; } // reset sys_inb counter
