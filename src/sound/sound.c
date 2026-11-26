/* the sound effects: the driver, the slots on its MIDI ports, the bank loads
 * and the sound test */
#include <ninja.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#ifdef NONMATCHING
/* port_stop, by which this side stops the game where the driver won't start */
#include "engine/dc/crash.h"
#endif

#include "camera/camera.h"
#include "hud/hud.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

#include "sound/sound/text.inc"

/* the slots. slots 0 to 31 play notes on the channels of NOTE_PORT and the
 * port after it. the character's sound plays in slots CHARACTER_SLOT to 47,
 * the channels of CHARACTER_PORT. it never plays in SKIPPED_SLOT, the port's
 * channel 9. slot SEQUENCE_SLOT + n plays sequences on port
 * SEQUENCE_PORT + n */
#define SLOTS 53
#define NOTE_SLOTS 32
#define NOTE_PORT 0
#define CHARACTER_SLOT 32
#define CHARACTER_PORT 2
#define SKIPPED_SLOT 41
#define SEQUENCE_SLOT 48
#define SEQUENCE_PORT 3
#define SEQUENCE_PORTS (SDD_MIDI_PORT_NUM - SEQUENCE_PORT)
#define SLOT_PORT(slot) ((slot) - SEQUENCE_SLOT + SEQUENCE_PORT)

/* ID_PLACE(value) is value in a sound id's place field, bits 12 to 15, whose
 * places sound.h gives. ID_INDEX_MASK keeps the bits below the field, which the
 * sound test steps. sound_of_id reads their low byte as the index */
#define ID_PLACE(value) ((value) << 12)
#define ID_PLACE_MASK 0xf000
#define ID_INDEX_MASK 0x0fff

/* the sound driver's file, and the slots sdSysSetSlotMax lets its MIDI, shot
 * and stream ports use */
#define DRIVER_MIDI_SLOTS 58
#define DRIVER_SHOT_SLOTS 0
#define DRIVER_STREAM_SLOTS 6
#define DRIVER_FILE "manatee.drv"
/* the effect program and output that sound_fx_on and the loads choose */
#define EFFECT_PROGRAM 0
#define EFFECT_OUTPUT 0
/* the master volume, 0 to 15, in mono and in stereo */
#define MONO_VOLUME 14
#define STEREO_VOLUME 15

/* the ports' reset plays RESET_SEQUENCE of RESET_BANK on the first RESET_PORTS
 * ports. where CHARACTER_PORT's flags then read SDD_PORT_FLG_PLAY alone, the
 * reset waits RESET_WAIT microseconds. where it picks a character's sound,
 * CHARACTER_PORT then plays RESET_SEQUENCE of CHARACTER_BANK */
#define RESET_PORTS 3
#define RESET_BANK 0
#define RESET_SEQUENCE 49
#define RESET_WAIT 2000000
#define CHARACTER_BANK 2
/* sound_load_player_sounds loads nothing on the stages from STAGE_CART_1 to
 * STAGE_CARTS_LAST */
#define STAGE_CARTS_LAST 79
/* an event's name that starts with EVENT_PREFIX loads EVENT_FILE first */
#define EVENT_PREFIX "e"
#define EVENT_FILE "e_test.mlt"

/* the MIDI note and velocity a note's slot plays its program at */
#define NOTE 48
#define VELOCITY 127
/* a note's MIDI volume at volume 0, and its rise for each step above 0 */
#define NOTE_VOLUME 100
#define NOTE_VOLUME_RISE 0.2126F
/* the highest MIDI volume or pan, and the middle pan. NOTE_PAN_SCALE is the
 * MIDI pan for each step of a note's pan */
#define MIDI_MAX 127
#define MIDI_PAN_MIDDLE 64.0F
#define NOTE_PAN_SCALE 0.5F
/* the pitch a slot sends runs from PITCH_MIN to PITCH_MAX. its bend is the
 * pitch plus BEND_MIDDLE. the bend's second byte takes the bend's bits under
 * MIDI_DATA_MASK, and its third byte those from BEND_HIGH_SHIFT up */
#define PITCH_MAX 0x3fff
#define PITCH_MIN (-0x4000)
#define BEND_MIDDLE 0x4000
#define MIDI_DATA_MASK 0x7f
#define BEND_HIGH_SHIFT 8
/* how far the volume by distance has fallen at a sound's range */
#define VOLUME_FALL 127.0F
/* PAN_DEAD_SINE is the sine of 25 degrees. the pan by angle is 0 while the
 * angle's sine is nearer 0 than that. past it, PAN_SCALE makes a pan of PAN_MAX
 * at right angles */
#define PAN_DEAD_SINE 0.42261F
#define PAN_SCALE 219.9553F
#define PAN_MAX 127
/* the pitch by distance: its change for each unit of distance, its bound,
 * and the distance beyond which it holds */
#define PITCH_PER_UNIT 10.24F
#define PITCH_BOUND 4096.0F
#define PITCH_REACH 400.0F
/* a slot of priority PRIORITY_TOP plays at PORT_PRIORITY_HIGH, the priority
 * sdMidiPlay and sdMidiSendMes take. a slot of any other plays at
 * PORT_PRIORITY_LOW */
#define PRIORITY_TOP 2
#define PORT_PRIORITY_HIGH 15
#define PORT_PRIORITY_LOW 1

/* a slot's mode: what the next update does with it, and how it ends */
#define MODE_START 0x0001 /* start the sound */
#define MODE_MOVED 0x0002 /* MODE_MOVING for one update. nothing sets it */
/* stop the sound once the time is 0. without it, the slot plays on */
#define MODE_ENDS 0x0004
/* at each update, copy a task call's pos from follow. under
 * MODE_VOLUME_BY_DISTANCE, set the volume by distance anew too */
#define MODE_MOVING 0x0008
#define MODE_VOLUME_BY_DISTANCE 0x0010 /* the volume falls with distance */
#define MODE_VOLUME_SET 0x0020 /* send the volume */
#define MODE_PAN_BY_POS 0x0040 /* pan by the pos's angle at each update */
#define MODE_PAN_SET 0x0080 /* send the pan */
/* set the pitch by distance at each update */
#define MODE_PITCH_BY_POS 0x0100
#define MODE_PITCH_SET 0x0200 /* send the pitch */
/* the bit of sound_play_sequence's slot. at time 0 the slot takes MODE_ENDS
 * once its port stops or its state can't be read */
#define MODE_SEQUENCE 0x0400
/* a task call's: the key is its task. the update that finds task_destroy as
 * the task's step ends the sound */
#define MODE_TASK 0x1000
#define MODE_RING 0x2000 /* sound_play_ring's, which nothing reads */
/* at time 0, play once more for repeat_time, then end */
#define MODE_REPEAT 0x4000

/* what a slot's channel holds already of the sound it starts. a start sends
 * only what the channel lacks */
#define CHANNEL_BANK_SET 0x01 /* the sound's bank */
#define CHANNEL_PROGRAM_SET 0x02 /* the sound's program */
#define CHANNEL_PAN_SET 0x08 /* a pan of 0 */
#define CHANNEL_PITCH_SET 0x10 /* a pitch of 0 */
/* the four bits above and bit 2, which nothing reads. a start of a slot whose
 * channel holds CHANNEL_SET sends no bank or program. it sends a pan or
 * pitch only where the mode asks */
#define CHANNEL_SET 0x1f
/* the channel has played a sound. a start sends the volume to a channel
 * without it */
#define CHANNEL_PLAYED 0x80

#define FX_HELD 0x1 /* sound_hold_fx_level holds a level */
/* with FX_HELD, the pause sets the held level again, and the resume sets
 * every port's level to fx_level. nothing sets it */
#define FX_PAUSED 0x2

#define PLAYS_AS_SEQUENCE 1
#define PLAYS_AS_NOTE 2

/* the id of no sound: a free slot's, and the character's sound's while there
 * is none */
#define NO_ID (-1)
/* the time of a held sound, and of sound_play's, sound_play_at's and
 * sound_play_ring's */
#define TIME_HELD (-1)
#define TIME_PLAY 120
/* the time sound_play_sequence's slot plays before its port is tested */
#define TIME_SEQUENCE 10
/* the time a slot of TIME_HELD gets once the stage is cleared */
#define TIME_CLEARED 10
/* the volume by distance falls from FALL_START on */
#define FALL_START 20.0F
#define VOLUME_MIN (-127)
/* a note's volume while the sound is paused, which silences it */
#define VOLUME_PAUSED (-128)
/* the bank and the number of sound/lists.h's rule for a sound that plays as
 * a sequence without SOUND_SEQUENCE */
#define SEQUENCE_BANK 6
#define SEQUENCE_NUMBER 50

/* a slot, and the sound it plays */
struct sound_slot {
    /* the sound's bank, and its program or sequence there */
    Sint8 bank;
    Sint8 number;
    Sint8 priority;
    Sint8 pan; /* the pan the next send takes */
    /* the volume last sent, which the resume sends again. a stop while the
     * sound is paused stores VOLUME_PAUSED or VOLUME_MIN here unsent */
    Sint8 volume_sent;
    /* the play call's volume, or sound_set_volume's. the volume by distance
     * is this less the fall */
    Sint8 volume;
    /* the play call's volume, above which no volume by distance goes */
    Sint8 volume_top;
    Uint8 channel; /* the CHANNEL_ bits */
    Uint16 mode; /* the MODE_ bits */
    Sint16 pitch; /* the pitch the next send takes */
    Sint16 time;
    Sint16 repeat_time; /* the time of MODE_REPEAT's play */
    const void *key;
    int id; /* the sound's id, or NO_ID for a free slot */
    /* the pos that a slot under MODE_TASK and MODE_MOVING copies to pos at
     * each update, or null */
    NJS_POINT3 *follow;
    NJS_POINT3 pos;
    /* the distance at which the volume by distance has fallen by VOLUME_FALL */
    Float range;
    /* the distance from the camera at the pitch by distance's last update,
     * or 0 since the start */
    Float distance;
};

struct sound_work {
    Sint8 flags; /* the FX_ flags */
    /* the level fx_level_change last set, or 0. its one call passes it
     * fx_level, so it stays 0 */
    Sint8 fx_level;
    Sint8 held_fx_level; /* the level sound_hold_fx_level holds */
    /* the slots where the search for a note and for a sequence start */
    Sint8 next_note_slot;
    Sint8 next_sequence_slot;
    Uint8 unk_05[3];
    int character_sound; /* the id of the character's sound, or NO_ID */
    Uint8 unk_0c[4];
    struct sound_slot slots[SLOTS];
    SDMIDI ports[SDD_MIDI_PORT_NUM];
    const struct sound_list *lists[SOUND_LISTS];
};

/* the play calls the sound test makes, in test_call_names' order */
enum test_call {
    TEST_PLAY,
    TEST_PLAY_AT,
    TEST_PLAY_HELD,
    TEST_PLAY_HELD_AT,
    TEST_PLAY_FOR,
    TEST_PLAY_AT_FOR,
    TEST_PLAY_REPEATING,
    TEST_PLAY_REPEATING_AT,
    TEST_PLAY_SEQUENCE,
    TEST_START_FOR,
    TEST_CALLS
};

/* what the sound test's L and R change: the volume, the duration, or
 * nothing */
#define OPTION_VOLUME 0
#define OPTION_DURATION 1
#define OPTION_NONE 2
/* the options a call takes, which its page shows. X picks one on
 * PAGE_BOTH */
#define PAGE_VOLUME 0
#define PAGE_DURATION 1
#define PAGE_NONE 2
#define PAGE_BOTH 3

/* the pad of port D, which drives the sound test */
#define TEST_PAD 3
/* the duration Start sets back */
#define TEST_DURATION 120
/* L and R step the sound test's volume by TEST_VOLUME_STEP. L steps it while
 * it is at least TEST_VOLUME_MIN, and R while it is at most TEST_VOLUME_MAX.
 * so each can pass its bound by a step */
#define TEST_VOLUME_STEP 0.4F
#define TEST_VOLUME_MIN (-128.0F)
#define TEST_VOLUME_MAX 127.0F
/* the bound meant for R's step of the duration. R's test compares the volume
 * with it instead */
#define TEST_DURATION_TOP 1200.0F

/* the sound test's work, its task's motion */
struct sound_test {
    Sint8 call; /* an enum test_call */
    /* the call the pad's A last made, or -1. the other played_ fields keep
     * its id, duration and volume */
    Sint8 played_call;
    Sint8 page; /* the PAGE_ of call */
    Sint8 option; /* the OPTION_ that L and R change */
    Uint8 unk_04[2];
    Sint16 duration;
    Sint16 played_duration;
    Uint8 unk_0a[2];
    int id;
    int played_id;
    Bool *running; /* the flag sound_test_end clears */
    /* TRUE from the call the pad's A makes until its B stops every sound */
    Bool playing;
    Float volume; /* the volume of the calls with pos, and the sequence's */
    Float played_volume;
};

/* @hack spelling: the flush in a function SHC inlines, where the call itself
 * would do */
#pragma inline(commands_flush)
static int commands_flush(void)
{
    return sdSysHostCmdBufFlush(SDE_HOST_CMD_FLUSH_MODE_FORCE_ALL);
}

static void sound_driver_init(void);
static void download_units(const void *data, Sint32 size);
#ifdef NONMATCHING
/* this side's download_bank returns 0, or -1 where the download fails */
static int download_bank(const void *data, int bank, Sint32 size);
#else
static void download_bank(const void *data, int bank, Sint32 size);
#endif
/* free every slot, without stopping its sound */
static void slots_reset(void);
/* empty the places from SOUND_LIST_STAGE or SOUND_LIST_LOADED on, as from says.
 * any other from empties every place */
