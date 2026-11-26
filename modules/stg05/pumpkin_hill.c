/* Pumpkin Hill, the stage of STG05: its start and end, and its tasks' steps.
 * the background task, BgExec, draws the background about the current camera.
 * the updraft task, ObjSetManExec, lifts the players out of the pit under the
 * stage */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "stg05/pumpkin_hill/text.inc"

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/emerald.h"
#include "shared/flash.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/skull.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg05/pumpkin_hill.h"
#include "stg05/object_archives.h"
#include "stg05/scenery.h"
#include "stg05/train_gate.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "vmu/vibration.h"

static struct fog_setting pumpkin_hill_fog;

#define BACKGROUND_LEVEL 1
#define UPDRAFT_LEVEL 2
/* the set file's block has room for at least SET_ENTRIES entries */
#define SET_ENTRIES 2048
#define EMERALD_PATHS 5
#define KILL_AREA_POINTS 4
#define BACKGROUND_MODELS 3
#define STAGE_LIGHTS 2

/* the updraft lifts the players in slots 0 and 1, each by a state of its own.
 * a player's state is UPDRAFT_NONE until the updraft catches it. the state
 * then counts the lift's steps from UPDRAFT_START. a lift runs at least
 * UPDRAFT_MIN_STEPS steps and at most UPDRAFT_MAX_STEPS. from step
 * UPDRAFT_MIN_STEPS on, it ends once the player's speed along y is below 0.
 * once the lift ends, the state is UPDRAFT_SPENT. it turns UPDRAFT_NONE again
 * once the player's mode is PLAYER_MODE_RUN, or a mode from
 * PLAYER_MODE_HUNTER_GLIDE to PLAYER_MODE_HUNTER_SPIRAL. the updraft does
 * nothing while game_state.cleared is 1 */
#define UPDRAFT_PLAYERS 2
#define UPDRAFT_NONE 0
#define UPDRAFT_START 1
#define UPDRAFT_MIN_STEPS 10
#define UPDRAFT_MAX_STEPS 120
#define UPDRAFT_SPENT (-1)
/* the updraft catches a player whose pos.y is below updraft_entry -
 * CATCH_DEPTH. its push up each step is updraft_power / (pos.y + LIFT_DEPTH). a
 * divisor below MIN_DIVISOR counts as MIN_DIVISOR */
#define CATCH_DEPTH 1500.0F
#define LIFT_DEPTH 1600.0F
#define MIN_DIVISOR 1.0F
/* the shake a caught player's pad takes */
#define VIBRATION_POWER 5
#define VIBRATION_FREQ 38
#define VIBRATION_LENGTH 12

/* player i's state, in byte i of the updraft task's timer */
/* @hack type: each state read as a byte of the timer through its address,
 * where a Sint8 array in the timer's place would do */
#define UPDRAFT_STATE(work, i) (((Sint8 *)&(work)->timer)[i])

static void updraft_step(struct task *task);
static void updraft_destroy(struct task *task);
static void background_step(struct task *task);
static void background_draw(struct task *task);

#include "stg05/emerald_routes.h"

/* the hunt's hints for each of the stage's places, as hunt_set_hints takes
 * them */
