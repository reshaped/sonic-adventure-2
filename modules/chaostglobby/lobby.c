/* the Chao Lobby's area: its land, its lights, its exit and its gates */
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/messages.h"
#include "chao/residents.h"
#include "chaostglobby/gate.h"
#include "chaostglobby/lobby.h"
#include "collision/world.h"
#include "draw/light.h"
#include "player/player.h"
#include "shared/garden_setup.h"
#include "shared/messenger.h"
#include "shared/object_light.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#ifdef NONMATCHING
/* the land's file and build buffer by their addresses, the plain form the hack
 * tag names. this side's lobby_start_land makes garden_land_file.h's checks */
#include "shared/garden_land_file.h"
#include "system/areas.h"
#define LOBBY_LAND_FILE ((struct land_table **)CHAO_LOBBY_LAND_AREA)
#define LAND_BUILD_AREA CHAO_LAND_BUILD_AREA
#else
/* @hack spelling: the land's file and build buffer as symbols the linker
 * places at CHAO_LOBBY_LAND_AREA and CHAO_LAND_BUILD_AREA, where those
 * addresses would do */
extern struct land_table *lobby_land_file[];
extern Uint8 lobby_land_buffer[];
#define LOBBY_LAND_FILE lobby_land_file
#define LAND_BUILD_AREA lobby_land_buffer
#endif

static const NJS_POINT3 exit_position = {
#include "chaostglobby/lobby/exit_position.inc"
};
static const NJS_POINT3 neutral_position = {
#include "chaostglobby/lobby/neutral_position.inc"
};
static const NJS_POINT3 hero_position = {
#include "chaostglobby/lobby/hero_position.inc"
};
static const NJS_POINT3 dark_position = {
#include "chaostglobby/lobby/dark_position.inc"
};
static const NJS_POINT3 kinder_position = {
#include "chaostglobby/lobby/kinder_position.inc"
};
/* the turn about y of each, in the order above */
#define EXIT_YAW 0
#define NEUTRAL_YAW (-0x6000)
#define HERO_YAW 0x4000
#define DARK_YAW (-0x4000)
#define KINDER_YAW 0x2000

/* @hack type: lobby_main_light, an array of one light, where a light of its
 * own would do */
static struct light lobby_main_light[1] = {
#include "chaostglobby/lobby/lobby_main_light.inc"
};
static struct light lobby_fill_light = {
#include "chaostglobby/lobby/lobby_fill_light.inc"
};

/* the lobby's lands. a land's file name holds h for the Hero Garden and d
 * for the Dark Garden. k stands for the kindergarten, and 0 for each the land
 * leaves out. lobby_layout picks a land by which are open. a land with h or d
 * has k too, whether the kindergarten is open or not */
enum lobby_land {
    LOBBY_EMPTY,
    LOBBY_KINDER,
    LOBBY_HERO,
    LOBBY_DARK,
    LOBBY_ALL
};

enum { GREETING_START = 0, GREETING_DONE = 4 };

/* where the lobby is the first area, a messenger greets the player. it
 * starts SUMMON_X along x from player 0 and SUMMON_HEIGHT above, and shows
 * mhChaoLobby's message GREETING_MESSAGE */
#define SUMMON_X 20.0F
#define SUMMON_HEIGHT 10.0F
#define GREETING_MESSAGE 0

/* the light both players take, and the one the camera hands Ninja, which
 * light_select_camera sets */
#define PLAYER_LIGHT 0
#define CAMERA_LIGHT 0

static void lobby_greeting(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack spelling: previous, a copy of chao_previous_area's result for its
     * one test, where the call would do */
    int previous;
    switch (work->mode) {
    case GREETING_START:
        work->mode = GREETING_DONE;
        work->submode = 0;
        work->timer = 0;
        work->tick = 0;
        previous = chao_previous_area();
        if (previous <= 0) {
            pos = player_work[0]->pos;
            pos.y += SUMMON_HEIGHT;
            pos.x += SUMMON_X;
            messenger_create(&pos, 0, 0, GREETING_MESSAGE, MESSENGER_SUMMON,
                             NULL);
        }
        break;
    /* @hack redundant: a case for GREETING_DONE and a default, which do
     * nothing */
    case GREETING_DONE:
        break;
    default:
        return;
    }
}

static void lobby_close(struct task *task)
{
    garden_messages_free();
    texture_release_shared(garden_pvms, garden_texture_groups);
    njReleaseTexture(&lobby_texlist);
    messenger_messages_free();
    world_land_stop();
}

/* @hack spelling: the placements and lobby_layout external, where static
 * would do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void lobby_place_exit(void);
void lobby_place_neutral_gate(void);
void lobby_place_hero_gate(void);
void lobby_place_dark_gate(void);
void lobby_place_kinder_gate(void);
enum lobby_land lobby_layout(void);
/* NOLINTEND(misc-use-internal-linkage) */

void lobby_place_exit(void)
{
    /* @hack spelling: pos, a copy of exit_position, where its address would
     * do */
    NJS_POINT3 pos;
    pos = exit_position;
    lobby_exit_create(&pos, EXIT_YAW);
}

