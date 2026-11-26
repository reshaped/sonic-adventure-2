/* the capsule bomb's code. shared/capsule_bomb.c compiles it, and
 * stg27/capsule_bomb.c compiles Lost Colony's with COLONY_BOMB defined. the
 * includer defines ACTIVE_SHAPES and the bomb's art and shapes first */
#ifndef SHARED_CAPSULE_BOMB_ACTOR_H
#define SHARED_CAPSULE_BOMB_ACTOR_H

#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/surface.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/earth_shake.h"
#include "shared/four_valve_tank.h"
#include "shared/item_box.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define SHARD_FLAGS (DEBRIS_FIELD | DEBRIS_FADE_COLORS)
#define BURST_SOUND 0x1011
#define BURST_PRIORITY 0
#define BURST_VOLUME 0
#define BLAST_PRIORITY 0
#ifdef COLONY_BOMB
#define BLAST_SOUND 11
#define BLAST_VOLUME 80
/* what the blast and its aftershock add to colony_flash */
#define BLAST_FLASH 0.3F
#define AFTERSHOCK_FLASH 3.0F
#else
#define BLAST_SOUND 21
#define BLAST_VOLUME 0
#endif
/* outside Lost Colony, the blast shakes player 0's pad */
#define BLAST_POWER 5
#define BLAST_PULSES 4
#define BLAST_INTERVAL 10
#define BLAST_FREQ 60

/* the bomb draws only where the current matrix takes its pos to a z from
 * NEAR_Z to FAR_Z */
#define NEAR_Z (-50.0F)
#define FAR_Z 500.0F
/* the capsule draws whole while it rides, and for the first
 * WHOLE_FUSE_STEPS steps of the fuse */
#define WHOLE_FUSE_STEPS 1

static void damage_step(struct task *task);
static void capsule_bomb_destroy(struct task *task);
static void capsule_bomb_step(struct task *task);
static void capsule_bomb_draw(struct task *task);
static void capsule_bomb_draw_built(struct task *task);

Uint8 *capsule_bomb_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, capsule_bomb_builds, BUILDS) == BUILDS) {
        capsule_bomb_built = TRUE;
    }
    return buf;
}

static void damage_step(struct task *task)
{
    struct task_work *work = task->work;

    DAMAGE_RADIUS(work) += damage_growth;
    if (DAMAGE_RADIUS(work) > BLAST_REACH) {
        task_kill(task);
    } else {
        work->collision->shapes->radius = DAMAGE_RADIUS(work);
        collision_fit_radius(task);
        collision_register(task);
    }
}

void capsule_bomb_init(struct task *task)
{
    struct task_work *work = task->work;
#ifndef COLONY_BOMB
    Float half;
#endif

    if (placement_try_leave(task)) {
        return;
    }
    if (capsule_bomb_built) {
        task->draw = capsule_bomb_draw_built;
    } else {
        task->draw = capsule_bomb_draw;
    }
    task->step = capsule_bomb_step;
    task->destructor = capsule_bomb_destroy;
    TRACK_MIDDLE(work) = work->pos.y;
    weak_point_create(task, WEAK_POINT_INDEX, &work->pos);
    collision_init(task, capsule_bomb_shapes, ACTIVE_SHAPES, COLLISION_OBJECT);
#ifndef COLONY_BOMB
    if (work->collision) {
        half = HALF_TRACK(work);
        work->collision->shapes[TOP_CAP].center.x = work->pos.x;
        work->collision->shapes[TOP_CAP].center.y = TRACK_MIDDLE(work) + half;
        work->collision->shapes[TOP_CAP].center.z = work->pos.z;
        work->collision->shapes[BOTTOM_CAP].center.x = work->pos.x;
        work->collision->shapes[BOTTOM_CAP].center.y =
            TRACK_MIDDLE(work) - half;
        work->collision->shapes[BOTTOM_CAP].center.z = work->pos.z;
        collision_fit_radius(task);
    }
#endif
    if (task->placement && placement_get_state(task)) {
        work->mode = MODE_GONE;
    }
}

