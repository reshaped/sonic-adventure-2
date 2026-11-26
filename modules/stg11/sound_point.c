/* the sound point, SE in Iron Gate's object list. it is stg09/sound_point.c
 * with one sound for both settings of ang.x. the stage's sound list lies in
 * the same source */
#include <ninja.h>

#include "object/set.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "stg11/sound_point.h"
#include "system/task.h"

/* in ang.x: the bit that picks between the step's two calls, which are the
 * same */
#define PICK_BIT 0x1
/* the id in iron_gate_sounds of the point's sound. SOUND_UPDATES is
 * sound_play_at_for's duration, in sound updates */
#define SOUND_POINT 14
#define SOUND_PRIORITY 1
#define SOUND_UPDATES 10

static const struct sound iron_gate_sound_table[15] = {
#include "stg11/sound_point/iron_gate_sound_table.inc"
};
const struct sound_list iron_gate_sounds = {
#include "stg11/sound_point/iron_gate_sounds.inc"
};

/* the volume of the point's sound, before its distance lowers it. no code
 * writes it */
static int sound_point_volume = 0;

static void sound_point_destroy(struct task *task);
static void sound_point_step(struct task *task);
static void sound_point_draw(struct task *task);

void iron_gate_sound_point_init(struct task *task)
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
    /* @hack redundant: a test of PICK_BIT whose two arms make the same call,
     * where the call alone would do */
    if (work->ang.x & PICK_BIT) {
        sound_play_at_for(SOUND_POINT, work, SOUND_PRIORITY, sound_point_volume,
                          SOUND_UPDATES, &work->pos);
    } else {
        sound_play_at_for(SOUND_POINT, work, SOUND_PRIORITY, sound_point_volume,
                          SOUND_UPDATES, &work->pos);
    }
}

static void sound_point_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_POINT3 screen;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants, and for the frame its
     * calls and locals give the function */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njScale(NULL, 1.0F, 1.0F, 1.0F);
        njCalcPoint(NULL, &pos, &screen);
        njPopMatrixEx();
        (void)"<- FAN";
    }
}