static void lists_empty(Sint8 from);
static void sound_start_over(void);
static void fx_level_change(int level);
static void fx_level_restore(void);
static void ports_set_fx_level(int level);
static void ports_clear_fx_level(void);
/* the count of the list in id's place. it returns -1 where id picks no place,
 * or the place is null */
static Sint16 id_list_count(int id);
/* the sound of id, or null where id names none */
static const struct sound *sound_of_id(int id);
/* set *bank and *number to the sound of id's, or each to -1 where id names
 * no sound. it returns 0, or -1 where id names no sound or its number is
 * below 0 */
static Sint8 sound_bank_and_number(int id, Sint8 *bank, Sint8 *number);
/* PLAYS_AS_SEQUENCE or PLAYS_AS_NOTE, as lists.h's flags say, or -1 where id
 * names no sound */
static Sint8 sound_plays_as(int id);
/* 1 for a sound with SOUND_AFTER_CLEAR, 0 for any other, and -1 where id
 * names no sound */
static Sint8 sound_plays_after_clear(int id);
/* 1 where a play call may play the sound of id, as sound.h says, else 0 */
static Sint8 sound_may_play(int id);
/* the ports' reset, as sound.h says, but for the reset of their levels */
static void ports_set_up(void);
static void ports_reset_params(void);
/* reset the ports, as sound.h says */
static void ports_reset(void);
/* the port and channel of a slot below SEQUENCE_SLOT */
static void slot_channel(Sint16 slot, Sint8 *port, Sint8 *channel);
/* send slot's channel program number of bank, as a bank select and a program
 * change. a bit of flags, CHANNEL_BANK_SET or CHANNEL_PROGRAM_SET, leaves
 * out its message */
static void channel_set_program(Sint8 slot, Sint8 bank, Sint8 number,
                                int flags);
static void channel_set_volume(Sint16 slot, Sint8 volume);
static void channel_set_pan(Sint8 slot, Sint8 pan);
static void channel_set_pitch(Sint8 slot, Sint16 pitch);
static void channel_note_on(Sint8 slot, Sint16 note, Sint8 velocity,
                            Sint8 priority);
static void channel_note_off(Sint16 slot);
/* the slot a sound of id at priority plays in, as sound.h says, or -1 */
static Sint8 slot_pick(Sint8 priority, int id);
/* slot_play, slot_stop and the slot_set_ calls act on slot now. they send to
 * the port of a sequence's slot, and to the channel of any other */
static void slot_play(int bank, int number, int priority, int slot);
static void slot_stop(Sint8 slot);
/* a note's MIDI volume falls by 1 for each step of volume below 0. it is held
 * within 0 to MIDI_MAX. a sequence's port takes volume as it is */
static void slot_set_volume(Sint8 volume, Sint8 slot);
/* a note's MIDI pan is MIDI_PAN_MIDDLE + pan * NOTE_PAN_SCALE, within 0 to
 * MIDI_MAX. a sequence's port takes pan as it is */
static void slot_set_pan(Sint8 pan, Sint8 slot);
static void slot_set_pitch(int pitch, int slot);
/* the first slot of id for key, or -1 */
static int slot_find(int id, const void *key);
/* 1 for a paused id, as sound.h names them, else 0 */
static Sint8 sound_plays_paused(int id);
static void slots_pause(void);
static void slots_resume(void);
/* 0 while the port of slot, a sequence's slot, plays, and 1 once it stops.
 * it returns -1 where SLOT_PORT(slot) names no port, or where the port's
 * state can't be read */
static Sint8 slot_sequence_ended(Sint8 slot);
/* take a slot for the sound of id, as sound.h says. give it the sound's bank,
 * number, priority and range. it returns the slot, or -1. the slot's CHANNEL_
 * bits then say what its channel holds already of the sound */
static Sint8 slot_take(int id, Sint8 priority, const NJS_POINT3 *pos);
static Bool task_can_sound(struct task *task);
/* the distance from camera's pos to pos, or -1 where the camera doesn't
 * exist. a non-null out takes the vector from the camera to pos where it
 * exists */
static Float camera_distance(const NJS_POINT3 *pos, Sint8 camera,
                             NJS_VECTOR *out);
/* the nearer to pos of cameras 0 and 1, or -1 where neither exists. a tie
 * goes to camera 0. a non-null distance takes the nearer's distance, or -1 */
static Sint8 nearer_camera(const NJS_POINT3 *pos, Float *distance);
/* the volume by distance of slot index, as sound.h says */
static Sint8 slot_volume_by_distance(Sint8 index);
/* slot index's volume_top plus a level by its distance d from camera 0, held
 * within -127 and volume_top. the level is volume_top while d is below
 * FALL_START, and -127 while d is past range. between, it is
 * 127 * (range - d) / (range - FALL_START). without camera 0, d is 0.
 * nothing reads unused */
static Sint8 slot_volume_in_range(Sint8 index, Float range, int unused);
/* the pan by angle of slot index, as sound_pan_by_pos says */
static Sint8 slot_pan_by_angle(Sint8 index);
/* the pitch by distance of slot index, as sound_pitch_by_pos says. it keeps
 * the distance in the slot for the next update */
static Sint16 slot_pitch_by_distance(Sint8 index);
/* mode, as a cleared stage changes it for slot index. sound.h's
 * sound_update says how. the call may change the slot's time too */
static int slot_mode_when_cleared(Sint8 index, int mode);
/* 1 where the sound test's call takes the volume, or the duration, else 0 */
static Sint8 test_call_takes_volume(struct task *task);
static Sint8 test_call_takes_duration(struct task *task);

/* no code of the game's images calls these, and no header declares them.
 * they are external because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* sound_load_bank, with no test of the read: it returns 0 */
int sound_load_bank_untested(int bank, const char *name);
/* send the file at data to sound memory as sound_load_file does, but with a
 * size of 0. it returns 0 */
int sound_download(const void *data);
/* turn game_state.mono over, and set the driver to it as sound_apply_mono
 * does */
void sound_toggle_mono(void);
/* these do nothing */
void fun_8c059fae(void);
void fun_8c059fb6(void);
void fun_8c059fba(void);
/* the count of place's list, or -1 where the place is null. place must be 0
 * to 7, unchecked */
int sound_list_count(Sint8 place);
/* task calls as sound.h describes them. sound_play_task_held holds its sound
 * until a stop call. the _repeating calls repeat their sound as
 * sound_play_repeating does. sound_play_task_repeating leaves a found slot's
 * time as it is, where sound.h's task calls give it its time again */
Sint8 sound_play_task_held(int id, struct task *task, Sint8 priority,
                           Sint16 volume);
Sint8 sound_play_task_repeating(int id, struct task *task, Sint8 priority,
                                Sint16 volume, Sint16 duration);
Sint8 sound_play_task_repeating_at(int id, struct task *task, Sint8 priority,
                                   Sint16 volume, Sint16 duration);
/* where pos isn't null, move each slot of id for key to pos. then set its
 * volume as slot_volume_in_range gives it for range, and end its fall with
 * distance */
void sound_set_volume_in_range(int id, const void *key, const NJS_POINT3 *pos,
                               Float range);
/* the sound test's step, by TEST_PAD's buttons. from A until B, each step
 * makes A's call again where that call takes a duration. the buttons:
 * - up and down step id to the first sound of the next or the previous place
 *   that holds a list. where no place holds one, they never return
 * - right and left step id through its place's list
 * - Y steps the call, and A makes it. a call with pos plays at player 0's,
 *   whose work must exist then, unchecked
 * - L and R step the option's value. Start sets the volume back to 0, or the
 *   duration to TEST_DURATION
 * - B stops every sound, and X with B kills the task */
void sound_test_step(struct task *task);
/* print the sound test's call, id, bank and number, and its option */
void sound_test_draw(struct task *task);
void sound_test_end(struct task *task);
/* send volume to channel of a sequence's slot. it returns sdMidiSendMes's
 * result, or -1 where slot isn't a sequence's or channel isn't one */
int sequence_set_channel_volume(Sint16 slot, Sint8 channel, Sint8 volume);
/* NOLINTEND(misc-use-internal-linkage) */

/* null until sound_system_init allocates it */
static struct sound_work *sound_work = NULL;
/* sound_play_ring's pans: row RING_ALONE in a one-player game, and row
 * RING_PLAYER + player in a two-player one. column RING_ODD is for an odd
 * count of rings, and RING_EVEN for an even one */
#define RING_ALONE 0
#define RING_PLAYER 1
#define RING_ODD 0
#define RING_EVEN 1
/* clang-format off */
static Sint8 ring_pans[3][2] = {
    {-64,  64},
    {-64, -64},
    { 64,  64},
};
/* clang-format on */
static Sint32 dat_8c0e8164[6] = {0};
static const char *test_call_names[TEST_CALLS] = {
#include "sound/sound/test_call_names.inc"
};

void sound_system_init(void)
{
    game_state.sound_paused = 0;
    sound_driver_init();
    sound_apply_mono();
}

void sound_load_stage(const char *name, const struct sound_list *list)
{
    game_state.sound_paused = 0;
    sdSndClearFxPrg();
    commands_flush();
    lists_empty(SOUND_LIST_STAGE);
    slots_reset();
    if (name != NULL && sound_load_file(name) == 0) {
        /* @hack spelling: the address of the first place, where the table
         * would do */
        sound_lists_set_4_and_7(&sound_work->lists[0]);
        sound_work->lists[SOUND_LIST_LOADED] = list;
    }
    sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
    commands_flush();
}

void sound_load_stage_and_voices(const char *name,
                                 const struct sound_list *list, Sint8 players)
{
    Sint8 character;

    game_state.sound_paused = 0;
    sdSndClearFxPrg();
    commands_flush();
    lists_empty(SOUND_LIST_STAGE);
    slots_reset();
    if (name != NULL && sound_load_file(name) == 0) {
        /* @hack spelling: the address of the first place, where the table
         * would do */
        sound_lists_set_4_and_7(&sound_work->lists[0]);
        sound_work->lists[SOUND_LIST_LOADED] = list;
    }
    character = stage_character(0);
    if (character >= 0) {
        sound_load_voices(players, character,
                          &sound_work->lists[SOUND_LIST_VOICES]);
        sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
        commands_flush();
        /* @hack spelling: the two calls joined by a comma, where two
         * statements would do */
        ports_reset_params(), ports_set_up();
    }
}

void sound_load_event(const char *name, const struct sound_list *list)
{
    sound_stop_all();
    game_state.sound_paused = 0;
    sound_release_fx_level();
    sdSndClearFxPrg();
    commands_flush();
    lists_empty(SOUND_LIST_STAGE);
    slots_reset();
    if (name != NULL) {
        if (strncmp(name, EVENT_PREFIX, 1) == 0 &&
            sound_load_file(EVENT_FILE) == 0) {
#ifdef NONMATCHING
            /* the hack's plain form: sound_list_set by its own name */
            sound_list_set(sound_work->lists, 0);
#else
            /* @hack type: the call through sound_list_set_without_unk, where
             * sound_list_set would do, declared without the unk lists.c
             * keeps */
            sound_list_set_without_unk(sound_work->lists, 0);
#endif
        }
        if (sound_load_file(name) == 0) {
            sound_work->lists[SOUND_LIST_LOADED] = list;
        }
    }
    sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
    commands_flush();
    /* @hack spelling: the two calls joined by a comma, where two statements
     * would do */
    ports_reset_params(), ports_set_up();
}

void sound_load_cart(const char *name, const struct sound_list *list)
{
    sound_stop_all();
    sound_release_fx_level();
    sound_fx_off();
    sound_start_over();
    game_state.sound_paused = 0;
    if (name != NULL && sound_load_file(name) == 0) {
        sound_work->lists[SOUND_LIST_LOADED] = list;
        sound_lists_set_0_and_1(sound_work->lists);
        sound_lists_set_4_and_7(sound_work->lists);
    }
    /* @hack spelling: the two calls joined by a comma, where two statements
     * would do */
    ports_reset(), sound_fx_on();
}

void sound_set_list(const struct sound_list *list, Sint8 place)
{
    /* @hack redundant: the store of null, which the next store overwrites */
    sound_work->lists[place] = NULL;
    sound_work->lists[place] = list;
}

#ifdef NONMATCHING
/* download size bytes at data to the sound library: the driver where type is
 * SDE_DATA_TYPE_DRV, a multi-unit file where it is SDE_DATA_TYPE_MULTI_UNIT,
 * and otherwise a bank of type into bank. the transfer is synchronous, so
 * data is the caller's again on return. the memory block it creates is
 * destroyed before return, whether the transfer worked or not. it returns the
 * library's first error, or SDE_ERR_NOTHING */
static SDE_ERR transfer_sound_file(const void *data, Sint32 size,
                                   SDE_DATA_TYPE type, int bank)
{
    SDMEMBLK block = NULL;
    SDE_ERR result = sdMemBlkCreate(&block);

    if (result == SDE_ERR_NOTHING) {
        result = sdMemBlkSetPrm(block, data, size, SDD_MEMBLK_SYNC_FUNC, NULL);
        if (result == SDE_ERR_NOTHING) {
            if (type == SDE_DATA_TYPE_DRV) {
                result = sdDrvInit(block);
            } else if (type == SDE_DATA_TYPE_MULTI_UNIT) {
                result = sdMultiUnitDownload(block);
            } else {
                result = sdBankDownload(block, type, bank);
            }
        }
    }
    if (block != NULL) {
        sdMemBlkDestroy(block);
    }
    return result;
}

