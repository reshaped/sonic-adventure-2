#include <ninja.h>

#include "collision/collision.h"
#include "stg21/fire_fixture.h"

#define FIRE_INIT chain_fire_init
#define FIRE_HOLDER chain_fire_model
#define FIRE_TEXLIST chain_fire_texlist
#define FIRE_FLAME chain_fire_flame_model
#define FLAME_Y 36.0F
#define FLAME_Z 0.0F
#define FLAME_SCALE 2.5F
#define SOUND_HEIGHT 40.0F

NJS_TEXLIST *chain_fire_texlists[] = {&chain_fire_texlist, NULL};

/* clang-format off */
static struct collision_shape fixture_shapes[] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 20.0F, 0.0F}, 20.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

#include "shared/wall_fire.c" /* NOLINT(bugprone-suspicious-include) */
