/* ASHIBA, Green Hill's platform. the placement's task makes the platform as its
 * child. the child takes its kind and its travel from the parent's work */
#ifdef NONMATCHING
/* errno and EDIV: this side's platform_frame sets errno to EDIV */
#include <errno.h>
#endif
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg31/prop_art.h"
#include "stg31/platform_collision.h"
#include "stg31/platforms.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the platform's kind, scl.x as placed, modulo PLATFORM_KINDS */
enum platform_kind {
    PLATFORM_HORIZONTAL,
    PLATFORM_VERTICAL,
    PLATFORM_STILL,
    PLATFORM_FALLING,
    PLATFORM_KINDS
};

#define PLATFORM_KIND(work) ((Uint32)(work)->scl.x % PLATFORM_KINDS)
/* a moving platform travels along its local x, up to |scl.z| + MIN_TRAVEL from
 * its place, and back. its speed is in units a step. it is scl.y plus MIN_SPEED
 * where scl.y is above 0, and scl.y less MIN_SPEED otherwise. with a negative
 * speed it travels toward -x */
#define MIN_TRAVEL 10.0F
#define MIN_SPEED 0.1F
/* once player 0 stands on a falling platform, it waits FALL_DELAY steps,
 * then falls FALL_SPEED a step */
#define FALL_DELAY 60
#define FALL_SPEED 0.8F
/* a falling platform more than FALL_LIMIT below its parent's pos sets the
 * parent's submode to PLATFORM_FELL. no code of this source reads the
 * submode */
#define FALL_LIMIT 200.0F
#define PLATFORM_FELL 1

NJS_TEXLIST *green_hill_platform_texlists[] = {&green_hill_platform_texlist,
                                               NULL};
/* the SET editor's names of the kinds, which no code reads */
static char *platform_kind_names[PLATFORM_KINDS] = {
    "O-HUKU(HORZ)", "O-HUKU(VERT)", "STABLE", "FALL AFTER 1sec"};

static void platform_start(struct task *task);
static void platform_destroy(struct task *task);
static void platform_step(struct task *task);
static void platform_draw(struct task *task);

void green_hill_platform_init(struct task *task)
{
    if (!placement_try_leave(task) && !task->child) {
        task_create_child(TASK_WORK | TASK_FORCE, platform_start, task);
    }
}

static void platform_start(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *parent;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->draw = platform_draw;
    task->step = platform_step;
    task->destructor = platform_destroy;
    work->mode = 0;
    parent = task->parent->work;
    *PLATFORM_OBJECT(task) = green_hill_platform_collision;
    object = task->motion.p;
    PLACE_PLATFORM(object, work);
    flags = WORLD_SOLID |
            (PLATFORM_KIND(parent) != PLATFORM_STILL ? WORLD_MOVING : 0);
    radius = object->model->r + SIZE_MARGIN;
    ADD_SIZE_CLASS(flags, radius);
    world_add(flags, task, object);
}

static void platform_destroy(struct task *task)
{
    platform_release(task);
}

/* @bug arithmetic: a speed above the travel's length makes the period 0,
 * and the frame's remainder divides by it. each _modlu call, at 0x8c501758
 * and 0x8c50186e, then sets errno to EDIV and returns 0. the platform stays
 * at its place */
#ifdef NONMATCHING
/* this side keeps the result of the tag above: for a period of 0, it sets errno
 * to EDIV and returns 0, as _modlu does. C leaves a remainder by 0 undefined */
static Uint32 platform_frame(Uint32 period)
{
    if (period == 0) {
        errno = EDIV;
        return 0;
    }
    return game_state.steps % period;
}
#define PLATFORM_FRAME(period) platform_frame(period)
#else
#define PLATFORM_FRAME(period) (game_state.steps % (period))
#endif

#pragma inline(platform_travel)
static void platform_travel(struct task_work *work, struct task_work *parent)
{
    NJS_POINT3 offset;
    Float speed;
    int half_period;
    int period;
    int frame;

    speed = parent->scl.y;
    if (speed > 0.0F) {
        speed += MIN_SPEED;
    } else {
        speed -= MIN_SPEED;
    }
    /* @hack spelling: offset's y and z cleared in each arm, where clearing
     * them once after the arms would do */
    if (speed > 0.0F) {
        half_period = (_builtin_fabsf(parent->scl.z) + MIN_TRAVEL) / speed;
        frame = PLATFORM_FRAME(half_period * 2);
        if (frame >= half_period) {
            offset.x = ((half_period * 2) - frame) * speed;
            offset.y = 0.0F;
            offset.z = 0.0F;
        } else {
            offset.x = frame * speed;
            offset.y = 0.0F;
            offset.z = 0.0F;
        }
    } else {
        half_period = (_builtin_fabsf(parent->scl.z) + MIN_TRAVEL) / -speed;
        /* @hack spelling: period, a local in this arm alone, where
         * half_period * 2 as in the other arm would do */
        period = half_period * 2;
        frame = period - PLATFORM_FRAME(period);
        if (frame >= half_period) {
            offset.x = (period - frame) * speed;
            offset.y = 0.0F;
            offset.z = 0.0F;
        } else {
            offset.x = frame * speed;
            offset.y = 0.0F;
            offset.z = 0.0F;
        }
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&parent->pos);
    njRotateY(NULL, parent->ang.y);
    njCalcPoint(NULL, &offset, &work->pos);
    njPopMatrixEx();
    work->ang.y = parent->ang.y;
}

static void platform_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *parent = task->parent->work;
    NJS_POINT3 previous;

    previous = work->pos;
    switch (PLATFORM_KIND(parent)) {
    /* @hack spelling: an arm for each moving kind, where one arm for both
     * would do */
    case PLATFORM_HORIZONTAL:
        platform_travel(work, parent);
        break;
    /* @bug logic: PLATFORM_VERTICAL travels along x, as PLATFORM_HORIZONTAL
     * does, though the SET editor names it O-HUKU(VERT), a vertical round
     * trip. the jump table's entries at 0x8c501714 and 0x8c501718 both lead
     * to the same travel */
    case PLATFORM_VERTICAL:
        platform_travel(work, parent);
        break;
    case PLATFORM_FALLING:
        if (work->timer) {
            if (work->timer > FALL_DELAY) {
                work->pos.y -= FALL_SPEED;
                if (parent->pos.y - FALL_LIMIT > work->pos.y) {
                    parent->submode = PLATFORM_FELL;
                }
            } else {
                /* @hack spelling: the timer plus one assigned to it, where an
                 * increment would do */
                work->timer = work->timer + 1;
            }
        } else if (player_on_task(0, task)) {
            work->timer = 1;
        } else {
            work->pos = parent->pos;
            work->ang.y = parent->ang.y;
        }
        break;
    default:
        work->pos = parent->pos;
        work->ang.y = parent->ang.y;
        break;
    }
    if (PLATFORM_OBJECT(task)->model) {
        FOLLOW_WORK(task, work, previous);
        if (player_near(&work->pos,
                        PLATFORM_OBJECT(task)->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void platform_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&green_hill_platform_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(green_hill_platform_object->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
