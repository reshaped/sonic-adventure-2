/* the laser barrier of Crazy Gadget, Eternal Engine and Lost Colony. it plays
 * its contact sound at the sound's own volume. Cannon's Core's barrier sets
 * that volume to contact_volume */
#include <ninja.h>
#include "collision/collision.h"
#include "shared/core_laser.h"
#include "shared/colony_laser/text.inc"
#include "shared/core_laser_names.h"

NJS_TEXLIST core_laser_texlist = {
#include "shared/colony_laser/core_laser_texlist.inc"
};

#include "shared/core_laser_art.h" /* IWYU pragma: keep -- the data it defines here */

NJS_TEXLIST *core_laser_texlists[2] = {
#include "shared/colony_laser/core_laser_texlists.inc"
};

/* clang-format off */
static struct collision_shape contact_shape[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(3) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* its init sets the radius, the sizes and the angles from the placement */
static struct collision_shape barrier_shape[1] = {
    {0, SHAPE_BOX2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(3) | SHAPE_HURTS_ALL, SHAPE_UNTURNED | SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 10.0F, 2.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

#define LASER_DEFAULT_VOLUME

#include "shared/core_laser_impl.h" /* IWYU pragma: keep -- the code it compiles here */
