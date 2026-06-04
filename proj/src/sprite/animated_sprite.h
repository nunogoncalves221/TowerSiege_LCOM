/**
 * @file animated_sprite.h
 * @brief Multi-frame animated sprite with tick-based playback control.
 *
 * An ::AnimSprite wraps a base ::sprite_t and a set of per-frame pixel maps.
 * The animation advances one frame every @c aspeed game ticks, and two
 * playback modes are supported: looping (e.g. walking/idle) and one-shot
 * (e.g. attack/death).
 */

#ifndef __ANIMATED_SPRITE_H__
#define __ANIMATED_SPRITE_H__

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>
#include "sprite.h"

/**
 * @brief Animated sprite composed of multiple frames.
 */
typedef struct {
    sprite_t  *sp;         /**< Base sprite used for dimensions and drawing. */
    int        aspeed;     /**< Game ticks that must pass per animation frame. */
    int        cur_aspeed; /**< Ticks remaining until the next frame switch. */
    int        num_fig;    /**< Total number of frames in the animation. */
    int        cur_fig;    /**< Index of the frame currently being displayed. */
    uint32_t **map;        /**< Array of pixel maps, one entry per frame. */
} AnimSprite;

/**
 * @brief Allocates and initialises an animated sprite from an array of XPMs.
 *
 * @param xpm_array Array of XPM maps, one per animation frame.
 * @param no_pic    Number of frames (length of @p xpm_array).
 * @param aspeed    Ticks per frame (higher = slower animation).
 * @return Pointer to the newly allocated ::AnimSprite, or NULL on failure.
 */
AnimSprite *create_animated_sprite(xpm_map_t *xpm_array, uint8_t no_pic, int aspeed);

/**
 * @brief Frees all memory associated with an animated sprite.
 *
 * @param as Animated sprite to destroy. May be NULL (no-op).
 */
void destroy_animated_sprite(AnimSprite *as);

/**
 * @brief Draws the current frame of the animated sprite.
 *
 * @param as Animated sprite to draw.
 * @param x  Horizontal screen coordinate of the top-left corner.
 * @param y  Vertical   screen coordinate of the top-left corner.
 * @return 0 on success, non-zero on error.
 */
int draw_animated_sprite(AnimSprite *as, uint16_t x, uint16_t y);

/**
 * @brief Draws the current frame mirrored horizontally.
 *
 * @param as Animated sprite to draw flipped.
 * @param x  Horizontal screen coordinate of the top-left corner.
 * @param y  Vertical   screen coordinate of the top-left corner.
 * @return 0 on success, non-zero on error.
 */
int draw_animated_sprite_flipped(AnimSprite *as, uint16_t x, uint16_t y);

/**
 * @brief Advances the animation by one tick, stopping at the last frame.
 *
 * Suitable for one-shot actions such as attack or death sequences.
 * Use animated_sprite_finished() to detect completion.
 *
 * @param as Animated sprite to tick.
 */
void animate_sprite_once(AnimSprite *as);

/**
 * @brief Advances the animation by one tick, looping back to frame 0.
 *
 * Suitable for continuous animations such as walking or idle cycles.
 *
 * @param as Animated sprite to tick.
 */
void animate_sprite_loop(AnimSprite *as);

/**
 * @brief Returns whether a one-shot animation has fully completed.
 *
 * @param as Animated sprite to query.
 * @return true if the last frame has been displayed, false otherwise.
 */
bool animated_sprite_finished(AnimSprite *as);

/**
 * @brief Resets the animation back to the first frame.
 *
 * @param as Animated sprite to reset.
 */
void reset_animated_sprite(AnimSprite *as);

#endif /* __ANIMATED_SPRITE_H__ */
