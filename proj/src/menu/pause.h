#ifndef PAUSE_H
#define PAUSE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PAUSE_CONTINUE = 0,
    PAUSE_QUIT,
    PAUSE_OPTION_COUNT
} pause_option_t;

void pause_reset(void);
pause_option_t pause_get_selected(void);
bool pause_handle_key(uint8_t scancode);
void pause_draw(void);

#endif
