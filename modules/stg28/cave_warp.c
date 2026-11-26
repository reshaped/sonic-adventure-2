/* the warp, WARP in Pyramid Cave's object list: a box whose touch moves
 * player 0 behind a fade */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/fade.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/cave_warp.h"
#include "system/game_state.h"
#include "system/task.h"

/* the angle about y that player 0 comes out at. the init takes it from ang.z */
#define OUT_ANGLE(work) ((work)->timer)

/* task's unk_3c counts a warp's steps, from 1 on the step the warp starts. it
 * holds 0 while no warp runs. the screen fades out through step WARP_MOVE,
 * when player 0 moves to scl, and fades in after it. the warp ends at step
 * WARP_END */
#define STEPS(task) ((task)->unk_3c.l)
#define WARP_MOVE 75
#define WARP_END 116

#define WARP_SHAPES 1

/* the warp's sound, an id in pyramid_cave_sound_list. a play of it lasts
 * WARP_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define WARP_SOUND 11
#define WARP_SOUND_PRIORITY 1
#define WARP_SOUND_STEPS 15

static void cave_warp_destroy(struct task *task);
static void cave_warp_step(struct task *task);
static void cave_warp_draw(struct task *task);

/* clang-format off */
static struct collision_shape cave_warp_shapes[WARP_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_UNK_2000, {0.0F, -10.0F, 0.0F}, 25.0F, 10.0F, 25.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the sound's volume, and how near the box player 0 must lie for the step to
 * play it */
static Sint32 cave_warp_volume = 40;
static Float cave_warp_sound_range = 120.0F;

void cave_warp_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y (   )";
        (void)"<- WARP OUT ANG Y(   )";
        (void)"<- WARP OUT POS X";
        (void)"<- WARP OUT POS Y";
        (void)"<- WARP OUT POS Z";
    }
    if (!placement_try_leave(task)) {
        task->draw = cave_warp_draw;
        task->step = cave_warp_step;
        task->destructor = cave_warp_destroy;
        OUT_ANGLE(task->work) = task->work->ang.z;
        task->work->ang.z = 0;
        collision_init(task, cave_warp_shapes, WARP_SHAPES, COLLISION_OBJECT);
        STEPS(task) = 0;
    }
}

static void cave_warp_destroy(struct task *task)
{
}

static void cave_warp_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack redundant: hit, a local for the player collision_next_player
     * finds, which nothing reads */
    struct task *hit;

    if (STEPS(task) == 0 && placement_try_leave(task)) {
        return;
    }
    if (STEPS(task) >= 1) {
        STEPS(task)++;
        if (STEPS(task) <= WARP_MOVE) {
            fade_out_step();
        } else {
            fade_in_step();
        }
        if (STEPS(task) == WARP_MOVE) {
            /* @hack spelling: task->work read again for scl, where work would
             * do */
            player_set_pos(0, task->work->scl.x, task->work->scl.y,
                           task->work->scl.z);
            /* @hack redundant: pos set to the work's scl, which nothing
             * reads */
            pos.x = task->work->scl.x;
            pos.y = task->work->scl.y;
            pos.z = task->work->scl.z;
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
            /* @hack spelling: task->work read again for OUT_ANGLE, where work
             * would do */
            player_set_angles(0, 0, OUT_ANGLE(task->work), 0);
            light_select(light_camera, 0);
            light_select(light_camera, 1);
            fade_set_black();
        }
        if (STEPS(task) == WARP_END) {
            game_state.idle_dim = 1;
            game_state.pause_disabled = 0;
            STEPS(task) = 0;
        }
    }
    if (STEPS(task) == 0 && (work->collision->flags & COLLISION_TOUCHED) &&
        (hit = collision_next_player(task)) != NULL) {
        game_state.idle_dim = 0;
        game_state.pause_disabled = 1;
        fade_reset_idle();
        STEPS(task) = 1;
    }
    collision_register(task);
    if (cave_warp_sound_range >
        njDistanceP2P(&player_work[0]->pos, &work->pos)) {
        sound_play_at_for(WARP_SOUND, task, WARP_SOUND_PRIORITY,
                          cave_warp_volume, WARP_SOUND_STEPS, &work->pos);
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        Float scale = work->scl.x;

        fade_set_black();
        if (scale) {
        }
    }
}

static void cave_warp_draw(struct task *task)
{
}
