/* the mini events: scenes a stage plays from an event's files */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#ifdef NONMATCHING
/* memset, with which this side's script reader clears a script it refuses */
#include <string.h>
#endif

#include "camera/camera.h"
#include "camera/motion.h"
#include "camera/stack.h"
#include "draw/fade.h"
#include "event/mini_event.h"
#ifdef NONMATCHING
/* event_message_list, by which this side's mini_event_load checks the
 * messages file */
#include "event/message_list.h"
#endif
#include "event/messages.h"
#include "hud/message_window.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#ifdef NONMATCHING
/* player_pads, where this side's skip reads Start */
#include "system/pad.h"
#endif
#include "system/task.h"
#ifdef NONMATCHING
/* TEXT_LANGUAGES, by which this side's mini_event_load bounds the language */
#include "system/text.h"
#endif
#include "vmu/vibration.h"

/* the characters an event's file has room for, numbered as player.h's
 * PLAYER_ characters */
#define EVENT_CHARACTERS 9

#define EVENT_FILE_AREA DEMO_AREA
#define SCRIPT_AREA_LONGS ((REQUEST_AREA - SCRIPT_AREA) / sizeof(Sint32))
/* evmesH holds the messages of the events below MESSAGES_H_END. evmesD holds
 * those below MESSAGES_D_END, and evmesL the rest */
#define MESSAGES_H_END 100
#define MESSAGES_D_END 200
#define EVENT_LEVEL 2

/* the head of me<event>.prs */
struct mini_event_file {
    Uint32 characters; /* bit n names character n */
    /* the camera's motion, whose nbFrame is the event's length */
    NJS_MOTION *camera;
    /* character n's entry at n, where characters names it */
    struct player_event *entries[EVENT_CHARACTERS];
};

#define EVENT_FILE ((struct mini_event_file *)EVENT_FILE_AREA)

/* line n opens a window with the event's message n at frame */
struct mini_event_line {
    Sint32 frame;
    Sint32 time; /* the window's time, as message_window_open takes it */
};

/* what a script plays at frame. music and jingle ask for nothing where they
 * start with a NUL or a space */
struct mini_event_cue {
    Sint32 frame;
    Sint8 fade; /* a FADE_ state for mini_event_fade, or 0 or less for none */
    Sint8 sounds[2]; /* ids for sound_play, each below 0 for none */
    char unk_07;
    /* a line whose voice mini_event_play_voice plays, or below 0 for none */
    Sint16 voice;
    /* a name for music_play_name, or one that starts with - for
     * music_play_deferred */
    char music[16];
    char jingle[16]; /* a name for jingle_play */
    Sint32 vibration; /* a vibration code */
    char unk_30[0x1c];
};

/* a script's room for lines and cues. message n opens at line n's frame, so
 * an event may have no more than SCRIPT_LINES messages, unchecked */
#define SCRIPT_LINES 32
#define SCRIPT_CUES 64

/* me<event>_<language>.scr */
struct mini_event_script {
    struct mini_event_line lines[SCRIPT_LINES];
    struct mini_event_cue cues[SCRIPT_CUES];
    /* where mini_event_start puts every player, and the camera motion's
     * origin */
    NJS_POINT3 pos;
    struct angles ang; /* the angles mini_event_start gives every player */
    Angle camera_ang; /* the turn the camera's motion plays under */
};

/* an event's windows show their first page WINDOW_DROP pixels below
 * MESSAGE_WINDOW_Y, with no band. the window that says an event has no
 * messages shows for NO_MESSAGES_STEPS steps */
#define WINDOW_DROP 30
#define NO_MESSAGES_STEPS 600

/* the record at REQUEST_AREA, which holds requests for
 * mini_event_serve_requests and the state it reports */
