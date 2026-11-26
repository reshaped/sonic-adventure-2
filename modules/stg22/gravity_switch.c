#include "stg22/fixture_models.h"
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg22/crazy_gadget.h"
#include "stg22/gravity_switch.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the steps since player 0 last held the handle, up to RELEASED_MOST, in
 * task->unk_3c. a press needs more than REARM_STEPS of them */
#define RELEASED_STEPS(task) ((task)->unk_3c.l)
#define RELEASED_MOST 200
#define REARM_STEPS 30
/* while a player lies within GRAB_REACH of the handle, the switch offers
 * PLAYER_MODE_HOLD_HANDLE as player 0's action. a press starts where player 0
 * is in that mode and a player lies within PRESS_REACH */
#define GRAB_REACH 8.0F
#define PRESS_REACH 11.0F
/* the handle lies HANDLE_OFFSET along the switch's own x from its pos */
#define HANDLE_OFFSET 4.0F
/* a press holds player 0 at the handle for PRESS_STEPS steps, then turns
 * gravity */
#define PRESS_STEPS 40
/* the bits of scl.x's whole part that hold the switch's type, its index in
 * local_directions */
#define TYPE_BITS 3
/* the switch's solid, its copy of gadget_switch_solid, at task->motion. the
 * solid collides while a player lies within its radius plus SOLID_MARGIN of
 * its pos. its size class takes its radius plus SIZE_MARGIN */
#define SOLID(task) ((NJS_OBJECT *)(task)->motion.p)
#define SOLID_MARGIN 32.0F
#define SIZE_MARGIN 5.0F
#define PRESS_SOUND 1
#define HOLD_SOUND 27
#define HUM_SOUND 0
/* the lever stands until a press has 1 / SINK_RATE steps left. it then tips
 * as those steps run out, by up to EIGHTH_TURN. once released, it stands
 * again over 1 / RISE_RATE steps */
#define SINK_RATE (0.9F / 20.0F)
#define RISE_RATE 0.05F
/* the arrow's texture scrolls scroll_step in v every scroll_period steps */
/* each step of a press plays HOLD_SOUND for HOLD_SOUND_DURATION steps. each
 * other step plays HUM_SOUND for HUM_SOUND_DURATION steps */
#define SOUND_PRIORITY 1
#define HOLD_SOUND_DURATION 30
#define HUM_SOUND_DURATION 5

static void lever_destroy(struct task *task);
static void lever_step(struct task *task);
static void lever_draw(struct task *task);

/* clang-format off */
static struct collision_shape lever_shape = {
    0, SHAPE_AXIS_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
    {-5.0F, 12.5F, 0.0F}, 5.0F, 12.5F, 9.0F, 0, {0, 0, 0},
};
/* clang-format on */
/* the set editor's labels for the four types */
static char *type_labels[] = {
    "<- TYPE(VERT-)",
    "<- TYPE(VERT+)",
    "<- TYPE(HORZ-)",
    "<- TYPE(HORZ+)",
};
/* each type's way, in the switch's own frame */
static NJS_VECTOR local_directions[] = {
    {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
/* the arrow's turn about its x, by the work's submode. where gravity points
 * along the type's way, the submode is the type. otherwise it is the type of
 * the opposite way */
static Angle panel_angles[] = {HALF_TURN, 0, 3 * QUARTER_TURN, QUARTER_TURN};
/* forward_gravity[up] is the unit gravity along an axis whose up axis, by
 * gravity_up_axis, is up */
static NJS_VECTOR forward_gravity[] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
};
/* reverse_gravity[up] is forward_gravity[up] negated */
static NJS_VECTOR reverse_gravity[] = {
    {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1},
};
static Sint16 scroll_step = 16;
static Uint32 scroll_period = 18;

void gadget_gravity_switch_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *solid;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->draw = lever_draw;
    task->step = lever_step;
    task->destructor = lever_destroy;
    collision_init(task, &lever_shape, 1, COLLISION_OBJECT);
    solid = task->motion.p;
    *solid = gadget_switch_solid;
    solid->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    solid->ang[1] = work->ang.y;
    solid->ang[2] = work->ang.z;
    solid->ang[0] = work->ang.x;
    solid->pos[0] = work->pos.x;
    solid->pos[1] = work->pos.y;
    solid->pos[2] = work->pos.z;
    flags = WORLD_SOLID;
    radius = solid->model->r + SIZE_MARGIN;
    if (!(radius > WORLD_SIZE_CLASS_1_MAX)) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, solid);
}

