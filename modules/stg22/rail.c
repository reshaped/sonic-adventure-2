#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/color.h"
#include "effect/burst.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stg22/rail.h"
#include "stg22/rail_intake.h"
#include "stg22/rail_layout.h"
#include "stg22/rail_marker.h"
#include "stg22/rail_sensor.h"
#include "stg22/rail_art.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a transport rail's walk along a path's points. past either end, the walk
 * takes a point every BEYOND_SPACING along a line. the line runs from the
 * third point from that end through the end */
#define BEYOND_SPACING 100.0F

#ifdef NONMATCHING
/* the plain forms the hack tags name */
static void rail_difference(const NJS_POINT3 *end, const NJS_POINT3 *start,
                            NJS_VECTOR *difference)
{
    difference->x = end->x - start->x;
    difference->y = end->y - start->y;
    difference->z = end->z - start->z;
}

#define RAIL_DIFFERENCE(a, b, result) rail_difference(a, b, result)

static void rail_offset(const NJS_POINT3 *origin, const NJS_VECTOR *delta,
                        NJS_POINT3 *result)
{
    result->x = origin->x + delta->x;
    result->y = origin->y + delta->y;
    result->z = origin->z + delta->z;
}
static void rail_scale(NJS_VECTOR *vector, Float scale)
{
    vector->x *= scale;
    vector->y *= scale;
    vector->z *= scale;
}
#else
/* @hack spelling: the difference walked through pointers to the vectors'
 * fields, where the fields by name would do */
#define RAIL_DIFFERENCE(a, b, result)                                          \
    do {                                                                       \
        Float *out = &(result)->x;                                             \
        Float *end = &(a)->x;                                                  \
        Float *start = &(b)->x;                                                \
        *out = *end++ - *start++;                                              \
        out++;                                                                 \
        *out = *end++ - *start++;                                              \
        out++;                                                                 \
        *out = *end - *start;                                                  \
    } while (0)

#pragma inline(rail_offset, rail_scale)
/* @hack spelling: the sum walked through pointers to the vectors' fields,
 * where the fields by name would do */
static void rail_offset(NJS_POINT3 *origin, NJS_VECTOR *delta,
                        NJS_POINT3 *result)
{
    Float *out = &result->x;
    Float *offset = &delta->x;
    Float *base = &origin->x;
    *out = *offset++ + *base++;
    out++;
    *out = *offset++ + *base++;
    out++;
    *out = *offset + *base;
}
/* @hack spelling: the scaling walked through a pointer to the vector's
 * fields, where the fields by name would do */
static void rail_scale(NJS_VECTOR *vector, Float scale)
{
    Float *coordinate = &vector->x;
    *coordinate++ *= scale;
    *coordinate++ *= scale;
    *coordinate *= scale;
}

#endif

static Bool rail_point(NJS_POINT3 *points, int count, int index,
                       NJS_POINT3 *position)
{
    NJS_VECTOR direction;
    Bool outside;

    if (index >= count) {
        RAIL_DIFFERENCE(&points[count - 1], &points[count - 3], &direction);
        rail_scale(&direction,
                   (index - count + 1) * BEYOND_SPACING / njScalor(&direction));
        rail_offset(&points[count - 1], &direction, position);
        outside = TRUE;
    } else if (index < 0) {
#ifdef NONMATCHING
        /* rail_difference, the plain form the hack tag names */
        rail_difference(&points[0], &points[2], &direction);
#else
        /* @hack spelling: the difference walked through pointers that step
         * after each subtraction, where RAIL_DIFFERENCE would do */
        {
            Float *out = &direction.x;
            Float *first = &points[0].x;
            Float *third = &points[2].x;
            *out++ = *first - *third;
            first++;
            third++;
            *out++ = *first - *third;
            first++;
            third++;
            *out = *first - *third;
        }
#endif
        rail_scale(&direction, -index * BEYOND_SPACING / njScalor(&direction));
        rail_offset(&points[0], &direction, position);
        outside = TRUE;
    } else {
        *position = points[index];
        outside = FALSE;
    }
    return outside;
}

