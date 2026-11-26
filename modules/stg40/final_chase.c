/* Final Chase, stg40: the stage's init, exit and step, and its background.
 * the background task, BgExec, draws the sky around the current camera,
 * animates the sky's land and throws meteors */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg40/sky_land.h"
#include "stg40/paths.h"
#include "stg40/terrain.h"
#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/land_draw.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/vertex_colors.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg40/final_chase.h"
#include "stg40/object_archives.h"
#include "stg40/meteor.h"
#include "stg40/sky.h"
#include "stg40/tunnel_sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
#define METEOR_LEVEL 2
#define BACKGROUND_MODELS 4
#define BACKGROUND_SCROLLS 28

/* while kill_areas_task has a child, player 0 sheds a spark in each step
 * whose RANDOM_UNIT() is above SPARK_ABOVE */
#define SPARK_ABOVE 0.5F

/* an object of final_chase_sky_land that the background animates. with an
 * anim, its texture coordinates scroll by u_speed and v_speed times
 * scroll_rate each step. without one, its vertices take the pulse's color */
struct background_scroll {
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 v_speed;
    Sint32 u_speed;
};

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

/* The six BASIC collision meshes beneath the course. */
static NJS_MATERIAL kill_area_0_material = {
#include "stg40/final_chase/kill_area_0_material.inc"
};
static Sint16 kill_area_0_meshes[12] = {
#include "stg40/final_chase/kill_area_0_meshes.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg40/final_chase/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[8] = {
#include "stg40/final_chase/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[8] = {
#include "stg40/final_chase/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg40/final_chase/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0 = {
#include "stg40/final_chase/kill_area_0.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg40/final_chase/kill_area_1_material.inc"
};
static Sint16 kill_area_1_meshes[7] = {
#include "stg40/final_chase/kill_area_1_meshes.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg40/final_chase/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[6] = {
#include "stg40/final_chase/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[6] = {
#include "stg40/final_chase/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg40/final_chase/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1 = {
#include "stg40/final_chase/kill_area_1.inc"
};
static NJS_MATERIAL kill_area_2_material = {
#include "stg40/final_chase/kill_area_2_material.inc"
};
static Sint16 kill_area_2_meshes[16] = {
#include "stg40/final_chase/kill_area_2_meshes.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg40/final_chase/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[12] = {
#include "stg40/final_chase/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[12] = {
#include "stg40/final_chase/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg40/final_chase/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2 = {
#include "stg40/final_chase/kill_area_2.inc"
};
static NJS_MATERIAL kill_area_3_material = {
#include "stg40/final_chase/kill_area_3_material.inc"
};
static Sint16 kill_area_3_meshes[7] = {
#include "stg40/final_chase/kill_area_3_meshes.inc"
};
static NJS_MESHSET kill_area_3_meshset = {
#include "stg40/final_chase/kill_area_3_meshset.inc"
};
static NJS_POINT3 kill_area_3_points[6] = {
#include "stg40/final_chase/kill_area_3_points.inc"
};
static NJS_VECTOR kill_area_3_normals[6] = {
#include "stg40/final_chase/kill_area_3_normals.inc"
};
static NJS_MODEL kill_area_3_model = {
#include "stg40/final_chase/kill_area_3_model.inc"
};
static NJS_OBJECT kill_area_3 = {
#include "stg40/final_chase/kill_area_3.inc"
};
static NJS_MATERIAL kill_area_4_material = {
#include "stg40/final_chase/kill_area_4_material.inc"
};
static Sint16 kill_area_4_meshes[18] = {
#include "stg40/final_chase/kill_area_4_meshes.inc"
};
static NJS_MESHSET kill_area_4_meshset = {
#include "stg40/final_chase/kill_area_4_meshset.inc"
};
static NJS_POINT3 kill_area_4_points[14] = {
#include "stg40/final_chase/kill_area_4_points.inc"
};
static NJS_VECTOR kill_area_4_normals[14] = {
#include "stg40/final_chase/kill_area_4_normals.inc"
};
static NJS_MODEL kill_area_4_model = {
#include "stg40/final_chase/kill_area_4_model.inc"
};
static NJS_OBJECT kill_area_4 = {
#include "stg40/final_chase/kill_area_4.inc"
};
static NJS_MATERIAL kill_area_5_material = {
#include "stg40/final_chase/kill_area_5_material.inc"
};
static Sint16 kill_area_5_meshes[5] = {
#include "stg40/final_chase/kill_area_5_meshes.inc"
};
static NJS_MESHSET kill_area_5_meshset = {
#include "stg40/final_chase/kill_area_5_meshset.inc"
};
static NJS_POINT3 kill_area_5_points[4] = {
#include "stg40/final_chase/kill_area_5_points.inc"
};
static NJS_VECTOR kill_area_5_normals[4] = {
#include "stg40/final_chase/kill_area_5_normals.inc"
};
static NJS_MODEL kill_area_5_model = {
#include "stg40/final_chase/kill_area_5_model.inc"
};
static NJS_OBJECT kill_area_5 = {
#include "stg40/final_chase/kill_area_5.inc"
};
static struct kill_area final_chase_kill_areas[7] = {
#include "stg40/final_chase/final_chase_kill_areas.inc"
};

