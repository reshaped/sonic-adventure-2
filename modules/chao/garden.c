/* the Chao world's stage, which reads and writes the garden file and plays
 * one area at a time */
#include <ninja.h>
#include <sg_xpt.h>
#ifdef NONMATCHING
/* uintptr_t and the areas, by which this side's garden_load_area bounds an
 * area's load. load_garden_textures reads into WORK_AREA, and stops the game
 * by port_stop */
#include <stdint.h>
#include "engine/dc/crash.h"
#include "system/areas.h"
#endif

#include "camera/camera.h"
#include "chao/palette.h"
#include "chao/garden.h"
#include "chao/garden_state.h"
#include "chao/icon.h"
#include "chao/model.h"
#include "chao/garden_save.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/sound.h"
#include "chao/spawn_grid.h"
#include "chao/world.h"
#include "draw/fade.h"
#include "effect/particle.h"
#include "main.h"
#include "player/action.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/water_ripple.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/task.h"
#include "system/texture.h"

/* the slot each area loads into, above CHAO.PRS and the model and motion
 * archives. it is addressed through 0x8c000000, and the pointers inside an
 * area address the same bytes through 0x0c000000 */
#ifdef NONMATCHING
/* the slot by its address, the plain form the hack tag names. this side's
 * objects don't define chao_area_slot, which only symbols.txt places.
 * loader_prepare_stage loads the two archives before chao_init runs, so this
 * side names neither */
#define CHAO_AREA_SLOT ((struct chao_area_head *)0x8c9c0000)
#else
/* where chao_init loads the two archives, also through 0x8c000000. the
 * pointers inside them address the same bytes through 0x0c000000 */
#define CHAO_MODEL_ARCHIVE ((void *)0x8c698700)
#define CHAO_MOTION_ARCHIVE ((void *)0x8c8ba800)
/* @hack type: the slot as an external object, where its address cast to
 * struct chao_area_head * would do */
extern struct chao_area_head chao_area_slot;
#define CHAO_AREA_SLOT (&chao_area_slot)
#endif
/* the steps after which a wait for the garden file's write to the memory
 * card gives up */
#define CARD_TIMEOUT (2 * 60 * STEPS_PER_SECOND)
#define CONTROL_TASK_LEVEL 0
#define CARD_TASK_LEVEL 2
#define AREA_TASK_LEVEL 3
#define GARDEN_FONT_SIZE 12
#define SHADOW_GARDEN 115
#define SHADOW_DEFAULT 80
#define SOUND_CHANGE_AREA 0x1032
#define SOUND_LEAVE_GARDEN 0x8012
/* a color channel at its most. the fade to white takes it for each */
#define FULL 0xff
/* player 0's height in the gardens. it is cut to GARDEN_CEILING, and set to
 * GARDEN_RETURN_HEIGHT once it falls below GARDEN_FALL_LIMIT */
#define GARDEN_CEILING 110.0F
#define GARDEN_FALL_LIMIT (-10.0F)
#define GARDEN_RETURN_HEIGHT 0.5F

/* chao_step's work->mode. it has no case for 3 to 6 */
enum garden_mode {
    GARDEN_START = 0,
    GARDEN_WAIT_FIRST_SAVE = 1,
    GARDEN_LOAD_AREA = 2,
    GARDEN_PLAY = 7,
    GARDEN_PLAY_NO_COUNT = 8,
    GARDEN_FADE_OUT = 9,
    GARDEN_SAVE_ON_LEAVE = 10,
    GARDEN_WAIT_LAST_SAVE = 11,
    GARDEN_END = 12
};

/* the player control task's work->mode. each area's load starts the task at
 * CONTROL_PLACE. it places player 0 at that step and the next */
enum player_control_mode { CONTROL_PLACE = 0, CONTROL_PLACE_AGAIN = 1 };

/* @hack redundant: external linkage, which no other source needs, where
 * static would do */
void garden_request_area(int area); /* NOLINT(misc-use-internal-linkage) */
static void garden_player_control_start(void);

