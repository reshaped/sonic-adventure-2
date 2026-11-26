#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last1/body.h"
#include "boss_last1/stage.h"
#include "boss_last1/object_archives.h"
#include "boss_last1/terrain.h"
#include "boss_last1/sphere.h"
#include "boss_last1/arena.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/big_splash.h"
#include "shared/last_boss_sphere.h"
#include "shared/motions.h"
#include "shared/water_ripple.h"
#include "shared/messenger.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/pad.h"
#include "system/vector.h"

#define SET_MIN_ENTRIES 512
/* the light both players take */
#define PLAYER_LIGHT 0
/* opaque, in a dark grey */
#define BACK_COLOR 0xff333333
/* arena_hold_player measures player 0 as if HELD_HEIGHT above the center */
#define HELD_HEIGHT 6.0F
#define EXIT_RANGE 50.0F

const NJS_POINT3 biolizard_arena_center = {
#include "boss_last1/stage/biolizard_arena_center.inc"
};
static const Float boundary_radius = 345.0F;
/* where the currents carry a player out across the boundary */
static const NJS_POINT3 current_exit = {
#include "boss_last1/stage/current_exit.inc"
};

static struct light stage_lights[5] = {
    {{0.0F, 0.707F, 0.707F}, 1.0F, 0.5F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.3F, 0.6F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 1.0F, 0.8F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.0F, 1.0F, 1.0F, 0.5F, 0.5F},
    {{0.0F, 0.707F, 0.707F}, 0.5F, 1.0F, 1.0F, 0.5F, 0.5F},
};

static struct path_point east_current_points[11] = {
#include "boss_last1/stage/east_current_points.inc"
};
static struct path east_current = {
#include "boss_last1/stage/east_current.inc"
};

static struct path_point west_current_points[11] = {
#include "boss_last1/stage/west_current_points.inc"
};
static struct path west_current = {
#include "boss_last1/stage/west_current.inc"
};

static struct path *currents[3] = {
#include "boss_last1/stage/currents.inc"
};
static Sint32 unused_e0b8 = 1;
static struct set_head *stage_set = NULL;
static void arena_step(struct task *task);
static void arena_draw(struct task *task);

void last1_stage_init(void)
{
    world_land_start(&biolizard_land);
    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    stage_set =
        set_read("set_b_Last1_s.bin", "set_b_Last1_u.bin", SET_MIN_ENTRIES);
    set_start(&last1_object_list, stage_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(1.0F, 1.0F, 1.0F);
    light_set(PLAYER_LIGHT, &stage_lights[0]);
    light_set(1, &stage_lights[1]);
    light_set(LIGHT_LAND, &stage_lights[1]);
    light_set(BIOLIZARD_LIGHT, &stage_lights[2]);
    light_set(LAST_BOSS_EFFECT_LIGHT, &stage_lights[2]);
    light_set(BIOLIZARD_LIFE_SUPPORT_LIGHT, &stage_lights[3]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(LIGHT_LAND);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stgbl1_light.bin");

    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    texture_load_shared(biolizard_pvms, biolizard_texture_groups);
    biolizard_textures_load();
    biolizard_build_models(
        world_build_land(&biolizard_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    module_motions_register();
    music_fade_in(0);
    music_play_or_defer("boss_06.adx");
    enemy_groups_clear();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    loader_read_camera_files();
    path_create_manager(currents);
    messenger_messages_load("mh0055");
    if (!player_hooks.splash) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    if (!player_hooks.ripple) {
        player_hooks.ripple = ripple_create;
    }
    sound_load_stage_and_voices("se_ac_lb1.mlt", &biolizard_sounds, 1);
}

void last1_stage_exit(void)
{
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    particle_core_stop();
    texture_release_shared(biolizard_pvms, biolizard_texture_groups);
    biolizard_textures_release();
    set_end();
    if (stage_set) {
        loader_free(stage_set);
        stage_set = NULL;
    }
    world_land_stop();
    messenger_messages_free();
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    if (player_hooks.ripple == ripple_create) {
        player_hooks.ripple = NULL;
    }
    module_motions_unregister();
}

void last1_stage_step(struct task *task)
{
    /* @hack spelling: arena, a copy of task, where task itself would do */
    struct task *arena = task;
    biolizard_create(&biolizard_arena_center, 0);
    arena->step = arena_step;
    arena->draw = arena_draw;
}

/* hold player 0 within boundary_radius of the arena's center. a current
 * carries player 0 past the boundary instead. past the boundary within
 * EXIT_RANGE of current_exit, player 0 misses and its pad turns off */
static void arena_hold_player(void)
{
    NJS_VECTOR offset;
    Float distance;

    offset = player_work[0]->pos;
    offset.x -= biolizard_arena_center.x;
    offset.y = HELD_HEIGHT;
    offset.z -= biolizard_arena_center.z;
    distance = njScalor(&offset);
    if (player_records[0]->anim.current != ANIM_IN_CURRENT) {
        if (distance > boundary_radius) {
            njUnitVector(&offset);
            offset.x = biolizard_arena_center.x + (offset.x * boundary_radius);
            offset.y = player_work[0]->pos.y;
            offset.z = biolizard_arena_center.z + (offset.z * boundary_radius);
            player_set_pos(0, offset.x, offset.y, offset.z);
        }
    } else if (vector_distance(&current_exit, &player_work[0]->pos) <
               EXIT_RANGE) {
        if (player_work[0]->mode != PLAYER_MODE_MISS &&
            distance > boundary_radius) {
            player_miss(0);
            pad_disable_player(0);
        }
    }
}

static void arena_step(struct task *task)
{
    arena_hold_player();
}

static void arena_draw(struct task *task)
{
    biolizard_water_draw();
}
