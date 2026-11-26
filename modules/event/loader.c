#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "event/areas.h"
#include "event/playback.h"
#include "event/loader.h"
#include "event/movie.h"
#include "event/scene.h"
#include "event/scene_fog.h"
#include "event/script.h"
#include "main.h"
#include "shared/staff_roll.h"
#include "sound/adx.h"
#include "sound/lists.h"
#include "sound/sound.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/texture.h"

#ifdef NONMATCHING
/* the event's file checks, its message list and mirrors, the warp hole's
 * texture slot and the areas. this side's load_event checks each file by
 * them */
#include <stddef.h>
#include <string.h>

#include "event/file.h"
#include "event/message_list.h"
#include "event/mirror.h"
#include "shared/warp_hole.h"
#include "system/areas.h"
#endif

/* the scan looks for the file of each event below MAX_EVENTS */
#define MAX_EVENTS 800
/* evmesH holds the messages of the events below FIRST_DARK_EVENT, evmesD
 * those below FIRST_LAST_EVENT, and evmesL the rest */
#define FIRST_DARK_EVENT 100
#define FIRST_LAST_EVENT 200
/* the hero and dark stories' endings, and the second ending. once either
 * story's ending has been seen, the other's gives way to SECOND_ENDING.
 * event_load_step makes the change in sequence_events at each call */
#define HERO_ENDING 429
#define DARK_ENDING 532
#define SECOND_ENDING 430
/* the movie over which the hero story draws Tails' plane */
#define PLANE_MOVIE 409
/* the event besides EVENT_STAFF_ROLL whose scene is an uncompressed
 * e%04d.bin */
#define BIN_SCENE_EVENT 203
/* the debug menu lists events from (MENU_X, MENU_Y), in columns of MENU_ROWS
 * events and MENU_COLUMN_WIDTH characters */
#define MENU_X 3
#define MENU_Y 3
#define MENU_ROWS 25
#define MENU_COLUMN_WIDTH 7
#define NEW_COLOR 0xffffff00
#define COMPLETE_COLOR 0xff00ff00
#define INCOMPLETE_COLOR 0xff888888
#define WHITE 0xffffffff
/* where the debug menu prints its memory sizes and its legend of the colors */
#define STATUS_X 28 /* the sizes' column */
#define HEAP_Y 23 /* the heap's label, above its two sizes */
#define TEXTURE_Y 17 /* the texture memory's label, above its three sizes */
#define LEGEND_X 23
#define LEGEND_Y 13 /* the legend's first line */
/* the frames SHOW_LOADING waits before the one that starts the load */
#define LOADING_DELAY 2
/* game_bits counts an event's views, up to MAX_VIEWS */
#define MAX_VIEWS 200

enum loader_state { SCAN_DISC, SELECT_EVENT, SHOW_LOADING, LOAD_FILES };

/* the events the debug menu shows as new and as complete, each list ending
 * at NO_EVENT */
static Sint32 new_events[] = {NO_EVENT};
/* clang-format off */
static Sint32 complete_events[] = {
    0, 2, 3, 4, 5, 6, 11, 14, 15, 16, 17, 19, 21, 22, 24, 25, 26, 27, 28,
    100, 101, 102, 103, 105, 106, 107, 109, 111, 112, 113, 116, 118, 119,
    120, 122, 123, 124, 126, 127, 128, 129, 130, 131,
    200, 201, 203, 204, 205, 206, 208, 210,
    400, 409, 411, 420, 429, 602, 609, NO_EVENT};
/* clang-format on */
static enum loader_state state = SCAN_DISC;
/* whether the disc holds each event's file */
static Sint8 on_disc[MAX_EVENTS];
static int cursor;
static int loading_frames;
static int selected;
#ifdef NONMATCHING
/* the views this side records once the event plays. the matching side counts
 * them as it shows the loading screen */
static int pending_views;
#endif

static Bool is_new(int event);
static Bool is_complete(int event);
static void load_sound_bank(int event);
static Sint32 first_free_cue(struct event_tracks *tracks);
static void release_empty_cue(struct event_tracks *tracks, Sint32 index);

