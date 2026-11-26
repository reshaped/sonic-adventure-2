/* Lost Colony's setup, exit and step, and its background task. the step sets
 * the fog from a light level that flashes raise. the flashes are bursts near
 * the first camera, FOGCOL zones, capsule bombs and dynamite. the background
 * task selects the flash light for each player in a light volume. it also
 * draws the sky and the land tables of stg27/scenery_land.h for each camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "system/port.h"
#include "stg27/lost_colony/text.inc"
#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/bomb_hat.h"
#include "shared/capsule_bomb.h"
#include "shared/common_object.h"
#include "shared/dynamite.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_icon.h"
#include "shared/kazobj.h"
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
#include "stg27/light.h"
#include "stg27/lost_colony.h"
#include "stg27/object_archives.h"
#include "stg27/scenery_land.h"
#include "stg27/sounds.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

static struct fog_setting colony_dark_fog;
static struct fog_setting colony_lit_fog;
static struct fog_setting colony_clear_fog;

#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
#define BURST_RADIUS 400.0F
#define MAX_FLASH 5.0F
#define FOG_KNEE 0.9F
/* the fog's level rises by KNEE_SLOPE for each unit of light past FOG_KNEE */
#define KNEE_SLOPE (0.1F * 0.2F)
/* the light past FOG_KNEE at which the fog's level reaches 1 */
#define KNEE_SPAN 5.0F
#define SCENERY_DRAW_DEPTH 2000.0F
/* the sky draws only while the camera lies SKY_HIDE_RADIUS or more from
 * (SKY_HIDE_X, SKY_HIDE_Z) in x and z */
#define SKY_HIDE_X 2500.0F
#define SKY_HIDE_Z (-2400.0F)
#define SKY_HIDE_RADIUS 500.0F
#define WHITE 0xffffffff
#define BLACK 0xff000000
#define DIM_LAMP 0xff202020
/* a level from 0 to 1 that follows the sine of angle */
#define PULSE_BRIGHTNESS(angle) ((njSin(angle) * 0.5F) + 0.5F)
#define POISON_FRAMES 13
#define POISON_FRAME_STEPS 2
/* the stage's light level, a Float in the bits of its task's unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define LIGHT_LEVEL(task) (*(Float *)&(task)->unk_3c)
/* the level eased toward LIGHT_LEVEL, by light_smoothing of the gap a step.
 * it sets the fog */
#define EASED_LEVEL(work) ((work)->scl.x)

/* a model that background_step colors from black to white and back */
struct pulse_model {
    NJS_CNK_OBJECT *object;
    int phase; /* how far its pulse runs ahead, as a count of quarter turns */
};

/* no paths: the list holds only the null that ends it */
static struct path *lost_colony_paths[] = {NULL};

/* the textures of the PVM bgtex27. each list of background_textures takes
 * its textures from these by name */
static NJS_TEXNAME colony_background_names[19] = {
#include "stg27/lost_colony/colony_background_names.inc"
};

NJS_TEXLIST colony_background_texlist = {
#include "stg27/lost_colony/colony_background_texlist.inc"
};

static NJS_TEXNAME colony_sky_names[2] = {
#include "stg27/lost_colony/colony_sky_names.inc"
};

static NJS_TEXLIST colony_sky_texlist = {
#include "stg27/lost_colony/colony_sky_texlist.inc"
};

static Sint16 colony_sky_polygons[921] = {
#include "stg27/lost_colony/colony_sky_polygons.inc"
};

static Sint32 colony_sky_vertices[463] = {
#include "stg27/lost_colony/colony_sky_vertices.inc"
};

static NJS_CNK_MODEL colony_sky_model = {
#include "stg27/lost_colony/colony_sky_model.inc"
};

static NJS_CNK_OBJECT colony_sky = {
#include "stg27/lost_colony/colony_sky.inc"
};

/* sky_copy_object takes colony_sky's layouts and textures. no code reads it */
static NJS_TEXNAME sky_copy_names[2] = {
#include "stg27/lost_colony/sky_copy_names.inc"
};

static NJS_TEXLIST sky_copy_texlist = {
#include "stg27/lost_colony/sky_copy_texlist.inc"
};

static Sint16 sky_copy_polygons[921] = {
#include "stg27/lost_colony/sky_copy_polygons.inc"
};

static Sint32 sky_copy_vertices[463] = {
#include "stg27/lost_colony/sky_copy_vertices.inc"
};

static NJS_CNK_MODEL sky_copy_model = {
#include "stg27/lost_colony/sky_copy_model.inc"
};

