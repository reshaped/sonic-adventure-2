/* the stage of stage.h: its init and exit, and its task's steps */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigbogy/boss.h"
#include "boss_bigbogy/stage.h"
#include "boss_bigbogy/object_archives.h"
#include "boss_bigbogy/terrain.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "enemy/enemy.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/messenger.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#ifdef NONMATCHING
/* bigbogy_motions, which this side's bigbogy_stage_step tests */
#include "boss_bigbogy/settings.h"
#endif

#define SET_MIN_ENTRIES 512
/* the light both players take */
#define PLAYER_LIGHT 0

const NJS_POINT3 bigbogy_arena_center = {200.0F, -40.0F, 0.0F};
const Float bigbogy_circle_radius = 200.0F;

static struct light stage_lights[4] = {
    {{0.0F, 0.707F, 0.707F}, 1.0F, 0.5F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.3F, 0.6F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.5F, 0.8F, 1.0F, 1.0F, 1.0F},
    {{0.0F, 0.707F, 0.707F}, 0.0F, 1.0F, 1.0F, 0.5F, 0.5F},
};
static struct set_head *stage_set = NULL;

static void arena_step(struct task *task);

void bigbogy_stage_init(void)
{
    world_land_start(&bigbogy_land);
    njSetBackColor(0xff3333ff, 0x003333ff, 0x00bbbbff);
    stage_set =
        set_read("set_b_BigBogy_s.bin", "set_b_BigBogy_u.bin", SET_MIN_ENTRIES);
    set_start(&bigbogy_object_list, stage_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(1.0F, 1.0F, 1.0F);
    light_set(PLAYER_LIGHT, &stage_lights[0]);
    light_set(1, &stage_lights[1]);
    light_set(LIGHT_LAND, &stage_lights[1]);
    light_set(BIGBOGY_BODY_LIGHT, &stage_lights[2]);
    light_set(9, &stage_lights[2]);
    light_set(BIGBOGY_FLASH_LIGHT, &stage_lights[3]);
    light_select(PLAYER_LIGHT, 0);
    light_select(PLAYER_LIGHT, 1);
    light_select_camera(LIGHT_LAND);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg50_light.bin");
    world_build_land(&bigbogy_land, (Uint32)STAGE_MODEL_BUILD_AREA);
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.unk_04) {
        player_hooks.unk_04 = dirt_create;
    }
    texture_load_shared(bigbogy_pvms, bigbogy_texture_groups);
    texture_load_pvm("modeltex_bossbogy", &bigbogy_model_texlist);
    texture_load_pvm("efftex_bossbogy", &bigbogy_effect_texlist);
    texture_load_pvm("titletex_bossbogy", &bigbogy_title_texlist);
    music_fade_in(0);
    music_play_or_defer("boss_02a.adx");
    messenger_messages_load("mh0053");
    sound_load_stage("se_ac_kbb.mlt", &bigbogy_sounds);
    enemy_groups_clear();
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
}

void bigbogy_stage_exit(void)
{
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.unk_04 == dirt_create) {
        player_hooks.unk_04 = NULL;
    }
    particle_core_stop();
    texture_release_shared(bigbogy_pvms, bigbogy_texture_groups);
    njReleaseTexture(&bigbogy_model_texlist);
    njReleaseTexture(&bigbogy_effect_texlist);
    njReleaseTexture(&bigbogy_title_texlist);
    set_end();
    if (stage_set) {
        loader_free(stage_set);
        stage_set = NULL;
    }
    messenger_messages_free();
    world_land_stop();
}

void bigbogy_stage_step(struct task *task)
{
    NJS_POINT3 position = {0.0F, 0.0F, 0.0F};
    position.x = bigbogy_arena_center.x - bigbogy_circle_radius;
    position.y = bigbogy_arena_center.y;
#ifdef NONMATCHING
    /* an entry 0 of bigbogy_motions whose start is its end leaves the arena
     * without the boss. a bigbogy_create that fails runs again next step */
    if (bigbogy_motions[0].start != bigbogy_motions[0].end &&
        !bigbogy_create(&position, 0, &bigbogy_arena_center)) {
        return;
    }
#else
    bigbogy_create(&position, 0, &bigbogy_arena_center);
#endif
    task->step = arena_step;
}

static void keep_player_in_arena(void)
{
    NJS_VECTOR offset;
    Float radius;
    offset = player_work[0]->pos;
    offset.x -= bigbogy_arena_center.x;
    offset.y = 0.0F;
    offset.z -= bigbogy_arena_center.z;
    radius = njScalor(&offset);
    if (radius > BIGBOGY_ARENA_RADIUS) {
        njUnitVector(&offset);
        player_work[0]->pos.x =
            (offset.x * BIGBOGY_ARENA_RADIUS) + bigbogy_arena_center.x;
        player_work[0]->pos.z =
            (offset.z * BIGBOGY_ARENA_RADIUS) + bigbogy_arena_center.z;
    }
}

static void arena_step(struct task *task)
{
    keep_player_in_arena();
}
