/* Green Hill's rides, which carry a player along a path from its last point
 * back to its first */
#include <ninja.h>
#include <sg_xpt.h>

#include "player/player.h"
#include "stage/path.h"
#include "stg31/path_ride.h"
#include "system/task.h"

/* a ride's work->mode. RIDE_FORWARD carries a player toward the last point,
 * though no code starts such a ride */
enum ride_mode { RIDE_WAIT, RIDE_FORWARD, RIDE_BACKWARD };

/* ride_point puts each index past an end END_EXTENSION further from the end
 * than the index before */
#define END_EXTENSION 100.0F
/* a waiting ride takes a player within ENTRY_RADIUS of the last point */
#define ENTRY_RADIUS 15.0F
/* the ride's speed starts at 0 and rises by RIDE_ACCELERATION a step, to
 * top_speed */
#define RIDE_ACCELERATION 0.2F
/* a released player leaves along the line out of the path's end, at
 * LAUNCH_SPEED */
#define LAUNCH_SPEED 3.0F
/* a waiting ride takes no player while its work's timer counts down. a
 * release at the first point sets the timer to START_DELAY. a player near the
 * last point while it runs sets it to REENTRY_DELAY. so would a release at the
 * last point, under RIDE_FORWARD */
#define REENTRY_DELAY 50
#define START_DELAY 100
/* the arg an entering rider's PLAYER_REQUEST_SPIN takes: ANIM_SPIN's number,
 * which the request's handlers play without reading it */
#define ENTRY_ARG 11

/* a ride's task is made with TASK_WORK alone, so its motion, force and extra
 * hold no records. the ride keeps values in them instead: the segment and the
 * fraction ride_advance takes, and the speed. ride_destroy nulls them, since
 * task_destroy frees each that isn't null. unk_3c holds the path. the work's
 * tick holds the rider's slot, and its timer the wait */
#define RIDE_PATH(task) ((struct path *)(task)->unk_3c.p)
#define SEGMENT(task) ((task)->motion.l)
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FRACTION(task) (*(Float *)&(task)->extra)

static Float top_speed = 25.0F;

static void ride_init(struct task *task);
static void ride_destroy(struct task *task);
static void ride_step(struct task *task);
static void ride_draw(struct task *task);

#ifdef NONMATCHING
/* the plain forms the hack tag names */
#pragma inline(path_difference, path_add, path_scale, release_speed)
static void path_difference(const NJS_POINT3 *a, const NJS_POINT3 *b,
                            NJS_VECTOR *out)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static void path_add(const NJS_POINT3 *base, const NJS_VECTOR *offset,
                     NJS_POINT3 *out)
{
    out->x = offset->x + base->x;
    out->y = offset->y + base->y;
    out->z = offset->z + base->z;
}

static void path_scale(NJS_VECTOR *vector, Float scale)
{
    vector->x *= scale;
    vector->y *= scale;
    vector->z *= scale;
}

static void release_speed(NJS_VECTOR *velocity)
{
    velocity->x *= LAUNCH_SPEED;
    velocity->y *= LAUNCH_SPEED;
    velocity->z *= LAUNCH_SPEED;
}

#define PATH_DIFFERENCE(a, b, out) path_difference(a, b, out)
#else
/* @hack spelling: x, y and z reached through Float pointers stepped across
 * them, and the difference a macro, where the NONMATCHING side's functions
 * would do */
#pragma inline(path_add, path_scale, release_speed)
#define PATH_DIFFERENCE(a, b, out)                                             \
    do {                                                                       \
        Float *result = &(out)->x;                                             \
        const Float *end = &(a)->x;                                            \
        const Float *start = &(b)->x;                                          \
        *result = *end++ - *start++;                                           \
        ++result;                                                              \
        *result = *end++ - *start++;                                           \
        ++result;                                                              \
        *result = *end - *start;                                               \
    } while (0)

static void path_add(const NJS_POINT3 *base, const NJS_VECTOR *offset,
                     NJS_POINT3 *out)
{
    Float *result = &out->x;
    const Float *delta = &offset->x;
    const Float *origin = &base->x;

    *result = *delta++ + *origin++;
    ++result;
    *result = *delta++ + *origin++;
    ++result;
    *result = *delta + *origin;
}

static void path_scale(NJS_VECTOR *vector, Float scale)
{
    Float *component = &vector->x;
    *component++ *= scale;
    *component++ *= scale;
    *component *= scale;
}

static void release_speed(NJS_VECTOR *velocity)
{
    Float *component = &velocity->x;
    *component++ *= LAUNCH_SPEED;
    *component++ *= LAUNCH_SPEED;
    *component *= LAUNCH_SPEED;
}

#endif

