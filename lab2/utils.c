#include <lcom/lcf.h>

#include <stdint.h>

//extract the rightmost 8 bits and keep them on lsb
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  //error handling
  if (lsb == NULL) return 1;

  //cast
  *lsb = (uint8_t) val;
  return 0;
}

//extract the leftmost 8 bits and keep them on msb
int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  //error handling
  if (msb == NULL) return 1;

  //cast + shift right
  *msb = (uint8_t) (val >> 8);
  return 0;
}

//invokes the minix sys_inb() function, but reads only 8 bits (1 byte) 
//instead of 32 bits, preventing memory corruption.
int(util_sys_inb)(int port, uint8_t *value) {
  //error handling
  if (value == NULL) return 1;

  //now we create a temporary 32-bit variable 
  uint32_t temp;

  //we pass the adress of temp
  if (sys_inb(port, &temp) != 0) return 1;

  //cast
  *value = (uint8_t) temp;
  return 0;
}
