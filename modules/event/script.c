#include <ninja.h>
#include <nj_motion.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "event/areas.h"
#include "event/playback.h"
#include "event/effect_track.h"
#include "event/messages.h"
#include "event/model_burst.h"
#include "event/movie.h"
#include "event/scene.h"
#include "event/screen_sprite.h"
#include "event/script.h"
#include "event/lighting.h"
#include "event/mirror.h"
#include "hud/message_window.h"
#include "shared/staff_roll.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"

/* interpolate_light takes no key at or past this frame as the next */
#define LAST_LIGHT_FRAME 0x7000
#define IN_FRAME(key)                                                          \
    ((key).frame <= event_frame && (key).frame > event_previous_frame)
/* a line's message window stands LINE_DROP below where it opens */
#define LINE_DROP 30
/* how far a fade moves each channel each step */
#define FADE_STEP 3
/* a slot request's afterimages each last TEST_POSE_FRAMES, and they go on
 * for TEST_EMISSION_FRAMES */
#define TEST_POSE_FRAMES 3
#define TEST_EMISSION_FRAMES 10000
/* a light request's marker draws LIGHT_MARKER_AHEAD along the camera's
 * view */
#define LIGHT_MARKER_AHEAD 50.0F

static Sint32 work_size = sizeof(struct event_script_work);
struct event_tracks *event_tracks = (struct event_tracks *)EVENT_TRACK_AREA;
struct event_script_work *event_script_work = EVENT_SCRIPT_AREA;
static Sint32 motion_event = -1;
static Sint32 lighting_event = -1;
static NJS_VECTOR motion_marker = {2.0F, 2.0F, 2.0F};
static NJS_VECTOR light_marker = {3.0F, 3.0F, 3.0F};
static Sint32 last_slot = EVENT_CANCEL_SLOTS;
struct event_repeat event_repeats[EVENT_SLOTS];
static Sint32 light_modes[EVENT_LIGHTS];

static void fade_foreground(void);
static void fade_background(void);
static void serve_requests(void);
static void interpolate_light(int track, struct event_lighting *out);
static void play_voice(int line);

#define START_SPRITE(fade)                                                     \
    {                                                                          \
        sprite.fade_in = (fade);                                               \
        sprite.first_frame = event_tracks->switches[i].frame;                  \
        sprite.fade_out = event_tracks->switches[i].fade_out;                  \
        sprite.texture = event_tracks->switches[i].texture;                    \
        sprite.lifetime = event_tracks->switches[i].lifetime;                  \
        sprite.offset = event_tracks->switches[i].position;                    \
        sprite.scale = event_tracks->switches[i].scale;                        \
        event_screen_sprite_start(&sprite);                                    \
    }

