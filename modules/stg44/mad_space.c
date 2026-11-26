/* Mad Space, stg44: the stage's init, exit and step, and its background */
#include <mathf.h>
#include <ninja.h>

#include "stg44/mad_space/text.inc"
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

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
#include "shared/dirt.h"
#include "shared/emerald.h"
#include "shared/enemy_bomb.h"
#include "shared/field.h"
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
#include "shared/emerald_hunt_data.h"
#include "stg44/routes.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg44/mad_space.h"
#include "stg44/scenery_shapes.h"
#include "stg44/sounds.h"
#include "stg44/object_archives.h"
#include "stg44/props.h"
#include "shared/space_sky_textures.h"
#include "stg44/sky.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
#define MANAGER_LEVEL 2
/* WIND_SOUND plays while player 0 lies below a y of LOW_WIND_CEILING. it plays
 * at camera 0's x and z, at a y of LOW_WIND_HEIGHT */
#define LOW_WIND_CEILING 500.0F
#define LOW_WIND_HEIGHT 160.0F
#define WIND_SOUND 12
#define WIND_STEPS 30
#define SOUND_PRIORITY 1
/* the test dots a player's way down with world_gravity as the step began. a
 * dot below GRAVITY_VIBRATION_DOT shakes the player's pad */
#define GRAVITY_VIBRATION_DOT 0.7F
#define VIBRATION_POWER 1
#define VIBRATION_FREQ 40
#define VIBRATION_TIME 1
/* while a miss's task lives under kill_areas_task, player 0 sheds a wobbling
 * spark on about half the steps. a step sheds one where rand() passes
 * SPARK_ODDS of its range */
#define SPARK_ODDS 0.5F
#define BACKGROUND_MODELS 5
/* the tick that has a car's trailer carry the emerald of that id, as
 * stg44/props.h says */
#define CARRIED_EMERALD(id) ((id) + 1)
/* the first two orbits start turned ORBIT_TILT about x */
#define ORBIT_TILT 0x3000

static struct kill_area mad_space_kill_areas[2] = {
#include "stg44/mad_space/mad_space_kill_areas.inc"
};
static struct light main_light = {
#include "stg44/mad_space/main_light.inc"
};
static struct light second_light = {
#include "stg44/mad_space/second_light.inc"
};
static struct set_head *stage_set = NULL;
static struct task *background_task = NULL;
static struct task *carriage_manager = NULL;

/* a surface of the sky: an object of stg44/sky.h, which holds a model of
 * mad_space_sky_land. the background's step scrolls a surface with an anim by
 * its speeds. it gives any other's vertices the pulse's color. no surface has
 * an anim, so none scrolls */
struct sky_surface {
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 v_speed;
    Sint32 u_speed;
};
/* each of the sky land's sixteen models. index 10 holds mad_space_land_8
 * again, which only colors that model twice a step */
static struct sky_surface surfaces[17] = {
#include "stg44/mad_space/surfaces.inc"
};
/* the pulse's color runs from dim_color to bright_color and back with the sine
 * of game_state.steps * pulse_rate */
static Uint32 dim_color = {
#include "stg44/mad_space/dim_color.inc"
};
static Uint32 bright_color = {
#include "stg44/mad_space/bright_color.inc"
};
static Uint32 pulse_rate = {
#include "stg44/mad_space/pulse_rate.inc"
};
static Sint16 scroll_rate = {
#include "stg44/mad_space/scroll_rate.inc"
};
static Float spark_size = 3.0F;
static Float spark_size_range = 3.5F;
static Bool background_built = FALSE;
static struct model_build background_builds[BACKGROUND_MODELS] = {
#include "stg44/mad_space/background_builds.inc"
};
static Angle first_orbit_x = {
#include "stg44/mad_space/first_orbit_x.inc"
};
static Angle first_orbit_y = {
#include "stg44/mad_space/first_orbit_y.inc"
};
static Angle second_orbit_x = {
#include "stg44/mad_space/second_orbit_x.inc"
};
static Angle second_orbit_y = {
#include "stg44/mad_space/second_orbit_y.inc"
};
static Angle third_orbit_x = {
#include "stg44/mad_space/third_orbit_x.inc"
};
static Angle third_orbit_y = {
#include "stg44/mad_space/third_orbit_y.inc"
};
static Float first_layer_height = {
#include "stg44/mad_space/first_layer_height.inc"
};
static Float first_layer_scale = {
#include "stg44/mad_space/first_layer_scale.inc"
};
static Float second_layer_height = {
#include "stg44/mad_space/second_layer_height.inc"
};
static Float second_layer_scale = {
#include "stg44/mad_space/second_layer_scale.inc"
};
static Float sky_height = {
#include "stg44/mad_space/sky_height.inc"
};
static Float orbit_scale = {
#include "stg44/mad_space/orbit_scale.inc"
};
static Float third_orbit_scale = {
#include "stg44/mad_space/third_orbit_scale.inc"
};
static struct fog_setting upper_fog;
static struct fog_setting lower_fog;
static struct fog_setting stage_fog;

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static Uint8 *background_build(Uint8 *start);
static void background_draw(struct task *task);

