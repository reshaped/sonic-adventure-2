/* the sound point, SE in Prison Lane's object list. it plays a gas leak's
 * sound or a fan's. the stage's sound list lies in the same source */
#include <ninja.h>

#include "object/set.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "stg09/sound_point.h"
#include "system/task.h"

/* in ang.x: the point plays the gas leak's sound, not the fan's */
#define GAS_BIT 0x1
/* the ids in prison_lane_sounds of the gas leak's sound and the fan's.
 * SOUND_UPDATES is sound_play_at_for's duration, in sound updates */
#define SOUND_GAS 9
#define SOUND_FAN 13
#define SOUND_PRIORITY 1
#define SOUND_UPDATES 10

static const struct sound prison_lane_sound_table[14] = {
#include "stg09/sound_point/prison_lane_sound_table.inc"
};
const struct sound_list prison_lane_sounds = {
#include "stg09/sound_point/prison_lane_sounds.inc"
};

/* the volume of the point's sound, before its distance lowers it. no code
 * writes it */
static int sound_point_volume = 0;

static void sound_point_destroy(struct task *task);
static void sound_point_step(struct task *task);
static void sound_point_draw(struct task *task);

void sound_point_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = sound_point_draw;
    task->step = sound_point_step;
    task->destructor = sound_point_destroy;
}

static void sound_point_destroy(struct task *task)
{
}

static void sound_point_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->ang.x & GAS_BIT) {
        sound_play_at_for(SOUND_GAS, work, SOUND_PRIORITY, sound_point_volume,
                          SOUND_UPDATES, &work->pos);
    } else {
        sound_play_at_for(SOUND_FAN, work, SOUND_PRIORITY, sound_point_volume,
                          SOUND_UPDATES, &work->pos);
    }
}

static void sound_point_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_POINT3 screen;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for the frame its
     * calls and locals give the function */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njScale(NULL, 1.0F, 1.0F, 1.0F);
        njCalcPoint(NULL, &pos, &screen);
        njPopMatrixEx();
        (void)"<- GAS";
        (void)"<- FAN";
    }
}
