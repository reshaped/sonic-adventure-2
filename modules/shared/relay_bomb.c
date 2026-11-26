/* Cannon's Core's capsule bomb, capsule_bomb_relay_init of
 * shared/capsule_bomb.h */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/blast.h"
#include "shared/capsule_bomb.h"
#include "shared/capsule_bomb_parts.h"
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

#define BLAST_SOUND 11

static void capsule_bomb_relay_damage_step(struct task *task);
static void capsule_bomb_relay_destroy(struct task *task);
static void capsule_bomb_relay_step(struct task *task);
static void capsule_bomb_relay_draw(struct task *task);

NJS_TEXLIST *capsule_bomb_texlists[] = {
    &capsule_bomb_texlist,
    &capsule_bomb_shard_b_texlist,
    &capsule_bomb_shard_a_texlist,
    &capsule_bomb_half_texlist,
    NULL,
};

static struct collision_shape capsule_bomb_shapes[BOMB_SHAPES] = {
#include "shared/relay_bomb/capsule_bomb_shapes.inc"
};
static struct collision_shape capsule_bomb_damage_shape[DAMAGE_SHAPES] = {
#include "shared/relay_bomb/capsule_bomb_damage_shape.inc"
};
/* a sphere of radius 5 */
static struct collision_shape dat_capsule_bomb_0[1] = {
#include "shared/relay_bomb/dat_capsule_bomb_0.inc"
};

/* the radius the blast's damage gains each step */
static Float damage_growth = 1.5F;
/* the power of the blast's quake, and of its aftershock */
static Float shake_power = 5.0F;
static Float aftershock_power = 10.0F;

static void capsule_bomb_relay_damage_step(struct task *task)
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

void capsule_bomb_relay_init(struct task *task)
{
    struct task_work *work = task->work;
    Float half;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = capsule_bomb_relay_draw;
    task->step = capsule_bomb_relay_step;
    task->destructor = capsule_bomb_relay_destroy;
    TRACK_MIDDLE(work) = work->pos.y;
    weak_point_create(task, WEAK_POINT_INDEX, &work->pos);
    collision_init(task, capsule_bomb_shapes, BOMB_SHAPES, COLLISION_OBJECT);
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
    if (task->placement && placement_get_state(task)) {
        work->mode = MODE_GONE;
    }
}

static void capsule_bomb_relay_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->mode == MODE_GONE) {
        placement_set_state(task, GONE_STATE);
    }
}

