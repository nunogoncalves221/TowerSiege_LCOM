#include "hero.h"
#include "game.h"
#include "player.h"
#include "video.h"
#include "hud.h"

#include "animated_sprite.h"

// hero assets
#include "../assets/hero.xpm"
#include "../assets/hero_walk_1.xpm"
#include "../assets/hero_walk_2.xpm"
#include "../assets/hero_walk_3.xpm"
#include "../assets/hero_atk_1.xpm"
#include "../assets/hero_atk_2.xpm"
#include "../assets/hero_atk_3.xpm"

// hero mortality stats
#define HERO_MAX_HP 100
#define HERO_RESPAWN_TICKS 180 // 3 sec
#define HERO_SPEED 3
#define HERO_ATTACK_COOLDOWN 40 // ticks between attacks (~1.3s at 30fps)

// hero hp bar visual constants
#define HERO_HP_BAR_WIDTH 40
#define HERO_HP_BAR_HEIGHT 4
#define HERO_HP_BAR_Y_OFFSET 10
#define HERO_HP_COLOR_GOOD 0x0000FF00 // green
#define HERO_HP_COLOR_BG   0x00FF0000 // red
#define HERO_HP_COLOR_BORDER 0x00000000 // black

typedef enum { HERO_IDLE, HERO_WALK, HERO_ATTACK } hero_state_t;

static hero_state_t current_hero_state = HERO_IDLE;
static AnimSprite *hero_idle = NULL;
static AnimSprite *hero_walk = NULL;
static AnimSprite *hero_attack = NULL;

static int16_t hero_x = 0;
static int16_t hero_y = 0;
static int hero_hp = HERO_MAX_HP;
static int hero_respawn_timer = 0;
static int hero_attack_cd = 0;

// internal input state
static bool key_w = false, key_a = false, key_s = false, key_d = false, key_f = false;

void hero_init(void) {
    xpm_map_t idle_frames[] = { (xpm_map_t) hero }; 
    xpm_map_t walk_frames[] = { (xpm_map_t) hero_walk_1, (xpm_map_t) hero_walk_2, (xpm_map_t) hero_walk_3 };
    xpm_map_t atk_frames[]  = { (xpm_map_t) hero_atk_1, (xpm_map_t) hero_atk_2, (xpm_map_t) hero_atk_3 };

    hero_idle = create_animated_sprite(idle_frames, 1, 15);
    hero_walk = create_animated_sprite(walk_frames, 3, 10);
    hero_attack = create_animated_sprite(atk_frames, 3, 8);

    hero_x = HERO_BASE_X;
    hero_y = HERO_BASE_Y;
    hero_hp = HERO_MAX_HP;
    hero_respawn_timer = 0;
    current_hero_state = HERO_IDLE;
}

void hero_destroy(void) {
    destroy_animated_sprite(hero_idle);
    destroy_animated_sprite(hero_walk);
    destroy_animated_sprite(hero_attack);
}

void hero_update_input(bool w, bool a, bool s, bool d, bool f) {
    key_w = w; key_a = a; key_s = s; key_d = d; key_f = f;
}

