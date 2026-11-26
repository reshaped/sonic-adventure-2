/* the Hot Shot stage, the program of BOSS_HOTSHOT.PRS: its init and exit,
 * and its task's steps */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_hotshot/hotshot.h"
#include "boss_hotshot/stage.h"
#include "boss_hotshot/object_archives.h"
#include "boss_hotshot/terrain.h"
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
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#define SET_MIN_ENTRIES 512
#define BOSS_START_X 170.0F
#define BOSS_START_ANG_Y (-QUARTER_TURN)
/* the light both players take */
#define PLAYER_LIGHT 0
/* the light the camera hands Ninja, which light_select_camera sets */
#define CAMERA_LIGHT 0
/* the light light_select_after_draw sets. Hot Shot's draw hands it back to
 * Ninja after the hurt light */
#define STAGE_LIGHT 2

static struct light hotshot_lights[3] = {
    {{0.0F, 0.707F, 0.707F}, 1.0F, 0.5F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.5F, 0.2F, 0.5F, 0.5F, 0.5F},
    {{0.0F, 0.707F, 0.707F}, 0.0F, 1.0F, 0.5F, 0.0F, 0.0F},
};
static struct set_head *hotshot_set = NULL;

static void arena_step(struct task *task);

void hotshot_stage_init(void)
{
    world_land_start(&hotshot_land);
    hotshot_set =
        set_read("SET_B_HOTSHOT_S.BIN", "SET_B_HOTSHOT_U.BIN", SET_MIN_ENTRIES);
    set_start(&hotshot_object_list, hotshot_set);
    loader_read_camera_files();
    particle_core_start();
    music_fade_in(0);
    music_play_or_defer("boss_01.adx");
    njCnkSetAmbient(1.0F, 1.0F, 1.0F);
    light_set(0, &hotshot_lights[0]);
    light_set(1, &hotshot_lights[1]);
    light_set(2, &hotshot_lights[0]);
    light_set(3, &hotshot_lights[0]);
    light_set(MECH_BOSS_HURT_LIGHT, &hotshot_lights[2]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(CAMERA_LIGHT);
    light_select_after_draw(STAGE_LIGHT);
    lights_load("stg50_light.bin");
    world_build_land(&hotshot_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    mech_boss_textures_load(MECH_BOSS_HOT_SHOT);
    texture_load_shared(hotshot_texture_pvms, hotshot_texture_groups);
    blast_load_texture();
    messenger_messages_load("mh0051");
    mech_boss_sounds_load();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
}

void hotshot_stage_exit(void)
{
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    particle_core_stop();
    mech_boss_textures_release(MECH_BOSS_HOT_SHOT);
    texture_release_shared(hotshot_texture_pvms, hotshot_texture_groups);
    set_end();
    if (hotshot_set != NULL) {
        loader_free(hotshot_set);
        hotshot_set = NULL;
    }
    messenger_messages_free();
    world_land_stop();
}

void hotshot_stage_step(struct task *task)
{
    NJS_POINT3 pos;

    pos.x = BOSS_START_X;
    pos.y = 0.0F;
    pos.z = 0.0F;
    hotshot_create(&pos, BOSS_START_ANG_Y);
    task->step = arena_step;
}

static void arena_hold_player(void)
{
    /* @hack spelling: the pointer to player 0's pos in a local, where
     * player_work[0]->pos at each use would do */
    NJS_POINT3 *pos = &player_work[0]->pos;

    if (pos->x > HOTSHOT_ARENA_HALF_X) {
        pos->x = HOTSHOT_ARENA_HALF_X;
    } else if (pos->x < -HOTSHOT_ARENA_HALF_X) {
        pos->x = -HOTSHOT_ARENA_HALF_X;
    }
    if (pos->z > HOTSHOT_ARENA_HALF_Z) {
        pos->z = HOTSHOT_ARENA_HALF_Z;
    } else if (pos->z < -HOTSHOT_ARENA_HALF_Z) {
        pos->z = -HOTSHOT_ARENA_HALF_Z;
    }
}

static void arena_step(struct task *task)
{
    arena_hold_player();
}