struct mini_event_requests {
    Sint32 event; /* the loaded event's number */
    Sint32 load; /* an event to load, or 0 for none */
    Sint32 start; /* nonzero to start the loaded event */
    Sint32 running; /* game_state.mini_event_running */
    Sint32 length; /* the loaded event's length, in whole frames */
    Sint32 message_count;
    Sint32 sound; /* an id for sound_play, or below 0 for none */
    char music[16]; /* a name for music_play_name, or a NUL for none */
    char jingle[16]; /* a name for jingle_play, or a NUL for none */
    Sint32 voice; /* a line for mini_event_play_voice, or below 0 for none */
    NJS_POINT3 pos; /* player 0's pos */
    struct angles ang; /* player 0's ang */
    Sint32 language; /* game_state.text_language */
    Sint32 vibration; /* a vibration code */
};

/* mini_event_fade's states. a cue's FADE_IN asks for a fade in from black. its
 * FADE_OUT asks for a fade out to the fade's color. the step that plays the cue
 * starts the fade and zeroes game_state.idle_calls. each step after it runs
 * fade_in_step or fade_out_step until the fade ends. a state above FADING_OUT
 * runs no fade, and stays until a cue sets another */
#define FADE_NONE 0
#define FADE_IN 1
#define FADE_OUT 2
#define FADING_IN 3
#define FADING_OUT 4

/* a vibration code gives vibration_start's player in one decimal digit. its
 * power, freq and time take two digits each. each _PLACE is the place value of
 * its field's lowest digit. a code of FADING_PLACE or more passes its fields
 * below FADING_PLACE to vibration_start_fading instead. a code of 0 or less
 * asks for none */
#define FADING_PLACE 10000000
#define PLAYER_PLACE 1000000
#define POWER_PLACE 10000
#define FREQ_PLACE 100

static void mini_event_select_messages(int event);
static void mini_event_end(void);
static void mini_event_step(struct task *task);
static void mini_event_destroy(struct task *task);
static void mini_event_play_frame(int event);
static void mini_event_clear_pending(void);
static void mini_event_fade_step(void);
static void mini_event_play_voice(int line);
static void mini_event_vibrate(int code);

static struct message_list *mini_event_message_lists =
    (struct message_list *)MESSAGE_AREA;
static struct mini_event_script *mini_event_script =
    (struct mini_event_script *)SCRIPT_AREA;
/* MESSAGE_AREA again */
static void *dat_8c0fbde8 = MESSAGE_AREA;
static struct mini_event_requests *mini_event_requests =
    (struct mini_event_requests *)REQUEST_AREA;

Float mini_event_frame;
static Float mini_event_length;
/* mini_event_frame before the last step. a step plays the lines and cues
 * whose frames lie above it, up to mini_event_frame. one at frame 0 or below
 * never plays */
static Float mini_event_previous_frame;
/* by character n of the event's file, the place of slot n's player before
 * player_start_event. mini_event_destroy puts it back. the loops take n as a
 * slot, though character n's own player may lie in another slot */
static struct {
    NJS_POINT3 pos;
    struct angles ang;
} mini_event_slot_places[EVENT_CHARACTERS];
static struct task *mini_event_task;
static Sint32 mini_event_number;
static Sint32 mini_event_message_count;
/* the event's list in the messages file, or null where it has none */
static struct message_list *mini_event_messages;
/* each slot's player's place before mini_event_start moves it to the
 * script's pos. mini_event_end puts it back */
static NJS_POINT3 mini_event_player_pos[PLAYER_SLOTS];
static struct angles mini_event_player_ang[PLAYER_SLOTS];
static Sint32 mini_event_fade; /* a FADE_ state */

#ifdef NONMATCHING
static Bool mini_event_record(Uint32 bytes, const void *record, Uint32 width)
{
    return ((uintptr_t)record & 3) == 0 &&
           width <= event_file_remaining(EVENT_FILE_AREA, bytes, record);
}

static Bool mini_event_motion(Uint32 bytes, const NJS_MOTION *motion)
{
    return mini_event_record(bytes, motion, sizeof(*motion)) &&
           motion->nbFrame > 0 && mini_event_record(bytes, motion->mdata, 4);
}