static struct task *garden_task = NULL;
#ifdef NONMATCHING
/* the area whose program the slot holds, or CHAO_AREA_NONE until one loads
 * whole. it is never CHAO_AREA_RACE_ENTRY, whose program is
 * CHAO_AREA_ENTRANCE's */
static int installed_area = CHAO_AREA_NONE;
#endif

Bool chao_take_bad_file(void)
{
    Bool invalid = garden_bad_file;

    garden_bad_file = FALSE;
    return invalid;
}

#ifdef NONMATCHING
/* load the eight lists the matching side's chao_init loads by
 * texture_load_pvm, each from its PVM file through WORK_AREA. the garden
 * can't go on without them: a read that fails stops the game, as loader_stop
 * says, and so does a PVM that won't load, as PVM with the file's name */
static void load_garden_textures(void)
{
    static const struct {
        const char *file;
        NJS_TEXLIST *texlist;
    } pvms[] = {
        {"AL_ICON.PRS", &chao_icon_texlist},
        {"AL_EYE.PRS", &chao_eye_texlist},
        {"AL_MOUTH.PRS", &chao_mouth_texlist},
        {"AL_MINI_PARTS_TEX.PRS", &chao_mini_parts_texlist},
        {"AL_BODY.PRS", &chao_body_texlist},
        {"AL_jewel.PRS", &chao_jewel_texlist},
        {"AL_COMMON_TEX.PRS", &chao_common_texlist},
        {"AL_ENV_TEX.PRS", &chao_env_texlist},
    };
    unsigned int index;
    Sint32 size;

    for (index = 0; index < sizeof(pvms) / sizeof(pvms[0]); index++) {
        size = loader_read_prs_buffer(pvms[index].file, WORK_AREA,
                                      WORK_AREA_END - WORK_AREA);
        if (size < 0) {
            loader_stop(pvms[index].file);
        }
        if (!texture_load_pvm_memory(WORK_AREA, (Uint32)size,
                                     pvms[index].texlist)) {
            port_stop("PVM", pvms[index].file, 0);
        }
    }
}
#endif