static void capsule_bomb_relay_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *damage;
    struct angles floor_ang;
    struct angles ang;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float *axis;
    Float floor;
    Float end_offset;
    int row;
    int turn_index;
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
            floor = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z,
                                        &floor_ang);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            for (turn_index = 0; turn_index < SHARD_TURNS; turn_index++) {
                turn = turn_index * EIGHTH_TURN;
                for (row = 0; row < SHARD_ROWS; row++) {
                    spd.x = njSin(turn);
                    spd.z = njCos(turn);
                    spd.y = ((row - SHARD_MIDDLE_ROW) * SHARD_RISE) +
                            (rand() * (SHARD_LIFT * RAND_SCALE));
                    pos.x = spd.x * SHARD_OUT;
                    pos.y = (row * SHARD_ROW_HEIGHT) - SHARD_LOW;
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
                    if ((turn_index + row) & 1) {
                        debris_launch_built_flags(
                            &pos, &ang, floor, &spd, capsule_bomb_shard_a_model,
                            NULL, &capsule_bomb_shard_a_texlist,
                            DEBRIS_FADE_COLORS);
                    } else {
                        debris_launch_built_flags(
                            &pos, &ang, floor, &spd, capsule_bomb_shard_b_model,
                            NULL, &capsule_bomb_shard_b_texlist,
                            DEBRIS_FADE_COLORS);
                    }
                }
            }
            njPopMatrixEx();
        } else if (!task->child && !work->timer) {
            weak_point_create(task, WEAK_POINT_INDEX, &work->pos);
        }
        break;
    case MODE_FUSE:
        if (work->timer++ > FUSE_STEPS) {
            sound_play(BLAST_SOUND, NULL, 0, 0);
            work->mode = MODE_GONE;
            work->timer = 0;
            blast_create(BLAST_HUGE, &work->pos);
            damage = task_create(TASK_WORK, DAMAGE_TASK_LEVEL,
                                 capsule_bomb_relay_damage_step, "DmgColExec");
            if (damage) {
                collision_init(damage, capsule_bomb_damage_shape, DAMAGE_SHAPES,
                               COLLISION_ITEM);
                damage->work->pos = work->pos;
                DAMAGE_RADIUS(damage->work) = 0.0F;
            }
            explosion_create_sized(&work->pos, BLAST_REACH);
            shake_create(&work->pos, SHAKE_RANGE, shake_power);
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
        }
    }
    if (work->mode == MODE_RIDE) {
        end_offset = HALF_TRACK(work) + RIDE_PAST;
        if (work->scl.x > 0.0F) {
            if (work->pos.y >= TRACK_MIDDLE(work) + end_offset) {
                if (work->timer > WAIT_STEPS(work)) {
                    work->pos.y = TRACK_MIDDLE(work) - end_offset;
                    work->timer = 0;
                } else {
                    work->timer++;
                    work->pos.y = TRACK_MIDDLE(work) + end_offset;
                    if (task->child) {
                        task_kill(task->child);
                    }
                }
            } else {
                work->pos.y += work->scl.x;
            }
        } else {
            if (work->pos.y <= TRACK_MIDDLE(work) - end_offset) {
                if (work->timer > WAIT_STEPS(work)) {
                    work->pos.y = TRACK_MIDDLE(work) + end_offset;
                    work->timer = 0;
                } else {
                    work->timer++;
                    work->pos.y = TRACK_MIDDLE(work) - end_offset;
                    if (task->child) {
                        task_kill(task->child);
                    }
                }
            } else {
                work->pos.y += work->scl.x;
            }
        }
        if (task->child) {
            task->child->work->pos.y = work->pos.y;
        }
        collision_register(task);
    }
}

static void capsule_bomb_relay_draw(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 frame;
    Float half_track;
    Float amount;
    Uint32 color;
    Sint16 tid;

    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- WAITTIME(*)";
        (void)"<- WAITTIME( )";
        (void)"<- ANGLE Y";
        (void)"<- SPD";
        (void)"<- HEIGHT";
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
        (work->mode == MODE_FUSE && work->timer < 1)) {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + (game_state.steps * SPIN_PER_STEP));
        njCnkEasyDrawModel(capsule_bomb_object->model);
        matrix_load_parent();
    }
    if (work->ang.x & SHOW_CAPS) {
        frame = (game_state.steps >> POISON_FRAME_SHIFT) % POISON_FRAMES;
        half_track = HALF_TRACK(work);
        njTranslate(NULL, work->pos.x, TRACK_MIDDLE(work) + half_track,
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, capsule_bomb_top_cap.ang[0]);
        njCnkEasyDrawModel(capsule_bomb_top_cap.model);
        tid = capsule_bomb_top_cap.child->model->plist[POISON_TID];
        tid = (tid & ~TID_BITS) | (frame + POISON_FIRST);
        capsule_bomb_top_cap.child->model->plist[POISON_TID] = tid;
        njTranslate(NULL, 0.0F, capsule_bomb_top_cap.child->pos[1], 0.0F);
        njCnkEasyDrawModel(capsule_bomb_top_cap.child->model);
        matrix_load_parent();
        njTranslate(NULL, work->pos.x, TRACK_MIDDLE(work) - half_track,
                    work->pos.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, capsule_bomb_bottom_cap.ang[0]);
        njCnkEasyDrawModel(capsule_bomb_bottom_cap.model);
        tid = capsule_bomb_bottom_cap.child->model->plist[POISON_TID];
        tid = (tid & ~TID_BITS) | (frame + POISON_FIRST);
        capsule_bomb_bottom_cap.child->model->plist[POISON_TID] = tid;
        njTranslate(NULL, 0.0F, capsule_bomb_bottom_cap.child->pos[1], 0.0F);
        njCnkEasyDrawModel(capsule_bomb_bottom_cap.child->model);
    }
    njPopMatrixEx();
}
