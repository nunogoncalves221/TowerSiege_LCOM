#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

// Menu option indices
typedef enum {
    MENU_PLAY = 0,
    MENU_INSTRUCTIONS,
    MENU_SETTINGS,
    MENU_HIGH_SCORES,
    MENU_QUIT,
    MENU_OPTION_COUNT
} menu_option_t;

// Returns true if the user selected PLAY
bool menu_handle_key(uint8_t scancode);

// Returns the currently selected option
menu_option_t menu_get_selected(void);

// Reset the menu selection to PLAY
void menu_reset(void);

// Draw the menu 
void menu_draw(void);

#endif