static struct hunt_hint pumpkin_hill_hints[149] = {
#include "stg05/pumpkin_hill/pumpkin_hill_hints.inc"
};
/* a table of hints that no code reads */
static struct hunt_hint dat_8c543f00[4] = {
    {EMERALD_KIND_A_PATHMOVE, 1, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
    {HUNT_HINT_END, 0, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
};
/* the hint file pumpkin_hill_init loaded, until pumpkin_hill_exit frees it */
static Sint32 *pumpkin_hill_hint_file = NULL;
/* the stage's paths for path_create_manager: none, only the null that ends
 * the list */
static struct path *pumpkin_hill_paths[1] = {
#include "stg05/pumpkin_hill/pumpkin_hill_paths.inc"
};
static struct path *pumpkin_hill_emerald_paths[EMERALD_PATHS + 1] = {
#include "stg05/pumpkin_hill/pumpkin_hill_emerald_paths.inc"
};

/* the background: three models whose textures bgtex05 holds. background_0
 * encloses the view, from 2067 below to 1560 above. background_1 and
 * background_2 are discs at a height of about -1500, which turn at rates of
 * their own */
static NJS_TEXNAME background_texnames[4] = {
#include "stg05/pumpkin_hill/background_texnames.inc"
};
static NJS_TEXLIST background_texlist = {
#include "stg05/pumpkin_hill/background_texlist.inc"
};

static Sint16 background_0_polygons[507] = {
#include "stg05/pumpkin_hill/background_0_polygons.inc"
};

static Sint32 background_0_vertices[267] = {
#include "stg05/pumpkin_hill/background_0_vertices.inc"
};
static NJS_CNK_MODEL background_0_model = {
#include "stg05/pumpkin_hill/background_0_model.inc"
};
static NJS_CNK_OBJECT background_0 = {
#include "stg05/pumpkin_hill/background_0.inc"
};

static Sint16 background_1_polygons[107] = {
#include "stg05/pumpkin_hill/background_1_polygons.inc"
};

static Sint32 background_1_vertices[71] = {
#include "stg05/pumpkin_hill/background_1_vertices.inc"
};
static NJS_CNK_MODEL background_1_model = {
#include "stg05/pumpkin_hill/background_1_model.inc"
};
static NJS_CNK_OBJECT background_1 = {
#include "stg05/pumpkin_hill/background_1.inc"
};
static Sint16 background_2_polygons[107] = {
#include "stg05/pumpkin_hill/background_2_polygons.inc"
};
static Sint32 background_2_vertices[71] = {
#include "stg05/pumpkin_hill/background_2_vertices.inc"
};
static NJS_CNK_MODEL background_2_model = {
#include "stg05/pumpkin_hill/background_2_model.inc"
};
static NJS_CNK_OBJECT background_2 = {
#include "stg05/pumpkin_hill/background_2.inc"
};
/* TRUE where background_build compiled all three models */
static Bool background_built = FALSE;
static struct model_build background_builds[BACKGROUND_MODELS] = {
#include "stg05/pumpkin_hill/background_builds.inc"
};

/* the kill area: a rectangle under the stage, at a height of -1550 */
static NJS_MATERIAL kill_area_material = {
#include "stg05/pumpkin_hill/kill_area_material.inc"
};
static Sint16 kill_area_meshes[5] = {
#include "stg05/pumpkin_hill/kill_area_meshes.inc"
};
static NJS_MESHSET kill_area_meshset = {
#include "stg05/pumpkin_hill/kill_area_meshset.inc"
};
static NJS_POINT3 kill_area_points[KILL_AREA_POINTS] = {
#include "stg05/pumpkin_hill/kill_area_points.inc"
};
static NJS_VECTOR kill_area_normals[KILL_AREA_POINTS] = {
#include "stg05/pumpkin_hill/kill_area_normals.inc"
};
static NJS_MODEL kill_area_model = {
#include "stg05/pumpkin_hill/kill_area_model.inc"
};
static NJS_OBJECT kill_area = {
#include "stg05/pumpkin_hill/kill_area.inc"
};
static struct kill_area pumpkin_hill_kill_areas[2] = {
#include "stg05/pumpkin_hill/pumpkin_hill_kill_areas.inc"
};

static struct light pumpkin_hill_lights[STAGE_LIGHTS] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *pumpkin_hill_set = NULL;
/* the discs' turns a step, in Angle units */
static Sint32 background_1_turn = 5;
static Sint32 background_2_turn = 2;
/* the updraft's task, or null while none lives */
static struct task *updraft_task = NULL;
static Float updraft_power = 25.0F;
static Float updraft_entry = 18.0F;

/* compile the background's models at buf, with the fog off, and return where
 * the builds end. unless all three compile, background_draw draws the models
 * uncompiled. the call turns the fog on as it ends */
static Uint8 *background_build(Uint8 *buf)
{
    njFogDisable();
    if (model_builds_compile(&buf, background_builds, BACKGROUND_MODELS) ==
        BACKGROUND_MODELS) {
        background_built = TRUE;
    }
    njFogEnable();
    return buf;
}

void pumpkin_hill_init(void)
{
    struct task *task;

    world_land_start(&pumpkin_hill_land);
    texture_load_shared(pumpkin_hill_pvms, pumpkin_hill_texture_groups);
    texture_await_shared(pumpkin_hill_pvms, pumpkin_hill_texture_groups);
    texture_load_pvm(BACKGROUND_ARCHIVE, &background_texlist);
    pumpkin_hill_set = set_read_stage(NULL, SET_ENTRIES);
    if (game_state.two_players != 0) {
        set_start(&pumpkin_hill_2p_objects, pumpkin_hill_set);
    } else {
        set_start(&pumpkin_hill_objects, pumpkin_hill_set);
    }
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &pumpkin_hill_lights[0]);
    light_set(1, &pumpkin_hill_lights[1]);
    light_set(2, &pumpkin_hill_lights[0]);
    light_set(3, &pumpkin_hill_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg05_light.bin");
    scenery_build(object_builds_run(background_build((Uint8 *)world_build_land(
        &pumpkin_hill_land, (Uint32)STAGE_MODEL_BUILD_AREA))));
    music_fade_in(0);
    music_play_or_defer("p_hill.adx");
    path_create_manager(pumpkin_hill_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
    }
    kill_areas_start(pumpkin_hill_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: the test of drop_rings made again */
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.unk_04 == NULL) {
        player_hooks.unk_04 = dirt_create;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.create_piece == NULL) {
        player_hooks.create_piece = emerald_create;
    }
    if (player_hooks.show_piece_sign == NULL) {
        player_hooks.show_piece_sign = emerald_sign_create;
    }
    if (player_hooks.explode == NULL) {
        player_hooks.explode = explosion_create;
    }
    if (player_hooks.throw_skull == NULL) {
        player_hooks.throw_skull = skull_throw_up;
    }
    if (player_hooks.flash == NULL) {
        player_hooks.flash = flash_manager_create_sparks_depth;
    }
    fog_start_file("stg05_fog.bin", &pumpkin_hill_fog);
    njFogDisable();
    object_light_create();
    hunt_set_hints(pumpkin_hill_hints);
    pumpkin_hill_hint_file = hunt_load_hint_file("eh0005");
    emerald_set_paths(pumpkin_hill_emerald_paths, EMERALD_PATHS);
    messenger_messages_load("mh0005");
    hint_box_hints_load("bh0005");
    smoke_task_create(0, NULL);
    if (game_state.two_players != 0) {
        smoke_task_create(1, NULL);
    }
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_ph.mlt", &pumpkin_hill_sounds);
}

void pumpkin_hill_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
    emerald_clear_paths();
    syFree(pumpkin_hill_hint_file);
    pumpkin_hill_hint_file = NULL;
    hunt_clear_hints();
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.unk_04 == dirt_create) {
        player_hooks.unk_04 = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    if (player_hooks.create_piece == emerald_create) {
        player_hooks.create_piece = NULL;
    }
    if (player_hooks.show_piece_sign == emerald_sign_create) {
        player_hooks.show_piece_sign = NULL;
    }
    if (player_hooks.explode == explosion_create) {
        player_hooks.explode = NULL;
    }
    if (player_hooks.throw_skull == skull_throw_up) {
        player_hooks.throw_skull = NULL;
    }
    if (player_hooks.flash == flash_manager_create_sparks_depth) {
        player_hooks.flash = NULL;
    }
    particle_core_stop();
    njReleaseTexture(&background_texlist);
    texture_release_shared(pumpkin_hill_pvms, pumpkin_hill_texture_groups);
    set_end();
    if (pumpkin_hill_set != NULL) {
        loader_free(pumpkin_hill_set);
        pumpkin_hill_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

static void updraft_step(struct task *task)
{
    struct task_work *work = task->work;
    Float height;
    int i;

    for (i = 0; i < UPDRAFT_PLAYERS; i++) {
        if (game_state.cleared == 1) {
            break;
        }
        /* @hack spelling: the player's steps in a block of their own, where a
         * continue past an empty slot would do */
        if (player_work[i] != NULL) {
            if (UPDRAFT_STATE(work, i) == UPDRAFT_NONE) {
                /* @hack spelling: height, a local for the player's height,
                 * where the sum in the test would do */
                height = player_work[i]->pos.y + CATCH_DEPTH;
                if (height < updraft_entry) {
                    UPDRAFT_STATE(work, i) = UPDRAFT_START;
                    player_set_speed(i, 0.0F, 0.0F, 0.0F);
                    vibration_start_fading(i, VIBRATION_POWER, VIBRATION_FREQ,
                                           VIBRATION_LENGTH);
                }
            }
            if (UPDRAFT_STATE(work, i) >= UPDRAFT_START) {
                /* @hack spelling: the count made in each branch, where one
                 * count before a test past UPDRAFT_MIN_STEPS would do */
                if (UPDRAFT_STATE(work, i) < UPDRAFT_MIN_STEPS) {
                    UPDRAFT_STATE(work, i)++;
                } else {
                    UPDRAFT_STATE(work, i)++;
                    if (UPDRAFT_STATE(work, i) > UPDRAFT_MAX_STEPS ||
                        player_motion[i]->spd.y < 0.0F) {
                        UPDRAFT_STATE(work, i) = UPDRAFT_SPENT;
                    }
                }
                height = player_work[i]->pos.y + LIFT_DEPTH;
                if (height < MIN_DIVISOR) {
                    height = MIN_DIVISOR;
                }
                /* @hack spelling: the lift stored back in height, where the
                 * quotient in the call would do */
                height = updraft_power / height;
                player_updraft(i, 0.0F, height, 0.0F);
            }
            /* @hack spelling: the spent state tested in an if of its own,
             * where && would do */
            if (UPDRAFT_STATE(work, i) == UPDRAFT_SPENT) {
                if ((player_work[i]->mode >= PLAYER_MODE_HUNTER_GLIDE &&
                     player_work[i]->mode <= PLAYER_MODE_HUNTER_SPIRAL) ||
                    player_work[i]->mode == PLAYER_MODE_RUN) {
                    UPDRAFT_STATE(work, i) = UPDRAFT_NONE;
                }
            }
        }
    }
}

static void updraft_destroy(struct task *task)
{
    if (task == updraft_task) {
        updraft_task = NULL;
    }
}

void pumpkin_hill_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read
     * in place would do */
    struct task_work *work = task->work;

    if (updraft_task == NULL) {
        updraft_task = task_create(TASK_WORK, UPDRAFT_LEVEL, updraft_step,
                                   "ObjSetManExec");
        if (updraft_task != NULL) {
            updraft_task->destructor = updraft_destroy;
            updraft_step(updraft_task);
        }
    }
    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
}

/* draw the background with the fog off. it centers on the current camera's x
 * and z, at a height of 0. each disc turns about y by its turn times
 * game_state.steps. the call turns the fog on as it ends */
static void background_draw(struct task *task)
{
    NJS_POINT3 pos;

    njFogDisable();
    njSetTexture(&background_texlist);
    njPushMatrixEx();
    pos.x = camera_views[camera_current_index]->pos.x;
    pos.y = 0.0F;
    pos.z = camera_views[camera_current_index]->pos.z;
    njTranslateV(NULL, &pos);
    if (background_built) {
        njDirectDrawObject(background_builds[0].built);
    } else {
        njCnkEasyDrawModel(&background_0_model);
    }
    njPushMatrixEx();
    njRotateY(NULL, game_state.steps * background_1_turn);
    if (background_built) {
        njDirectDrawObject(background_builds[1].built);
    } else {
        njCnkEasyDrawModel(&background_1_model);
    }
    njPopMatrix(1);
    njRotateY(NULL, game_state.steps * background_2_turn);
    if (background_built) {
        njDirectDrawObject(background_builds[2].built);
    } else {
        njCnkEasyDrawModel(&background_2_model);
    }
    njPopMatrix(1);
    njFogEnable();
}
