/* Ordinary mini-event loads, optional files and retry on a synthetic disc. */
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "system/areas.h"

static union {
    unsigned char data[0xe0000];
    double alignment;
} memory;
#undef DEMO_AREA
#undef MESSAGE_AREA
#undef SCRIPT_AREA
#undef REQUEST_AREA
#undef DEMO_AREA_BYTES
#define DEMO_AREA (memory.data)
#define MESSAGE_AREA (memory.data + 0xd0000)
#define SCRIPT_AREA (memory.data + 0xdc000)
#define REQUEST_AREA (memory.data + 0xdf000)
#define DEMO_AREA_BYTES ((Uint32)(MESSAGE_AREA - DEMO_AREA))

#include "event/mini_event.c"

struct game_state game_state;
static int directory;
static Bool has_scene = TRUE;
static Bool has_catalog = TRUE;
static Bool has_script = TRUE;
static int stops;
static int windows;
static struct scene_file {
    struct mini_event_file file;
    NJS_MOTION camera;
    NJS_MDATA1 track;
} scene;
static struct catalog_file {
    struct message_list lists[2];
    struct message line;
    char text[8];
} catalog;
static struct mini_event_script script;

Sint32 gdFsChangeDir(const char *name)
{
    if (!directory) {
        assert(strcmp(name, "EVENT") == 0);
        directory = 1;
    } else {
        assert(strcmp(name, "..") == 0);
        directory = 0;
    }
    return GDD_ERR_OK;
}

Sint32 loader_read_prs_checked(const char *name, void *dest, Uint32 size)
{
    assert(directory);
    if (dest == DEMO_AREA) {
        assert(size == DEMO_AREA_BYTES && strcmp(name, "me0001.prs") == 0);
        if (!has_scene) {
            return -1;
        }
        memcpy(dest, &scene, sizeof(scene));
        return sizeof(scene);
    }
    assert(dest == MESSAGE_AREA &&
           size == (Uint32)(SCRIPT_AREA - MESSAGE_AREA));
    assert(strcmp(name, "evmesH1.prs") == 0);
    if (!has_catalog) {
        return -1;
    }
    memcpy(dest, &catalog, sizeof(catalog));
    return sizeof(catalog);
}

Sint32 loader_read_file_checked(const char *name, void *dest, Uint32 size)
{
    assert(directory && strcmp(name, "me0001_1.scr") == 0);
    assert(dest == SCRIPT_AREA && size == (Uint32)(REQUEST_AREA - SCRIPT_AREA));
    if (!has_script) {
        return -1;
    }
    memcpy(dest, &script, sizeof(script));
    return sizeof(script);
}

void music_stop(void)
{
    stops++;
}

struct task *message_window_open_unowned(int kind, const char *text, int time,
                                         int language)
{
    assert(kind == MESSAGE_WINDOW_FADE && time == NO_MESSAGES_STEPS);
    assert(language == game_state.text_language && text[0]);
    windows++;
    return NULL;
}

struct message_window *message_window_record(struct task *task)
{
    assert(task != NULL);
    return NULL;
}

/* what the skip and a step's cues reach */
static PDS_PERIPHERAL pad;
const PDS_PERIPHERAL *player_pads[PAD_PORTS] = {&pad};
struct event_voice event_voices[] = {{EVENT_VOICES_END, 0}};
static struct task window;
static struct task *open_window;
static int skip_steps;
static int camera_skips;
static int deferred;
static int named;
static char last_name[16];
static int sounds;
static int vibrations;
static int kills;
static int voice_stops;
static int blacks;

int camera_motion_skip(int camera)
{
    int steps = skip_steps;

    assert(camera == 0);
    camera_skips++;
    skip_steps = 0;
    return steps;
}

void music_play_deferred(void)
{
    deferred++;
}

void music_play_name(const char *name)
{
    named++;
    strcpy(last_name, name);
}

void jingle_play(const char *name)
{
    assert(0);
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    assert(id == 8);
    sounds++;
    return 0;
}

void voice_play_priority_2(int id)
{
    assert(0);
}

