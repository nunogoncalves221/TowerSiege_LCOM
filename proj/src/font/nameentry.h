#ifndef NAMEENTRY_H
#define NAMEENTRY_H

#include <stdint.h>
#include <stdbool.h>

void nameentry_reset(void);
bool nameentry_handle_key(uint8_t scancode); // returns true when Enter is pressed
const char *nameentry_get_name(void);
void nameentry_draw(uint16_t x, uint16_t y, uint32_t color);

#endif