/* read the driver, and start the sound library with it. the game can't go on
 * without the driver: a read that fails stops it, as loader_stop says. so
 * does an empty file, as EMPTY, and a heap with no room for sound_work, as
 * HEAP. a library that refuses the driver stops it as SOUND DRIVER, with the
 * library's error */
static void start_sound_driver(void)
{
    Sint32 size = loader_read_file_checked(DRIVER_FILE, WORK_AREA,
                                           WORK_AREA_END - WORK_AREA);
    SDE_ERR error;

    if (size < 0) {
        loader_stop(DRIVER_FILE);
    }
    if (size == 0) {
        port_stop("EMPTY", DRIVER_FILE, 0);
    }
    sound_work = syCalloc(1, sizeof(struct sound_work));
    if (sound_work == NULL) {
        port_stop("HEAP", DRIVER_FILE, 0);
    }
    error = sdLibInit(NULL, 0, 0);
    if (error == SDE_ERR_NOTHING) {
        error = sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE_DMA);
    }
    if (error == SDE_ERR_NOTHING) {
        error = transfer_sound_file(WORK_AREA, size, SDE_DATA_TYPE_DRV, 0);
    }
    if (error != SDE_ERR_NOTHING) {
        port_stop("SOUND DRIVER", DRIVER_FILE, (Uint32)error);
    }
}
#endif

static void sound_driver_init(void)
{
    int i;
#ifdef NONMATCHING
    /* this side checks each step of the driver's start, and stops the game
     * where one fails */
    start_sound_driver();
#else
    SDMEMBLK block = NULL;
    Sint32 size;

    sound_work = syCalloc(1, sizeof(struct sound_work));
    if (sound_work == NULL) {
        return;
    }
    if (sdLibInit(NULL, 0, 0) != SDE_ERR_NOTHING) {
        return;
    }
    size = loader_read_file_get_size(DRIVER_FILE, WORK_AREA);
    sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE_DMA);
    /* @hack spelling: the nested tests, where one test joined by && would
     * do */
    if (sdMemBlkCreate(&block) == SDE_ERR_NOTHING) {
        if (sdMemBlkSetPrm(block, WORK_AREA, size, SDD_MEMBLK_SYNC_FUNC,
                           NULL) == SDE_ERR_NOTHING) {
            sdDrvInit(block);
        }
    }
    if (block != NULL) {
        sdMemBlkDestroy(block);
    }
#endif
    sdSysSetSlotMax(DRIVER_MIDI_SLOTS, DRIVER_SHOT_SLOTS, DRIVER_STREAM_SLOTS);
    for (i = 0; i < SDD_MIDI_PORT_NUM; i++) {
        sdMidiOpenPort(&sound_work->ports[i]);
    }
    slots_reset();
    sound_work->character_sound = NO_ID;
    lists_empty(SOUND_LISTS);
    sound_load_character(PLAYER_SONIC, &sound_work->lists[SOUND_LIST_CHARACTER],
                         &sound_work->lists[SOUND_LIST_VOICES]);
    sound_lists_set_0_and_1(sound_work->lists);
    ports_reset_params();
    ports_set_up();
    sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
    commands_flush();
}

int sound_fx_off(void)
{
    sdSndClearFxPrg();
    commands_flush();
    return 0;
}

int sound_fx_on(void)
{
    sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
    commands_flush();
    return 0;
}

int sound_load_file(const char *name)
{
    Sint32 size = loader_read_prs_or_file(name);

#ifdef NONMATCHING
    /* this side returns -1 where the read or the download fails, and stops
     * every sound before it downloads */
    if (size <= 0) {
        return -1;
    }
    sound_stop_all();
    if (transfer_sound_file(WORK_AREA, size, SDE_DATA_TYPE_MULTI_UNIT, 0) !=
        SDE_ERR_NOTHING) {
        return -1;
    }
    return 0;
#else
    if (size == 0) {
        return -1;
    }
    download_units(WORK_AREA, size);
    return 0;
#endif
}

int sound_load_bank_untested(int bank, const char *name)
{
#ifdef NONMATCHING
    /* this side tests the load, as sound_load_bank does */
    return sound_load_bank(bank, name);
#else
    download_bank(WORK_AREA, bank, loader_read_file_get_size(name, WORK_AREA));
    return 0;
#endif
}

int sound_load_bank(int bank, const char *name)
{
#ifdef NONMATCHING
    /* this side reads at most WORK_AREA's bytes, and returns -1 for a file
     * shorter than its kind word or a download that fails */
    Sint32 size =
        loader_read_file_checked(name, WORK_AREA, WORK_AREA_END - WORK_AREA);

    if (size < (Sint32)sizeof(Uint32)) {
        return -1;
    }
    return download_bank(WORK_AREA, bank, size);
#else
    Sint32 size = loader_read_file_get_size(name, WORK_AREA);

    if (size == 0) {
        return -1;
    }
    download_bank(WORK_AREA, bank, size);
    return 0;
#endif
}

int sound_download(const void *data)
{
    download_units(data, 0);
    return 0;
}

static void download_units(const void *data, Sint32 size)
{
    SDMEMBLK block = NULL;

    sound_stop_all();
    if (sdMemBlkCreate(&block) == SDE_ERR_NOTHING &&
        sdMemBlkSetPrm(block, data, size, SDD_MEMBLK_SYNC_FUNC, NULL) ==
            SDE_ERR_NOTHING) {
        sdMultiUnitDownload(block);
    }
    if (block != NULL) {
        sdMemBlkDestroy(block);
    }
}

#ifdef NONMATCHING
/* this side returns 0, or -1 where the download fails, for sound_load_bank to
 * pass on */
static int download_bank(const void *data, int bank, Sint32 size)
{
    Uint32 kind;

    memcpy(&kind, data, sizeof(kind));
    if (kind != SDE_DATA_TYPE_MIDI_DRM_BANK &&
        kind != SDE_DATA_TYPE_MIDI_SEQ_BANK &&
        kind != SDE_DATA_TYPE_SHOT_BANK) {
        kind = SDE_DATA_TYPE_MIDI_PRG_BANK;
    }
    if (transfer_sound_file(data, size, (SDE_DATA_TYPE)kind, bank) !=
        SDE_ERR_NOTHING) {
        return -1;
    }
    return 0;
}
#else
static void download_bank(const void *data, int bank, Sint32 size)
{
    SDMEMBLK block = NULL;
    Uint32 kind;
    SDE_DATA_TYPE type;

    /* @hack spelling: kind, a local for the file's first word, where the
     * word would do */
    kind = *(const Uint32 *)data;
    switch (kind) {
    case SDE_DATA_TYPE_MIDI_DRM_BANK:
        type = SDE_DATA_TYPE_MIDI_DRM_BANK;
        break;
    case SDE_DATA_TYPE_MIDI_SEQ_BANK:
        type = SDE_DATA_TYPE_MIDI_SEQ_BANK;
        break;
    case SDE_DATA_TYPE_MIDI_PRG_BANK:
        type = SDE_DATA_TYPE_MIDI_PRG_BANK;
        break;
    case SDE_DATA_TYPE_SHOT_BANK:
        type = SDE_DATA_TYPE_SHOT_BANK;
        break;
    default:
        type = SDE_DATA_TYPE_MIDI_PRG_BANK;
        break;
    }
    if (sdMemBlkCreate(&block) == SDE_ERR_NOTHING &&
        sdMemBlkSetPrm(block, data, size, SDD_MEMBLK_SYNC_FUNC, NULL) ==
            SDE_ERR_NOTHING) {
        sdBankDownload(block, type, bank);
    }
    if (block != NULL) {
        sdMemBlkDestroy(block);
    }
}
#endif

static void slots_reset(void)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;

    for (i = 0; i < SLOTS; i++, slot++) {
        slot->priority = -1;
        slot->bank = -1;
        slot->number = -1;
        slot->pan = 0;
        slot->volume_sent = 0;
        slot->volume = 0;
        slot->volume_top = 0;
        slot->pitch = 0;
        slot->time = 0;
        slot->channel = 0;
        slot->key = NULL;
        slot->mode = 0;
        slot->id = NO_ID;
        slot->pos.x = 0.0F;
        slot->pos.y = 0.0F;
        slot->pos.z = 0.0F;
        slot->follow = NULL;
        slot->distance = 0.0F;
        slot->range = 0.0F;
    }
}

static void lists_empty(Sint8 from)
{
    const struct sound_list **list;
    Sint8 i;
    Sint8 first;
    Sint8 end;

    switch (from) {
    /* @hack redundant: the case of SOUND_LISTS, whose places the default
     * empties too */
    case SOUND_LISTS:
        first = 0;
        end = SOUND_LISTS;
        break;
    case SOUND_LIST_STAGE:
        first = SOUND_LIST_STAGE;
        end = SOUND_LISTS;
        break;
    case SOUND_LIST_LOADED:
        first = SOUND_LIST_LOADED;
        end = SOUND_LISTS;
        break;
    default:
        first = 0;
        end = SOUND_LISTS;
        break;
    }
    /* @hack spelling: the walk of a pointer, where an index into the places
     * would do */
    list = &sound_work->lists[first];
    for (i = first; i < end; i++, list++) {
        *list = NULL;
    }
}

static void sound_start_over(void)
{
    slots_reset();
    sound_work->character_sound = NO_ID;
    lists_empty(SOUND_LISTS);
}

void sound_hold_fx_level(int level)
{
    sound_work->flags |= FX_HELD;
    sound_work->flags &= ~FX_PAUSED;
    sound_work->held_fx_level = level;
    ports_set_fx_level(level);
}

void sound_release_fx_level(void)
{
    if (sound_work->flags & FX_HELD) {
        sound_work->fx_level = 0;
        sound_work->held_fx_level = 0;
        ports_clear_fx_level();
        sound_work->flags &= ~FX_HELD;
        sound_work->flags &= ~FX_PAUSED;
    }
}

static void fx_level_change(int level)
{
    if (sound_work->flags & FX_HELD) {
        sound_work->fx_level = level;
        ports_set_fx_level(sound_work->fx_level);
    }
}

static void fx_level_restore(void)
{
    if (sound_work->flags & FX_HELD) {
        ports_set_fx_level(sound_work->held_fx_level);
    }
}

static void ports_set_fx_level(int level)
{
    Sint8 i;

    for (i = 0; i < SDD_MIDI_PORT_NUM; i++) {
        sdMidiSetFxLev(sound_work->ports[i], level);
    }
}

static void ports_clear_fx_level(void)
{
    Sint8 i;

    for (i = 0; i < SDD_MIDI_PORT_NUM; i++) {
        sdMidiSetFxLev(sound_work->ports[i], 0);
    }
}

void sound_pause_all(void)
{
    game_state.sound_paused = 1;
    sound_pause();
    slots_pause();
}

void sound_resume_all(void)
{
    game_state.sound_paused = 0;
    sound_resume();
    slots_resume();
}

void sound_stop_all_and_unpause(void)
{
    sound_stop_all();
    game_state.sound_paused = 0;
}

void sound_toggle_mono(void)
{
    if (game_state.mono == 0) {
        sdSndSetPanMode(SDE_PAN_MODE_MONO);
        game_state.mono = 1;
        sdSndSetMasterVol(MONO_VOLUME);
    } else {
        sdSndSetPanMode(SDE_PAN_MODE_STEREO);
        game_state.mono = 0;
        sdSndSetMasterVol(STEREO_VOLUME);
    }
}

void sound_apply_mono(void)
{
    if (game_state.mono == 1) {
        sdSndSetPanMode(SDE_PAN_MODE_MONO);
        sdSndSetMasterVol(MONO_VOLUME);
    } else {
        sdSndSetPanMode(SDE_PAN_MODE_STEREO);
        sdSndSetMasterVol(STEREO_VOLUME);
    }
}

void fun_8c059fae(void)
{
}

void fun_8c059fb2(void)
{
}

void fun_8c059fb6(void)
{
}

void fun_8c059fba(void)
{
}

int sound_list_count(Sint8 place)
{
    const struct sound_list *list = sound_work->lists[place];

    if (list == NULL) {
        return -1;
    }
    return list->count;
}

static Sint16 id_list_count(int id)
{
    int bits;
    Sint8 place;
    const struct sound_list *list;

    if (id < 0) {
        return -1;
    }
    bits = id & ID_PLACE_MASK;
    switch (bits) {
    case ID_PLACE(8):
        place = 0;
        break;
    case ID_PLACE(1):
        place = 1;
        break;
    case ID_PLACE(2):
        place = 2;
        break;
    case ID_PLACE(3):
        place = 3;
        break;
    case ID_PLACE(4):
        place = 4;
        break;
    case ID_PLACE(6):
        place = 6;
        break;
    case ID_PLACE(7):
        place = 7;
        break;
    case ID_PLACE(0):
        place = 5;
        break;
    default:
        return -1;
    }
    list = sound_work->lists[place];
    if (list == NULL) {
        return -1;
    }
    return list->count;
}