void lobby_place_neutral_gate(void)
{
    NJS_POINT3 pos;
    pos = neutral_position;
    lobby_neutral_gate_create(&pos, NEUTRAL_YAW);
}

void lobby_place_hero_gate(void)
{
    NJS_POINT3 pos;
    pos = hero_position;
    if (garden_flag_test(GARDEN_HERO_OPEN)) {
        lobby_hero_gate_create(&pos, HERO_YAW);
    }
}

void lobby_place_dark_gate(void)
{
    NJS_POINT3 pos;
    pos = dark_position;
    if (garden_flag_test(GARDEN_DARK_OPEN)) {
        lobby_dark_gate_create(&pos, DARK_YAW);
    }
}

void lobby_place_kinder_gate(void)
{
    NJS_POINT3 pos;
    pos = kinder_position;
    if (garden_flag_test(GARDEN_KINDER_OPEN)) {
        lobby_kinder_gate_create(&pos, KINDER_YAW);
    }
}

enum lobby_land lobby_layout(void)
{
    enum lobby_land layout = LOBBY_EMPTY;

    if (garden_flag_test(GARDEN_KINDER_OPEN)) {
        layout = LOBBY_KINDER;
    }
    if (garden_flag_test(GARDEN_HERO_OPEN)) {
        if (garden_flag_test(GARDEN_DARK_OPEN)) {
            layout = LOBBY_ALL;
        } else {
            layout = LOBBY_HERO;
        }
    } else if (garden_flag_test(GARDEN_DARK_OPEN)) {
        layout = LOBBY_DARK;
    }
    return layout;
}

#ifdef NONMATCHING
/* this side loads the lobby's land for layout into its area, checks it and
 * starts it. it returns whether it did */
static Bool lobby_start_land(enum lobby_land layout, struct land_table **land)
{
    static const char *const names[] = {
        "ChaoStgLobbyLand000.PRS", "ChaoStgLobbyLand00k.PRS",
        "ChaoStgLobbyLandh0k.PRS", "ChaoStgLobbyLand0dk.PRS",
        "ChaoStgLobbyLandhdk.PRS",
    };
    Sint32 size;
    size = loader_read_prs_checked(names[layout], land,
                                   (uintptr_t)CHAO_LAND_BUILD_AREA -
                                       (uintptr_t)CHAO_LOBBY_LAND_AREA);
    return size >= (Sint32)sizeof(*land) &&
           garden_land_ready(land, size, *land) && world_land_start(*land);
}
#endif

void chaostglobby_step(struct task *task)
{
    enum lobby_land layout = lobby_layout();
#ifdef NONMATCHING
    /* this side starts the land by lobby_start_land, and tries again the next
     * step where it can't */
    struct land_table **land = LOBBY_LAND_FILE;
    void (*set_light)(int, const struct light *);

    if (!lobby_start_land(layout, land)) {
        return;
    }
#else
    /* @hack spelling: loader_read_prs called through load, where direct calls
     * would do */
    int (*load)(const char *, void *) = loader_read_prs;
    struct land_table **land = LOBBY_LAND_FILE;
    void (*set_light)(int, const struct light *);

    switch (layout) {
    case LOBBY_EMPTY:
        load("ChaoStgLobbyLand000.PRS", land);
        break;
    case LOBBY_KINDER:
        load("ChaoStgLobbyLand00k.PRS", land);
        break;
    case LOBBY_HERO:
        load("ChaoStgLobbyLandh0k.PRS", land);
        break;
    case LOBBY_DARK:
        load("ChaoStgLobbyLand0dk.PRS", land);
        break;
    case LOBBY_ALL:
        load("ChaoStgLobbyLandhdk.PRS", land);
        break;
    }
    world_land_start(*land);
#endif
    texture_load_pvm("AL_LOBBY_OBJ_TEX", &lobby_texlist);
    texture_load_shared(garden_pvms, garden_texture_groups);
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    /* @hack spelling: light_set called through set_light, where direct calls,
     * as GARDEN_SET_LIGHTING makes them, would do */
    set_light = light_set;
    set_light(PLAYER_LIGHT, lobby_main_light);
    set_light(1, &lobby_fill_light);
    set_light(LIGHT_LAND, lobby_main_light);
    set_light(3, lobby_main_light);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(CAMERA_LIGHT);
    light_select_after_draw(LIGHT_LAND);
    light_apply_player(0);
    lights_load("stg50_light.bin");
    world_build_land(*land, (Uint32)LAND_BUILD_AREA);
    music_fade_in(0);
    music_play_name("chao_l_m.adx");
    njSetBackColor(0xff000000, 0xff000000, 0xff000000);
    camera_set_view_angle(0, GARDEN_VIEW_ANGLE, 0);
    garden_camera_start();
    garden_camera_create();
    garden_control_start();
    task->step = lobby_greeting;
    task->draw = NULL;
    task->destructor = lobby_close;
    camera_active_count = 1;
    garden_restore_carried();
    lobby_place_exit();
    lobby_place_neutral_gate();
    lobby_place_hero_gate();
    lobby_place_dark_gate();
    lobby_place_kinder_gate();
    messenger_messages_load("mhChaoLobby");
    garden_messages_load("msgChaoOsanpo");
    if (chao_take_bad_file()) {
        lobby_bad_file_message();
    }
}
