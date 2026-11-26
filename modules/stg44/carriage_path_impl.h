/* the carriages of Mad Space, which its carriage manager makes. a carriage is
 * a lead car and the trailer it makes behind itself. the two run round a path.
 * carriage.c compiles the train, an engine and its trailer.
 * orbital_platform.c defines FLAT_CARRIAGE and compiles the orbital platform,
 * whose cars stay level and gather speed. each source includes this header,
 * defines carriage_draw, and then includes stg44/carriage_step_impl.h.
 *
 * the source defines:
 * - CARRIAGE_INIT, the init's name;
 * - LEAD_COLLISION, the collision model the lead copies. the trailer copies it
 *   too, unless TRAILER_COLLISION names its own;
 * - CARRIAGE_ANGLES, the call by which the step reads the node's angles from
 *   the current matrix. the init reads them by current_matrix_angles_yxz;
 * - LOOK_AHEAD, how far along the path a car's front lies past its cursor;
 * - HEAD_START, how far along the path the lead starts ahead of its trailer;
 * - NEAR_RANGE, the distance from a car's node within which a player keeps
 *   WORLD_ADDED in the car's work;
 * - PIECE_HEIGHT and PIECE_OFFSET, the y and z in the trailer's frame at which
 *   it holds its emerald;
 * - acceleration and damping under FLAT_CARRIAGE, and travel_speed otherwise */
#ifndef STG44_CARRIAGE_PATH_IMPL_H
#define STG44_CARRIAGE_PATH_IMPL_H

#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "player/player.h"
#include "shared/emerald.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stg44/loop_path_impl.h"
#include "stg44/props.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"

#ifdef NONMATCHING
/* this side repairs the call the tag names: it calls emerald_create_plain_child
 * by its own declaration. the cars' kind and ids read alike either way */
#define CARRIAGE_EMERALD emerald_create_plain_child
#else
/* @bug mismatched-call: passes kind and id as Sint8s, where
 * emerald_create_plain_child takes Uint8s (JSR at 0x8c5018dc and at
 * 0x8c502bdc). the cars pass EMERALD_KIND_A_PATHMOVE and ids 0 to 2, which
 * read alike either way */
struct task *mad_space_carriage_emerald(struct task *parent, NJS_POINT3 *pos,
                                        Sint8 kind, Sint8 id);
#define CARRIAGE_EMERALD mad_space_carriage_emerald
#endif

enum carriage_mode {
    START_FORWARD,
    MOVE_FORWARD,
    START_BACKWARD,
    MOVE_BACKWARD
};

/* a car's work's submode: 0 in a lead, and TRAILER in the trailer it makes */
#define TRAILER 1

/* a car's record, in its task's motion. object is the car's node in the
 * collision world, and cursor its place on the path. speed is the distance the
 * cursor moves a step. no code reads unused */
struct carriage_work {
    NJS_OBJECT object;
    struct loop_cursor cursor;
    Float speed;
    Uint8 unused[24];
};

#define CARRIAGE(task) ((struct carriage_work *)(task)->motion.p)
/* the path the carriage manager puts in the task's unk_3c */
#define CARRIAGE_PATH(task) ((struct path *)(task)->unk_3c.p)
#define COLLISION_FLAGS (WORLD_SOLID | WORLD_MOVING)
/* the node takes the size class that its model's radius plus CLASS_MARGIN
 * fits */
#define CLASS_MARGIN 15.0F
/* a moving car plays its sounds while a player lies within SOUND_RANGE */
#define SOUND_RANGE 400.0F
/* a moving car turns TURN_EASE of the way to its new angles a step */
#define TURN_EASE 0.3F

static void carriage_destroy(struct task *task);
static void carriage_draw(struct task *task);
static void carriage_step(struct task *task);

#pragma inline(carriage_difference, carriage_offset)
/* @hack spelling: the difference as an inline walk of three Floats, where three
 * statements on the fields would do */
static void carriage_difference(const Float *start, const Float *end,
                                Float *out)
{
    *out++ = *end++ - *start++;
    *out++ = *end++ - *start++;
    *out = *end - *start;
}

/* @hack spelling: the sum as an inline walk of three Floats, where three
 * statements on the fields would do */
static void carriage_offset(const Float *start, const Float *delta, Float *out)
{
    *out++ = *delta++ + *start++;
    *out++ = *delta++ + *start++;
    *out = *delta + *start;
}

/* move task's cursor by distance along its path, back for a distance below 0.
 * the car then stands between two points of the path. its back is at the
 * cursor, and its front LOOK_AHEAD past it. its pos is their midpoint, and its
 * yaw faces from its back to its front. its tilt is halfway between the two
 * points' slopes, or 0 under FLAT_CARRIAGE.
 *
 * a distance of 0 sets the work's angles to these. any other turns them
 * TURN_EASE of the way there. *direction, unless direction is null, gets the
 * unit vector from the back toward the front. returns 0, loop_advance's
 * result */
