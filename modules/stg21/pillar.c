#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "sound/sound.h"
#include "stg21/charge.h"
#include "stg21/explosion.h"
#include "stg21/pillar.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

enum pillar_mode { PILLAR_UPRIGHT, PILLAR_FALLING, PILLAR_DOWN };
#define NO_NEXT_MODE (-1)
#define NEXT_MODE(task) ((task)->work->submode)
/* the pillar's frame of pillar_motion, which its step moves on by fall_speed
 * while it falls */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FRAME(task) (*(Float *)&(task)->unk_3c)
#define LAST_FRAME (pillar_motion->nbFrame - 1)
/* whether at lies in this step's fall, at or past FRAME(task) and short of
 * FRAME(task) + fall_speed */
#define CROSSING(task, at)                                                     \
    (FRAME(task) <= (at) && FRAME(task) + fall_speed > (at))
#define LIVE_SHARDS(task) ((task)->unk_3c.l)
#define SHARDS 40
#define QUAKE_SOUND 7
#define SPLIT_SOUND 0
/* while a player is within SOLID_RANGE of the pillar's collision object, the
 * pillar's work keeps WORLD_ADDED. world_collect_near_players then can take
 * the object */
#define SOLID_RANGE 455.0F
#define CHARGE_ID(work) ((Uint8)(work)->ang.z)
/* the blast goes off BLAST_LIFT above the pillar's pos and BLAST_OFFSET along
 * its turned z. the impact's shake is centered impact_offset along the same
 * z, on the other side of pos */
#define BLAST_OFFSET 20.0F
#define BLAST_LIFT 40.0F
/* the bits of ang.x, any of which makes the pillar fall while
 * game_state.unk_09 is set */
#define FALL_BITS 0xf
#define STONE_MODELS 3
#define RUBBLE_MODELS 2

struct pillar_shard {
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *textures;
    Bool alive;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scale;
    NJS_VECTOR velocity;
    struct angles spin;
    int age;
    int life;
};

static void pillar_destroy(struct task *task);
static void pillar_step(struct task *task);
static void pillar_draw(struct task *task);
static void pillar_take_mode(struct task *task);
static void pillar_wait(struct task *task);
static void pillar_fall(struct task *task);
static void pillar_down(struct task *task);
static void shaft_init(struct task *task);
static void shaft_destroy(struct task *task);
static void shaft_step(struct task *task);
static void shaft_draw(struct task *task);
static void shards_create(struct task *task, NJS_POINT3 *origin);
static void shards_destroy(struct task *task);
static void shards_step(struct task *task);
static void shards_draw(struct task *task);

NJS_TEXLIST *ocean_pillar_texlists[] = {
    &pillar_shaft_texlist,       &pillar_whole_base_texlist,
    &pillar_broken_base_texlist, &pillar_stone_texlist,
    &pillar_rubble_texlist,      NULL,
};
static Float shard_gravity = 0.08F;
static Float shard_drag = 0.98F;
static Float spin_drag = 0.99F;
/* the shaft's capsule, which can hurt players. shaft_step registers it only in
 * the step whose fall brings the pillar's FRAME to impact_frame. pillar_fall
 * puts the fallen pillar's collision object in the world a step later */
/* clang-format off */
static struct collision_shape shaft_shape = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
    {0.0F, 182.5F, 0.0F}, 24.0F, 156.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Float impact_offset = -360.0F;
static Sint32 split_volume = 120;
static Sint32 quake_volume = 120;
static Sint32 echo_volume = 60;
static void (*pillar_steps[])(struct task *) = {pillar_wait, pillar_fall,
                                                pillar_down};