void chao_init(void)
{
    Bool finished;
    Bool empty;
    Bool ready;
    /* @hack spelling: state, which holds garden_file_state() for the switch,
     * where a switch on the call would do. under NONMATCHING, the case of
     * GARDEN_FILE_INVALID reads it too */
    int state;

#ifdef NONMATCHING
    /* this side starts with no area installed. its loader_prepare_stage has
     * read the two archives, and load_garden_textures stops the game where a
     * texture won't load */
    installed_area = CHAO_AREA_NONE;
    load_garden_textures();
#else
    loader_read_prs("Chao_model.PRS", CHAO_MODEL_ARCHIVE);
    loader_read_prs("Chao_motion.PRS", CHAO_MOTION_ARCHIVE);
    texture_load_pvm("AL_ICON", &chao_icon_texlist);
    texture_load_pvm("AL_EYE", &chao_eye_texlist);
    texture_load_pvm("AL_MOUTH", &chao_mouth_texlist);
    texture_load_pvm("AL_MINI_PARTS_TEX", &chao_mini_parts_texlist);
    texture_load_pvm("AL_BODY", &chao_body_texlist);
    texture_load_pvm("AL_jewel", &chao_jewel_texlist);
    texture_load_pvm("AL_COMMON_TEX", &chao_common_texlist);
    texture_load_pvm("AL_ENV_TEX", &chao_env_texlist);
#endif
    njGarbageTexture(texmemlist, TEXTURE_COUNT);
    garden_load_palettes();
    particle_core_start();
    if (player_hooks.ripple == NULL) {
        player_hooks.ripple = ripple_create;
    }
    if (player_hooks.unk_04 == NULL) {
        player_hooks.unk_04 = dirt_create;
    }
    njPrintSize(GARDEN_FONT_SIZE);
    njSetCheapShadowMode(SHADOW_GARDEN);
    garden_sound_load();
    game_state.rings_shown = game_state.lives_shown = game_state.hud_on = 0;
    game_state.idle_dim = 0;
    if (game_state.chao_race_menu != CHAO_RACE_MENU_NONE) {
        garden_request_area(CHAO_AREA_RACE_ENTRY);
    } else {
        garden_request_area(CHAO_AREA_LOBBY);
    }
    loader_read_camera_files();
    camera_active_count = 1;
    stage_hooks.open_message = NULL;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = chao_current_area;
    stage_hooks.save = garden_file_write;
    save_data.played[stage_character(0)] = 1;
    garden_needs_first_save = FALSE;
    garden_bad_file = FALSE;
    garden_file_loaded = FALSE;
    garden_file_read();
    /* @hack redundant: ready and its test, which always passes */
    ready = TRUE;
    if (ready) {
        for (;;) {
            state = garden_file_state();
            finished = FALSE;
            empty = TRUE;
            switch (state) {
            case GARDEN_FILE_BUSY:
                break;
            case GARDEN_FILE_OK:
                garden_file_unpack();
                if (garden_chao_count(CHAO_AREA_NEUTRAL) > 0) {
                    empty = FALSE;
                } else {
                    if (garden_flag_test(GARDEN_HERO_OPEN)) {
                        if (garden_chao_count(CHAO_AREA_HERO) > 0) {
                            empty = FALSE;
                        }
                    }
                    if (garden_flag_test(GARDEN_DARK_OPEN)) {
                        if (garden_chao_count(CHAO_AREA_DARK) > 0) {
                            empty = FALSE;
                        }
                    }
                }
                if (empty) {
                    garden_seed_eggs();
                }
                finished = TRUE;
                garden_file_loaded = TRUE;
                break;
            case GARDEN_FILE_ABSENT:
            case GARDEN_FILE_ERROR:
#ifdef NONMATCHING
            /* this side settles each failed read by garden_file_recover, as
             * garden_save.h says, and marks an invalid file. the matching side
             * starts a new garden for each */
            case GARDEN_FILE_INVALID:
                garden_bad_file = state == GARDEN_FILE_INVALID;
                garden_needs_first_save = garden_file_recover(state);
                finished = TRUE;
                break;
#else
                garden_file_init();
                garden_needs_first_save = TRUE;
                finished = TRUE;
                break;
            case GARDEN_FILE_INVALID:
                garden_bad_file = TRUE;
                garden_file_init();
                garden_needs_first_save = TRUE;
                finished = TRUE;
                break;
#endif
            }
            if (finished) {
                garden_race_restore();
                break;
                /* @hack redundant: an else after the break, where its two
                 * calls after the if would do */
            } else {
                task_step_level(CARD_TASK_LEVEL);
                njWaitVSync();
            }
        }
    }
}

void chao_exit(void)
{
    int frames = 0;
    Bool finished;
    /* @hack spelling: state, which holds garden_file_state() for the switch,
     * where a switch on the call would do */
    int state;

    if (player_hooks.unk_04 == dirt_create) {
        player_hooks.unk_04 = NULL;
    }
    if (player_hooks.ripple == ripple_create) {
        player_hooks.ripple = NULL;
    }
    particle_core_stop();
    njSetCheapShadowMode(SHADOW_DEFAULT);
    sound_load_player_sounds();
    game_state.hud_on = 1;
    game_state.lives_shown = 1;
    game_state.rings_shown = 1;
    game_state.split_screen = 0;
    game_state.four_cameras = 0;
    game_state.idle_dim = 1;
    game_state.split_layout = 0;
    garden_task = NULL;
    if (garden_save_on_exit) {
        njSetBackColor(0, 0, 0);
        garden_file_write();
        for (;;) {
            state = garden_file_state();
            finished = FALSE;
            switch (state) {
            case GARDEN_FILE_BUSY:
                if (frames++ > CARD_TIMEOUT) {
                    finished = TRUE;
                }
                break;
            /* @hack spelling: a case of its own for GARDEN_FILE_OK, where its
             * label on the case of GARDEN_FILE_ABSENT and GARDEN_FILE_ERROR
             * would do */
            case GARDEN_FILE_OK:
                finished = TRUE;
                break;
            case GARDEN_FILE_ABSENT:
            case GARDEN_FILE_ERROR:
                finished = TRUE;
                break;
            }
            if (finished) {
                break;
            }
            task_step_level(CARD_TASK_LEVEL);
            njWaitVSync();
        }
    }
    game_state.card_warning = 0;
    game_state.card_state = CARD_IDLE;
}