#ifdef NONMATCHING
/* whether each table the scene file names lies within its bytes, with
 * textures entries in texture_sizes */
static Bool scene_directory(const struct event_scene_file *file, Sint32 bytes,
                            Uint32 textures)
{
    Sint32 i;
    const struct event_scene *scene;

    if (!event_file_records(file, bytes, file, 1, sizeof(*file)) ||
        file->scene_count < 1 ||
        (Uint32)file->scene_count >= sizeof(event_script_work->starts) /
                                         sizeof(event_script_work->starts[0]) ||
        !event_file_records(file, bytes, file->scenes,
                            (Uint32)file->scene_count + 1,
                            sizeof(*file->scenes)) ||
        !event_file_records(file, bytes, file->texture_sizes, textures,
                            sizeof(*file->texture_sizes)) ||
        !event_file_records(file, bytes, file->slot_objects, EVENT_SLOTS,
                            sizeof(*file->slot_objects)) ||
        !event_file_records(file, bytes, file->upgrades,
                            EVENT_UPGRADE_LISTS * EVENT_UPGRADES,
                            sizeof(*file->upgrades)) ||
        !event_file_records(file, bytes, file->attachments, EVENT_ATTACHMENTS,
                            sizeof(*file->attachments)) ||
        !event_file_records(file, bytes, file->tails, 1,
                            sizeof(*file->tails))) {
        return FALSE;
    }
    if (!event_file_records(file, bytes, file->mirrors, 1,
                            sizeof(*file->mirrors)) ||
        (Uint32)file->mirrors->count > MIRRORS ||
        (file->mirrors->count &&
         !event_file_records(file, bytes, file->mirrors->quad,
                             (Uint32)file->mirrors->count,
                             sizeof(*file->mirrors->quad)))) {
        return FALSE;
    }
    for (i = 0; i <= file->scene_count; i++) {
        scene = &file->scenes[i];
        /* only the scenes after scene 0 have a Big. a count of 0 names no
         * motions */
        if (i != 0 && (!event_file_records(file, bytes, scene->big, 1,
                                           sizeof(*scene->big)) ||
                       scene->big->count < 0 ||
                       (scene->big->count &&
                        !event_file_records(file, bytes, scene->big->motions,
                                            (Uint32)scene->big->count,
                                            sizeof(*scene->big->motions))))) {
            return FALSE;
        }
        if (scene->entity_count < 0 || scene->camera_count < 0 ||
            scene->motion_count < 0 ||
            (scene->entity_count &&
             !event_file_records(file, bytes, scene->entities,
                                 (Uint32)scene->entity_count,
                                 sizeof(*scene->entities))) ||
            (scene->camera_count &&
             !event_file_records(file, bytes, scene->cameras,
                                 (Uint32)scene->camera_count,
                                 sizeof(*scene->cameras))) ||
            (scene->motion_count &&
             !event_file_records(file, bytes, scene->motions,
                                 (Uint32)scene->motion_count,
                                 sizeof(*scene->motions)))) {
            return FALSE;
        }
    }
    return TRUE;
}

static void language_track(char *name, int event, int language)
{
    if (language) {
        sprintf(name, "e%04d_%1d.prs", event, language);
    } else {
        sprintf(name, "e%04d_j.prs", event);
    }
}