static const struct sound *sound_of_id(int id)
{
    int bits;
    Sint8 place;
    Uint8 index;
    const struct sound_list *list;
    const struct sound *sound;

    if (id < 0) {
        return NULL;
    }
    bits = id & ID_PLACE_MASK;
    switch (bits) {
    case ID_PLACE(8):
        place = 0;
        break;
    case ID_PLACE(1):
        place = 1;
        break;
    case ID_PLACE(2):
        place = 2;
        break;
    case ID_PLACE(3):
        place = 3;
        break;
    case ID_PLACE(4):
        place = 4;
        break;
    case ID_PLACE(6):
        place = 6;
        break;
    case ID_PLACE(7):
        place = 7;
        break;
    case ID_PLACE(0):
        place = 5;
        break;
    default:
        return NULL;
    }
    index = id;
    /* @hack spelling: the place read for its test and again for list, where
     * one read into list would do */
    if (sound_work->lists[place] == NULL) {
        return NULL;
    }
    list = sound_work->lists[place];
    if (index >= list->count || list->sounds == NULL) {
        return NULL;
    }
    sound = &list->sounds[index];
    /* @hack redundant: the test of the sound's address, which is never
     * null */
    if (sound == NULL) {
        return NULL;
    }
    return sound;
}

static Sint8 sound_bank_and_number(int id, Sint8 *bank, Sint8 *number)
{
    const struct sound *sound = sound_of_id(id);

    if (sound == NULL) {
        *bank = -1;
        *number = -1;
        return -1;
    }
    *bank = sound->bank;
    *number = sound->number;
    if (*number < 0) {
        return -1;
    }
    return 0;
}

static Sint8 sound_plays_as(int id)
{
    const struct sound *sound = sound_of_id(id);
    Sint8 flags;

    if (sound == NULL) {
        return -1;
    }
    /* @hack spelling: flags, a local for the sound's flags, where the
     * sound's would do */
    flags = sound->flags;
    if (flags & SOUND_SEQUENCE) {
        return PLAYS_AS_SEQUENCE;
    } else if (flags & SOUND_NOTE) {
        return PLAYS_AS_NOTE;
    }
    if (sound->bank == SEQUENCE_BANK && stage_current_id() < STAGE_CHAO) {
        return PLAYS_AS_SEQUENCE;
    }
    if (sound->number >= SEQUENCE_NUMBER) {
        return PLAYS_AS_SEQUENCE;
    }
    return PLAYS_AS_NOTE;
}

static Sint8 sound_plays_after_clear(int id)
{
    const struct sound *sound = sound_of_id(id);
    Sint8 flags;

    if (sound == NULL) {
        return -1;
    }
    /* @hack spelling: flags, a local for the sound's flags, where the
     * sound's would do */
    flags = sound->flags;
    if (flags & SOUND_AFTER_CLEAR) {
        return 1;
    }
    return 0;
}

/* @hack type: the result a Sint8, where a Bool would do */
static Sint8 sound_may_play(int id)
{
    if (game_state.sound_paused == 1) {
        if (sound_plays_paused(id) == 1) {
            return 1;
        }
        return 0;
    } else if (game_state.cleared == 1) {
        if (sound_plays_after_clear(id) == 1) {
            return 1;
        }
        return 0;
    }
    if (sound_of_id(id) == NULL) {
        return 0;
    }
    return 1;
}

static void ports_set_up(void)
{
    Sint8 i;
    /* @hack type: found a Sint8, where a Bool would do */
    Sint8 found;
    Uint32 start;
    Uint32 elapsed;
    SDS_MIDI_STAT stat;

    for (i = RESET_PORTS; i > 0; i--) {
        sdMidiPlay(sound_work->ports[i - 1], RESET_BANK, RESET_SEQUENCE, 0);
    }
    sdSysFlushHostCmd();
    elapsed = 0;
    start = syTmrGetCount();
    sdMidiGetStat(sound_work->ports[CHARACTER_PORT], &stat);
    /* @bug logic: the wait for a port that plays runs all of RESET_WAIT,
     * however soon the port stops. the wait tests the flags of the one
     * sdMidiGetStat call before it, JSR @R3 at 0x8c05a360. MOV.L @(28,R15),R0
     * at 0x8c05a37e reads them again on each pass, but nothing reads the port
     * anew */
    while (stat.m_Flg == SDD_PORT_FLG_PLAY && elapsed < RESET_WAIT) {
        /* @hack spelling: the time worked out in three statements, where one
         * expression would do */
        elapsed = syTmrGetCount();
        elapsed = syTmrDiffCount(start, elapsed);
        elapsed = syTmrCountToMicro(elapsed);
    }
    if (stage_current_id() >= STAGE_CART_1 ||
        stage_current_id() == STAGE_GREEN_HILL) {
        return;
    }
    found = 0;
    switch (stage_character(0)) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
        sound_work->character_sound = SOUND_RING_ID;
        found = 1;
        break;
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        sound_work->character_sound = SOUND_KNUCKLES_ID;
        found = 1;
        break;
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        sound_work->character_sound = SOUND_MECH_ID;
        found = 1;
        break;
    }
    if (found == 1) {
        sdMidiPlay(sound_work->ports[CHARACTER_PORT], CHARACTER_BANK,
                   RESET_SEQUENCE, 0);
    } else {
        sound_work->character_sound = NO_ID;
    }
}

static void ports_reset_params(void)
{
    sdMidiResetAllPrm();
}

static void ports_reset(void)
{
    sdMidiResetAllPrm();
    ports_set_up();
}

static void slot_channel(Sint16 slot, Sint8 *port, Sint8 *channel)
{
    int port_number;

    if (slot > NOTE_SLOTS - 1) {
        port_number = CHARACTER_PORT;
    } else if (slot > SDD_MIDI_CH_MAX) {
        port_number = NOTE_PORT + 1;
    } else {
        port_number = NOTE_PORT;
    }
    /* @hack spelling: slot masked to its channel in place, where a store of
     * the masked slot would do */
    slot &= SDD_MIDI_CH_MAX;
    *port = port_number;
    *channel = slot;
}

static void channel_set_program(Sint8 slot, Sint8 bank, Sint8 number, int flags)
{
    Sint8 port;
    Sint8 channel;
    SDS_MIDI_MES mes;

    slot_channel(slot, &port, &channel);
    /* @hack spelling: slot given the channel, where channel itself would
     * do */
    slot = channel;
    if (!(flags & CHANNEL_BANK_SET)) {
        sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_CTL_CHG,
                     SDD_MIDI_CTL_CHG_MSB + SDD_MIDI_CTL_CHG_BANK, bank);
        sdMidiSendMes(sound_work->ports[port], &mes, 0);
    }
    if (!(flags & CHANNEL_PROGRAM_SET)) {
        sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_PRG_CHG,
                     number);
        sdMidiSendMes(sound_work->ports[port], &mes, 0);
    }
}

static void channel_set_volume(Sint16 slot, Sint8 volume)
{
    Sint8 port;
    Sint8 channel;
    SDS_MIDI_MES mes;

    slot_channel(slot, &port, &channel);
    /* @hack spelling: slot given the channel, where channel itself would
     * do */
    slot = channel;
    sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_CTL_CHG,
                 SDD_MIDI_CTL_CHG_VOL, volume);
    sdMidiSendMes(sound_work->ports[port], &mes, 0);
}

static void channel_set_pan(Sint8 slot, Sint8 pan)
{
    Sint8 port;
    Sint8 channel;
    SDS_MIDI_MES mes;

    slot_channel(slot, &port, &channel);
    /* @hack spelling: slot given the channel, where channel itself would
     * do */
    slot = channel;
    sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_CTL_CHG,
                 SDD_MIDI_CTL_CHG_PAN, pan);
    sdMidiSendMes(sound_work->ports[port], &mes, 0);
}

static void channel_set_pitch(Sint8 slot, Sint16 pitch)
{
    Sint8 port;
    Sint8 channel;
    SDS_MIDI_MES mes;

    slot_channel(slot, &port, &channel);
    /* @hack spelling: slot given the channel, where channel itself would
     * do */
    slot = channel;
    pitch += BEND_MIDDLE;
    /* @bug logic: the bend's second byte takes bits 0 to 6 of the pitch, and
     * its third bits 8 to 14, so bit 7 reaches neither. AND #127,R0 at
     * 0x8c05a6b8 and SHAD R2,R0 at 0x8c05a6b0 cut the two */
    sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_PITCH_BEND,
                 pitch & MIDI_DATA_MASK, pitch >> BEND_HIGH_SHIFT);
    sdMidiSendMes(sound_work->ports[port], &mes, 0);
}

static void channel_note_on(Sint8 slot, Sint16 note, Sint8 velocity,
                            Sint8 priority)
{
    Sint8 port;
    Sint8 channel;
    SDS_MIDI_MES mes;

    slot_channel(slot, &port, &channel);
    /* @hack spelling: slot given the channel, where channel itself would
     * do */
    slot = channel;
    sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_NOTE_ON,
                 note, velocity);
    sdMidiSendMes(sound_work->ports[port], &mes, priority);
}

static void channel_note_off(Sint16 slot)
{
    Sint8 port;
    Sint8 channel;
    SDS_MIDI_MES mes;

    slot_channel(slot, &port, &channel);
    /* @hack spelling: slot given the channel, where channel itself would
     * do */
    slot = channel;
    sdMidiSetMes(sound_work->ports[port], &mes, slot | SDD_MIDI_MES_NOTE_OFF,
                 NOTE, 0);
    sdMidiSendMes(sound_work->ports[port], &mes, 0);
}

static Sint8 slot_pick(Sint8 priority, int id)
{
    struct sound_slot *slot;
    Sint16 found;
    Sint16 i;
    Sint16 left;
    Sint8 same;
    Sint8 lower;
    /* @bug arithmetic: time keeps a slot's time in a Sint8, so a time
     * outside -128 to 127 compares as its low byte. EXTS.B R12,R3 at
     * 0x8c05a932 cuts the time the character's slots compare, and MOV.B
     * @R15,R1 at 0x8c05a896 the one the sequences' compare */
    Sint8 time;
    Sint8 kind;
    Sint8 bank;
    Sint8 number;

    /* @hack redundant: the call of sound_bank_and_number, whose bank and
     * number nothing reads */
    sound_bank_and_number(id, &bank, &number);
    kind = sound_plays_as(id);
    found = -1;
    if (kind == PLAYS_AS_SEQUENCE) {
        i = sound_work->next_sequence_slot;
        slot = &sound_work->slots[i];
        /* @hack spelling: left set ahead of same and lower, where any order
         * would do */
        left = SLOTS;
        same = -1;
        lower = -1;
        /* @bug uninitialized: where a slot below priority comes before the
         * first slot at priority, the test time >= slot->time at that slot
         * reads time unset. MOV.B @R15,R1 at 0x8c05a896 reads it from the
         * frame's byte where MOV.B R4,@R15 at 0x8c05a808 stored priority */
#ifdef NONMATCHING
        /* this side keeps the tag's result: time starts as priority, the
         * byte the image's frame holds where it reads time */
        time = priority;
#endif
        while (left > SEQUENCE_SLOT) {
            if (slot->id < 0) {
                found = i;
                break;
            }
            if (slot->priority < priority) {
                lower = i;
            } else if (slot->priority == priority) {
                if (lower < 0) {
                    lower = i;
                    time = slot->time;
                } else if (time >= slot->time) {
                    same = i;
                    time = slot->time;
                }
            }
            i++;
            left--;
            slot++;
            if (i >= SLOTS) {
                i = SEQUENCE_SLOT;
                slot = &sound_work->slots[SEQUENCE_SLOT];
            }
        }
        if (found < 0) {
            if (same > 0) {
                found = same;
            } else if (lower > 0) {
                found = lower;
            } else {
                return -1;
            }
        }
        sound_work->next_sequence_slot = found + 1;
        if (sound_work->next_sequence_slot >= SLOTS) {
            sound_work->next_sequence_slot = SEQUENCE_SLOT;
        }
        return found;
    } else if (id == sound_work->character_sound) {
        i = CHARACTER_SLOT;
        /* here lower is the slot of least time so far. the sequences' search
         * keeps a slot below priority in it */
        lower = CHARACTER_SLOT;
        slot = &sound_work->slots[CHARACTER_SLOT];
        time = slot->time;
        left = SEQUENCE_SLOT;
        /* @hack spelling: the scan a do-while, where a while would do */
        do {
            if (i == SKIPPED_SLOT) {
                i++;
                if (i >= SEQUENCE_SLOT) {
                    i = 0;
                }
            } else {
                if (slot->time == 0) {
                    found = i;
                } else if (slot->time < time) {
                    lower = i;
                    time = slot->time;
                }
                i++;
                if (i >= SEQUENCE_SLOT) {
                    i = 0;
                }
            }
            left--;
            slot++;
        } while (found < 0 && left > CHARACTER_SLOT);
        if (found < 0) {
            found = lower;
        }
    } else {
        /* @hack spelling: same set ahead of i and slot, where any order
         * would do */
        same = -1;
        i = sound_work->next_note_slot;
        slot = &sound_work->slots[i];
        left = NOTE_SLOTS;
        while (found < 0 && left > 0) {
            if (slot->id < 0 || slot->priority < priority) {
                found = i;
            } else if (slot->priority == priority) {
                same = i;
            }
            i++;
            left--;
            slot++;
            if (i >= NOTE_SLOTS) {
                i = 0;
                slot = sound_work->slots;
            }
        }
        if (found < 0) {
            found = same;
        }
        sound_work->next_note_slot = found + 1;
        if (sound_work->next_note_slot >= NOTE_SLOTS) {
            sound_work->next_note_slot = 0;
        }
    }
    if (found >= 0) {
        slot = &sound_work->slots[found];
        if (sound_plays_paused(slot->id) == 0 && sound_plays_paused(id) == 1) {
            slot_stop(found);
            if (game_state.sound_paused == 1) {
                slot->volume_sent = VOLUME_MIN;
            }
        }
    }
    return found;
}

