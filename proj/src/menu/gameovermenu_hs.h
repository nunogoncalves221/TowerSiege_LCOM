/**
 * @file gameovermenu_hs.h
 * @brief Game-over screen with high-score name entry.
 *
 * Shown instead of the regular game-over screen when the player's score
 * qualifies for the leaderboard. The player types a name and confirms
 * with Enter before the score is saved.
 */

#ifndef GAMEOVERMENU_HS_H
#define GAMEOVERMENU_HS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Resets the name-entry state for a new high-score submission.
 *
 * Should be called when entering this screen.
 */
void gameover_hs_reset(void);

/**
 * @brief Processes one keyboard scancode for the name-entry widget.
 *
 * Forwards the scancode to the nameentry module. Once the player confirms
 * with Enter, the score is saved to the high-score table.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @return true when the player confirms the name and the score has been saved.
 */
bool gameover_hs_handle_key(uint8_t scancode);

/**
 * @brief Draws the high-score game-over screen with the name-entry widget.
 *
 * @param score Final score to display and submit.
 */
void gameover_hs_draw(int score);

#endif /* GAMEOVERMENU_HS_H */
