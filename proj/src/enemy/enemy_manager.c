#include "enemy_manager.h"
#include "player.h"
#include "video.h"

bool enemy_manager_tick(enemy_t **enemies, int *enemy_count) {
    int16_t screen_w = (int16_t) vg_get_x_res();
    int16_t screen_h = (int16_t) vg_get_y_res();
    bool game_over_triggered = false;

    for (int i = 0; i < *enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_update(enemies[i]);

            // enemy escaped through the right edge 
            if (enemies[i]->x > screen_w) {
                enemies[i]->active = false;
               
                player_take_damage(1);
                
                if (player_is_dead()) {
                    game_over_triggered = true;
                }
            }
            // safety: deactivate if out of bounds in any other direction
            else if (enemies[i]->y > screen_h ||
                     enemies[i]->y < -(int16_t) enemies[i]->sp->height ||
                     enemies[i]->x < -(int16_t) enemies[i]->sp->width) {
                enemies[i]->active = false;
            }

            // if the enemy is inactive (dead or off-screen), destroy it
            if (!enemies[i]->active) {
                if (enemies[i]->hp <= 0) {
                    player_add_score(10);
                }
                enemy_destroy(enemies[i]);

                // shift array to maintain strict visual depth order
                for (int j = i; j < *enemy_count - 1; j++) {
                    enemies[j] = enemies[j + 1];
                }
                enemies[*enemy_count - 1] = NULL;
                (*enemy_count)--;
                i--; // re-check this index since a new enemy shifted into it
            }
        }
    }
    
    return game_over_triggered; // tell game.c if we died
}

void enemy_manager_draw(enemy_t **enemies, int enemy_count) {
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_draw(enemies[i]);
        }
    }
}
