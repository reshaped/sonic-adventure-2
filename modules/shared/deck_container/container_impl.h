/* CONTA's and CONTC's init and callbacks, which shared/deck_container_a.c and
 * shared/deck_container_c.c compile, CONTC's under CROSS_RAIL. the includer
 * defines the container's art, boxes, cargo, RAIL_AXIS and
 * EMPTY_CARGO_COLLISION */
#ifndef SHARED_DECK_CONTAINER_CONTAINER_IMPL_H
#define SHARED_DECK_CONTAINER_CONTAINER_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/deck_container.h"
#include "shared/deck_rail.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* CARGO_BROKEN is the work's submode bit once the cargo breaks, and its
 * placement's state then. a container whose placement's state isn't 0 starts
 * with its cargo broken */
#define CARGO_BROKEN 1
/* the work's tick counts the steps from the cargo's break, from 1 up to
 * TICK_LIMIT. the cargo draws until the tick reaches CARGO_DRAW_STEPS. the
 * init gives a container whose cargo starts broken a tick of BROKEN_TICK */
#define TICK_LIMIT 60
#define CARGO_DRAW_STEPS 5
#define BROKEN_TICK 10
/* boxes holds BOXES shapes, the cargo's at CARGO_BOX */
#define BOXES 2
#define CARGO_BOX 1
/* the cargo's CARGO_PIECES pieces fly at PIECE_SPEED and up to PIECE_SPREAD
 * more, and rise at PIECE_RISE. they fall through pos.y to BREAK_FLOOR, but
 * land at pos.y where ang.x holds PIECES_LAND */
#define CARGO_PIECES 12
#define PIECE_SPEED 0.7F
#define PIECE_SPREAD 0.5F
#define PIECE_RISE 0.8F
#define PIECES_LAND 1
#define BREAK_FLOOR (-100000.0F)
#define BREAK_POINTS 20
#define BREAK_SOUND 0x1010
#define BREAK_PRIORITY 1
#define BREAK_SOUND_DURATION 80

static void deck_container_a_step(struct task *task);
static void deck_container_a_draw(struct task *task);
static void deck_container_a_destroy(struct task *task);

void deck_container_a_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *node;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct deck_rail));
    if (!task->motion.p) {
        return;
    }
    task->draw = deck_container_a_draw;
    task->step = deck_container_a_step;
    task->destructor = deck_container_a_destroy;
    work->mode = RAIL_START;
    node = &RAIL(task)->collision;
    *node = deck_container_a_collision;
    RAIL_REGISTER_NODE(task, work, node);
    model_object_create(task, NULL, NULL, boxes, BOXES, 0);
    if (task->child) {
        task->child->work->pos = work->pos;
        task->child->work->ang = work->ang;
        if (task->placement && placement_get_state(task)) {
            work->submode |= CARGO_BROKEN;
            work->tick = BROKEN_TICK;
            task->child->work->collision->shapes[CARGO_BOX].flags |= SHAPE_OFF;
        }
    }
    SPEED(task) = 0.0F;
}

static void deck_container_a_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->tick) {
        placement_set_state(task, CARGO_BROKEN);
    }
    if (RAIL(task)->collision.model) {
        world_remove(task, &RAIL(task)->collision);
    }
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#define POSITION(task) (RAIL(task)->offset.RAIL_AXIS)
/* the margin CONTA and CONTC pass RAIL_MOVE_NODE, where CONTB passes
 * REACH_MARGIN */
#define NEAR_MARGIN 15.0F

/* @hack redundant: task declared register, where a plain parameter would
 * do */
