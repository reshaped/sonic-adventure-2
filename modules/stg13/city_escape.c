/* City Escape, the stage of STG13. its background task, BgExec, draws the
 * skyline about the current camera. the task also draws five pairs of models
 * at their own places */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/hud.h"
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
#include "stg13/car.h"
#include "stg13/city_escape.h"
#include "stg13/city_escape/text.inc"
#include "stg13/crash_piece.h"
#include "stg13/model_objects.h"
#include "stg13/paths.h"
#include "stg13/road_objects.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* the fog city_escape_init reads from fog_file */
static struct fog_setting city_escape_fog;
/* the skyline and its texture list, which the source defines at its end, in
 * section DATA */
static NJS_TEXLIST background_texlist;
static NJS_CNK_OBJECT background_object;

static const char block_map[16] = "CITY_ESCAPE.BMP";
static const char light_file[16] = "stg13_light.bin";
static const char board_music[16] = "c_escap1.adx";
static const char sonic_music[16] = "t9_sonic.adx";
static const char background_task_name[8] = "BgExec";
static const char fog_file[16] = "stg13_fog.bin";
static const char mission_hint_file[8] = "mh0013";
static const char hint_file[8] = "bh0013";
static const char sound_bank[16] = "se_ac_ce.mlt";
static const char skyline_near_name[12] = {
#include "stg13/city_escape/skyline_near_name.inc"
};
static const char skyline_middle_name[12] = {
#include "stg13/city_escape/skyline_middle_name.inc"
};
static const char skyline_far_name[12] = {
#include "stg13/city_escape/skyline_far_name.inc"
};

/* the skyline's PVM. texture_load_shared gives each texture of
 * background_texlist the PVM's texture of the same name */
typedef NJS_TEXLIST *city_background_textures;

static NJS_TEXNAME background_pvm_texnames[3] = {
#include "stg13/city_escape/background_pvm_texnames.inc"
};
static NJS_TEXLIST background_pvm_texlist = {
#include "stg13/city_escape/background_pvm_texlist.inc"
};
static NJS_TEXLIST *background_group[2] = {
#include "stg13/city_escape/background_group.inc"
};
static city_background_textures *background_texture_groups[2] = {
#include "stg13/city_escape/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg13/city_escape/background_pvms.inc"
};
/* the skyline as background_build compiled it, or null where it didn't fit
 * below DEMO_AREA */
static NJS_DIRECT_HEAD *background_compiled = NULL;

/* the stage's three kill areas, each a flat rectangle that kills every
 * character */
static NJS_MATERIAL kill_area_0_material = {
#include "stg13/city_escape/kill_area_0_material.inc"
};
static Uint16 kill_area_0_mesh[5] = {
#include "stg13/city_escape/kill_area_0_mesh.inc"
};
static NJS_MESHSET kill_area_0_meshset = {
#include "stg13/city_escape/kill_area_0_meshset.inc"
};
static NJS_POINT3 kill_area_0_points[4] = {
#include "stg13/city_escape/kill_area_0_points.inc"
};
static NJS_VECTOR kill_area_0_normals[4] = {
#include "stg13/city_escape/kill_area_0_normals.inc"
};
static NJS_MODEL kill_area_0_model = {
#include "stg13/city_escape/kill_area_0_model.inc"
};
static NJS_OBJECT kill_area_0_object = {
#include "stg13/city_escape/kill_area_0_object.inc"
};
static NJS_MATERIAL kill_area_1_material = {
#include "stg13/city_escape/kill_area_1_material.inc"
};
static Uint16 kill_area_1_mesh[5] = {
#include "stg13/city_escape/kill_area_1_mesh.inc"
};
static NJS_MESHSET kill_area_1_meshset = {
#include "stg13/city_escape/kill_area_1_meshset.inc"
};
static NJS_POINT3 kill_area_1_points[4] = {
#include "stg13/city_escape/kill_area_1_points.inc"
};
static NJS_VECTOR kill_area_1_normals[4] = {
#include "stg13/city_escape/kill_area_1_normals.inc"
};
static NJS_MODEL kill_area_1_model = {
#include "stg13/city_escape/kill_area_1_model.inc"
};
static NJS_OBJECT kill_area_1_object = {
#include "stg13/city_escape/kill_area_1_object.inc"
};
static NJS_MATERIAL kill_area_2_material = {
#include "stg13/city_escape/kill_area_2_material.inc"
};
static Uint16 kill_area_2_mesh[5] = {
#include "stg13/city_escape/kill_area_2_mesh.inc"
};
static NJS_MESHSET kill_area_2_meshset = {
#include "stg13/city_escape/kill_area_2_meshset.inc"
};
static NJS_POINT3 kill_area_2_points[4] = {
#include "stg13/city_escape/kill_area_2_points.inc"
};
static NJS_VECTOR kill_area_2_normals[4] = {
#include "stg13/city_escape/kill_area_2_normals.inc"
};
static NJS_MODEL kill_area_2_model = {
#include "stg13/city_escape/kill_area_2_model.inc"
};
static NJS_OBJECT kill_area_2_object = {
#include "stg13/city_escape/kill_area_2_object.inc"
};
static struct kill_area city_escape_kill_areas[4] = {
#include "stg13/city_escape/city_escape_kill_areas.inc"
};

