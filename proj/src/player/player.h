#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdbool.h>

// resets player stats for a new game
void player_init(void);

// gold management
int player_get_gold(void);
void player_add_gold(int amount);
bool player_spend_gold(int amount); // returns true if we had enough money

// lives management
int player_get_lives(void);
int player_get_max_lives(void);
void player_take_damage(int amount);
bool player_is_dead(void);

// score management
int player_get_score(void);
void player_add_score(int amount);

#endif