void event_script_step(int event)
{
    Sint32 i;
    Sint32 j;
    struct message_window *window;

    if (event_messages != NULL) {
        for (i = 0; i < event_messages->count; i++) {
            if (IN_FRAME(event_tracks->lines[i])) {
                window = message_window_record(message_window_open_unowned(
                    MESSAGE_WINDOW_LOW, event_messages->messages[i].text,
                    event_tracks->lines[i].duration, game_state.text_language));
                window->sprite.band_color.color = 0;
                window->sprite.y += LINE_DROP;
            }
        }
    }
    for (i = 0; i < EVENT_SOUND_CUES; i++) {
        if (IN_FRAME(event_tracks->sounds[i])) {
            if (event_tracks->sounds[i].sound >= 0) {
                sound_play_sequence(event_tracks->sounds[i].sound, NULL, 2, 0);
            }
            if (event_tracks->sounds[i].staff_roll_length >= 0 &&
                event == EVENT_STAFF_ROLL) {
                staff_roll_start();
                staff_roll_set_duration(
                    event_tracks->sounds[i].staff_roll_length *
                    EVENT_STAFF_ROLL_UNIT);
            }
            if (event_tracks->sounds[i].voice >= 0) {
                play_voice(event_tracks->sounds[i].voice);
            }
            if (event_tracks->sounds[i].music[0] != '\0' &&
                event_tracks->sounds[i].music[0] != ' ') {
                if (event_tracks->sounds[i].music[0] == '-') {
                    music_fade_out();
                } else {
                    music_play_name(event_tracks->sounds[i].music);
                }
            }
            if (event_tracks->sounds[i].jingle[0] != '\0' &&
                event_tracks->sounds[i].jingle[0] != ' ') {
                jingle_play(event_tracks->sounds[i].jingle);
            }
        }
    }
    for (i = 0; i < EVENT_SWITCHES; i++) {
        if (IN_FRAME(event_tracks->switches[i])) {
            struct event_screen_sprite sprite;
            /* @bug uninitialized: START_SPRITE leaves the sprite's color unset,
             * and the calls at 0x8c503d5a and 0x8c503dce copy it to the
             * sprite's task. where the switch fires on a frame's last step,
             * the task draws it at 0x8c50703a before its first step sets
             * one */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the sprite's color starts
             * white */
            sprite.color.a = sprite.color.r = sprite.color.g = sprite.color.b =
                1.0F;
#endif
            switch (event_tracks->switches[i].kind) {
            case EVENT_FADE_FOREGROUND:
                event_foreground.target = event_tracks->switches[i].color;
                break;
            case EVENT_SET_FOREGROUND:
                event_foreground.current = event_tracks->switches[i].color;
                event_foreground.target = event_tracks->switches[i].color;
                break;
            case EVENT_SPRITE_FADE_IN:
                START_SPRITE(TRUE)
                break;
            case EVENT_SPRITE:
                START_SPRITE(FALSE)
                break;
            case EVENT_SET_BACKGROUND:
                event_background.current = event_tracks->switches[i].color;
                event_background.target = event_tracks->switches[i].color;
                break;
            case EVENT_FADE_BACKGROUND:
                event_background.target = event_tracks->switches[i].color;
                break;
            }
        }
    }
    for (i = 0; i < EVENT_EFFECTS; i++) {
        if (IN_FRAME(event_tracks->effects[i])) {
            event_effect_fire(&event_tracks->effects[i]);
        }
    }
    for (j = 0; j < EVENT_LIGHTS; j++) {
        for (i = 0; i < EVENT_LIGHT_KEYS; i++) {
            if (IN_FRAME(event_tracks->lights[j][i])) {
                event_lights[j] = event_tracks->lights[j][i].light;
                light_modes[j] = event_tracks->lights[j][i].kind;
            }
        }
        if (light_modes[j] == EVENT_LIGHT_BLEND) {
            interpolate_light(j, &event_lights[j]);
        }
    }
    /* @hack redundant: a light update no path reaches */
    if (0) {
        light_modes[0] = EVENT_LIGHT_HOLD;
    }
    for (i = 0; i < EVENT_REPEATS; i++) {
        if (IN_FRAME(event_tracks->repeats[i])) {
            for (j = 0; j < EVENT_REPEAT_SLOTS; j++) {
                Sint32 slot = event_tracks->repeats[i].slots[j];
                if (slot >= 0) {
                    event_repeats[slot].active = TRUE;
                    event_repeats[slot].pose_frames =
                        event_tracks->repeats[i].pose_frames;
                    event_repeats[slot].emission_frames =
                        event_tracks->repeats[i].emission_frames;
                    event_repeats[slot].age = 0;
                }
            }
        }
    }
    for (i = 0; i < EVENT_BURSTS; i++) {
        if (IN_FRAME(event_tracks->bursts[i])) {
            struct event_model_burst burst;
            burst.direction.x =
                event_tracks->bursts[i].degrees[0] * ANGLE_PER_DEGREE;
            burst.direction.y =
                event_tracks->bursts[i].degrees[1] * ANGLE_PER_DEGREE;
            /* @bug uninitialized: the third angle overwrites x at 0x8c504376,
             * so the burst's pieces take z unset */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the third angle goes to
             * z */
            burst.direction.z =
                event_tracks->bursts[i].degrees[2] * ANGLE_PER_DEGREE;
#else
            burst.direction.x =
                event_tracks->bursts[i].degrees[2] * ANGLE_PER_DEGREE;
#endif
            burst.position.x = event_tracks->bursts[i].position.x;
            burst.position.y = event_tracks->bursts[i].position.y;
            burst.position.z = event_tracks->bursts[i].position.z;
            burst.acceleration.x = event_tracks->bursts[i].acceleration.x;
            burst.acceleration.y = event_tracks->bursts[i].acceleration.y;
            burst.acceleration.z = event_tracks->bursts[i].acceleration.z;
            burst.spread = event_tracks->bursts[i].spread;
            burst.speed = event_tracks->bursts[i].speed;
            burst.scale = event_tracks->bursts[i].scale;
            burst.count = event_tracks->bursts[i].count;
            burst.model = event_tracks->bursts[i].model;
            burst.trail = event_tracks->bursts[i].trail;
            burst.trail_interval = event_tracks->bursts[i].trail_interval;
            event_model_burst_start(&burst);
        }
    }
    for (i = 0; i < EVENT_MOVIE_CUES; i++) {
        if (IN_FRAME(event_tracks->movies[i])) {
            struct event_movie_scene movie;
            switch (event_tracks->movies[i].kind) {
            case MOVIE_OVERLAY:
            case MOVIE_OPAQUE:
            case MOVIE_MASKED:
                movie.position = event_tracks->movies[i].position;
                if (event_tracks->movies[i].depth == 0.0F) {
                    movie.draw.screen.depth = 1.0F;
                } else {
                    movie.draw.screen.depth = event_tracks->movies[i].depth;
                }
                movie.name = event_tracks->movies[i].filename;
                movie.draw.screen.kind = event_tracks->movies[i].kind;
                event_movie_start_scene(&movie);
                break;
            case MOVIE_TEXTURE:
                movie.draw.texture.name = event_tracks->movies[i].filename;
                movie.draw.texture.texture = event_tracks->movies[i].texture;
                event_movie_start_texture(&movie.draw.texture);
            }
        }
    }
    if (event_big_motion >= 0) {
        if (event_scenes[event_scene_index].big->count != 0 &&
            /* @hack spelling: event_big_triggered compared with TRUE, where
             * the flag alone would do */
            event_big_triggered != TRUE) {
            if (player_pad_records[EVENT_PLAYER_PAD].press & PDD_DGT_TA) {
                event_big_triggered = TRUE;
                event_big_frame = 0.0F;
            }
        }
    }
    fade_foreground();
    fade_background();
    serve_requests();
}

