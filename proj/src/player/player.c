#include "player.h"

#define STARTING_GOLD 500
#define MAX_LIVES 2

static int player_gold = STARTING_GOLD;
static int player_lives = MAX_LIVES;
static int player_score = 0;

void player_init(void) {
    player_gold = STARTING_GOLD;
    player_lives = MAX_LIVES;
    player_score = 0;
}

int player_get_gold(void) {
    return player_gold;
}

void player_add_gold(int amount) {
    player_gold += amount;
    
    // safety net so we don't end up in debt
    if (player_gold < 0) {
        player_gold = 0;
    }
}

bool player_spend_gold(int amount) {
    if (player_gold >= amount) {
        player_gold -= amount;
        return true;
    }
    return false; // too broke to buy this
}

int player_get_lives(void) {
    return player_lives;
}

int player_get_max_lives(void) {
    return MAX_LIVES;
}

void player_take_damage(int amount) {
    player_lives -= amount;
    
    if (player_lives < 0) {
        player_lives = 0;
    }
}

bool player_is_dead(void) {
    return player_lives <= 0;
}

int player_get_score(void) {
    return player_score;
}

void player_add_score(int amount) {
    player_score += amount;
}
