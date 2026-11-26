/* the Big Foot stage, the program of BOSS_BIGFOOT.PRS: its init and exit,
 * and its task's steps */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigfoot/bigfoot.h"
#include "boss_bigfoot/prop.h"
#include "boss_bigfoot/stage.h"
#include "boss_bigfoot/object_archives.h"
#include "boss_bigfoot/terrain.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_sounds.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/parked_car.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#define SET_MIN_ENTRIES 512
#define BOSS_START_X 90.0F
#define BOSS_START_ANG_Y (-QUARTER_TURN)
/* the light both players take */
#define PLAYER_LIGHT 0
/* the light the camera hands Ninja, which light_select_camera sets */
#define CAMERA_LIGHT 0
/* the light light_select_after_draw sets, which Big Foot's draw hands back to
 * Ninja while Big Foot is hurt */
#define STAGE_LIGHT 2

static struct light bigfoot_lights[3] = {
    {{0.0F, 0.707F, 0.707F}, 1.0F, 0.5F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.5F, 0.2F, 0.5F, 0.5F, 0.5F},
    {{0.0F, 0.707F, 0.707F}, 0.0F, 1.0F, 0.5F, 0.0F, 0.0F},
};
static struct set_head *bigfoot_set = NULL;

static void arena_step(struct task *task);

void bigfoot_stage_init(void)
{
    Uint8 *buf;

    world_land_start(&bigfoot_land);
    bigfoot_set =
        set_read("set_b_bigfoot_s.bin", "set_b_bigfoot_u.bin", SET_MIN_ENTRIES);
    set_start(&bigfoot_object_list, bigfoot_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(1.0F, 1.0F, 1.0F);
    light_set(0, &bigfoot_lights[0]);
    light_set(1, &bigfoot_lights[1]);
    light_set(2, &bigfoot_lights[0]);
    light_set(BIGFOOT_LIGHT, &bigfoot_lights[0]);
    light_set(MECH_BOSS_HURT_LIGHT, &bigfoot_lights[2]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(CAMERA_LIGHT);
    light_select_after_draw(STAGE_LIGHT);
    lights_load("stg50_light.bin");
    texture_load_shared(bigfoot_texture_pvms, bigfoot_texture_groups);
    texture_await_shared(bigfoot_texture_pvms, bigfoot_texture_groups);
    buf = (Uint8 *)world_build_land(&bigfoot_land,
                                    (Uint32)STAGE_MODEL_BUILD_AREA);
    buf = object_builds_run(buf);
    buf = parked_car_build(buf);
    prop_build(buf);
    messenger_messages_load("mh0050");
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    mech_boss_textures_load(MECH_BOSS_BIG_FOOT);
    blast_load_texture();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    music_fade_in(0);
    music_play_or_defer("boss_01.adx");
    mech_boss_sounds_load();
}

void bigfoot_stage_exit(void)
{
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    particle_core_stop();
    mech_boss_textures_release(MECH_BOSS_BIG_FOOT);
    texture_release_shared(bigfoot_texture_pvms, bigfoot_texture_groups);
    messenger_messages_free();
    set_end();
    if (bigfoot_set != NULL) {
        loader_free(bigfoot_set);
        bigfoot_set = NULL;
    }
    world_land_stop();
}

void bigfoot_stage_step(struct task *task)
{
    NJS_POINT3 pos;

    pos.x = BOSS_START_X;
    pos.y = 0.0F;
    pos.z = 0.0F;
    bigfoot_create(&pos, BOSS_START_ANG_Y);
    task->step = arena_step;
}

/* hold player 0 within BIGFOOT_ARENA_RADIUS of the origin, along x and z. the
 * unit vector the hold scales counts y, so a player off y = 0 lands nearer */
static void arena_hold_player(void)
{
    Float distance;
    NJS_VECTOR dir;

    distance = njSqrt((player_work[0]->pos.x * player_work[0]->pos.x) +
                      (player_work[0]->pos.z * player_work[0]->pos.z));
    if (distance > BIGFOOT_ARENA_RADIUS) {
        dir = player_work[0]->pos;
        njUnitVector(&dir);
        player_work[0]->pos.x = dir.x * BIGFOOT_ARENA_RADIUS;
        player_work[0]->pos.z = dir.z * BIGFOOT_ARENA_RADIUS;
    }
}

static void arena_step(struct task *task)
{
    arena_hold_player();
}
