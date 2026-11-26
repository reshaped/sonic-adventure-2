/* the dash panel, KASOKU in a stage's object list. a player who touches it
 * on the ground is sent off along its angles */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dash_panel.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the panel's modes, in work->mode. nothing in this source sets MODE_IDLE */
enum dash_panel_mode { MODE_INIT, MODE_READY, MODE_IDLE };

/* what the first step gives a scl.x or a scl.y of 0 or below */
#define DEFAULT_SPEED 14.0F
#define DEFAULT_LOCK 60.0F

/* the slot of the player the panel sent last, or NO_PLAYER before the first
 * send */
#define LAST_SENT(work) ((work)->submode)
#define NO_PLAYER 13

/* the first step clears UNK_00. each draw that shows the panel adds
 * UNK_00_STEP to it, an eighteenth of a turn. nothing in this source reads
 * it */
#define UNK_00(task) ((task)->motion.w[0])
#define UNK_00_STEP (FULL_TURN / 18)
/* each step first takes 1 from WAIT, down to 0. the panel sends the player of
 * slot LAST_SENT again only at a WAIT of 0 */
#define WAIT(task) ((task)->extra.u)

#define VIEW_RADIUS 30.0F

#define SEND_SOUND 0x1003
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15
#define DASH_PANEL_BUILDS 1
#define DASH_PANEL_SHAPES 1

/* TRUE once a call of dash_panel_build has built the base model */
static Bool dash_panel_built = FALSE;
static struct model_build dash_panel_builds[DASH_PANEL_BUILDS] = {
    {MODEL_BUILD_ALONE_AS_LAND, &dash_panel_texlist, &dash_panel_object, NULL},
};
/* the panel's shape, which each panel's record uses in place */
/* clang-format off */
static struct collision_shape dash_panel_shapes[DASH_PANEL_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_HURTS_ALL, 0,
     {0.0F, 0.0F, 0.0F}, 10.0F, 4.44F, 0.0F, 0, {QUARTER_TURN, 0, 0}},
};
/* clang-format on */

Uint8 *dash_panel_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, dash_panel_builds, DASH_PANEL_BUILDS) ==
        DASH_PANEL_BUILDS) {
        dash_panel_built = TRUE;
    }
    return buf;
}

static void dash_panel_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (!sphere_in_front_no_depth(&work->pos, VIEW_RADIUS)) {
        return;
    }
    njPushMatrixEx();
    draw_control_set(DRAW_CONTROL_SHADOW);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y + HALF_TURN);
    njSetTexture(&dash_panel_anim_texlist);
    texture_anim_apply(&dash_panel_anim_model, &dash_panel_texture_anim,
                       game_state.steps);
    njCnkEasyDrawModel(&dash_panel_anim_model);
    njSetTexture(&dash_panel_texlist);
    njCnkEasyDrawModel(&dash_panel_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
    UNK_00(task) += UNK_00_STEP;
}

/* dash_panel_draw, with the base model drawn from its build */
static void dash_panel_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    if (!sphere_in_front_no_depth(&work->pos, VIEW_RADIUS)) {
        return;
    }
    njPushMatrixEx();
    draw_control_set(DRAW_CONTROL_SHADOW);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y + HALF_TURN);
    njSetTexture(&dash_panel_anim_texlist);
    texture_anim_apply(&dash_panel_anim_model, &dash_panel_texture_anim,
                       game_state.steps);
    njCnkEasyDrawModel(&dash_panel_anim_model);
    njSetTexture(&dash_panel_texlist);
    njDirectDrawObject(dash_panel_builds[0].built);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
    UNK_00(task) += UNK_00_STEP;
}

/* clear motion, force and extra, which task_destroy would free. the closing
 * task_kill(task) changes nothing: task_destroy frees the task once its
 * destructor returns */
static void dash_panel_destroy(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
    task_kill(task);
}

void dash_panel_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *other;
    /* @hack type: player a Sint8, where an int would do */
    Sint8 player;
    Float speed;
    Float lock;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: lock and speed, set before the switch, where
     * DEFAULT_LOCK and DEFAULT_SPEED in place would do */
    lock = DEFAULT_LOCK;
    speed = DEFAULT_SPEED;
    /* @hack redundant: an else no path takes, kept for its strings, which SHC
     * still places in this source's constants, and for its store of
     * MODE_READY */
    if (1) {
    } else {
        (void)"SCL_X IS FOR SPEED.";
        (void)"IF 0.0F OR LOWER,";
        (void)"IT'S ";
        (void)".";
        (void)"SCL_Y IS FOR DISABLE TIME.";
        work->mode = MODE_READY;
    }
    switch (work->mode) {
    case MODE_INIT:
        UNK_00(task) = 0;
        work->mode = MODE_READY;
        LAST_SENT(work) = NO_PLAYER;
        task->destructor = dash_panel_destroy;
        if (dash_panel_built) {
            task->draw = dash_panel_draw_built;
        } else {
            task->draw = dash_panel_draw;
        }
        collision_init_in_place(task, dash_panel_shapes, DASH_PANEL_SHAPES,
                                COLLISION_OBJECT);
        if (work->scl.x <= 0.0F) {
            work->scl.x = speed;
        }
        if (work->scl.y <= 0.0F) {
            work->scl.y = lock;
        }
        break;
    case MODE_READY:
        if (WAIT(task) > 0) {
            WAIT(task)--;
        }
        other = collision_next_player(task);
        if (other != NULL) {
            player = other == player_tasks[1];
            if ((player_work[player]->flags & PLAYER_WORK_ON_GROUND) &&
                (WAIT(task) == 0 || LAST_SENT(work) != player)) {
                struct angles turn;
                NJS_VECTOR spd = {0.0F, 0.0F, 0.0F};
                /* @hack spelling: ang, a pointer to turn, where turn's fields
                 * set and &turn passed would do */
                struct angles *ang = &turn;

                spd.x = work->scl.x;
                ang->x = work->ang.x;
                ang->y = work->ang.y;
                ang->z = work->ang.z;
                /* @hack type: ceilf's result converted to a Sint32, where its
                 * conversion to the Uint32 WAIT would do. the two agree while
                 * scl.y lies below 2^31 */
                WAIT(task) = (Sint32)ceilf(work->scl.y);
                player_launch_speed(player, &spd, ang, WAIT(task));
                sound_play(SEND_SOUND, NULL, 0, 0);
                vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                                VIBRATION_UNTIMED);
                LAST_SENT(work) = player;
            }
        }
        collision_register(task);
        break;
    /* @hack redundant: the empty case of MODE_IDLE, where leaving the mode out
     * of the switch would do */
    case MODE_IDLE:
        break;
    }
}