static void capsule_bomb_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->mode == MODE_GONE) {
        placement_set_state(task, GONE_STATE);
    }
}

/* @hack spelling: the checks written out at both ends by macros, where a
 * function taking the end test would do */
#ifdef COLONY_BOMB
/* Lost Colony's bomb has no weak point while it waits past its track's far
 * end */
#define REMOVE_STOPPED_WEAK_POINT(task)                                        \
    do {                                                                       \
        if ((task)->child) {                                                   \
            task_kill((task)->child);                                          \
        }                                                                      \
    } while (0)
#define CHECK_CAP_WEAK_POINT(task, work, past_end) ((void)0)
#else
#define REMOVE_STOPPED_WEAK_POINT(task) ((void)0)
/* outside Lost Colony, the bomb has no weak point from its track's far end
 * until it starts again */
#define CHECK_CAP_WEAK_POINT(task, work, past_end)                             \
    do {                                                                       \
        if (past_end) {                                                        \
            if ((task)->child) {                                               \
                task_kill((task)->child);                                      \
            }                                                                  \
            /* @hack redundant: the test that mode is 0, which the step's      \
             * test for MODE_RIDE has settled */                               \
        } else if (!(task)->child && !(work)->timer && !(work)->mode &&        \
                   !(work)->submode) {                                         \
            weak_point_create((task), WEAK_POINT_INDEX, &(work)->pos);         \
        }                                                                      \
    } while (0)
#endif

static void capsule_bomb_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *damage;
    struct angles floor_ang;
    struct angles ang;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float *axis;
    Float floor;
    Float end;
    int i;
    int j;
    Angle turn;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_RIDE:
        if (work->submode) {
            floor_ang.x = 0;
            floor_ang.y = work->ang.y;
            floor_ang.z = 0;
            work->mode = MODE_FUSE;
            work->timer = 0;
            sound_play(BURST_SOUND, NULL, BURST_PRIORITY, BURST_VOLUME);
            floor = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z,
                                        &floor_ang);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            for (j = 0; j < SHARD_TURNS; j++) {
                turn = j * EIGHTH_TURN;
                for (i = 0; i < SHARD_ROWS; i++) {
                    spd.x = njSin(turn);
                    spd.z = njCos(turn);
                    spd.y = ((i - SHARD_MIDDLE_ROW) * SHARD_RISE) +
                            (rand() * (SHARD_LIFT * RAND_SCALE));
                    pos.x = spd.x * SHARD_OUT;
                    pos.y = (i * SHARD_ROW_HEIGHT) - SHARD_LOW;
                    pos.z = spd.z * SHARD_OUT;
                    ang.x = 0;
                    ang.y = work->ang.y + turn;
                    ang.z = 0;
                    njCalcPoint(NULL, &pos, &pos);
                    njCalcVector(NULL, &spd, &spd);
                    /* @hack spelling: axis, a pointer that walks spd's x, y
                     * and z, where the three named in turn would do */
                    axis = &spd.x;
                    *axis++ *=
                        (rand() * (SHARD_SPREAD * RAND_SCALE)) + SHARD_SPEED;
                    *axis++ *=
                        (rand() * (SHARD_SPREAD * RAND_SCALE)) + SHARD_SPEED;
                    *axis *=
                        (rand() * (SHARD_SPREAD * RAND_SCALE)) + SHARD_SPEED;
                    if ((j + i) & 1) {
                        debris_launch_built_flags(
                            &pos, &ang, floor, &spd, capsule_bomb_shard_a_model,
                            capsule_bomb_builds[BUILD_SHARD_A].built,
                            &capsule_bomb_shard_a_texlist, SHARD_FLAGS);
                    } else {
                        debris_launch_built_flags(
                            &pos, &ang, floor, &spd, capsule_bomb_shard_b_model,
                            capsule_bomb_builds[BUILD_SHARD_B].built,
                            &capsule_bomb_shard_b_texlist, SHARD_FLAGS);
                    }
                }
            }