/* set *out to points[index], where count points make the path. an index past an
 * end gives a point on a line through that end. the line comes from the third
 * point from the end. returns TRUE for such an index */
static Bool ride_point(NJS_POINT3 *points, int count, int index,
                       NJS_POINT3 *out)
{
    NJS_VECTOR direction;
    Bool outside;

    if (index >= count) {
        PATH_DIFFERENCE(&points[count - 1], &points[count - 3], &direction);
        path_scale(&direction, ((index - count + 1) * END_EXTENSION) /
                                   njScalor(&direction));
        path_add(&points[count - 1], &direction, out);
        outside = TRUE;
    } else if (index < 0) {
#ifdef NONMATCHING
        /* PATH_DIFFERENCE, the plain form the hack tag names */
        PATH_DIFFERENCE(&points[0], &points[2], &direction);
#else
        /* @hack spelling: the difference written out, its pointers stepped
         * after each store, where PATH_DIFFERENCE would do */
        {
            Float *result = &direction.x;
            Float *origin = &points[0].x;
            Float *inner = &points[2].x;
            *result++ = *origin - *inner;
            ++origin;
            ++inner;
            *result++ = *origin - *inner;
            ++origin;
            ++inner;
            *result = *origin - *inner;
        }
#endif
        path_scale(&direction, (-index * END_EXTENSION) / njScalor(&direction));
        path_add(&points[0], &direction, out);
        outside = TRUE;
    } else {
        *out = points[index];
        outside = FALSE;
    }
    return outside;
}

#define BLEND(out, a, a_weight, b, b_weight)                                   \
    do {                                                                       \
        (out)->x = ((a).x * (a_weight)) + ((b).x * (b_weight));                \
        (out)->y = ((a).y * (a_weight)) + ((b).y * (b_weight));                \
        (out)->z = ((a).z * (a_weight)) + ((b).z * (b_weight));                \
    } while (0)

/* move travel along path, from fraction of the way through segment. segment n
 * runs from point n to point n + 1. a positive travel moves toward the last
 * point, and any other toward the first. *out takes the place reached.
 * *segment takes its segment, and *fraction how far through it the place lies,
 * from 0 to 1. returns TRUE where a segment it passed through has an end past
 * the path's first or last point.
 *
 * on a path whose flags aren't PATH_LAYOUT_POS alone, it changes nothing and
 * returns FALSE */
static Bool ride_advance(struct path *path, Sint32 *segment, Float *fraction,
                         Float travel, NJS_POINT3 *out)
{
    NJS_POINT3 *points;
    int count;
    Bool outside;
    /* @hack spelling: next, a local that steps with index in each loop,
     * where index + 1 would do */
    int next;
    int index;
    Float length;
    Float from;
    Float remaining;
    Float start_weight;
    Float end_weight;
    NJS_POINT3 *out_copy;
    NJS_POINT3 start_point;
    NJS_POINT3 end_point;

    /* @hack spelling: out_copy, a copy of out, where out would do */
    out_copy = out;
    outside = FALSE;
    if (path->flags == PATH_LAYOUT_POS) {
        count = path->count;
        points = path->points;
        /* @hack redundant: a call no path reaches */
        if (0) {
            njDistanceP2P(points, NULL);
        }
        if (travel > 0.0F) {
            for (index = *segment, next = index + 1, from = *fraction;;
                 ++index, from = 0.0F, ++next) {
                if (ride_point(points, count, index, &start_point)) {
                    outside = TRUE;
                }
                if (ride_point(points, count, next, &end_point)) {
                    outside = TRUE;
                }
                length = njDistanceP2P(&start_point, &end_point);
                /* @hack spelling: length * from stored in remaining before
                 * the subtraction, where length - length * from would do */
                remaining = length * from;
                remaining = length - remaining;
                if (remaining < 0.0F) {
                    remaining = 0.0F;
                }
                if (travel > remaining) {
                    travel -= remaining;
                    continue;
                }
                start_weight = (remaining - travel) / length;
                end_weight = 1.0F - start_weight;
                if (end_weight < 0.0F) {
                    end_weight = 0.0F;
                    start_weight = 1.0F;
                }
                *segment = index;
                *fraction = end_weight;
                BLEND(out_copy, start_point, start_weight, end_point,
                      end_weight);
                return outside;
            }
        } else {
            for (index = *segment, next = index + 1, from = *fraction;;
                 --index, from = 1.0F, --next) {
                if (ride_point(points, count, index, &start_point)) {
                    outside = TRUE;
                }
                if (ride_point(points, count, next, &end_point)) {
                    outside = TRUE;
                }
                length = njDistanceP2P(&start_point, &end_point);
                remaining = length * from;
                /* @hack spelling: length - remaining kept in end_weight for
                 * the test, where testing length - remaining would do */
                end_weight = length;
                end_weight -= remaining;
                if (end_weight < 0.0F) {
                    remaining = length;
                }
                if (-travel > remaining) {
                    travel += remaining;
                    continue;
                }
                /* @hack spelling: travel added into remaining before the
                 * division, where (remaining + travel) / length would do */
                remaining += travel;
                end_weight = remaining / length;
                start_weight = 1.0F - end_weight;
                if (start_weight < 0.0F) {
                    end_weight = 1.0F;
                    start_weight = 0.0F;
                }
                *segment = index;
                *fraction = end_weight;
                BLEND(out_copy, start_point, start_weight, end_point,
                      end_weight);
                return outside;
            }
        }
    }
    return FALSE;
}