static void slot_play(int bank, int number, int priority, int slot)
{
    Sint8 level;
    /* @hack redundant: result, which keeps sdMidiPlay's result that nothing
     * reads */
    SDE_ERR result; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if (priority == PRIORITY_TOP) {
        level = PORT_PRIORITY_HIGH;
    } else {
        level = PORT_PRIORITY_LOW;
    }
    if (slot >= SEQUENCE_SLOT) {
        /* @hack spelling: the port as slot less the difference of
         * SEQUENCE_SLOT and SEQUENCE_PORT, where SLOT_PORT(slot) would do */
        result = sdMidiPlay(
            sound_work->ports[slot - (SEQUENCE_SLOT - SEQUENCE_PORT)], bank,
            number, level);
    } else {
        channel_note_on(slot, NOTE, VELOCITY, level);
    }
}

static void slot_stop(Sint8 slot)
{
    if (slot >= SEQUENCE_SLOT) {
        sdMidiStop(sound_work->ports[SLOT_PORT(slot)]);
    } else {
        channel_note_off(slot);
    }
}

static void slot_set_volume(Sint8 volume, Sint8 slot)
{
    if (slot >= SEQUENCE_SLOT) {
        sdMidiSetVol(sound_work->ports[SLOT_PORT(slot)], volume, 0);
    } else {
        if (volume > 0) {
            volume = volume * NOTE_VOLUME_RISE;
        }
        volume += NOTE_VOLUME;
        if (volume < 0) {
            volume = 0;
        }
        if (volume > MIDI_MAX) {
            volume = MIDI_MAX;
        }
        channel_set_volume(slot, volume);
    }
}

static void slot_set_pan(Sint8 pan, Sint8 slot)
{
    Sint8 value;
    /* @hack redundant: result, which keeps sdMidiSetPan's result that
     * nothing reads */
    SDE_ERR result; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    if (slot >= SEQUENCE_SLOT) {
        result = sdMidiSetPan(sound_work->ports[SLOT_PORT(slot)], pan, 0);
    } else {
        value = (pan * NOTE_PAN_SCALE) + MIDI_PAN_MIDDLE;
        if (value > MIDI_MAX) {
            value = MIDI_MAX;
        } else if (value < 0) {
            value = 0;
        }
        channel_set_pan(slot, value);
    }
}

static void slot_set_pitch(int pitch, int slot)
{
    if (pitch > PITCH_MAX) {
        pitch = PITCH_MAX;
    } else if (pitch < PITCH_MIN) {
        pitch = PITCH_MIN;
    }
    if (slot >= SEQUENCE_SLOT) {
        sdMidiSetPitch(sound_work->ports[SLOT_PORT(slot)], pitch, 0);
    } else {
        channel_set_pitch(slot, pitch);
    }
}

static int slot_find(int id, const void *key)
{
    struct sound_slot *slot = sound_work->slots;
    int i;
    /* @hack spelling: found, a local for the result, where a return from
     * the loop would do */
    int found = -1;

    for (i = 0; i < SLOTS; i++, slot++) {
        if (slot->key == key && slot->id == id) {
            found = i;
            break;
        }
    }
    return found;
}

/* @hack type: the result a Sint8, where a Bool would do */
static Sint8 sound_plays_paused(int id)
{
    if (id == SOUND_CURSOR_ID || id == SOUND_CHOOSE_ID ||
        id == SOUND_PAUSE_ID) {
        return 1;
    }
    return 0;
}

static void slots_pause(void)
{
    struct sound_slot *slot;
    Sint8 i;
    Sint8 j;

    if (sound_work->flags & FX_HELD && sound_work->flags & FX_PAUSED) {
        fx_level_restore();
    }
    slot = sound_work->slots;
    /* @hack spelling: the step written slot = slot + 1, where slot++ would
     * do */
    for (i = 0; i < SEQUENCE_SLOT; slot = slot + 1, i++) {
        if (slot->id < 0) {
            continue;
        }
        if (sound_plays_paused(slot->id) == 1) {
            continue;
        }
        slot_set_volume(VOLUME_PAUSED, i);
    }
    for (j = 0; j < SEQUENCE_PORTS; j++) {
        sdMidiPause(sound_work->ports[j + SEQUENCE_PORT]);
    }
}

static void slots_resume(void)
{
    struct sound_slot *slot;
    SDMIDI *port;
    SDS_MIDI_STAT stat;
    Sint8 i;

    if (sound_work->flags & FX_HELD && sound_work->flags & FX_PAUSED) {
        fx_level_change(sound_work->fx_level);
    }
    slot = sound_work->slots;
    for (i = 0; i < SEQUENCE_SLOT; slot++, i++) {
        if (slot->id < 0) {
            continue;
        }
        if (sound_plays_paused(slot->id) == 1) {
            continue;
        }
        slot_set_volume(slot->volume_sent, i);
    }
    /* @hack spelling: the step of slot on each path, where the loop's step
     * would do */
    for (i = 0; i < SEQUENCE_PORTS; i++) {
        if (slot->id < 0) {
            slot++;
            continue;
        }
        /* @hack spelling: port, a local for the port's handle, where the
         * port would do */
        port = &sound_work->ports[i + SEQUENCE_PORT];
        sdMidiGetStat(*port, &stat);
        if (stat.m_Flg & SDD_PORT_FLG_PLAY && stat.m_Flg & SDD_PORT_FLG_PAUSE) {
            sdMidiContinue(*port);
        } else if (!(slot->mode & MODE_SEQUENCE)) {
            slot_play(slot->bank, slot->number, slot->priority,
                      i + SEQUENCE_SLOT);
        }
        slot++;
    }
}

static Sint8 slot_sequence_ended(Sint8 slot)
{
    SDS_MIDI_STAT stat;
    /* @hack spelling: result and playing, locals, where the tests would
     * do */
    SDE_ERR result;
    int playing;

    slot = SLOT_PORT(slot);
    if (slot < 0 || slot >= SDD_MIDI_PORT_NUM) {
        return -1;
    }
    result = sdMidiGetStat(sound_work->ports[slot], &stat);
    if (result != SDE_ERR_NOTHING) {
        return -1;
    }
    playing = stat.m_Flg & SDD_PORT_FLG_PLAY;
    if (playing == 1) {
        return 0;
    }
    return 1;
}

static Sint8 slot_take(int id, Sint8 priority, const NJS_POINT3 *pos)
{
    const struct sound *sound;
    struct sound_slot *slot;
    Sint8 index;
    Sint8 bank;
    Sint8 number;
    Sint8 slot_priority;
    Uint8 channel;
    Float range;
    Float distance;
    Float slot_range;

    channel = 0;
    sound = sound_of_id(id);
    if (sound == NULL) {
        return -1;
    }
    number = sound->number;
    bank = sound->bank;
    if (number < 0) {
        return -1;
    }
    range = SOUND_RANGE_DEFAULT;
    if ((sound->flags & SOUND_RANGE) != 0 && pos != NULL) {
        range = (Float)sound->range;
        if (range == 0.0F) {
            range = SOUND_RANGE_DEFAULT;
        }
        nearer_camera(pos, &distance);
        if (distance > range || distance < 0.0F) {
            return -1;
        }
    }
    slot_priority = priority;
    if (sound->priority > 0) {
        slot_priority = sound->priority;
    }
    index = slot_pick(slot_priority, id);
    if (index < 0) {
        return -1;
    }
    slot = &sound_work->slots[index];
    if ((slot->channel & CHANNEL_PLAYED) != 0 &&
        (slot->mode & MODE_START) == 0) {
        if (bank == slot->bank) {
            channel = CHANNEL_BANK_SET;
            if (number == slot->number) {
                channel = CHANNEL_BANK_SET | CHANNEL_PROGRAM_SET;
            }
        }
        if (slot->pan == 0) {
            channel |= CHANNEL_PAN_SET;
        }
        if (slot->pitch == 0) {
            channel |= CHANNEL_PITCH_SET;
        }
        channel |= CHANNEL_PLAYED;
    }
    slot->bank = bank;
    slot->number = number;
    slot->pan = 0;
    slot->pitch = 0;
    slot->channel = channel;
    slot->priority = slot_priority;
    if (sound->range == 0) {
        slot_range = SOUND_RANGE_DEFAULT;
    } else {
        slot_range = (Float)sound->range;
    }
    slot->range = slot_range;
    return index;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    struct sound_slot *slot;
    Sint8 index;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_take(id, priority, NULL);
    if (index < 0) {
        return -1;
    }
    slot = &sound_work->slots[index];
    slot->time = TIME_PLAY;
    slot->key = key;
    slot->mode = MODE_START | MODE_ENDS;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *pos)
{
    struct sound_slot *slot;
    Sint8 index;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_take(id, priority, pos);
    if (index < 0) {
        return -1;
    }
    slot = &sound_work->slots[index];
    slot->time = TIME_PLAY;
    slot->key = key;
    slot->mode = MODE_START | MODE_ENDS | MODE_VOLUME_BY_DISTANCE;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = pos->x;
    slot->pos.y = pos->y;
    slot->pos.z = pos->z;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_held(int id, const void *key, Sint8 priority, Sint16 volume)
{
    struct sound_slot *slot;
    Sint8 index;
    int mode;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = 0;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->time = TIME_HELD;
    slot->key = key;
    slot->mode = mode;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_held_at(int id, const void *key, Sint8 priority, Sint16 volume,
                         const NJS_POINT3 *pos)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, pos);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->time = TIME_HELD;
    slot->key = key;
    slot->mode = mode | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = pos->x;
    slot->pos.y = pos->y;
    slot->pos.z = pos->z;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_for(int id, const void *key, Sint8 priority, Sint16 volume,
                     Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->time = duration + 1;
    slot->key = key;
    slot->mode = mode | MODE_ENDS;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, pos);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->time = duration + 1;
    slot->key = key;
    slot->mode = mode | MODE_ENDS | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = pos->x;
    slot->pos.y = pos->y;
    slot->pos.z = pos->z;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_repeating(int id, const void *key, Sint8 priority,
                           Sint16 volume, Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    Sint16 time;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    time = duration + 1;
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        slot->time = time;
        mode = MODE_START;
    }
    slot->repeat_time = time;
    slot->key = key;
    slot->mode = mode | MODE_REPEAT;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_repeating_at(int id, const void *key, Sint8 priority,
                              Sint16 volume, Sint16 duration,
                              const NJS_POINT3 *pos)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    Sint16 time;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    time = duration + 1;
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, pos);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        slot->time = time;
        mode = MODE_START;
    }
    slot->repeat_time = time;
    slot->key = key;
    slot->mode = mode | MODE_REPEAT | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = pos->x;
    slot->pos.y = pos->y;
    slot->pos.z = pos->z;
    slot->follow = NULL;
    return index;
}

Sint8 sound_play_ring(Sint8 player)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint8 pan;

    if (sound_may_play(SOUND_RING_ID) == 0) {
        return -1;
    }
    index = slot_take(SOUND_RING_ID, 0, NULL);
    if (index < 0) {
        return -1;
    }
    slot = &sound_work->slots[index];
    slot->time = TIME_PLAY;
    slot->key = NULL;
    slot->mode = MODE_START | MODE_ENDS | MODE_PAN_SET | MODE_RING;
    slot->id = SOUND_RING_ID;
#ifdef NONMATCHING
    /* the hack's plain form: this side indexes ring_pans by row and parity,
     * from ring_count's own Sint16. player is 0 or 1, as sound.h says. in a
     * two-player game the matching side leaves pan unset for any other
     * player, and this side reads past ring_pans */
    {
        int row = game_state.two_players ? RING_PLAYER + player : RING_ALONE;
        int parity = ring_count(player) % 2 == 1 ? RING_ODD : RING_EVEN;
        pan = ring_pans[row][parity];
    }
#else
    /* @hack type: ring_count's result taken as an int, through its second
     * name, where ring_count's own Sint16 would do */
    if (game_state.two_players == 0) {
        if (ring_count_int(player) % 2 == 1) {
            pan = ring_pans[RING_ALONE][RING_ODD];
        } else {
            pan = ring_pans[RING_ALONE][RING_EVEN];
        }
    } else {
        if (player == 0) {
            if (ring_count_int(player) % 2 == 1) {
                pan = ring_pans[RING_PLAYER][RING_ODD];
            } else {
                pan = ring_pans[RING_PLAYER][RING_EVEN];
            }
        }
        if (player == 1) {
            if (ring_count_int(player) % 2 == 1) {
                pan = ring_pans[RING_PLAYER + 1][RING_ODD];
            } else {
                pan = ring_pans[RING_PLAYER + 1][RING_EVEN];
            }
        }
    }
#endif
    slot->pan = pan;
    slot->volume = 0;
    slot->volume_top = 0;
    if (player_work[player] != NULL) {
        slot->pos.x = player_work[player]->pos.x;
        slot->pos.y = player_work[player]->pos.y;
        slot->pos.z = player_work[player]->pos.z;
        slot->follow = &player_work[player]->pos;
    }
    slot->channel = 0;
    return index;
}

Sint8 sound_play_sequence(int id, const void *key, Sint8 priority,
                          Sint16 volume)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint8 bank;
    Sint8 number;

    if (sound_bank_and_number(id, &bank, &number) < 0 ||
        sound_plays_as(id) != PLAYS_AS_SEQUENCE) {
        return -1;
    }
    index = slot_pick(PRIORITY_TOP, id);
    if (index < 0) {
        return -1;
    }
    slot = &sound_work->slots[index];
    slot->bank = bank;
    slot->number = number;
    slot->time = TIME_SEQUENCE;
    slot->priority = PRIORITY_TOP;
    slot->key = key;
    slot->mode = MODE_START | MODE_SEQUENCE;
    slot->id = id;
    slot->pan = 0;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pitch = 0;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    slot->follow = NULL;
    slot->channel = CHANNEL_SET;
    return index;
}