static int carriage_advance(struct task *task, Float distance,
                            NJS_VECTOR *direction)
{
    struct task_work *work = task->work;
    Angle start_x;
    Angle start_z;
    Angle end_x;
    Angle end_z;
    Angle dz;
    Angle dx;
    Angle dy;
    Float fraction;
    int segment;
    int result;
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_VECTOR start_direction;
    NJS_VECTOR end_direction;
    NJS_VECTOR half_segment;
    struct angles orientation;

    result = loop_advance(CARRIAGE_PATH(task), &CARRIAGE(task)->cursor.segment,
                          &CARRIAGE(task)->cursor.fraction, distance, &start,
                          &start_direction, &start_x, &start_z);
#ifdef FLAT_CARRIAGE
    start_x = start_z = 0;
#endif
    segment = CARRIAGE(task)->cursor.segment;
    fraction = CARRIAGE(task)->cursor.fraction;
    loop_advance(CARRIAGE_PATH(task), &segment, &fraction, LOOK_AHEAD, &end,
                 &end_direction, &end_x, &end_z);
#ifdef FLAT_CARRIAGE
    end_x = end_z = 0;
#endif
    carriage_difference(&start.x, &end.x, &half_segment.x);
    if (direction) {
        *direction = half_segment;
        njUnitVector(direction);
    }
    /* @hack spelling: the halving as a walk of three Floats in a block of its
     * own, where three statements on the fields would do */
    {
        Float *axis = &half_segment.x;
        *axis++ *= 0.5F;
        *axis++ *= 0.5F;
        *axis *= 0.5F;
    }
    carriage_offset(&start.x, &half_segment.x, &work->pos.x);
    dx = end_x - start_x;
    dz = end_z - start_z;
    SCALE_TURN(dx, 0.5F);
    SCALE_TURN(dz, 0.5F);
    orientation.x = start_x + dx;
    orientation.z = start_z + dz;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -orientation.x);
    njRotateZ(NULL, -orientation.z);
    njCalcVector(NULL, &half_segment, &half_segment);
    njPopMatrixEx();
    orientation.y =
        atan2f(half_segment.x, half_segment.z) * (Float)ANGLE_PER_RADIAN;
    if (distance == 0.0F) {
        work->ang = orientation;
    } else {
        dx = orientation.x - work->ang.x;
        dy = orientation.y - work->ang.y;
        dz = orientation.z - work->ang.z;
        SCALE_TURN(dx, TURN_EASE);
        SCALE_TURN(dy, TURN_EASE);
        SCALE_TURN(dz, TURN_EASE);
        work->ang.x += dx;
        work->ang.y += dy;
        work->ang.z += dz;
    }
    return result;
}

void CARRIAGE_INIT(struct task *task)
{
    struct task_work *work = task->work;
    struct task *child;
    struct task *piece;
    NJS_OBJECT *object;
    /* @hack type: flags a Uint32, where the Sint32 world_add takes would do */
    Uint32 flags;
    Float reach;
    Angle x;
    Angle y;
    Angle z;

    task->motion.p = syCalloc(1, sizeof(struct carriage_work));
    if (!task->motion.p) {
        return;
    }
    task->step = carriage_step;
    task->draw = carriage_draw;
    task->destructor = carriage_destroy;
    CARRIAGE(task)->cursor.fraction = 0.0F;
    CARRIAGE(task)->cursor.segment = 0;
    if (work->submode) {
        carriage_advance(task, 0.0F, NULL);
    } else {
        carriage_advance(task, HEAD_START, NULL);
    }
    if (!work->submode) {
        child = task_create_child(TASK_WORK | TASK_FORCE, CARRIAGE_INIT, task);
        if (child) {
            child->work->submode = TRAILER;
            child->work->pos = work->pos;
            child->work->ang = work->ang;
            child->work->tick = work->tick;
            if (work->tick) {
                piece = CARRIAGE_EMERALD(
                    child, &work->pos, EMERALD_KIND_A_PATHMOVE, work->tick - 1);
                if (piece) {
                    piece->work->tick = EMERALD_TICK_PARENT_ANGLES;
                }
            }
            child->unk_3c.l = task->unk_3c.l;
        }
    }
#ifdef TRAILER_COLLISION
    if (work->submode) {
        CARRIAGE(task)->object = TRAILER_COLLISION;
    } else {
        CARRIAGE(task)->object = LEAD_COLLISION;
    }
#else
    CARRIAGE(task)->object = LEAD_COLLISION;
#endif
    object = &CARRIAGE(task)->object;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    current_matrix_angles_yxz(&x, &y, &z);
    njPopMatrixEx();
    object->ang[2] = z;
    object->ang[0] = x;
    object->ang[1] = y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = COLLISION_FLAGS;
    reach = object->model->r + CLASS_MARGIN;
    if (reach <= WORLD_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SIZE_CLASS_2_MAX) {
            flags = COLLISION_FLAGS | WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SIZE_CLASS_3_MAX) {
            flags = COLLISION_FLAGS | WORLD_SIZE_CLASS(2);
        } else {
            flags = COLLISION_FLAGS | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
}

static void carriage_destroy(struct task *task)
{
    if (CARRIAGE(task)->object.model) {
        world_remove(task, &CARRIAGE(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}
#endif
