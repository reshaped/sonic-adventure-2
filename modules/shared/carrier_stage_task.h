/* the callbacks carrier_stage.h declares. shared/carrier_stage.c compiles
 * them for Weapons Bed, and WEAPONS_BED is then defined below.
 * stg15/weapons_bed_2p.c compiles them with CARRIER_2P for the two-player
 * deck, and stg20/versus_mech_1.c with CARRIER_DUEL for the first walker
 * duel */
#ifndef SHARED_CARRIER_STAGE_TASK_H
#define SHARED_CARRIER_STAGE_TASK_H

#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/black_smoke.h"
#include "shared/bomb_hat.h"
#include "shared/carrier_land.h"
#include "shared/carrier_sounds.h"
#include "shared/carrier_stage.h"
#include "shared/carrier_textures.h"
#include "shared/common_object.h"
#include "shared/fog.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/sea.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#ifdef CARRIER_DUEL
#include "camera/bosspoint.h"
#include "camera/stack.h"
#include "player/walker.h"
#include "stg20/rival_walker.h"
#include "system/pad.h"
#else
#include "shared/hint_box.h"
#endif
#if !defined(CARRIER_DUEL) && !defined(CARRIER_2P)
#include "shared/deck_figure.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "stage/path.h"
#define WEAPONS_BED
#endif

#ifdef WEAPONS_BED
#define STAGE_NUMBER "12"
#define MUSIC "W_BED.ADX"
#elif defined(CARRIER_2P)
#define STAGE_NUMBER "15"
#define MUSIC "w_bed.adx"
#else
#define STAGE_NUMBER "20"
#define MUSIC "BOSS_03.ADX"
#endif

#define BACKGROUND_LEVEL 1
/* the block the SET files are read into holds at least SET_ENTRIES 32-byte
 * slots, the head's among them */
#ifdef CARRIER_DUEL
#define SET_ENTRIES 1024
#else
#define SET_ENTRIES 2048
#endif

#ifdef CARRIER_DUEL
/* the stage task's modes in the duel. it starts the boss camera, then holds
 * the camera's target TARGET_HEIGHT above the rival */
enum duel_mode { MODE_START_CAMERA, MODE_FOLLOW_RIVAL };
#define TARGET_HEIGHT 18.0F
#endif

/* the stage's fog, which carrier_stage_init reads from the stage's fog file */
static struct fog_setting carrier_fog;

static struct set_head *loaded_set = NULL;

static void background_step(struct task *task);
static void background_destroy(struct task *task);
static void background_draw(struct task *task);

#ifdef WEAPONS_BED
#define SKY_BUILDS 1

static Uint8 *build_sky(Uint8 *buffer)
{
    njFogDisable();
    if (model_builds_compile(&buffer, sky_builds, SKY_BUILDS) == SKY_BUILDS) {
        sky_compiled = TRUE;
    }
    njFogEnable();
    return buffer;
}
#endif

void carrier_stage_init(void)
{
    struct task *task;
#ifdef CARRIER_DUEL
    if (stage_character(0) == PLAYER_EGGMAN_MECH) {
        walker_create_tails(1);
    } else if (stage_character(0) == PLAYER_TAILS_MECH) {
        walker_create_eggman(1);
    }
    if (player_tasks[1]) {
        stage_place_player(1);
        player_work[1]->id = WORK_ID_RIVAL;
        pad_disable_player(1);
        first_rival_walker_create();
        broken_down_smoke_create(0, NULL);
        broken_down_smoke_create(1, NULL);
    }
#endif
    world_land_start(&carrier_land);
    texture_load_shared(carrier_pvms, carrier_texture_groups);
    texture_await_shared(carrier_pvms, carrier_texture_groups);
    texture_load_shared(background_pvms, background_groups);
    texture_await_shared(background_pvms, background_groups);
#ifdef WEAPONS_BED
    loaded_set = set_read_stage(NULL, SET_ENTRIES);
#else
    loaded_set = set_read("set00" STAGE_NUMBER "_s.bin",
                          "set00" STAGE_NUMBER "_u.bin", SET_ENTRIES);
#endif
    set_start(&carrier_objects, loaded_set);
    loader_read_camera_files();
    particle_core_start();
#ifndef CARRIER_2P
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &deck_lights[0]);
    light_set(1, &deck_lights[1]);
    light_set(LIGHT_LAND, &deck_lights[0]);
    light_set(3, &deck_lights[0]);
#endif
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg" STAGE_NUMBER "_light.bin");
#ifdef WEAPONS_BED
    deck_figures_build(object_builds_run(build_sky((Uint8 *)world_build_land(
        &carrier_land, (Uint32)STAGE_MODEL_BUILD_AREA))));