static Float blast_start = 7.5F;
static Float blast_end = 15.0F;
static Sint32 blast_life = 45;
static Float blast_shake = 60.0F;
static Sint32 blast_shake_steps = 3;
static Float fall_speed = 1.0F;
static Float first_quake_frame = 30.0F;
static Float first_shake = 65.0F;
static Sint32 first_shake_steps = 3;
static Float first_echo_frame = 54.0F;
static Float split_frame = 70.0F;
static Float impact_frame = 121.0F;
static Float impact_shake = 70.0F;
static Sint32 impact_shake_steps = 3;
static Float last_quake_frame = 134.0F;
static Float last_shake = 65.0F;
static Sint32 last_shake_steps = 3;
static Float last_echo_frame = 146.0F;
static Float shard_radius = 22.5F;
static Float shard_speed = 1.6F;
static Angle shard_spin = 0x380;
static Sint32 shard_life = 45;
static Float shard_size = 0.55F;
static Float fade_start = 0.75F;
static Float stone_scale = 0.8F;
static Float stone_speed = 1.275F;
static Float rubble_chance = 0.425F;
static Angle pitch_range = 0x1c00;
static Angle pitch_base = 0x1400;
KEPT static NJS_CNK_MODEL *stones[STONE_MODELS] = {
#include "stg21/pillar/stones.inc"
};
KEPT static NJS_CNK_MODEL *rubble[RUBBLE_MODELS] = {
#include "stg21/pillar/rubble.inc"
};

void ocean_pillar_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    if (task_create_child(TASK_WORK, shaft_init, task) == NULL) {
        task_kill(task);
        return;
    }
    task->draw = pillar_draw;
    task->step = pillar_step;
    task->destructor = pillar_destroy;
    /* @hack spelling: the mode set through task, where work would do */
    task->work->mode = PILLAR_UPRIGHT;
    /* @hack redundant: a next mode of NO_NEXT_MODE, which each arm below
     * overwrites */
    NEXT_MODE(task) = NO_NEXT_MODE;
    /* @hack spelling: pillar, a copy of task in each arm, where task would
     * do */
    if (placement_used(task)) {
        struct task *pillar = task;
        NEXT_MODE(pillar) = PILLAR_DOWN;
        FRAME(pillar) = LAST_FRAME;
    } else {
        struct task *pillar = task;
        NEXT_MODE(pillar) = PILLAR_UPRIGHT;
        FRAME(pillar) = 0.0F;
    }
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: writes through null where the world's pool has no object
     * to lend. the copy at 0x8c500a94 or 0x8c500ace writes through the result
     * of the call at 0x8c500a7c, which nothing tests. pillar_destroy tests
     * the same pointer at 0x8c500ba8 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the pillar and its shaft, and set.c makes it again on a later
     * pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    if (placement_used(task)) {
        *object = pillar_fallen_collision[0];
    } else {
        *object = pillar_upright_collision[0];
    }
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID, task, object);
}

