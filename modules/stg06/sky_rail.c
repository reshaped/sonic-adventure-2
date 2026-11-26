/* Sky Rail, the stage of STG06: its start and end, and its tasks' steps. the
 * background task, BgExec, draws the background about the current camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/emerald.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stage/stage.h"
#include "stg06/sky_rail.h"
#include "stg06/object_archives.h"
#include "stg06/sky_rail_sounds.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#include "stg06/sky_rail/text.inc"

static struct fog_setting sky_rail_fog;

/* the set file's block has room for at least SET_ENTRIES entries */
#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
#define BACKGROUND_BUILDS 2
#define KILL_AREAS 4
#define STAGE_LIGHTS 2
/* the background task's mode holds BG_LOW_CORNER for each of cameras 0 and 1.
 * a camera whose x is over LOW_CORNER_X and whose y is under LOW_CORNER_Y
 * takes BG_LOW_CORNER. nothing reads the bits */
#define BG_LOW_CORNER(camera) (1 << (camera))
#define BG_CAMERAS 2
#define LOW_CORNER_X 6530.394F
#define LOW_CORNER_Y (-1827.0F)
/* the background task's timer counts from 0 to TIMER_CYCLE - 1, and then
 * starts again. nothing reads it */
#define TIMER_CYCLE 14

static void background_step(struct task *task);
static void background_draw(struct task *task);

#include "stg06/grind_rails.h"

/* the background's four models and their texture lists. background_build
 * compiles the first and the last. the middle two scroll their textures by
 * their animations */
#include "stg06/background/layers.h"

/* TRUE where background_build compiled both models */
static Bool background_built = FALSE;
static struct model_build background_builds[BACKGROUND_BUILDS] = {
#include "stg06/sky_rail/background_builds.inc"
};
static NJS_TEXLIST *background_group[5] = {
#include "stg06/sky_rail/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg06/sky_rail/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg06/sky_rail/background_pvms.inc"
};
/* SHC refuses braces around a material's two colors, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL kill_area_0_material = {
#include "stg06/sky_rail/kill_area_0_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Uint16 kill_area_0_meshes[11] = {
#include "stg06/sky_rail/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg06/sky_rail/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[10] = {
#include "stg06/sky_rail/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[10] = {
#include "stg06/sky_rail/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg06/sky_rail/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0 = {
#include "stg06/sky_rail/kill_area_0.inc"
};
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL kill_area_1_material = {
#include "stg06/sky_rail/kill_area_1_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Uint16 kill_area_1_meshes[13] = {
#include "stg06/sky_rail/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg06/sky_rail/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[12] = {
#include "stg06/sky_rail/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[12] = {
#include "stg06/sky_rail/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg06/sky_rail/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1 = {
#include "stg06/sky_rail/kill_area_1.inc"
};
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL kill_area_2_material = {
#include "stg06/sky_rail/kill_area_2_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Uint16 kill_area_2_meshes[5] = {
#include "stg06/sky_rail/kill_area_2_meshes.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg06/sky_rail/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[4] = {
#include "stg06/sky_rail/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[4] = {
#include "stg06/sky_rail/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg06/sky_rail/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2 = {
#include "stg06/sky_rail/kill_area_2.inc"
};
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL kill_area_3_material = {
#include "stg06/sky_rail/kill_area_3_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Uint16 kill_area_3_meshes[7] = {
#include "stg06/sky_rail/kill_area_3_meshes.inc"
};
static NJS_MESHSET kill_area_3_meshset = {
#include "stg06/sky_rail/kill_area_3_meshset.inc"
};
static NJS_POINT3 kill_area_3_points[6] = {
#include "stg06/sky_rail/kill_area_3_points.inc"
};
static NJS_VECTOR kill_area_3_normals[6] = {
#include "stg06/sky_rail/kill_area_3_normals.inc"
};
static NJS_MODEL kill_area_3_model = {
#include "stg06/sky_rail/kill_area_3_model.inc"
};
static NJS_OBJECT kill_area_3 = {
#include "stg06/sky_rail/kill_area_3.inc"
};
static struct kill_area sky_rail_kill_areas[KILL_AREAS + 1] = {
#include "stg06/sky_rail/sky_rail_kill_areas.inc"
};
static struct light sky_rail_lights[STAGE_LIGHTS] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *sky_rail_set = NULL;
/* a word that no code reads */
static Sint32 dat_8c546c9c = 8;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color sky_rail_block_colors[] = {
    {0xff000000, 0x00},
    {0xffff0000, 0x32},
    {0xffffff00, 0x26},
    {0xff00ffff, 0x0c},
    {0xff0000ff, 0xcc},
    {0xffff00ff, 0x88},
    {0xff00ff00, 0x18},
    {0xffff6400, 0x10},
    {0xff006400, 0x60},
};
/* clang-format on */
static NJS_POINT3 sky_rail_block_origin = {
#include "stg06/sky_rail/sky_rail_block_origin.inc"
};
static NJS_POINT3 sky_rail_block_cell = {
#include "stg06/sky_rail/sky_rail_block_cell.inc"
};
/* the background's height: background_rise times the height of the camera's
 * player above background_floor, plus background_lift */
