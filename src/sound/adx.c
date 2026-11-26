/* the ADX streams: one for the music, and VOICES for voices. the music can
 * fade, and can give way to a jingle */
#include <cri_adxf.h>
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "sound/adx.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/sequence.h"
#include "system/task.h"

/* the music's channels and a voice's, and the bytes of work ADXT_Create
 * takes for each */
#define MUSIC_CHANNELS 2
#define VOICE_CHANNELS 1
#define MUSIC_WORK_SIZE 0x391c4
#define VOICE_WORK_SIZE 0x8482

/* the ADXF partition the voices play from */
#define VOICE_PARTITION 0
/* the task levels of the sound task and a jingle's */
#define SOUND_LEVEL 0
#define JINGLE_LEVEL 2
/* the loads of its table to try. after each that fails, an empty loop turns
 * PARTITION_WAIT times */
#define PARTITION_TRIES 10
#define PARTITION_WAIT 0x100000

/* the calls a second each stream's server is told to expect */
#define SERVER_FREQUENCY 30

/* the volume a fade in starts from and a fade out ends at. the sound output
 * takes none quieter, and clamps a lower one to it */
#define MUSIC_SILENT (-999)
/* the volume a fade adds or takes away on each step */
#define MUSIC_FADE_STEP 8

/* One GDFS filename, including its terminator, waiting for a mini event. */
static char music_deferred[GDD_FS_FNAMESIZE + 1];

/* the volume each step gives every unpaused voice. nothing writes it, so the
 * voices play at full volume */
static int voice_volume = VOLUME_FULL;
/* the sound task, or null for none */
static struct task *sound_task = NULL;
struct sound_streams *sound_streams = NULL;
/* the stream voice_play took last, which its next search begins after */
static int voice_last = 0;

static void sound_task_setup(struct task *task);
static void sound_task_destroy(struct task *task);
static void music_start_pending(struct sound_streams *streams);
static void sound_task_step(struct task *task);
static void music_set_next(const char *name);
static void voices_pause(void);
static void voices_resume(void);
static void jingle_destroy(struct task *task);
static void jingle_step(struct task *task);
static void music_mark_heard(const char *name);
/* no other object calls these, and nothing calls music_time, music_replay
 * or voice_time */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
int music_status(void);
int music_time(void);
Bool music_replay(void);
int voice_time(int index);
/* NOLINTEND(misc-use-internal-linkage) */

static void adx_ignore_error(void *obj, char *msg)
{
}

Bool sound_init(void)
{
    ADXT_Init();
    sound_task = NULL;
    sound_streams = NULL;
    return TRUE;
}

void sound_exit(void)
{
    if (sound_task != NULL) {
        task_destroy(sound_task);
    }
    ADXT_Finish();
}

Bool sound_task_create(const char *name)
{
    struct task *task;
    struct sound_streams *streams;
    int i;

    task = task_create(0, SOUND_LEVEL, sound_task_step, "ADX_TaskLoop");
    if (task == NULL) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((streams = syCalloc(1, sizeof(*streams))) == NULL) {
        task_destroy(task);
        return FALSE;
    }
    task->destructor = sound_task_destroy;
    sound_streams = streams;
    task->extra.p = streams;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((streams->work = syMalloc(MUSIC_WORK_SIZE)) == NULL) {
        task_destroy(task);
        return FALSE;
    }
    for (i = 0; i < VOICES; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((streams->voices[i].work = syMalloc(VOICE_WORK_SIZE)) == NULL) {
            task_destroy(task);
            return FALSE;
        }
    }
    sound_task_setup(task);
    if (name != NULL) {
        music_set_name(name);
        music_play();
    }
    return TRUE;
}

/* make task the sound task and start its streams, or destroy it where a
 * sound task runs already. a table that fails to load PARTITION_TRIES times
 * is given up */