static struct light final_chase_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *final_chase_set = NULL;
/* the background task, or null while there is none */
static struct task *background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
/* clang-format off */
static struct land_block_color final_chase_block_colors[] = {
    {0xff000000,    0x0},
    {0xffffff00,  0x602},
    {0xffff00ff,  0x404},
    {0xff00ffff,  0xc0c},
    {0xffff7d00,  0x808},
    {0xff0000ff, 0x1818},
    {0xff007d00, 0x1010},
    {0xff00007d, 0x3030},
    {0xff7d007d, 0x2020},
    {0xff7d0000, 0x6060},
    {0xffff7dff, 0x4040},
    {0xff007d7d, 0xc0c0},
    {0xffff0000,   0x86},
    {0xff00ff00,  0x180},
    {0xff7d7d00,  0x100},
    {0xff7d7d7d, 0xc300},
    {0xffffffff, 0x4200},
};
/* clang-format on */
static NJS_POINT3 final_chase_block_origin = {
#include "stg40/final_chase/final_chase_block_origin.inc"
};
static NJS_POINT3 final_chase_block_cell = {
#include "stg40/final_chase/final_chase_block_cell.inc"
};
/* the scrolls of the sky's objects */
static Sint16 sky_8_uvs[170] = {
#include "stg40/final_chase/sky_8_uvs.inc"
};
static struct texture_anim sky_8_anim = {
#include "stg40/final_chase/sky_8_anim.inc"
};
static Sint16 sky_5_uvs[314] = {
#include "stg40/final_chase/sky_5_uvs.inc"
};
static struct texture_anim sky_5_anim = {
#include "stg40/final_chase/sky_5_anim.inc"
};
static Sint16 sky_6_uvs[320] = {
#include "stg40/final_chase/sky_6_uvs.inc"
};
static struct texture_anim sky_6_anim = {
#include "stg40/final_chase/sky_6_anim.inc"
};
static Sint16 sky_7_uvs[838] = {
#include "stg40/final_chase/sky_7_uvs.inc"
};
static struct texture_anim sky_7_anim = {
#include "stg40/final_chase/sky_7_anim.inc"
};
static Sint16 sky_4_uvs[176] = {
#include "stg40/final_chase/sky_4_uvs.inc"
};
static struct texture_anim sky_4_anim = {
#include "stg40/final_chase/sky_4_anim.inc"
};
static Sint16 sky_2_uvs[186] = {
#include "stg40/final_chase/sky_2_uvs.inc"
};
static struct texture_anim sky_2_anim = {
#include "stg40/final_chase/sky_2_anim.inc"
};
static Sint16 sky_3_uvs[204] = {
#include "stg40/final_chase/sky_3_uvs.inc"
};
static struct texture_anim sky_3_anim = {
#include "stg40/final_chase/sky_3_anim.inc"
};
static Sint16 sky_1_uvs[444] = {
#include "stg40/final_chase/sky_1_uvs.inc"
};
static struct texture_anim sky_1_anim = {
#include "stg40/final_chase/sky_1_anim.inc"
};
static Sint16 sky_10_uvs[30] = {
#include "stg40/final_chase/sky_10_uvs.inc"
};
static struct texture_anim sky_10_anim = {
#include "stg40/final_chase/sky_10_anim.inc"
};
static Sint16 sky_9_uvs[16] = {
#include "stg40/final_chase/sky_9_uvs.inc"
};
static struct texture_anim sky_9_anim = {
#include "stg40/final_chase/sky_9_anim.inc"
};
static Sint16 sky_14_uvs[458] = {
#include "stg40/final_chase/sky_14_uvs.inc"
};
static struct texture_anim sky_14_anim = {
#include "stg40/final_chase/sky_14_anim.inc"
};
static Sint16 sky_15_uvs[170] = {
#include "stg40/final_chase/sky_15_uvs.inc"
};
static struct texture_anim sky_15_anim = {
#include "stg40/final_chase/sky_15_anim.inc"
};
static Sint16 sky_17_uvs[368] = {
#include "stg40/final_chase/sky_17_uvs.inc"
};
static struct texture_anim sky_17_anim = {
#include "stg40/final_chase/sky_17_anim.inc"
};
static Sint16 sky_18_uvs[398] = {
#include "stg40/final_chase/sky_18_uvs.inc"
};
static struct texture_anim sky_18_anim = {
#include "stg40/final_chase/sky_18_anim.inc"
};
static Sint16 sky_16_uvs[90] = {
#include "stg40/final_chase/sky_16_uvs.inc"
};
static struct texture_anim sky_16_anim = {
#include "stg40/final_chase/sky_16_anim.inc"
};
static Sint16 sky_19_uvs[378] = {
#include "stg40/final_chase/sky_19_uvs.inc"
};
static struct texture_anim sky_19_anim = {
#include "stg40/final_chase/sky_19_anim.inc"
};
static Sint16 sky_20_uvs[170] = {
#include "stg40/final_chase/sky_20_uvs.inc"
};
static struct texture_anim sky_20_anim = {
#include "stg40/final_chase/sky_20_anim.inc"
};
static Sint16 sky_21_uvs[320] = {
#include "stg40/final_chase/sky_21_uvs.inc"
};
static struct texture_anim sky_21_anim = {
#include "stg40/final_chase/sky_21_anim.inc"
};
static Sint16 sky_22_uvs[376] = {
#include "stg40/final_chase/sky_22_uvs.inc"
};
static struct texture_anim sky_22_anim = {
#include "stg40/final_chase/sky_22_anim.inc"
};
static Sint16 sky_26_uvs[122] = {
#include "stg40/final_chase/sky_26_uvs.inc"
};
static struct texture_anim sky_26_anim = {
#include "stg40/final_chase/sky_26_anim.inc"
};
static Sint16 sky_23_uvs[416] = {
#include "stg40/final_chase/sky_23_uvs.inc"
};
static struct texture_anim sky_23_anim = {
#include "stg40/final_chase/sky_23_anim.inc"
};
static Sint16 sky_25_uvs[724] = {
#include "stg40/final_chase/sky_25_uvs.inc"
};
static struct texture_anim sky_25_anim = {
#include "stg40/final_chase/sky_25_anim.inc"
};
static Sint16 sky_24_uvs[672] = {
#include "stg40/final_chase/sky_24_uvs.inc"
};
static struct texture_anim sky_24_anim = {
#include "stg40/final_chase/sky_24_anim.inc"
};
static Sint16 sky_27_uvs[54] = {
#include "stg40/final_chase/sky_27_uvs.inc"
};
static struct texture_anim sky_27_anim = {
#include "stg40/final_chase/sky_27_anim.inc"
};
static Sint16 sky_12_uvs[148] = {
#include "stg40/final_chase/sky_12_uvs.inc"
};
static struct texture_anim sky_12_anim = {
#include "stg40/final_chase/sky_12_anim.inc"
};
static Sint16 sky_13_uvs[148] = {
#include "stg40/final_chase/sky_13_uvs.inc"
};
static struct texture_anim sky_13_anim = {
#include "stg40/final_chase/sky_13_anim.inc"
};
static struct background_scroll background_scrolls[BACKGROUND_SCROLLS] = {
#include "stg40/final_chase/background_scrolls.inc"
};
/* the pulse's color, which the sky objects without an anim take. it runs
 * from dim_color to bright_color and back, its phase turning pulse_rate each
 * step */
