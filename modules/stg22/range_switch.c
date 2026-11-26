#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "stg22/crazy_gadget.h"
#include "stg22/gravity_switch.h"
#include "system/task.h"

/* after a tilt, the switch passes over touches for RETRIGGER_STEPS steps.
 * each step its timer gains WIND_STEP for each of those steps left, which no
 * path reads */
#define RETRIGGER_STEPS 30
#define WIND_STEP 15

/* the set editor's labels for ang.x and ang.z */
static const char gravity_pitch_caption[12] = "<- GRAV X";
static const char gravity_roll_caption[12] = "<- GRAV Z";

static void range_switch_destroy(struct task *task);
static void range_switch_step(struct task *task);
static void range_switch_draw(struct task *task);

/* clang-format off */
static struct collision_shape touch_sphere = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 15.0F, 0.0F},
    23.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */

void gadget_range_switch_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = range_switch_draw;
    task->step = range_switch_step;
    task->destructor = range_switch_destroy;
    collision_init(task, &touch_sphere, 1, COLLISION_OBJECT);
}

static void range_switch_destroy(struct task *task)
{
    (void)task;
}

static void range_switch_step(struct task *task)
{
    struct task_work *work = task->work;
    int up;

    /* @hack spelling: an else after the return, where an if of its own would
     * do */
    if (placement_try_leave(task)) {
        return;
    } else if (collision_next_player(task) && work->tick == 0) {
        /* up, as gravity_axis_impl.h's gravity_up_axis finds it for the tilted
         * gravity */
        up = COLLISION_UP_NEG_X;
        world_tilt_gravity(work->ang.x, work->ang.z);
        work->tick = RETRIGGER_STEPS;
        /* @hack redundant: a test of x's component that sets up to the
         * COLLISION_UP_NEG_X it holds already */
        if (world_gravity.x > GRAVITY_ON_AXIS) {
            up = COLLISION_UP_NEG_X;
        }
        if (world_gravity.x < -GRAVITY_ON_AXIS) {
            up = COLLISION_UP_X;
        }
        if (world_gravity.y > GRAVITY_ON_AXIS) {
            up = COLLISION_UP_NEG_Y;
        }
        if (world_gravity.y < -GRAVITY_ON_AXIS) {
            up = COLLISION_UP_Y;
        }
        if (world_gravity.z > GRAVITY_ON_AXIS) {
            up = COLLISION_UP_NEG_Z;
        }
        if (world_gravity.z < -GRAVITY_ON_AXIS) {
            up = COLLISION_UP_Z;
        }
        collision_set_up_axis(up);
    } else if (work->tick) {
        --work->tick;
    }
    work->timer += work->tick * WIND_STEP;
    /* @hack redundant: a test of tick, timer and flags with an empty body, in
     * a block no path reaches */
    if (0) {
        if (work->tick && work->timer && work->flags) {
        }
    }
    collision_register(task);
}

static void range_switch_draw(struct task *task)
{
#ifdef NONMATCHING
    /* this side draws nothing, as the matching side's return does, without the
     * unreached draw that the hack tag names */
    (void)task;
#else
    /* @hack redundant: a draw of the switch's models after a return, which no
     * path reaches */
    struct task_work *work = task->work;
    Float width;
    Float height;
    Float depth;
    Float pulse;
    extern NJS_CNK_MODEL gadget_switch_base_model[1];
    extern NJS_CNK_MODEL gadget_switch_panel_model[1];
    extern NJS_CNK_MODEL gadget_switch_lever_model[1];

#define DRAW_PART(model, x, y, z)                                              \
    njPushMatrixEx();                                                          \
    njScale(NULL, x, y, z);                                                    \
    njCnkEasyDrawModel(model);                                                 \
    njPopMatrixEx()

    if (1) {
        return;
    }
    width = work->scl.x;
    height = work->scl.y;
    depth = work->scl.z;
    pulse = work->tick;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->timer);
    njPushMatrixEx();
    DRAW_PART(gadget_switch_base_model, width, height, depth);
    DRAW_PART(gadget_switch_panel_model, pulse, pulse, pulse);
    DRAW_PART(gadget_switch_lever_model, width, height, depth);
    DRAW_PART(gadget_switch_base_model, pulse, pulse, pulse);
    DRAW_PART(gadget_switch_panel_model, width, height, depth);
    DRAW_PART(gadget_switch_lever_model, pulse, pulse, pulse);
    DRAW_PART(gadget_switch_base_model, width, height, depth);
    DRAW_PART(gadget_switch_panel_model, pulse, pulse, pulse);
    DRAW_PART(gadget_switch_lever_model, width, height, depth);
    njPopMatrix(2);
#undef DRAW_PART
#endif
}

/* gravity_switch.c's code starts at the next 32-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