/* move a walk along path by distance, back where it is negative. the walk
 * lies *fraction of the way from point *segment to the next. it sets
 * *segment, *fraction and *position where it stops. it sets *direction,
 * unless null, to its segment's last point less its first. it returns TRUE
 * where it took a point past either end, and FALSE otherwise. it walks only a
 * path whose flags are 0, an open path of PATH_LAYOUT_POS points. for any
 * other, it changes nothing and returns FALSE */
static Bool rail_advance(struct path *path, Sint32 *segment, Float *fraction,
                         Float distance, NJS_POINT3 *position,
                         NJS_VECTOR *direction)
{
    NJS_POINT3 *points;
    int count;
    Bool outside;
    int next;
    int index;
    Float length;
    Float along;
    Float left;
    Float first_weight;
    Float last_weight;
    NJS_POINT3 first;
    NJS_POINT3 last;

    outside = FALSE;
    if (!path->flags) {
        count = path->count;
        points = path->points;
        if (distance > 0.0F) {
            for (index = *segment, next = index + 1, along = *fraction;;
                 index++, along = 0.0F, next++) {
                if (rail_point(points, count, index, &first)) {
                    outside = TRUE;
                }
                if (rail_point(points, count, next, &last)) {
                    outside = TRUE;
                }
                length = njDistanceP2P(&first, &last);
                left = length * along;
                left = length - left;
                if (left < 0.0F) {
                    left = 0.0F;
                }
                if (distance > left) {
                    distance -= left;
                    continue;
                }
                first_weight = (left - distance) / length;
                last_weight = 1.0F - first_weight;
                if (last_weight < 0.0F) {
                    last_weight = 0.0F;
                    first_weight = 1.0F;
                }
                *segment = index;
                *fraction = last_weight;
                position->x = (first.x * first_weight) + (last.x * last_weight);
                position->y = (first.y * first_weight) + (last.y * last_weight);
                position->z = (first.z * first_weight) + (last.z * last_weight);
                if (direction != NULL) {
                    RAIL_DIFFERENCE(&last, &first, direction);
                }
                return outside;
            }
        } else {
            for (index = *segment, next = index + 1, along = *fraction;;
                 index--, along = 1.0F, next--) {
                if (rail_point(points, count, index, &first)) {
                    outside = TRUE;
                }
                if (rail_point(points, count, next, &last)) {
                    outside = TRUE;
                }
                length = njDistanceP2P(&first, &last);
                left = length * along;
                last_weight = length;
                last_weight -= left;
                if (last_weight < 0.0F) {
                    left = length;
                }
                if (-distance > left) {
                    distance += left;
                    continue;
                }
                left += distance;
                last_weight = left / length;
                first_weight = 1.0F - last_weight;
                if (first_weight < 0.0F) {
                    last_weight = 1.0F;
                    first_weight = 0.0F;
                }
                *segment = index;
                *fraction = last_weight;
                position->x = (first.x * first_weight) + (last.x * last_weight);
                position->y = (first.y * first_weight) + (last.y * last_weight);
                position->z = (first.z * first_weight) + (last.z * last_weight);
                if (direction != NULL) {
                    RAIL_DIFFERENCE(&last, &first, direction);
                }
                return outside;
            }
        }
    }
    return FALSE;
}

/* a rail's work holds its rider's slot in tick. scl.z holds the ride's
 * speed, the distance it moves a step. RAIL is the rail's entry of
 * gadget_rails. SEGMENT is the segment of the ride its walk is on, and
 * FRACTION the walk's fraction along it. rail_destroy nulls motion, force
 * and extra, which task_destroy would free */
#ifdef NONMATCHING
/* this side keeps RAIL as an index in the work's submode, SEGMENT in unk_3c
 * and FRACTION in scl.x */
#define RAIL(task) (&gadget_rails[(task)->work->submode])
#define SEGMENT(task) ((task)->unk_3c.l)
#define FRACTION(task) ((task)->work->scl.x)
#else
/* @hack type: RAIL, SEGMENT and FRACTION kept in unk_3c, motion and extra,
 * where an index in the work's submode, unk_3c and scl.x would do */
#define RAIL(task) ((struct gadget_rail *)(task)->unk_3c.p)
#define SEGMENT(task) ((task)->motion.l)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FRACTION(task) (*(Float *)&(task)->extra)
#endif
/* a player within RAIL_BOARD_REACH of an idle rail's first point boards it.
 * the rail first waits RAIL_REENTRY_STEPS after a forward ride. a player
 * that comes near during the wait starts it over */