static void lever_destroy(struct task *task)
{
    if (SOLID(task)->model) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#include "stg22/gravity_axis_impl.h"

#pragma inline(select_panel)
static void select_panel(struct task_work *work, int opposite)
{
    /* @hack spelling: the type's two bits tested one by one, where
     * ((int)scl.x & TYPE_BITS) ^ opposite would do */
    if ((int)work->scl.x & 2) {
        if ((int)work->scl.x & 1) {
            work->submode = 3 ^ opposite;
        } else {
            work->submode = 2 ^ opposite;
        }
    } else if ((int)work->scl.x & 1) {
        work->submode = 1 ^ opposite;
    } else {
        work->submode = opposite;
    }
}

static void lever_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR direction;
    NJS_VECTOR gravity;
    NJS_POINT3 handle;
    int axis;
    Angle turn;
#ifdef NONMATCHING
#else
    Sint16 *short_turn;
#endif
    Float rate;
    Float dot;
    NJS_OBJECT *solid;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a call of select_panel in a block no path reaches,
     * with the step as its else */
    if (0) {
        select_panel(work, 0);
    } else {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateEx(&work->ang.x, 0);
        njCalcVector(NULL, &local_directions[(int)work->scl.x & TYPE_BITS],
                     &direction);
        njPopMatrixEx();
        axis = gravity_up_axis(&direction);
        /* @hack redundant: a call of gravity_up_axis whose result no code
         * reads */
        gravity_up_axis(&world_gravity);
        dot = njInnerProduct(&direction, &world_gravity);
        /* @hack redundant: the test of dot below -GRAVITY_ON_AXIS, whose
         * branch does as the else does */
        if (dot > GRAVITY_ON_AXIS) {
            select_panel(work, 0);
            gravity = reverse_gravity[axis];
        } else if (dot < -GRAVITY_ON_AXIS) {
            select_panel(work, 1);
            gravity = forward_gravity[axis];
        } else {
            select_panel(work, 1);
            gravity = forward_gravity[axis];
        }
        {
            NJS_POINT3 offset;
            /* @hack redundant: a point that only sizes the frame */
            NJS_POINT3 unused;
            (void)unused;
            offset.x = HANDLE_OFFSET;
            offset.y = 0.0F;
            offset.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateEx(&work->ang.x, 0);
            njCalcPoint(NULL, &offset, &handle);
            njPopMatrixEx();
        }
        if (player_near(&handle, PRESS_REACH)) {
            if (player_work[0]->mode == PLAYER_MODE_HOLD_HANDLE) {
                if (RELEASED_STEPS(task) > REARM_STEPS && !work->timer) {
                    sound_play(PRESS_SOUND, NULL, 0, 0);
                    work->timer = PRESS_STEPS;
                }
                RELEASED_STEPS(task) = 0;
            } else {
                if (player_near(&handle, GRAB_REACH)) {
                    player_records[0]->action = PLAYER_MODE_HOLD_HANDLE;
                } else {
                    player_records[0]->action = 0;
                }
                if (RELEASED_STEPS(task) < RELEASED_MOST) {
                    ++RELEASED_STEPS(task);
                }
            }
        } else if (RELEASED_STEPS(task) < RELEASED_MOST) {
            ++RELEASED_STEPS(task);
        }
        if (work->timer) {
            struct angles angles;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateEx(&work->ang.x, 0);
            matrix_get_angles(NULL, &angles);
            njPopMatrixEx();
            turn = -angles.y + HALF_TURN;
            turn = turn - player_work[0]->ang.y;
            /* player 0 turns toward HALF_TURN - angles.y by a third of turn's
             * low half, taken signed. that is a third of the shorter way
             * round. the bits above ANGLE_MASK are whole turns, and stay */
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            rate = (Sint16)turn;
            rate *= 1.0F / 3.0F;
            turn = (turn & ~ANGLE_MASK) | (Uint16)(Sint16)rate;
#else
            /* @hack type: turn's low half written through a Sint16 pointer,
             * where the mask above would do */
            short_turn = (Sint16 *)&turn;
            rate = *short_turn;
            rate *= 1.0F / 3.0F;
            *short_turn = rate;
#endif
            player_work[0]->ang.y += turn;
            player_motion[0]->aim_y = player_work[0]->ang.y;
            player_work[0]->ang.x = angles.x;
            player_work[0]->ang.z = angles.z;
            player_set_pos(0, handle.x, handle.y, handle.z);
            --work->timer;
            if (!work->timer) {
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                world_gravity = gravity;
                world_gravity_ang_x =
                    (Angle)(asinf(-gravity.z) * ANGLE_PER_RADIAN);
                world_gravity_ang_z =
                    -(Angle)(atan2f(-gravity.x, -gravity.y) * ANGLE_PER_RADIAN);
                collision_set_up_axis(gravity_up_axis(&world_gravity));
            }
            sound_play_at_for(HOLD_SOUND, task, SOUND_PRIORITY, 0,
                              HOLD_SOUND_DURATION, &work->pos);
        } else {
            sound_play_at_for(HUM_SOUND, work, SOUND_PRIORITY, 0,
                              HUM_SOUND_DURATION, &work->pos);
        }
        /* @hack redundant: an if (0) with an empty body, with the solid's
         * test as its else */
        if (0) {
        } else {
            solid = task->motion.p;
            if (player_near((NJS_POINT3 *)solid->pos,
                            solid->model->r + SOLID_MARGIN)) {
                work->flags |= WORLD_ADDED;
            } else {
                work->flags &= ~WORLD_ADDED;
            }
        }
    }
}

static void lever_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;
    Float fraction;
    Angle depression;

    njSetTexture(&gadget_gravity_switch_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    njPushMatrixEx();
    if (work->timer) {
        fraction = work->timer * SINK_RATE;
        if (fraction > 1.0F) {
            fraction = 1.0F;
        } else {
            /* @hack redundant: an empty else on the press's clamp */
        }
    } else {
        fraction = RELEASED_STEPS(task) * RISE_RATE;
        if (fraction > 1.0F) {
            fraction = 1.0F;
        }
    }
    fraction = 1.0F - fraction;
    depression = -(fraction * EIGHTH_TURN);
    part = &gadget_switch_body;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(part->model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    part = part->child;
    njTranslateEx((NJS_VECTOR *)part->pos);
    njRotateX(NULL, panel_angles[work->submode]);
    texture_anim_scroll(
        gadget_switch_arrow[0].model, &gadget_switch_scroll, 0,
        (scroll_step * (Sint16)(game_state.steps / scroll_period)) &
            (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(part->model);
    matrix_load_parent();
    part = part->sibling;
    njTranslateEx((NJS_VECTOR *)part->pos);
    njRotateZ(NULL, depression);
    njCnkEasyDrawModel(part->model);
    njPopMatrix(2);
}
