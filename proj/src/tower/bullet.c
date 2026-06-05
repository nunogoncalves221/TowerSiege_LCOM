#include "bullet.h"
#include "sprite.h"
#include "video.h"
#include "game.h"
#include "player.h"

#define MAX_BULLETS  16
#define BULLET_SPEED 10

typedef struct {
    float    x, y;
    float    vx, vy;
    enemy_t  *target;  // enemy the bullet is heading to
    int      damage;
    sprite_t *sp;
    bool     active;
} bullet_t;

static bullet_t bullets[MAX_BULLETS];

void bullet_init(void) {
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i].active = false;
}

void bullet_fire(int16_t from_x, int16_t from_y, enemy_t *target, int damage, sprite_t *sp) {
    if (target == NULL) return;
    // where bullet aims (center of enemy sprite)
    int to_x = target->x + target->sp->width  / 2;
    int to_y = target->y + target->sp->height / 2;

    // vector from tower to enemy
    int dx = to_x - from_x;
    int dy = to_y - from_y;

    // absolute values
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int maxd = adx > ady ? adx : ady; // max to calculate speed
    if (maxd == 0) return;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) continue;
        // bullet start position (tower)
        bullets[i].x      = (float)from_x;
        bullets[i].y      = (float)from_y;

        //velocity of bullet
        bullets[i].vx     = (float)(dx * BULLET_SPEED) / maxd;
        bullets[i].vy     = (float)(dy * BULLET_SPEED) / maxd;

        // bullet data
        bullets[i].target = target;
        bullets[i].damage = damage;
        bullets[i].sp     = sp;
        bullets[i].active = true;
        return;
    }
}

void bullet_invalidate_target(enemy_t *e) {
    for (int i = 0; i < MAX_BULLETS; i++)
        // if enemy that the bullet was aiming at is destroyed, the bullet flies off-screen
        if (bullets[i].active && bullets[i].target == e)
            bullets[i].target = NULL;
}

void bullet_tick(void) {
    // screen dimensions
    int sw = (int)vg_get_x_res();
    int sh = (int)vg_get_y_res();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        // if target was invalidated, bullet flies off-screen
        if (bullets[i].target == NULL || !bullets[i].target->active) {
            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;
            int bx = (int)bullets[i].x;
            int by = (int)bullets[i].y;
            if (bx < 0 || bx >= sw || by < 0 || by >= sh)
                bullets[i].active = false;
            continue;
        }

        // steer toward current enemy position each frame
        int ex  = bullets[i].target->x + bullets[i].target->sp->width  / 2;
        int ey  = bullets[i].target->y + bullets[i].target->sp->height / 2;
        int dx  = ex - (int)bullets[i].x;
        int dy  = ey - (int)bullets[i].y;
        int adx = dx < 0 ? -dx : dx;
        int ady = dy < 0 ? -dy : dy;
        int maxd = adx > ady ? adx : ady;

        if (maxd <= BULLET_SPEED) {
            bullets[i].target->hp -= bullets[i].damage; // apply damage to enemy
            if (bullets[i].target->hp <= 0) {
                bullets[i].target->active = false; // mark enemy as dead
                player_add_gold(bullets[i].target->gold_value); 
            }
            bullets[i].active = false; // remove bullet
            continue;
        }

        // update velocity on moving target
        bullets[i].vx = (float)(dx * BULLET_SPEED) / maxd;
        bullets[i].vy = (float)(dy * BULLET_SPEED) / maxd;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
    }
}

void bullet_draw(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active || bullets[i].sp == NULL) continue;
        // calculate center of bullet sprite
        int16_t hw = (int16_t)(bullets[i].sp->width  / 2);
        int16_t hh = (int16_t)(bullets[i].sp->height / 2);
        draw_sprite(bullets[i].sp,
                    (int16_t)bullets[i].x - hw,
                    (int16_t)bullets[i].y - hh);
    }
}