static Bool read_tracks(int event)
{
    char name[32];
    const Sint32 language_bytes = offsetof(struct event_tracks, switches);
    const struct event_tracks *alternate =
        (const struct event_tracks *)EVENT_SCENE_AREA;
    Sint32 i;
    Sint32 slot;

    memset(EVENT_TRACK_AREA, 0, EVENT_TRACK_BYTES);
    sprintf(name, "e%04d_0.prs", event);
    if (loader_read_prs_checked(name, EVENT_TRACK_AREA, EVENT_TRACK_BYTES) !=
        sizeof(*event_tracks)) {
        return FALSE;
    }
    language_track(name, event, game_state.text_language);
    if (loader_read_prs_checked(name, EVENT_TRACK_AREA,
                                (Uint32)language_bytes) != language_bytes) {
        return FALSE;
    }
    if (game_state.text_language == game_state.voice_language) {
        return TRUE;
    }
    language_track(name, event, game_state.voice_language);
    if (loader_read_prs_checked(name, EVENT_SCENE_AREA,
                                (Uint32)language_bytes) != language_bytes) {
        return FALSE;
    }
    for (i = 0; i < EVENT_SOUND_CUES; i++) {
        if (event_tracks->sounds[i].voice >= 0) {
            event_tracks->sounds[i].voice = -1;
            release_empty_cue(event_tracks, i);
        }
    }
    for (i = 0; i < EVENT_SOUND_CUES; i++) {
        if (alternate->sounds[i].voice >= 0) {
            slot = first_free_cue(event_tracks);
            event_tracks->sounds[slot].voice = alternate->sounds[i].voice;
            event_tracks->sounds[slot].frame = alternate->sounds[i].frame;
        }
    }
    return TRUE;
}

static Sint32 read_scene(int event)
{
    char name[32];

    if (event == BIN_SCENE_EVENT || event == EVENT_STAFF_ROLL) {
        sprintf(name, "e%04d.bin", event);
        return loader_read_file_checked(name, EVENT_SCENE_AREA,
                                        EVENT_SCENE_BYTES);
    }
    sprintf(name, "e%04d.prs", event);
    return loader_read_prs_checked(name, EVENT_SCENE_AREA, EVENT_SCENE_BYTES);
}

/* go up from EVENT to the game's directory, /SONIC2. where ".." fails, it
 * changes to /SONIC2 by its full path. it returns whether ".." worked */
static Bool leave_event_directory(void)
{
    if (gdFsChangeDir("..") == GDD_ERR_OK) {
        return TRUE;
    }
    gdFsChangeDir("/SONIC2");
    return FALSE;
}

/* load the event's files from EVENT, then leave EVENT. it returns FALSE
 * where a file fails, having released the textures it loaded */
static Bool load_event(int event)
{
    char name[32];
    Sint32 bytes;
    NJS_TEXLIST *list = NULL;
    const NJS_TEXNAME *warp_slot;
    struct message_list *messages;
    char bank = event < FIRST_DARK_EVENT   ? 'H'
                : event < FIRST_LAST_EVENT ? 'D'
                                           : 'L';

    njGarbageTexture(texmemlist, TEXTURE_COUNT);
    sprintf(name, "evmes%c%1d.prs", bank, game_state.text_language);
    bytes =
        loader_read_prs_checked(name, EVENT_MESSAGE_AREA, EVENT_MESSAGE_BYTES);
    if (bytes <= 0 ||
        !event_message_list(EVENT_MESSAGE_AREA, (Uint32)bytes, event,
                            EVENT_LINES, &messages) ||
        !read_tracks(event)) {
        goto fail;
    }
    sprintf(name, "e%04dtexlist.prs", event);
    bytes =
        loader_read_prs_checked(name, EVENT_TEXLIST_AREA, EVENT_TEXLIST_BYTES);
    if (bytes < (Sint32)sizeof(*EVENT_TEXLIST_AREA) ||
        !event_file_texlist(EVENT_TEXLIST_AREA, bytes, *EVENT_TEXLIST_AREA)) {
        goto fail;
    }
    sprintf(name, "e%04dtexture.prs", event);
    if (!texture_load_pvm_in(name, *EVENT_TEXLIST_AREA, EVENT_SCENE_AREA,
                             EVENT_SCENE_BYTES)) {
        goto fail;
    }
    list = *EVENT_TEXLIST_AREA;
    if (!scene_directory(event_scene_file, read_scene(event),
                         list->nbTexture)) {
        goto fail;
    }
    bytes = loader_read_file_checked("eveff.pvm", WORK_AREA,
                                     WORK_AREA_END - WORK_AREA);
    if (bytes <= 0 || !texture_load_pvm_memory(WORK_AREA, (Uint32)bytes,
                                               &event_effect_texlist)) {
        goto fail;
    }
    event_fog_load(event, event_scene_file->scene_count);
    /* the warp hole's particle borrows its texture from OBJTEX_COMMON, which
     * the event doesn't load. its slot may stay empty, and the particle then
     * draws nothing */
    warp_slot = warp_hole_texlist.textures;
    if (!leave_event_directory() ||
        !texture_load_shared_with_optional(event_pvms, event_texture_groups,
                                           &warp_slot, 1)) {
        texture_discard_pvm(&event_effect_texlist);
        texture_discard_pvm(list);
        return FALSE;
    }
    event_scene_file->texlist = list;
    event_scenes = event_scene_file->scenes;
    event_messages = messages;
    return TRUE;

fail:
    if (list != NULL) {
        texture_discard_pvm(list);
    }
    leave_event_directory();
    return FALSE;
}