static Float background_floor = -400.0F;
static Float background_lift = 0.0F;
static Float background_rise = 1.0F;
/* how far a game step scrolls each middle model's texture along u and
 * along v */
static Sint32 background_1_scroll_u = 0;
static Sint32 background_1_scroll_v = 1;
static Sint32 background_2_scroll_u = 0;
static Sint32 background_2_scroll_v = 2;

/* compile the background's first and last models at buf, with the fog off,
 * and return where they end. unless both compile, background_draw draws them
 * uncompiled. the call turns the fog on as it ends */
static Uint8 *background_build(Uint8 *buf)
{
    njFogDisable();
    if (model_builds_compile(&buf, background_builds, BACKGROUND_BUILDS) ==
        BACKGROUND_BUILDS) {
        background_built = TRUE;
    }
    njFogEnable();
    return buf;
}

void sky_rail_init(void)
{
    struct task *task;

    world_land_start(&sky_rail_land);
    world_load_block_grid("SKY.BMP", sky_rail_block_colors,
                          &sky_rail_block_origin, &sky_rail_block_cell);
    texture_load_shared(sky_rail_pvms, sky_rail_texture_groups);
    texture_await_shared(sky_rail_pvms, sky_rail_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    sky_rail_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&sky_rail_objects, sky_rail_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &sky_rail_lights[0]);
    light_set(1, &sky_rail_lights[1]);
    light_set(2, &sky_rail_lights[0]);
    light_set(3, &sky_rail_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg06_light.bin");
    object_builds_run(background_build((Uint8 *)world_build_land(
        &sky_rail_land, (Uint32)STAGE_MODEL_BUILD_AREA)));
    music_fade_in(0);
    music_play_or_defer("s_rail.adx");
    path_create_manager(sky_rail_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
    }
    kill_areas_start(sky_rail_kill_areas);
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
    fog_start_file("stg06_fog.bin", &sky_rail_fog);
    njFogDisable();
    object_light_create();
    messenger_messages_load("mh0006");
    hint_box_hints_load("bh0006");
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
    sound_load_stage("se_ac_sr.mlt", &sky_rail_sound_list);
}

void sky_rail_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
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
    particle_core_stop();
    texture_release_shared(sky_rail_pvms, sky_rail_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (sky_rail_set != NULL) {
        loader_free(sky_rail_set);
        sky_rail_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void sky_rail_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

/* set or clear the BG_LOW_CORNER bit of each of cameras 0 and 1. the loop
 * stops at the first camera that camera_views lacks. the bits of that camera
 * and those after it stay as they were */
static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    int i;

    for (i = 0; i < BG_CAMERAS && camera_views[i] != NULL; i++) {
        pos = camera_views[i]->pos;
        if (pos.x > LOW_CORNER_X && pos.y < LOW_CORNER_Y) {
            work->mode |= BG_LOW_CORNER(i);
        } else {
            work->mode &= ~BG_LOW_CORNER(i);
        }
    }
    work->timer++;
    if (work->timer >= TIMER_CYCLE) {
        work->timer = 0;
    }
}

/* draw the background with the fog off. it stands at the current camera's x
 * and z, and its height follows that camera's player. the middle models
 * scroll their textures with game_state.steps. the call turns the fog on as
 * it ends */
static void background_draw(struct task *task)
{
    /* @hack redundant: work, a local that nothing reads */
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack spelling: height, a local for the player's height above
     * background_floor, where the difference in the product would do */
    Float height;

    njFogDisable();
    njPushMatrixEx();
    pos.x = camera_views[camera_current_index]->pos.x;
    /* @hack redundant: the + 0.0F, which changes no height */
    height = player_work[camera_current_index]->pos.y - background_floor;
    pos.y = (height * background_rise) + background_lift + 0.0F;
    pos.z = camera_views[camera_current_index]->pos.z;
    njTranslateEx(&pos);
    njSetTexture(&sky_rail_background_0_texlist);
    if (background_built) {
        njDirectDrawObject(background_builds[0].built);
    } else {
        njCnkEasyDrawModel(&sky_rail_background_0_model);
    }
    njSetTexture(&sky_rail_background_1_texlist);
    texture_anim_scroll(
        &sky_rail_background_1_model, &sky_rail_background_1_anim,
        (background_1_scroll_u * game_state.steps) & (SCROLL_WRAP - 1),
        (background_1_scroll_v * game_state.steps) & (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(&sky_rail_background_1_model);
    njSetTexture(&sky_rail_background_2_texlist);
    texture_anim_scroll(
        &sky_rail_background_2_model, &sky_rail_background_2_anim,
        (background_2_scroll_u * game_state.steps) & (SCROLL_WRAP - 1),
        (background_2_scroll_v * game_state.steps) & (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(&sky_rail_background_2_model);
    njSetTexture(&sky_rail_background_3_texlist);
    if (background_built) {
        njDirectDrawObject(background_builds[1].built);
    } else {
        njCnkEasyDrawModel(&sky_rail_background_3_model);
    }
    njPopMatrixEx();
    njFogEnable();
}
