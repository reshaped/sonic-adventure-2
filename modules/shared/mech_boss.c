/* the mech bosses' helpers: the collision, textures and effects that Big Foot,
 * Hot Shot and Flying Dog share. their texture files call the three Grobo */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/mover.h"
#include "shared/black_smoke.h"
#include "shared/boss.h"
#include "shared/dust.h"
#include "shared/mech_boss.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#define SMOKE_SCALE 5.0F

void mech_boss_ring(NJS_POINT3 *pos, struct mech_boss_ring *spec)
{
    NJS_VECTOR spd;
    NJS_POINT3 offset = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 puff;
    NJS_VECTOR puff_spd;
    Float scale;
    Angle step;
    Angle start;
    Angle ang;
    /* @hack type: i, a Uint8, where an int would do */
    Uint8 i;

    scale = spec->scale;
    ang = 0;
    /* @bug uninitialized: a count other than 16, 8, 6 or 4 leaves step
     * unset, and the bosses pass 10 and 15 (the FLOAT at 0x76 of the
     * object's code reads step) */
#ifdef NONMATCHING
    /* this side repairs the tag above: for a count the switch doesn't name,
     * it spaces the puffs evenly. the image's step for such a count is
     * whatever the caller left in R13, which C can't give */
    step = spec->count != 0 ? FULL_TURN / spec->count : 0;
#endif
    switch (spec->count) {
    case 16:
        step = FULL_TURN / 16;
        break;
    case 8:
        step = FULL_TURN / 8;
        break;
    case 6:
        step = FULL_TURN / 6;
        break;
    case 4:
        step = FULL_TURN / 4;
        break;
    }
    start = rand() * (Float)step * RAND_SCALE;
    offset.z = spec->radius;
    spd = spec->spd;
    spd.x *= scale;
    spd.y *= scale;
    spd.z *= scale;
    for (i = 0; i < spec->count; i++) {
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, start + ang);
        njCalcPoint(NULL, &offset, &puff);
        njCalcVector(NULL, &spd, &puff_spd);
        puff.x += pos->x;
        puff.y += pos->y;
        puff.z += pos->z;
        dust_create(&puff, &puff_spd, scale);
        ang += step;
        njPopMatrix(1);
    }
}

void mech_boss_guard_all(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_shape *shape = work->collision->shapes;
    /* @hack type: i, a Sint8, where an int would do */
    Sint8 i;

    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < work->collision->count; i++) {
        shape->levels = (shape->levels & ~SHAPE_ATTACK) | SHAPE_ATTACK_LEVEL(0);
        shape->levels = (shape->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(3);
        shape++;
    }
}

void mech_boss_set_levels(struct task *task, struct mech_boss_levels *levels)
{
    struct task_work *work = task->work;
    /* @hack spelling: collision, a local for the record, where
     * work->collision read in place would do */
    struct collision_work *collision = work->collision;
    struct collision_shape *shape = collision->shapes;
    /* @hack type: i, a Sint8, where an int would do */
    Sint8 i;

    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < work->collision->count; i++) {
        shape->levels =
            (shape->levels & ~SHAPE_ATTACK) | (levels->attack & SHAPE_ATTACK);
        shape->levels = (shape->levels & ~SHAPE_GUARD) |
                        ((levels->guard << SHAPE_GUARD_SHIFT) & SHAPE_GUARD);
        levels++;
        shape++;
    }
}

void mech_boss_set_shape_angles(struct task *task, Angle x, Angle y, Angle z)
{
    struct task_work *work = task->work;
    struct collision_shape *shape = work->collision->shapes;
    /* @hack type: i, a Sint8, where an int would do */
    Sint8 i;

    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < work->collision->count; i++) {
        shape->ang.x = x;
        shape->ang.y = y;
        shape->ang.z = z;
        shape++;
    }
}

void mech_boss_count(Uint8 *counter, Uint8 count)
{
    (*counter)++;
    if (*counter == count) {
        *counter = 0;
    }
}

void mech_boss_smoke_points(struct mech_boss_smoke *smoke, NJS_POINT3 *points)
{
    NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
    NJS_VECTOR down = {0.0F, -1.0F, 0.0F};
    /* @hack type: i, a Sint8, where an int would do */
    Sint8 i;

    njPushMatrix(&_nj_unit_matrix_);
    /* @hack spelling: ang declared in a block of its own, where the
     * function's top would do */
    {
        /* @hack spelling: ang, a copy of the frame's angles, where
         * smoke->ang read in place would do */
        struct angles ang;

        ang = smoke->ang;
        njRotateY(NULL, ang.y);
        njRotateZ(NULL, ang.z);
        njRotateX(NULL, ang.x);
    }
    for (i = 0; i < smoke->count; i++) {
        NJS_POINT3 below;
        NJS_POINT3 pos;

        njPushMatrixEx();
        njTranslateV(NULL, points);
        njCalcPoint(NULL, &down, &below);
        njPopMatrixEx();
        pos = smoke->pos;
        pos.x += below.x;
        pos.y += below.y;
        pos.z += below.z;
        black_smoke_rising_create(&pos, &up, SMOKE_SCALE);
        points++;
    }
    njPopMatrix(1);
}

void mech_boss_textures_load(Sint8 boss)
{
    texture_load_pvm("grobo_tex", &grobo_texlist);
    texture_load_pvm("grobo_title_mojitex", &grobo_title_moji_texlist);
    switch (boss) {
    case MECH_BOSS_BIG_FOOT:
        texture_load_pvm("bigfoot_title_tex", &bigfoot_title_texlist);
        break;
    case MECH_BOSS_HOT_SHOT:
        texture_load_pvm("hotshot_title_tex", &hotshot_title_texlist);
        break;
    case MECH_BOSS_FLYING_DOG:
        texture_load_pvm("fdog_title_tex", &fdog_title_texlist);
        break;
    }
    texture_load_pvm("boss_pwguage_tex", &boss_gauge_texlist);
    texture_load_pvm("grobo_eff2_tex", &grobo_eff2_texlist);
}

void mech_boss_textures_release(Sint8 boss)
{
    njReleaseTexture(&grobo_texlist);
    njReleaseTexture(&grobo_title_moji_texlist);
    switch (boss) {
    case MECH_BOSS_BIG_FOOT:
        njReleaseTexture(&bigfoot_title_texlist);
        break;
    case MECH_BOSS_HOT_SHOT:
        njReleaseTexture(&hotshot_title_texlist);
        break;
    case MECH_BOSS_FLYING_DOG:
        njReleaseTexture(&fdog_title_texlist);
        break;
    }
    njReleaseTexture(&boss_gauge_texlist);
    njReleaseTexture(&grobo_eff2_texlist);
}

Bool mech_boss_pose_for_winner(struct task *task, NJS_POINT3 *pos)
{
    struct task_work *work = task->work;

    if (game_state.winner_posed == 1) {
        work->pos = *pos;
        work->ang.y = mover_turn_to_player(task, 0);
        return TRUE;
    }
    return FALSE;
}