static void sound_task_setup(struct task *task)
{
    struct sound_streams *streams = task->extra.p;
    int tries;
    int result;
    int wait;
    int i;

    if (sound_task == NULL) {
        sound_task = task;
        ADXT_EntryErrFunc(adx_ignore_error, NULL);
        for (tries = PARTITION_TRIES; tries > 0; tries--) {
            if (game_state.voice_language == VOICE_JAPANESE) {
                result = ADXF_LoadPartition(VOICE_PARTITION, "event_adx.afs",
                                            streams->partition, VOICE_FILES);
            } else {
                result = ADXF_LoadPartition(VOICE_PARTITION, "event_adx_e.afs",
                                            streams->partition, VOICE_FILES);
            }
            if (result == ADXF_ERR_OK) {
                break;
            }
            for (wait = 0; wait < PARTITION_WAIT; wait++) {
            }
        }
        for (i = 0; i < VOICES; i++) {
            streams->voices[i].adxt = ADXT_Create(
                VOICE_CHANNELS, streams->voices[i].work, VOICE_WORK_SIZE);
        }
        streams->adxt =
            ADXT_Create(MUSIC_CHANNELS, streams->work, MUSIC_WORK_SIZE);
        if (streams->adxt != NULL) {
            streams->volume = VOLUME_FULL;
            ADXT_SetAutoRcvr(streams->adxt, ADXT_RMODE_REPLAY);
            ADXT_SetOutVol(streams->adxt, streams->volume);
            ADXT_SetSvrFreq(streams->adxt, SERVER_FREQUENCY);
        }
        for (i = 0; i < VOICES; i++) {
            if (streams->voices[i].adxt != NULL) {
                streams->voices[i].volume = voice_volume;
                ADXT_SetAutoRcvr(streams->voices[i].adxt, ADXT_RMODE_STOP);
                ADXT_SetOutVol(streams->voices[i].adxt,
                               streams->voices[i].volume);
                ADXT_SetSvrFreq(streams->voices[i].adxt, SERVER_FREQUENCY);
            }
        }
        streams->state = STREAM_WAITING;
        for (i = 0; i < VOICES; i++) {
            streams->voices[i].state = STREAM_PLAYING;
            streams->voices[i].id = VOICE_ID_NONE;
        }
    } else {
        task_destroy(task);
    }
}

static void sound_task_destroy(struct task *task)
{
    struct sound_streams *streams = task->extra.p;
    int i;

    if (streams != NULL) {
        if (streams->name != NULL) {
            syFree(streams->name);
            streams->name = NULL;
        }
        if (streams->next != NULL) {
            syFree(streams->next);
            streams->next = NULL;
        }
        if (streams->played != NULL) {
            syFree(streams->played);
            streams->played = NULL;
        }
        if (streams->work != NULL) {
            ADXT_Destroy(streams->adxt);
            streams->adxt = NULL;
            syFree(streams->work);
            streams->work = NULL;
        }
        for (i = 0; i < VOICES; i++) {
            if (streams->voices[i].work != NULL) {
                ADXT_Destroy(streams->voices[i].adxt);
                streams->voices[i].adxt = NULL;
                syFree(streams->voices[i].work);
                streams->voices[i].work = NULL;
            }
        }
    }
    /* @bug logic: destroying a second sound task loses the first. the
     * destructor clears sound_task and sound_streams whichever task it
     * destroys, and sound_task_setup destroys every second one. the first
     * then plays on out of reach of every call here. MOV.L R13,@R2 at
     * 0x8c011478 and MOV.L R13,@R3 at 0x8c01147a clear the two */
    sound_task = NULL;
    sound_streams = NULL;
}

/* count the wait down. once the timer is 0, start next, or the music where next
 * is null. with neither, the wait goes on. starting next moves it to played,
 * and starting the music frees played */