static Uint32 dim_color = 0xff101010;
static Uint32 bright_color = 0xffffffff;
static Angle pulse_rate = 0x200;
static Uint32 scroll_rate = 0x23; /* 256ths of a texture a step */
/* each step, the background throws a meteor with the chance meteor_chance.
 * the meteor's speed in y is fall_speed. it drifts up to drift a step either
 * way along x and along z */
static Float meteor_chance = 0.05F;
static Float fall_speed = -50.0F;
static Float drift = 40.0F;
/* a meteor starts meteor_height above camera 0. across x and z, it lies
 * meteor_reach plus up to meteor_spread from the camera, in a random
 * direction */
static Float meteor_reach = 3000.0F;
static Float meteor_spread = 500.0F;
static Float meteor_height = 2500.0F;
/* a meteor's ang.z is the steps that a fall of meteor_drop takes at
 * fall_speed */
static Float meteor_drop = 5000.0F;
/* the size of a spark player 0 sheds: spark_size and up to spark_size_range
 * more */
static Float spark_size = 3.0F;
static Float spark_size_range = 3.5F;
/* the angles models 2 and 3 turn each step, about x and about y */
static Angle background_2_turn_x = 1;
static Angle background_2_turn_y = 2;
static Angle background_3_turn_x = 2;
static Angle background_3_turn_y = 4;
/* the draw lifts the four models background_height above the current
 * camera. it lifts models 0 and 1 their own heights more, and scales them
 * across x and z. it scales models 2 and 3 along every axis */