static NJS_CNK_OBJECT sky_copy_object = {
#include "stg27/lost_colony/sky_copy_object.inc"
};

static Bool colony_sky_built = FALSE;
static struct model_build colony_sky_build[1] = {
#include "stg27/lost_colony/colony_sky_build.inc"
};
static NJS_TEXLIST *background_textures[6] = {
#include "stg27/lost_colony/background_textures.inc"
};
typedef NJS_TEXLIST *colony_texture_group;

static colony_texture_group *colony_bg_groups[2] = {
#include "stg27/lost_colony/colony_bg_groups.inc"
};
static struct texture_pvm colony_bg_pvms[2] = {
#include "stg27/lost_colony/colony_bg_pvms.inc"
};

static NJS_MATERIAL kill_0_solid_materials = {
#include "stg27/lost_colony/kill_0_solid_materials.inc"
};

static Uint16 kill_0_solid_strips[5] = {
#include "stg27/lost_colony/kill_0_solid_strips.inc"
};

static NJS_MESHSET kill_0_solid_faces = {
#include "stg27/lost_colony/kill_0_solid_faces.inc"
};

static NJS_POINT3 kill_0_solid_points[4] = {
#include "stg27/lost_colony/kill_0_solid_points.inc"
};

static NJS_VECTOR kill_0_solid_normals[4] = {
#include "stg27/lost_colony/kill_0_solid_normals.inc"
};

static NJS_MODEL kill_0_solid_model = {
#include "stg27/lost_colony/kill_0_solid_model.inc"
};

static NJS_OBJECT colony_kill_0 = {
#include "stg27/lost_colony/colony_kill_0.inc"
};

static NJS_MATERIAL kill_1_solid_materials = {
#include "stg27/lost_colony/kill_1_solid_materials.inc"
};

static Uint16 kill_1_solid_strips[5] = {
#include "stg27/lost_colony/kill_1_solid_strips.inc"
};

static NJS_MESHSET kill_1_solid_faces = {
#include "stg27/lost_colony/kill_1_solid_faces.inc"
};

static NJS_POINT3 kill_1_solid_points[4] = {
#include "stg27/lost_colony/kill_1_solid_points.inc"
};

static NJS_VECTOR kill_1_solid_normals[4] = {
#include "stg27/lost_colony/kill_1_solid_normals.inc"
};

static NJS_MODEL kill_1_solid_model = {
#include "stg27/lost_colony/kill_1_solid_model.inc"
};

static NJS_OBJECT colony_kill_1 = {
#include "stg27/lost_colony/colony_kill_1.inc"
};

static NJS_MATERIAL kill_2_solid_materials = {
#include "stg27/lost_colony/kill_2_solid_materials.inc"
};

static Uint16 kill_2_solid_strips[5] = {
#include "stg27/lost_colony/kill_2_solid_strips.inc"
};

static NJS_MESHSET kill_2_solid_faces = {
#include "stg27/lost_colony/kill_2_solid_faces.inc"
};

static NJS_POINT3 kill_2_solid_points[4] = {
#include "stg27/lost_colony/kill_2_solid_points.inc"
};

static NJS_VECTOR kill_2_solid_normals[4] = {
#include "stg27/lost_colony/kill_2_solid_normals.inc"
};

static NJS_MODEL kill_2_solid_model = {
#include "stg27/lost_colony/kill_2_solid_model.inc"
};

static NJS_OBJECT colony_kill_2 = {
#include "stg27/lost_colony/colony_kill_2.inc"
};

static NJS_MATERIAL kill_3_solid_materials = {
#include "stg27/lost_colony/kill_3_solid_materials.inc"
};

static Uint16 kill_3_solid_strips[5] = {
#include "stg27/lost_colony/kill_3_solid_strips.inc"
};

static NJS_MESHSET kill_3_solid_faces = {
#include "stg27/lost_colony/kill_3_solid_faces.inc"
};

static NJS_POINT3 kill_3_solid_points[4] = {
#include "stg27/lost_colony/kill_3_solid_points.inc"
};

static NJS_VECTOR kill_3_solid_normals[4] = {
#include "stg27/lost_colony/kill_3_solid_normals.inc"
};

static NJS_MODEL kill_3_solid_model = {
#include "stg27/lost_colony/kill_3_solid_model.inc"
};

static NJS_OBJECT colony_kill_3 = {
#include "stg27/lost_colony/colony_kill_3.inc"
};