#ifdef NONMATCHING
/* an area's program file, the name its struct chao_area_head holds, and the
 * address it must end before */
struct garden_area_file {
    const char *name;
    const char *tag;
    uintptr_t limit;
};

/* by enum chao_area, from CHAO_AREA_NEUTRAL. each limit is the lowest address
 * above the slot that another load fills. the race's head and the entrance's
 * both hold ALSTRACE */
static const struct garden_area_file area_files[] = {
    {"ChaoStgNeut.PRS", "ALSTNEUT", (uintptr_t)MESSAGE_AREA},
    {"ChaoStgHero.PRS", "ALSTHERO", (uintptr_t)MESSAGE_AREA},
    {"ChaoStgDark.PRS", "ALSTDARK", (uintptr_t)MESSAGE_AREA},
    {"ChaoStgRace.PRS", "ALSTRACE", (uintptr_t)CHAO_RACE_LAND_AREA},
    {"ChaoStgEntrance.PRS", "ALSTRACE", (uintptr_t)MESSAGE_AREA},
    {"ChaoStgKinder.PRS", "ALSTKNDR", (uintptr_t)MESSAGE_AREA},
    {"ChaoStgLobby.PRS", "ALSTLOBY", (uintptr_t)CHAO_LOBBY_LAND_AREA},
};

static void garden_load_area(struct task *task, struct chao_area_head *area)
{
    struct task *child;
    const struct garden_area_file *file;
    int image =
        garden_next == CHAO_AREA_RACE_ENTRY ? CHAO_AREA_ENTRANCE : garden_next;
    Uint32 capacity;

    garden_task = NULL;
    if (image < CHAO_AREA_NEUTRAL || image > CHAO_AREA_LOBBY) {
        return;
    }
    if (image != installed_area) {
        file = &area_files[image - CHAO_AREA_NEUTRAL];
        /* MESSAGE_AREA is addressed through 0x0c000000. bit 31 puts the
         * limit in the slot's view, through 0x8c000000 */
        capacity =
            (Uint32)((file->limit | 0x80000000U) - (uintptr_t)CHAO_AREA_SLOT);
        installed_area = CHAO_AREA_NONE;
        if (!loader_install_chao_area(file->name, file->tag, area, capacity)) {
            return;
        }
        installed_area = image;
    }

    child = task_create(TASK_WORK, AREA_TASK_LEVEL, area->step,
                        "((AL_SUBPRG_HEADER *)_CHAO_SUBPRG)->al_subprgmanager");
    if (child == NULL) {
        return;
    }
    garden_player_control_start();
    if (garden_player_control == NULL) {
        task_destroy(child);
        return;
    }
    if (garden_current != garden_next) {
        garden_previous = garden_current;
        garden_current = garden_next;
    }
    particle_core_stop();
    garden_spawn_grid_init();
    garden_area_sound_load();
    particle_core_start();
    garden_task = task;
    task->work->mode = GARDEN_PLAY;
    task->work->submode = 0;
    task->work->timer = 0;
    task->work->tick = 0;
}
#else
/* @hack redundant: the parameter task, which nothing reads */
static void garden_load_area(struct task *task)
{
    struct chao_area_head *area;

    garden_current = garden_next;
    particle_core_stop();
    garden_spawn_grid_init();
    area = CHAO_AREA_SLOT;
    if (garden_current != garden_previous) {
        switch (garden_current) {
        case CHAO_AREA_NEUTRAL:
            loader_read_prs("ChaoStgNeut.PRS", area);
            break;
        case CHAO_AREA_HERO:
            loader_read_prs("ChaoStgHero.PRS", area);
            break;
        case CHAO_AREA_DARK:
            loader_read_prs("ChaoStgDark.PRS", area);
            break;
        case CHAO_AREA_RACE:
            loader_read_prs("ChaoStgRace.PRS", area);
            break;
        case CHAO_AREA_ENTRANCE:
        case CHAO_AREA_RACE_ENTRY:
            loader_read_prs("ChaoStgEntrance.PRS", area);
            break;
        case CHAO_AREA_KINDERGARTEN:
            loader_read_prs("ChaoStgKinder.PRS", area);
            break;
        case CHAO_AREA_LOBBY:
        default:
            loader_read_prs("ChaoStgLobby.PRS", area);
            break;
        }
    }
    task_create(TASK_WORK, AREA_TASK_LEVEL, area->step,
                "((AL_SUBPRG_HEADER *)_CHAO_SUBPRG)->al_subprgmanager");
    garden_area_sound_load();
    particle_core_start();
}
#endif

