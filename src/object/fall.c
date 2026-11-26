/* the fall steps, and the shared words, as fall.h declares them */
#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/fall.h"
#include "object/switch_state.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/task.h"
#include "system/util.h"

/* each moving step subtracts its gravity from spd.y, and keeps its drag's
 * share of spd.x and spd.z. the large step's are the medium step's */
#define SMALL_GRAVITY 0.02F
#define SMALL_DRAG 0.996F
#define MEDIUM_GRAVITY 0.1F
#define MEDIUM_DRAG 0.98F
#define LARGE_GRAVITY MEDIUM_GRAVITY
#define LARGE_DRAG MEDIUM_DRAG
/* a landed work keeps GROUND_DRAG of spd.x and spd.z. each stops where what
 * it keeps lies within STOP_SPEED of 0 */
#define GROUND_DRAG 0.1F
#define STOP_SPEED 1.0F
#define MAX_SPEED_Y 6.0F /* how far from 0 spd.y can go */

/* each step's sphere: its radius, and its center's heights above pos along
 * the work's turned y axis. the sweep starts the center at the probe height.
 * where the sweep meets a surface, the step puts pos the height below where
 * the center stops */
#define SMALL_RADIUS 2.5F
#define SMALL_HEIGHT 2.5F
#define SMALL_PROBE_HEIGHT 2.48F
#define MEDIUM_RADIUS 5.0F
#define MEDIUM_HEIGHT 5.0F
#define MEDIUM_PROBE_HEIGHT 4.9F
#define LARGE_RADIUS MEDIUM_RADIUS
#define LARGE_HEIGHT 15.0F
#define LARGE_PROBE_HEIGHT 14.9F

/* the flute stands' bits, the low byte of kept_object_bits. object_states_clear
 * keeps them on the relay's stages, and clears them on the rest */
#define RELAY_KEPT_BITS 0xff

static void fall_step_end(struct task_work *work, struct task_motion *motion);
static void fall_turn(struct task_work *work, NJS_VECTOR *vector);

/* turn vector in place by the inverse of work's angles. no header declares
 * it, and nothing calls it. as a static, SHC would leave it out */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void fall_turn_back(struct task_work *work, NJS_VECTOR *vector);
/* NOLINTEND(misc-use-internal-linkage) */

Uint8 *dummy_object_buffer = WORK_AREA;
struct dummy_request dummy_request = {0, FALSE, 0, FALSE, 0, FALSE, FALSE};
Sint32 dummy_object_count = 0;
Sint32 far_safe_count = 0;
Sint32 dynamite_flags = 0;
Uint32 kept_object_bits = 0;

Uint8 stage_switch_flags[SWITCH_IDS];

int fall_set_kind(struct task_work *work, struct task_motion *motion, int kind)
{
    if (motion == NULL) {
        return 0;
    }
    switch (kind) {
    case FALL_KIND_NORMAL:
        work->flags |= FALL_OFFERABLE | FALL_UNK_200;
        break;
    case FALL_KIND_11:
        work->flags |= FALL_OFFERABLE | FALL_PUSHABLE;
        break;
    case FALL_KIND_HEAVY:
        work->flags |= FALL_OFFERABLE | FALL_UNK_200;
        break;
    case FALL_KIND_SHAKE:
        work->flags |= FALL_OFFERABLE | FALL_SHAKABLE;
        break;
    case FALL_KIND_BURIED:
        work->flags |= FALL_OFFERABLE | FALL_BURIED;
        break;
    default:
        return 0;
    }
    work->id = kind;
    return 1;
}

