/* Run the overlay's loader frame by frame with writable host replacements
 * for its fixed RAM and a disc that can fail after earlier assets loaded. */
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "event/areas.h"
#include "event/messages.h"
#include "event/mirror.h"
#include "event/scene.h"
#include "event/script.h"
#include "system/areas.h"

static union {
    struct event_tracks tracks;
    unsigned char bytes[EVENT_TRACK_BYTES];
} track;
static union {
    struct event_tracks buffer;
    struct {
        struct event_scene_file file;
        struct event_scene scenes[2];
        struct event_texture_size sizes[1];
        struct event_mirrors mirrors;
        struct event_big big;
        NJS_MOTION camera, *camera_slot;
        NJS_CNK_OBJECT *slots[EVENT_SLOTS], *upgrades[3 * EVENT_UPGRADES],
            *tails[1];
        struct event_attachment attachments[EVENT_ATTACHMENTS];
    } data;
} scene_storage;
#define scene_data scene_storage.data
static struct {
    NJS_TEXLIST *head;
    NJS_TEXLIST list;
    NJS_TEXNAME names[1];
} textures;
static struct message_list message_bank[2];
static struct event_script_work script;
static Uint8 work[2048];

#undef EVENT_TRACK_AREA
#undef EVENT_SCENE_AREA
#undef EVENT_TEXLIST_AREA
#undef EVENT_MESSAGE_AREA
#undef WORK_AREA
#undef WORK_AREA_END
#define EVENT_TRACK_AREA ((Uint32 *)&track)
#define EVENT_SCENE_AREA ((Uint8 *)&scene_data)
#define EVENT_TEXLIST_AREA (&textures.head)
#define EVENT_MESSAGE_AREA ((Uint8 *)message_bank)
#define WORK_AREA work
#define WORK_AREA_END (work + sizeof(work))
#include "event/loader.c"

struct game_state game_state;
Uint8 game_bits[GAME_BITS], game_flags[GAME_FLAGS];
Sint32 sequence_events[SEQUENCE_EVENTS], event_sequence_index;
Uint32 pad_held[PAD_PORTS], pad_press_repeat[PAD_PORTS];
struct event_tracks *event_tracks = &track.tracks;
struct event_script_work *event_script_work = &script;
struct event_scene_file *event_scene_file = &scene_data.file;
struct event_scene *event_scenes;
struct message_list *event_messages;
NJS_TEXLIST event_effect_texlist;
static NJS_TEXNAME warp_name;
NJS_TEXLIST warp_hole_texlist = {&warp_name, 1};
struct texture_pvm event_pvms[1];
NJS_TEXLIST **event_texture_groups[1];
NJS_TEXMEMLIST *texmemlist;
Bool event_movie_draw_plane;
static const char *fault;
static int event_dir, scene_refs, effect_refs, starts, sound_loads, music,
    movies;

Sint32 gdFsChangeDir(const char *name)
{
    if (!strcmp(name, "/SONIC2")) {
        event_dir = 0;
        return GDD_ERR_OK;
    }
    if (strcmp(name, "EVENT") == 0) {
        assert(!event_dir);
        if (strcmp(fault, "directory") == 0) {
            return GDD_ERR_NOTFOUND;
        }
        event_dir = 1;
    } else {
        assert(event_dir && strcmp(name, "..") == 0);
        if (!strcmp(fault, "parent directory") && effect_refs) {
            return GDD_ERR_NOTFOUND;
        }
        event_dir = 0;
    }
    return GDD_ERR_OK;
}

GDFS gdFsOpen(const char *name, GDFS_DIRREC dir)
{
    assert(event_dir);
    return NULL;
}
void gdFsClose(GDFS file)
{
    assert(0);
}

static void write_scene(void)
{
    memset(&scene_data, 0, sizeof(scene_data));
    scene_data.file.scenes = scene_data.scenes;
    scene_data.file.scene_count = 1;
    scene_data.file.mirrors = &scene_data.mirrors;
    scene_data.scenes[1].big = &scene_data.big;
    scene_data.scenes[1].camera_count = 1;
    scene_data.camera_slot = &scene_data.camera;
    scene_data.scenes[1].cameras = &scene_data.camera_slot;
    scene_data.file.texture_sizes = scene_data.sizes;
    scene_data.file.slot_objects = scene_data.slots;
    scene_data.file.upgrades = scene_data.upgrades;
    scene_data.file.attachments = scene_data.attachments;
    scene_data.file.tails = scene_data.tails;
}

