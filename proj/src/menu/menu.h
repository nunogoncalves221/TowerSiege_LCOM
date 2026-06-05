/**
 * @file menu.h
 * @brief Main menu screen and option selection.
 *
 * Handles keyboard navigation across the main menu options and exposes
 * the selected option so the application state machine can react to it.
 */

#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Options available in the main menu. */
typedef enum {
    MENU_PLAY = 0,    /**< Start a new game (goes to difficulty selection). */
    MENU_INSTRUCTIONS,/**< Show the how-to-play screen. */
    MENU_HIGH_SCORES, /**< Show the high-score leaderboard. */
    MENU_QUIT,        /**< Exit the application. */
    MENU_OPTION_COUNT /**< Number of selectable options. */
} menu_option_t;

/**
 * @brief Processes one keyboard scancode and updates the highlighted option.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @return true if the player pressed Enter to confirm the selection.
 */
bool menu_handle_key(uint8_t scancode);

/**
 * @brief Returns the option the player currently has highlighted.
 *
 * @return The currently selected ::menu_option_t value.
 */
menu_option_t menu_get_selected(void);

/**
 * @brief Resets the highlighted option back to MENU_PLAY.
 *
 * Should be called when entering the main menu.
 */
void menu_reset(void);

/**
 * @brief Draws the main menu screen.
 */
void menu_draw(void);

#endif /* MENU_H */
