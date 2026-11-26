/* the shutter's init and callbacks, which shared/deck_shutter.c compiles for
 * SHUT and shared/small_shutter.c for SHUTS, under SMALL_SHUTTER. the
 * includer defines the shutter's art, parts and shutter_box, and
 * BOX_HALF_WIDTH */
#ifndef SHARED_DECK_BREAKABLES_SHUTTER_IMPL_H
#define SHARED_DECK_BREAKABLES_SHUTTER_IMPL_H

#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/deck_shutter.h"
#include "shared/dynamite.h"
#include "system/angle.h"
#include "system/task.h"

/* the half height and depth of the lock-on box, whose half width is the
 * includer's BOX_HALF_WIDTH */
#define BOX_HALF_HEIGHT 60.0F
#define BOX_HALF_DEPTH 1.0F
/* BROKEN is the work's mode once the shutter has broken, and its placement's
 * state then. a shutter whose placement's state isn't 0 starts broken */
#define BROKEN 1
/* a broken shutter's task ends once its tick passes REMOVE_DELAY */
#define REMOVE_DELAY 5
/* the bit a broken shutter's step sets in its submode, which no code reads */
#define BROKEN_FLAG 0x80
/* the low byte of ang.x: the id of the charges whose blasts break the
 * shutter. an id of 0 keeps it whole. dynamite_count counts the charges of an
 * id left */
#define GROUP(work) ((Uint8)(work)->ang.x)
#define SHUTTER_SHAPES 1
/* the pieces fly at PIECE_SPEED and up to PIECE_SPREAD more, and rise at
 * PIECE_RISE. their floor probe starts PROBE_LIFT above pos */
#define PIECES 5
#define PIECE_SPEED 1.2F
#define PIECE_SPREAD 0.7F
#define PIECE_RISE 0.7F
#define PROBE_LIFT 1.0F
#define BREAK_POINTS 20

static void deck_shutter_destroy(struct task *task);
static void deck_shutter_step(struct task *task);
static void deck_shutter_draw(struct task *task);

void deck_shutter_init(struct task *task)
{
    struct task_work *work = task->work;
    Angle turn[3];
    NJS_VECTOR size;

    if (!placement_try_leave(task)) {
        task->draw = deck_shutter_draw;
        task->step = deck_shutter_step;
        task->destructor = deck_shutter_destroy;
        collision_init(task, shutter_box, SHUTTER_SHAPES, COLLISION_OBJECT);
        size.x = BOX_HALF_WIDTH;
        size.y = BOX_HALF_HEIGHT;
        size.z = BOX_HALF_DEPTH;
        turn[0] = 0;
        turn[1] = work->ang.y;
        turn[2] = 0;
        /* @hack type: the array's address cast to Angle *, where turn would
         * do */
        lockon_box_create(task, (Angle *)&turn, &size);
        if (task->placement && placement_get_state(task)) {
            work->mode = BROKEN;
        }
    }
}

static void deck_shutter_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement && work->mode == BROKEN) {
        placement_set_state(task, BROKEN);
    }
}

static void deck_shutter_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;
    struct angles ground;
    NJS_VECTOR velocity;
    Float floor;
    int group;

    if (placement_try_leave(task)) {
        return;
    }
#ifndef SMALL_SHUTTER
    if (work->mode != BROKEN) {
        collision_register(task);
    }
#endif
    if (work->mode != BROKEN) {
        /* @hack spelling: the assignment of group inside the test, where a
         * statement of its own would do */
        if ((group = GROUP(work)) && !dynamite_count(group)) {
            position = work->pos;
            velocity.x = 0.0F;
            velocity.y = PIECE_RISE;
            velocity.z = 0.0F;
            floor = surface_probe_floor(position.x, position.y + PROBE_LIFT,
                                        position.z, &ground);
            debris_break_flags(&position, work->ang.y, parts, PIECES,
                               PIECE_SPEED, PIECE_SPREAD, floor, &velocity,
                               DEBRIS_FADE_COLORS | DEBRIS_VANISH);
#ifndef SMALL_SHUTTER
            score_add(BREAK_POINTS);
#endif
            work->mode = BROKEN;
        }
    } else {
        work->submode |= BROKEN_FLAG;
        if (work->tick > REMOVE_DELAY) {
            if (task->placement) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            work->tick++;
        }
    }
#ifdef SMALL_SHUTTER
    if (work->mode != BROKEN) {
        collision_register(task);
    }
#endif
}

static void deck_shutter_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njSetTexture(&deck_shutter_texlist);
    object = &deck_shutter_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}

#endif