void event_script_reset(void)
{
    Sint32 i;
    Sint32 total;
    struct event_repeat *slot;
    (void)work_size;
    event_script_work->event = event_number;
    event_script_work->sound = -1;
    if (event_messages != NULL) {
        event_script_work->line_count = event_messages->count;
    } else {
        event_script_work->line_count = 0;
    }
    event_script_work->frames = event_scenes->frames;
    event_script_work->scene_count = event_scene_file->scene_count;
    total = 0;
    for (i = 0; i < EVENT_SCENES; i++) {
        event_script_work->starts[i] = -1;
    }
    for (i = 0; i < event_scene_file->scene_count; i++) {
#ifdef NONMATCHING
        if (i == EVENT_SCENES - 1) {
            break; /* the last entry is left for the event's length */
        }
#endif
        event_script_work->starts[i] = total;
        total += event_scene_file->scenes[i + 1].frames;
    }
    event_script_work->starts[i] = event_scenes->frames;
    event_effects_reset();
    for (i = 0; i < EVENT_SCENES; i++) {
        /* @bug memory: the loop reads scenes[i + 1].motion_count for each i
         * below EVENT_SCENES, at 0x8c504af2 and 0x8c504b0a, past the
         * scene_count + 1 scenes of the scene file */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: a scene past the file's count
         * takes no motions */
        if (i >= event_scene_file->scene_count) {
            event_script_work->motion_counts[i] = 0;
            continue;
        }
#endif
        event_script_work->motion_counts[i] =
            event_scene_file->scenes[i + 1].motion_count;
    }
    event_script_work->music[0] = '\0';
    event_script_work->jingle[0] = '\0';
    event_script_work->voice = -1;
    if (motion_event != event_number) {
        event_script_work->motion = -1;
        motion_event = event_number;
    }
    if (lighting_event != event_number) {
        event_script_work->fog_on = FALSE;
        for (i = 0; i < EVENT_LIGHTS; i++) {
            light_modes[i] = EVENT_LIGHT_HOLD;
            event_script_work->light_on[i] = FALSE;
        }
        lighting_event = event_number;
    }
    /* @hack redundant: a walk through the slots that only takes each one's
     * address */
    for (i = 0; i < EVENT_SLOTS; i++) {
        slot = &event_repeats[i];
    }
    (void)slot;
    i = 0;
    while (event_scene_file->slot_objects[i] != NULL) {
        i++;
        if (i >= EVENT_SLOTS) {
            break;
        }
    }
    event_script_work->slot_count = i;
    event_script_work->slot = -1;
    event_foreground.current.color = 0;
    event_foreground.target.color = 0;
    event_background.current.color = 0;
    event_background.target.color = 0;
    event_flare_off();
    event_set_frame_interval(event_tracks->sounds[0].mode == EVENT_HALF_RATE
                                 ? EVENT_EVERY_OTHER_BLANK
                                 : EVENT_EVERY_BLANK);
    event_script_work->language = -1;
}