void fall_step_small(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    NJS_VECTOR move;
    NJS_VECTOR offset;
    NJS_POINT3 pos;
    struct angles ang;
    int hit;

    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        /* @hack spelling: two tests of FALL_LANDED in a row, where one if and
         * its else would do */
        if (work->flags & FALL_LANDED) {
            motion->acc.y = 0.0F;
            motion->spd.y = 0.0F;
        } else {
            motion->acc.y = -SMALL_GRAVITY;
        }
        if (!(work->flags & FALL_LANDED)) {
            motion->spd.x *= SMALL_DRAG;
            motion->spd.z *= SMALL_DRAG;
        } else {
            motion->spd.x *= GROUND_DRAG;
            motion->spd.z *= GROUND_DRAG;
            if (_builtin_fabsf(motion->spd.x) <= STOP_SPEED) {
                motion->spd.x = 0.0F;
            }
            if (_builtin_fabsf(motion->spd.z) <= STOP_SPEED) {
                motion->spd.z = 0.0F;
            }
        }
        motion->spd.y += motion->acc.y;
        if (motion->spd.y > MAX_SPEED_Y) {
            motion->spd.y = MAX_SPEED_Y;
        } else if (motion->spd.y < -MAX_SPEED_Y) {
            motion->spd.y = -MAX_SPEED_Y;
        }
        pos = work->pos;
        move.x = motion->spd.x + motion->acc.x;
        move.y = motion->spd.y + motion->acc.y;
        move.z = motion->spd.z + motion->acc.z;
        ang = work->ang;
        offset.x = 0.0F;
        offset.y = SMALL_PROBE_HEIGHT;
        offset.z = 0.0F;
        fall_turn(work, &offset);
        njAddVector(&pos, &offset);
        hit =
            world_sweep_ball(&pos, &move, &ang, WORLD_FLAT_THIN, SMALL_RADIUS);
        if (((work->flags & FALL_FLOOR_ONLY) && (hit & SWEEP_FLOOR)) ||
            (!(work->flags & FALL_FLOOR_ONLY) && hit != 0)) {
            work->pos.x = pos.x + move.x;
            work->pos.y = pos.y + move.y;
            work->pos.z = pos.z + move.z;
            work->ang = ang;
            offset.x = 0.0F;
            offset.y = SMALL_HEIGHT;
            offset.z = 0.0F;
            fall_turn(work, &offset);
            njSubVector(&work->pos, &offset);
            work->flags |= FALL_LANDED;
        } else {
            if ((work->flags & FALL_FLOOR_ONLY) && (hit & SWEEP_WALL)) {
                work->pos.x = pos.x + move.x;
                work->pos.y = pos.y + move.y;
                work->pos.z = pos.z + move.z;
                offset.x = 0.0F;
                offset.y = SMALL_HEIGHT;
                offset.z = 0.0F;
                fall_turn(work, &offset);
                njSubVector(&work->pos, &offset);
                motion->spd.x *= GROUND_DRAG;
                motion->spd.z *= GROUND_DRAG;
            } else {
                work->pos.x += motion->spd.x;
                work->pos.y += motion->spd.y;
                work->pos.z += motion->spd.z;
            }
            work->ang.x = 0;
            work->ang.z = 0;
            work->flags &= ~FALL_LANDED;
        }
    } else {
        work->flags &= ~FALL_LANDED;
    }
    /* @hack spelling: the held test as a return of its own, where one if around
     * the two calls would do */
    if (work->flags & WORK_HELD_BY_PLAYER) {
        return;
    }
    collision_register(task);
    fall_step_end(work, motion);
}

