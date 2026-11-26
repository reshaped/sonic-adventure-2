/* the stage's init, and the lights, kill areas and block grid it starts the
 * stage with */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
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
#include "stage/stage.h"
#include "stg22/breakables.h"
#include "stg22/crazy_gadget.h"
#include "stg22/fall_surfaces.h"
#include "stg22/rail_marker.h"
#include "stg22/sound_list.h"
#include "stg22/stage_data.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#define BACKGROUND_LEVEL 1
/* the set file's block is at least SET_ENTRIES entries long, its head
 * counted as one */
#define SET_ENTRIES 2048

static struct fog_setting gadget_fog;

static struct kill_area stage_kill_areas[10] = {
#include "stg22/crazy_gadget/stage_kill_areas.inc"
};
static struct light stage_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
struct set_head *gadget_set = NULL;
struct task *gadget_background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color block_colors[] = {
    {0xff000000, 0x00000},
    {0xffffff00, 0x008c2},
    {0xffff0000, 0x01886},
    {0xffff00ff, 0x01180},
    {0xff00ffff, 0x18004},
    {0xffff7d00, 0x0210c},
    {0xff0000ff, 0x16300},
    {0xff007d00, 0x20010},
    {0xff7d007d, 0x04200},
    {0xff7d0000, 0x0c620},
    {0xff007d7d, 0x00460},
    {0xff00ff00, 0x30040},
    {0xff00007d, 0x60000},
};
/* clang-format on */
static NJS_POINT3 grid_origin = {-11300.0F, 0.0F, 703.149414F};
static NJS_POINT3 grid_cell = {26.59091F, 0.0F, 26.5748024F};
struct task *gadget_event_task = NULL;

void crazy_gadget_init(void)
{
    struct task *background;
    Uint8 *cursor;

    world_land_start(&crazy_gadget_land);
    world_load_block_grid("CRAZY.BMP", block_colors, &grid_origin, &grid_cell);
    texture_load_shared(gadget_background_pvms, gadget_background_groups);
    texture_await_shared(gadget_background_pvms, gadget_background_groups);
    texture_load_shared(gadget_object_pvms, gadget_object_groups);
    texture_await_shared(gadget_object_pvms, gadget_object_groups);
    gadget_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&crazy_gadget_objects, gadget_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &stage_lights[0]);
    light_set(1, &stage_lights[1]);
    light_set(2, &stage_lights[0]);
    light_set(3, &stage_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg22_light.bin");
    cursor = (Uint8 *)world_build_land(&crazy_gadget_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    cursor = object_builds_run(cursor);
    cursor = gadget_cage_build(cursor);
    gadget_rail_marker_build(cursor);
    music_fade_in(0);
    music_play_or_defer("c_gadget.adx");
    path_create_manager(crazy_gadget_paths);
    background = task_create(TASK_WORK, BACKGROUND_LEVEL,
                             gadget_background_step, "BgExec");
    if (background) {
        background->draw = gadget_background_draw;
        background->destructor = gadget_background_destroy;
    }
    kill_areas_start(stage_kill_areas);
    module_motions_register();
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    fog_start_file("stg22_fog.bin", &gadget_fog);
    njFogEnable();
    object_light_create();
    messenger_messages_load("mh0022");
    hint_box_hints_load("bh0022");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    object_builds_init();
    sound_load_stage("se_ac_cg.mlt", &gadget_sounds);
}

/* events.c's code starts at the next 32-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
