/**
 * @file player.h
 * @brief Player resource management (gold, lives, score).
 *
 * Encapsulates all player-owned resources behind a clean accessor/mutator
 * interface. All state is module-private; other modules must use these
 * functions rather than accessing globals directly.
 */

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdbool.h>

/**
 * @brief Resets all player resources to their initial values.
 *
 * Must be called once before a new game begins (e.g. from game_reset()).
 */
void player_init(void);

/* ------------------------------------------------------------------ */
/** @name Gold management */
/**@{*/

/**
 * @brief Returns the player's current gold balance.
 * @return Gold amount (>= 0).
 */
int  player_get_gold(void);

/**
 * @brief Adds gold to the player's balance.
 *
 * Typically called when an enemy is killed.
 *
 * @param amount Positive gold amount to add.
 */
void player_add_gold(int amount);

/**
 * @brief Deducts gold from the player's balance if sufficient funds exist.
 *
 * Typically called when placing a tower.
 *
 * @param amount Cost to deduct.
 * @return true if the player had enough gold and the amount was deducted;
 *         false if the balance was insufficient (balance unchanged).
 */
bool player_spend_gold(int amount);

/**@}*/
/* ------------------------------------------------------------------ */
/** @name Lives management */
/**@{*/

/**
 * @brief Returns the player's current life count.
 * @return Lives remaining (0 means game over).
 */
int  player_get_lives(void);

/**
 * @brief Returns the maximum number of lives (defined by ::MAX_LIVES).
 * @return Maximum lives for this game session.
 */
int  player_get_max_lives(void);

/**
 * @brief Reduces the player's life count by @p amount.
 *
 * Called when an enemy reaches the base. Lives cannot drop below 0.
 *
 * @param amount Number of lives to remove (usually 1).
 */
void player_take_damage(int amount);

/**
 * @brief Returns whether the player has run out of lives.
 * @return true when lives == 0, false otherwise.
 */
bool player_is_dead(void);

/**@}*/
/** @name Score management */
/**@{*/

/**
 * @brief Returns the player's current score.
 * @return Score (>= 0).
 */
int  player_get_score(void);

/**
 * @brief Adds points to the player's score.
 *
 * @param amount Positive point value to add.
 */
void player_add_score(int amount);

/**@}*/

#endif /* _PLAYER_H_ */