static Sint32 fade_channel(Uint8 now, Uint8 target)
{
    Sint32 next;
    next = now;
    if (now < target) {
        next += FADE_STEP;
        if (next > target) {
            next = target;
        }
    } else if (now > target) {
        next -= FADE_STEP;
        if (next < target) {
            next = target;
        }
    }
    return next;
}

#define STEP_CHANNEL(fade, channel)                                            \
    (fade).current.argb.channel =                                              \
        fade_channel((fade).current.argb.channel, (fade).target.argb.channel)
static void fade_foreground(void)
{
    STEP_CHANNEL(event_foreground, a);
    STEP_CHANNEL(event_foreground, r);
    STEP_CHANNEL(event_foreground, g);
    STEP_CHANNEL(event_foreground, b);
}
static void fade_background(void)
{
    STEP_CHANNEL(event_background, a);
    STEP_CHANNEL(event_background, r);
    STEP_CHANNEL(event_background, g);
    STEP_CHANNEL(event_background, b);
}

static void serve_requests(void)
{
    Sint32 i;
    Sint32 scene;
    Sint32 motion;
    NJS_VECTOR direction;
    NJS_POINT3 position;
    struct angles angles;

    if (event_script_work->sound >= 0) {
        sound_play_sequence(event_script_work->sound, NULL, 2, 0);
        event_script_work->sound = -1;
    }
    if (event_script_work->music[0] != '\0') {
        music_fade_in(VOLUME_FULL);
        music_play_name(event_script_work->music);
        event_script_work->music[0] = '\0';
    }
    if (event_script_work->jingle[0] != '\0') {
        jingle_play(event_script_work->jingle);
        event_script_work->jingle[0] = '\0';
    }
    if (event_script_work->voice >= 0) {
        play_voice(event_script_work->voice);
        event_script_work->voice = -1;
    }
    if (game_state.total_steps & 1) {
        if (event_script_work->motion >= 0) {
            scene = event_script_work->motion / EVENT_MOTION_STRIDE;
            motion = event_script_work->motion % EVENT_MOTION_STRIDE;
            if (scene + 1 == event_scene_index) {
                /* @hack spelling: direction holding the motion's root, where
                 * position would do */
                event_motion_position(
                    event_scenes[event_scene_index].motions[motion],
                    event_scene_frame, &direction);
                debug_draw_model(&direction, NULL, &motion_marker,
                                 DEBUG_MODEL_MARKER);
            }
        }
    }
    if (event_script_work->fog_on != 0) {
        event_script_fog = event_script_work->fog;
    }
    for (i = 0; i < EVENT_LIGHTS; i++) {
        if (event_script_work->light_on[i] != 0) {
            event_lights[i] = event_script_work->lights[i];
            direction = event_lights[i].direction;
            njUnitVector(&direction);
            position.x = _nj_motion_camera_.px +
                         (_nj_motion_camera_.vx * LIGHT_MARKER_AHEAD);
            position.y = _nj_motion_camera_.py +
                         (_nj_motion_camera_.vy * LIGHT_MARKER_AHEAD);
            position.z = _nj_motion_camera_.pz +
                         (_nj_motion_camera_.vz * LIGHT_MARKER_AHEAD);
            z_axis_angles(&direction, &angles.x, &angles.y);
            /* @bug uninitialized: the light marker draws at 0x8c5050d6 with
             * its z angle unset, since z_axis_angles sets only x and y */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the z angle is 0 */
            angles.z = 0;
#endif
            debug_draw_model(&position, &angles, &light_marker,
                             DEBUG_MODEL_MARKER);
        }
    }
    if (event_script_work->slot >= 0) {
        if (last_slot != event_script_work->slot) {
            for (i = 0; i < EVENT_SLOTS; i++) {
                event_repeats[i].active = FALSE;
            }
        }
        event_repeats[event_script_work->slot].active = TRUE;
        event_repeats[event_script_work->slot].pose_frames = TEST_POSE_FRAMES;
        event_repeats[event_script_work->slot].emission_frames =
            TEST_EMISSION_FRAMES;
        event_repeats[event_script_work->slot].age = 0;
        njPrint(NJM_LOCATION(1, 20), "blare testing[%d]",
                (int)event_script_work->slot);
        last_slot = event_script_work->slot;
    } else if (event_script_work->slot == EVENT_CANCEL_SLOTS) {
        for (i = 0; i < EVENT_SLOTS; i++) {
            event_repeats[i].active = FALSE;
        }
    }
    if (event_script_work->language >= 0) {
        game_state.text_language = event_script_work->language;
        event_script_work->language = -1;
    }
}