static Bool task_can_sound(struct task *task)
{
    if (task->work == NULL || task->step == NULL ||
        task->step == task_destroy) {
        return FALSE;
    }
    return TRUE;
}

Sint8 sound_play_task_held(int id, struct task *task, Sint8 priority,
                           Sint16 volume)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    const void *key;

    if (sound_may_play(id) == 0 || task_can_sound(task) == FALSE) {
        return -1;
    }
    /* @hack spelling: key, a copy of task, where task itself would do */
    key = task;
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->id = id;
    slot->time = TIME_HELD;
    slot->key = key;
    slot->mode = mode | MODE_TASK;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->follow = NULL;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    return index;
}

Sint8 sound_play_task_held_at(int id, struct task *task, Sint8 priority,
                              Sint16 volume)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    const void *key;

    if (sound_may_play(id) == 0 || task_can_sound(task) == FALSE) {
        return -1;
    }
    /* @hack spelling: key, a copy of task, where task itself would do */
    key = task;
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, &task->work->pos);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->id = id;
    slot->time = TIME_HELD;
    slot->key = key;
    slot->mode = mode | MODE_TASK | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->follow = &task->work->pos;
    slot->pos.x = slot->follow->x;
    slot->pos.y = slot->follow->y;
    slot->pos.z = slot->follow->z;
    return index;
}

Sint8 sound_play_task_for(int id, struct task *task, Sint8 priority,
                          Sint16 volume, Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    const void *key;

    if (sound_may_play(id) == 0 || task_can_sound(task) == FALSE) {
        return -1;
    }
    /* @hack spelling: key, a copy of task, where task itself would do */
    key = task;
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->id = id;
    slot->time = duration + 1;
    slot->key = key;
    slot->mode = mode | MODE_TASK | MODE_ENDS;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->follow = NULL;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    return index;
}

Sint8 sound_play_task_at_for(int id, struct task *task, Sint8 priority,
                             Sint16 volume, Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    const void *key;

    if (sound_may_play(id) == 0 || task_can_sound(task) == FALSE) {
        return -1;
    }
    /* @hack spelling: key, a copy of task, where task itself would do */
    key = task;
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, &task->work->pos);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->id = id;
    slot->time = duration + 1;
    slot->key = key;
    slot->mode =
        mode | MODE_TASK | MODE_ENDS | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->follow = &task->work->pos;
    slot->pos.x = slot->follow->x;
    slot->pos.y = slot->follow->y;
    slot->pos.z = slot->follow->z;
    return index;
}

Sint8 sound_play_task_repeating(int id, struct task *task, Sint8 priority,
                                Sint16 volume, Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    Sint16 time;
    const void *key;

    if (sound_may_play(id) == 0 || task_can_sound(task) == FALSE) {
        return -1;
    }
    /* @hack spelling: key, a copy of task, where task itself would do */
    key = task;
    index = slot_find(id, key);
    time = duration + 1;
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        slot->time = time;
        mode = MODE_START;
    }
    slot->id = id;
    slot->repeat_time = time;
    slot->key = key;
    slot->mode = mode | MODE_TASK | MODE_REPEAT;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->follow = NULL;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    return index;
}

Sint8 sound_play_task_repeating_at(int id, struct task *task, Sint8 priority,
                                   Sint16 volume, Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;
    Sint16 mode;
    Sint16 time;
    const void *key;

    if (sound_may_play(id) == 0 || task_can_sound(task) == FALSE) {
        return -1;
    }
    /* @hack spelling: key, a copy of task, where task itself would do */
    key = task;
    index = slot_find(id, key);
    if (index >= 0) {
        slot = &sound_work->slots[index];
        mode = slot->mode;
    } else {
        index = slot_take(id, priority, &task->work->pos);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
        mode = MODE_START;
    }
    slot->id = id;
    time = duration + 1;
    slot->time = time;
    slot->repeat_time = time;
    slot->key = key;
    slot->mode =
        mode | MODE_TASK | MODE_REPEAT | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->follow = &task->work->pos;
    slot->pos.x = slot->follow->x;
    slot->pos.y = slot->follow->y;
    slot->pos.z = slot->follow->z;
    return index;
}

void fun_8c05bce0(void)
{
}

Sint8 sound_start_for(int id, const void *key, Sint8 priority, Sint16 volume,
                      Sint16 duration)
{
    struct sound_slot *slot;
    Sint8 index;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    /* @hack spelling: the else after the return, where the code after the
     * if would do */
    if (index >= 0) {
        return index;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
    }
    slot->time = duration + 1;
    slot->key = key;
    slot->mode = MODE_START | MODE_ENDS;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = 0.0F;
    slot->pos.y = 0.0F;
    slot->pos.z = 0.0F;
    slot->follow = NULL;
    return index;
}

Sint8 sound_start_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                         Sint16 duration, const NJS_POINT3 *pos)
{
    struct sound_slot *slot;
    Sint8 index;

    if (sound_may_play(id) == 0) {
        return -1;
    }
    index = slot_find(id, key);
    /* @hack spelling: the else after the return, where the code after the
     * if would do */
    if (index >= 0) {
        slot = &sound_work->slots[index];
        slot->pos.x = pos->x;
        slot->pos.y = pos->y;
        slot->pos.z = pos->z;
        return index;
    } else {
        index = slot_take(id, priority, NULL);
        if (index < 0) {
            return -1;
        }
        slot = &sound_work->slots[index];
    }
    slot->time = duration + 1;
    slot->key = key;
    slot->mode = MODE_START | MODE_ENDS | MODE_MOVING | MODE_VOLUME_BY_DISTANCE;
    slot->id = id;
    slot->volume = volume;
    slot->volume_top = volume;
    slot->pos.x = pos->x;
    slot->pos.y = pos->y;
    slot->pos.z = pos->z;
    slot->follow = NULL;
    return index;
}

void sound_stop_id(int id)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;

    for (i = 0; i < SLOTS; slot++, i++) {
        if (slot->id == id) {
            slot->time = 0;
            slot->mode |= MODE_ENDS;
            slot->mode &= ~MODE_TASK;
        }
    }
}

#ifdef NONMATCHING
void sound_detach_task(const struct task *task)
{
    int i;

    if (sound_work == NULL) {
        return;
    }
    for (i = 0; i < SLOTS; i++) {
        struct sound_slot *slot = &sound_work->slots[i];
        if (slot->key == task) {
            if (slot->mode & MODE_TASK) {
                slot->time = 0;
                slot->mode |= MODE_ENDS;
                slot->mode &= ~MODE_TASK;
            }
            slot->follow = NULL;
            /* Use a private key until the pending stop completes. A new task
             * at this address and NULL-key sounds must remain independent. */
            slot->key = slot;
        }
    }
}
#endif

void sound_stop_key(const void *key)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;

    for (i = 0; i < SLOTS; slot++, i++) {
        if (slot->key == key) {
            slot->time = 0;
            slot->mode |= MODE_ENDS;
            slot->mode &= ~MODE_TASK;
        }
    }
}

void sound_stop(int id, const void *key)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;

    for (i = 0; i < SLOTS; slot++, i++) {
        if (slot->key == key && slot->id == id) {
            slot->time = 0;
            slot->mode |= MODE_ENDS;
            slot->mode &= ~MODE_TASK;
        }
    }
}

void sound_stop_all(void)
{
    struct sound_slot *slot = sound_work->slots;
    int i;

    for (i = 0; i < SLOTS; slot++, i++) {
        /* @hack spelling: the continue, where an if around the rest would
         * do */
        if (slot->id < 0) {
            continue;
        }
        slot->time = 0;
        slot->id = NO_ID;
        slot->mode |= MODE_ENDS;
        slot->mode &= ~MODE_TASK;
        if (game_state.sound_paused == 1) {
            slot->volume_sent = VOLUME_PAUSED;
        }
        slot_stop(i);
    }
    sound_work->next_note_slot = 0;
    sound_work->next_sequence_slot = SEQUENCE_SLOT;
    if (sound_work->flags & FX_HELD) {
        fx_level_restore();
    }
}

void sound_set_volume(const void *key, int id, Sint8 volume)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;
    /* @hack spelling: value, an int copy of volume for the test, where
     * volume would do */
    int value = volume;

    for (i = 0; i < SLOTS; i++, slot++) {
        if (slot->key == key && (slot->id == id || id == SOUND_ANY_ID) &&
            slot->volume != value) {
            slot->volume = volume;
            slot->mode &= ~MODE_VOLUME_BY_DISTANCE;
            slot->mode |= MODE_VOLUME_SET;
        }
    }
}

void sound_set_pan(const void *key, int id, Sint8 pan)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;

    for (i = 0; i < SLOTS; i++, slot++) {
        if (slot->key == key && (slot->id == id || id == SOUND_ANY_ID)) {
            slot->pan = pan;
            slot->mode &= ~MODE_PAN_BY_POS;
            slot->mode |= MODE_PAN_SET;
        }
    }
}

void sound_set_pitch(const void *key, int id, Sint16 pitch)
{
    struct sound_slot *slot = sound_work->slots;
    int i;

    for (i = 0; i < SLOTS; slot++, i++) {
        if (slot->key == key && (slot->id == id || id == SOUND_ANY_ID)) {
            slot->pitch = pitch;
            slot->mode &= ~MODE_PITCH_BY_POS;
            slot->mode |= MODE_PITCH_SET;
        }
    }
}

void sound_pan_by_pos(int id, const void *key, const NJS_POINT3 *pos)
{
    struct sound_slot *slot = sound_work->slots;
    int i;

    for (i = 0; i < SLOTS; slot++, i++) {
        if (slot->key == key && (slot->id == id || id == SOUND_ANY_ID)) {
            if (pos != NULL) {
                slot->pos = *pos;
            }
            slot->mode &= ~MODE_PAN_SET;
            slot->mode |= MODE_PAN_BY_POS;
        }
    }
}

void sound_pitch_by_pos(int id, const void *key, const NJS_POINT3 *pos)
{
    struct sound_slot *slot = sound_work->slots;
    int i;

    for (i = 0; i < SLOTS; slot++, i++) {
        if (slot->key == key && (slot->id == id || id == SOUND_ANY_ID)) {
            if (pos != NULL) {
                slot->pos = *pos;
            }
            slot->mode &= ~MODE_PITCH_SET;
            slot->mode |= MODE_PITCH_BY_POS;
        }
    }
}

void sound_set_volume_in_range(int id, const void *key, const NJS_POINT3 *pos,
                               Float range)
{
    struct sound_slot *slot = sound_work->slots;
    int i;

    for (i = 0; i < SLOTS; i++, slot++) {
        if (slot->key == key && (slot->id == id || id == SOUND_ANY_ID)) {
            if (pos != NULL) {
                slot->pos = *pos;
            }
            slot->volume = slot_volume_in_range(i, range, 0);
            slot->mode &= ~MODE_VOLUME_BY_DISTANCE;
            slot->mode |= MODE_VOLUME_SET;
        }
    }
}

static Float camera_distance(const NJS_POINT3 *pos, Sint8 camera,
                             NJS_VECTOR *out)
{
    NJS_VECTOR to;

    if (camera_views[camera] == NULL) {
        return -1.0F;
    }
    to.x = pos->x - camera_views[camera]->pos.x;
    to.y = pos->y - camera_views[camera]->pos.y;
    to.z = pos->z - camera_views[camera]->pos.z;
    if (out != NULL) {
        *out = to;
    }
    return njSqrt((to.x * to.x) + (to.y * to.y) + (to.z * to.z));
}

static Sint8 nearer_camera(const NJS_POINT3 *pos, Float *distance)
{
    Float first = camera_distance(pos, 0, NULL);
    Float second = camera_distance(pos, 1, NULL);

    if (first < 0.0F && second < 0.0F) {
        if (distance != NULL) {
            *distance = -1.0F;
        }
        return -1;
    }
    if (first < 0.0F) {
        if (distance != NULL) {
            *distance = second;
        }
        return 1;
    }
    if (second < 0.0F) {
        if (distance != NULL) {
            *distance = first;
        }
        return 0;
    }
    if (first > second) {
        if (distance != NULL) {
            *distance = second;
        }
        return 1;
    }
    if (distance != NULL) {
        *distance = first;
    }
    return 0;
}

static Sint8 slot_volume_by_distance(Sint8 index)
{
    struct sound_slot *slot = &sound_work->slots[index];
    Float range = slot->range;
    Float distance;
    Float dx;
    Float dy;
    Float dz;
    Float fall;
    Sint16 volume;

    if (range <= 0.0F) {
        range = SOUND_RANGE_DEFAULT;
    }
    if (game_state.two_players != 0) {
        nearer_camera(&slot->pos, &distance);
    } else if (camera_views[0] != NULL) {
        dx = slot->pos.x - camera_views[0]->pos.x;
        dy = slot->pos.y - camera_views[0]->pos.y;
        dz = slot->pos.z - camera_views[0]->pos.z;
        distance = njSqrt((dx * dx) + (dy * dy) + (dz * dz));
    } else {
        distance = 0.0F;
    }
    if (distance < FALL_START) {
        volume = 0;
    } else {
        fall = (distance - FALL_START) * njInvertSqrt(range - FALL_START) *
               njInvertSqrt(range - FALL_START);
        if (fall < 0.0F) {
            fall = 0.0F;
        }
        /* @hack spelling: the fall subtracted from 0, where its negation
         * would do */
        volume = (int)(0.0F - (fall * VOLUME_FALL));
    }
    volume += slot->volume;
    if (volume < VOLUME_MIN) {
        volume = VOLUME_MIN;
    }
    if (volume > slot->volume_top) {
        volume = slot->volume_top;
    }
    return volume;
}