static struct light city_escape_light = {
    {0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F};
static struct light city_escape_light_1 = {
    {0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F};
/* the set file city_escape_init read, until city_escape_exit frees it */
static struct set_head *city_escape_set = NULL;
/* the background task, for background_destroy's test */
static struct task *background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
static struct land_block_color city_escape_block_colors[] = {
    {0xff000000, 0xffffffff}, {0xffffff00, 0x18432}, {0xffff0000, 0x3d826},
    {0xffff00ff, 0xc0104},    {0xff00ffff, 0x830c},  {0xffff7d00, 0x46008},
    {0xff0000ff, 0x218},      {0xff007d00, 0x60cd0}, {0xff7d007d, 0x10860},
    {0xff7d0000, 0x3040},     {0xff007d7d, 0x1080},  {0xff00ff00, 0x4200},
    {0xff00007d, 0x2180},
};
static NJS_POINT3 city_escape_block_origin = {-3000.0F, 0.0F, 16640.0F};
static NJS_POINT3 city_escape_block_cell = {33.14917F, 0.0F, 33.203125F};

/* the background's pairs of models: a stand, and its child, a face. each
 * pair draws at its place in background_places, the stand turned a quarter
 * turn about y. the face's bearing is its angle about y from the view's
 * forward axis. the face turns about y to the camera's ang.y, less
 * bearing_share times its bearing */
#define BACKGROUND_PAIRS 5
/* a pair's models in background_builds, in order */
enum { STAND, FACE, PAIR_MODELS };
static NJS_TEXNAME background_0_texnames[3] = {
#include "stg13/city_escape/background_0_texnames.inc"
};
static NJS_TEXLIST background_0_texlist = {
#include "stg13/city_escape/background_0_texlist.inc"
};
static Sint16 background_0_face_polygons[193] = {
#include "stg13/city_escape/background_0_face_polygons.inc"
};
static Sint32 background_0_face_vertices[163] = {
#include "stg13/city_escape/background_0_face_vertices.inc"
};
static NJS_CNK_MODEL background_0_face_model = {
#include "stg13/city_escape/background_0_face_model.inc"
};
static NJS_CNK_OBJECT background_0_face = {
#include "stg13/city_escape/background_0_face.inc"
};
static Sint16 background_0_stand_polygons[45] = {
#include "stg13/city_escape/background_0_stand_polygons.inc"
};
static Sint32 background_0_stand_vertices[35] = {
#include "stg13/city_escape/background_0_stand_vertices.inc"
};
static NJS_CNK_MODEL background_0_stand_model = {
#include "stg13/city_escape/background_0_stand_model.inc"
};
static NJS_CNK_OBJECT background_0_stand = {
#include "stg13/city_escape/background_0_stand.inc"
};
static NJS_TEXNAME background_1_texnames[3] = {
#include "stg13/city_escape/background_1_texnames.inc"
};
static NJS_TEXLIST background_1_texlist = {
#include "stg13/city_escape/background_1_texlist.inc"
};
static Sint16 background_1_face_polygons[193] = {
#include "stg13/city_escape/background_1_face_polygons.inc"
};
static Sint32 background_1_face_vertices[163] = {
#include "stg13/city_escape/background_1_face_vertices.inc"
};
static NJS_CNK_MODEL background_1_face_model = {
#include "stg13/city_escape/background_1_face_model.inc"
};
static NJS_CNK_OBJECT background_1_face = {
#include "stg13/city_escape/background_1_face.inc"
};
static Sint16 background_1_stand_polygons[51] = {
#include "stg13/city_escape/background_1_stand_polygons.inc"
};
static Sint32 background_1_stand_vertices[43] = {
#include "stg13/city_escape/background_1_stand_vertices.inc"
};
static NJS_CNK_MODEL background_1_stand_model = {
#include "stg13/city_escape/background_1_stand_model.inc"
};
static NJS_CNK_OBJECT background_1_stand = {
#include "stg13/city_escape/background_1_stand.inc"
};
static NJS_TEXNAME background_2_texnames[3] = {
#include "stg13/city_escape/background_2_texnames.inc"
};
static NJS_TEXLIST background_2_texlist = {
#include "stg13/city_escape/background_2_texlist.inc"
};
static Sint16 background_2_face_polygons[193] = {
#include "stg13/city_escape/background_2_face_polygons.inc"
};
static Sint32 background_2_face_vertices[163] = {
#include "stg13/city_escape/background_2_face_vertices.inc"
};
static NJS_CNK_MODEL background_2_face_model = {
#include "stg13/city_escape/background_2_face_model.inc"
};
static NJS_CNK_OBJECT background_2_face = {
#include "stg13/city_escape/background_2_face.inc"
};
static Sint16 background_2_stand_polygons[67] = {
#include "stg13/city_escape/background_2_stand_polygons.inc"
};
static Sint32 background_2_stand_vertices[75] = {
#include "stg13/city_escape/background_2_stand_vertices.inc"
};
static NJS_CNK_MODEL background_2_stand_model = {
#include "stg13/city_escape/background_2_stand_model.inc"
};
static NJS_CNK_OBJECT background_2_stand = {
#include "stg13/city_escape/background_2_stand.inc"
};
static NJS_TEXNAME background_3_texnames[3] = {
#include "stg13/city_escape/background_3_texnames.inc"
};
static NJS_TEXLIST background_3_texlist = {
#include "stg13/city_escape/background_3_texlist.inc"
};
static Sint16 background_3_face_polygons[193] = {
#include "stg13/city_escape/background_3_face_polygons.inc"
};
static Sint32 background_3_face_vertices[163] = {
#include "stg13/city_escape/background_3_face_vertices.inc"
};
static NJS_CNK_MODEL background_3_face_model = {
#include "stg13/city_escape/background_3_face_model.inc"
};
static NJS_CNK_OBJECT background_3_face = {
#include "stg13/city_escape/background_3_face.inc"
};
static Sint16 background_3_stand_polygons[67] = {
#include "stg13/city_escape/background_3_stand_polygons.inc"
};
static Sint32 background_3_stand_vertices[75] = {
#include "stg13/city_escape/background_3_stand_vertices.inc"
};
static NJS_CNK_MODEL background_3_stand_model = {
#include "stg13/city_escape/background_3_stand_model.inc"
};
static NJS_CNK_OBJECT background_3_stand = {
#include "stg13/city_escape/background_3_stand.inc"
};
static NJS_TEXNAME background_4_texnames[3] = {
#include "stg13/city_escape/background_4_texnames.inc"
};
static NJS_TEXLIST background_4_texlist = {
#include "stg13/city_escape/background_4_texlist.inc"
};
static Sint16 background_4_face_polygons[193] = {
#include "stg13/city_escape/background_4_face_polygons.inc"
};
static Sint32 background_4_face_vertices[163] = {
#include "stg13/city_escape/background_4_face_vertices.inc"
};
static NJS_CNK_MODEL background_4_face_model = {
#include "stg13/city_escape/background_4_face_model.inc"
};
static NJS_CNK_OBJECT background_4_face = {
#include "stg13/city_escape/background_4_face.inc"
};
static Sint16 background_4_stand_polygons[45] = {
#include "stg13/city_escape/background_4_stand_polygons.inc"
};
static Sint32 background_4_stand_vertices[35] = {
#include "stg13/city_escape/background_4_stand_vertices.inc"
};
static NJS_CNK_MODEL background_4_stand_model = {
#include "stg13/city_escape/background_4_stand_model.inc"
};
static NJS_CNK_OBJECT background_4_stand = {
#include "stg13/city_escape/background_4_stand.inc"
};
static struct model_build background_builds[BACKGROUND_PAIRS * PAIR_MODELS] = {
#include "stg13/city_escape/background_builds.inc"
};
NJS_TEXLIST *background_build_texlists[6] = {
#include "stg13/city_escape/background_build_texlists.inc"
};
static NJS_POINT3 background_places[BACKGROUND_PAIRS] = {
#include "stg13/city_escape/background_places.inc"
};
static Float bearing_share = 0.5F;

/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
/* the background task's level */
#define BACKGROUND_LEVEL 1
/* the light both players take, which camera_apply hands Ninja too */
#define PLAYER_LIGHT 0
#define OPENING_VOICE 1806
#define OPENING_VOICE_PRIORITY 0
#define OPENING_VOICE_FRAME 10
#define FALL_PLAYERS 2
#define FALL_OUT_HEIGHT (-22195.0F)
/* a pair draws while its place's depth in the view lies between
 * BACKGROUND_NEAR and BACKGROUND_FAR */
#define BACKGROUND_NEAR (-80.0F)
#define BACKGROUND_FAR 1500.0F

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);
static Uint8 *background_build(Uint8 *buf);

void city_escape_init(void)
{
    struct task *task;
    Uint8 *buf;
    /* @hack spelling: set and light, locals for light_set and
     * &city_escape_light, where those by their names would do */
    void (*set)(int index, const struct light *light);
    struct light *light;

    world_land_start(&city_escape_land);
    world_load_block_grid((char *)block_map, city_escape_block_colors,
                          &city_escape_block_origin, &city_escape_block_cell);
    texture_load_shared(city_escape_object_pvms,
                        city_escape_object_texture_groups);
    texture_await_shared(city_escape_object_pvms,
                         city_escape_object_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    city_escape_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&city_escape_objects, city_escape_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    set = light_set;
    light = &city_escape_light;
    set(PLAYER_LIGHT, light);
    set(1, &city_escape_light_1);
    set(LIGHT_LAND, light);
    set(3, light);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(PLAYER_LIGHT);
    light_select_after_draw(LIGHT_LAND);
    lights_load(light_file);
    buf = (Uint8 *)world_build_land_at_origin(&city_escape_land,
                                              (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = background_build(buf);
    buf = car_build(buf);
    buf = object_builds_run(buf);
    buf = model_objects_build(buf);
    buf = road_objects_build(buf);
    music_fade_in(0);
    if (game_state.demo == 0) {
        music_play_or_defer(board_music);
    } else {
        music_play_or_defer(sonic_music);
    }
    path_create_manager(city_escape_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step,
                       background_task_name);
    if (task != NULL) {
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(city_escape_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    /* @hack redundant: the test and the set made again, though the first
     * set drop_rings */
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    fog_start_file(fog_file, &city_escape_fog);
    njFogEnable();
    object_light_create();
    messenger_messages_load(mission_hint_file);
    hint_box_hints_load(hint_file);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage(sound_bank, &city_escape_sound_list);
    object_builds_init();
}

void city_escape_exit(void)
{
    object_builds_exit();
    hint_box_hints_free();
    messenger_messages_free();
    object_light_destroy();
    fog_stop();
    njFogDisable();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(city_escape_object_pvms,
                           city_escape_object_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (city_escape_set != NULL) {
        loader_free(city_escape_set);
        city_escape_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void city_escape_step(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 minute;
    Sint8 second;
    Sint8 frame;
    int player;

    stage_clock_get(&minute, &second, &frame);
    if (minute == 0 && second == 0 && frame == OPENING_VOICE_FRAME &&
        game_state.mission == MISSION_FIRST && game_state.demo == 0) {
        voice_play(OPENING_VOICE, OPENING_VOICE_PRIORITY);
    }
    if (kill_areas_task != NULL && kill_areas_task->child == NULL) {
        for (player = 0; player < FALL_PLAYERS; player++) {
            if (player_work[player] != NULL &&
                !(player_records[player]->flags & PLAYER_MISSED)) {
                /* @hack spelling: the height tested in an if of its own, where
                 * one test of all three would do */
                if (player_work[player]->pos.y < FALL_OUT_HEIGHT) {
                    player_fall_out(player);
                }
            }
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

static void background_destroy(struct task *task)
{
    /* @bug logic: the test never holds. no code stores a task in
     * background_task, whose only read is at 0x8c500478 */
    if (background_task == task) {
        background_task = NULL;
    }
}

static void background_draw(struct task *task)
{
    NJS_POINT3 camera;
    NJS_POINT3 point;
    struct model_build *build;
    NJS_POINT3 *place;
    /* @hack spelling: texlist, a local for &background_texlist, where that
     * passed in place would do */
    NJS_TEXLIST *texlist;
    Angle bearing;
    Sint16 *bearing_low;
    Float turn;
    int i;

    camera.x = camera_views[camera_current_index]->pos.x;
    camera.y = camera_views[camera_current_index]->pos.y;
    camera.z = camera_views[camera_current_index]->pos.z;
    njPushMatrixEx();
    njTranslateV(NULL, &camera);
    texlist = &background_texlist;
    njSetTexture(texlist);
    if (background_compiled != NULL) {
        njDirectDrawObject(background_compiled);
    } else {
        njCnkEasyDrawObject(&background_object);
    }
    njFogEnable();
    matrix_load_parent();
    place = background_places;
    build = background_builds;
    for (i = 0; i < BACKGROUND_PAIRS; i++, place++, build += PAIR_MODELS) {
        point = *place;
        njCalcPoint(NULL, &point, &point);
        if (point.z > BACKGROUND_NEAR && point.z < BACKGROUND_FAR) {
            njSetTexture(build[STAND].texlist);
            njTranslateEx(place);
            njRotateY(NULL, -QUARTER_TURN);
            if (build[STAND].built != NULL) {
                njDirectDrawObject(build[STAND].built);
            } else {
                njCnkEasyDrawModel(build[STAND].object->model);
            }
            njTranslateEx((NJS_VECTOR *)build[FACE].object->pos);
            njGetTranslation(NULL, &point);
            bearing = atan2f(point.x, point.z) * ANGLE_PER_RADIAN;
            /* @hack type: the bearing's low half scaled through a Sint16
             * pointer, where its low 16 bits masked and stored back would
             * do */
            bearing_low = (Sint16 *)&bearing;
            /* @hack spelling: the scaling through turn in three statements,
             * where *bearing_low = *bearing_low * bearing_share would do */
            turn = *bearing_low;
            turn *= bearing_share;
            *bearing_low = turn;
            njRotateY(NULL, -bearing +
                                camera_views[camera_current_index]->ang.y +
                                QUARTER_TURN);
            if (build[FACE].built != NULL) {
                njDirectDrawObject(build[FACE].built);
            } else {
                njCnkEasyDrawModel(build[FACE].object->model);
            }
            matrix_load_parent();
        }
    }
    njPopMatrixEx();
}

static Uint8 *background_build(Uint8 *buf)
{
    Uint8 *end;

    njFogDisable();
    njSetTexture(&background_texlist);
    end = buf + njCnkDirectObjectCompileSize(&background_object);
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&background_object, buf);
        background_compiled = (NJS_DIRECT_HEAD *)buf;
        buf = end;
    } else {
        background_compiled = NULL;
    }
    njFogEnable();
    model_builds_compile(&buf, background_builds,
                         BACKGROUND_PAIRS * PAIR_MODELS);
    return buf;
}

#pragma section ATA
static NJS_TEXNAME background_textures[3] = {
#include "stg13/city_escape/background_textures.inc"
};

static NJS_TEXLIST background_texlist = {
#include "stg13/city_escape/background_texlist.inc"
};

static Sint16 background_polygons[815] = {
#include "stg13/city_escape/background_polygons.inc"
};

static Sint32 background_vertices[459] = {
#include "stg13/city_escape/background_vertices.inc"
};

static NJS_CNK_MODEL background_model = {
#include "stg13/city_escape/background_model.inc"
};

static NJS_CNK_OBJECT background_object = {
#include "stg13/city_escape/background_object.inc"
};
#pragma section