static int failed_load(void)
{
    event_messages = NULL;
    event_scenes = NULL;
    *EVENT_TEXLIST_AREA = NULL;
    pending_views = 0;
    state = SCAN_DISC;
    return EVENT_LOAD_FAILED;
}

static void record_views(void)
{
    if (selected >= 0 && selected < GAME_BITS) {
        int views = game_bits[selected] + pending_views;
        game_bits[selected] = views > MAX_VIEWS ? MAX_VIEWS : views;
    }
    pending_views = 0;
    if (game_flags[FLAG_SEQUENCE]) {
        event_sequence_index++;
    }
}
#endif

int event_load_step(void)
{
    int count;
    Uint32 free_bytes;
    Uint32 largest_block;
    char filename[32];
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint32 unused[3];
#ifdef NONMATCHING
    /* whether this step works in EVENT, which this side then leaves */
    Bool needs_dir;
#else
    NJS_TEXLIST **texlist;
#endif
    int i;

#ifdef NONMATCHING
    /* this side fails the load for an index past sequence_events, or where
     * EVENT won't open */
    needs_dir = state == SCAN_DISC || state == LOAD_FILES ||
                (state == SHOW_LOADING && selected >= EVENT_FIRST_MOVIE);
    if (event_sequence_index < 0 || event_sequence_index >= SEQUENCE_EVENTS) {
        return failed_load();
    }
    if (needs_dir && gdFsChangeDir("EVENT") != GDD_ERR_OK) {
        gdFsChangeDir("/SONIC2");
        return failed_load();
    }
#else
    gdFsChangeDir("EVENT");
#endif
    (void)unused;
    if (sequence_events[event_sequence_index] == HERO_ENDING &&
        game_bits[DARK_ENDING]) {
        sequence_events[event_sequence_index] = SECOND_ENDING;
    } else if (sequence_events[event_sequence_index] == DARK_ENDING &&
               game_bits[HERO_ENDING]) {
        sequence_events[event_sequence_index] = SECOND_ENDING;
    }
    switch (state) {
    case SCAN_DISC:
        for (i = 0; i < EVENT_FIRST_MOVIE; i++) {
            GDFS file;
            sprintf(filename, "e%04d.prs", i);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((file = gdFsOpen(filename, NULL)) != NULL) {
                on_disc[i] = TRUE;
                gdFsClose(file);
            } else {
                on_disc[i] = FALSE;
            }
        }
        cursor = 0;
        state = SELECT_EVENT;
        for (i = EVENT_FIRST_MOVIE; i < MAX_EVENTS; i++) {
            GDFS file;
            sprintf(filename, "e%04d.sfd", i);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((file = gdFsOpen(filename, NULL)) != NULL) {
                on_disc[i] = TRUE;
                gdFsClose(file);
            } else {
                on_disc[i] = FALSE;
            }
        }
        /* the scan doesn't look for the two .bin scenes */
        on_disc[BIN_SCENE_EVENT] = TRUE;
        on_disc[EVENT_STAFF_ROLL] = TRUE;
        /* fall through */
    case SELECT_EVENT:
        if (game_flags[FLAG_SEQUENCE]) {
#ifdef NONMATCHING
            /* this side fails the load for an event past MAX_EVENTS. it steps
             * the index only once the event has played */
            selected = sequence_events[event_sequence_index];
            if (selected < 0 || selected >= MAX_EVENTS) {
                if (needs_dir) {
                    leave_event_directory();
                }
                return failed_load();
            }
#else
            selected = sequence_events[event_sequence_index++];
#endif
            state = SHOW_LOADING;
            break;
        }
        count = 0;
        njPrintC(NJM_LOCATION(1, 2), "SELECT EVENT & PUSH A");
        for (i = 0; i < MAX_EVENTS; i++) {
            if (on_disc[i]) {
                sprintf(filename, "e%04d", i);
                if (is_new(i)) {
                    njPrintColor(NEW_COLOR);
                } else if (is_complete(i)) {
                    njPrintColor(COMPLETE_COLOR);
                } else {
                    njPrintColor(INCOMPLETE_COLOR);
                }
                njPrintC(NJM_LOCATION((count / MENU_ROWS * MENU_COLUMN_WIDTH) +
                                          MENU_X,
                                      (count % MENU_ROWS) + MENU_Y),
                         filename);
                njPrintColor(WHITE);
                if (count == cursor) {
                    selected = i;
                }
                count++;
            }
        }
        if (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KU) {
            cursor--;
        }
        if (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KD) {
            cursor++;
        }
        if (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KL) {
            cursor -= MENU_ROWS;
        }
        if (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KR) {
            cursor += MENU_ROWS;
        }
        if (cursor < 0) {
            cursor = 0;
        }
        if (cursor >= count) {
            cursor = count - 1;
        }
        syMallocStat(&free_bytes, &largest_block);
        njPrintC(NJM_LOCATION(STATUS_X, HEAP_Y), "HEAP");
        njPrintH(NJM_LOCATION(STATUS_X, HEAP_Y + 1), free_bytes, 8);
        njPrintH(NJM_LOCATION(STATUS_X, HEAP_Y + 2), largest_block, 8);
        njPrint(NJM_LOCATION(STATUS_X, TEXTURE_Y), "TEXTURE");
        njPrintH(NJM_LOCATION(STATUS_X, TEXTURE_Y + 1),
                 njCalcTexture(NJD_TEXMEM_FREESIZE), 8);
        njPrintH(NJM_LOCATION(STATUS_X, TEXTURE_Y + 2),
                 njCalcTexture(NJD_TEXMEM_MAXBLOCK), 8);
        njPrintH(NJM_LOCATION(STATUS_X, TEXTURE_Y + 3),
                 njCalcTexture(NJD_TEXMEM_MAXSIZE), 8);
        njPrintColor(NEW_COLOR);
        njPrintC(NJM_LOCATION(LEGEND_X, LEGEND_Y), "NEW EVENT");
        njPrintColor(COMPLETE_COLOR);
        njPrintC(NJM_LOCATION(LEGEND_X, LEGEND_Y + 1), "COMPLETE EVENT");
        njPrintColor(INCOMPLETE_COLOR);
        njPrintC(NJM_LOCATION(LEGEND_X, LEGEND_Y + 2), "INCOMPLETE EVENT");
        njPrintColor(WHITE);
        njPrintC(NJM_LOCATION((cursor / MENU_ROWS * MENU_COLUMN_WIDTH) +
                                  (MENU_X - 1),
                              (cursor % MENU_ROWS) + MENU_Y),
                 "*");
        if (pad_held[EVENT_PLAYER_PAD] & PDD_DGT_TA) {
            state = SHOW_LOADING;
            loading_frames = 0;
        }
        break;
    case SHOW_LOADING:
#ifdef NONMATCHING
        /* this side holds the view until the event plays, and counts it once
         * then. the matching side counts it into game_bits by selected
         * unbounded */
        if (pending_views < MAX_VIEWS) {
            pending_views++;
        }
#else
        /* game_bits holds the first GAME_BITS events, fewer than MAX_EVENTS.
         * the disc holds no event's file past them */
        game_bits[selected]++;
        if (game_bits[selected] > MAX_VIEWS) {
            game_bits[selected] = MAX_VIEWS;
        }
#endif
        if (selected >= EVENT_FIRST_MOVIE) {
            struct event_movie_request movie = {NULL, 0, EVENT_MOVIE_FADE_OUT};
            sprintf(filename, "e%04d.sfd", selected);
            movie.name = filename;
            if (game_flags[FLAG_SEQUENCE] == SEQUENCE_HERO &&
                selected == PLANE_MOVIE) {
                event_movie_draw_plane = TRUE;
            } else {
                event_movie_draw_plane = FALSE;
            }
#ifdef NONMATCHING
            /* this side fails the load where the movie can't play or EVENT
             * won't close. it records the views once it has */
            if (!event_movie_play(&movie)) {
                leave_event_directory();
                return failed_load();
            }
            if (!leave_event_directory()) {
                return failed_load();
            }
            record_views();
#else
            event_movie_play(&movie);
            gdFsChangeDir("..");
#endif
            state = SCAN_DISC;
            return selected;
            /* @hack redundant: a break after the return, which never runs */
            break;
        }
        loading_frames++;
        if (game_flags[FLAG_SEQUENCE] == SEQUENCE_NONE) {
            njPrint(NJM_LOCATION(7, 15), "Now Loading E%04d ...", selected);
        }
        if (loading_frames > LOADING_DELAY) {
            state = LOAD_FILES;
        }
        break;
    case LOAD_FILES:
#ifdef NONMATCHING
        /* this side loads the event by load_event, which checks each file, and
         * fails the load where it can't */
        if (!load_event(selected)) {
            return failed_load();
        }
        record_views();
        event_movie_start_scenes();
#else
        njGarbageTexture(texmemlist, TEXTURE_COUNT);
        if (selected < FIRST_DARK_EVENT) {
            sprintf(filename, "evmesH%1d.prs", game_state.text_language);
        } else if (selected < FIRST_LAST_EVENT) {
            sprintf(filename, "evmesD%1d.prs", game_state.text_language);
        } else {
            sprintf(filename, "evmesL%1d.prs", game_state.text_language);
        }
        loader_read_prs(filename, EVENT_MESSAGE_AREA);
        /* @hack spelling: the track area cleared a word at a time, counting
         * down, where memset would do */
        {
            Uint32 *word = EVENT_TRACK_AREA;
            Sint32 left;
            for (left = EVENT_TRACK_BYTES / sizeof(*word); left != 0; left--) {
                *word++ = 0;
            }
        }
        sprintf(filename, "e%04d_0.prs", selected);
        loader_read_prs(filename, EVENT_TRACK_AREA);
        if (game_state.text_language) {
            sprintf(filename, "e%04d_%1d.prs", selected,
                    game_state.text_language);
            loader_read_prs(filename, EVENT_TRACK_AREA);
        } else {
            sprintf(filename, "e%04d_j.prs", selected);
            loader_read_prs(filename, EVENT_TRACK_AREA);
        }
        if (game_state.text_language != game_state.voice_language) {
            struct event_tracks *tracks = event_tracks;
            const struct event_tracks *alternate =
                (const struct event_tracks *)EVENT_SCENE_AREA;
            Sint32 free_slot;
            if (game_state.voice_language) {
                sprintf(filename, "e%04d_%1d.prs", selected,
                        game_state.voice_language);
            } else {
                sprintf(filename, "e%04d_j.prs", selected);
            }
            loader_read_prs(filename, EVENT_SCENE_AREA);
            for (i = 0; i < EVENT_SOUND_CUES; i++) {
                if (tracks->sounds[i].voice >= 0) {
                    tracks->sounds[i].voice = -1;
                    release_empty_cue(tracks, i);
                }
            }
            for (i = 0; i < EVENT_SOUND_CUES; i++) {
                if (alternate->sounds[i].voice >= 0) {
                    free_slot = first_free_cue(tracks);
                    tracks->sounds[free_slot].voice =
                        alternate->sounds[i].voice;
                    tracks->sounds[free_slot].frame =
                        alternate->sounds[i].frame;
                }
            }
        }
        sprintf(filename, "e%04dtexlist.prs", selected);
        texlist = EVENT_TEXLIST_AREA;
        loader_read_prs(filename, texlist);
        sprintf(filename, "e%04dtexture.prs", selected);
        texture_load_pvm_in(filename, *texlist, EVENT_SCENE_AREA,
                            EVENT_SCENE_BYTES);
        if (selected == BIN_SCENE_EVENT) {
            sprintf(filename, "e0203.bin");
            loader_read_file(filename, EVENT_SCENE_AREA);
        } else if (selected == EVENT_STAFF_ROLL) {
            sprintf(filename, "e0210.bin");
            loader_read_file(filename, EVENT_SCENE_AREA);
        } else {
            sprintf(filename, "e%04d.prs", selected);
            loader_read_prs(filename, EVENT_SCENE_AREA);
        }
        event_scenes = event_scene_file->scenes;
        /* @hack spelling: the scene area cast at the use, where
         * event_scene_file would do */
        ((struct event_scene_file *)EVENT_SCENE_AREA)->texlist = *texlist;
        /* @hack redundant: an if with no body on the load's result */
        if (njLoadTexturePvmFile("eveff.pvm", &event_effect_texlist) == -1) {
        }
        event_fog_load(selected, event_scene_file->scene_count);
        gdFsChangeDir("..");
        event_movie_start_scenes();
        texture_load_shared(event_pvms, event_texture_groups);
#endif
        load_sound_bank(selected);
        state = SCAN_DISC;
        if (selected == EVENT_STAFF_ROLL) {
            staff_roll_load(SEQUENCE_NONE);
        }
        event_script_work->loaded_language = game_state.text_language;
        music_fade_in(VOLUME_FULL);
        return selected;
    }
#ifdef NONMATCHING
    /* this side fails the load where EVENT won't close */
    if (needs_dir && !leave_event_directory()) {
        return failed_load();
    }
#endif
    return NO_EVENT;
}