static struct kill_area kill_areas_list[5] = {
#include "stg27/lost_colony/kill_areas_list.inc"
};
static struct light colony_lights[] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.45F, 1.0F, 1.0F, 1.0F},
    {{0.3F, 0.1F, 1.0F}, 0.5F, 0.43F, 1.0F, 1.0F, 1.0F},
};
static struct set_head *colony_set = NULL;
static struct task *background_task = NULL;
/* LOST.BMP's colors, each with the land blocks drawn where the camera stands
 * on it. -1 draws every block */
/* clang-format off */
static struct land_block_color block_colors[] = {
    {0xff000000,    -1},
    {0xffff0000, 0x222},
    {0xffffff00, 0x666},
    {0xff00ffff, 0x044},
    {0xff0000ff, 0x0cc},
    {0xffff00ff, 0x088},
    {0xff00ff00, 0x198},
    {0xffff6400, 0x110},
    {0xff006400, 0x330},
};
/* clang-format on */
static NJS_POINT3 grid_origin = {
#include "stg27/lost_colony/grid_origin.inc"
};
static NJS_POINT3 grid_cell = {
#include "stg27/lost_colony/grid_cell.inc"
};
static Float burst_gain = 0.1F;
Float colony_flash = 0.0F;
static Float light_decay = 0.968F;
/* no code reads it */
static Float dat_8c547ec8 = 1.5F;
static Float light_smoothing = 0.38F;
static Float dynamite_gain = 0.8F;
/* the scenery objects of stg27/scenery_land.h, a table no code reads */
static NJS_CNK_OBJECT *scenery_models[9] = {
#include "stg27/lost_colony/scenery_models.inc"
};
KEPT static struct pulse_model pulse_models[3] = {
#include "stg27/lost_colony/pulse_models.inc"
};
/* no code reads it */
static Sint32 dat_8c547f10 = 4;
static Angle pulse_speed = 600;
static Float pulse_phase_scale = 15.0F;
static Bool light_overridden[GAME_PLAYERS] = {FALSE, FALSE};
Bool colony_light_request[GAME_PLAYERS] = {FALSE, FALSE};
static Sint32 previous_light[GAME_PLAYERS] = {0, 0};

/* the rates a sky layer turns by, in angle units a step. Eternal Engine and
 * Meteor Herd hold the same four. Lost Colony's sky doesn't turn, and no code
 * reads them */
static Angle unused_outer_sky_pitch = 1;
static Angle unused_outer_sky_yaw = 2;
static Angle unused_inner_sky_pitch = 2;
static Angle unused_inner_sky_yaw = 4;

static void add_burst_light(NJS_POINT3 *pos, NJS_VECTOR *speed, Float scale,
                            Float fade);
static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

static Uint8 *build_sky(Uint8 *buffer)
{
    njFogDisable();
    if (model_builds_compile(&buffer, colony_sky_build, 1) == 1) {
        colony_sky_built = TRUE;
    }
    njFogEnable();
    return buffer;
}

