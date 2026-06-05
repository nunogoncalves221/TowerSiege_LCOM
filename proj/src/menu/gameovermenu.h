/**
 * @file gameovermenu.h
 * @brief Game-over screen for runs that do not qualify for the high-score table.
 *
 * Presents the player's final score and two options: retry the game or
 * return to the main menu.
 */

#ifndef GAMEOVERMENU_H
#define GAMEOVERMENU_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Options available on the game-over screen. */
typedef enum {
    GAMEOVER_RETRY = 0,   /**< Start a new game immediately. */
    GAMEOVER_MENU,        /**< Return to the main menu. */
    GAMEOVER_OPTION_COUNT /**< Number of selectable options. */
} gameover_option_t;

/**
 * @brief Resets the highlighted option back to GAMEOVER_RETRY.
 *
 * Should be called when entering the game-over screen.
 */
void gameover_reset(void);

/**
 * @brief Returns the option the player currently has highlighted.
 *
 * @return The currently selected ::gameover_option_t value.
 */
gameover_option_t gameover_get_selected(void);

/**
 * @brief Processes one keyboard scancode and updates the highlighted option.
 *
 * @param scancode Raw keyboard scancode.
 * @return true if the player pressed Enter to confirm the selection.
 */
bool gameover_handle_key(uint8_t scancode);

/**
 * @brief Draws the game-over screen with the player's final score.
 *
 * @param score Final score to display.
 */
void gameover_draw(int score);

#endif /* GAMEOVERMENU_H */