void fall_step_medium(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    NJS_VECTOR move;
    NJS_VECTOR offset;
    NJS_POINT3 pos;
    struct angles ang;
    Float gravity;
    int hit;

    /* @hack spelling: two tests of FALL_LANDED in a row, where one if and its
     * else would do */
    if (work->flags & FALL_LANDED) {
        gravity = 0.0F;
        motion->spd.y = 0.0F;
    } else {
        gravity = -MEDIUM_GRAVITY;
    }
    if (!(work->flags & FALL_LANDED)) {
        motion->spd.x *= MEDIUM_DRAG;
        motion->spd.z *= MEDIUM_DRAG;
    } else {
        motion->spd.x *= GROUND_DRAG;
        motion->spd.z *= GROUND_DRAG;
        if (_builtin_fabsf(motion->spd.x) <= STOP_SPEED) {
            motion->spd.x = 0.0F;
        }
        if (_builtin_fabsf(motion->spd.z) <= STOP_SPEED) {
            motion->spd.z = 0.0F;
        }
    }
    motion->spd.y += gravity;
    if (motion->spd.y > MAX_SPEED_Y) {
        motion->spd.y = MAX_SPEED_Y;
    } else if (motion->spd.y < -MAX_SPEED_Y) {
        motion->spd.y = -MAX_SPEED_Y;
    }
    if (!(work->flags & FALL_LANDED) || njScalor(&motion->acc) != 0.0F) {
        pos = work->pos;
        move.x = motion->spd.x + motion->acc.x;
        move.y = motion->spd.y + motion->acc.y;
        move.z = motion->spd.z + motion->acc.z;
        ang = work->ang;
        offset.x = 0.0F;
        offset.y = MEDIUM_PROBE_HEIGHT;
        offset.z = 0.0F;
        fall_turn(work, &offset);
        njAddVector(&pos, &offset);
        hit =
            world_sweep_ball(&pos, &move, &ang, WORLD_FLAT_THIN, MEDIUM_RADIUS);
        if (((work->flags & FALL_FLOOR_ONLY) && (hit & SWEEP_FLOOR)) ||
            (!(work->flags & FALL_FLOOR_ONLY) && hit != 0)) {
            work->pos.x = pos.x + move.x;
            work->pos.y = pos.y + move.y;
            work->pos.z = pos.z + move.z;
            work->ang = ang;
            offset.x = 0.0F;
            offset.y = MEDIUM_HEIGHT;
            offset.z = 0.0F;
            fall_turn(work, &offset);
            njSubVector(&work->pos, &offset);
            work->flags |= FALL_LANDED;
        } else {
            if ((work->flags & FALL_FLOOR_ONLY) && (hit & SWEEP_WALL)) {
                work->pos.x = pos.x + move.x;
                work->pos.y = pos.y + move.y;
                work->pos.z = pos.z + move.z;
                offset.x = 0.0F;
                offset.y = MEDIUM_HEIGHT;
                offset.z = 0.0F;
                fall_turn(work, &offset);
                njSubVector(&work->pos, &offset);
            } else {
                work->pos.x += motion->spd.x;
                work->pos.y += motion->spd.y;
                work->pos.z += motion->spd.z;
            }
            work->ang.x = 0;
            work->ang.z = 0;
            work->flags &= ~FALL_LANDED;
        }
    }
    /* @hack spelling: a goto past collision_register, where an if around the
     * call would do */
    if (work->flags & WORK_HELD_BY_PLAYER) {
        goto done;
    }
    collision_register(task);
done:
    fall_step_end(work, motion);
}

