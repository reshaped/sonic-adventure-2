/* an event's script: its tracks, from e<event>_0.prs, and its work. the text
 * language's e<event>_<language>.prs replaces the tracks' lines and sound
 * cues */
#ifndef EVENT_SCRIPT_H
#define EVENT_SCRIPT_H
#include <sg_xpt.h>
#include <ninja.h>
#include "event/effect_track.h"
#include "event/lighting.h"
#include "event/playback.h"
#include "event/screen_sprite.h"

#define EVENT_LINES 256
#define EVENT_SOUND_CUES 512
#define EVENT_SWITCHES 64
#define EVENT_EFFECTS 2048
#define EVENT_LIGHT_KEYS 256
#define EVENT_REPEATS 64
#define EVENT_REPEAT_SLOTS 6
#define EVENT_BURSTS 64
#define EVENT_MOVIE_CUES 64

/* line i of event_messages opens at frame, for duration frames. typing.h's
 * typed line, whose frame is EVENT_TYPED_FRAME, holds its x in duration
 * instead */
struct event_line {
    Sint32 frame, duration;
};

/* the sounds a cue plays. sound and voice are -1 for none, and music and
 * jingle are "" or " " for none. music "-" fades the music out. voice is the
 * line whose voice event_voices gives. on EVENT_STAFF_ROLL, a
 * staff_roll_length of 0 or more starts the staff roll. the roll then lasts
 * staff_roll_length * EVENT_STAFF_ROLL_UNIT frames. cue 0's mode of
 * EVENT_HALF_RATE plays the event at EVENT_EVERY_OTHER_BLANK */
struct event_sound_cue {
    Sint32 frame; /* 0 for a free cue, which never fires */
    Sint8 sound, staff_roll_length;
    Sint16 voice;
    char music[16], jingle[16];
    Sint32 mode;
    Uint8 unk_2c[28];
};
#define EVENT_STAFF_ROLL_UNIT 600
#define EVENT_HALF_RATE 2

/* a change of the screen at frame. the EVENT_FADE_ kinds set the color that the
 * foreground or the background fades to. the EVENT_SET_ kinds set its present
 * color as well. the sprite kinds start a screen sprite, which fades in under
 * EVENT_SPRITE_FADE_IN */
enum event_switch_kind {
    EVENT_FADE_FOREGROUND = 1,
    EVENT_SET_FOREGROUND,
    EVENT_SPRITE_FADE_IN,
    EVENT_SPRITE,
    EVENT_SET_BACKGROUND,
    EVENT_FADE_BACKGROUND
};
struct event_switch {
    Sint32 frame;
    Sint8 kind;
    Uint8 unk_05[3];
    NJS_COLOR color;
    Sint16 fade_out, texture;
    Sint32 lifetime;
    struct event_screen_point position;
    NJS_POINT2 scale;
    Uint8 unk_20[32];
};

/* a key of one of the event's lights, which takes light at frame. while the
 * last key to fire is EVENT_LIGHT_BLEND, each step blends the light between the
 * keys around event_frame */
struct event_light_cue {
    Sint32 frame, kind;
    struct event_lighting light;
    Uint8 unk_30[20];
};
#define EVENT_LIGHT_HOLD 1
#define EVENT_LIGHT_BLEND 2

/* at frame, each slot that slots names starts its afterimages, with
 * emission_frames and pose_frames. a negative entry names none */
struct event_repeat_cue {
    Sint32 frame, emission_frames;
    Sint8 slots[EVENT_REPEAT_SLOTS];
    Sint32 pose_frames;
    Uint8 unk_14[44];
};

/* a model burst, which event_model_burst_start takes. degrees holds
 * direction's angles, in degrees */
struct event_burst_cue {
    NJS_POINT3 position;
    NJS_VECTOR acceleration;
    Sint16 degrees[3];
    Sint32 frame, spread;
    Float speed, scale;
    Sint32 count, model, trail, trail_interval;
};

/* a movie of an enum event_movie_kind. a MOVIE_TEXTURE movie plays into texture
 * of the scene file's texlist. the others play on the screen at position. a
 * depth of 0 stands for 1 */