/* @hack redundant: the parameter task, which nothing reads */
static void garden_limit_player_height(struct task *task)
{
    NJS_POINT3 *pos;

    switch (garden_current) {
    case CHAO_AREA_NEUTRAL:
    case CHAO_AREA_HERO:
    case CHAO_AREA_DARK:
        pos = &player_work[0]->pos;
        if (pos->y > GARDEN_CEILING) {
            pos->y = GARDEN_CEILING;
        }
        if (pos->y < GARDEN_FALL_LIMIT) {
            pos->y = GARDEN_RETURN_HEIGHT;
        }
        break;
    }
}

void chao_step(struct task *task)
{
    struct task_work *work = task->work;
    Bool finished;
    /* @hack spelling: state, which holds garden_file_state() for each switch,
     * where a switch on the call would do */
    int state;

    switch (work->mode) {
    case GARDEN_START:
        if (garden_needs_first_save) {
            garden_file_write();
            player_pause(0);
            work->mode = GARDEN_WAIT_FIRST_SAVE;
            work->submode = 0;
            work->timer = 0;
            work->tick = 0;
            garden_needs_first_save = FALSE;
        } else {
            work->mode = GARDEN_LOAD_AREA;
            work->submode = 0;
            work->timer = 0;
            work->tick = 0;
        }
        break;
    case GARDEN_WAIT_FIRST_SAVE:
        state = garden_file_state();
        finished = FALSE;
        switch (state) {
        case GARDEN_FILE_BUSY:
            if (work->timer++ > CARD_TIMEOUT) {
                finished = TRUE;
            }
            break;
        case GARDEN_FILE_OK:
        case GARDEN_FILE_ABSENT:
        case GARDEN_FILE_ERROR:
            finished = TRUE;
            break;
        }
        if (!finished) {
            break;
        }
        player_resume(0);
        work->mode = GARDEN_LOAD_AREA;
        work->submode = 0;
        break;
    case GARDEN_LOAD_AREA:
#ifdef NONMATCHING
        /* this side's garden_load_area takes the slot. it sets the play mode
         * and the player control only where the area loads */
        garden_load_area(task, CHAO_AREA_SLOT);
#else
        garden_task = task;
        garden_load_area(task);
        work->mode = GARDEN_PLAY;
        work->submode = 0;
        work->timer = 0;
        work->tick = 0;
        garden_player_control_start();
#endif
        break;
    case GARDEN_PLAY:
        garden_save_on_exit = TRUE;
        garden_save_record()->frames++;
        /* fall through */
    case GARDEN_PLAY_NO_COUNT:
        garden_limit_player_height(task);
        break;
    case GARDEN_FADE_OUT:
        pause_disable();
        pad_disable();
        if (fade_to_color_step(FULL, FULL, FULL, FULL)) {
            work->mode = GARDEN_SAVE_ON_LEAVE;
            work->submode = 0;
            work->timer = 0;
            work->tick = 0;
        }
        break;
    case GARDEN_SAVE_ON_LEAVE:
        garden_file_write();
        work->mode = GARDEN_WAIT_LAST_SAVE;
        work->submode = 0;
        work->timer = 0;
        work->tick = 0;
        /* fall through */
    case GARDEN_WAIT_LAST_SAVE:
        state = garden_file_state();
        finished = FALSE;
        switch (state) {
        case GARDEN_FILE_BUSY:
            if (work->timer++ > CARD_TIMEOUT) {
                finished = TRUE;
            }
            break;
        case GARDEN_FILE_OK:
        case GARDEN_FILE_ABSENT:
        case GARDEN_FILE_ERROR:
            finished = TRUE;
            break;
        }
        if (!finished) {
            break;
        }
        work->mode = GARDEN_END;
        work->submode = 0;
        game_state.card_state = CARD_IDLE;
        break;
    case GARDEN_END:
        pad_enable();
        pause_enable();
        stage_end(STAGE_END_CLEAR);
        garden_save_on_exit = FALSE;
        break;
    }
}