static void pillar_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void pillar_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (!placement_try_leave(task)) {
        pillar_take_mode(task);
        /* @hack spelling: the mode read through task, where work would do */
        pillar_steps[task->work->mode](task);
        object = task->extra.p;
        if (player_near((NJS_POINT3 *)object->pos, SOLID_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void pillar_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the mode read through task at each use, where work would
     * do */
    if (task->work->mode == PILLAR_DOWN) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (task->work->mode == PILLAR_UPRIGHT) {
        njSetTexture(&pillar_whole_base_texlist);
        njCnkEasyDrawModel(&pillar_whole_base);
    } else {
        njSetTexture(&pillar_broken_base_texlist);
        njCnkEasyDrawModel(&pillar_broken_base);
    }
    njPopMatrixEx();
    if (task->work->mode == PILLAR_DOWN) {
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
}

static void pillar_take_mode(struct task *task)
{
    if (NEXT_MODE(task) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task);
        NEXT_MODE(task) = NO_NEXT_MODE;
    }
}

/* @hack spelling: the next mode set through a call, where an assignment at
 * each caller would do */
static void pillar_set_next_mode(struct task *task, int mode)
{
    NEXT_MODE(task) = mode;
}

static void pillar_wait(struct task *task)
{
    struct task_work *work = task->work;
    Float x;
    Float z;
    NJS_POINT3 pos;

    if (game_state.unk_09) {
        if (work->ang.x & FALL_BITS) {
            pillar_set_next_mode(task, PILLAR_FALLING);
        }
    } else if (!ocean_charge_count(CHARGE_ID(work))) {
        ocean_polar_xz(work->ang.y, BLAST_OFFSET, &x, &z);
        pos = work->pos;
        pos.x += x;
        pos.y += BLAST_LIFT;
        pos.z += z;
        ocean_explosion_create(&pos, blast_start, blast_end, blast_life);
        ocean_camera_shake_at(&pos, blast_shake, blast_shake_steps);
        shards_create(task, &pos);
        placement_set_used(task);
        pillar_set_next_mode(task, PILLAR_FALLING);
    }
}

static void pillar_fall(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR offset;
    NJS_OBJECT *object;

    if (CROSSING(task, first_quake_frame)) {
        ocean_camera_shake_at(&work->pos, first_shake, first_shake_steps);
        sound_play_at(QUAKE_SOUND, NULL, 0, quake_volume, &work->pos);
    }
    if (CROSSING(task, first_echo_frame)) {
        sound_play_at(QUAKE_SOUND, NULL, 0, echo_volume, &work->pos);
    }
    if (CROSSING(task, split_frame)) {
        sound_play_at(SPLIT_SOUND, NULL, 0, split_volume, &work->pos);
    }
    if (CROSSING(task, impact_frame)) {
        pos = work->pos;
        ocean_polar_xz(work->ang.y, impact_offset, &offset.x, &offset.z);
        pos.x += offset.x;
        pos.z += offset.z;
        ocean_camera_shake_at(&pos, impact_shake, impact_shake_steps);
        sound_play_at(QUAKE_SOUND, NULL, 0, quake_volume, &work->pos);
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
        /* @bug memory: writes through null where the world's pool has no
         * object to lend. after the free above, that happens only where
         * ocean_pillar_init's call found none. the copy at 0x8c500f94 writes
         * through the result of the call at 0x8c500f86, which nothing tests */
        task->extra.p = world_pool_alloc();
        object = task->extra.p;
        *object = pillar_fallen_collision[0];
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
        object->ang[0] = 0;
        object->ang[1] = work->ang.y;
        object->ang[2] = 0;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        world_add(WORLD_SOLID, task, object);
    }
    if (CROSSING(task, last_quake_frame)) {
        ocean_camera_shake_at(&work->pos, last_shake, last_shake_steps);
        sound_play_at(QUAKE_SOUND, NULL, 0, quake_volume, &work->pos);
    }
    if (CROSSING(task, last_echo_frame)) {
        sound_play_at(QUAKE_SOUND, NULL, 0, echo_volume, &work->pos);
    }
    if ((FRAME(task) += fall_speed) >= LAST_FRAME) {
        FRAME(task) = LAST_FRAME;
        pillar_set_next_mode(task, PILLAR_DOWN);
    }
}

static void pillar_down(struct task *task)
{
}

static void shaft_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
    struct task_work *work = task->work;

    task->draw = shaft_draw;
    task->step = shaft_step;
    task->destructor = shaft_destroy;
    work->pos = parent_work->pos;
    work->ang.x = 0;
    work->ang.y = parent_work->ang.y;
    work->ang.z = 0;
    collision_init(task, &shaft_shape, 1, COLLISION_OBJECT);
}

static void shaft_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void shaft_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
    struct task_work *work = task->work;
    Float pos[3];
    Angle ang[3];

    njSetCnkCurrentMotion(pillar_motion, FRAME(parent));
    njGetCnkMotionTranslate(pos, FALSE);
    njGetCnkMotionRotate(ang, FALSE);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&parent_work->pos);
    njRotateY(NULL, parent_work->ang.y);
    njTranslate(NULL, pos[0], pos[1], pos[2]);
    njRotateEx(ang, 0);
    njGetTranslation(NULL, &work->pos);
    current_matrix_angles_yxz(&work->ang.x, &work->ang.y, &work->ang.z);
    njPopMatrixEx();
    /* @hack spelling: the mode read through parent, where parent_work would
     * do */
    /* @hack spelling: the crossing test written out, ending in impact_frame <
     * FRAME(parent) + fall_speed, where CROSSING would do */
    if (parent->work->mode == PILLAR_FALLING && FRAME(parent) <= impact_frame &&
        impact_frame < FRAME(parent) + fall_speed) {
        collision_register(task);
    }
}

