/* the mechs' lock-on assets: the shot's shape and sprite, the blast, the
 * marker and the laser's animations */
#ifndef PLAYER_WALKER_LOCK_ASSETS_H
#define PLAYER_WALKER_LOCK_ASSETS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"

extern struct collision_shape walker_lock_shot_shape;
/* the blast's four meshes in Eggman's green, then the same four in Tails'
 * orange */
extern NJS_CNK_MODEL *walker_blast_parts[8];
/* @hack type: an array of one sprite, where one NJS_SPRITE would do */
extern NJS_SPRITE walker_shot_sprite[1];
extern NJS_SPRITE walker_lock_marker;
/* the lock marker's cells, by walker.c's LOCK_CELL_ numbers: the marker, the
 * count's backdrop and a digit. the draw sets the digit cell's texture to
 * each digit it draws */
extern NJS_TEXANIM walker_lock_cells[3];
/* the entries of walker_lock_scales, a power of two. the marker's draw masks
 * its index with LOCK_SCALES - 1 */
#define LOCK_SCALES 8
extern Float walker_lock_scales[LOCK_SCALES];
extern struct texture_anim laser_anim_a;
extern struct texture_anim laser_anim_b;
extern struct texture_anim laser_anim_c;

#endif
