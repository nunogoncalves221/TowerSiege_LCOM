/**
 * @file instructions.h
 * @brief How-to-play instructions screen.
 *
 * Renders a static screen explaining the game controls and objectives.
 * No interactivity beyond the calling state machine — the caller handles
 * the key press that exits this screen.
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/**
 * @brief Draws the instructions screen.
 */
void instructions_draw(void);

#endif /* INSTRUCTIONS_H */
