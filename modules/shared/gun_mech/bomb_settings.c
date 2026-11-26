/* the beam's bomb: its texture animation, which scrolls its strips' uvs, its
 * tuning and its collision shape */
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "shared/gun_mech/bomb_art.h"

static Sint16 bomb_uvs[474] = {
#include "shared/gun_mech/bomb_settings/bomb_uvs.inc"
};

struct texture_anim mech_boss_beam_bomb_anim = {
#include "shared/gun_mech/bomb_settings/mech_boss_beam_bomb_anim.inc"
};

struct bomb_tuning mech_boss_beam_bomb_tuning = {
    0.8F, 0.1F, 0.1F, 1.0F, 0.07F, 0.04F, 0.07F, 0.08F, {0.0F, 0.0F}};

struct collision_shape mech_boss_beam_bomb_shape = {
    0,
    SHAPE_SPHERE,
    PUSH_PLAYERS,
    SHAPE_ATTACK_LEVEL(3) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_PLAYERS |
        SHAPE_HURTS_OBJECTS,
    SHAPE_HURTABLE,
    {0.0F, 0.0F, 0.0F},
    15.0F,
    0.0F,
    0.0F,
    0,
    {0, 0, 0}};