Sint32 loader_read_prs_checked(const char *name, void *dst, Uint32 capacity)
{
    Uint32 i;
    assert(event_dir);
    if (dst == message_bank) {
        assert(capacity == EVENT_MESSAGE_BYTES);
        if (strcmp(fault, "message") == 0) {
            return -1;
        }
        message_bank[0].event = selected;
        message_bank[0].count = 0;
        message_bank[1].event = -1;
        return sizeof(message_bank);
    }
    if (dst == &track) {
        if (strstr(name, "_0.prs") != NULL) {
            assert(capacity >= sizeof(*event_tracks));
            return sizeof(*event_tracks);
        }
        assert(strstr(name, "_1.prs") &&
               capacity == offsetof(struct event_tracks, switches));
        for (i = 0; i < EVENT_SOUND_CUES; i++) {
            event_tracks->sounds[i].voice = -1;
        }
        return strcmp(fault, "language") == 0 ? (Sint32)capacity - 1
                                              : (Sint32)capacity;
    }
    if (dst == &textures.head) {
        assert(capacity == EVENT_TEXLIST_BYTES);
        textures.head = &textures.list;
        textures.list.nbTexture = 1;
        textures.list.textures =
            strcmp(fault, "texlist") == 0 ? NULL : textures.names;
        return sizeof(textures);
    }
    assert(dst == &scene_data);
    if (strstr(name, "_j.prs")) {
        assert(capacity == offsetof(struct event_tracks, switches));
        if (strcmp(fault, "voice") == 0) {
            return -1;
        }
        for (i = 0; i < EVENT_SOUND_CUES; i++) {
            scene_storage.buffer.sounds[i].voice = -1;
        }
        scene_storage.buffer.sounds[0].voice = 42;
        scene_storage.buffer.sounds[0].frame = 35;
        return (Sint32)capacity;
    }
    assert(capacity == EVENT_SCENE_BYTES);
    write_scene();
    if (!strcmp(fault, "mirrors")) {
        scene_data.mirrors.count = 1;
    }
    if (!strcmp(fault, "Big motions")) {
        scene_data.big.count = 1;
    }
    if (strcmp(fault, "scene") == 0) {
        return sizeof(scene_data.file) - 1;
    }
    if (strcmp(fault, "scene table") == 0) {
        scene_data.scenes[1].cameras = NULL;
    }
    return sizeof(scene_data);
}

Sint32 loader_read_file_checked(const char *name, void *dst, Uint32 capacity)
{
    assert(event_dir);
    if (dst == work) {
        assert(strcmp(name, "eveff.pvm") == 0 && capacity == sizeof(work));
        return strcmp(fault, "effects") == 0 ? -1 : 32;
    }
    assert(dst == &scene_data && capacity == EVENT_SCENE_BYTES);
    assert(!strcmp(name, "e0203.bin") || !strcmp(name, "e0210.bin"));
    write_scene();
    return strcmp(fault, "raw short") == 0
               ? (Sint32)((char *)(scene_data.attachments + EVENT_ATTACHMENTS) -
                          (char *)&scene_data) -
                     1
               : (Sint32)sizeof(scene_data);
}