void voices_stop(void)
{
    voice_stops++;
}

void vibration_start(int player, int power, int freq, int time)
{
    assert(player == 0 && power == 1 && freq == 1 && time == 1);
    vibrations++;
}

void vibration_start_fading(int player, int power, int freq, int length)
{
    assert(0);
}

struct task *message_window_current(void)
{
    return open_window;
}

void task_kill(struct task *task)
{
    assert(task == &window);
    kills++;
}

void fade_set_black(void)
{
    blacks++;
}

void fade_reset_idle(void)
{
}

Bool fade_in_step(void)
{
    return FALSE;
}

Bool fade_out_step(void)
{
    return FALSE;
}

/* what an event that starts reaches. none starts here */
struct task_work *player_work[PLAYER_SLOTS];

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *), const char *name)
{
    assert(0);
    return NULL;
}

void task_destroy(struct task *task)
{
    assert(0);
}

void player_start_event(int character, struct player_event *event)
{
    assert(0);
}

void player_end_event(int character)
{
    assert(0);
}

void player_set_pos(int slot, Float x, Float y, Float z)
{
    assert(0);
}

int camera_top(int camera)
{
    assert(0);
    return 0;
}

void camera_remove_mode(int camera, int index)
{
    assert(0);
}

int camera_motion_start(int camera, struct camera_motion *motion)
{
    assert(0);
    return 0;
}

void vibration_stop(void)
{
    assert(0);
}

static void cue_music(int index, Sint32 frame, const char *music)
{
    script.cues[index].frame = frame;
    script.cues[index].sounds[0] = script.cues[index].sounds[1] = -1;
    script.cues[index].voice = -1;
    strcpy(script.cues[index].music, music);
}

/* Start on the first pad skips a mini event to its last step. of the cues
 * it passes, only the latest music plays */
static void skip(void)
{
    has_catalog = TRUE;
    has_script = TRUE;
    cue_music(0, 5, "-");
    cue_music(1, 20, "a");
    cue_music(2, 40, "-");
    cue_music(3, 30, "b");
    cue_music(4, 59, "z");
    cue_music(5, 25, "");
    script.cues[5].sounds[0] = 8;
    script.cues[5].fade = FADE_IN;
    script.cues[5].vibration = 10101;
    mini_event_load(1);
    assert(mini_event_length == 60.0F && mini_event_message_count == 0);

    /* a step without Start plays its frame's cue whole */
    mini_event_frame = 24.0F;
    mini_event_fade = FADE_NONE;
    mini_event_step(NULL);
    assert(mini_event_frame == 25.0F && camera_skips == 0);
    assert(sounds == 1 && vibrations == 1 && blacks == 1);
    assert(mini_event_fade == FADING_IN && deferred == 0 && named == 0);

    /* Start at frame 10 passes the 48 steps the camera's motion passes.
     * frame 40's music, the latest passed, starts. the passed sound,
     * vibration and fade don't. the step itself then plays frame 59 */
    mini_event_frame = 10.0F;
    mini_event_fade = FADE_NONE;
    sounds = vibrations = blacks = 0;
    skip_steps = 48;
    open_window = &window;
    pad.press = PDD_DGT_ST;
    mini_event_step(NULL);
    assert(camera_skips == 1 && mini_event_frame == 59.0F);
    assert(deferred == 1 && named == 1 && strcmp(last_name, "z") == 0);
    assert(sounds == 0 && vibrations == 0 && blacks == 0);
    assert(mini_event_fade == FADE_NONE);
    assert(kills == 1 && voice_stops == 1);

    /* Start with no step left to pass changes nothing */
    mini_event_frame = 30.0F;
    mini_event_step(NULL);
    assert(camera_skips == 2 && mini_event_frame == 31.0F);
    assert(deferred == 1 && named == 1 && kills == 1 && voice_stops == 1);
    pad.press = 0;
}