static void music_start_pending(struct sound_streams *streams)
{
    if (streams->next != NULL) {
        if (streams->timer <= 0) {
            streams->state = STREAM_PLAYING;
            streams->timer = 0;
            ADXT_StartFname(streams->adxt, streams->next);
            if (streams->played != NULL) {
                syFree(streams->played);
            }
            streams->played = streams->next;
            streams->next = NULL;
        } else {
            streams->timer--;
        }
    } else if (streams->name != NULL) {
        if (streams->timer <= 0) {
            streams->state = STREAM_PLAYING;
            streams->timer = 0;
            ADXT_StartFname(streams->adxt, streams->name);
            if (streams->played != NULL) {
                syFree(streams->played);
                streams->played = NULL;
            }
        } else {
            streams->timer--;
        }
    }
}

static void sound_task_step(struct task *task)
{
    struct sound_streams *streams = task->extra.p;
    int level;
    int volume;
    int i;

    if (streams->adxt != NULL) {
        if (streams->fade != 0 && streams->state != STREAM_WAITING &&
            streams->state != STREAM_PAUSED) {
            streams->state = streams->fade;
            streams->fade = 0;
        }
        switch (streams->state) {
        case STREAM_WAITING:
            music_start_pending(streams);
            break;
        case STREAM_PLAYING:
            break;
        case STREAM_FADING_IN:
            /* @hack spelling: level, a local of its own, where volume would
             * do */
            level = ADXT_GetOutVol(streams->adxt);
            if (level + MUSIC_FADE_STEP < streams->volume) {
                ADXT_SetOutVol(streams->adxt, level + MUSIC_FADE_STEP);
            } else {
                ADXT_SetOutVol(streams->adxt, streams->volume);
                streams->state = STREAM_PLAYING;
            }
            break;
        case STREAM_FADING_OUT:
            volume = ADXT_GetOutVol(streams->adxt);
            /* @bug logic: the fade out ends one or two steps late. the test
             * adds MUSIC_FADE_STEP where the fade takes it away. so a fade
             * from 0 sets -1000 and -1008 after -992, and only then
             * MUSIC_SILENT, -999. the sound output clamps the two late
             * volumes to -999. ADD #8,R3 at 0x8c0115c0 adds the step, and
             * CMP/GT R2,R3 at 0x8c0115c2 tests */
            if (volume + MUSIC_FADE_STEP > MUSIC_SILENT) {
                ADXT_SetOutVol(streams->adxt, volume - MUSIC_FADE_STEP);
            } else {
                ADXT_SetOutVol(streams->adxt, MUSIC_SILENT);
                music_stop();
            }
            break;
        }
        streams->status = ADXT_GetStat(streams->adxt);
    }
    for (i = 0; i < VOICES; i++) {
        /* @hack spelling: two ifs, where one with && would do */
        if (streams->voices[i].adxt != NULL) {
            if (streams->voices[i].state == STREAM_PLAYING) {
                if (streams->voices[i].id >= 0) {
                    ADXT_StartAfs(streams->voices[i].adxt, VOICE_PARTITION,
                                  streams->voices[i].id);
                }
                streams->voices[i].id = VOICE_ID_NONE;
                streams->voices[i].status =
                    ADXT_GetStat(streams->voices[i].adxt);
                ADXT_SetOutVol(streams->voices[i].adxt, voice_volume);
            }
        }
    }
    /* @hack redundant: the test of adxpd_internal_error, where clearing it
     * alone would do */
    if (adxpd_internal_error) {
        adxpd_internal_error = FALSE;
    }
}

void sound_task_kill(void)
{
    if (sound_task != NULL) {
        task_destroy(sound_task);
    }
}

void sound_pause(void)
{
    music_pause();
    voices_pause();
}

void sound_resume(void)
{
    music_resume();
    voices_resume();
}

#ifdef NONMATCHING
/* Copy the filename before replacing its owner. The caller can pass a name
 * that already lives in the stream. */
static char *copy_music_name(const char *name)
{
    size_t bytes = strlen(name) + 1;
    char *copy = syMalloc(bytes);

    if (copy != NULL) {
        memcpy(copy, name, bytes);
    }
    return copy;
}
#endif

