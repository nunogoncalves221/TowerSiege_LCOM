#include "player.h"

static int   player_gold  = 500;
static int   player_lives = 20;
static int   player_max_lives = 20;
static int   player_score = 0;
static float score_mult   = 1.0f;

void player_init(difficulty_t diff) {
    if (diff == DIFF_MEDIUM) {
        player_gold = 400; player_max_lives = 15; score_mult = 1.5f;
    } else if (diff == DIFF_HARD) {
        player_gold = 300; player_max_lives = 10; score_mult = 2.0f;
    } else {
        player_gold = 500; player_max_lives = 20; score_mult = 1.0f;
    }
    player_lives = player_max_lives;
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
    return player_max_lives;
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
    player_score += (int)(amount * score_mult);
}
