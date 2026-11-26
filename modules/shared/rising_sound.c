/* the rising sound: the mech bosses' sound 5, held while its pitch climbs to
 * the sound's own. Big Foot and Hot Shot play it */
#include <ninja.h>

#include "shared/rising_sound.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/task.h"

#define SOUND 5
#define PRIORITY 1
/* each step plays the sound by sound_play_for, for DURATION + 1 sound
 * updates */
#define DURATION 5
#define LIFE 240

/* the pitches are sound_set_pitch's, with 0 the sound's own. the volumes are
 * Floats that sound_set_volume's Sint8 cuts to an integer */
#define START_PITCH (-1536)
#define RISE 32
#define VOLUME_PER_PITCH 0.0025F
#define FADE_PITCH 256
#define FADE_VOLUME 30.0F
#define FADE_STEP 0.4F
#define FADE_END (-120.0F)

/* the sound's modes, in its work's mode */
enum rising_sound_mode { MODE_RISE, MODE_FADE };

#define PITCH(work) ((work)->ang.x)
#define VOLUME(work) ((work)->scl.x)

static void rising_sound_step(struct task *task);
static void rising_sound_destroy(struct task *task);

/* the sound's task while it runs, or null */
static struct task *rising_sound_task = NULL;

static void rising_sound_step(struct task *task)
{
    struct task_work *work = task->work;

    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    switch (work->mode) {
    case MODE_RISE:
        if (PITCH(work) < 0) {
            PITCH(work) += RISE;
        }
        VOLUME(work) = PITCH(work) * VOLUME_PER_PITCH;
        break;
    case MODE_FADE:
        VOLUME(work) -= FADE_STEP;
        /* @bug logic: tests submode, which rising_sound_fade sets to 0, so
         * the test never holds and the timer ends the task (FCMP/GT at 0x60
         * of the object's code) */
        if (work->submode <= FADE_END) {
            task_kill(task);
            return;
        }
        break;
    }
    sound_play_for(SOUND, task, PRIORITY, 0, DURATION);
    sound_set_volume(task, SOUND_ANY_ID, VOLUME(work));
    sound_set_pitch(task, SOUND_ANY_ID, PITCH(work));
    /* @hack spelling: the early return before the last task_kill, where a
     * test around the call would do */
    if (--work->timer != 0) {
        return;
    }
    task_kill(task);
}

static void rising_sound_destroy(struct task *task)
{
    rising_sound_task = NULL;
}

struct task *rising_sound_start(struct task *parent)
{
    struct task *task;
    struct task_work *work;

    if (rising_sound_task == NULL) {
        if (parent == NULL) {
            return NULL;
        }
        task = task_create_child(TASK_WORK, rising_sound_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = task->work;
        work->pos = parent->work->pos;
        PITCH(work) = START_PITCH;
        work->timer = LIFE;
        task->destructor = rising_sound_destroy;
        rising_sound_task = task;
        return task;
    }
#ifdef NONMATCHING
    /* where a sound runs already, the image's function sets no result,
     * which C can't express. under NONMATCHING it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void rising_sound_fade(void)
{
    struct task *task = rising_sound_task;
    struct task_work *work;

    if (task == NULL) {
        return;
    }
    work = task->work;
    PITCH(work) = FADE_PITCH;
    VOLUME(work) = FADE_VOLUME;
    work->mode = MODE_FADE;
    work->submode = 0;
}

void rising_sound_stop(void)
{
    struct task *task = rising_sound_task;

    if (task != NULL) {
        task_kill(task);
    }
}
