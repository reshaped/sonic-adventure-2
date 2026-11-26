/* a seed in the garden: a pickup that the player holds, and a Chao carries
 * and plants */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/carry.h"
#include "chao/egg.h"
#include "chao/movement.h"
#include "chao/orchard_art.h"
#include "chao/pickup.h"
#include "chao/planting.h"
#include "chao/seed.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/task.h"
#include "system/util.h"

#define SEED_DRAW_CONTROL                                                      \
    (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
/* a step slower than STILL_SPEED is a still step. a roll ends after more
 * than STILL_STEPS still steps in a row */
#define STILL_SPEED 0.01F
#define STILL_STEPS 30
/* a dropped seed's shapes stay off for DROP_FRAMES frames */
#define DROP_FRAMES 30
#define SEED_SCALE 1.5F
static const struct collision_shape seed_shapes[PICKUP_SHAPES] =
    GARDEN_PICKUP_SHAPES(PUSH_ENEMIES | PUSH_OBJECTS, 0.8F,
                         (Sint8)GARDEN_SEED_HIT, -4, 8);

static void seed_rest(struct task *seed)
{
    struct task_work *work = seed->work;
    /* @hack spelling: a switch of one case on submode, here and in each
     * state below, where if (work->submode == 0) would do */
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(seed);
        work->submode++;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = PICKUP_HELD;
        work->submode = 0;
        return;
    }
    if (collision_pushed(seed)) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
}
static void seed_roll(struct task *seed)
{
    struct task_work *work = seed->work;
    struct mover *mover = seed->motion.p;
    NJS_VECTOR horizontal;
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(seed);
        work->submode++;
        work->timer = 0;
        break;
    }
    mover_step(seed);
    mover_meet_stage(seed);
    if (work->flags & MOVER_WORK_ON_FLOOR) {
        horizontal.x = mover->spd.x;
        horizontal.y = 0;
        horizontal.z = mover->spd.z;
        /* @bug logic: a rolling seed never returns to PICKUP_REST. a still
         * step that doesn't settle the seed zeroes the count of still steps,
         * at 0x0c528890. the count goes from 0 to 1 and back, and never
         * passes STILL_STEPS */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. a moving step zeroes the
         * count, and still steps add to it until the seed rests */
        if (njScalor(&horizontal) < STILL_SPEED) {
            if (work->timer++ > STILL_STEPS) {
                mover_clear_spd(seed);
                mover_clear_acc(seed);
                work->mode = PICKUP_REST;
                work->submode = 0;
            }
        } else {
            work->timer = 0;
        }
#else
        if (njScalor(&horizontal) < STILL_SPEED) {
            if (work->timer++ > STILL_STEPS) {
                mover_clear_spd(seed);
                mover_clear_acc(seed);
                work->mode = PICKUP_REST;
                work->submode = 0;
            } else {
                work->timer = 0;
            }
        }
#endif
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = PICKUP_HELD;
        work->submode = 0;
    }
}
static void seed_drop(struct task *seed)
{
    struct task_work *work = seed->work;
    switch (work->submode) {
    case 0:
        PICKUP_HIDE_SHAPES(seed);
        work->submode++;
        break;
    }
    mover_step(seed);
    mover_meet_stage(seed);
    if (work->timer++ > DROP_FRAMES) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
}
static void seed_held(struct task *seed)
{
    struct task_work *work = seed->work;
    struct mover *mover = seed->motion.p;
    switch (work->submode) {
    case 0:
        PICKUP_HELD_SHAPES(seed);
        mover->aim_y = work->ang.y + player_work[0]->ang.y;
        garden_separate_partners(seed);
        work->submode++;
        break;
    }
    work->ang.y = -player_work[0]->ang.y + mover->aim_y;
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        if (garden_object_locked(seed)) {
            work->mode = PICKUP_CARRIED;
            work->submode = 0;
        } else {
            work->mode = PICKUP_ROLL;
            work->submode = 0;
        }
    }
}
static void seed_carried(struct task *seed)
{
    struct task_work *work = seed->work;
    garden_take_command(seed);
    switch (work->submode) {
    case 0:
        PICKUP_CARRIED_SHAPES(seed);
        work->submode++;
        work->timer = 0;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        garden_separate_partners(seed);
        work->mode = PICKUP_HELD;
        work->submode = 0;
    }
    if (!garden_follow_chao(seed)) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
}
static void seed_step(struct task *seed)
{
    struct task_work *work = seed->work;
    switch (work->mode) {
    case PICKUP_REST:
        seed_rest(seed);
        break;
    case PICKUP_DROP:
        seed_drop(seed);
        break;
    case PICKUP_ROLL:
        seed_roll(seed);
        break;
    case PICKUP_HELD:
        seed_held(seed);
        break;
    case PICKUP_CARRIED:
        seed_carried(seed);
        break;
    }
    collision_register(seed);
}
static void seed_draw(struct task *seed)
{
    struct task_work *work = seed->work;
    if (garden_visible_at_height(seed, 1.5F, 1.0F)) {
        njSetTexture(&egg_texlist);
        njPushMatrix(NULL);
        if (work->flags & WORK_HELD_BY_PLAYER) {
            njTranslate(NULL, 0, -0.25F, 0);
        }
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njScale(NULL, SEED_SCALE, SEED_SCALE, SEED_SCALE);
        draw_control_set(SEED_DRAW_CONTROL);
        njCnkEasyDrawModel(garden_seed_models[SEED_KIND(work)]->model);
        draw_control_clear(SEED_DRAW_CONTROL);
        if (((struct garden_entry *)seed->force.p)->distance <
            chao_tuning.full_shadow_distance) {
            njScale(NULL, 0.38F, 0.7F, 0.38F);
            njTranslate(NULL, 0, 0.1F, 0);
            njCnkModDrawModel(&garden_shadow_model);
        }
        njPopMatrix(1);
    }
}
static void seed_destroy(struct task *seed)
{
    garden_unregister_object(seed);
}
static void seed_init(struct task *seed)
{
    struct task_work *work = seed->work;
#ifdef NONMATCHING
    /* this side takes the garden slot that garden_prepare_pickup reserved. it
     * destroys a seed whose reservation clear_entries has emptied since. its
     * shapes were made with the reservation */
    if (!garden_take_pickup_reservation(seed)) {
        task_destroy(seed);
        return;
    }
#else
    collision_init(seed, seed_shapes, PICKUP_SHAPES, COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(work, seed->motion.p, FALL_KIND_NORMAL);
    seed->step = seed_step;
    seed->destructor = seed_destroy;
    seed->draw = seed_draw;
    work->mode = PICKUP_DROP;
    work->submode = 0;
    garden_register_object(GARDEN_OBJECT_SEED, seed, 0);
    garden_set_carry_height(seed, 0.2F);
    garden_set_carry_radius(seed, 0.35F);
}
struct task *garden_seed_create(int kind, NJS_POINT3 *position, Angle yaw,
                                NJS_VECTOR *velocity)
{
#ifdef NONMATCHING
    /* this side makes the task, its mover and its shapes by
     * garden_prepare_pickup, which also reserves a garden slot. it makes no
     * seed where any of them fails */
    struct task *seed = garden_prepare_pickup(GARDEN_OBJECT_SEED, 2, seed_init,
                                              "ALO_Seed", seed_shapes);
    struct task_work *work;
    struct mover *mover;
    if (seed == NULL) {
        return NULL;
    }
    work = seed->work;
    mover = seed->motion.p;
#else
    struct task *seed = task_create(TASK_WORK, 2, seed_init, "ALO_Seed");
    struct task_work *work = seed->work;
    struct mover *mover = mover_alloc(seed);
#endif
    work->pos = *position;
    if (velocity) {
        mover->spd = *velocity;
    }
    SEED_KIND(work) = kind;
    mover_set_gravity(seed, -0.05F);
    mover->probe_height = 3.0F;
    mover->floor_bounce = 0.7F;
    mover->friction = 0.7F;
    mover->hold_turn = 0x1800;
    return seed;
}
void fun_0c528d24(void) /* NOLINT(misc-use-internal-linkage) */
{
}