void garden_request_area(int area)
{
#ifdef NONMATCHING
    /* this side sets garden_next even to the current area. it leaves
     * garden_previous to garden_load_area, which sets it as it changes the
     * area */
    garden_next = area;
#else
    if (garden_current != area) {
        garden_previous = garden_current;
        garden_next = area;
    }
#endif
}

int chao_current_area(void)
{
    return garden_current;
}

int chao_previous_area(void)
{
    return garden_previous;
}

/* @hack spelling: player 0's held task read through a function, where
 * player_records[0]->held would do */
static struct task *held_task(int slot)
{
    return player_records[slot]->held;
}

void chao_change_area(int area)
{
    /* @hack spelling: category, which holds the held task's category for the
     * switch, where a switch on garden_object_category(held) would do */
    int category;
    struct task *held = held_task(0);
    /* @hack spelling: garden_current read through a pointer here and below,
     * where its name would do */
    int *active = &garden_current;
    int current = *active;

    garden_carry_chao(NULL);
    if (area != CHAO_AREA_ENTRANCE && area != CHAO_AREA_RACE &&
        current != CHAO_AREA_ENTRANCE && current != CHAO_AREA_RACE &&
        held != NULL) {
        category = garden_object_category(held);
        switch (category) {
        case GARDEN_OBJECT_EGG:
            garden_carry_egg(held);
            break;
        case GARDEN_OBJECT_CHAO:
            garden_carry_chao(held);
        }
    }
    if (area == CHAO_AREA_LOBBY) {
        switch (current) {
        case CHAO_AREA_NEUTRAL:
        case CHAO_AREA_HERO:
        case CHAO_AREA_DARK:
            if (game_flags[FLAG_SEQUENCE] == SEQUENCE_NONE) {
                break;
            }
            chao_leave();
            return;
        }
    }
    /* @hack redundant: a test whose arms both call garden_file_gather */
    if (area == CHAO_AREA_RACE) {
        garden_file_gather();
    } else {
        garden_file_gather();
    }
#ifdef NONMATCHING
    /* this side calls garden_request_area, the plain form the hack tag names.
     * it clears garden_task until the next area loads */
    garden_request_area(area);
    garden_task = NULL;
#else
    /* @hack spelling: garden_request_area(area)'s body in a block, where the
     * call would do */
    {
        int next = area;
        if (*active != next) {
            garden_previous = *active;
            garden_next = next;
        }
    }
#endif
    stage_restart();
    if (area != CHAO_AREA_RACE) {
        sound_play(SOUND_CHANGE_AREA, NULL, 0, 0);
    }
    music_fade_out();
}