void mad_space_init(void)
{
    struct task *task;
    Uint8 *buffer;
    /* @hack spelling: main_light passed through light, where &main_light would
     * do */
    struct light *light;
    /* @hack spelling: light_set called through set_light, where light_set
     * itself would do */
    void (*set_light)(int index, const struct light *light);

    world_land_start(&mad_space_land);
    texture_load_shared(mad_space_pvms, mad_space_texture_groups);
    texture_await_shared(mad_space_pvms, mad_space_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    stage_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&mad_space_objects, stage_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    set_light = light_set;
    light = &main_light;
    set_light(0, light);
    set_light(1, &second_light);
    set_light(2, light);
    set_light(3, light);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load(MAD_SPACE_LIGHT_FILE);
    buffer = (Uint8 *)world_build_land(&mad_space_land,
                                       (Uint32)STAGE_MODEL_BUILD_AREA);
    buffer = object_builds_run(buffer);
    buffer = background_build(buffer);
    mad_space_sl_build(buffer);
    music_fade_in(0);
    music_play_or_defer("m_space.adx");
    path_create_manager(mad_space_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(mad_space_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.unk_04 == NULL) {
        player_hooks.unk_04 = dirt_create;
    }
    if (player_hooks.create_piece == NULL) {
        player_hooks.create_piece = emerald_create;
    }
    if (player_hooks.show_piece_sign == NULL) {
        player_hooks.show_piece_sign = emerald_sign_create;
    }
    hunt_set_hints(hunt_location_hints);
    hunt_stage_messages = hunt_load_hint_file("eh0044");
    emerald_set_paths(hunt_piece_routes, HUNT_PIECE_ROUTES);
    njFogEnable();
    messenger_messages_load("mh0044");
    hint_box_hints_load("bh0044");
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    fog_start();
    fog_clear(&stage_fog);
    fog_clear(&upper_fog);
    fog_clear(&lower_fog);
    fog_read(MAD_SPACE_FOGA_FILE, &upper_fog);
    fog_read(MAD_SPACE_FOGB_FILE, &lower_fog);
    sound_load_stage("se_ac_mp.mlt", &mad_space_sounds);
    object_builds_init();
    enemy_bomb_load();
    debris_friction = 0.99F;
    debris_gravity = 0.03F;
    debris_life = 60;
    debris_life_spread = 30.0F;
}

void mad_space_exit(void)
{
    object_builds_exit();
    fog_stop();
    njFogDisable();
    emerald_clear_paths();
    syFree(hunt_stage_messages);
    hunt_stage_messages = NULL;
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
    hint_box_hints_free();
    messenger_messages_free();
    particle_core_stop();
    texture_release_shared(mad_space_pvms, mad_space_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (stage_set != NULL) {
        loader_free(stage_set);
        stage_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

static void carriage_manager_step(struct task *task)
{
    struct task *child;

    if (task->child == NULL) {
        child = task_create_child(TASK_WORK | TASK_FORCE,
                                  mad_space_carriage_init, task);
        if (child != NULL) {
            child->unk_3c.p = &mad_space_first_carriage_path;
            child->work->tick = CARRIED_EMERALD(2);
            mad_space_carriage_init(child);
        }
        child = task_create_child(TASK_WORK | TASK_FORCE,
                                  mad_space_carriage_init, task);
        if (child != NULL) {
            child->unk_3c.p = &mad_space_second_carriage_path;
            child->work->tick = CARRIED_EMERALD(1);
            mad_space_carriage_init(child);
        }
        child = task_create_child(TASK_WORK | TASK_FORCE,
                                  mad_space_platform_init, task);
        if (child != NULL) {
            child->unk_3c.p = &mad_space_platform_path;
            child->work->tick = CARRIED_EMERALD(0);
            mad_space_platform_init(child);
        }
    }
}

static void carriage_manager_destroy(struct task *task)
{
    if (task == carriage_manager) {
        carriage_manager = NULL;
    }
}

void mad_space_step(struct task *task)
{
    struct task_work *work = task->work;
    Float dot;
    int player;

    if (carriage_manager == NULL) {
        carriage_manager = task_create(TASK_WORK, MANAGER_LEVEL,
                                       carriage_manager_step, "ObjSetManExec");
        if (carriage_manager != NULL) {
            carriage_manager->destructor = carriage_manager_destroy;
            carriage_manager_step(carriage_manager);
        }
    }
    /* @bug memory: reads player 0's pos through null where player_work[0] is
     * null (FMOV.S at 0x8c5005d2). background_step tests player_work[0] for
     * null (TST at 0x8c500938) */
#ifdef NONMATCHING
    /* this side repairs the tag above: it plays no low wind while slot 0
     * holds no player */
    if (player_work[0] != NULL && player_work[0]->pos.y < LOW_WIND_CEILING) {
#else
    if (player_work[0]->pos.y < LOW_WIND_CEILING) {
#endif
        NJS_POINT3 wind;

        wind.x = camera_views[0]->pos.x;
        wind.y = LOW_WIND_HEIGHT;
        wind.z = camera_views[0]->pos.z;
        sound_play_at_for(WIND_SOUND, task, SOUND_PRIORITY, SOUND_LOUDEST,
                          WIND_STEPS, &wind);
    }
    /* @hack spelling: sample and previous in a block of their own, where
     * declarations at the function's top would do */
    {
        struct field_sample sample;
        NJS_VECTOR previous;

        previous = world_gravity;
        for (player = 0; player < GAME_PLAYERS; player++) {
            if (player_work[player] != NULL) {
                field_sample_at(&player_work[player]->pos, &sample);
                if (sample.flags & FIELD_GRAVITY) {
                    world_gravity = sample.gravity;
                } else {
                    world_gravity.x = 0.0F;
                    world_gravity.y = -1.0F;
                    world_gravity.z = 0.0F;
                }
                dot = njInnerProduct(&previous, &world_gravity);
                if (dot < GRAVITY_VIBRATION_DOT) {
                    vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                                    VIBRATION_TIME);
                }
                world_gravity_ang_x =
                    (Angle)(asinf(-world_gravity.z) * ANGLE_PER_RADIAN);
                world_gravity_ang_z =
                    -(Angle)(atan2f(-world_gravity.x, -world_gravity.y) *
                             ANGLE_PER_RADIAN);
            }
        }
    }
    /* @hack redundant: a switch of one empty case, where nothing would do */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void background_step(struct task *task)
{
    Float pitch;
    Sint32 frame;
    int step;
    Uint32 color;
    struct sky_surface *surface;
    unsigned int i;

    /* stage_fog stays clear. where the camera's pitch has a sine above 0, the
     * fog blends from stage_fog toward upper_fog by it. otherwise it blends
     * toward lower_fog by the sine's negation */
    pitch = njSin(camera_views[camera_current_index]->ang.x);
    if (pitch > 0.0F) {
        fog_set_blend(&stage_fog, &upper_fog, pitch);
    } else {
        fog_set_blend(&stage_fog, &lower_fog, -pitch);
    }
    /* @hack spelling: the assignment to frame inside the product, where a
     * statement of its own would do */
    step = (Uint16)((frame = game_state.steps) * scroll_rate);
    surface = surfaces;
    color = color_blend(dim_color, bright_color,
                        0.5F + (0.5F * njSin(frame * pulse_rate)));
    for (i = 0; i < sizeof(surfaces) / sizeof(surfaces[0]); i++, surface++) {
        if (surface->anim != NULL) {
            texture_anim_scroll(surface->object->model, surface->anim,
                                (surface->u_speed * step) & (SCROLL_WRAP - 1),
                                (surface->v_speed * step) & (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(surface->object->model->vlist, color);
        }
    }
    if (player_work[0] != NULL && kill_areas_task != NULL &&
        kill_areas_task->child != NULL &&
        (Float)rand() * RAND_SCALE > SPARK_ODDS) {
        burst_create_wobbling(
            &player_work[0]->pos, &player_motion[0]->spd,
            spark_size + ((Float)rand() * spark_size_range * RAND_SCALE));
    }
}

static void background_destroy(struct task *task)
{
    if (background_task == task) {
        background_task = NULL;
    }
}

static Uint8 *background_build(Uint8 *start)
{
    /* @hack spelling: buffer, a copy of start, where start itself would do */
    Uint8 *buffer = start;

    /* @hack redundant: a goto to the statement after it */
    goto build;
build:
    njFogDisable();
    if (model_builds_compile(&buffer, background_builds, BACKGROUND_MODELS) ==
        BACKGROUND_MODELS) {
        background_built = TRUE;
    }
    njFogEnable();
    return buffer;
}

static void background_draw(struct task *task)
{
    NJS_CNK_MODEL *model;
    /* @hack spelling: scale, a copy of orbit_scale, where orbit_scale itself
     * would do */
    Float scale;

    njFogDisable();
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, sky_height, 0.0F);

    njSetTexture(&background_0_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, first_layer_height, 0.0F);
    njScale(NULL, first_layer_scale, 1.0F, first_layer_scale);
    if (background_built) {
        njTranslateEx((NJS_VECTOR *)background_0_object->pos);
        njDirectDrawObject(background_builds[0].built);
    } else {
        njCnkEasyDrawObject(background_0_object);
    }
    njPopMatrixEx();
    njSetTexture(&background_1_texlist);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, second_layer_height, 0.0F);
    njScale(NULL, second_layer_scale, 1.0F, second_layer_scale);
    if (background_built) {
        njTranslateEx((NJS_VECTOR *)background_1_object->pos);
        njDirectDrawObject(background_builds[1].built);
    } else {
        njCnkEasyDrawObject(background_1_object);
    }
    njPopMatrixEx();

    njPushMatrixEx();
    /* @hack redundant: a block no path reaches, which pushes the matrix
     * twice */
    if (0) {
        njPushMatrixEx();
        njPushMatrixEx();
    }
    njSetTexture(&background_2_texlist);
    njTranslateEx((NJS_VECTOR *)background_2_object->pos);
    model = background_2_object->model;
    /* @hack redundant: a read of model->r, which nothing uses */
    (void)model->r;
    njTranslate(NULL, model->center.x, model->center.y, model->center.z);
    njRotateX(NULL, (first_orbit_x * game_state.steps) + ORBIT_TILT);
    njRotateY(NULL, first_orbit_y * game_state.steps);
    scale = orbit_scale;
    njScale(NULL, scale, scale, scale);
    /* @hack spelling: the center read through background_2_object, where model
     * would do */
    njTranslate(NULL, -background_2_object->model->center.x,
                -background_2_object->model->center.y,
                -background_2_object->model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[2].built);
    } else {
        njCnkEasyDrawModel(background_2_object->model);
    }
    matrix_load_parent();

    njSetTexture(&background_3_texlist);
    njTranslateEx((NJS_VECTOR *)background_3_object->pos);
    njTranslate(NULL, background_3_object->model->center.x,
                background_3_object->model->center.y,
                background_3_object->model->center.z);
    njRotateX(NULL, (second_orbit_x * game_state.steps) + ORBIT_TILT);
    njRotateY(NULL, second_orbit_y * game_state.steps);
    scale = orbit_scale;
    njScale(NULL, scale, scale, scale);
    njTranslate(NULL, -background_3_object->model->center.x,
                -background_3_object->model->center.y,
                -background_3_object->model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[3].built);
    } else {
        njCnkEasyDrawModel(background_3_object->model);
    }
    matrix_load_parent();

    njSetTexture(&background_4_texlist);
    njTranslateEx((NJS_VECTOR *)background_4_object->pos);
    model = background_4_object->model;
    njTranslate(NULL, model->center.x, model->center.y, model->center.z);
    njRotateX(NULL, third_orbit_x * game_state.steps);
    njRotateY(NULL, third_orbit_y * game_state.steps);
    njScale(NULL, third_orbit_scale, third_orbit_scale, third_orbit_scale);
    /* @hack redundant: a second load of model, which already holds
     * background_4_object->model */
    model = background_4_object->model;
    njTranslate(NULL, -model->center.x, -model->center.y, -model->center.z);
    if (background_built) {
        njDirectDrawObject(background_builds[4].built);
    } else {
        njCnkEasyDrawModel(background_4_object->model);
    }
    matrix_load_parent();

    njPopMatrix(2);
    njFogEnable();
    land_table_draw(&mad_space_sky_land);
}