static Bool is_new(int event)
{
    int i;
    for (i = 0; new_events[i] >= 0; i++) {
        if (new_events[i] == event) {
            return TRUE;
        }
    }
    return FALSE;
}

static Bool is_complete(int event)
{
    int i;
    for (i = 0; complete_events[i] >= 0; i++) {
        if (complete_events[i] == event) {
            return TRUE;
        }
    }
    return FALSE;
}

static const struct sound sounds[1] = {
#include "event/loader/sounds.inc"
};
static const struct sound_list sound_bank = {
#include "event/loader/sound_bank.inc"
};

static void load_sound_bank(int event)
{
    char filename[32];
    sprintf(filename, "e%04d.mlt", event);
#ifdef NONMATCHING
    /* where EVENT can't be entered, it loads no bank */
    if (gdFsChangeDir("EVENT") != GDD_ERR_OK) {
        gdFsChangeDir("/SONIC2");
        return;
    }
    sound_load_event(filename, &sound_bank);
    leave_event_directory();
#else
    gdFsChangeDir("EVENT");
    sound_load_event(filename, &sound_bank);
    gdFsChangeDir("..");
#endif
}

/* the first cue whose frame is 0, or cue 0 where none is */
static Sint32 first_free_cue(struct event_tracks *tracks)
{
    Sint32 i;
    for (i = 0; i < EVENT_SOUND_CUES; i++) {
        if (tracks->sounds[i].frame == 0) {
            return i;
        }
    }
    return 0;
}

/* free the cue at index where it plays nothing and starts no staff roll */
static void release_empty_cue(struct event_tracks *tracks, Sint32 index)
{
    if (tracks->sounds[index].sound < 0 &&
        tracks->sounds[index].staff_roll_length < 0 &&
        tracks->sounds[index].voice < 0 &&
        tracks->sounds[index].music[0] == '\0' &&
        tracks->sounds[index].jingle[0] == '\0') {
        tracks->sounds[index].frame = 0;
    }
}
