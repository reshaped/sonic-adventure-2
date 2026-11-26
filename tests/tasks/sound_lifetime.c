/* Task-keyed sounds borrow the task and sometimes its work until destruction.
 */
#include <assert.h>
#include <stdlib.h>
#include "sound/sound.c"
#include "collision/collision.h"

struct game_state game_state;
struct camera_place *camera_views[CAMERAS];

static struct task tasks[2];
static int occupied[2];
static int notes_started, notes_stopped;

void *syMalloc(Uint32 bytes)
{
    int i;

    assert(bytes == sizeof(struct task));
    for (i = 0; i < 2; i++) {
        if (!occupied[i]) {
            occupied[i] = 1;
            return &tasks[i];
        }
    }
    abort();
}

void *syCalloc(Uint32 count, Uint32 size)
{
    return calloc(count, size);
}

void syFree(void *block)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (block == &tasks[i]) {
            assert(occupied[i]);
            occupied[i] = 0;
            return;
        }
    }
    free(block);
}

void collision_free(struct task *task)
{
    abort();
}

static void step(struct task *task)
{
}

int main(void)
{
    static struct sound_work audio;
    static const struct sound notes[] = {{0, 1, 0, SOUND_NOTE, 0, 0},
                                         {0, 2, 0, SOUND_NOTE, 0, 0},
                                         {0, 3, 0, SOUND_NOTE, 0, 0},
                                         {0, 4, 0, SOUND_NOTE, 0, 0}};
    static const struct sound_list list = {4, notes};
    struct task *parent, *child, *next;
    int held, timed, repeat, effect, fresh, tick;

    parent = task_create(TASK_WORK, 2, step, "owner");
    assert(parent != NULL);
    child = task_create_child(TASK_WORK, step, parent);
    assert(child != NULL);
    sound_work = &audio;
    slots_reset();
    audio.character_sound = NO_ID;
    sound_set_list(&list, SOUND_LIST_LOADED);
    held = sound_play_task_held_at(0, parent, 1, 0);
    timed = sound_play_task_for(1, child, 1, 0, 15);
    repeat = sound_play_task_repeating_at(2, parent, 1, 0, 4);
    effect = sound_play_for(3, parent, 1, 0, 6);
    assert(held >= 0 && timed >= 0 && repeat >= 0 && effect >= 0);
    assert(audio.slots[held].follow == &parent->work->pos);
    assert(audio.slots[repeat].follow == &parent->work->pos);
    sound_update();
    parent->work->pos.x = 37;
    sound_update();
    assert(audio.slots[held].pos.x == 37 && audio.slots[repeat].pos.x == 37);

    assert(notes_started == 4 && notes_stopped == 0);
    sound_pause_all();
    assert(game_state.sound_paused);
    task_destroy(parent);
    assert(!occupied[0] && !occupied[1]);
    assert(!(audio.slots[held].mode & MODE_TASK) &&
           audio.slots[held].follow == NULL);
    assert(!(audio.slots[timed].mode & MODE_TASK));
    assert(!(audio.slots[repeat].mode & MODE_TASK) &&
           audio.slots[repeat].follow == NULL);
    assert(audio.slots[held].time == 0 && audio.slots[timed].time == 0);
    assert(audio.slots[repeat].time == 0 && audio.slots[effect].time > 0);
    sound_update();
    next = task_create(TASK_WORK, 2, step, "successor");
    assert(next == &tasks[0]);
    assert(slot_find(0, next) == -1 && slot_find(3, next) == -1);

    assert(notes_started == 4 && notes_stopped == 0);
    sound_resume_all();
    assert(!game_state.sound_paused);
    fresh = sound_play_task_held_at(0, next, 1, 0);
    assert(fresh >= 0 && fresh != held);
    sound_update();
    assert(audio.slots[held].id == NO_ID && audio.slots[timed].id == NO_ID);
    assert(audio.slots[effect].id == 3 && audio.slots[effect].time > 0);
    next->work->pos.x = 61;
    sound_update();
    assert(audio.slots[fresh].pos.x == 61 && audio.slots[repeat].pos.x == 37);
    for (tick = 0; tick < 8; tick++) {
        sound_update();
    }
    assert(audio.slots[effect].id == NO_ID && audio.slots[repeat].id == NO_ID);
    assert(audio.slots[fresh].id == 0);
    task_destroy(next);
    sound_update();
    assert(audio.slots[fresh].id == NO_ID && !occupied[0]);
    /* The repeat queued one last note before its slot retired. */
    assert(notes_started == 6 && notes_stopped == 5);
    return 0;
}

/* The note ports accept MIDI messages. This fixture installs no camera,
 * sequence or hardware driver. */
SDE_ERR sdMidiSetMes(SDMIDI port, SDS_MIDI_MES *message, Uint8 status, ...)
{
    message->m_Member[3] = status;
    return SDE_ERR_NOTHING;
}

SDE_ERR sdMidiSendMes(SDMIDI port, const SDS_MIDI_MES *message, Sint8 priority)
{
    switch (message->m_Member[3] & 0xf0) {
    case SDD_MIDI_MES_NOTE_ON:
        notes_started++;
        break;
    case SDD_MIDI_MES_NOTE_OFF:
        notes_stopped++;
        break;
    }
    return SDE_ERR_NOTHING;
}

void sound_pause(void)
{
}
void sound_resume(void)
{
}
SDE_ERR sdMidiPause(SDMIDI port)
{
    return SDE_ERR_NOTHING;
}
SDE_ERR sdMidiContinue(SDMIDI port)
{
    abort();
}
SDE_ERR sdMidiSetFxLev(SDMIDI port, Sint8 level)
{
    abort();
}
SDE_ERR sdMidiGetStat(SDMIDI port, SDS_MIDI_STAT *status)
{
    abort();
}
SDE_ERR sdMidiPlay(SDMIDI port, Sint8 bank, Sint8 number, Sint8 priority)
{
    abort();
}
SDE_ERR sdMidiStop(SDMIDI port)
{
    abort();
}
SDE_ERR sdMidiSetVol(SDMIDI port, Sint8 volume, Sint32 fade)
{
    abort();
}
SDE_ERR sdMidiSetPan(SDMIDI port, Sint8 pan, Sint32 fade)
{
    abort();
}
SDE_ERR sdMidiSetPitch(SDMIDI port, Sint16 pitch, Sint32 fade)
{
    abort();
}
int stage_current_id(void)
{
    abort();
}
Float njInvertSqrt(Float value)
{
    abort();
}
Float njScalor(NJS_VECTOR *v)
{
    abort();
}
Float njSin(Angle value)
{
    abort();
}
Float njSqrt(Float value)
{
    abort();
}
Float njUnitVector(NJS_VECTOR *v)
{
    abort();
}
Angle vector_angle(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *axis)
{
    abort();
}