/* an attract demo's file is a dump of RAM from DEMO_AREA up to CAMSET_AREA,
 * so a demo leaves DEMO_AREA and the three areas past it holding whatever its
 * file dumped. here that is a whole event: a scene, a list of event 1's
 * messages, and a script whose first cue plays a jingle at frame 1. a load
 * after the demo takes each area from the event's own files, and none from
 * the dump: without the scene no event starts, without the messages the
 * event has none, and without the script no cue plays */
static void after_demo(void)
{
    static struct mini_event_script dumped;
    struct catalog_file listed = catalog;
    int opened = windows;
    Uint32 i;

    listed.lists[0].count = 1;
    dumped.cues[0].frame = 1;
    dumped.cues[0].sounds[0] = dumped.cues[0].sounds[1] = -1;
    dumped.cues[0].voice = -1;
    strcpy(dumped.cues[0].jingle, "x");
    memset(memory.data, 0x5a, sizeof(memory.data));
    memcpy(DEMO_AREA, &scene, sizeof(scene));
    memcpy(MESSAGE_AREA, &listed, sizeof(listed));
    memcpy(SCRIPT_AREA, &dumped, sizeof(dumped));

    has_scene = has_catalog = has_script = FALSE;
    mini_event_load(1);
    assert(mini_event_number == 0 && mini_event_length == 0.0F);
    assert(mini_event_start() == -1 && !game_state.mini_event_running);

    has_scene = TRUE;
    mini_event_load(1);
    assert(mini_event_number == 1 && mini_event_length == 60.0F);
    assert(mini_event_messages == NULL && mini_event_message_count == 0);
    assert(windows == opened + 1);
    for (i = 0; i < sizeof(dumped); i++) {
        assert(SCRIPT_AREA[i] == 0);
    }
    mini_event_frame = 0.0F;
    mini_event_fade = FADE_NONE;
    mini_event_step(NULL);
    assert(mini_event_frame == 1.0F);

    memcpy(MESSAGE_AREA, &listed, sizeof(listed));
    memcpy(SCRIPT_AREA, &dumped, sizeof(dumped));
    has_catalog = has_script = TRUE;
    mini_event_load(1);
    assert(mini_event_message_count == catalog.lists[0].count);
    assert(memcmp(SCRIPT_AREA, &script, sizeof(script)) == 0);
}

int main(void)
{
    game_state.text_language = 1;
    scene.file.camera =
        (NJS_MOTION *)(DEMO_AREA + offsetof(struct scene_file, camera));
    scene.camera.nbFrame = 60;
    scene.camera.mdata = DEMO_AREA + offsetof(struct scene_file, track);
    scene.camera.inp_fn = 1;
    scene.camera.type = NJD_MTYPE_POS_0;
    catalog.lists[0].event = 1;
    catalog.lists[0].count = 1;
    catalog.lists[0].messages =
        (struct message *)(MESSAGE_AREA + offsetof(struct catalog_file, line));
    catalog.line.text =
        (char *)MESSAGE_AREA + offsetof(struct catalog_file, text);
    strcpy(catalog.text, "Hello");
    catalog.lists[1].event = -1;
    script.pos.x = 10.0F;
    mini_event_load(1);
    assert(!directory && mini_event_number == 1 && mini_event_length == 60.0F);
    assert(mini_event_message_count == 1 && mini_event_script->pos.x == 10.0F);
    assert(stops == 1 && windows == 0);

    has_scene = FALSE;
    mini_event_load(1);
    assert(!directory && mini_event_number == 0 && mini_event_length == 0.0F);
    assert(mini_event_messages == NULL && mini_event_message_count == 0);
    assert(stops == 1);

    has_scene = TRUE;
    has_script = FALSE;
    catalog.lists[0].count = 0;
    mini_event_load(1);
    assert(mini_event_length == 60.0F && mini_event_script->pos.x == 0.0F);
    assert(mini_event_message_count == 0 && mini_event_messages != NULL);
    assert(stops == 2 && windows == 0);

    has_catalog = FALSE;
    mini_event_load(1);
    assert(!directory && mini_event_length == 60.0F && stops == 3);
    assert(mini_event_messages == NULL && windows == 1);
    skip();
    after_demo();
    return 0;
}
