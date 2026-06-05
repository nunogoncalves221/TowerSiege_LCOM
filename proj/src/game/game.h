/**
 * @file game.h
 * @brief Core game loop, input routing, and global game constants.
 *
 * Owns the top-level game state: the map, hero, towers, enemies, and waves.
 * The main application loop calls game_tick() and game_draw() every frame,
 * and forwards hardware events through the handle functions below.
 */

#ifndef GAME_H
#define GAME_H

#include <lcom/lcf.h>
#include "rtc.h"

// constants

#define TIMER_FREQ 60
#define FPS 30
#define MAX_LIVES 15
#define MAX_ENEMIES 20

#define HERO_SPEED  3
#define HERO_BASE_X 1038
#define HERO_BASE_Y 646

#define WAVE_DELAY 600

// keyboard scancodes

#define KEY_W_MAKE     0x11
#define KEY_A_MAKE     0x1E
#define KEY_S_MAKE     0x1F
#define KEY_D_MAKE     0x20
#define KEY_W_BREAK    0x91
#define KEY_A_BREAK    0x9E
#define KEY_S_BREAK    0x9F
#define KEY_D_BREAK    0xA0

#define KEY_UP_MAKE    0x48
#define KEY_DOWN_MAKE  0x50
#define KEY_ENTER_MAKE 0x1C

#define KEY_F_MAKE  0x21
#define KEY_F_BREAK 0xA1


/**
 * @brief Initialises all game subsystems and places the first wave.
 *
 * Must be called before any other game function and again whenever the
 * player retries after a game-over.
 */
void game_reset(void);

/**
 * @brief Frees all resources allocated by game_reset().
 *
 * Call when leaving the game state entirely (e.g. returning to the main menu).
 */
void game_destroy(void);

/**
 * @brief Advances the game simulation by one tick.
 *
 * Updates the hero, enemies, towers, bullets, and wave spawner.
 * Must be called once per frame before game_draw().
 */
void game_tick(void);

/**
 * @brief Renders the current game frame to the back-buffer.
 *
 * Draws the map, enemies, towers, bullets, hero, and HUD in order.
 */
void game_draw(void);

/**
 * @brief Returns whether the game session has ended.
 *
 * Becomes true in two cases: the player's life count reaches zero (defeat),
 * or wave 10 is completed with no enemies remaining (victory).
 * Callers must use wave_is_complete() to distinguish the two outcomes.
 *
 * @return true when the game is over (win or loss).
 */
bool game_is_over(void);

/**
 * @brief Forwards a keyboard scancode to the relevant in-game handler.
 *
 * Routes movement keys to the hero, Escape to the pause screen, and
 * tower-popup keys when a popup is open.
 *
 * @param scancode Raw keyboard scancode.
 */
void game_handle_key(uint8_t scancode);

/**
 * @brief Applies a mouse delta to the in-game cursor position.
 *
 * @param dx Horizontal movement in pixels (positive = right).
 * @param dy Vertical   movement in pixels (positive = down).
 */
void game_update_cursor(int16_t dx, int16_t dy);

/**
 * @brief Processes a left mouse-button click at the current cursor position.
 *
 * Checks whether the click lands on a grass tile and opens the tower
 * placement popup if so.
 */
void game_handle_click(void);

/**
 * @brief Returns the player's current score.
 *
 * @return Score accumulated during this game session.
 */
int game_get_score(void);

/**
 * @brief Returns the vertical screen coordinate of the cursor.
 *
 * Used by the game-over menu to position the cursor-dependent UI.
 *
 * @return Cursor Y position in screen pixels.
 */
int16_t game_get_cursor_y(void);

/**
 * @brief Returns the RTC timestamp recorded when the game started.
 *
 * @return ::rtc_date captured at the beginning of the current session.
 */
rtc_date game_get_start_time(void);

#endif /* GAME_H */
