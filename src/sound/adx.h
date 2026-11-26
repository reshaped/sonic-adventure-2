/* the ADX streams: one for the music, and VOICES for voices. the music can
 * fade, and can give way to a jingle.
 *
 * the music is the file name music_set_name keeps, which music_play starts.
 * a voice is a file of the voice partition: event_adx.afs where
 * game_state.voice_language was VOICE_JAPANESE as sound_task_create ran, and
 * event_adx_e.afs otherwise.
 *
 * a step is one run of the sound task, which runs the streams. a start or a
 * fade takes effect on a step.
 *
 * music_play, music_play_name, music_change, music_play_next, music_stop and
 * music_play_deferred need a sound task. so does music_play_or_defer where it
 * plays. every other call does nothing without one, or returns what its
 * comment says. a name must not be null unless its comment says so */
#ifndef SOUND_ADX_H
#define SOUND_ADX_H
#include <cri_adxf.h>
#include <cri_adxt.h>
#include <sg_xpt.h>

/* the voices that can play at once, each on a stream of its own. a voice
 * call names a stream by its index */
#define VOICES 2

/* the steps the music waits before it starts */
#define MUSIC_START_STEPS 5

/* the files of the voice partition */
#define VOICE_FILES 0xa76
/* the bytes of the voice partition's table, as
 * ADXF_CALC_PTINFO_SIZE(VOICE_FILES) gives them */
#define VOICE_PARTITION_SIZE 0x152c
/* struct voice's id where no file waits to start */
#define VOICE_ID_NONE (-1)

/* the volume, in tenths of a decibel as ADXT_SetOutVol takes it, at which a
 * stream plays unattenuated */
#define VOLUME_FULL 0

/* the names in music_names. each has a bit in system/flags.h's
 * MUSIC_HEARD_BITS */
#define MUSIC_NAMES 137

/* the ADX filename for each bit in the save's music-heard set */
extern const char *music_names[MUSIC_NAMES];

/* a stream's states */
enum {
    STREAM_WAITING, /* the music waits for its timer to run out */
    /* no wait, fade or pause holds the stream. it may have played to its
     * end */
    STREAM_PLAYING,
    STREAM_FADING_IN,
    STREAM_FADING_OUT,
    STREAM_PAUSED
};

/* a voice's stream */
struct voice {
    Sint8 state; /* STREAM_PLAYING or STREAM_PAUSED */
    Sint8 priority; /* the priority its voice started at */
    /* the partition's file to start on the next step, or VOICE_ID_NONE */
    int id;
    int volume;
    /* what ADXT_GetStat returned on the last step that found it unpaused */
    int status;
    ADXT adxt;
    void *work; /* ADXT_Create's work */
};

/* the sound task's streams, in the task's extra */
struct sound_streams {
    Sint8 state; /* the music's STREAM_ state */
    Sint16 timer; /* the steps left before the music starts */
    /* STREAM_FADING_IN or STREAM_FADING_OUT for a fade asked for, or 0 for
     * none. it becomes state on the first step that finds the music neither
     * waiting nor paused */
    int fade;
    int volume; /* the music's volume where a fade in ends */
    int status; /* what ADXT_GetStat returned on the last step */
    ADXT adxt;
    char *name; /* the music, or null for none */
    /* the name music_play_next gave, which starts in the music's place, or
     * null. its start moves it to played */
    char *next;
    /* the name that started from next, or null. the music's start frees it,
     * as music_drop_played does */
    char *played;
    void *work; /* ADXT_Create's work */
    struct voice voices[VOICES];
    /* the voice partition's table, as ADXF_LoadPartition loads it */
    Uint8 partition[VOICE_PARTITION_SIZE];
};
/* the sound task's streams, or null without a sound task */
extern struct sound_streams *sound_streams;

/* start the ADX library, and forget any sound task. returns TRUE */
Bool sound_init(void);
/* sound_task_kill, then end the ADX library */
void sound_exit(void);
/* create the sound task and its streams, and load the voice partition's
 * table. where name isn't null, play the music name. returns FALSE where an
 * allocation fails, and leaves no sound task. returns TRUE otherwise, even
 * where the table fails to load.
 *
 * there must be no sound task. a second call's task destroys itself at once,
 * but leaves sound_streams null while the first plays on. with a name, the call
 * then reads through that null */