void music_set_name(const char *name)
{
    struct sound_streams *streams = sound_streams;
    char *copy;

    if (streams != NULL) {
#ifdef NONMATCHING
        /* this side repairs the tag below: copy_music_name allocates the copy's
         * null too */
        copy = name != NULL ? copy_music_name(name) : NULL;
        if (streams->name != NULL) {
            syFree(streams->name);
        }
        streams->name = copy;
#else
        /* @hack spelling: sound_streams read again for each store, not
         * through streams */
        if (streams->name != NULL) {
            syFree(streams->name);
            sound_streams->name = NULL;
        }
        if (name != NULL) {
            /* @bug memory: strcpy writes the copy's terminating null a byte
             * past its allocation, which takes strlen(name) bytes. MOV R0,R4
             * at 0x8c011706, in syMalloc's delay slot, passes strlen's
             * result */
            if ((copy = syMalloc(strlen(name))) != NULL) {
                strcpy(copy, name);
                sound_streams->name = copy;
            }
        }
#endif
    }
}

void music_play(void)
{
    int status;

    music_mark_heard(NULL);
    if (game_state.music_on) {
        /* @hack spelling: status, a local tested by a switch, where two
         * tests of sound_streams->status would do */
        status = sound_streams->status;
        switch (status) {
        case ADXT_STAT_STOP:
        case ADXT_STAT_PLAYEND:
            break;
        default:
            music_stop();
            break;
        }
        if (sound_streams->name != NULL) {
            sound_streams->state = STREAM_WAITING;
            sound_streams->timer = MUSIC_START_STEPS;
        }
    }
}

void music_play_name(const char *name)
{
    music_set_name(name);
    music_play();
}

void music_change(const char *name)
{
    if (strcmp(sound_streams->name, name) != 0) {
        music_play_name(name);
    }
}

/* keep a copy of name as next. where the heap runs out, next keeps what it
 * held */