#ifdef COLONY_BOMB
            /* @hack redundant: a push no path reaches */
            if (0) {
                njPushMatrixEx();
            }
#endif
            njPopMatrixEx();
        }
#ifdef COLONY_BOMB
        else if (!task->child && !work->timer) {
            weak_point_create(task, WEAK_POINT_INDEX, &work->pos);
        }
#endif
        break;
    case MODE_FUSE:
        if (work->timer++ > FUSE_STEPS) {
            sound_play(BLAST_SOUND, NULL, BLAST_PRIORITY, BLAST_VOLUME);
            work->mode = MODE_GONE;
            work->timer = 0;
            blast_create(BLAST_HUGE, &work->pos);
            damage = task_create(TASK_WORK, DAMAGE_TASK_LEVEL, damage_step,
                                 damage_task_name);
            if (damage) {
                collision_init(damage, capsule_bomb_damage_shape, DAMAGE_SHAPES,
                               COLLISION_ITEM);
                damage->work->pos = work->pos;
                DAMAGE_RADIUS(damage->work) = 0.0F;
            }
            explosion_create_sized(&work->pos, BLAST_REACH);
            shake_create(&work->pos, SHAKE_RANGE, shake_power);
#ifdef COLONY_BOMB
            colony_flash += BLAST_FLASH;
#else
            vibration_pulses_create(0, BLAST_POWER, BLAST_PULSES,
                                    BLAST_INTERVAL, BLAST_FREQ);
#endif
        }
        break;
    case MODE_GONE:
        if (work->timer++ > GONE_STEPS) {
            work->pos.y = TRACK_MIDDLE(work) - HALF_TRACK(work) - RIDE_PAST;
            work->submode = 0;
            work->timer = 0;
            work->mode = MODE_RIDE;
        }
        if (work->timer == AFTERSHOCK_STEP) {
            shake_create(&work->pos, AFTERSHOCK_RANGE, aftershock_power);
#ifdef COLONY_BOMB
            colony_flash += AFTERSHOCK_FLASH;
#endif
        }
#ifdef COLONY_BOMB
        /* @hack redundant: a break that ends the switch's last case */
        break;
#endif
    }
    if (work->mode == MODE_RIDE) {
        end = HALF_TRACK(work) + RIDE_PAST;
        if (RIDE_SPEED(work) > 0.0F) {
            if (work->pos.y >= TRACK_MIDDLE(work) + end) {
                if (work->timer > WAIT_STEPS(work)) {
                    work->pos.y = TRACK_MIDDLE(work) - end;
                    work->timer = 0;
                } else {
                    /* @hack spelling: the timer added to by an assignment,
                     * where timer++ would do */
                    work->timer = work->timer + 1;
                    work->pos.y = TRACK_MIDDLE(work) + end;
                    REMOVE_STOPPED_WEAK_POINT(task);
                }
            } else {
                work->pos.y += RIDE_SPEED(work);
            }
            CHECK_CAP_WEAK_POINT(task, work,
                                 work->pos.y >=
                                     TRACK_MIDDLE(work) + end - RIDE_PAST);
        } else {
            if (work->pos.y <= TRACK_MIDDLE(work) - end) {
                if (work->timer > WAIT_STEPS(work)) {
                    work->pos.y = TRACK_MIDDLE(work) + end;
                    work->timer = 0;
                } else {
                    /* @hack spelling: the timer added to by an assignment,
                     * where timer++ would do */
                    work->timer = work->timer + 1;
                    work->pos.y = TRACK_MIDDLE(work) - end;
                    REMOVE_STOPPED_WEAK_POINT(task);
                }
            } else {
                work->pos.y += RIDE_SPEED(work);
            }
            CHECK_CAP_WEAK_POINT(task, work,
                                 work->pos.y <=
                                     TRACK_MIDDLE(work) - end + RIDE_PAST);
        }
        if (task->child) {
            task->child->work->pos.y = work->pos.y;
        }
        collision_register(task);
    }
}

