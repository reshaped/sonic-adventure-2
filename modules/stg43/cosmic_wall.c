/* Cosmic Wall, stg43: the stage's init, exit and step, and its background.
 * the background task animates the sky's land and draws the sky around the
 * current camera. while a player falls out of the stage, it throws sparks from
 * player 0 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg43/cosmic_wall/text.inc"

#include "stg43/sky_land.h"
#include "stg43/paths.h"
#include "stg43/terrain.h"
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
#include "shared/black_smoke.h"
#include "shared/bomb_hat.h"
#include "shared/common_object.h"
#ifdef NONMATCHING
/* sky_earth_texture, which this side's tests of the sky's textures read */
#include "shared/space_sky_textures.h"
#endif
#include "shared/debris.h"
#include "shared/enemy_laser.h"
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
#include "stg43/sounds.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg43/cosmic_wall.h"
#include "stg43/object_archives.h"
#include "stg43/drill.h"
#include "stg43/sky.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* the set files' block holds at least SET_ENTRIES slots, the head's
 * included */
#define SET_ENTRIES 2048
#define BACKGROUND_LEVEL 1
#define DRILL_MANAGER_LEVEL 2
#define BACKGROUND_MODELS 4
/* while kill_areas_task has a child, player 0 sheds a spark in each step
 * whose RANDOM_UNIT() is above SPARK_ABOVE */
#define SPARK_ABOVE 0.5F
/* the lights the init sets. both players and camera_apply take PLAYER_LIGHT,
 * the land draws in LIGHT_LAND, and meteors.c's rocks and each DEBRIS_LIGHT
 * piece in LIGHT_DEBRIS */
#define PLAYER_LIGHT 0
#define SECOND_LIGHT 1

#include "stg43/kill_planes.h"

/* the kill areas, as kill_areas_start takes them: kill_0 to kill_16, and the
 * null that ends them. kill_3 to kill_16 carry KILL_AREA_BELOW. each of them
 * meets a player above it too, and one below it at any height */
static struct kill_area cosmic_wall_kill_areas[18] = {
#include "stg43/cosmic_wall/cosmic_wall_kill_areas.inc"
};
static struct light main_light = {
#include "stg43/cosmic_wall/main_light.inc"
};
static struct light second_light = {
#include "stg43/cosmic_wall/second_light.inc"
};
/* the set files' block, or null */
static struct set_head *cosmic_wall_set = NULL;
/* the background task, or null while there is none */
static struct task *background_task = NULL;
/* the block grid's colors, its origin and its cell, as world_load_block_grid
 * takes them */
static struct land_block_color block_colors[14] = {
#include "stg43/cosmic_wall/block_colors.inc"
};
static NJS_POINT3 grid_origin = {
#include "stg43/cosmic_wall/grid_origin.inc"
};
static NJS_POINT3 grid_cell = {
#include "stg43/cosmic_wall/grid_cell.inc"
};
/* the drill manager, or null while there is none */
static struct task *drill_manager = NULL;
/* TRUE has the drill manager destroy both rides, which its next step starts
 * anew. nothing sets it */
static Bool restart_drills = FALSE;

/* the anims of the last two of surfaces, the objects that scroll. each anim's
 * uvs are the coordinates its scroll starts from */
static Sint16 first_scroll_uvs[16] = {
#include "stg43/cosmic_wall/first_scroll_uvs.inc"
};
static struct texture_anim first_scroll = {
#include "stg43/cosmic_wall/first_scroll.inc"
};
static Sint16 second_scroll_uvs[30] = {
#include "stg43/cosmic_wall/second_scroll_uvs.inc"
};
static struct texture_anim second_scroll = {
#include "stg43/cosmic_wall/second_scroll.inc"
};

/* an object of cosmic_wall_sky_land that the background animates. a tinted
 * object's vertices take the pulse's color. any other's texture coordinates
 * scroll by u_speed and v_speed times scroll_rate each step */
struct sky_surface {
    Bool tinted;
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 v_speed;
    Sint32 u_speed;
};
static struct sky_surface surfaces[8] = {
#include "stg43/cosmic_wall/surfaces.inc"
};
/* the pulse's color, which the tinted objects take. it runs from dim_color to
 * bright_color and back, its phase turning pulse_rate each step */