static void shaft_draw(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *parent_work = parent->work;
    struct task_work *work = task->work;

    if (parent_work->mode == PILLAR_DOWN) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&pillar_shaft_texlist);
    njCnkEasyDrawModel(&pillar_shaft);
    njPopMatrixEx();
    /* @hack spelling: the mode read through parent again, where parent_work
     * would do */
    if (parent->work->mode == PILLAR_DOWN) {
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
}

#define COIN_FLIP() (RANDOM_UNIT() < 0.5F)
#define RANDOM_SINE()                                                          \
    njSin((Angle)((RANDOM_UNIT() * (Float)HALF_TURN) - (Float)QUARTER_TURN))
/* a random factor from 0.9 to 1.1 */
#define JITTER() (0.9F + (RANDOM_UNIT() * 0.2F))
/* shard_spin times a random factor from 0.8 to 1.2 */
#define RANDOM_SPIN() ((Angle)(shard_spin * (0.8F + (RANDOM_UNIT() * 0.4F))))
/* shard_size times a random factor from 0.85 to 1.15 */
#define RANDOM_SIZE() (shard_size * (0.85F + (RANDOM_UNIT() * 0.3F)))
/* a color channel's greatest value */
#define CHANNEL_MAX 255
/* a packed ARGB color: alpha, then gray in red, green and blue */
#define ARGB_GRAY(alpha, gray)                                                 \
    (((Uint32)(alpha) << 24) | ((Uint32)(gray) << 16) |                        \
     ((Uint32)(gray) << 8) | (gray))

static void shards_create(struct task *task, NJS_POINT3 *origin)
{
    struct task *child;
    struct pillar_shard *shards;
    int i;
    struct angles ang;

    /* @hack redundant: an empty test of task, in a block no path reaches */
    if (0) {
        if (task != NULL) {
        }
    }
    /* @hack spelling: child assigned inside the test, where a statement of
     * its own would do */
    if ((child = task_create_child(TASK_WORK, shards_step, task)) != NULL) {
        /* @hack spelling: child->extra assigned inside the test, where a
         * statement of its own would do */
        if ((child->extra.p = syCalloc(SHARDS, sizeof(struct pillar_shard))) ==
            NULL) {
            task_kill(child);
            return;
            /* @hack redundant: an empty test of shard_spin, which the return
             * skips */
            if (shard_spin) {
            }
        }
        child->draw = shards_draw;
        child->destructor = shards_destroy;
        for (i = 0, shards = child->extra.p; i < SHARDS; i++, shards++) {
            /* @hack spelling: shard, a copy of shards, where shards would do */
            struct pillar_shard *shard = shards;
            Float size;
            Float spread = RANDOM_SINE();

            /* @hack spelling: the half turn toggled in by ^, where + would
             * give njRotateY the same angle */
            ang.y = (Angle)((spread * (Float)QUARTER_TURN) +
                            camera_views[camera_current_index]->ang.y) ^
                    HALF_TURN;
            ang.x = (Angle)((RANDOM_SINE() * pitch_range) + pitch_base);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, ang.y);
            njRotateX(NULL, ang.x);
            njPushMatrixEx();
            njTranslate(NULL, 0.0F, 0.0F, -shard_radius * JITTER());
            njGetTranslation(NULL, &shard->pos);
            njPopMatrixEx();
            shard->pos.x += origin->x;
            shard->pos.y += origin->y;
            shard->pos.z += origin->z;
            njPushMatrixEx();
            njTranslate(NULL, 0.0F, 0.0F, -shard_speed * JITTER());
            njGetTranslation(NULL, &shard->velocity);
            njPopMatrixEx();
            shard->ang.x = RANDOM_TURN();
            shard->ang.y = RANDOM_TURN();
            shard->ang.z = RANDOM_TURN();
            shard->spin.x = RANDOM_SPIN();
            if (COIN_FLIP()) {
                shard->spin.x = -shard->spin.x;
            }
            shard->spin.y = RANDOM_SPIN();
            if (COIN_FLIP()) {
                shard->spin.y = -shard->spin.y;
            }
            shard->spin.z = RANDOM_SPIN();
            if (COIN_FLIP()) {
                shard->spin.z = -shard->spin.z;
            }
            size = RANDOM_SIZE();
            shard->scale.x = size * JITTER();
            shard->scale.y = size * JITTER();
            shard->scale.z = size * JITTER();
            shard->age = 0;
            shard->life = shard_life;
            shard->alive = TRUE;
            if (RANDOM_UNIT() < rubble_chance) {
                shard->model = rubble[(int)(RANDOM_UNIT() * RUBBLE_MODELS)];
                shard->textures = &pillar_rubble_texlist;
            } else {
                shard->model = stones[(int)(RANDOM_UNIT() * STONE_MODELS)];
                shard->textures = &pillar_stone_texlist;
                shard->velocity.x *= stone_speed;
                shard->velocity.y *= stone_speed;
                shard->velocity.z *= stone_speed;
                shard->scale.x *= stone_scale;
                shard->scale.y *= stone_scale;
                shard->scale.z *= stone_scale;
            }
            njPopMatrixEx();
        }
        /* @hack redundant: two empty tests, a njPopMatrixEx and a store to
         * stone_scale, in a block no path reaches */
        if (0) {
            if (shard_spin) {
            }
            njPopMatrixEx();
            stone_scale = stone_speed;
            if (child != NULL) {
            }
        }
        LIVE_SHARDS(child) = SHARDS;
    }
}