void chao_leave(void)
{
    garden_file_gather();
    sound_play(SOUND_LEAVE_GARDEN, NULL, 0, 0);
    if (garden_task != NULL) {
        /* @hack redundant: a test whose arms both start the fade */
        if (game_flags[FLAG_SEQUENCE] != SEQUENCE_NONE) {
            garden_task->work->mode = GARDEN_FADE_OUT;
        } else {
            garden_task->work->mode = GARDEN_FADE_OUT;
        }
    }
}

static void player_control_step(struct task *task)
{
    struct task_work *work = task->work;
    int current = chao_current_area();
    int previous = chao_previous_area();
    NJS_POINT3 *pos;
    struct angles *ang;

    switch (current) {
    case CHAO_AREA_LOBBY:
        pad_action_held[0] = pad_action_pressed[0] = 0;
        break;
    case CHAO_AREA_KINDERGARTEN:
        pad_action_held[0] = pad_action_pressed[0] = 0;
        pad_jump_held[0] = pad_jump_pressed[0] = 0;
        break;
    }
    switch (work->mode) {
    case CONTROL_PLACE:
    case CONTROL_PLACE_AGAIN:
        pos = &player_work[0]->pos;
        ang = &player_work[0]->ang;
        switch (current) {
        case CHAO_AREA_LOBBY:
            switch (previous) {
            case CHAO_AREA_NONE:
            case 0:
                pos->x = 0.0F;
                pos->y = 0.0F;
                pos->z = 0.0F;
                ang->y = EIGHTH_TURN;
                break;
            case CHAO_AREA_NEUTRAL:
                pos->x = 80.0F;
                pos->y = 0.0F;
                pos->z = 80.0F;
                ang->y = HALF_TURN + EIGHTH_TURN;
                break;
            case CHAO_AREA_HERO:
                pos->x = 33.0F;
                pos->y = 51.0F;
                pos->z = -120.0F;
                ang->y = 0x1540;
                break;
            case CHAO_AREA_DARK:
                pos->x = -33.0F;
                pos->y = -51.0F;
                pos->z = 120.0F;
                ang->y = 0x9a00;
                break;
            case CHAO_AREA_KINDERGARTEN:
                pos->x = -80.0F;
                pos->y = 0.0F;
                pos->z = -80.0F;
                ang->y = EIGHTH_TURN;
                break;
            }
            break;
        case CHAO_AREA_NEUTRAL:
            switch (previous) {
            case CHAO_AREA_LOBBY:
                pos->x = 59.5F;
                pos->y = 0.0F;
                pos->z = -154.0F;
                ang->y = 0x5a62;
                break;
            case CHAO_AREA_ENTRANCE:
                pos->x = -119.5F;
                pos->y = 4.0F;
                pos->z = -98.0F;
                ang->y = 0x9fe;
                break;
            }
            break;
        case CHAO_AREA_HERO:
            pos->x = -91.0F;
            pos->y = 0.0F;
            pos->z = 88.0F;
            ang->y = 0xc950;
            break;
        case CHAO_AREA_DARK:
            pos->x = -100.0F;
            pos->y = 0.0F;
            pos->z = 63.0F;
            ang->y = FULL_TURN - EIGHTH_TURN;
            break;
        }
        work->mode++;
        break;
    }
}

static void player_control_destroy(struct task *task)
{
    garden_player_control = NULL;
}

static void garden_player_control_start(void)
{
    if (garden_player_control != NULL) {
        /* @hack redundant: a second test of garden_player_control, which
         * the first has passed */
        if (garden_player_control != NULL) {
            garden_player_control->work->mode = CONTROL_PLACE;
        }
        return;
    }
    garden_player_control =
        task_create(TASK_WORK, CONTROL_TASK_LEVEL, player_control_step,
                    "AL_PlayerControlManager");
#ifdef NONMATCHING
    /* this side makes no player control where task_create fails. the matching
     * side writes through the null */
    if (garden_player_control == NULL) {
        return;
    }
#endif
    garden_player_control->destructor = player_control_destroy;
}