struct task *green_hill_path_ride_create(struct path *path, struct task *parent)
{
    struct task *task = task_create_child(TASK_WORK, ride_init, parent);

    /* @hack spelling: a return of task in the test's arm and of NULL after
     * it, where one return of task would do */
    if (task) {
        task->unk_3c.p = path;
        return task;
    }
    return NULL;
}

static void ride_init(struct task *task)
{
    task->draw = ride_draw;
    task->step = ride_step;
    task->destructor = ride_destroy;
    SEGMENT(task) = 0;
    FRACTION(task) = 0.0F;
    SPEED(task) = 0.0F;
}

static void ride_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

static void ride_step(struct task *task)
{
    struct task_work *work;
    struct path *path;
    NJS_POINT3 *points;
    int count;
    int player;
    Bool outside;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    NJS_POINT3 first;
    NJS_POINT3 last;
    NJS_VECTOR start_direction;
    NJS_VECTOR end_direction;
    NJS_POINT3 position;
    NJS_VECTOR velocity;

    path = RIDE_PATH(task);
    points = path->points;
    count = path->count;
    work = task->work;
    /* @hack redundant: a copy of the first point, which nothing reads */
    first = points[0];
    PATH_DIFFERENCE(&points[0], &points[1], &start_direction);
    last = points[count - 1];
    PATH_DIFFERENCE(&points[count - 1], &points[count - 2], &end_direction);
    njUnitVector(&start_direction);
    njUnitVector(&end_direction);
    /* @hack redundant: a block no path reaches, which stores the task in its
     * own motion and extra */
    if (0) {
        task->motion.p = task;
        task->extra.p = task;
    }
    /* @hack spelling: RIDE_PATH(task) read again in each of the switch's
     * ride_advance calls, where path would do */
    switch (work->mode) {
    case RIDE_FORWARD:
        outside = ride_advance(RIDE_PATH(task), &SEGMENT(task), &FRACTION(task),
                               SPEED(task), &position);
        SPEED(task) += RIDE_ACCELERATION;
        if (SPEED(task) > top_speed) {
            SPEED(task) = top_speed;
        }
        if (!outside) {
            player_set_pos(work->tick, position.x, position.y, position.z);
        } else {
            work->mode = RIDE_WAIT;
            work->timer = REENTRY_DELAY;
            player_request(work->tick, PLAYER_REQUEST_RELEASE, 0);
            velocity = end_direction;
            release_speed(&velocity);
            player_set_speed(work->tick, velocity.x, velocity.y, velocity.z);
        }
        break;
    case RIDE_BACKWARD:
        outside = ride_advance(RIDE_PATH(task), &SEGMENT(task), &FRACTION(task),
                               -SPEED(task), &position);
        SPEED(task) += RIDE_ACCELERATION;
        if (SPEED(task) > top_speed) {
            SPEED(task) = top_speed;
        }
        if (!outside) {
            player_set_pos(work->tick, position.x, position.y, position.z);
        } else {
            player_request(work->tick, PLAYER_REQUEST_RELEASE, 0);
            velocity = start_direction;
            release_speed(&velocity);
            player_set_speed(work->tick, velocity.x, velocity.y, velocity.z);
            work->mode = RIDE_WAIT;
            work->timer = START_DELAY;
        }
        break;
    case RIDE_WAIT:
        player = player_near(&last, ENTRY_RADIUS);
        if (player >= 1) {
            if (work->timer) {
                work->timer = REENTRY_DELAY;
            } else {
                work->tick = player - 1;
                work->mode = RIDE_BACKWARD;
                SEGMENT(task) = count - 2;
                FRACTION(task) = 1.0F;
                player_request(work->tick, PLAYER_REQUEST_SPIN, ENTRY_ARG);
                SPEED(task) = 0.0F;
            }
        }
        if (work->timer) {
            --work->timer;
        }
        break;
    }
}

static void ride_draw(struct task *task)
{
}