static Sint8 slot_volume_in_range(Sint8 index, Float range, int unused)
{
    struct sound_slot *slot = &sound_work->slots[index];
    Float distance;
    Float dx;
    Float dy;
    Float dz;
    Float fall;
    Sint16 volume;

    if (camera_views[0] != NULL) {
        dx = slot->pos.x - camera_views[0]->pos.x;
        dy = slot->pos.y - camera_views[0]->pos.y;
        dz = slot->pos.z - camera_views[0]->pos.z;
        distance = njSqrt((dx * dx) + (dy * dy) + (dz * dz));
    } else {
        distance = 0.0F;
    }
    if (distance < FALL_START) {
        volume = slot->volume_top;
    } else if (distance > range) {
        volume = VOLUME_MIN;
    } else {
        fall = (distance - FALL_START) * njInvertSqrt(range - FALL_START) *
               njInvertSqrt(range - FALL_START);
        if (fall < 0.0F) {
            fall = 0.0F;
        }
        /* @hack spelling: the fall scaled in a statement of its own, where
         * one expression would do */
        fall *= VOLUME_FALL;
        volume = (int)(VOLUME_FALL - fall);
    }
    volume += slot->volume_top;
    if (volume < VOLUME_MIN) {
        volume = VOLUME_MIN;
    }
    if (volume > slot->volume_top) {
        volume = slot->volume_top;
    }
    return volume;
}

static Sint8 slot_pan_by_angle(Sint8 index)
{
    Sint8 camera;
    struct sound_slot *slot = &sound_work->slots[index];
    /* @hack redundant: the read of the slot's pan into pan, which every path
     * that returns pan overwrites */
    int pan = slot->pan;

    if (game_state.mono == 1) {
        return 0;
    }
    if (game_state.two_players != 0) {
        camera = nearer_camera(&slot->pos, NULL);
    } else {
        camera = 0;
    }
    /* @hack spelling: the vectors in a block of their own, where the
     * function's own locals would do */
    {
        NJS_VECTOR to;
        NJS_VECTOR look;
        NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
        Float side;

        /* @bug logic: the pan mixes camera 0's place with camera 1's look,
         * where camera 1 is the nearer in a two-player game. the way to the
         * sound is from camera 0's place: MOV #0,R5 at 0x8c05ca0c passes
         * camera 0 to the call of camera_distance at 0x8c05ca1e. the look is
         * camera's, by MOV.L @(R0,R13),R2 at 0x8c05ca32.
         * slot_pitch_by_distance takes its place from camera, by MOV.L
         * @(R0,R5),R5 at 0x8c05cb38 */
        camera_distance(&slot->pos, 0, &to);
        njUnitVector(&to);
        look = camera_views[camera]->dir;
        njUnitVector(&look);
        side = njSin((Uint16)(vector_angle(&look, &to, &up) + HALF_TURN));
        if (_builtin_fabsf(side) < PAN_DEAD_SINE) {
            pan = 0;
        } else {
            if (side > 0.0F) {
                pan = (side - PAN_DEAD_SINE) * PAN_SCALE;
            } else {
                pan = (side + PAN_DEAD_SINE) * PAN_SCALE;
            }
            if (pan < -PAN_MAX) {
                pan = -PAN_MAX;
            } else if (pan > PAN_MAX) {
                pan = PAN_MAX;
            }
        }
    }
    return pan;
}

static Sint16 slot_pitch_by_distance(Sint8 index)
{
    struct sound_slot *slot = &sound_work->slots[index];
    Float pitch_was = slot->pitch;
    Sint8 camera;
    NJS_VECTOR to;
    NJS_POINT3 *eye;
    NJS_POINT3 *pos;
    Float last;
    Float distance;
    Float pitch;

    if (game_state.two_players != 0) {
        camera = nearer_camera(&slot->pos, NULL);
    } else {
        camera = 0;
    }
    eye = &camera_views[camera]->pos;
    pos = &slot->pos;
    last = slot->distance;
    to.x = eye->x - pos->x;
    to.y = eye->y - pos->y;
    to.z = eye->z - pos->z;
    distance = njScalor(&to);
    if (last == 0.0F && distance != 0.0F) {
        slot->distance = distance;
        return 0;
    }
    if (distance > PITCH_REACH) {
        slot->distance = distance;
        return pitch_was;
    }
    if (last > distance && pitch_was > 0.0F) {
        pitch = -(distance * PITCH_PER_UNIT);
    } else {
        /* @hack spelling: the sum in two statements, where one expression
         * would do */
        pitch = slot->pitch;
        pitch += (last - distance) * PITCH_PER_UNIT;
    }
    if (pitch > PITCH_BOUND) {
        pitch = PITCH_BOUND;
    } else if (pitch < -PITCH_BOUND) {
        pitch = -PITCH_BOUND;
    }
    slot->distance = distance;
    return pitch;
}

static int slot_mode_when_cleared(Sint8 index, int mode)
{
    struct sound_slot *slot = &sound_work->slots[index];

    /* @hack spelling: the early return, where one test joined by && would
     * do */
    if (game_state.cleared == 0) {
        return mode;
    }
    if (sound_plays_after_clear(slot->id) == 0) {
        if (slot->time == TIME_HELD) {
            slot->time = TIME_CLEARED;
        }
        /* @hack redundant: the test of MODE_START before its clear */
        if (mode & MODE_START) {
            mode &= ~MODE_START;
        }
        mode &= ~MODE_REPEAT;
        mode |= MODE_ENDS;
        mode &= ~MODE_TASK;
    }
    return mode;
}

void sound_update(void)
{
    struct sound_slot *slot = sound_work->slots;
    Sint8 i;
    int mode;
    Sint8 volume;
    Sint8 pan;
    Sint16 pitch;
    struct task *task;
    int ended;

    for (i = 0; i < SLOTS; slot++, i++) {
        mode = slot->mode;
        if (slot->id == NO_ID) {
            continue;
        }
        if (game_state.sound_paused == 1 && sound_plays_paused(slot->id) == 0) {
            continue;
        }
        mode = slot_mode_when_cleared(i, mode);
        if (mode & MODE_START) {
            slot->distance = 0.0F;
            if (i < SEQUENCE_SLOT && slot->id != sound_work->character_sound) {
                channel_set_program(i, slot->bank, slot->number, slot->channel);
            }
            if (mode & MODE_VOLUME_BY_DISTANCE) {
                volume = slot_volume_by_distance(i);
            } else if (mode & MODE_VOLUME_SET) {
                volume = slot->volume;
                mode &= ~MODE_VOLUME_SET;
            } else {
                volume = slot->volume;
            }
            if (volume != slot->volume_sent ||
                !(slot->channel & CHANNEL_PLAYED)) {
                slot_set_volume(volume, i);
                slot->volume_sent = volume;
            }
            if ((mode & MODE_PAN_SET) || !(slot->channel & CHANNEL_PAN_SET)) {
                slot_set_pan(slot->pan, i);
                mode &= ~MODE_PAN_SET;
            }
            if (mode & MODE_PAN_BY_POS) {
                pan = slot_pan_by_angle(i);
                /* @bug logic: sends the old pan, not the new one, as sound.h
                 * says of sound_update. JSR @R3 at 0x8c05cd94 sends the old,
                 * and MOV.B R0,@(3,R14) at 0x8c05cd9a stores the new after
                 * it */
                slot_set_pan(slot->pan, i);
                slot->pan = pan;
            }
            if (mode & MODE_PITCH_BY_POS) {
                pitch = slot_pitch_by_distance(i);
                slot_set_pitch(slot->pitch, i);
                slot->pitch = pitch;
            }
            if ((mode & MODE_PITCH_SET) ||
                !(slot->channel & CHANNEL_PITCH_SET)) {
                slot_set_pitch(slot->pitch, i);
                mode &= ~MODE_PITCH_SET;
                slot->channel |= CHANNEL_PITCH_SET;
            }
            slot_play(slot->bank, slot->number, slot->priority, i);
            mode &= ~MODE_START;
            slot->channel |= CHANNEL_PLAYED;
            if (slot->time > 0) {
                slot->time--;
            }
        } else {
            if (mode & MODE_TASK) {
                task = (struct task *)slot->key;
                /* @bug memory: reads task's step after task_destroy may have
                 * freed task, and for an _at call the freed work's pos. the
                 * test sees task_destroy only until the walk of the tasks runs
                 * that step, which stores null in it at 0x8c05fdc0 and frees
                 * task. while the heap hasn't given the block out again, the
                 * test then fails, and the sound plays out its time. MOV.L
                 * @(16,R4),R3 at 0x8c05ce18 loads the step through the slot's
                 * key, and FMOV.S @R3,FR3 at 0x8c05ce70 reads the pos */
                if (task->step == task_destroy) {
                    slot->time = 0;
                    slot->follow = NULL;
                    mode |= MODE_ENDS;
                    mode &= ~MODE_TASK;
                } else if ((mode & (MODE_MOVING | MODE_MOVED)) &&
                           slot->follow != NULL) {
                    slot->pos.x = slot->follow->x;
                    slot->pos.y = slot->follow->y;
                    slot->pos.z = slot->follow->z;
                }
            }
            if (slot->time != 0) {
                if (mode & (MODE_MOVING | MODE_MOVED)) {
                    if (mode & MODE_VOLUME_BY_DISTANCE) {
                        volume = slot_volume_by_distance(i);
                        if (slot->volume_sent != volume) {
                            slot_set_volume(volume, i);
                            slot->volume_sent = volume;
                        }
                    }
                    mode &= ~MODE_MOVED;
                }
                if (mode & MODE_VOLUME_SET) {
                    volume = slot->volume;
                    if (slot->volume_sent != volume) {
                        slot_set_volume(volume, i);
                        slot->volume_sent = volume;
                    }
                    mode &= ~MODE_VOLUME_SET;
                }
                if (mode & MODE_PAN_BY_POS) {
                    pan = slot_pan_by_angle(i);
                    if (slot->pan != pan) {
                        /* @bug logic: sends the old pan, not the new one, as
                         * sound.h says of sound_update. JSR @R3 at 0x8c05cef4
                         * sends the old, and MOV.B R0,@(3,R14) at 0x8c05cefa
                         * stores the new after it */
                        slot_set_pan(slot->pan, i);
                        slot->pan = pan;
                    }
                }
                if (mode & MODE_PAN_SET) {
                    slot_set_pan(slot->pan, i);
                    mode &= ~MODE_PAN_SET;
                }
                if (mode & MODE_PITCH_BY_POS) {
                    pitch = slot_pitch_by_distance(i);
                    if (slot->pitch != pitch) {
                        /* @bug logic: sends the old pitch, not the new one,
                         * as sound.h says of sound_update. JSR @R3 at
                         * 0x8c05cf32 sends the old, and MOV.W R0,@(10,R14) at
                         * 0x8c05cf38 stores the new after it */
                        slot_set_pitch(slot->pitch, i);
                        slot->pitch = pitch;
                    }
                }
                if (mode & MODE_PITCH_SET) {
                    slot_set_pitch(slot->pitch, i);
                    mode &= ~MODE_PITCH_SET;
                }
                slot->time--;
            }
            if (slot->time == 0) {
                if (mode & MODE_SEQUENCE) {
                    ended = slot_sequence_ended(i);
                    if (ended != 0) {
                        mode |= MODE_ENDS;
                    }
                }
                if (mode & MODE_REPEAT) {
                    /* @hack spelling: the bits set around the two stores,
                     * where one change of mode would do */
                    mode |= MODE_START;
                    slot->time = slot->repeat_time;
                    slot->channel = CHANNEL_SET;
                    mode &= ~MODE_REPEAT;
                    mode |= MODE_ENDS;
                } else if (mode & MODE_ENDS) {
                    slot_stop(i);
                    if (slot->pan != 0) {
                        slot_set_pan(0, i);
                        slot->pan = 0;
                    }
                    if (slot->pitch != 0) {
                        slot_set_pitch(0, i);
                        slot->pitch = 0;
                    }
                    mode = 0;
                    slot->key = NULL;
                    slot->id = NO_ID;
                    slot->time = 0;
                    /* @hack redundant: the store of 0 in the slot's mode,
                     * which the store of mode after it makes again */
                    slot->mode = 0;
                }
            }
        }
        slot->mode = mode;
    }
}

/* @hack type: the result a Sint8, where a Bool would do */
static Sint8 test_call_takes_volume(struct task *task)
{
    struct sound_test *test = task->motion.p;
    Sint8 takes;

    switch (test->call) {
    case TEST_PLAY:
    case TEST_PLAY_HELD:
    case TEST_PLAY_FOR:
    case TEST_PLAY_REPEATING:
    case TEST_START_FOR:
        takes = 0;
        break;
    default:
        takes = 1;
        break;
    }
    return takes;
}

/* @hack type: the result a Sint8, where a Bool would do */
static Sint8 test_call_takes_duration(struct task *task)
{
    struct sound_test *test = task->motion.p;
    Sint8 takes;

    switch (test->call) {
    case TEST_PLAY_FOR:
    case TEST_PLAY_AT_FOR:
    case TEST_PLAY_REPEATING:
    case TEST_START_FOR:
    case TEST_PLAY_REPEATING_AT:
        takes = 1;
        break;
    default:
        takes = 0;
        break;
    }
    return takes;
}

