/* the hornet, E NAMIE in a stage's object list: an enemy that carries bombs.
 * it hovers, and lets its bombs loose at a player ahead of it */
#include <ninja.h>

#include "shared/hornet_art.h"
#include "shared/hornet_shapes.h" /* IWYU pragma: keep -- the data it defines here */

/* The models each kind breaks into, null-ended for enemy_break. */
static NJS_CNK_MODEL *hornet_3_pieces[5] = {
#include "shared/hornet/hornet_3_pieces.inc"
};
static NJS_CNK_MODEL *hornet_6_pieces[5] = {
#include "shared/hornet/hornet_6_pieces.inc"
};
static NJS_CNK_MODEL *hornet_9_pieces[5] = {
#include "shared/hornet/hornet_9_pieces.inc"
};
static NJS_CNK_MODEL *laser_hornet_pieces[6] = {
#include "shared/hornet/laser_hornet_pieces.inc"
};

#define BOMBS_MAX BOMBS_9
#include "shared/hornet_actor.h"

static NJS_POINT3 laser_muzzles[LASERS] = {
#include "shared/hornet/laser_muzzles.inc"
};