static Bool mini_event_camera(Uint32 bytes, const NJS_MOTION *motion)
{
    const void *const *keys;
    const Uint32 *counts;
    unsigned int channels;
    unsigned int type;
    unsigned int bit;
    unsigned int index;
    Uint32 width;

    if (!mini_event_motion(bytes, motion)) {
        return FALSE;
    }
    channels = motion->inp_fn & ~NJD_MTYPE_MASK;
    if (channels < 1 || channels > 4 ||
        !mini_event_record(bytes, motion->mdata,
                           channels * (sizeof(*keys) + sizeof(*counts)))) {
        return FALSE;
    }
    keys = motion->mdata;
    counts = (const Uint32 *)(keys + channels);
    /* njGetCameraMotion groups target/direction and spot/point, and skips
     * a slot for rotation, scale, color, intensity and spot or point. */
    type = motion->type;
    if (type & NJD_MTYPE_TARGET_3) {
        type &= ~NJD_MTYPE_VEC_3;
    }
    if (type & NJD_MTYPE_SPOT_10) {
        type &= ~NJD_MTYPE_POINT_10;
    }
    type &= ~(NJD_MTYPE_VERT_4 | NJD_MTYPE_NORM_5);
    index = 0;
    for (bit = 1; bit <= NJD_MTYPE_POINT_10; bit <<= 1) {
        if (!(type & bit)) {
            continue;
        }
        if (index >= channels) {
            return FALSE;
        }
        switch (bit) {
        case NJD_MTYPE_POS_0:
        case NJD_MTYPE_TARGET_3:
        case NJD_MTYPE_VEC_3:
            width = sizeof(NJS_MKEY_F);
            break;
        case NJD_MTYPE_ROLL_6:
        case NJD_MTYPE_ANGLE_7:
            width = sizeof(NJS_MKEY_A1);
            break;
        default:
            width = 0;
            break;
        }
        if (width && keys[index] != NULL &&
            (!counts[index] || ((uintptr_t)keys[index] & 3) ||
             counts[index] >
                 event_file_remaining(EVENT_FILE_AREA, bytes, keys[index]) /
                     width)) {
            return FALSE;
        }
        index++;
    }
    return TRUE;
}

static Bool mini_event_part(Uint32 bytes, const struct player_event_part *part)
{
    return part->object == NULL ||
           (mini_event_record(bytes, part->object, sizeof(*part->object)) &&
            (part->motion == NULL || mini_event_motion(bytes, part->motion)) &&
            (part->shape == NULL || mini_event_motion(bytes, part->shape)));
}