Bool sound_task_create(const char *name);
/* destroy the sound task now, if there is one */
void sound_task_kill(void);
/* music_pause, and pause every voice */
void sound_pause(void);
/* music_resume, and resume every voice */
void sound_resume(void);

/* keep a copy of name as the music, and start nothing. a null name, or a copy
 * the heap can't hold, leaves no music */
void music_set_name(const char *name);
/* mark the music heard. while game_state.music_on isn't 0, stop what plays
 * and start the music MUSIC_START_STEPS steps later, where there is music. a
 * name music_play_next left waiting starts in its place */
void music_play(void);
/* music_set_name, then music_play */
void music_play_name(const char *name);
/* music_play_name, unless name is the music already. there must be music */
void music_change(const char *name);
/* stop what plays, and start a copy of name in the music's place
 * MUSIC_START_STEPS steps later. the music doesn't come back when name ends.
 * music_play_next does nothing while game_state.music_on is 0 */
void music_play_next(const char *name);
/* stop the music's stream, and end any wait or fade the music is in */
void music_stop(void);
/* pause the music. music waiting to start starts at once, and pauses. music
 * that is fading, or waiting with nothing to start, isn't paused */
void music_pause(void);
/* resume the music's stream, and end any wait or fade the music is in */
void music_resume(void);
/* fade the music in from silence to volume, in tenths of a decibel as
 * ADXT_SetOutVol takes it. the fade starts on the first step that finds the
 * music neither waiting nor paused. while the music waits or plays, the
 * stream takes volume at once. the fade then has nothing to raise */
void music_fade_in(int volume);
/* fade the music out to silence, a step at a time, and then stop it. the
 * fade starts on the first step that finds the music neither waiting nor
 * paused */
void music_fade_out(void);

/* whether stream index may start a voice of priority. it may where the
 * stream's voice has stopped or played to its end, or gives way to priority.
 * a voice gives way by its own priority:
 * - 0 to none;
 * - 1 to priority 0;
 * - 2 to priorities 0, 1 and 2;
 * - 3 to priorities 0, 1 and 3;
 * - any other to every priority.
 * returns FALSE without a sound task, and for an index of VOICES or more. a
 * negative index is unchecked */
Bool voice_can_start(int priority, int index);
/* start file id of the voice partition at priority, on the next step. it
 * takes the first stream voice_can_start lets start it, from the one after the
 * stream it took last. returns that stream's index, or -1 where no stream may
 * start the voice or game_state.voices_on is 0 */
int voice_play(int id, int priority);
/* voice_play at priority 2, without its result */
void voice_play_priority_2(int id);
void voices_stop(void);
/* stop stream index, which must be below VOICES, unchecked */
void voice_stop(int index);
/* what ADXT_GetStat returned for stream index on the last step that found it
 * unpaused. returns -1 without a sound task. index must be below VOICES,
 * unchecked */
int voice_status(int index);

/* forget the name music_play_next started last, then music_play */
void music_drop_played(void);
/* mark name heard. while there is a sound task and game_state.music_on isn't
 * 0, play the jingle name as music_play_next does. a task then plays the
 * music again once the jingle ends, unless other music takes its place */
void jingle_play(const char *name);
/* music_play_name, or while sequence_mini_event_due() is TRUE, copy name for
 * music_play_deferred to play instead. The portable build keeps the pending
 * filename when a request has no name or exceeds the GDFS filename limit. */
void music_play_or_defer(const char *name);
/* music_play_name of the name music_play_or_defer copied last */
void music_play_deferred(void);

/* whether the music name is heard: its bit of MUSIC_HEARD_BITS, or 0 where
 * it isn't. music number n's bit is 1 << (n % 8), as it lies in its byte.
 * returns 0 where name isn't in music_names, and no defined value where name
 * is null */
int music_heard(const char *name);
/* mark music number heard. number must be below MUSIC_NAMES, unchecked */
void music_mark_heard_number(int number);

#endif