Bool texture_load_pvm_in(const char *name, NJS_TEXLIST *list, Uint8 *buffer,
                         Sint32 size)
{
    assert(event_dir && list == &textures.list && buffer == EVENT_SCENE_AREA &&
           size == EVENT_SCENE_BYTES);
    scene_refs = strcmp(fault, "scene PVM") != 0;
    return scene_refs;
}
Bool texture_load_pvm_memory(const void *buffer, Uint32 size, NJS_TEXLIST *list)
{
    assert(event_dir && buffer == work && size == 32 &&
           list == &event_effect_texlist);
    effect_refs = strcmp(fault, "effects upload") != 0;
    return effect_refs;
}
void texture_discard_pvm(NJS_TEXLIST *list)
{
    if (list == &textures.list) {
        assert(scene_refs);
        scene_refs = 0;
    } else {
        assert(list == &event_effect_texlist && effect_refs);
        effect_refs = 0;
    }
}
Bool texture_load_shared_with_optional(struct texture_pvm *pvms,
                                       NJS_TEXLIST ***groups,
                                       const NJS_TEXNAME *const *optional,
                                       Uint32 optional_count)
{
    assert(pvms == event_pvms && groups == event_texture_groups);
    assert(optional_count == 1 && optional != NULL &&
           optional[0] == &warp_name);
    assert(!event_dir && scene_refs && effect_refs);
    return strcmp(fault, "shared PVM") != 0;
}
void event_fog_load(int event, int count)
{
    assert(event_dir && count == 1);
}
void event_movie_start_scenes(void)
{
    assert(!event_dir);
    starts++;
}
void sound_load_event(const char *name, const struct sound_list *list)
{
    assert(event_dir);
    sound_loads++;
}
void music_fade_in(int volume)
{
    assert(!event_dir);
    music++;
}
void staff_roll_load(int story)
{
    assert(selected == 210);
}
Bool event_movie_play(struct event_movie_request *request)
{
    assert(event_dir);
    movies++;
    return strcmp(fault, "movie") != 0;
}
void syMallocStat(Uint32 *a, Uint32 *b)
{
    *a = *b = 0;
}
void njPrintC(Int loc, const char *s)
{
}
void njPrintColor(Uint32 c)
{
}
void njPrintH(Int loc, Uint32 n, Int digit)
{
}
void njPrint(Int loc, const char *s, ...)
{
}
Uint32 njCalcTexture(Uint32 flag)
{
    return 0;
}
void njGarbageTexture(NJS_TEXMEMLIST *list, Uint32 n)
{
}

static void run(int id, const char *failure)
{
    int result = NO_EVENT;
    int frame;
    int before = starts;
    int previous_movies = movies;

    fault = failure;
    game_flags[FLAG_SEQUENCE] = SEQUENCE_HERO;
    game_state.text_language = 1;
    game_state.voice_language = 0;
    sequence_events[0] = id;
    sequence_events[1] = NO_EVENT;
    event_sequence_index = 0;
    event_scenes = scene_data.scenes;
    event_messages = message_bank;
    textures.head = &textures.list;
    game_bits[id] = 0;
    script.loaded_language = -1;
    for (frame = 0; frame < 8 && result == NO_EVENT; frame++) {
        result = event_load_step();
        assert(!event_dir);
    }
    if (*failure) {
        assert(result == EVENT_LOAD_FAILED && event_sequence_index == 0 &&
               !game_bits[id]);
        assert(!event_scenes && !event_messages && !textures.head &&
               !effect_refs && !scene_refs);
        assert(starts == before && sound_loads == before && music == before &&
               script.loaded_language == -1);
    } else {
        assert(result == id && event_sequence_index == 1 && game_bits[id] > 0);
        if (id < EVENT_FIRST_MOVIE) {
            assert(event_scenes == scene_data.scenes &&
                   event_messages == message_bank &&
                   scene_data.file.texlist == &textures.list);
            assert(starts == before + 1 && sound_loads == starts &&
                   music == starts && script.loaded_language == 1);
            assert(event_tracks->sounds[0].voice == 42 &&
                   event_tracks->sounds[0].frame == 35);
            texture_discard_pvm(&textures.list);
            texture_discard_pvm(&event_effect_texlist);
        }
    }
    assert(movies == previous_movies + (id >= EVENT_FIRST_MOVIE));
}

int main(void)
{
    static const char *failures[] = {
        "directory", "message",        "language",  "voice",
        "texlist",   "scene PVM",      "scene",     "scene table",
        "effects",   "effects upload", "shared PVM"};
    unsigned i;
    for (i = 0; i < sizeof(failures) / sizeof(*failures); i++) {
        run(2, failures[i]);
    }
    run(2, "mirrors");
    run(2, "Big motions");
    run(2, "parent directory");
    run(203, "raw short");
    run(2, "");
    run(203, "");
    run(210, "");
    run(409, "movie");
    run(409, "");
    return 0;
}
