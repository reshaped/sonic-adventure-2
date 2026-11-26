/* WARPCOLI in Egg Quarters' object list: an unseen cylinder that takes
 * player 0 elsewhere */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/fade.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/player.h"
#include "stg26/warp.h"
#include "system/game_state.h"
#include "system/task.h"

/* the steps since a player's touch, from 1 on the step that saw it. it is 0
 * while the warp waits for one. player 0 arrives at ARRIVAL_STEP, and the warp
 * waits again from LAST_STEP */
#define STEP(task) ((task)->unk_3c.l)
#define ARRIVAL_STEP 75
#define LAST_STEP 116

static void warp_destroy(struct task *task);
static void warp_step(struct task *task);
static void warp_draw(struct task *task);

/* the set editor's labels for the yaw in ang.z and the destination in scl,
 * which no code reads */
static const char out_angle_label[24] = "<- WARP OUT ANG Y(   )";
static const char out_x_label[20] = "<- WARP OUT POS X";
static const char out_y_label[20] = "<- WARP OUT POS Y";
static const char out_z_label[] = "<- WARP OUT POS Z";

/* clang-format off */
static struct collision_shape warp_shape = {
    0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
    SHAPE_UNK_2000, {0.0F, -10.0F, 0.0F}, 15.0F, 10.0F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */
static Sint32 dat_8c5e8900 = 50;
static Float player_range = 120.0F;

void quarters_warp_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = warp_draw;
        task->step = warp_step;
        task->destructor = warp_destroy;
        task->work->timer = task->work->ang.z;
        task->work->ang.z = 0;
        collision_init(task, &warp_shape, 1, COLLISION_OBJECT);
        STEP(task) = 0;
    }
}

static void warp_destroy(struct task *task)
{
}

/* @hack redundant: task and work register variables, where plain ones would
 * do */
static void warp_step(register struct task *task)
{
    register struct task_work *work = task->work;
    NJS_POINT3 dest;
    struct task *player;

    if (STEP(task) == 0 && placement_try_leave(task)) {
        return;
    }
    if (STEP(task) >= 1) {
        STEP(task)++;
        if (STEP(task) <= ARRIVAL_STEP) {
            fade_out_step();
        } else {
            fade_in_step();
        }
        if (STEP(task) == ARRIVAL_STEP) {
            /* @hack spelling: task->work read again for scl and timer, where
             * work would do */
            player_set_pos(0, task->work->scl.x, task->work->scl.y,
                           task->work->scl.z);
            /* @hack spelling: dest copied a coordinate at a time, where one
             * assignment of scl would do */
            dest.x = task->work->scl.x;
            dest.y = task->work->scl.y;
            dest.z = task->work->scl.z;
            camera_put_at(0, &dest);
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
            player_set_angles(0, 0, task->work->timer, 0);
            light_select(light_camera, 0);
            light_select(light_camera, 1);
            fade_set_black();
        }
        if (STEP(task) == LAST_STEP) {
            game_state.idle_dim = TRUE;
            game_state.pause_disabled = FALSE;
            STEP(task) = 0;
        }
    }
    /* @hack spelling: player, a local for the call's result, where the call
     * tested in place would do */
    if (STEP(task) == 0 && (work->collision->flags & COLLISION_TOUCHED) &&
        (player = collision_next_player(task)) != NULL) {
        game_state.idle_dim = FALSE;
        game_state.pause_disabled = TRUE;
        fade_reset_idle();
        STEP(task) = 1;
    }
    collision_register(task);
    /* @hack redundant: a test of player 0's distance, with an empty body */
    if (njDistanceP2P(&player_work[0]->pos, &work->pos) < player_range) {
    }
}

static void warp_draw(struct task *task)
{
}