#define RAIL_REENTRY_STEPS 50
#define RAIL_BOARD_REACH 15.0F
/* the arg a boarding rider's PLAYER_REQUEST_SPIN takes: ANIM_SPIN's number,
 * which the request's handlers play without reading it */
#define RAIL_BOARD_ARG 11
/* a ride starts at a speed of 0, which gains RAIL_ACCELERATION a step, up to
 * rail_speed_limit */
#define RAIL_ACCELERATION 0.1F
/* a ride ends once its walk passes the path's last point. the rider then
 * lies RAIL_EXIT_OFFSET on along the last segment. it leaves along it at
 * rail_exit_speed, its stick locked for RAIL_LOCK_STEPS. where the rail's
 * exit stands, the end opens it and plays EXIT_SOUND. it shakes player 0's
 * pad at EXIT_POWER and EXIT_FREQ too */
#define RAIL_EXIT_OFFSET 15.0F
#define RAIL_LOCK_STEPS 10
#define EXIT_SOUND 5
#define EXIT_POWER 5
#define EXIT_FREQ 10
#define EXIT_BURST_SIZE 25.0F
/* a reverse ride, which ends past the first point, waits
 * REVERSE_REENTRY_STEPS */
#define REVERSE_REENTRY_STEPS 100
/* a rail puts up to MOST_MARKERS markers, marker_spacing apart, along its
 * markers path. their walk starts FIRST_MARKER_FRACTION of the way from the
 * path's second point to its third. a spacing below LEAST_MARKER_SPACING
 * puts none */
#define FIRST_MARKER_FRACTION 0.01F
#define MOST_MARKERS 500
#define LEAST_MARKER_SPACING 1.0F

/* a rail's modes, in its work's mode. no source sets RAIL_REVERSE */
enum rail_mode { RAIL_IDLE, RAIL_FORWARD, RAIL_REVERSE };
Float rail_speed_limit = 15.0F;
/* the first rail whose step finds it set clears it, and destroys its exit
 * with the exit's children. its next step makes them again. no source sets
 * it */
Sint32 rail_reset_requested = 0;
/* each marker turns about its z by rail_marker_roll more than the one
 * before */
Angle rail_marker_roll = 0x1000;
Float rail_exit_speed = 4.5F;

Angle rail_exit_open_rate = 0x300;
Angle rail_exit_close_rate = -0x180;
Angle rail_marker_spin = 0x200;

/* an end's solid takes the size class of its model's radius plus
 * RAIL_SENSOR_MARGIN. the exit's solid collides while a player lies within
 * that sum of the model's center */
#define RAIL_SENSOR_MARGIN 15.0F
/* copy source into task's sensor, at the work's pos and ang, and put the copy
 * in the world. it leaves pushed a matrix of the copy's place and turn, which
 * the caller pops */
/* @hack spelling: a macro, where an inline function would do */
#define RAIL_PLACE_SENSOR(task, work, source)                                  \
    {                                                                          \
        NJS_OBJECT *object = (task)->extra.p;                                  \
        Float radius;                                                          \
        Uint32 flags;                                                          \
                                                                               \
        *object = (source);                                                    \
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);         \
        object->child = NULL;                                                  \
        object->sibling = NULL;                                                \
        object->ang[1] = (work)->ang.y;                                        \
        object->ang[2] = (work)->ang.z;                                        \
        object->ang[0] = (work)->ang.x;                                        \
        object->pos[0] = (work)->pos.x;                                        \
        object->pos[1] = (work)->pos.y;                                        \
        object->pos[2] = (work)->pos.z;                                        \
        flags = WORLD_SOLID;                                                   \
        radius = object->model->r + RAIL_SENSOR_MARGIN;                        \
        if (!(radius > WORLD_SIZE_CLASS_1_MAX)) {                              \
            if (radius > WORLD_SIZE_CLASS_2_MAX) {                             \
                flags = WORLD_SOLID | WORLD_SIZE_CLASS(1);                     \
            } else if (radius > WORLD_SIZE_CLASS_3_MAX) {                      \
                flags = WORLD_SOLID | WORLD_SIZE_CLASS(2);                     \
            } else {                                                           \
                flags = WORLD_SOLID | WORLD_SIZE_CLASS(3);                     \
            }                                                                  \
        }                                                                      \
        world_add(flags, task, object);                                        \
        njPushMatrixEx();                                                      \
        njUnitMatrix(NULL);                                                    \
        njTranslateEx((NJS_VECTOR *)RAIL_SENSOR(task)->object.pos);            \
        njRotateZ(NULL, RAIL_SENSOR(task)->object.ang[2]);                     \
        njRotateY(NULL, RAIL_SENSOR(task)->object.ang[1]);                     \
        njRotateX(NULL, RAIL_SENSOR(task)->object.ang[0]);                     \
    }

