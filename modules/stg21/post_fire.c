#include <ninja.h>

#include "collision/collision.h"
#include "shared/wall_fire.h" /* IWYU pragma: keep -- names shared/wall_fire.c takes */
#include "stg21/fire_fixture.h"

#define FIRE_INIT post_fire_init
#define FIRE_HOLDER post_fire_model
#define FIRE_TEXLIST post_fire_texlist
#define FIRE_FLAME wall_fire_flame_model
#define FIRE_HAS_POST
#define FLAME_Y 35.0F
#define FLAME_Z 0.0F
#define FLAME_SCALE 0.5F
#define SOUND_HEIGHT 40.0F

NJS_TEXLIST *post_fire_texlists[] = {&post_fire_texlist, NULL};

/* clang-format off */
static struct collision_shape fixture_shapes[] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 35.0F, 0.0F}, 7.5F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL | PUSH_FIRM(PUSH_ALL), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, 15.0F, 0.0F}, 7.5F, 15.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

#include "shared/wall_fire.c" /* NOLINT(bugprone-suspicious-include) */