#define LIGHT(key, member) event_tracks->lights[track][key].light.member
#define INTERPOLATE(member)                                                    \
    out->member =                                                              \
        (((LIGHT(next, member) - LIGHT(previous, member)) * fraction) +        \
         LIGHT(previous, member))

static void interpolate_light(int track, struct event_lighting *out)
{
    Sint32 i;
    Sint32 next;
    Sint32 previous;
    Sint32 next_frame;
    Sint32 previous_frame;
    Float fraction;
    next_frame = LAST_LIGHT_FRAME;
    previous_frame = 0;
    next = -1;
    previous = -1;
    for (i = 0; i < EVENT_LIGHT_KEYS; i++) {
        if (event_tracks->lights[track][i].frame >= event_frame &&
            event_tracks->lights[track][i].frame < next_frame) {
            next = i;
            next_frame = event_tracks->lights[track][i].frame;
        }
        if (event_tracks->lights[track][i].frame <= event_frame &&
            event_tracks->lights[track][i].frame > previous_frame) {
            previous = i;
            previous_frame = event_tracks->lights[track][i].frame;
        }
    }
    if (next == -1) {
        return;
    }
    if (previous == -1) {
        return;
    }
    if (next_frame == previous_frame) {
        fraction = 0.0F;
    } else {
        fraction = (event_frame - previous_frame) /
                   (Float)(next_frame - previous_frame);
    }
    INTERPOLATE(direction.x);
    INTERPOLATE(direction.y);
    INTERPOLATE(direction.z);
    INTERPOLATE(color[0]);
    INTERPOLATE(color[1]);
    INTERPOLATE(color[2]);
    INTERPOLATE(ambient_intensity);
    INTERPOLATE(ambient[0]);
    INTERPOLATE(ambient[1]);
    INTERPOLATE(ambient[2]);
}

static void play_voice(int line)
{
    Sint32 i;
    Sint32 key;

    /* @hack spelling: the key summed in two statements, where one would do */
    key = event_number * EVENT_KEYS;
    key += line;
    for (i = 0;; i++) {
        if (event_voices[i].key == EVENT_VOICES_END) {
            return;
        } else if (event_voices[i].key == key) {
            break;
        }
    }
    voice_play_priority_2(event_voices[i].voice);
}