static void deck_container_a_step(register struct task *task)
{
    struct task_work *work;
    NJS_OBJECT *collision;
    NJS_POINT3 before;
    NJS_POINT3 center;
    NJS_VECTOR displacement;
    NJS_VECTOR velocity;
    /* @hack redundant: stopped declared register, where a plain local would
     * do */
    /* @hack spelling: stopped, a local for 0, where 0 in place would do */
    register Uint16 stopped;
#ifdef NONMATCHING
#else
    /* @hack redundant: current and previous declared register, where plain
     * locals would do */
    /* @hack spelling: the displacement's components walked by pointers, where
     * named components would do */
    Float *delta;
    register Float *current;
    register Float *previous;
#endif
    /* @hack redundant: rest_speed and damping, locals for 0.0F and DAMPING,
     * where the constants in place would do */
    Float rest_speed;
    Float damping;

    work = task->work;
    if (placement_try_leave(task)) {
        return;
    }
    before = RAIL(task)->offset;
    rest_speed = 0.0F;
    damping = DAMPING;
    stopped = 0;
    if (work->mode != RAIL_BACK) {
        if (work->timer) {
            POSITION(task) += SPEED(task);
            if (POSITION(task) > work->scl.x) {
                POSITION(task) = work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) = ((work->scl.x - POSITION(task)) * SPRING) +
                              (SPEED(task) * damping);
            }
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = RAIL_BACK;
                work->timer = stopped;
                SPEED(task) = rest_speed;
            }
        } else {
            SPEED(task) += TOP_SPEED(work) * ACCEL_SHARE;
            if (SPEED(task) > TOP_SPEED(work)) {
                SPEED(task) = TOP_SPEED(work);
            }
            POSITION(task) += SPEED(task);
            if (POSITION(task) > work->scl.x) {
                POSITION(task) = work->scl.x;
                work->timer++;
            }
            sound_play_at_for(SLIDE_SOUND, work, SLIDE_PRIORITY, SLIDE_VOLUME,
                              SLIDE_SOUND_STEPS, &work->pos);
        }
    } else {
        if (work->timer) {
            POSITION(task) -= SPEED(task);
            if (POSITION(task) < -work->scl.x) {
                POSITION(task) = -work->scl.x;
                SPEED(task) = -(SPEED(task) * REBOUND);
            } else {
                SPEED(task) = ((work->scl.x + POSITION(task)) * SPRING) +
                              (SPEED(task) * damping);
            }
            if (work->timer++ > SETTLE_STEPS) {
                work->mode = RAIL_OUT;
                work->timer = stopped;
                SPEED(task) = rest_speed;
            }
        } else {
            SPEED(task) += TOP_SPEED(work) * ACCEL_SHARE;
            if (SPEED(task) > TOP_SPEED(work)) {
                SPEED(task) = TOP_SPEED(work);
            }
            POSITION(task) -= SPEED(task);
            if (POSITION(task) < -work->scl.x) {
                POSITION(task) = -work->scl.x;
                work->timer++;
            }
            sound_play_at_for(SLIDE_SOUND, work, SLIDE_PRIORITY, SLIDE_VOLUME,
                              SLIDE_SOUND_STEPS, &work->pos);
        }
    }
    if (work->submode && !work->tick) {
        if (RAIL(task)->collision.model) {
            center.x = RAIL(task)->collision.pos[0];
            center.y = RAIL(task)->collision.pos[1];
            center.z = RAIL(task)->collision.pos[2];
            velocity.x = rest_speed;
            velocity.y = PIECE_RISE;
            velocity.z = rest_speed;
            debris_break_flags(
                &center, work->ang.y, cargo, CARGO_PIECES, PIECE_SPEED,
                PIECE_SPREAD,
                !(work->ang.x & PIECES_LAND) ? BREAK_FLOOR : work->pos.y,
                &velocity,
                DEBRIS_FADE_COLORS | DEBRIS_LANDING_BURST | DEBRIS_TRAIL);
#ifdef CROSS_RAIL
            score_add(BREAK_POINTS);
            sound_play_at_for(BREAK_SOUND, work, BREAK_PRIORITY, SOUND_LOUDEST,
                              BREAK_SOUND_DURATION, &work->pos);
#endif
        }
        work->tick = 1;
#ifndef CROSS_RAIL
        score_add(BREAK_POINTS);
        sound_play_at_for(BREAK_SOUND, work, BREAK_PRIORITY, SOUND_LOUDEST,
                          BREAK_SOUND_DURATION, &work->pos);
#endif
    }
    /* @hack spelling: the tick's test apart from its limit's, where one test
     * of both would do */
    if (work->tick) {
        if (work->tick < TICK_LIMIT) {
            work->tick++;
        }
    }
    if (task->child) {
        rail_push_frame(work);
        njCalcPoint(NULL, &RAIL(task)->offset, &center);
        njPopMatrix(1);
        task->child->work->pos = center;
        if (collision_next_attack(task->child)) {
            /* @hack spelling: CONTC's two stores in the other order, where
             * CONTA's order would do */
#ifdef CROSS_RAIL
            work->submode |= CARGO_BROKEN;
            task->child->work->collision->shapes[CARGO_BOX].flags |= SHAPE_OFF;
#else
            task->child->work->collision->shapes[CARGO_BOX].flags |= SHAPE_OFF;
            work->submode |= CARGO_BROKEN;
#endif
        }
    }
    if (RAIL(task)->collision.model) {
        collision = &RAIL(task)->collision;
        if (work->submode) {
            collision->model = EMPTY_CARGO_COLLISION->model;
        }
#ifdef NONMATCHING
        /* the displacement by named components, where the matching side walks
         * pointers for SHC's bytes */
        displacement.x = RAIL(task)->offset.x - before.x;
        displacement.y = RAIL(task)->offset.y - before.y;
        displacement.z = RAIL(task)->offset.z - before.z;
#else
        delta = &displacement.x;
        previous = &before.x;
        current = &RAIL(task)->offset.x;
        *delta++ = *current++ - *previous++;
        *delta++ = *current++ - *previous++;
        *delta = *current - *previous;
#endif
        RAIL_MOVE_NODE(task, work, collision, displacement, center,
                       NEAR_MARGIN);
    }
}

static void deck_container_a_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &deck_container_a_object;

    njSetTexture(&deck_container_a_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslateV(NULL, &RAIL(task)->offset);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(object->model);
    if (work->tick < CARGO_DRAW_STEPS) {
        object = object->child;
        njCnkEasyDrawModel(object->model);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
}

#endif