static Uint32 dim_color = {
#include "stg43/cosmic_wall/dim_color.inc"
};
static Uint32 bright_color = {
#include "stg43/cosmic_wall/bright_color.inc"
};
/* @hack type: pulse_rate a Uint32, where an Angle, the type of a turn's
 * rate, would do */
static Uint32 pulse_rate = {
#include "stg43/cosmic_wall/pulse_rate.inc"
};
static Sint16 scroll_rate = {
#include "stg43/cosmic_wall/scroll_rate.inc"
};
/* the size of a spark player 0 sheds: spark_size and up to spark_size_range
 * more */
static Float spark_size = {
#include "stg43/cosmic_wall/spark_size.inc"
};
static Float spark_size_range = {
#include "stg43/cosmic_wall/spark_size_range.inc"
};
/* the angles models 2 and 3 turn each step, about x and about y */
static Angle background_2_turn_x = {
#include "stg43/cosmic_wall/background_2_turn_x.inc"
};
static Angle background_2_turn_y = {
#include "stg43/cosmic_wall/background_2_turn_y.inc"
};
static Angle background_3_turn_x = {
#include "stg43/cosmic_wall/background_3_turn_x.inc"
};
static Angle background_3_turn_y = {
#include "stg43/cosmic_wall/background_3_turn_y.inc"
};
/* the draw lifts the four models background_height above the current
 * camera. it lifts models 0 and 1 their own heights more, and scales them
 * across x and z. it scales models 2 and 3 along every axis */
static Float background_0_height = {
#include "stg43/cosmic_wall/background_0_height.inc"
};
static Float background_0_scale = {
#include "stg43/cosmic_wall/background_0_scale.inc"
};
static Float background_1_height = {
#include "stg43/cosmic_wall/background_1_height.inc"
};
static Float background_1_scale = {
#include "stg43/cosmic_wall/background_1_scale.inc"
};
static Float background_height = {
#include "stg43/cosmic_wall/background_height.inc"
};
static Float background_turned_scale = {
#include "stg43/cosmic_wall/background_turned_scale.inc"
};
static struct fog_setting background_fog;

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

static Uint8 *background_build(Uint8 *buffer)
{
    njFogDisable();
    /* @bug logic: the sky's models never compile. the compile runs only
     * while background_built is TRUE, and only a compile sets it TRUE, so the
     * test at 0x8c500030 always fails */
    if (background_built &&
        model_builds_compile(&buffer, background_builds, BACKGROUND_MODELS) ==
            BACKGROUND_MODELS) {
        background_built = TRUE;
    } else {
        background_built = FALSE;
    }
    njFogEnable();
    return buffer;
}

