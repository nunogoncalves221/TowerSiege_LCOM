/**
 * @file sprite.h
 * @brief Static sprite representation and drawing utilities.
 *
 * A sprite holds only the visual representation of a game object:
 * its dimensions and the raw pixel colour map. As recommended in the
 * lectures, game-logic data (position, velocity, etc.) is kept
 * separate from the visual data.
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <lcom/lcf.h>

/**
 * @brief Holds the visual data of a single, non-animated image.
 */
typedef struct {
    uint16_t  width;  /**< Width of the sprite in pixels. */
    uint16_t  height; /**< Height of the sprite in pixels. */
    uint32_t *map;    /**< Flat array of ARGB pixel colours (width × height). */
} sprite_t;

/**
 * @brief Allocates memory and loads an XPM image into a new sprite.
 *
 * @param xpm XPM map describing the image to load.
 * @return Pointer to the newly allocated ::sprite_t, or NULL on failure.
 */
sprite_t *create_sprite(xpm_map_t xpm);

/**
 * @brief Frees all memory associated with a sprite.
 *
 * @param sp Pointer to the sprite to destroy. May be NULL (no-op).
 */
void destroy_sprite(sprite_t *sp);

/**
 * @brief Draws the sprite at the given screen coordinates.
 *
 * Transparent pixels (colour 0x00000000) are skipped.
 *
 * @param sp Sprite to draw.
 * @param x  Horizontal screen coordinate of the top-left corner.
 * @param y  Vertical   screen coordinate of the top-left corner.
 * @return 0 on success, non-zero on error.
 */
int draw_sprite(sprite_t *sp, int16_t x, int16_t y);

/**
 * @brief Draws the sprite mirrored horizontally.
 *
 * Useful when a character is facing right but only a left-facing
 * sprite asset exists.
 *
 * @param sp Sprite to draw flipped.
 * @param x  Horizontal screen coordinate of the top-left corner.
 * @param y  Vertical   screen coordinate of the top-left corner.
 * @return 0 on success, non-zero on error.
 */
int draw_sprite_flipped(sprite_t *sp, int16_t x, int16_t y);

#endif /* SPRITE_H */
