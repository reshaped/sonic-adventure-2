/* the music change, ADXCHG in City Escape's object list. it changes the
 * stage's music once a player enters its ball */
#include <ninja.h>

#include "object/set.h"
#include "player/player.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "stg13/music_change.h"
#include "system/task.h"

/* the change's modes, in its work's mode */
enum music_change_mode {
    MODE_START, /* as the task starts */
    MODE_CHANGED, /* a player entered the ball, and c_escap2.adx plays */
    MODE_RETRY, /* the stage plays again from its start */
    MODE_MISS, /* a player missed, and plays again from a restart point */
    /* play goes on before the change. each step names c_escap3.adx as the
     * music */
    MODE_PLAY
};

#define RADIUS(work) ((work)->scl.x)

static void music_change_destroy(struct task *task);
static void music_change_step(struct task *task);
static void music_change_draw(struct task *task);

void music_change_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = music_change_draw;
    task->step = music_change_step;
    task->destructor = music_change_destroy;
}

static void music_change_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (stage_get_mode() == STAGE_FADE_IN) {
        work->mode = MODE_RETRY;
    }
    switch (work->mode) {
    case MODE_RETRY:
        music_set_name("c_escap1.adx");
        break;
    case MODE_CHANGED:
    case MODE_PLAY:
    case MODE_MISS:
        if (restart_point_refresh(0)) {
            music_set_name("c_escap1.adx");
        } else {
            music_set_name("c_escap3.adx");
        }
        break;
    /* @hack redundant: the case for MODE_START, which does nothing, where the
     * switch's end would do */
    case MODE_START:
        break;
    }
}

static void music_change_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- COL SCL";
    }
    if (placement_try_leave(task)) {
        return;
    }
    if (player_near(&work->pos, RADIUS(work))) {
        /* @hack spelling: the mode tested with !, where == MODE_START would
         * do */
        if (!work->mode || work->mode == MODE_RETRY ||
            work->mode == MODE_PLAY) {
            work->mode = MODE_CHANGED;
            music_stop();
            music_play_name("c_escap2.adx");
        }
    }
    switch (stage_get_mode()) {
    case STAGE_PLAY:
        if (work->mode != MODE_CHANGED) {
            music_set_name("c_escap3.adx");
            work->mode = MODE_PLAY;
        }
        break;
    case STAGE_RETRY_FADE:
        work->mode = MODE_RETRY;
        break;
    case STAGE_MISS_FADE:
        if (work->mode != MODE_CHANGED) {
            work->mode = MODE_MISS;
        }
        break;
    }
}

static void music_change_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scale;

    /* @hack redundant: a return that always runs, and a body after it that no
     * path reaches. the image holds the body's frame but none of its code, so
     * the body is a stand-in that makes the same frame */
    if (1) {
        return;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    scale.x = scale.y = scale.z = RADIUS(work);
    njScaleEx(&scale);
    njPopMatrixEx();
}