static void capsule_bomb_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame;
    NJS_POINT3 view;
    Float amount;
    Uint32 color;
    NJS_CNK_OBJECT *cap;
    Sint16 tid;

    njCalcPoint(NULL, &work->pos, &view);
    if (view.z < NEAR_Z || view.z > FAR_Z) {
        return;
    }
    njPushMatrixEx();
    njSetTexture(&capsule_bomb_half_texlist);
    switch (work->mode) {
    case MODE_FUSE:
        amount = work->timer * HALF_RATE;
        amount = amount * amount;
        if (amount > 1.0F) {
            amount = 1.0F;
        }
        if (amount < 0.0F) {
            amount = 0.0F;
        }
        amount = 1.0F - amount;
        njTranslate(NULL, work->pos.x, work->pos.y + (amount * HALF_OPEN),
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawModel(capsule_bomb_upper_half->model);
        matrix_load_parent();
        njTranslate(NULL, work->pos.x, work->pos.y - (amount * HALF_OPEN),
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawModel(capsule_bomb_lower_half->model);
        matrix_load_parent();
        break;
    case MODE_GONE:
        if (work->timer < HALF_FADE_STEPS) {
            amount = 1.0F - (work->timer * (1.0F / HALF_FADE_STEPS));
            if (amount > 1.0F) {
                amount = 1.0F;
            }
            if (amount < 0.0F) {
                amount = 0.0F;
            }
            color = color_blend(HALF_GONE_COLOR, WHITE, amount);
            njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
            njRotateY(NULL, work->ang.y);
            model_draw_tinted(capsule_bomb_upper_half->model, color);
            model_draw_tinted(capsule_bomb_lower_half->model, color);
            matrix_load_parent();
        }
        break;
    }
    njSetTexture(&capsule_bomb_texlist);
    if (work->mode == MODE_RIDE ||
        (work->mode == MODE_FUSE && work->timer < WHOLE_FUSE_STEPS)) {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + (game_state.steps * SPIN_PER_STEP));
        njCnkEasyDrawModel(capsule_bomb_object->model);
        matrix_load_parent();
    }
    if (work->ang.x & SHOW_CAPS) {
        frame = (game_state.steps >> POISON_FRAME_SHIFT) % POISON_FRAMES;
        amount = HALF_TRACK(work);
        njTranslate(NULL, work->pos.x, TRACK_MIDDLE(work) + amount,
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        cap = &capsule_bomb_top_cap;
        njRotateX(NULL, cap->ang[0]);
        njCnkEasyDrawModel(cap->model);
        /* @hack spelling: the new texture id stored straight into the list,
         * where the bottom cap's three statements would do */
        tid = cap->child->model->plist[POISON_TID];
        cap->child->model->plist[POISON_TID] =
            (tid & ~TID_BITS) | (frame + POISON_FIRST);
        njTranslate(NULL, 0.0F, cap->child->pos[1], 0.0F);
        njCnkEasyDrawModel(cap->child->model);
        matrix_load_parent();
        njTranslate(NULL, work->pos.x, TRACK_MIDDLE(work) - amount,
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        cap = &capsule_bomb_bottom_cap;
        njRotateX(NULL, cap->ang[0]);
        njCnkEasyDrawModel(cap->model);
        tid = cap->child->model->plist[POISON_TID];
        tid = (tid & ~TID_BITS) | (frame + POISON_FIRST);
        cap->child->model->plist[POISON_TID] = tid;
        njTranslate(NULL, 0.0F, cap->child->pos[1], 0.0F);
        njCnkEasyDrawModel(cap->child->model);
    }
    njPopMatrixEx();
}

/* draw as capsule_bomb_draw does, but with njDirectDrawObject and the builds
 * in the models' place. the halves' fade and the caps' poison still draw the
 * models */
static void capsule_bomb_draw_built(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame;
    NJS_POINT3 view;
    Float amount;
    Uint32 color;
    NJS_CNK_OBJECT *cap;
    Sint16 tid;

    njCalcPoint(NULL, &work->pos, &view);
    if (view.z < NEAR_Z || view.z > FAR_Z) {
        return;
    }
    njPushMatrixEx();
    njSetTexture(&capsule_bomb_half_texlist);
    switch (work->mode) {
    case MODE_FUSE:
        amount = work->timer * HALF_RATE;
        amount = amount * amount;
        if (amount > 1.0F) {
            amount = 1.0F;
        }
        if (amount < 0.0F) {
            amount = 0.0F;
        }
        amount = 1.0F - amount;
        njTranslate(NULL, work->pos.x, work->pos.y + (amount * HALF_OPEN),
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njDirectDrawObject(capsule_bomb_builds[BUILD_UPPER_HALF].built);
        matrix_load_parent();
        njTranslate(NULL, work->pos.x, work->pos.y - (amount * HALF_OPEN),
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njDirectDrawObject(capsule_bomb_builds[BUILD_LOWER_HALF].built);
        matrix_load_parent();
        break;
    case MODE_GONE:
        if (work->timer < HALF_FADE_STEPS) {
            amount = 1.0F - (work->timer * (1.0F / HALF_FADE_STEPS));
            if (amount > 1.0F) {
                amount = 1.0F;
            }
            if (amount < 0.0F) {
                amount = 0.0F;
            }
            color = color_blend(HALF_GONE_COLOR, WHITE, amount);
            njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
            njRotateY(NULL, work->ang.y);
            model_draw_tinted(capsule_bomb_upper_half->model, color);
            model_draw_tinted(capsule_bomb_lower_half->model, color);
            matrix_load_parent();
        }
        break;
    }
    njSetTexture(&capsule_bomb_texlist);
    if (work->mode == MODE_RIDE ||
        (work->mode == MODE_FUSE && work->timer < WHOLE_FUSE_STEPS)) {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + (game_state.steps * SPIN_PER_STEP));
        njDirectDrawObject(capsule_bomb_builds[BUILD_CAPSULE].built);
        matrix_load_parent();
    }
    if (work->ang.x & SHOW_CAPS) {
        frame = (game_state.steps >> POISON_FRAME_SHIFT) % POISON_FRAMES;
        amount = HALF_TRACK(work);
        njTranslate(NULL, work->pos.x, TRACK_MIDDLE(work) + amount,
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        cap = &capsule_bomb_top_cap;
        njRotateX(NULL, cap->ang[0]);
        njDirectDrawObject(capsule_bomb_builds[BUILD_TOP_CAP].built);
        tid = cap->child->model->plist[POISON_TID];
        tid = (tid & ~TID_BITS) | (frame + POISON_FIRST);
        cap->child->model->plist[POISON_TID] = tid;
        njTranslate(NULL, 0.0F, cap->child->pos[1], 0.0F);
        njCnkEasyDrawModel(cap->child->model);
        matrix_load_parent();
        njTranslate(NULL, work->pos.x, TRACK_MIDDLE(work) - amount,
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        cap = &capsule_bomb_bottom_cap;
        njRotateX(NULL, cap->ang[0]);
        njDirectDrawObject(capsule_bomb_builds[BUILD_BOTTOM_CAP].built);
        tid = cap->child->model->plist[POISON_TID];
        tid = (tid & ~TID_BITS) | (frame + POISON_FIRST);
        cap->child->model->plist[POISON_TID] = tid;
        njTranslate(NULL, 0.0F, cap->child->pos[1], 0.0F);
        njCnkEasyDrawModel(cap->child->model);
    }
    njPopMatrixEx();
}

#endif