void hero_tick(enemy_t **enemies, int enemy_count) {
    if (hero_idle == NULL) return; // safety check just in case

    int16_t screen_w = (int16_t) vg_get_x_res();
    int16_t screen_h = (int16_t) vg_get_y_res();

    // handle the respawn countdown
    if (hero_respawn_timer > 0) {
        hero_respawn_timer--; 
        if (hero_respawn_timer == 0) {
            hero_hp = HERO_MAX_HP; // back from the dead with full hp
        }
    }

    if (hero_respawn_timer == 0) {
        bool is_moving = false;

        if (key_w) { hero_y -= HERO_SPEED; is_moving = true; }
        if (key_s) { hero_y += HERO_SPEED; is_moving = true; }
        if (key_a) { hero_x -= HERO_SPEED; is_moving = true; }
        if (key_d) { hero_x += HERO_SPEED; is_moving = true; }

        // clamp hero to screen bounds 
        int16_t hw = (int16_t) hero_idle->sp->width;
        int16_t hh = (int16_t) hero_idle->sp->height;
        if (hero_x < 0) hero_x = 0;
        if (hero_y < 0) hero_y = 0;
        if (hero_x > screen_w - hw) hero_x = screen_w - hw;
        if (hero_y > screen_h - hh) hero_y = screen_h - hh;

        if (hero_attack_cd > 0) hero_attack_cd--;

        // trigger attack if f is pressed
        if (key_f && current_hero_state != HERO_ATTACK && hero_attack_cd == 0) {
            current_hero_state = HERO_ATTACK;
            reset_animated_sprite(hero_attack);
            
            // DAMAGE IS APPLIED IMMEDIATELY UPON PRESSING THE ATTACK BUTTON
            int h_center_x = hero_x + (hero_attack->sp->width / 2);
            int h_center_y = hero_y + (hero_attack->sp->height / 2);

            for (int i = 0; i < enemy_count; i++) {
                if (enemies[i] != NULL && enemies[i]->active) {
                    int e_center_x = enemies[i]->x + (enemies[i]->sp->width / 2);
                    int e_center_y = enemies[i]->y + (enemies[i]->sp->height / 2);
                    int dx = e_center_x - h_center_x;
                    int dy = e_center_y - h_center_y;
                    
                    // smash within 100 pixels
                    if ((dx * dx) + (dy * dy) <= (100 * 100)) {
                        enemies[i]->hp -= 15;
                        if (enemies[i]->hp <= 0) {
                            enemies[i]->active = false;
                            player_add_gold(enemies[i]->gold_value);
                        }
                    }
                }
            }
        }

        // state machine only handles visuals and reverting to idle/walk now
        if (current_hero_state == HERO_ATTACK) {
            animate_sprite_once(hero_attack);
            
            if (animated_sprite_finished(hero_attack)) {
                hero_attack_cd = HERO_ATTACK_COOLDOWN;
                current_hero_state = is_moving ? HERO_WALK : HERO_IDLE;
            }
        } else {
            if (is_moving) {
                current_hero_state = HERO_WALK;
                animate_sprite_loop(hero_walk);
            } else {
                current_hero_state = HERO_IDLE;
                animate_sprite_loop(hero_idle);
            }
        }
    }

    // handle getting hit by enemies
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL && enemies[i]->active && hero_respawn_timer == 0) {
            int h_center_x = hero_x + (hero_idle->sp->width / 2);
            int h_center_y = hero_y + (hero_idle->sp->height / 2);
            int e_center_x = enemies[i]->x + (enemies[i]->sp->width / 2);
            int e_center_y = enemies[i]->y + (enemies[i]->sp->height / 2);
            
            int dx = e_center_x - h_center_x;
            int dy = e_center_y - h_center_y;
            
            if ((dx * dx) + (dy * dy) <= (30 * 30)) {
                hero_hp -= 1; // lost hp
                
                if (hero_hp <= 0) {
                    hero_hp = 0;
                    hero_respawn_timer = HERO_RESPAWN_TICKS;
                    
                    // throw him back to base
                    hero_x = HERO_BASE_X;
                    hero_y = HERO_BASE_Y;
                }
            }
        }
    }
}

void hero_draw(void) {
    if (hero_respawn_timer > 0) return; // safety check

    if (current_hero_state == HERO_ATTACK && hero_attack != NULL) {
        draw_animated_sprite(hero_attack, hero_x, hero_y);
    } 
    else if (current_hero_state == HERO_WALK && hero_walk != NULL) {
        draw_animated_sprite(hero_walk, hero_x, hero_y);
    } 
    else if (hero_idle != NULL) {
        draw_animated_sprite(hero_idle, hero_x, hero_y);
    }

    // grab the ui helper function from game.c to draw the hp bar
    int sprite_w = hero_idle->sp->width;
    int bar_start_x = hero_x + (sprite_w / 2) - (HERO_HP_BAR_WIDTH / 2);
    int bar_start_y = hero_y - HERO_HP_BAR_Y_OFFSET;
    
    draw_hp_bar(bar_start_x, bar_start_y, HERO_HP_BAR_WIDTH, HERO_HP_BAR_HEIGHT,
                hero_hp, HERO_MAX_HP, HERO_HP_COLOR_GOOD, HERO_HP_COLOR_BG, HERO_HP_COLOR_BORDER);
}