void sound_test_step(struct task *task)
{
    struct sound_test *test = task->motion.p;
    Sint8 bank;
    Sint8 number;
    int id;
    Sint16 count;
    NJS_POINT3 *pos;
    const PDS_PERIPHERAL *pad;

    /* @hack redundant: the call of sound_bank_and_number, whose bank and
     * number nothing reads */
    sound_bank_and_number(test->id, &bank, &number);
    if (player_pads[TEST_PAD]->press & PDD_DGT_KU) {
        id = test->id & ID_PLACE_MASK;
        id += ID_PLACE(1);
        while (id_list_count(id) < 0) {
            id += ID_PLACE(1);
            if (id > ID_PLACE(8)) {
                id = 0;
            }
        }
        test->id = id;
    } else if (player_pads[TEST_PAD]->press & PDD_DGT_KD) {
        id = test->id & ID_PLACE_MASK;
        id -= ID_PLACE(1);
        while (id_list_count(id) < 0) {
            if (id < 0) {
                id = ID_PLACE(8);
            } else {
                id -= ID_PLACE(1);
            }
        }
        test->id = id;
    }
    if (player_pads[TEST_PAD]->press & PDD_DGT_KR) {
        test->id++;
        if (id_list_count(test->id) <= (test->id & ID_INDEX_MASK)) {
            /* @hack spelling: the mask written as an assignment of test->id
             * & ID_PLACE_MASK, where &= would do */
            test->id = test->id & ID_PLACE_MASK;
        }
    } else if (player_pads[TEST_PAD]->press & PDD_DGT_KL) {
        if (!(test->id & ID_INDEX_MASK)) {
            count = id_list_count(test->id);
            /* @hack spelling: the mask and the sum in two statements, where
             * one expression would do */
            test->id = test->id & ID_PLACE_MASK;
            test->id += count;
        }
        test->id--;
    }
    if (player_pads[TEST_PAD]->press & PDD_DGT_TY) {
        test->call++;
        if (test->call > TEST_CALLS - 1) {
            test->call = TEST_PLAY;
        }
        if (test_call_takes_duration(task) && test_call_takes_volume(task)) {
            test->page = PAGE_BOTH;
            test->option = OPTION_DURATION;
        } else if (test_call_takes_duration(task)) {
            test->page = PAGE_DURATION;
            test->option = OPTION_DURATION;
        } else if (test_call_takes_volume(task)) {
            test->page = PAGE_VOLUME;
            test->option = OPTION_VOLUME;
        } else {
            test->page = PAGE_NONE;
            test->option = OPTION_NONE;
        }
    }
    if (player_pads[TEST_PAD]->press & PDD_DGT_TB) {
        test->playing = FALSE;
        test->played_id = NO_ID;
        test->played_duration = 0;
        test->played_volume = 0.0F;
        test->played_call = -1;
        sound_stop_all();
    }
    if (player_pads[TEST_PAD]->press & PDD_DGT_ST) {
        if (test->option == OPTION_VOLUME) {
            test->volume = 0.0F;
        }
        if (test->option == OPTION_DURATION) {
            test->duration = TEST_DURATION;
        }
    }
    if (test->page == PAGE_BOTH &&
        (player_pads[TEST_PAD]->press & PDD_DGT_TX)) {
        if (test->option == OPTION_DURATION) {
            test->option = OPTION_VOLUME;
        } else {
            test->option = OPTION_DURATION;
        }
    }
    if (test->option == OPTION_VOLUME) {
        if (player_pads[TEST_PAD]->on & PDD_DGT_TL) {
            if (test->volume >= TEST_VOLUME_MIN) {
                test->volume -= TEST_VOLUME_STEP;
            }
        } else if (player_pads[TEST_PAD]->on & PDD_DGT_TR) {
            if (test->volume <= TEST_VOLUME_MAX) {
                test->volume += TEST_VOLUME_STEP;
            }
        }
        /* @bug logic: the test assigns OPTION_DURATION to option, where the
         * test before it compares. it always holds, so OPTION_NONE becomes
         * OPTION_DURATION, and L and R step the duration on PAGE_NONE. MOV.B
         * R0,@(3,R14) at 0x8c05d2c8 stores it */
    } else if ((test->option = OPTION_DURATION)) {
        if (player_pads[TEST_PAD]->on & PDD_DGT_TL) {
            if (test->duration >= 0.0F) {
                test->duration--;
            }
        } else if (player_pads[TEST_PAD]->on & PDD_DGT_TR) {
            /* @bug logic: R's test reads the volume, where the duration's
             * bound is meant, as L's test reads the duration. FMOV.S
             * @(R0,R14),FR2 at 0x8c05d2f6 reads the volume */
            if (test->volume <= TEST_DURATION_TOP) {
                test->duration++;
            }
        }
    }
    if (player_pads[TEST_PAD]->press & PDD_DGT_TA) {
        /* @hack redundant: origin, a local that nothing reads */
        NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

        pos = &player_work[0]->pos;
        switch (test->call) {
        case TEST_PLAY:
            sound_play(test->id, NULL, 0, 0);
            break;
        case TEST_PLAY_AT:
            sound_play_at(test->id, NULL, 0, (Sint8)test->volume, pos);
            break;
        case TEST_PLAY_HELD:
            sound_play_held(test->id, test, 1, 0);
            break;
        case TEST_PLAY_HELD_AT:
            sound_play_held_at(test->id, test, 1, (Sint8)test->volume, pos);
            break;
        case TEST_PLAY_FOR:
            sound_play_for(test->id, test, 1, 0, test->duration);
            break;
        case TEST_PLAY_AT_FOR:
            sound_play_at_for(test->id, test, 1, (Sint8)test->volume,
                              test->duration, pos);
            break;
        case TEST_PLAY_REPEATING:
            sound_play_repeating(test->id, test, 1, 0, test->duration);
            break;
        case TEST_PLAY_REPEATING_AT:
            sound_play_repeating_at(test->id, test, 1, (Sint8)test->volume,
                                    test->duration, pos);
            break;
        case TEST_PLAY_SEQUENCE:
            sound_play_sequence(test->id, NULL, PRIORITY_TOP,
                                (Sint8)test->volume);
            break;
        case TEST_START_FOR:
            sound_start_for(test->id, test, 0, 0, test->duration);
            break;
        }
        test->playing = TRUE;
        test->played_duration = test->duration;
        test->played_id = test->id;
        test->played_volume = test->volume;
        test->played_call = test->call;
    }
    if (test->playing == TRUE) {
        /* @hack redundant: origin, a local that nothing reads */
        NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

        pos = &player_work[0]->pos;
        switch (test->played_call) {
        case TEST_PLAY_FOR:
            sound_play_for(test->played_id, test, 1, 0, test->played_duration);
            break;
        case TEST_PLAY_AT_FOR:
            sound_play_at_for(test->played_id, test, 1,
                              (Sint8)test->played_volume, test->played_duration,
                              pos);
            break;
        case TEST_PLAY_REPEATING:
            sound_play_repeating(test->played_id, test, 1, 0,
                                 test->played_duration);
            break;
        case TEST_PLAY_REPEATING_AT:
            sound_play_repeating_at(test->played_id, test, 1,
                                    (Sint8)test->played_volume,
                                    test->played_duration, pos);
            break;
        case TEST_START_FOR:
            sound_start_for(test->played_id, test, 0, 0, test->played_duration);
            break;
        }
    }
    /* @hack spelling: the assignment of pad inside the test, where a
     * statement of its own would do */
    if ((pad = player_pads[TEST_PAD])->on & PDD_DGT_TX &&
        pad->on & PDD_DGT_TB) {
        task_kill(task);
    }
}

void sound_test_draw(struct task *task)
{
    struct sound_test *test = task->motion.p;
    int id;
    Sint8 bank;
    Sint8 number;
    const char *name;
    Sint8 character;

    njPrint(NJM_LOCATION(5, 3), "SOUNT TEST MONITOR");
    njPrint(NJM_LOCATION(5, 5), "CALL MODE:%s", test_call_names[test->call]);
    /* @hack spelling: id, a local for test->id, where test->id would do */
    id = test->id;
    sound_bank_and_number(id, &bank, &number);
    switch (bank) {
    case 0:
        name = "SY";
        break;
    case 1:
        name = "GM";
        break;
    case CHARACTER_BANK:
        /* @hack type: character, a Sint8 for stage_character(0), where its
         * int would do */
        character = stage_character(0);
        switch (character) {
        case PLAYER_SONIC:
        case PLAYER_SHADOW:
            name = "SS";
            break;
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            name = "KR";
            break;
        case PLAYER_TAILS_MECH:
        case PLAYER_EGGMAN_MECH:
            name = "WK";
            break;
        default:
            name = "SS";
            break;
        }
        break;
    case 3:
        name = "VC";
        break;
    case 4:
        name = "EM";
        break;
    case 5:
        name = "STG SE";
        break;
    case SEQUENCE_BANK:
        name = "STG ENV";
        break;
    case 7:
        name = "WT";
        break;
    default:
        name = "";
        break;
    }
    njPrint(NJM_LOCATION(5, 6), "TONE NO. %xH  %s %d", (Uint16)test->id, name,
            number + 1);
    if (bank == SEQUENCE_BANK || number >= SEQUENCE_NUMBER) {
        njPrint(NJM_LOCATION(27, 6), "[SEQ]");
    }
    njPrint(NJM_LOCATION(5, 7), "BANK NO. %d   LIST NO. %d", bank, number);
    if (test->page != PAGE_NONE) {
        if (test->option == OPTION_VOLUME) {
            njPrint(NJM_LOCATION(5, 8), "VOLUME OFFSET %d",
                    (Sint8)test->volume);
        } else if (test->option == OPTION_DURATION) {
            njPrint(NJM_LOCATION(5, 8), "TIMER CHANGE %d", test->duration);
        }
    }
}

void sound_test_end(struct task *task)
{
    struct sound_test *test = task->motion.p;

    *test->running = FALSE;
}

void sound_load_player_sounds(void)
{
    Sint8 character;

    if (stage_current_id() < STAGE_CHAO &&
        (stage_current_id() < STAGE_CART_1 ||
         stage_current_id() > STAGE_CARTS_LAST)) {
        sound_release_fx_level();
        sound_fx_off();
        sound_start_over();
        game_state.sound_paused = 0;
        /* @hack type: character, a Sint8 for stage_character(0), where its
         * int would do */
        character = stage_character(0);
        sound_load_character(character,
                             &sound_work->lists[SOUND_LIST_CHARACTER],
                             &sound_work->lists[SOUND_LIST_VOICES]);
        /* @hack spelling: the address of the first place, where the table
         * would do */
        sound_lists_set_0_and_1(&sound_work->lists[0]);
        /* @hack spelling: the two calls joined by a comma, where two
         * statements would do */
        ports_reset(), sound_fx_on();
    }
}

void sound_load_menus(const char *name, const struct sound_list *list)
{
    sound_release_fx_level();
    sound_fx_off();
    sound_start_over();
    game_state.sound_paused = 0;
    if (name != NULL && sound_load_file(name) == 0) {
#ifdef NONMATCHING
        /* the hack's plain form: sound_list_set by its own name */
        sound_list_set(sound_work->lists, 0);
#else
        /* @hack type: the call through sound_list_set_without_unk, where
         * sound_list_set would do, declared without the unk lists.c keeps */
        /* @hack spelling: the address of the first place, where the table
         * would do */
        sound_list_set_without_unk(&sound_work->lists[0], 0);
#endif
        sound_work->lists[SOUND_LIST_LOADED] = list;
    }
    /* @hack spelling: the two calls joined by a comma, where two statements
     * would do */
    ports_reset(), sound_fx_on();
}

void sound_set_character_lists(Sint8 character)
{
    sound_lists_set_character(character, sound_work->lists);
}

void sound_load_begin(void)
{
    sound_release_fx_level();
    sdSndClearFxPrg();
    commands_flush();
    slots_reset();
    sound_work->character_sound = NO_ID;
    lists_empty(SOUND_LISTS);
    game_state.sound_paused = 0;
}

void sound_load_end(void)
{
    ports_reset_params();
    ports_set_up();
    sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
    commands_flush();
}

void sound_load_chao(const char *name, const struct sound_list *list)
{
    game_state.sound_paused = 0;
    sdSndClearFxPrg();
    commands_flush();
    slots_reset();
    if (name != NULL && sound_load_file(name) == 0) {
        sound_work->lists[SOUND_LIST_LOADED] = list;
    }
    sdSndSetFxPrg(EFFECT_PROGRAM, EFFECT_OUTPUT);
    commands_flush();
}

int sequence_set_channel_volume(Sint16 slot, Sint8 channel, Sint8 volume)
{
    Sint8 port;
    SDS_MIDI_MES mes;

    if (slot < SEQUENCE_SLOT || slot >= SLOTS) {
        return -1;
    }
    if (channel > SDD_MIDI_CH_MAX || channel < SDD_MIDI_CH_MIN) {
        return -1;
    }
    port = SLOT_PORT(slot);
    sdMidiSetMes(sound_work->ports[port], &mes, channel | SDD_MIDI_MES_CTL_CHG,
                 SDD_MIDI_CTL_CHG_VOL, volume);
    return sdMidiSendMes(sound_work->ports[port], &mes, 0);
}

static const char sound_test_label[] = "IsndTestModeExec";
