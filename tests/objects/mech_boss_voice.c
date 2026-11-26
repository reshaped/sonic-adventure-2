/* the Flying Dog's script, the last in voice_scripts, played to its end. the
 * last line plays out while the boss's power holds at the line's own, and the
 * script reads no line past its last. the test's table gives the lines
 * voices 1 to 3 and powers AT_ONCE, 1 and 0 */
#include <assert.h>
#include <stddef.h>

#include <cri_adxt.h>
#include <shinobi.h>

#include "shared/mech_boss.h"
#include "shared/mech_boss_voice.h"
#include "sound/adx.h"
#include "system/game_state.h"
#include "system/task.h"

struct game_state game_state;

static struct task voice_task;
static struct task_work voice_work;
static int voices[4];
static int played, playing, killed;

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    assert(flags == TASK_WORK && voice_task.step == NULL);
    voice_task.step = step;
    voice_task.work = &voice_work;
    return &voice_task;
}

void task_kill(struct task *task)
{
    assert(task == &voice_task);
    killed++;
}

int voice_play(int id, int priority)
{
    assert(played < 4);
    voices[played++] = id;
    return played;
}

int voice_status(int index)
{
    assert(index == played);
    return playing ? ADXT_STAT_PLAYING : ADXT_STAT_PLAYEND;
}

static void step(int count)
{
    while (count-- > 0) {
        voice_task.step(&voice_task);
    }
}

int main(void)
{
    Float power = 5.0F;

    assert(mech_boss_voice_create(NULL, MECH_BOSS_FLYING_DOG, &power) ==
           &voice_task);
    playing = 1;
    step(5);
    assert(played == 0);
    step(1);
    assert(played == 1 && voices[0] == 1);
    power = 1.0F;
    step(2);
    assert(played == 2 && voices[1] == 2);
    power = 0.0F;
    step(2);
    assert(played == 3 && voices[2] == 3);
    step(100);
    assert(played == 3);
    playing = 0;
    step(1 + 16 + 90);
    assert(played == 3 && killed == 0);
    step(1);
    assert(killed == 1);
    return 0;
}
