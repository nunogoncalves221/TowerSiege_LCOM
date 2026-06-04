#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DIFF_EASY = 0,
    DIFF_MEDIUM,
    DIFF_HARD,
    DIFF_OPTION_COUNT
} difficulty_t;

// Returns the currently selected difficulty
difficulty_t diff_get_selected(void);

// Reset selection to EASY
void diff_reset(void);

// Returns true if ENTER was pressed
bool diff_handle_key(uint8_t scancode);

// Draw the difficulty selection screen
void diff_draw(void);

#endif