static void shards_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void shards_step(struct task *task)
{
    struct pillar_shard *shards;
    int i;

    for (i = 0, shards = task->extra.p; i < SHARDS; i++, shards++) {
        /* @hack spelling: shard, a copy of shards, where shards would do */
        struct pillar_shard *shard = shards;
        if (shard->alive) {
            shard->pos.x += shard->velocity.x;
            shard->pos.y += shard->velocity.y;
            shard->pos.z += shard->velocity.z;
            shard->velocity.y -= shard_gravity;
            shard->velocity.x *= shard_drag;
            shard->velocity.y *= shard_drag;
            shard->velocity.z *= shard_drag;
            shard->ang.x += shard->spin.x;
            shard->ang.y += shard->spin.y;
            shard->ang.z += shard->spin.z;
            shard->spin.x = (Angle)(shard->spin.x * spin_drag);
            shard->spin.y = (Angle)(shard->spin.y * spin_drag);
            shard->spin.z = (Angle)(shard->spin.z * spin_drag);
            /* @hack redundant: a second test of shard->alive, which the first
             * passed */
            if (shard->alive) {
                if (++shard->age >= shard->life) {
                    shard->alive = FALSE;
                    LIVE_SHARDS(task)--;
                }
            }
        }
    }
    if (LIVE_SHARDS(task) <= 0) {
        task_kill(task);
    }
}

static void shards_draw(struct task *task)
{
    struct pillar_shard *shards;
    int i;

    /* @hack spelling: shards = shards + 1, where shards++ would do */
    for (i = 0, shards = task->extra.p; i < SHARDS; i++, shards = shards + 1) {
        /* @hack spelling: shard, a copy of shards, where shards would do */
        struct pillar_shard *shard = shards;
        NJS_VECTOR scale;
        Float age;
        Float remaining;
        Uint8 alpha;
        Uint8 shade;
        Uint32 color;

        if (shard->alive) {
            age = (Float)shard->age / (Float)shard->life;
            /* @hack spelling: the scale copied a component at a time, where
             * scale = shard->scale would do */
            scale.x = shard->scale.x;
            scale.y = shard->scale.y;
            scale.z = shard->scale.z;
            remaining = 1.0F - age;
            if (age < fade_start) {
                alpha = CHANNEL_MAX;
            } else {
                alpha = remaining / (1.0F - fade_start) * (Float)CHANNEL_MAX;
            }
            shade = remaining * (Float)CHANNEL_MAX;
            color = ARGB_GRAY(alpha, shade);
            njPushMatrixEx();
            njTranslateEx(&shard->pos);
            njRotateXYZ(NULL, shard->ang.x, shard->ang.y, shard->ang.z);
            draw_control_save();
            draw_attr_save();
            draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
            njSetTexture(shard->textures);
            ocean_model_draw_tinted_scaled_v(shard->model, color, &scale);
            draw_attr_restore();
            draw_control_restore();
            njPopMatrixEx();
        }
    }
}