static Float background_0_height = 1500.0F;
static Float background_0_scale = 1.6F;
static Float background_1_height = 0.0F;
static Float background_1_scale = 1.0F;
static Float background_height = 0.0F;
static Float background_turned_scale = 2.0F;

static Uint8 *background_build(Uint8 *buf)
{
    njFogDisable();
    /* @bug logic: the background's models never compile. the compile runs
     * only while background_built is TRUE, and only a compile sets it TRUE,
     * so the test at 0x8c500030 always fails. Mission Street's build, at
     * 0x8c50003c of STG17.PRS, compiles without that test and sets its flag
     * from the result */
    if (background_built &&
        model_builds_compile(&buf, background_builds, BACKGROUND_MODELS) ==
            BACKGROUND_MODELS) {
        background_built = TRUE;
    } else {
        background_built = FALSE;
    }
    njFogEnable();
    return buf;
}

void final_chase_init(void)
{
    struct task *task;
    /* @hack spelling: set and light, locals for light_set and
     * &final_chase_lights[0], where those by their names would do */
    void (*set)(int index, const struct light *light);
    struct light *light;
    Uint8 *buf;

    world_land_start(&final_chase_land);
    world_load_block_grid("FINALCHASE.BMP", final_chase_block_colors,
                          &final_chase_block_origin, &final_chase_block_cell);
    texture_load_shared(final_chase_pvms, final_chase_texture_groups);
    texture_await_shared(final_chase_pvms, final_chase_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    final_chase_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&final_chase_objects, final_chase_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    set = light_set;
    light = &final_chase_lights[0];
    set(0, light);
    set(1, &final_chase_lights[1]);
    set(2, light);
    set(3, light);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg40_light.bin");
    buf = (Uint8 *)world_build_land(&final_chase_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = object_builds_run(buf);
    background_build(buf);
    music_fade_in(0);
    music_play_or_defer("f_chase.adx");
    path_create_manager(final_chase_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(final_chase_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: a second test of drop_rings, which the first test
     * leaves non-null */
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    njFogEnable();
    messenger_messages_load("mh0040");
    hint_box_hints_load("bh0040");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_ac_fc.mlt", &final_chase_sound_list);
    object_builds_init();
    debris_friction = 0.99F;
    debris_gravity = 0.03F;
    debris_life = 90;
    debris_life_spread = 30.0F;
}

void final_chase_exit(void)
{
    object_builds_exit();
    fog_stop();
    njFogDisable();
    hint_box_hints_free();
    messenger_messages_free();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(final_chase_pvms, final_chase_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (final_chase_set != NULL) {
        loader_free(final_chase_set);
        final_chase_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void final_chase_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

/* player_fall_out makes a fall's miss kill_areas_task's child */
static void background_step(struct task *task)
{
    struct task *meteor;
    NJS_POINT3 pos;
    struct background_scroll *scroll;
    Uint32 steps;
    Uint32 color;
    int offset;
    Uint32 i;

    if (camera_views[0] != NULL && RANDOM_UNIT() < meteor_chance) {
        pos.x = (RANDOM_UNIT() * meteor_spread) + meteor_reach;
        pos.y = meteor_height;
        pos.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&camera_views[0]->pos);
        njRotateY(NULL, RANDOM_TWO_TURNS());
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrixEx();
        meteor = task_create(TASK_WORK, METEOR_LEVEL, meteor_fall_init,
                             "ObjectMeteoFall");
        if (meteor != NULL) {
            meteor->work->pos = pos;
            meteor->work->scl.y = fall_speed;
            meteor->work->scl.z = drift * ((RANDOM_UNIT() * 2.0F) - 1.0F);
            meteor->work->scl.x = drift * ((RANDOM_UNIT() * 2.0F) - 1.0F);
            meteor->work->ang.z = _builtin_fabsf(meteor_drop / fall_speed);
        }
    }
    /* @hack spelling: steps set inside the product, where a statement of its
     * own would do */
    /* @hack type: the offset cut to a Uint16, where the whole product would
     * do: the scrolls wrap at SCROLL_WRAP, which divides 0x10000 */
    offset = (Uint16)((steps = game_state.steps) * scroll_rate);
    scroll = background_scrolls;
    /* @hack type: the steps cast to an Angle before the product, where the
     * product cast would do */
    color = color_blend(dim_color, bright_color,
                        (njSin((Angle)steps * pulse_rate) * 0.5F) + 0.5F);
    for (i = 0; i < BACKGROUND_SCROLLS; i++, scroll++) {
        if (scroll->anim != NULL) {
            texture_anim_scroll(scroll->object->model, scroll->anim,
                                (scroll->u_speed * offset) & (SCROLL_WRAP - 1),
                                (scroll->v_speed * offset) & (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(scroll->object->model->vlist, color);
        }
    }
    if (player_work[0] != NULL && kill_areas_task != NULL &&
        kill_areas_task->child != NULL && RANDOM_UNIT() > SPARK_ABOVE) {
        burst_create_wobbling(&player_work[0]->pos, &player_motion[0]->spd,
                              (RANDOM_UNIT() * spark_size_range) + spark_size);
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

#define SPACE_SKY_LAND final_chase_sky_land
#include "shared/space_sky_draw.h"
