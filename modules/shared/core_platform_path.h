/* the paths of the moving platforms STEP, STEPB and LASTSTEP, and their motion
 * record */
#ifndef SHARED_CORE_PLATFORM_PATH_H
#define SHARED_CORE_PLATFORM_PATH_H
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "system/task.h"

/* a platform's motion record, at its task's motion. collision is its node in
 * the world. the node stands bob_height above the platform's place on its path.
 * origin is the placement's pos, where each path starts */
struct core_platform_path {
    NJS_OBJECT collision;
    NJS_POINT3 origin;
    NJS_POINT3 pivot; /* an arc's center */
    Angle orbit_angle; /* an arc's turn from origin, to QUARTER_TURN */
    Float bob_height;
    Angle bob_phase; /* the bob's angle */
    Float travel; /* the part of its length a horizontal platform has gone */
};

enum core_path_kind {
    PATH_STILL,
    PATH_VERTICAL,
    PATH_HORIZONTAL,
    PATH_ARC,
    PATH_RISE,
    PATH_SINK,
    PATH_COUNT
};

/* a placement's path, its wait at each end in steps, and its speed in units a
 * step. the wait is scl.z cut to a whole number after WAIT_ROUNDING is
 * added. the speed is scl.y plus SPEED_BASE. ARC_SPEED gives an arc's speed
 * in degrees a step, by the code's 2 pi, TWO_PI */
#define PATH_KIND(work) (((work)->ang.x & 15) % PATH_COUNT)
#define END_WAIT(work) ((int)((work)->scl.z + WAIT_ROUNDING))
#define PATH_SPEED(work) ((work)->scl.y + SPEED_BASE)
#define WAIT_ROUNDING 0.1F
#define SPEED_BASE 0.1F
#define ARC_SPEED(work)                                                        \
    (PATH_SPEED(work) / ((work)->scl.x * TWO_PI) * DEGREES_PER_TURN)
#define TWO_PI 6.28F
#define DEGREES_PER_TURN 360.0F
/* the slowest speed of a path, in units a step, and of an arc, in degrees a
 * step. an arc sweeps ARC_DEGREES */
#define MIN_PATH_SPEED 0.01F
#define MIN_ARC_SPEED 0.1F
#define ARC_DEGREES 90.0F
/* a rising or sinking platform moves once ridden after more than
 * RIDER_COOLDOWN steps without a rider. the work's tick counts those steps, up
 * to MAX_EMPTY_STEPS. the init starts it at START_READY, so the first ride
 * moves the platform. at the far end, a player within TRIGGER_RADIUS of
 * origin sends the platform back as a ride does */
#define RIDER_COOLDOWN 30
#define START_READY 120
#define MAX_EMPTY_STEPS 125
#define TRIGGER_RADIUS(work)                                                   \
    ((((Uint8)((work)->ang.x >> 8) * 10.0F) + 10.0F) * 2.0F)
/* the bits of a rising or sinking platform's submode */
#define AWAY_FROM_START 0x1
#define IN_TRANSIT 0x2
/* a platform's node takes the size class that its model's radius plus
 * CLASS_MARGIN fits */
#define CLASS_MARGIN 15.0F

/* set result to the part of its path that a platform has gone at frame. a cycle
 * takes period steps out and period back. each half waits wait steps, then
 * moves at an even rate. result is held within 0 to 1 */
/* @hack spelling: a macro, where an inline function returning the part would
 * do */
#define SAMPLE_TRAVEL(result, frame, period, wait)                             \
    do {                                                                       \
        int tick = (frame) % ((period) * 2);                                   \
        if (tick < (period)) {                                                 \
            if (tick < (wait)) {                                               \
                (result) = 0.0F;                                               \
            } else {                                                           \
                (result) =                                                     \
                    (Float)(tick - (wait)) / (Float)((period) - (wait));       \
            }                                                                  \
        } else {                                                               \
            tick -= (period);                                                  \
            if (tick < (wait)) {                                               \
                (result) = 1.0F;                                               \
            } else {                                                           \
                (result) = 1.0F - ((Float)(tick - (wait)) /                    \
                                   (Float)((period) - (wait)));                \
            }                                                                  \
        }                                                                      \
        if ((result) < 0.0F) {                                                 \
            (result) = 0.0F;                                                   \
        }                                                                      \
        if ((result) > 1.0F) {                                                 \
            (result) = 1.0F;                                                   \
        }                                                                      \
    } while (0)

#define MIN_RADIUS 0.1F

/* start path at work's placement, with no bob and no turn. pivot lies scl.x
 * from pos along x turned by ang.z, or at pos where scl.x is below MIN_RADIUS.
 * offset is the caller's scratch point */
/* @hack spelling: a macro, where an inline function taking path and offset
 * would do */
#define CORE_PATH_START(path, work, offset)                                    \
    do {                                                                       \
        (path)->origin = (work)->pos;                                          \
        if ((work)->scl.x < MIN_RADIUS) {                                      \
            (path)->pivot = (work)->pos;                                       \
        } else {                                                               \
            (offset).x = (work)->scl.x;                                        \
            (offset).y = 0.0F;                                                 \
            (offset).z = 0.0F;                                                 \
            njPushMatrixEx();                                                  \
            njUnitMatrix(NULL);                                                \
            njTranslateEx(&(work)->pos);                                       \
            njRotateY(NULL, (work)->ang.z);                                    \
            njCalcPoint(NULL, &(offset), &(offset));                           \
            njPopMatrixEx();                                                   \
            (path)->pivot = (offset);                                          \
        }                                                                      \
        (path)->bob_height = 0.0F;                                             \
        (path)->bob_phase = 0;                                                 \
        (path)->orbit_angle = 0;                                               \
    } while (0)

/* place node at work's pos, turned by ang.y alone */
/* @hack spelling: a macro, where an inline function taking node and work would
 * do */
#define CORE_PATH_PLACE(node, work)                                            \
    do {                                                                       \
        (node)->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);         \
        (node)->ang[2] = 0;                                                    \
        (node)->ang[0] = 0;                                                    \
        (node)->ang[1] = (work)->ang.y;                                        \
        (node)->pos[0] = (work)->pos.x;                                        \
        (node)->pos[1] = (work)->pos.y;                                        \
        (node)->pos[2] = (work)->pos.z;                                        \
    } while (0)

#define CORE_PATH(task) ((struct core_platform_path *)(task)->motion.p)

#pragma inline(core_path_release)
static void core_path_release(struct task *task)
{
    if (CORE_PATH(task)->collision.model) {
        world_remove(task, &CORE_PATH(task)->collision);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#endif
