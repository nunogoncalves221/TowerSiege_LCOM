/**
 * @file difficulty.h
 * @brief Difficulty selection screen and difficulty type definition.
 *
 * Defines the ::difficulty_t enumeration used throughout the game to scale
 * enemy stats and spawn rates, and provides the keyboard-driven selection
 * screen shown before a new game begins.
 */

#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Available difficulty levels. */
typedef enum {
    DIFF_EASY = 0,    /**< Fewer enemies, lower stats. */
    DIFF_MEDIUM,      /**< Balanced enemy count and stats. */
    DIFF_HARD,        /**< More enemies, higher stats, faster spawns. */
    DIFF_OPTION_COUNT /**< Number of selectable difficulty options. */
} difficulty_t;

/**
 * @brief Returns the difficulty option the player currently has highlighted.
 *
 * @return The currently selected ::difficulty_t value.
 */
difficulty_t diff_get_selected(void);

/**
 * @brief Resets the highlighted option back to DIFF_EASY.
 *
 * Should be called when entering the difficulty selection screen.
 */
void diff_reset(void);

/**
 * @brief Processes one keyboard scancode and updates the highlighted option.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @return true if the player pressed Enter to confirm the selection.
 */
bool diff_handle_key(uint8_t scancode);

/**
 * @brief Draws the difficulty selection screen.
 */
void diff_draw(void);

#endif /* DIFFICULTY_H */