static void music_set_next(const char *name)
{
    char *copy;
    struct sound_streams *streams;

#ifdef NONMATCHING
    /* this side repairs the tag below: copy_music_name allocates the copy's
     * null too */
    if ((copy = copy_music_name(name)) != NULL) {
#else
    /* @bug memory: strcpy overruns the copy by its null, as in
     * music_set_name. MOV R0,R4 at 0x8c0117a2 passes strlen's result */
    if ((copy = syMalloc(strlen(name))) != NULL) {
#endif
        streams = sound_streams;
        if (streams->next != NULL) {
            syFree(streams->next);
        }
#ifdef NONMATCHING
        /* this side repairs the tag above here too: copy_music_name made the
         * copy whole */
        streams->next = copy;
#else
        strcpy(copy, name);
        /* @hack spelling: sound_streams read again for the store, not
         * through streams */
        sound_streams->next = copy;
#endif
    }
}

void music_play_next(const char *name)
{
    int status;

    if (game_state.music_on) {
        /* @hack spelling: status, a local tested by a switch, where two
         * tests of sound_streams->status would do */
        status = sound_streams->status;
        switch (status) {
        case ADXT_STAT_STOP:
        case ADXT_STAT_PLAYEND:
            break;
        default:
            music_stop();
            break;
        }
        music_set_next(name);
        sound_streams->state = STREAM_WAITING;
        sound_streams->timer = MUSIC_START_STEPS;
    }
}

void music_stop(void)
{
    struct sound_streams *streams = sound_streams;

    if (streams->adxt != NULL) {
        ADXT_Stop(streams->adxt);
        /* @hack spelling: sound_streams read again for the store, not
         * through streams */
        sound_streams->state = STREAM_PLAYING;
    }
}

/* what ADXT_GetStat returned for the music on the last step. there must be
 * a sound task */
int music_status(void)
{
    return sound_streams->status;
}

/* the time the music has played, as ADXT_GetTime's count. the count's unit
 * is 1 / tscale seconds, and the call returns no tscale. returns 0 where the
 * music has no stream. there must be a sound task */
int music_time(void)
{
    struct sound_streams *streams = sound_streams;
    Sint32 count;
    Sint32 scale;

    if (streams->adxt != NULL) {
        ADXT_GetTime(streams->adxt, &count, &scale);
        return count;
    }
    return 0;
}

/* music_play_next of played, or else music_play. returns FALSE, and plays
 * nothing, where there is neither played nor music. returns TRUE otherwise.
 * there must be a sound task */
Bool music_replay(void)
{
    struct sound_streams *streams = sound_streams;

    if (streams->played != NULL) {
        music_play_next(streams->played);
        return TRUE;
    } else if (streams->name != NULL) {
        music_play();
        return TRUE;
    }
    return FALSE;
}

void music_pause(void)
{
    if (sound_streams != NULL && sound_streams->adxt != NULL) {
        /* @hack spelling: a switch of one case, where an if would do */
        switch (sound_streams->state) {
        case STREAM_WAITING:
            sound_streams->timer = 0;
            music_start_pending(sound_streams);
        }
        if (sound_streams->state == STREAM_PLAYING) {
            ADXT_Pause(sound_streams->adxt, TRUE);
            sound_streams->state = STREAM_PAUSED;
        }
    }
}

void music_resume(void)
{
    struct sound_streams *streams = sound_streams;

    if (streams != NULL && streams->adxt != NULL) {
        ADXT_Pause(streams->adxt, FALSE);
        /* @hack spelling: sound_streams read again for the store, not
         * through streams */
        sound_streams->state = STREAM_PLAYING;
    }
}

/* keep volume as the music's, where a fade in ends. while the music waits or
 * plays, the stream takes volume at once. otherwise the stream keeps the volume
 * it has, and music_resume doesn't give it the new one */
static void music_set_volume(int volume)
{
    if (sound_streams != NULL) {
        sound_streams->volume = volume;
        switch (sound_streams->state) {
        case STREAM_WAITING:
        case STREAM_PLAYING:
            if (sound_streams->adxt != NULL) {
                ADXT_SetOutVol(sound_streams->adxt, volume);
            }
            break;
        }
    }
}

void music_fade_in(int volume)
{
    struct sound_streams *streams;

    /* @hack redundant: a test that sets volume to 0 where it is 0 already */
    if (volume == 0) {
        volume = 0;
    }
    streams = sound_streams;
    if (streams != NULL) {
        if (streams->adxt != NULL) {
            ADXT_SetOutVol(streams->adxt, MUSIC_SILENT);
        }
        /* @hack spelling: sound_streams read again for the store, not
         * through streams */
        sound_streams->fade = STREAM_FADING_IN;
        /* @bug logic: while the music waits or plays, no fade is heard.
         * music_set_volume sets the stream to volume over the MUSIC_SILENT
         * above, so the fade's first step finds nothing to raise. BRA at
         * 0x8c0119da enters music_set_volume, whose JMP @R3 at 0x8c01199a
         * sets the volume */
        music_set_volume(volume);
    }
}

void music_fade_out(void)
{
    struct sound_streams *streams = sound_streams;

    if (streams != NULL) {
        streams->fade = STREAM_FADING_OUT;
    }
}

Bool voice_can_start(int priority, int index)
{
    struct sound_streams *streams;

    if (index >= VOICES) {
        return FALSE;
    }
    streams = sound_streams;
    if (streams == NULL) {
        return FALSE;
    }
    switch (streams->voices[index].status) {
    case ADXT_STAT_STOP:
    case ADXT_STAT_PLAYEND:
        return TRUE;
    }
    switch (streams->voices[index].priority) {
    case 0:
        return FALSE;
    case 1:
        if (priority == 0) {
            return TRUE;
        }
        return FALSE;
    case 2:
        switch (priority) {
        case 0:
        case 1:
        case 2:
            return TRUE;
        }
        return FALSE;
    case 3:
        switch (priority) {
        case 0:
        case 1:
        case 3:
            return TRUE;
        }
        return FALSE;
    /* @hack redundant: the cases -1 and 4, which return TRUE as every other
     * priority does */
    case -1:
    case 4:
        return TRUE;
    }
    return TRUE;
}

int voice_play(int id, int priority)
{
    int i;

    if (game_state.voices_on) {
        for (i = 0; i < VOICES; i++) {
            voice_last++;
            if (voice_last >= VOICES) {
                voice_last = 0;
            }
            if (voice_can_start(priority, voice_last)) {
                sound_streams->voices[voice_last].id = id;
                sound_streams->voices[voice_last].state = STREAM_PLAYING;
                sound_streams->voices[voice_last].priority = priority;
                return voice_last;
            }
        }
    }
    return -1;
}

void voice_play_priority_2(int id)
{
    voice_play(id, 2);
}

void voices_stop(void)
{
    int i;
    struct sound_streams *streams;

    if (sound_streams != NULL) {
        for (i = 0; i < VOICES; i++) {
            /* @hack spelling: sound_streams read again on each pass, where one
             * read ahead of the loop would do */
            streams = sound_streams;
            if (streams->voices[i].adxt != NULL) {
                ADXT_Stop(streams->voices[i].adxt);
            }
        }
    }
}

void voice_stop(int index)
{
    struct sound_streams *streams = sound_streams;

    if (streams == NULL) {
        return;
    }
    if (streams->voices[index].adxt != NULL) {
        ADXT_Stop(streams->voices[index].adxt);
    }
}

int voice_status(int index)
{
    struct sound_streams *streams = sound_streams;

    if (streams == NULL) {
        return -1;
    }
    return streams->voices[index].status;
}

/* ADXT_GetTime's count of the time stream index has played, as music_time
 * gives the music's. returns 0 without a sound task, or where the stream
 * wasn't created. index must be below VOICES, unchecked */
int voice_time(int index)
{
    struct sound_streams *streams = sound_streams;
    Sint32 count;
    Sint32 scale;

    if (streams != NULL && streams->voices[index].adxt != NULL) {
        ADXT_GetTime(streams->voices[index].adxt, &count, &scale);
        return count;
    }
    return 0;
}

static void voices_pause(void)
{
    int i;
    struct sound_streams *streams;

    for (i = 0; i < VOICES; i++) {
        streams = sound_streams;
        if (streams != NULL && streams->voices[i].adxt != NULL) {
            ADXT_Pause(streams->voices[i].adxt, TRUE);
            /* @hack spelling: sound_streams read again for the store, not
             * through streams */
            sound_streams->voices[i].state = STREAM_PAUSED;
        }
    }
}

static void voices_resume(void)
{
    int i;
    struct sound_streams *streams;

    for (i = 0; i < VOICES; i++) {
        streams = sound_streams;
        if (streams != NULL && streams->voices[i].adxt != NULL) {
            ADXT_Pause(streams->voices[i].adxt, FALSE);
            /* @hack spelling: sound_streams read again for the store, not
             * through streams */
            sound_streams->voices[i].state = STREAM_PLAYING;
        }
    }
}

void music_drop_played(void)
{
    struct sound_streams *streams = sound_streams;

    if (streams == NULL) {
        return;
    }
    if (streams->played != NULL) {
        syFree(streams->played);
        /* @hack spelling: sound_streams read again for the store, not
         * through streams */
        sound_streams->played = NULL;
    }
    music_play();
}

/* where played or next still holds the jingle, the name in task's extra, call
 * music_play. a jingle still in next is dropped first */
static void jingle_destroy(struct task *task)
{
    char *name = task->extra.p;
    char *other;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((other = sound_streams->played) != NULL && strcmp(other, name) == 0) {
        music_play();
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((other = sound_streams->next) != NULL && strcmp(other, name) == 0) {
        syFree(sound_streams->next);
        sound_streams->next = NULL;
        music_play();
    }
}

/* kill the task once the jingle, the name in task's extra, is over. it is
 * over where the music's stream has stopped or ended and next isn't the
 * jingle. it is over too where the stream runs in STREAM_PLAYING and played
 * isn't the jingle */
static void jingle_step(struct task *task)
{
    char *other;

    switch (music_status()) {
    case ADXT_STAT_STOP:
    case ADXT_STAT_PLAYEND:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((other = sound_streams->next) == NULL ||
            strcmp(other, task->extra.p) != 0) {
            task_kill(task);
        }
        break;
    default:
        if (sound_streams->state == STREAM_PLAYING) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((other = sound_streams->played) == NULL ||
                strcmp(other, task->extra.p) != 0) {
                task_kill(task);
            }
        }
    }
}

void jingle_play(const char *name)
{
    struct task *task;
    char *copy;

    music_mark_heard(name);
    if (game_state.music_on) {
        if (sound_task != NULL) {
            task = task_create(0, JINGLE_LEVEL, jingle_step, "JingleTask");
            if (task != NULL) {
#ifdef NONMATCHING
                /* this side repairs the tag below: copy_music_name allocates
                 * the copy's null too */
                copy = copy_music_name(name);
                if (copy != NULL) {
                    task->extra.p = copy;
#else
                /* @bug memory: strcpy overruns the copy by its null, as in
                 * music_set_name. MOV R0,R4 at 0x8c011eec passes strlen's
                 * result */
                copy = syMalloc(strlen(name));
                if (copy != NULL) {
                    task->extra.p = copy;
                    strcpy(copy, name);
#endif
                    task->destructor = jingle_destroy;
                    music_play_next(name);
                } else {
                    task_kill(task);
                }
            }
        }
    }
}

void music_play_or_defer(const char *name)
{
    if (sequence_mini_event_due()) {
#ifdef NONMATCHING
        /* this side defers no name music_deferred can't hold, which strcpy
         * would write past */
        if (name == NULL || strlen(name) >= sizeof(music_deferred)) {
            return;
        }
#endif
        strcpy(music_deferred, name);
    } else {
        music_play_name(name);
    }
}

void music_play_deferred(void)
{
    music_play_name(music_deferred);
}

/* set name's bit of MUSIC_HEARD_BITS, or the music's for a null name, which
 * needs a sound task. a name not in music_names has no bit */
static void music_mark_heard(const char *name)
{
    /* @hack spelling: bits, a static pointer to MUSIC_HEARD_BITS, where
     * indexing the bits themselves would do */
    static Uint8 *bits = MUSIC_HEARD_BITS;
    const char *key;
    int i;

    if (name == NULL) {
        if (sound_streams->name == NULL) {
            return;
        }
        key = sound_streams->name;
    } else {
        key = name;
    }
    for (i = 0; i < MUSIC_NAMES; i++) {
        if (strcmp(key, music_names[i]) == 0) {
            bits[i / 8] |= 1 << (i % 8);
            break;
        }
    }
}

int music_heard(const char *name)
{
    /* @hack spelling: bits, a static pointer to MUSIC_HEARD_BITS, where
     * indexing the bits themselves would do */
    static Uint8 *bits = MUSIC_HEARD_BITS;
    int i;

    if (name != NULL) {
        for (i = 0; i < MUSIC_NAMES; i++) {
            if (strcmp(name, music_names[i]) == 0) {
                return bits[i / 8] & (1 << (i % 8));
            }
        }
        return 0;
    }
    /* @hack implicit: a null name returns no value, where return 0 would do.
     * the game's one call of music_heard, in ADVERTISE.PRS, passes an entry
     * of music_names. no entry of music_names is null */
#ifdef NONMATCHING
    /* the tag's plain form */
    return 0;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void music_mark_heard_number(int number)
{
    /* @hack spelling: bits, a static pointer to MUSIC_HEARD_BITS, where
     * indexing the bits themselves would do */
    static Uint8 *bits = MUSIC_HEARD_BITS;

    bits[number / 8] |= 1 << (number % 8);
}
