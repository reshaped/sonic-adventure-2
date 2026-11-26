/* the gust, SE_TOPPU in Radical Highway's object list: a sound that plays at
 * the work's pos. it is shared/stage_sounds.c's gust with another sound and
 * key, and no step plays it while time stops. this source defines the stage's
 * sound list too */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "stg14/gust_sound.h"
#include "system/game_state.h"
#include "system/task.h"

#define GUST_SOUND 8
#define PRIORITY 1
/* each step plays the gust for GUST_DURATION + 1 sound updates */
#define GUST_DURATION 120

static const struct sound radical_highway_sounds[9] = {
#include "stg14/gust_sound/radical_highway_sounds.inc"
};
const struct sound_list radical_highway_sound_list = {
#include "stg14/gust_sound/radical_highway_sound_list.inc"
};

static Float dat_8c5d97dc = 0.01F;
static Sint32 gust_volume = 0;

static void gust_sound_destroy(struct task *task);
static void gust_sound_step(struct task *task);
static void gust_sound_draw(struct task *task);

void highway_gust_sound_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->draw = gust_sound_draw;
        task->step = gust_sound_step;
        task->destructor = gust_sound_destroy;
        work->mode = 0;
    }
}

static void gust_sound_destroy(struct task *task)
{
}

static void gust_sound_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the return in an arm of its own, where a test of the
     * call's result around the rest would do */
    if (placement_try_leave(task)) {
        return;
    } else if (!game_state.time_stopped) {
        sound_play_at_for(GUST_SOUND, (void *)highway_gust_sound_init, PRIORITY,
                          gust_volume, GUST_DURATION, &work->pos);
    }
}

static void gust_sound_draw(struct task *task)
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