static Bool mini_event_scene(Sint32 bytes)
{
    const struct player_event *entry;
    int i;

    if (bytes < (Sint32)sizeof(*EVENT_FILE) ||
        EVENT_FILE->characters >> PLAYER_SLOTS ||
        !mini_event_camera((Uint32)bytes, EVENT_FILE->camera)) {
        return FALSE;
    }
    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (EVENT_FILE->characters & (1U << i)) {
            entry = EVENT_FILE->entries[i];
            if (!mini_event_record(bytes, entry, sizeof(*entry)) ||
                !mini_event_motion(bytes, entry->motion) ||
                !mini_event_part(bytes, &entry->part0) ||
                !mini_event_part(bytes, &entry->part1) ||
                !mini_event_part(bytes, &entry->part2) ||
                !mini_event_part(bytes, &entry->part3)) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

static Bool mini_event_cue_name(const char *name, Uint32 size)
{
    Uint32 i;

    if (name[0] == ' ') {
        return TRUE;
    }
    for (i = 0; i < size; i++) {
        if (name[i] == '\0') {
            return TRUE;
        }
    }
    return FALSE;
}

static void mini_event_read_script(const char *name)
{
    struct mini_event_cue *cue;
    Uint32 i;

    if (loader_read_file_checked(name, SCRIPT_AREA,
                                 (Uint32)(REQUEST_AREA - SCRIPT_AREA)) >=
        (Sint32)sizeof(*mini_event_script)) {
        for (i = 0; i < SCRIPT_CUES; i++) {
            cue = &mini_event_script->cues[i];
            if (cue->frame > 0 &&
                (!mini_event_cue_name(cue->music, sizeof(cue->music)) ||
                 !mini_event_cue_name(cue->jingle, sizeof(cue->jingle)))) {
                break;
            }
        }
        if (i == SCRIPT_CUES) {
            return;
        }
    }
    /* An absent script uses the zeroed script, including the player and
     * camera origin. A short transfer must discard any sectors it wrote. */
    memset(mini_event_script, 0, sizeof(*mini_event_script));
}

void mini_event_load(int event)
{
    char path[32];
    char group;
    Sint32 size;
    struct message_list *selected;

    if (game_state.mini_event_running) {
        return;
    }
    mini_event_number = 0;
    mini_event_length = 0.0F;
    mini_event_messages = NULL;
    mini_event_message_count = 0;
    if (event < 0 || event > 9999 ||
        (Uint32)game_state.text_language >= TEXT_LANGUAGES ||
        gdFsChangeDir("EVENT") != GDD_ERR_OK) {
        return;
    }
    sprintf(path, "me%04d.prs", event);
    size = loader_read_prs_checked(path, EVENT_FILE_AREA, DEMO_AREA_BYTES);
    if (!mini_event_scene(size)) {
        gdFsChangeDir("..");
        return;
    }
    group = event < MESSAGES_H_END ? 'H' : event < MESSAGES_D_END ? 'D' : 'L';
    sprintf(path, "evmes%c%d.prs", group, game_state.text_language);
    size = loader_read_prs_checked(path, MESSAGE_AREA,
                                   (Uint32)(SCRIPT_AREA - MESSAGE_AREA));
    if (size < 0 || !event_message_list(MESSAGE_AREA, (Uint32)size, event,
                                        SCRIPT_LINES, &selected)) {
        mini_event_message_lists[0].event = -1;
    }
    sprintf(path, "me%04d_%d.scr", event, game_state.text_language);
    mini_event_read_script(path);
    if (gdFsChangeDir("..") != GDD_ERR_OK) {
        return;
    }
    music_stop();
    mini_event_number = event;
    mini_event_length = EVENT_FILE->camera->nbFrame;
    mini_event_select_messages(event);
}

#else
void mini_event_load(int event)
{
    char path[256];
    Sint32 *at;
    Sint32 left;

    music_stop();
    gdFsChangeDir("EVENT");
    sprintf(path, "me%04d.prs", event);
    loader_read_prs(path, EVENT_FILE_AREA);
    mini_event_number = event;
    if (event < MESSAGES_H_END) {
        sprintf(path, "evmesH%1d.prs", game_state.text_language);
    } else if (event < MESSAGES_D_END) {
        sprintf(path, "evmesD%1d.prs", game_state.text_language);
    } else {
        sprintf(path, "evmesL%1d.prs", game_state.text_language);
    }
    loader_read_prs(path, MESSAGE_AREA);
    mini_event_select_messages(event);
    /* @hack spelling: the area cleared through a pointer under a count that
     * runs down to 0, where for (i = 0; i < SCRIPT_AREA_LONGS; i++) over an
     * index would do */
    at = (Sint32 *)SCRIPT_AREA;
    for (left = SCRIPT_AREA_LONGS; left != 0; left--) {
        *at++ = 0;
    }
    sprintf(path, "me%04d_%1d.scr", event, game_state.text_language);
    loader_read_file(path, SCRIPT_AREA);
    gdFsChangeDir("..");
    mini_event_length = EVENT_FILE->camera->nbFrame;
}
#endif

int mini_event_start(void)
{
    Sint32 i;
    Sint32 top;
    struct camera_motion motion;

#ifdef NONMATCHING
    /* this side starts no event that mini_event_load didn't load, which leaves
     * the length 0, nor one while another runs */
    if (mini_event_length == 0.0F || game_state.mini_event_running) {
        return -1;
    }
#endif
    mini_event_task =
        task_create(TASK_WORK, EVENT_LEVEL, mini_event_step, "miniEventExec");
    if (mini_event_task == NULL) {
        return -1;
    }
    mini_event_task->destructor = mini_event_destroy;
    game_state.mini_event_running = 1;
    /* @bug memory: reads player_work[8], past its PLAYER_SLOTS slots and into
     * player_motion at 0x8c251298, for a file that names character 8. the
     * loop's last pass (MOV #9,R8 at 0x8c0b9636) reads it with
     * MOV.L @(R0,R13),R3 at 0x8c0b964a */
    for (i = 0; i < EVENT_CHARACTERS; i++) {
        if (EVENT_FILE->characters & (1 << i)) {
            if (player_work[i] != NULL) {
                mini_event_slot_places[i].pos = player_work[i]->pos;
                mini_event_slot_places[i].ang = player_work[i]->ang;
            }
            player_start_event(i, EVENT_FILE->entries[i]);
        }
    }
    top = camera_top(0);
    while (top > CAMERA_MOTION_SLOT) {
        camera_remove_mode(0, top);
        top = camera_top(0);
    }
    motion.speed = 1.0F;
    motion.ang = mini_event_script->camera_ang;
    motion.motion = EVENT_FILE->camera;
    motion.origin = &mini_event_script->pos;
    motion.flags = CAMERA_MOTION_CALLS_DONE | CAMERA_MOTION_TURNS;
    motion.done = mini_event_end;
#ifdef NONMATCHING
    /* this side ends the event before it starts where camera_motion_start
     * returns 0. camera_motion_start returns 0 for a motion of no frames, as
     * motion.h says */
    if (!camera_motion_start(0, &motion)) {
        task_destroy(mini_event_task);
        return -1;
    }
#else
    camera_motion_start(0, &motion);
#endif
    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (player_work[i] != NULL) {
            mini_event_player_pos[i] = player_work[i]->pos;
            mini_event_player_ang[i] = player_work[i]->ang;
            player_set_pos(i, mini_event_script->pos.x,
                           mini_event_script->pos.y, mini_event_script->pos.z);
            player_work[i]->ang = mini_event_script->ang;
        }
    }
    mini_event_clear_pending();
    mini_event_frame = 0.0F;
    /* @hack spelling: mini_event_previous_frame set from mini_event_frame,
     * where 0.0F would do */
    mini_event_previous_frame = mini_event_frame;
    game_state.idle_dim = 0;
    return 0;
}

#ifdef NONMATCHING
/* the record of a new unowned window, or null where none opens.
 * message_window_record takes no null task */
static struct message_window *mini_event_open_window(const char *text, int time)
{
    struct task *task;

    task = message_window_open_unowned(MESSAGE_WINDOW_FADE, text, time,
                                       game_state.text_language);
    return task != NULL ? message_window_record(task) : NULL;
}
#endif

/* the messages file's lists must hold event's list or end at one below 0,
 * unchecked. where they end first, open a window that says so */
static void mini_event_select_messages(int event)
{
    char text[48];
    Sint32 i;
    struct message_window *window;

    for (i = 0; mini_event_message_lists[i].event != event; i++) {
        if (mini_event_message_lists[i].event < 0) {
            mini_event_messages = NULL;
            /* "e<event>_<language> has no message table", in Shift JIS */
            sprintf(text,
                    "e%04d_%d\202\314\203\201\203b\203Z\201[\203W\203e\201["
                    "\203u\203\213\202\315\202\240\202\350\202\334\202\271"
                    "\202\361",
                    event, game_state.text_language);
#ifdef NONMATCHING
            /* this side lists no messages where the window can't open, whose
             * record the lines below write */
            window = mini_event_open_window(text, NO_MESSAGES_STEPS);
            if (window == NULL) {
                mini_event_message_count = 0;
                return;
            }
#else
            window = message_window_record(message_window_open_unowned(
                MESSAGE_WINDOW_FADE, text, NO_MESSAGES_STEPS,
                game_state.text_language));
#endif
            window->sprite.band_color.color = 0;
            window->sprite.y += WINDOW_DROP;
            mini_event_message_count = 0;
            return;
        }
    }
    mini_event_messages = &mini_event_message_lists[i];
    mini_event_message_count = mini_event_messages->count;
}

/* the camera motion's done: end the event, and put each player back where
 * mini_event_start found it */
static void mini_event_end(void)
{
    Sint32 i;

    task_destroy(mini_event_task);
    for (i = 0; i < PLAYER_SLOTS; i++) {
        if (player_work[i] != NULL) {
            player_set_pos(i, mini_event_player_pos[i].x,
                           mini_event_player_pos[i].y,
                           mini_event_player_pos[i].z);
            player_work[i]->ang = mini_event_player_ang[i];
        }
    }
}

#ifdef NONMATCHING
/* this side's skip, which SEGA's game has for an event of the EVENT overlay
 * and not for a mini event. it moves the camera's motion and the event's
 * frame to the step before their last. the next step then ends the event as
 * SEGA's code ends it: the players go back to their places, and the stage
 * plays on.
 *
 * of the cues the skip passes, the latest music alone plays, since a stage
 * holds its music back for its mini event's cue. the rest are the scene's
 * own, and none plays: no sound, voice, fade or vibration, and no line's
 * window. the window that shows closes, and the voices stop. a fade a cue
 * began runs on as the stage steps it */
static void mini_event_skip(void)
{
    struct mini_event_cue *cue;
    struct mini_event_cue *music = NULL;
    Float to;
    int steps;
    int i;

    steps = camera_motion_skip(0);
    if (steps == 0) {
        return;
    }
    to = mini_event_frame + (Float)steps;
    if (to >= mini_event_length - 1.0F) {
        to = mini_event_length - 1.0F;
    }
    for (i = 0; i < SCRIPT_CUES; i++) {
        cue = &mini_event_script->cues[i];
        if (cue->frame <= to && cue->frame > mini_event_frame &&
            cue->music[0] != '\0' && cue->music[0] != ' ' &&
            (music == NULL || cue->frame >= music->frame)) {
            music = cue;
        }
    }
    if (music != NULL) {
        if (music->music[0] == '-') {
            music_play_deferred();
        } else {
            music_play_name(music->music);
        }
    }
    mini_event_frame = to;
    if (message_window_current() != NULL) {
        task_kill(message_window_current());
    }
    voices_stop();
}
#endif

static void mini_event_step(struct task *task)
{
#ifdef NONMATCHING
    /* this side takes Start on the first pad for the skip above. the stage
     * takes it for no pause while an event runs */
    if (player_pads[0]->press & PDD_DGT_ST) {
        mini_event_skip();
    }
#endif
    mini_event_previous_frame = mini_event_frame;
    mini_event_frame += 1.0F;
    if (mini_event_frame >= mini_event_length - 1.0F) {
        mini_event_frame = mini_event_length - 1.0F;
    }
    /* @hack redundant: the parameter event, which mini_event_play_frame never
     * reads, and the mini_event_number passed for it */
    mini_event_play_frame(mini_event_number);
}

static void mini_event_destroy(struct task *task)
{
    Sint32 i;

    /* @bug memory: writes a place through player_work[8], past its
     * PLAYER_SLOTS slots, for a file that names character 8. the loop's last
     * pass (MOV #9,R8 at 0x8c0b9d2e) reads player_motion[0] there with
     * MOV.L @(R0,R13),R3 at 0x8c0b9d48, and the stores from 0x8c0b9d66 write
     * through it */
    for (i = 0; i < EVENT_CHARACTERS; i++) {
        if (EVENT_FILE->characters & (1 << i)) {
            player_end_event(i);
            if (player_work[i] != NULL) {
                player_work[i]->pos = mini_event_slot_places[i].pos;
                player_work[i]->ang = mini_event_slot_places[i].ang;
            }
        }
    }
    vibration_stop();
    game_state.mini_event_running = 0;
    game_state.idle_dim = 1;
}

static void mini_event_play_frame(int event)
{
    Sint32 i;
    struct message_window *window;

    if (mini_event_messages != NULL) {
        for (i = 0; i < mini_event_messages->count; i++) {
            if (mini_event_script->lines[i].frame <= mini_event_frame &&
                mini_event_script->lines[i].frame > mini_event_previous_frame) {
#ifdef NONMATCHING
                /* this side skips a line whose window can't open, whose record
                 * the lines below write */
                window = mini_event_open_window(
                    mini_event_messages->messages[i].text,
                    mini_event_script->lines[i].time);
                if (window == NULL) {
                    continue;
                }
#else
                window = message_window_record(message_window_open_unowned(
                    MESSAGE_WINDOW_FADE, mini_event_messages->messages[i].text,
                    mini_event_script->lines[i].time,
                    game_state.text_language));
#endif
                window->sprite.band_color.color = 0;
                window->sprite.y += WINDOW_DROP;
            }
        }
    }
    for (i = 0; i < SCRIPT_CUES; i++) {
        if (mini_event_script->cues[i].frame <= mini_event_frame &&
            mini_event_script->cues[i].frame > mini_event_previous_frame) {
            if (mini_event_script->cues[i].sounds[0] >= 0) {
                sound_play(mini_event_script->cues[i].sounds[0], NULL, 0, 0);
            }
            if (mini_event_script->cues[i].sounds[1] >= 0) {
                sound_play(mini_event_script->cues[i].sounds[1], NULL, 0, 0);
            }
            if (mini_event_script->cues[i].voice >= 0) {
                mini_event_play_voice(mini_event_script->cues[i].voice);
            }
            if (mini_event_script->cues[i].music[0] != '\0' &&
                mini_event_script->cues[i].music[0] != ' ') {
                if (mini_event_script->cues[i].music[0] == '-') {
                    music_play_deferred();
                } else {
                    music_play_name(mini_event_script->cues[i].music);
                }
            }
            if (mini_event_script->cues[i].jingle[0] != '\0' &&
                mini_event_script->cues[i].jingle[0] != ' ') {
                jingle_play(mini_event_script->cues[i].jingle);
            }
            if (mini_event_script->cues[i].fade > 0) {
                mini_event_fade = mini_event_script->cues[i].fade;
            }
            if (mini_event_script->cues[i].vibration > 0) {
                mini_event_vibrate(mini_event_script->cues[i].vibration);
            }
        }
    }
    mini_event_fade_step();
}

static void mini_event_clear_pending(void)
{
    mini_event_requests->sound = -1;
    mini_event_requests->music[0] = '\0';
    mini_event_requests->jingle[0] = '\0';
    mini_event_requests->voice = -1;
    mini_event_fade = FADE_NONE;
}

static void mini_event_fade_step(void)
{
    switch (mini_event_fade) {
    case FADE_IN:
        fade_set_black();
        mini_event_fade = FADING_IN;
        break;
    case FADE_OUT:
        fade_reset_idle();
        mini_event_fade = FADING_OUT;
        break;
    case FADING_IN:
        /* @hack spelling: == TRUE, where a truth test, if (fade_in_step()),
         * would do */
        if (fade_in_step() == TRUE) {
            mini_event_fade = FADE_NONE;
        }
        break;
    case FADING_OUT:
        /* @hack spelling: == TRUE, where a truth test, if (fade_out_step()),
         * would do */
        if (fade_out_step() == TRUE) {
            mini_event_fade = FADE_NONE;
        }
        break;
    }
}

#define SETTLE_STEPS 10

/* serve the requests in the record at REQUEST_AREA, and report there the state
 * after any load or start. a call does nothing where
 * camera_no_player(camera_current_index) isn't 0. it needs player 0's work
 * otherwise, unchecked.
 *
 * while no event runs, a nonzero load loads that event, and a nonzero start
 * starts the loaded one. the call clears both either way, so either is lost
 * while an event runs. it then plays and clears what sound, music, jingle,
 * voice and vibration ask for. a call that loads or starts an event drops the
 * sound, music, jingle and voice unplayed. so does a call while
 * game_state.steps is below SETTLE_STEPS */
/* nothing calls it, and no header declares it */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mini_event_serve_requests(void)
{
    if (camera_no_player(camera_current_index)) {
        return;
    }
    if (game_state.steps < SETTLE_STEPS) {
        mini_event_requests->sound = -1;
        mini_event_requests->music[0] = '\0';
        mini_event_requests->jingle[0] = '\0';
        mini_event_requests->voice = -1;
    }
    if (game_state.mini_event_running == 0 && mini_event_requests->load != 0) {
        mini_event_load(mini_event_requests->load);
        mini_event_requests->sound = -1;
        mini_event_requests->music[0] = '\0';
        mini_event_requests->jingle[0] = '\0';
        mini_event_requests->voice = -1;
    }
    if (game_state.mini_event_running == 0 && mini_event_requests->start != 0) {
        mini_event_start();
    }
    mini_event_requests->load = 0;
    mini_event_requests->start = 0;
    mini_event_requests->event = mini_event_number;
    mini_event_requests->running = game_state.mini_event_running;
    mini_event_requests->length = mini_event_length;
    mini_event_requests->message_count = mini_event_message_count;
    if (mini_event_requests->sound >= 0) {
        sound_play(mini_event_requests->sound, NULL, 0, 0);
        mini_event_requests->sound = -1;
    }
    if (mini_event_requests->music[0] != '\0') {
        music_fade_in(VOLUME_FULL);
        music_play_name(mini_event_requests->music);
        mini_event_requests->music[0] = '\0';
    }
    if (mini_event_requests->jingle[0] != '\0') {
        jingle_play(mini_event_requests->jingle);
        mini_event_requests->jingle[0] = '\0';
    }
    if (mini_event_requests->voice >= 0) {
        mini_event_play_voice(mini_event_requests->voice);
        mini_event_requests->voice = -1;
    }
    if (mini_event_requests->vibration > 0) {
        mini_event_vibrate(mini_event_requests->vibration);
        mini_event_requests->vibration = 0;
    }
    mini_event_requests->pos = player_work[0]->pos;
    mini_event_requests->ang = player_work[0]->ang;
    mini_event_requests->language = game_state.text_language;
}
/* NOLINTEND(misc-use-internal-linkage) */

/* play the voice of the loaded event's line, where event_voices holds
 * one */
static void mini_event_play_voice(int line)
{
    Sint32 i;
    Sint32 key;

    /* @hack spelling: the key summed in a statement of its own, where one
     * expression would do */
    key = mini_event_number * EVENT_KEYS;
    key += line;
    /* @hack spelling: a loop without a condition, which returns at the
     * table's end and breaks at the key, where for (i = 0;
     * event_voices[i].key != -1; i++), playing the voice and returning at
     * the key, would do */
    for (i = 0;; i++) {
        /* @hack redundant: an else after an arm that returns */
        if (event_voices[i].key == EVENT_VOICES_END) {
            return;
        } else if (event_voices[i].key == key) {
            break;
        }
    }
    voice_play_priority_2(event_voices[i].voice);
}

static void mini_event_vibrate(int code)
{
    if (code <= 0) {
        return;
    }
    if (code >= FADING_PLACE) {
        code %= FADING_PLACE;
        vibration_start_fading(
            code / PLAYER_PLACE, code % PLAYER_PLACE / POWER_PLACE,
            code % POWER_PLACE / FREQ_PLACE, code % FREQ_PLACE);
    } else {
        vibration_start(code / PLAYER_PLACE, code % PLAYER_PLACE / POWER_PLACE,
                        code % POWER_PLACE / FREQ_PLACE, code % FREQ_PLACE);
    }
}