void cosmic_wall_init(void)
{
    struct task *task;
    /* @hack spelling: set_light and light, locals for light_set and
     * &main_light, where those by their names would do */
    struct light *light;
    void (*set_light)(int index, const struct light *light);

    world_land_start(&cosmic_wall_land);
    world_load_block_grid(COSMIC_WALL_GRID, block_colors, &grid_origin,
                          &grid_cell);
#ifdef NONMATCHING
    /* this side loads the sky's textures only where sky_earth_texture, the
     * first name of their list, isn't empty. a build without the disc's data
     * leaves it empty */
    if (sky_earth_texture[0] != '\0')
#endif
    {
        texture_load_shared(background_pvms, background_texture_groups);
        texture_await_shared(background_pvms, background_texture_groups);
    }
    texture_load_shared(cosmic_wall_pvms, cosmic_wall_texture_groups);
    texture_await_shared(cosmic_wall_pvms, cosmic_wall_texture_groups);
    cosmic_wall_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&cosmic_wall_objects, cosmic_wall_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    set_light = light_set;
    light = &main_light;
    set_light(PLAYER_LIGHT, light);
    set_light(SECOND_LIGHT, &second_light);
    set_light(LIGHT_LAND, light);
    set_light(LIGHT_DEBRIS, light);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(PLAYER_LIGHT);
    light_select_after_draw(LIGHT_LAND);
    lights_load(COSMIC_WALL_LIGHTS);
    background_build(object_builds_run((Uint8 *)world_build_land(
        &cosmic_wall_land, (Uint32)STAGE_MODEL_BUILD_AREA)));
    music_fade_in(0);
    music_play_or_defer(COSMIC_WALL_MUSIC);
    path_create_manager(cosmic_wall_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step,
                       COSMIC_WALL_BACKGROUND_TASK);
    if (task != NULL) {
        background_task = task;
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(cosmic_wall_kill_areas);
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
    messenger_messages_load(COSMIC_WALL_MESSENGER_MESSAGES);
    hint_box_hints_load(COSMIC_WALL_BOX_HINTS);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    fog_start();
    fog_read(COSMIC_WALL_FOG, &background_fog);
    sound_load_stage(COSMIC_WALL_SOUNDS, &cosmic_wall_sounds);
    broken_down_smoke_create(0, NULL);
    enemy_laser_load();
    debris_friction = 0.99F;
    debris_gravity = 0.03F;
    debris_life = 90;
    debris_life_spread = 30.0F;
}

void cosmic_wall_exit(void)
{
    burst_hook = NULL;
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
    texture_release_shared(cosmic_wall_pvms, cosmic_wall_texture_groups);
#ifdef NONMATCHING
    /* this side releases the sky's textures only where the init loaded them:
     * where sky_earth_texture isn't empty */
    if (sky_earth_texture[0] != '\0')
#endif
    {
        texture_release_shared(background_pvms, background_texture_groups);
    }
    set_end();
    if (cosmic_wall_set != NULL) {
        loader_free(cosmic_wall_set);
        cosmic_wall_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

/* start the first ride and the second whenever the manager has no child */
static void drill_manager_step(struct task *task)
{
    struct task *child;
    if (restart_drills) {
        task_destroy_children(task);
        restart_drills = FALSE;
    } else if (task->child == NULL) {
        child = task_create_child(TASK_WORK | TASK_FORCE,
                                  cosmic_drill_platform_init, task);
        if (child != NULL) {
            child->work->submode = COSMIC_DRILL_FIRST_RIDE;
            child->unk_3c.l = COSMIC_DRILL_FIRST_TRACK;
        }
        child = task_create_child(TASK_WORK | TASK_FORCE,
                                  cosmic_drill_platform_init, task);
        if (child != NULL) {
            child->unk_3c.l = COSMIC_DRILL_SECOND_TRACK;
            child->work->submode = COSMIC_DRILL_SECOND_RIDE;
        }
    }
}

static void drill_manager_destroy(struct task *task)
{
    if (task == drill_manager) {
        drill_manager = NULL;
    }
}

void cosmic_wall_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode whose cases do nothing */
    switch (work->mode) {
    case 0:
        break;
    default:
        break;
    }
    if (drill_manager == NULL) {
        drill_manager =
            task_create(TASK_WORK, DRILL_MANAGER_LEVEL, drill_manager_step,
                        COSMIC_WALL_DRILL_MANAGER);
        if (drill_manager != NULL) {
            drill_manager->destructor = drill_manager_destroy;
        }
    }
}

static void background_step(struct task *task)
{
    Sint32 steps;
    int offset;
    Uint32 color;
    struct sky_surface *surface;
    unsigned int i;

    /* @hack spelling: steps set inside the product, where a statement of
     * its own would do */
    /* @hack type: the offset cut to a Uint16, where the whole product would
     * do: the scrolls wrap at SCROLL_WRAP, which divides 0x10000 */
    offset = (Uint16)((steps = game_state.steps) * scroll_rate);
    surface = surfaces;
    color = color_blend(dim_color, bright_color,
                        0.5F + (0.5F * njSin(steps * pulse_rate)));
    for (i = 0; i < sizeof(surfaces) / sizeof(surfaces[0]); i++, surface++) {
        if (!surface->tinted) {
            texture_anim_scroll(surface->object->model, surface->anim,
                                (surface->u_speed * offset) & (SCROLL_WRAP - 1),
                                (surface->v_speed * offset) &
                                    (SCROLL_WRAP - 1));
        } else {
            vlist_set_color(surface->object->model->vlist, color);
        }
    }
    /* player_fall_out makes the miss's task kill_areas_task's child. so the
     * task has a child while a player falls out of the stage */
    if (player_work[0] != NULL && kill_areas_task != NULL &&
        kill_areas_task->child != NULL && RANDOM_UNIT() > SPARK_ABOVE) {
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

#define SPACE_SKY_LAND cosmic_wall_sky_land
#include "shared/space_sky_draw.h"
