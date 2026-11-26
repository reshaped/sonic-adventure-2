#include <ninja.h>

#include "chao/garden.h"
#include "chaostgentrance/fixture_art.h"
#include "chaostgentrance/entrance.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/task.h"

static struct task *gate;

enum gate_phase { GATE_SHUT, GATE_OPENING, GATE_OPEN };
/* the work holds the frame of the gate's motion, which runs to OPEN_FRAME.
 * once the gate is open, the area changes when work->timer passes
 * AREA_DELAY */
#define FRAME(work) ((work)->scl.x)
#define OPEN_FRAME 7.0F
#define OPEN_SPEED 0.1F
#define AREA_DELAY 240
#define OPEN_SOUND 3
#define OPEN_PRIORITY 1
#define OPEN_VOLUME 0
/* sound_play_at_for's duration, in sound updates */
#define OPEN_UPDATES 10

static void gate_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case GATE_SHUT:
        break;
    case GATE_OPENING:
        FRAME(work) += OPEN_SPEED;
        if (FRAME(work) > OPEN_FRAME) {
            FRAME(work) = OPEN_FRAME;
            work->mode = GATE_OPEN;
        }
        sound_play_at_for(OPEN_SOUND, task, OPEN_PRIORITY, OPEN_VOLUME,
                          OPEN_UPDATES, &work->pos);
        break;
    case GATE_OPEN:
        if (work->timer++ > AREA_DELAY) {
            chao_change_area(CHAO_AREA_RACE);
        }
        break;
    }
}

static void gate_draw(struct task *task)
{
    struct task_work *work = task->work;
    njSetTexture(&entrance_object_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njCnkEasyDrawMotion(&entrance_gate_model, &entrance_gate_motion,
                        FRAME(work));
    njPopMatrixEx();
}

static void gate_destroy(struct task *task)
{
    gate = NULL;
}

void entrance_gate_open(void)
{
    if (gate) {
        gate->work->mode = GATE_OPENING;
        jingle_play("chao_r_gate_open.adx");
    }
}

void entrance_gate_create(void)
{
    struct task *task =
        task_create(TASK_WORK, 2, gate_step, "ALO_EntranceGateExecutor");
    struct task_work *work;
    if (task) {
        gate = task;
        work = task->work;
        work->pos.x = 0;
        work->pos.y = 0;
        work->pos.z = HALL_END_Z;
        FRAME(work) = 0;
        task->draw = gate_draw;
        task->destructor = gate_destroy;
    }
}