#else
    world_build_land(&carrier_land, (Uint32)STAGE_MODEL_BUILD_AREA);
#endif
#ifndef CARRIER_DUEL
    music_fade_in(0);
    music_play_or_defer(MUSIC);
#endif
#ifdef WEAPONS_BED
    path_create_manager(carrier_paths);
#endif
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
        task->destructor = background_destroy;
    }
    kill_areas_start(kill_areas_on_deck);
#ifdef WEAPONS_BED
    module_motions_register();
#endif
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (!player_hooks.splash) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    if (!player_hooks.draw_blast_ring) {
        player_hooks.draw_blast_ring = bomb_hat_draw_ring;
    }
    fog_start_file("stg" STAGE_NUMBER "_fog.bin", &carrier_fog);
    njFogDisable();
    messenger_messages_load("mh00" STAGE_NUMBER);
#ifndef CARRIER_DUEL
    hint_box_hints_load("bh00" STAGE_NUMBER);
#endif
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
#ifdef CARRIER_DUEL
    music_fade_in(0);
    music_play_or_defer(MUSIC);
#else
    broken_down_smoke_create(0, NULL);
#ifdef CARRIER_2P
    if (player_tasks[1]) {
        broken_down_smoke_create(1, NULL);
    }
#endif
#endif
    sound_load_stage("se_ac_wb.mlt", &carrier_sound_list);
}

void carrier_stage_exit(void)
{
#ifndef CARRIER_DUEL
    hint_box_hints_free();
#endif
    messenger_messages_free();
    fog_stop();
    njFogDisable();
    if (player_hooks.draw_blast_ring == bomb_hat_draw_ring) {
        player_hooks.draw_blast_ring = NULL;
    }
#ifdef CARRIER_2P
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
#endif
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
#ifndef CARRIER_2P
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
#endif
    particle_core_stop();
    texture_release_shared(carrier_pvms, carrier_texture_groups);
    texture_release_shared(background_pvms, background_groups);
    set_end();
    if (loaded_set) {
        loader_free(loaded_set);
        loaded_set = NULL;
    }
    world_land_stop();
#ifdef WEAPONS_BED
    module_motions_unregister();
#endif
}

void carrier_stage_step(struct task *task)
{
    struct task_work *work = task->work;
#ifdef CARRIER_DUEL
    struct camera_bosspoint camera;
    switch (work->mode) {
    case MODE_START_CAMERA:
        camera.target = &work->pos;
        camera.dist = 50.0F;
        camera.height = 50.0F;
        camera.part = 0.5F;
        camera.adjust = CAMERA_ADJUST_RELATIVE3;
        camera.collides = 1;
        camera.trigger_push = 0;
        camera.lift = 20.0F;
        camera.level = 0;
        camera_bosspoint_start(0, &camera);
        work->mode = MODE_FOLLOW_RIVAL;
        /* fall through */
    case MODE_FOLLOW_RIVAL:
        if (player_work[1]) {
            work->pos = player_work[1]->pos;
            work->pos.y += TARGET_HEIGHT;
        }
        break;
    }
#else
    /* @hack redundant: a switch on the mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
#endif
}

static void background_step(struct task *task)
{
    sea_waves_step(0.0F, 0.0F);
}

static void background_destroy(struct task *task)
{
}

static void background_draw(struct task *task)
{
    NJS_VECTOR position;

    position.x = camera_views[camera_current_index]->pos.x;
    position.y = 0.0F;
    position.z = camera_views[camera_current_index]->pos.z;
    njFogDisable();
    njPushMatrixEx();
#ifdef CARRIER_DUEL
    njTranslateEx(&position);
#else
    njTranslateV(NULL, &position);
#endif
    njSetTexture(&carrier_sky_texlist);
#ifdef WEAPONS_BED
    if (sky_compiled) {
        njPushMatrixEx();
        njTranslateEx((NJS_VECTOR *)carrier_sky->pos);
        njDirectDrawObject(sky_builds[0].built);
        njPopMatrixEx();
    } else {
        njCnkSimpleDrawObject(carrier_sky);
    }
#else
    njCnkSimpleDrawObject(carrier_sky);
#endif
    njSetTexture(&sea_texlist);
#ifdef CARRIER_DUEL
    njTranslateEx((NJS_VECTOR *)sea_object->pos);
    sea_draw();
    njPopMatrixEx();
#else
    njTranslateV(NULL, (NJS_VECTOR *)sea_object->pos);
    sea_draw();
    njPopMatrix(1);
#endif
    njFogEnable();
}

#endif