void lost_colony_init(void)
{
    struct task *task;
    Uint8 *cursor;
    void (*set_light)(int, const struct light *);

    objects_fog_off = TRUE;
    world_land_start(&lost_colony_land);
    world_load_block_grid("LOST.BMP", block_colors, &grid_origin, &grid_cell);
    texture_load_shared(colony_bg_pvms, colony_bg_groups);
    texture_await_shared(colony_bg_pvms, colony_bg_groups);
    texture_load_shared(colony_object_pvms, colony_object_groups);
    texture_await_shared(colony_object_pvms, colony_object_groups);
    colony_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&lost_colony_objects, colony_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    /* @hack spelling: light_set called through a local, where direct calls
     * would do */
    set_light = light_set;
    set_light(0, &colony_lights[0]);
    set_light(1, &colony_lights[1]);
    set_light(LIGHT_LAND, &colony_lights[0]);
    set_light(FLASH_LIGHT, &colony_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg27_light.bin");
    cursor = (Uint8 *)world_build_land(&lost_colony_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    cursor = object_builds_run(cursor);
    cursor = build_sky(cursor);
    cursor = capsule_bomb_build(cursor);
    kazobj_build(cursor);
    music_fade_in(0);
    music_play_or_defer("l_colony.adx");
    path_create_manager(lost_colony_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(kill_areas_list);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.draw_blast_ring == NULL) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    njFogEnable();
    messenger_messages_load("mh0027");
    hint_box_hints_load("bh0027");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    fog_start();
    fog_read("stg27_fogA.bin", &colony_dark_fog);
    fog_read("stg27_fogB.bin", &colony_lit_fog);
    fog_clear(&colony_clear_fog);
    object_builds_init();
    burst_hook = add_burst_light;
    blast_load_texture();
    sound_load_stage("se_ac_lc.mlt", &lost_colony_sounds);
}

void lost_colony_exit(void)
{
    burst_hook = NULL;
    object_builds_exit();
    fog_stop();
    njFogDisable();
    hint_box_hints_free();
    messenger_messages_free();
    if (player_hooks.draw_blast_ring == bomb_hat_draw_ring) {
        player_hooks.draw_blast_ring = NULL;
    }
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    particle_core_stop();
    texture_release_shared(colony_object_pvms, colony_object_groups);
    texture_release_shared(colony_bg_pvms, colony_bg_groups);
    set_end();
    if (colony_set != NULL) {
        loader_free(colony_set);
        colony_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

static void add_burst_light(NJS_POINT3 *pos, NJS_VECTOR *speed, Float scale,
                            Float fade)
{
    Float falloff = (BURST_RADIUS - njDistanceP2P(&camera_views[0]->pos, pos)) *
                    (1.0F / BURST_RADIUS);

    if (falloff > 0.0F) {
        colony_flash += burst_gain * scale * falloff;
    }
}

void lost_colony_step(struct task *task)
{
    struct task_work *work = task->work;
    Float level;

    fog_start();
    EASED_LEVEL(work) =
        ((LIGHT_LEVEL(task) - EASED_LEVEL(work)) * light_smoothing) +
        EASED_LEVEL(work);
    level = EASED_LEVEL(work);
    if (level > FOG_KNEE) {
        level -= FOG_KNEE;
        if (level > KNEE_SPAN) {
            level = 1.0F;
        } else {
            level = (level * KNEE_SLOPE) + FOG_KNEE;
        }
    }
    if (level < 0.0F) {
        level = 0.0F;
    }
    if (game_state.unk_25 || game_state.unk_09) {
        fog_set_blend(&colony_clear_fog, &colony_clear_fog, 1.0F);
    } else {
        fog_set_blend(&colony_dark_fog, &colony_lit_fog,
                      njSin(level * QUARTER_TURN));
    }
    LIGHT_LEVEL(task) *= light_decay;
    LIGHT_LEVEL(task) += colony_flash + (dynamite_gain * dynamite_blasts);
    dynamite_blasts = 0;
    colony_flash = 0.0F;
    if (LIGHT_LEVEL(task) > MAX_FLASH) {
        LIGHT_LEVEL(task) = MAX_FLASH;
    }
    /* @hack redundant: an empty switch on mode, which reads mode after the
     * clamp */
    switch (work->mode) {
    case 0:
        break;
    }
}

/* blink_level of shared/common_object.h, for the step count steps rather
 * than game_state.steps. nothing calls it. it isn't static, since SHC leaves
 * out a static function that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Float colony_blink_level(int period, int rest, int fade, int steps)
{
    int step = steps % period;
    Float level;

    if (step < fade) {
        /* @hack spelling: level set in two statements, where one that takes
         * the quotient from 1.0F would do */
        level = (Float)step / (Float)fade;
        level = 1.0F - level;
    } else if (step > rest) {
        step -= rest;
        if (step < fade) {
            level = (Float)step / (Float)fade;
        } else {
            level = 1.0F;
        }
    } else {
        level = 0.0F;
    }
    if (level < 0.0F) {
        level = 0.0F;
    }
    if (level > 1.0F) {
        level = 1.0F;
    }
    return level;
}

static void background_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    /* @hack type: the packed color an int, where Uint32 would do */
    int color;
    int i;

    for (i = 0; i < GAME_PLAYERS; i++) {
        if (player_work[i] != NULL) {
            if (colony_light_request[i]) {
                if (light_selected[i] != FLASH_LIGHT) {
                    previous_light[i] = light_selected[i];
                    light_select(FLASH_LIGHT, i);
                }
                colony_light_request[i] = FALSE;
                light_overridden[i] = TRUE;
            } else {
                if (light_overridden[i] && light_selected[i] == FLASH_LIGHT) {
                    light_select(previous_light[i], i);
                }
                light_overridden[i] = FALSE;
            }
        }
    }
    /* @hack type: the players' index reused for the models, where an index
     * of the models' own would do */
    for (i = 0; (unsigned)i < sizeof(pulse_models) / sizeof(pulse_models[0]);
         i++) {
        object = pulse_models[i].object;
        color = color_blend(
            BLACK, WHITE,
            PULSE_BRIGHTNESS((work->timer * pulse_speed) +
                             (pulse_models[i].phase * QUARTER_TURN)));
        vlist_set_color(object->model->vlist, color);
    }
    work->timer++;
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

static void background_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float x;
    Float z;
    int blocks;
    struct land_entry *entry;
    NJS_CNK_OBJECT *object;
    int count;
    int frame;
    Angle phase;
    /* @hack type: the packed color an int, where Uint32 would do */
    int color;
    NJS_POINT3 pos;
    NJS_TEXLIST texlist;

    x = -camera_views[camera_current_index]->pos.x + SKY_HIDE_X;
    z = -camera_views[camera_current_index]->pos.z + SKY_HIDE_Z;
    if ((x * x) + (z * z) >= SKY_HIDE_RADIUS * SKY_HIDE_RADIUS) {
        njFogDisable();
        njSetTexture(&colony_sky_texlist);
        njPushMatrixEx();
        njTranslateEx(&camera_views[camera_current_index]->pos);
        njPushMatrixEx();
        if (colony_sky_built) {
            njDirectDrawObject(colony_sky_build[0].built);
        } else {
            njCnkEasyDrawObject(&colony_sky);
        }
        matrix_load_parent();
        njFogDisable();
        /* @hack type: each camera's visible blocks read by name, here and
         * for the lamps and the poison below, where
         * game_state.land_blocks[camera_current_index] would do */
        if (camera_current_index == 0) {
            blocks = STAGE_VISIBLE_LAND(0);
        } else {
            blocks = STAGE_VISIBLE_LAND(1);
        }
        count = colony_sky_land.count;
        entry = colony_sky_land.entries;
        njPushMatrixEx();
        njSetTexture(&colony_sky_land_texlist);
        while (count--) {
            if (!entry->blocks || (entry->blocks & blocks)) {
                /* @hack redundant: a transform of the center whose result
                 * nothing reads */
                njCalcPoint(NULL, &entry->center, &pos);
                object = (NJS_CNK_OBJECT *)entry->object;
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njCnkEasyDrawModel(object->model);
                matrix_load_parent();
            }
            entry++;
        }
        njPopMatrixEx();
        njPopMatrix(2);
        njFogEnable();
    }
    njFogDisable();
    if (camera_current_index == 0) {
        blocks = STAGE_VISIBLE_LAND(0);
    } else {
        blocks = STAGE_VISIBLE_LAND(1);
    }
    count = colony_lamp_land.count;
    entry = colony_lamp_land.entries;
    njPushMatrixEx();
    njSetTexture(&colony_lamp_texlist);
    while (count--) {
        if (!entry->blocks || (entry->blocks & blocks)) {
            njCalcPoint(NULL, &entry->center, &pos);
            if (pos.z < SCENERY_DRAW_DEPTH && pos.z > -entry->radius) {
                object = (NJS_CNK_OBJECT *)entry->object;
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                phase = (njAbs(entry->center.x) + njAbs(entry->center.y) +
                         njAbs(entry->center.z)) *
                        pulse_phase_scale;
                color = color_blend(
                    DIM_LAMP, WHITE,
                    PULSE_BRIGHTNESS((work->timer * pulse_speed) + phase));
                vlist_set_color(object->model->vlist, color);
                njCnkEasyDrawModel(object->model);
                matrix_load_parent();
            }
        }
        entry++;
    }
    njPopMatrixEx();
    njFogEnable();
    if (camera_current_index == 0) {
        blocks = STAGE_VISIBLE_LAND(0);
    } else {
        blocks = STAGE_VISIBLE_LAND(1);
    }
    count = colony_poison_land.count;
    entry = colony_poison_land.entries;
    njPushMatrixEx();
    texlist = colony_poison_texlist;
    frame = (work->timer / POISON_FRAME_STEPS) % POISON_FRAMES;
    texlist.textures += frame;
    texlist.nbTexture = 1;
    njSetTexture(&texlist);
    while (count--) {
        if (!entry->blocks || (entry->blocks & blocks)) {
            njCalcPoint(NULL, &entry->center, &pos);
            if (pos.z < SCENERY_DRAW_DEPTH && pos.z > -entry->radius) {
                object = (NJS_CNK_OBJECT *)entry->object;
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njCnkEasyDrawModel(object->model);
                matrix_load_parent();
            }
        }
        entry++;
    }
    njPopMatrixEx();
}
