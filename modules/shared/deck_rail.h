/* the rail that CONTA, CONTB, CONTC and TRUCK ride. pos places the rail's
 * middle, and ang.y turns it. each end lies scl.x from the middle along the
 * rail */
#ifndef SHARED_DECK_RAIL_H
#define SHARED_DECK_RAIL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "player/player.h"
#include "system/task.h"

/* the record at task->motion, which TRUCK's begins with. collision is the
 * copy of the collision node that the world holds. offset is the place on the
 * rail, from its middle, in the axes ang.y turns */
struct deck_rail {
    NJS_OBJECT collision;
    NJS_POINT3 offset;
};
#define RAIL(task) ((struct deck_rail *)(task)->motion.p)
/* the speed along the rail, a Float kept in the pointer task->extra. the
 * destructor clears the word, which task_destroy would free */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define SPEED(task) (*(Float *)&(task)->extra)
/* a slide gains ACCEL_SHARE of TOP_SPEED a step, up to TOP_SPEED */
#define TOP_SPEED_MARGIN 0.1F
#define TOP_SPEED(work) ((work)->scl.y + TOP_SPEED_MARGIN)
#define ACCEL_SHARE 0.1F
/* the work's timer counts the steps from an arrival at an end. the slide
 * leaves that end once the count passes SETTLE_STEPS */
#define SETTLE_STEPS 30
/* CONTA, CONTC and TRUCK settle at an end. each step there sets the speed to
 * SPRING times the distance left plus DAMPING times the old speed. a step that
 * passes the end puts the work back at it, with -REBOUND times its speed */
#define SPRING 0.6F
#define DAMPING 0.92F
#define REBOUND 0.6F
#define SLIDE_SOUND 4
#define SLIDE_PRIORITY 1
#define SLIDE_VOLUME 0
#define SLIDE_SOUND_STEPS 5
/* RAIL_REGISTER_NODE sizes the node's class by its model's radius plus
 * REACH_MARGIN. a player within that sum of the model's center is near CONTB
 * or TRUCK */
#define REACH_MARGIN 5.0F
#define MOVING_SOLID (WORLD_MOVING | WORLD_SOLID)

/* the work's mode. RAIL_START, which the inits set, slides toward the end at
 * +scl.x as RAIL_OUT does. RAIL_BACK slides toward the end at -scl.x */
enum rail_direction { RAIL_START, RAIL_OUT, RAIL_BACK };

/* put node in the world for task as a moving solid, at pos and turned by
 * ang.y */
/* @hack spelling: a macro, where an inline function of task, work and node
 * would do */
#define RAIL_REGISTER_NODE(task, work, node)                                   \
    do {                                                                       \
        Uint32 flags;                                                          \
        Float radius;                                                          \
        (node)->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);         \
        (node)->ang[1] = (work)->ang.y;                                        \
        (node)->ang[2] = 0;                                                    \
        (node)->ang[0] = 0;                                                    \
        (node)->pos[0] = (work)->pos.x;                                        \
        (node)->pos[1] = (work)->pos.y;                                        \
        (node)->pos[2] = (work)->pos.z;                                        \
        flags = MOVING_SOLID;                                                  \
        radius = (node)->model->r + REACH_MARGIN;                              \
        if (radius <= WORLD_SIZE_CLASS_1_MAX) {                                \
            if (radius > WORLD_SIZE_CLASS_2_MAX) {                             \
                flags = MOVING_SOLID | WORLD_SIZE_CLASS(1);                    \
            } else if (radius > WORLD_SIZE_CLASS_3_MAX) {                      \
                flags = MOVING_SOLID | WORLD_SIZE_CLASS(2);                    \
            } else {                                                           \
                flags = MOVING_SOLID | WORLD_SIZE_CLASS(3);                    \
            }                                                                  \
        }                                                                      \
        world_add(flags, (task), (node));                                      \
    } while (0)

#pragma inline(rail_push_frame)
/* NOLINTNEXTLINE(clang-diagnostic-unused-function) */
static void rail_push_frame(struct task_work *work)
{
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
}

/* place node at the rail's offset. both force records take delta, the step's
 * move along the rail, in world axes. world_center takes the model's center
 * in the world. the node is in the world's collision only while a player is
 * near world_center. near is within the model's radius plus margin */
/* @hack spelling: a macro, where a function of the same arguments would
 * do */
#define RAIL_MOVE_NODE(task, work, node, delta, world_center, margin)          \
    do {                                                                       \
        rail_push_frame(work);                                                 \
        njCalcPoint(NULL, &RAIL(task)->offset, (NJS_POINT3 *)(node)->pos);     \
        njCalcVector(NULL, &(delta), &TASK_FORCES_OF(task)[0].speed);          \
        njTranslateV(NULL, &(node)->model->center);                            \
        njCalcPoint(NULL, &RAIL(task)->offset, &(world_center));               \
        njPopMatrix(1);                                                        \
        TASK_FORCES_OF(task)[1].speed = TASK_FORCES_OF(task)[0].speed;         \
        if (player_near(&(world_center), (node)->model->r + (margin))) {       \
            (work)->flags |= WORLD_ADDED;                                      \
        } else {                                                               \
            (work)->flags &= ~WORLD_ADDED;                                     \
        }                                                                      \
    } while (0)

#endif