#pragma inline(rail_release_sensor)
static void rail_release_sensor(struct task *task)
{
    if (task->extra.p != NULL) {
        if (RAIL_SENSOR(task)->object.model != NULL) {
            world_remove(task, task->extra.p);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void gadget_rail_draw(struct task *task);
static void gadget_rail_exit_init(struct task *task);
static void gadget_rail_exit_draw(struct task *task);
static void gadget_rail_exit_destroy(struct task *task);
static void gadget_rail_marker_step(struct task *task);
static void gadget_rail_marker_draw(struct task *task);
static void rail_intake_destroy(struct task *task);
static void rail_init(struct task *task);
static void rail_destroy(struct task *task);
static void rail_step(struct task *task);

#pragma inline(rail_intake_center)
static void rail_intake_center(NJS_OBJECT *object, NJS_POINT3 *center)
{
    njCalcPoint(NULL, &object->model->center, center);
}

static void rail_intake_init(struct task *task)
{
    struct task_work *work = task->work;

    task->extra.p = syCalloc(1, sizeof(struct gadget_rail_sensor));
    if (task->extra.p == NULL) {
        return;
    }
#ifdef NONMATCHING
    /* where collision_init fails, this side frees the sensor and leaves the
     * intake unbuilt. the shipped code goes on without a collision record */
    if (!collision_init(task, rail_intake_shapes, 3, COLLISION_OBJECT)) {
        syFree(task->extra.p);
        task->extra.p = NULL;
        return;
    }
#endif
    task->draw = gadget_rail_intake_draw;
    task->destructor = rail_intake_destroy;
    task->step = gadget_rail_intake_step;
#ifdef NONMATCHING
#else
    collision_init(task, rail_intake_shapes, 3, COLLISION_OBJECT);
#endif
    RAIL_PLACE_SENSOR(task, work, rail_intake_collision);
    rail_intake_center(task->extra.p, &RAIL_SENSOR(task)->center);
    njPopMatrixEx();
}

static void rail_intake_destroy(struct task *task)
{
    rail_release_sensor(task);
}

static struct task *rail_create(struct gadget_rail *rail, struct task *parent)
{
    struct task *task;

    task = task_create_child(TASK_WORK, rail_init, parent);
    /* @hack spelling: a return of task inside the test and of NULL after it,
     * where one return of task would do */
    if (task != NULL) {
#ifdef NONMATCHING
        /* this side stores the rail as its index in gadget_rails, which RAIL
         * reads on this side */
        task->work->submode = (Sint8)(rail - gadget_rails);
#else
        task->unk_3c.p = rail;
#endif
        return task;
    }
    return NULL;
}

void gadget_rails_create(struct task *parent)
{
    int i;
    struct gadget_rail *rail;

    rail = gadget_rails;
    for (i = 0; (unsigned)i < sizeof(gadget_rails) / sizeof(gadget_rails[0]);
         i++) {
        rail_create(rail, parent);
        rail++;
    }
}

static void rail_init(struct task *task)
{
    struct task_work *work = task->work;

    task->draw = gadget_rail_draw;
    task->step = rail_step;
    task->destructor = rail_destroy;
    SEGMENT(task) = 0;
    FRACTION(task) = 0.0F;
    work->scl.z = 0.0F;
}

static void rail_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

Bool gadget_rail_board(struct task *rail, int player)
{
    struct task_work *work = rail->work;
    if (work->mode == RAIL_IDLE && work->timer == 0) {
        work->tick = player;
        work->mode = RAIL_FORWARD;
        SEGMENT(rail) = 0;
        FRACTION(rail) = 0.0F;
        player_request(work->tick, PLAYER_REQUEST_SPIN, RAIL_BOARD_ARG);
        work->scl.z = 0.0F;
        return TRUE;
    }
    return FALSE;
}

static void rail_step(struct task *task)
{
    struct task_work *work;
    struct gadget_rail *rail;
    NJS_POINT3 *points;
    int count;
    Bool outside;
    int near_slot;
    struct task *exit;
    struct task *intake;
    struct task *marker;
    int markers;
    Float marker_fraction;
    Sint32 marker_segment;
    Angle roll;
    NJS_POINT3 first;
    NJS_POINT3 last;
    NJS_VECTOR backward;
    NJS_VECTOR forward;
    NJS_POINT3 position;
    NJS_VECTOR velocity;
    NJS_POINT3 release;
#ifdef NONMATCHING
#else
    Float *component;
    Float *out;
    Float *end;
    Float *start;
#endif
    Float x;

    work = task->work;
    /* @hack spelling: the assignment to rail inside the expression, where a
     * statement of its own would do */
    points = (rail = RAIL(task))->ride->points;
    count = rail->ride->count;
    first = points[0];
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    last = points[count - 1];
    rail_difference(&first, &points[1], &backward);
    rail_difference(&last, &points[count - 2], &forward);
#else
    /* @hack spelling: the two differences, and the exit speeds' scaling
     * below, walked through pointers to the vectors' fields, where
     * RAIL_DIFFERENCE and rail_scale would do */
    out = &backward.x;
    end = &points[0].x;
    start = &points[1].x;
    *out++ = *end++ - *start++;
    *out++ = *end++ - *start++;
    *out = *end - *start;
    last = points[count - 1];
    out = &forward.x;
    end = &points[count - 1].x;
    start = &points[count - 2].x;
    *out++ = *end++ - *start++;
    *out++ = *end++ - *start++;
    *out = *end - *start;
#endif
    njUnitVector(&backward);
    njUnitVector(&forward);
    switch (work->mode) {
    case RAIL_FORWARD:
        outside = rail_advance(RAIL(task)->ride, &SEGMENT(task),
                               &FRACTION(task), work->scl.z, &position, NULL);
        work->scl.z += RAIL_ACCELERATION;
        if (work->scl.z > rail_speed_limit) {
            work->scl.z = rail_speed_limit;
        }
        /* @hack spelling: x, a local for position.x, where position.x read in
         * place would do */
        x = position.x;
        if (!outside) {
            player_set_pos(work->tick, x, position.y, position.z);
        } else {
            release.x = x + (forward.x * RAIL_EXIT_OFFSET);
            release.y = position.y + (forward.y * RAIL_EXIT_OFFSET);
            release.z = position.z + (forward.z * RAIL_EXIT_OFFSET);
            player_set_pos(work->tick, release.x, release.y, release.z);
            player_records[work->tick]->stick_lock = RAIL_LOCK_STEPS;
            work->mode = RAIL_IDLE;
            work->timer = RAIL_REENTRY_STEPS;
            player_request(work->tick, PLAYER_REQUEST_RELEASE, 0);
            velocity = forward;
#ifdef NONMATCHING
            /* rail_scale, the plain form of the scaling that the hack tag at
             * the differences above names */
            rail_scale(&velocity, rail_exit_speed);
#else
            component = &velocity.x;
            *component++ *= rail_exit_speed;
            *component++ *= rail_exit_speed;
            *component *= rail_exit_speed;
#endif
            player_set_speed(work->tick, velocity.x, velocity.y, velocity.z);
            burst_create_wobbling(&release, &velocity, EXIT_BURST_SIZE);
            if (task->child != NULL) {
                sound_play(EXIT_SOUND, NULL, 0, 0);
                vibration_start(0, EXIT_POWER, EXIT_FREQ, VIBRATION_UNTIMED);
                task->child->work->mode = RAIL_EXIT_OPENING;
            }
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: without the exit, it reads
         * none of the exit's work */
        if (task->child == NULL) {
            break;
        }
#endif
        /* @bug memory: reads the exit's work at 0x8c50653e without a test of
         * task->child. the child is null where the exit couldn't be made, as
         * the test at 0x8c506518 expects */
        if (task->child->work->collision != NULL) {
            task->child->work->tick = 0;
            task->child->work->collision->shapes[0].push &=
                ~PUSH_ALL_BUT_PICKUPS;
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side: it writes
             * no shapes[1] past the exit's one shape */
#else
            /* @bug memory: writes shapes[1] of the exit's collision record,
             * which holds one shape, at 0x8c50656e */
            task->child->work->collision->shapes[1].push &=
                ~PUSH_ALL_BUT_PICKUPS;
#endif
        }
        break;
    case RAIL_REVERSE:
        outside = rail_advance(RAIL(task)->ride, &SEGMENT(task),
                               &FRACTION(task), -work->scl.z, &position, NULL);
        work->scl.z += RAIL_ACCELERATION;
        if (work->scl.z > rail_speed_limit) {
            work->scl.z = rail_speed_limit;
        }
        if (!outside) {
            player_set_pos(work->tick, position.x, position.y, position.z);
        } else {
            /* @bug logic: offsets the rider's z by backward.y, where
             * backward.z is meant, at 0x8c5065c4 */
            player_set_pos(work->tick,
                           position.x + (backward.x * RAIL_EXIT_OFFSET),
                           position.y + (backward.y * RAIL_EXIT_OFFSET),
                           position.z + (backward.y * RAIL_EXIT_OFFSET));
            player_records[work->tick]->stick_lock = RAIL_LOCK_STEPS;
            player_request(work->tick, PLAYER_REQUEST_RELEASE, 0);
            velocity = backward;
#ifdef NONMATCHING
            /* rail_scale, the plain form of the scaling that the hack tag at
             * the differences above names */
            rail_scale(&velocity, rail_exit_speed);
#else
            component = &velocity.x;
            *component++ *= rail_exit_speed;
            *component++ *= rail_exit_speed;
            *component *= rail_exit_speed;
#endif
            player_set_speed(work->tick, velocity.x, velocity.y, velocity.z);
            work->mode = RAIL_IDLE;
            work->timer = REVERSE_REENTRY_STEPS;
        }
        break;
    case RAIL_IDLE:
        near_slot = player_near(&first, RAIL_BOARD_REACH);
        if (near_slot >= 1) {
            if (work->timer) {
                work->timer = RAIL_REENTRY_STEPS;
            } else {
                work->tick = near_slot - 1;
                work->mode = RAIL_FORWARD;
                SEGMENT(task) = 0;
                FRACTION(task) = 0.0F;
                player_request(work->tick, PLAYER_REQUEST_SPIN, RAIL_BOARD_ARG);
                work->scl.z = 0.0F;
            }
        }
        if (work->timer) {
            work->timer--;
        }
    }
    if (task->child == NULL) {
        exit = task_create_child(TASK_WORK | TASK_FORCE, gadget_rail_exit_init,
                                 task);
        if (exit == NULL) {
            return;
        }
        exit->work->pos = last;
        if (forward.y < 0.0F) {
            exit->work->ang.x = HALF_TURN;
        }
        exit->work->ang.y = RAIL(task)->exit_yaw;
        /* @hack redundant: a test of exit, which the test after its creation
         * has made */
        if (exit == NULL) {
            return;
        }
        intake = task_create_child(TASK_WORK, rail_intake_init, exit);
        if (intake != NULL) {
            intake->work->pos = RAIL(task)->intake_position;
            intake->work->scl = first;
            intake->work->ang.x = 0;
            intake->work->ang.z = 0;
            intake->work->ang.y = RAIL(task)->intake_yaw;
        }
        /* @hack redundant: another test of exit, which the test after its
         * creation has made */
        if (exit == NULL) {
            return;
        }
        if (RAIL(task)->marker_spacing < LEAST_MARKER_SPACING) {
            return;
        }
        marker_segment = 1;
        marker_fraction = FIRST_MARKER_FRACTION;
        markers = 0;
        roll = 0;
        while (!rail_advance(RAIL(task)->markers, &marker_segment,
                             &marker_fraction, RAIL(task)->marker_spacing,
                             &position, &velocity) &&
               markers++ < MOST_MARKERS) {
            marker =
                task_create_child(TASK_WORK, gadget_rail_marker_step, exit);
            if (marker != NULL) {
                marker->work->pos = position;
                marker->work->ang.x =
                    (Angle)(atan2f(-velocity.y,
                                   njSqrt((velocity.z * velocity.z) +
                                          (velocity.x * velocity.x))) *
                            ANGLE_PER_RADIAN);
                marker->work->ang.y =
                    (Angle)(atan2f(velocity.x, velocity.z) * ANGLE_PER_RADIAN);
                marker->work->ang.z = roll;
                marker->draw = gadget_rail_marker_draw;
                roll += rail_marker_roll;
            }
        }
    } else if (rail_reset_requested) {
        rail_reset_requested = 0;
        task_destroy_children(task->child);
        task_kill(task->child);
    }
}

/* the exit draws while the view z of its pos lies from EXIT_NEAR to EXIT_FAR.
 * a marker draws from MARKER_NEAR to MARKER_FAR */
#define EXIT_NEAR (-60.0F)
#define EXIT_FAR 600.0F
#define MARKER_NEAR (-50.0F)
#define MARKER_FAR 800.0F
/* the exit opens as sin(ang.z) climbs from 0 to 1. up to TILT_SHARE, its
 * parts other than the panel tilt by sin(ang.z) times TILT_RATE. past it they
 * hold MOST_TILT, and the panel rolls by the rest of the climb times
 * ROLL_RATE. each rate is an angle per unit of sin(ang.z). each reaches
 * MOST_TILT at the end of its part of the climb. the panel rises PANEL_RISE
 * times sin(ang.z) */
#define TILT_SHARE 0.3F
#define MOST_TILT 20024
#define TILT_RATE 66749.633F
#define ROLL_RATE 28606.986F
#define PANEL_RISE 3.0F
/* an opening exit closes once it has stayed open at QUARTER_TURN for more
 * than EXIT_OPEN_STEPS steps. a closed exit is solid once its tick, the
 * steps since a ride's last step, passes EXIT_SETTLE_STEPS. tick counts up to
 * EXIT_TICK_MOST */
#define EXIT_OPEN_STEPS 10
#define EXIT_SETTLE_STEPS 10
#define EXIT_TICK_MOST 100
#define EXIT_CLOSE_SOUND 22
/* a marker's brightness, in scl.x, halves its gap to 1 each step a player
 * lies within MARKER_REACH. each step it then keeps MARKER_FADE of itself,
 * and never falls below MARKER_DIM. the marker draws tinted an opaque gray,
 * whose red, green and blue are its brightness out of MOST_BRIGHTNESS */
#define MARKER_REACH 35.0F
#define MARKER_FADE 0.983F
#define MARKER_DIM 0.1F
#define OPAQUE_BLACK 0xff000000
#define MOST_BRIGHTNESS 255

static void gadget_rail_draw(struct task *task)
{
}

static void gadget_rail_exit_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;
    NJS_POINT3 view;
    Float opening;
    Angle roll;
    Angle tilt;

    njCalcPoint(NULL, &work->pos, &view);
    if (view.z < EXIT_NEAR || view.z > EXIT_FAR) {
        return;
    }
    opening = njSin(work->ang.z);
    if (opening > TILT_SHARE) {
        roll = (opening - TILT_SHARE) * ROLL_RATE;
        tilt = MOST_TILT;
    } else {
        roll = 0;
        tilt = opening * TILT_RATE;
    }
    part = &rail_exit_root;
    njSetTexture(&rail_exit_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    /* @hack redundant: a call of njSetTexture in a block no path reaches */
    if (0) {
        njSetTexture(NULL);
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(part->model);
    for (part = part->child; part != NULL; part = part->sibling) {
        if (part == &rail_exit_panel) {
            njTranslate(NULL, part->pos[0],
                        part->pos[1] + work->scl.x + (opening * PANEL_RISE),
                        part->pos[2]);
            njRotateY(NULL, part->ang[1]);
            njRotateZ(NULL, roll);
        } else {
            njTranslateEx((NJS_VECTOR *)part->pos);
            njRotateY(NULL, part->ang[1]);
            njRotateX(NULL, tilt);
        }
        njCnkEasyDrawModel(part->model);
        matrix_load_parent();
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(2);
}

static void gadget_rail_exit_destroy(struct task *task)
{
    rail_release_sensor(task);
}

static void rail_exit_step(struct task *task);

static void gadget_rail_exit_init(struct task *task)
{
    struct task_work *work = task->work;

    task->extra.p = syCalloc(1, sizeof(struct gadget_rail_sensor));
    if (task->extra.p == NULL) {
        return;
    }
#ifdef NONMATCHING
    /* where collision_init fails, this side frees the sensor and leaves the
     * exit unbuilt. the shipped code goes on without a collision record */
    if (!collision_init(task, rail_exit_shape, 1, COLLISION_OBJECT)) {
        syFree(task->extra.p);
        task->extra.p = NULL;
        return;
    }
#endif
    task->draw = gadget_rail_exit_draw;
    task->destructor = gadget_rail_exit_destroy;
    task->step = rail_exit_step;
#ifdef NONMATCHING
#else
    collision_init(task, rail_exit_shape, 1, COLLISION_OBJECT);
#endif
    RAIL_PLACE_SENSOR(task, work, rail_exit_collision);
    njCalcPoint(NULL, &RAIL_SENSOR(task)->object.model->center,
                &RAIL_SENSOR(task)->center);
    njPopMatrixEx();
}

static void rail_exit_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->tick < EXIT_TICK_MOST) {
        work->tick++;
    }
    switch (work->mode) {
    case RAIL_EXIT_OPENING:
        work->ang.z += rail_exit_open_rate;
        if (work->ang.z > QUARTER_TURN) {
            work->ang.z = QUARTER_TURN;
            work->timer++;
            if (work->timer > EXIT_OPEN_STEPS) {
                work->mode = RAIL_EXIT_CLOSING;
            }
        }
        break;
    case RAIL_EXIT_CLOSING:
        work->ang.z += rail_exit_close_rate;
        if (work->ang.z < EIGHTH_TURN) {
            sound_play(EXIT_CLOSE_SOUND, NULL, 0, 0);
            work->ang.z = 0;
            work->mode = RAIL_EXIT_IDLE;
        }
        break;
    default:
        work->timer = 0;
        work->ang.z = 0;
        break;
    }
    if (work->collision != NULL) {
        if (work->mode == RAIL_EXIT_IDLE && work->tick > EXIT_SETTLE_STEPS) {
            work->collision->shapes[0].push |= PUSH_ALL_BUT_PICKUPS;
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side: it writes
             * no shapes[1] past the exit's one shape */
#else
            /* @bug memory: writes shapes[1] past the exit's one shape, as
             * rail_step does, at 0x8c506d58 */
            work->collision->shapes[1].push |= PUSH_ALL_BUT_PICKUPS;
#endif
        }
        collision_register(task);
    }
    if (work->mode == RAIL_EXIT_IDLE && work->tick > EXIT_SETTLE_STEPS &&
        player_near(&RAIL_SENSOR(task)->center,
                    RAIL_SENSOR(task)->object.model->r + RAIL_SENSOR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void gadget_rail_marker_step(struct task *task)
{
    struct task_work *work = task->work;

    work->ang.z += rail_marker_spin;
    if (player_near(&work->pos, MARKER_REACH)) {
        work->scl.x = work->scl.x + ((1.0F - work->scl.x) * 0.5F);
    }
    work->scl.x *= MARKER_FADE;
    if (work->scl.x < MARKER_DIM) {
        work->scl.x = MARKER_DIM;
    }
}

static void gadget_rail_marker_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXLIST *textures;
    NJS_CNK_OBJECT *object;
    NJS_DIRECT_HEAD *build;
    int color;
    NJS_POINT3 view;
    int brightness;

    njCalcPoint(NULL, &work->pos, &view);
    if (view.z < MARKER_NEAR || view.z > MARKER_FAR) {
        return;
    }
    njFogDisable();
    gadget_rail_marker_art(&textures, &object, &build);
    njSetTexture(textures);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 1);
    brightness = work->scl.x * MOST_BRIGHTNESS;
    if (brightness > MOST_BRIGHTNESS) {
        brightness = MOST_BRIGHTNESS;
    }
    color = OPAQUE_BLACK | (brightness << RED_SHIFT) |
            (brightness << GREEN_SHIFT) | brightness;
    model_draw_tinted(object->model, color);
    njPopMatrixEx();
    njFogEnable();
}