struct event_movie_cue {
    Sint32 frame;
    struct event_screen_point position;
    Float depth;
    Sint8 kind, texture;
    char unk_0e[2];
    char filename[48];
};

/* the script's tracks. a cue fires on the step that takes event_frame from
 * below its frame to or past it */
struct event_tracks {
    struct event_line lines[EVENT_LINES];
    struct event_sound_cue sounds[EVENT_SOUND_CUES];
    struct event_switch switches[EVENT_SWITCHES];
    struct event_effect effects[EVENT_EFFECTS];
    struct event_light_cue lights[EVENT_LIGHTS][EVENT_LIGHT_KEYS];
    struct event_repeat_cue repeats[EVENT_REPEATS];
    struct event_burst_cue bursts[EVENT_BURSTS];
    struct event_movie_cue movies[EVENT_MOVIE_CUES];
};

/* the slots of afterimages, which the scene file's slot_objects names */
#define EVENT_SLOTS 64
/* a slot's afterimages. while active, each step leaves an afterimage of each
 * entity whose object is the slot's in slot_objects. each afterimage lasts
 * pose_frames. age counts the afterimages, and the slot stops once it passes
 * emission_frames */
struct event_repeat {
    Bool active;
    Sint32 pose_frames, emission_frames, age;
};

#define EVENT_SCENES 64
#define EVENT_MOTION_STRIDE 256
#define EVENT_CANCEL_SLOTS (-100)
/* the script's work, at EVENT_SCRIPT_AREA. event_script_reset sets event,
 * line_count, frames, scene_count, starts, motion_counts and slot_count from
 * the event. a field marked as a request asks each step for an action, and
 * nothing in the game makes one */
struct event_script_work {
    Sint32 event, line_count;
    Sint32 sound; /* a request: the sound to play, or -1 */
    Sint32 frames;
    char music[16]; /* a request: the music to play, or "" */
    char jingle[16]; /* a request: the jingle to play, or "" */
    Sint32 voice; /* a request: the line to voice, or -1 */
    Sint32 scene_count;
    /* starts[i] is the frame scene i + 1 starts at, starts[scene_count] the
     * event's length, and the rest -1 */
    Sint32 starts[EVENT_SCENES];
    Sint32 motion_counts[EVENT_SCENES]; /* scene i + 1's motion_count */
    /* a request, or -1: (scene - 1) * EVENT_MOTION_STRIDE plus a motion's
     * index. the motion's root draws a marker while its scene plays */
    Sint32 motion;
    struct event_fog_cue fog;
    Bool fog_on; /* a request: fog in place of the script's */
    struct event_lighting lights[EVENT_LIGHTS];
    /* a request: lights[i] in place of the script's light i */
    Bool light_on[EVENT_LIGHTS];
    /* the sprite editor's */
    Uint32 texture_count;
    Sint32 texture;
    struct event_screen_point sprite_position;
    NJS_POINT2 sprite_scale;
    /* a request, or -1: the slot whose afterimages to show, or
     * EVENT_CANCEL_SLOTS to stop every slot's */
    Sint32 slot;
    Sint32 slot_count; /* the slots slot_objects names */
    Uint8 unk_320[4];
    /* the burst editor's, as struct event_burst_cue holds them */
    Sint16 degrees[3];
    NJS_POINT3 position;
    NJS_VECTOR acceleration;
    Sint32 spread;
    Float speed, scale;
    Sint32 count, model, trail, trail_interval;
    Sint32 loaded_language; /* the text's language, as the loader set it */
    Sint32 language; /* a request: the text language to set, or -1 */
};

extern struct event_repeat event_repeats[EVENT_SLOTS];
/* fire the cues the step reaches, step the fades, and carry out the work's
 * requests. event is event_number */
void event_script_step(int event);
/* ready the script to play the event from its start. it sets the work's
 * fields from the event, and clears the fades, the effects and the requests.
 * for the event it reset last, it leaves motion, fog_on and light_on set,
 * and each light's mode. it paces frames by cue 0's mode */
void event_script_reset(void);

extern struct event_tracks *event_tracks;
extern struct event_script_work *event_script_work;
#endif