void fall_step_large(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;
    NJS_VECTOR move;
    NJS_VECTOR offset;
    NJS_POINT3 pos;
    struct angles ang;
    Float gravity;
    int hit;

    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        /* @hack spelling: two tests of FALL_LANDED in a row, where one if and
         * its else would do */
        if (work->flags & FALL_LANDED) {
            gravity = 0.0F;
            motion->spd.y = 0.0F;
        } else {
            gravity = -LARGE_GRAVITY;
        }
        if (!(work->flags & FALL_LANDED)) {
            motion->spd.x *= LARGE_DRAG;
            motion->spd.z *= LARGE_DRAG;
        } else {
            motion->spd.x *= GROUND_DRAG;
            motion->spd.z *= GROUND_DRAG;
            if (_builtin_fabsf(motion->spd.x) <= STOP_SPEED) {
                motion->spd.x = 0.0F;
            }
            if (_builtin_fabsf(motion->spd.z) <= STOP_SPEED) {
                motion->spd.z = 0.0F;
            }
        }
        motion->spd.y += gravity;
        if (motion->spd.y > MAX_SPEED_Y) {
            motion->spd.y = MAX_SPEED_Y;
        } else if (motion->spd.y < -MAX_SPEED_Y) {
            motion->spd.y = -MAX_SPEED_Y;
        }
        if (!(work->flags & FALL_LANDED)) {
            pos = work->pos;
            move.x = motion->spd.x + motion->acc.x;
            move.y = motion->spd.y + motion->acc.y;
            move.z = motion->spd.z + motion->acc.z;
            ang = work->ang;
            offset.x = 0.0F;
            offset.y = LARGE_PROBE_HEIGHT;
            offset.z = 0.0F;
            fall_turn(work, &offset);
            njAddVector(&pos, &offset);
            hit = world_sweep_ball(&pos, &move, &ang, WORLD_FLAT_THIN,
                                   LARGE_RADIUS);
            if (((work->flags & FALL_FLOOR_ONLY) && (hit & SWEEP_FLOOR)) ||
                (!(work->flags & FALL_FLOOR_ONLY) && hit != 0)) {
                work->pos.x = pos.x + move.x;
                work->pos.y = pos.y + move.y;
                work->pos.z = pos.z + move.z;
                work->ang = ang;
                offset.x = 0.0F;
                offset.y = LARGE_HEIGHT;
                offset.z = 0.0F;
                fall_turn(work, &offset);
                njSubVector(&work->pos, &offset);
                work->flags |= FALL_LANDED;
            } else {
                if ((work->flags & FALL_FLOOR_ONLY) && (hit & SWEEP_WALL)) {
                    work->pos.x = pos.x + move.x;
                    work->pos.y = pos.y + move.y;
                    work->pos.z = pos.z + move.z;
                    offset.x = 0.0F;
                    offset.y = LARGE_HEIGHT;
                    offset.z = 0.0F;
                    fall_turn(work, &offset);
                    njSubVector(&work->pos, &offset);
                } else {
                    work->pos.x += motion->spd.x;
                    work->pos.y += motion->spd.y;
                    work->pos.z += motion->spd.z;
                }
                work->ang.x = 0;
                work->ang.z = 0;
                work->flags &= ~FALL_LANDED;
            }
        }
    } else {
        work->flags &= ~FALL_LANDED;
    }
    /* @hack spelling: a goto past collision_register, where an if around the
     * call would do */
    if (work->flags & WORK_HELD_BY_PLAYER) {
        goto done;
    }
    collision_register(task);
done:
    fall_step_end(work, motion);
}

void fall_step_fixed(struct task *task)
{
    struct task_work *work = task->work;
    struct task_motion *motion = task->motion.p;

    collision_register(task);
    fall_step_end(work, motion);
}

static void fall_step_end(struct task_work *work, struct task_motion *motion)
{
    work->flags &= ~FALL_STEP_BITS;
    if (motion != NULL) {
        motion->acc.x = motion->acc.y = motion->acc.z = 0.0F;
    }
}

void fall_turn_back(struct task_work *work, NJS_VECTOR *vector)
{
    NJS_VECTOR tmp;

    /* @hack spelling: vector copied field by field, where tmp = *vector would
     * do */
    tmp.x = vector->x;
    tmp.y = vector->y;
    tmp.z = vector->z;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, -work->ang.y);
    njRotateX(NULL, -work->ang.x);
    njRotateZ(NULL, -work->ang.z);
    njCalcVector(NULL, &tmp, vector);
    njPopMatrix(1);
}

/* turn vector in place by work's angles */
static void fall_turn(struct task_work *work, NJS_VECTOR *vector)
{
    NJS_VECTOR tmp;

    /* @hack spelling: vector copied field by field, where tmp = *vector would
     * do */
    tmp.x = vector->x;
    tmp.y = vector->y;
    tmp.z = vector->z;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, &tmp, vector);
    njPopMatrix(1);
}

void object_states_clear(void)
{
    memset(stage_switch_flags, 0, sizeof(stage_switch_flags));
    memset(&dummy_request, 0, sizeof(dummy_request));
    far_safe_count = 0;
    dynamite_flags = 0;
    if (stage_current_id() < STAGE_RELAY_SONIC ||
        stage_current_id() > STAGE_RELAY_KNUCKLES) {
        kept_object_bits &= ~RELAY_KEPT_BITS;
    }
}

void kept_object_bits_clear(void)
{
    kept_object_bits = 0;
}
