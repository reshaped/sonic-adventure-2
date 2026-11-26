/* SE_TUNNEL's init and callbacks. stg30/tunnel_sound.c compiles them for
 * Final Rush, and stg40/tunnel_sound.c for Final Chase. the includer defines
 * FALL_SOUND, an id of its stage's sound list */
#ifndef SHARED_TUNNEL_SOUND_TASK_H
#define SHARED_TUNNEL_SOUND_TASK_H

#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"

/* a step plays FALL_SOUND while a player within FALL_RANGE of pos falls, for
 * FALL_DURATION + 1 sound updates */
#define FALL_RANGE 15.0F
#define FALL_PRIORITY 1
#define FALL_VOLUME 0
#define FALL_DURATION 70

static Sint32 unused_tunnel_word = 0;

static void tunnel_sound_destroy(struct task *task);
static void tunnel_sound_step(struct task *task);
static void tunnel_sound_draw(struct task *task);

void tunnel_sound_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = tunnel_sound_draw;
        task->step = tunnel_sound_step;
        task->destructor = tunnel_sound_destroy;
        work->mode = 0;
    }
}

static void tunnel_sound_destroy(struct task *task)
{
}

static void tunnel_sound_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    /* @hack spelling: the return in an arm of its own, where a test of the
     * call's result around the rest would do */
    if (placement_try_leave(task)) {
        return;
    } else if (!game_state.time_stopped &&
               (player = player_near(&work->pos, FALL_RANGE)) &&
               player_motion[player - 1]->spd.y < 0.0F) {
        /* every tunnel sound keys its sound by tunnel_sound_step, so the
         * tasks share one slot, which each call renews */
        sound_play_for(FALL_SOUND, (void *)tunnel_sound_step, FALL_PRIORITY,
                       FALL_VOLUME, FALL_DURATION);
    }
}

static void tunnel_sound_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: points nothing writes or reads */
    NJS_POINT3 first;
    NJS_POINT3 second;

    /* @hack redundant: calls no path makes */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njScale(NULL, 1.0F, 1.0F, 1.0F);
        njCalcPoint(NULL, &first, &second);
        njPopMatrixEx();
    }
}

#endif
