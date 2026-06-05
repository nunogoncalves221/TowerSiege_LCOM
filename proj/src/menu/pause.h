/**
 * @file pause.h
 * @brief In-game pause overlay with continue, retry, and quit options.
 *
 * Rendered on top of the frozen game frame. The application state machine
 * acts on the confirmed selection to resume play, restart, or exit to menu.
 */

#ifndef PAUSE_H
#define PAUSE_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Options available in the pause overlay. */
typedef enum {
    PAUSE_CONTINUE = 0, /**< Resume the current game. */
    PAUSE_RETRY,        /**< Restart the game from wave 1. */
    PAUSE_QUIT,         /**< Return to the main menu. */
    PAUSE_OPTION_COUNT  /**< Number of selectable options. */
} pause_option_t;

/**
 * @brief Resets the highlighted option back to PAUSE_CONTINUE.
 *
 * Should be called when the pause overlay is opened.
 */
void pause_reset(void);

/**
 * @brief Returns the option the player currently has highlighted.
 *
 * @return The currently selected ::pause_option_t value.
 */
pause_option_t pause_get_selected(void);

/**
 * @brief Processes one keyboard scancode and updates the highlighted option.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @return true if the player pressed Enter to confirm the selection.
 */
bool pause_handle_key(uint8_t scancode);

/**
 * @brief Draws the pause overlay on top of the current frame.
 */
void pause_draw(void);

#endif /* PAUSE_H */
