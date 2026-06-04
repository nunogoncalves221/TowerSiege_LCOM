/**
 * @file states.h
 * @brief Application state machine enumeration.
 *
 * Defines all possible high-level states that the application can be in.
 * The main loop in @c proj.c reads the current state each iteration and
 * dispatches input, update, and draw calls to the appropriate module.
 */

#ifndef STATES_H
#define STATES_H

/**
 * @brief Top-level application states.
 *
 * Transitions between states are driven by user input (menu selections,
 * pause key, game-over condition, etc.).
 */
typedef enum {
    STATE_MENU,         /**< Main menu is being displayed. */
    STATE_DIFFICULTY,   /**< Difficulty selection screen. */
    STATE_GAME,         /**< Active gameplay loop. */
    STATE_GAMEOVER,     /**< Game-over screen (no high-score entry). */
    STATE_INSTRUCTIONS, /**< How-to-play / instructions screen. */
    STATE_PAUSE,        /**< In-game pause overlay. */
    STATE_HIGH_SCORES,  /**< High-score leaderboard view. */
    STATE_GAMEOVER_HS,  /**< Game-over screen with high-score entry form. */
} app_state_t;

#endif /* STATES_H */
