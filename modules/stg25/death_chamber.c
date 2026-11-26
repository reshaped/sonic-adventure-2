/* Death Chamber, stg25: the stage's init, exit and step, and its background.
 * the background picks the land blocks camera 0 draws. it plays the sounds
 * of the turning center and the pools. it sets the fog, and draws the
 * scenery and the water */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/big_splash.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/emerald.h"
#include "shared/egg_key_art.h"
#include "shared/flash.h"
#include "shared/fog.h"
#include "shared/footsteps.h"
#include "shared/hint_box.h"
#include "shared/iron_box_e.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/motions.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/painting.h"
#include "shared/ring.h"
#include "shared/skull.h"
#include "shared/store_box.h"
#include "shared/switch.h"
#include "shared/tint_pulse.h"
#include "shared/water_ripple.h"
#include "shared/water_specks.h"
#include "shared/wood_box_e.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg25/biribiri.h"
#include "stg25/bug_ornament.h"
#include "stg25/chamber_util.h"
#include "stg25/death_chamber.h"
#include "stg25/hunt_routes.h"
#include "stg25/room_scenery.h"
#include "stg25/object_archives.h"
#include "stg25/death_chamber_sounds.h"
#include "stg25/door_light.h"
#include "stg25/fire_mouth.h"
#include "stg25/fire_pot.h"
#include "stg25/floor_tiles.h"
#include "stg25/lion_head.h"
#include "stg25/map.h"
#include "stg25/sandglass_door.h"
#include "stg25/scorpion.h"
#include "stg25/torch.h"
#include "stg25/torch2.h"
#include "stg25/treasure.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"
#include "vmu/vibration.h"

#define BACK_COLOR 0
/* the task levels of the land blocks' task and of the background's */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
/* room for the set file's entries */
#define SET_ENTRIES 2048
#define EMERALD_PATHS 16
#define SPECKS 1270
/* player 0's footstep, an id in death_chamber_sound_list */
#define FOOTSTEP_SOUND 9

#define PIECE_SPIN 0xc0
#define PIECE_BOB_HEIGHT 3.0F
#define PIECE_BOB_SPEED 0x180

/* the background's sounds, ids in death_chamber_sound_list. a play of
 * SPIN_SOUND lasts SPIN_SOUND_STEPS + 1 updates, as sound_play_for says. a play
 * of WATER_SOUND lasts WATER_SOUND_STEPS + 1 */
#define CENTER_SOUND 6
#define CENTER_SOUND_PRIORITY 0
#define SPIN_SOUND 7
#define SPIN_SOUND_PRIORITY 1
#define SPIN_SOUND_STEPS 15
#define WATER_SOUND 10
#define WATER_SOUND_PRIORITY 1
#define WATER_SOUND_STEPS 10
/* CENTER_SOUND plays at the center's point, CENTER_Y above the origin. it
 * plays at frames CENTER_FRAME_1 and CENTER_FRAME_2 of the center's motion */
#define CENTER_FRAME_1 20
#define CENTER_FRAME_2 50
#define CENTER_Y 150.0F
/* SPIN_SOUND plays, and a spark draws, at four points that turn with the
 * spinner. before the turn, one lies at SPIN_X, SPIN_Y and SPIN_Z. the others
 * are its quarter turns about y */
#define SPIN_X 35.5F
#define SPIN_Y 160.0F
#define SPIN_Z 35.5F
#define VIBRATION_PLAYER 0
/* WATER_SOUND plays while player 0 lies in one of four pools. the west and
 * east pools share their bounds in z. the north and south pools share theirs
 * in x. west is toward -x, and south toward -z */
#define POOL_FLOOR (-30.1F)
#define WEST_POOL_WEST_X (-590.0F)
#define WEST_POOL_EAST_X (-300.0F)
#define EAST_POOL_WEST_X 300.0F
#define EAST_POOL_EAST_X 590.0F
#define HALL_POOLS_SOUTH_Z 740.0F
#define HALL_POOLS_NORTH_Z 1030.0F
#define FAR_POOLS_WEST_X 1360.0F
#define FAR_POOLS_EAST_X 1440.0F
#define NORTH_POOL_SOUTH_Z 300.0F
#define NORTH_POOL_NORTH_Z 680.0F
#define SOUTH_POOL_SOUTH_Z (-680.0F)
#define SOUTH_POOL_NORTH_Z (-300.0F)

/* the bounds of the places land_clip_step tells apart */
#define NORTH_Z 1100.0F
#define FAR_EAST_X 1100.0F
#define SIDE_X 290.0F
#define SIDE_Z 290.0F
#define SOUTH_FAR_Z (-600.0F)
#define HALL_HALF_WIDTH 410.0F
#define HALL_SOUTH_Z 600.0F
#define HALL_NORTH_Z 870.0F
#define SIDE_FAR_Z 600.0F
#define WEST_X (-250.0F)
#define WEST_SOUTH_Z 250.0F
#define CENTER_WEST_X (-400.0F)
#define CENTER_SOUTH_Z (-400.0F)
#define CENTER_LOW_Y (-30.0F)
#define CENTER_SPLIT_Z 215.0F

/* where the scenery draws */
#define CENTER_TOP_Y 200.0F
#define CENTER_MIDDLE_Y 80.0F
#define WALL_X 470.0F
#define WALL_Y (-30.0F)
#define WALL_Z 860.0F
#define HALL_PIECE_Y (-130.0F)
#define HALL_PIECE_Z 785.0F
#define NORTH_PIECE_X (-160.0F)
#define NORTH_PIECE_Y 100.0F
#define NORTH_PIECE_Z 1330.0F
#define EAST_PIECE_X 1400.0F
#define EAST_PIECE_Y (-30.0F)
#define EAST_PIECE_SIDE_Z 480.0F

/* a water surface shows its texture list's WATER_FRAMES textures in turn,
 * one a step. WATER_TID indexes the halfword of the surface's polygon list
 * that holds its texture's id */
#define WATER_FRAMES 14
#define WATER_TID 7
#define CENTER_WATER_X (-200.0F)
#define CENTER_WATER_Z (-200.0F)
#define SOUTH_WEST_WATER_X (-770.0F)
#define SOUTH_WEST_WATER_Z (-770.0F)
#define SOUTH_EAST_WATER_X 800.0F
#define SOUTH_EAST_WATER_Z (-800.0F)

static void background_step(struct task *task);
static void background_draw(struct task *task);
static void land_clip_step(struct task *task);
static void water_draw(void);

static struct kill_area death_chamber_kill_areas[2] = {
#include "stg25/death_chamber/death_chamber_kill_areas.inc"
};
static struct light death_chamber_lights[2] = {
    {{0.2F, -1.0F, 0.3F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.2F, -1.0F, 0.3F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* the set file death_chamber_init read, until death_chamber_exit frees it */
static struct set_head *death_chamber_set = NULL;
/* the water's specks, while their task lives */
static struct task *specks_task = NULL;
/* the spinner's turn each step */
static Sint32 spin_speed = 64;
/* the volumes of CENTER_SOUND and SPIN_SOUND. both play only while player 0
 * lies within center_sound_range of the center's point */
static Sint32 center_volume = 20;
static Sint32 spin_volume = 12;
static Float center_sound_range = 250.0F;
/* the vibration a CENTER_SOUND starts in player 0's pad. it starts only
 * where player 0 lies within vibration_range of the center's point */
static Sint32 vibration_power = 1;
static Sint32 vibration_freq = 2;
static Sint32 vibration_time = 0;
static Float vibration_range = 120.0F;
/* WATER_SOUND's volume. it runs from floor_volume at POOL_FLOOR to
 * top_volume at pool_depth above it */
static Sint32 floor_volume = -15;
static Sint32 top_volume = -100;
static Float pool_depth = 75.0F;
/* a light that no code reads */
static struct light dat_8c55b6c4 = {
    {0.3F, 0.1F, 1.0F}, 0.9F, 0.3F, 0.9F, 0.7F, 1.0F};

void death_chamber_init(void)
{
    struct task *task;
    Uint8 *buf;
    NJS_CNK_MODEL *model;

    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&death_chamber_land);
    texture_load_shared(death_chamber_pvms, death_chamber_texture_groups);
    texture_await_shared(death_chamber_pvms, death_chamber_texture_groups);
    texture_load_shared(death_chamber_background_pvms,
                        death_chamber_background_texture_groups);
    texture_await_shared(death_chamber_background_pvms,
                         death_chamber_background_texture_groups);
    death_chamber_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&death_chamber_objects, death_chamber_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &death_chamber_lights[0]);
    light_set(1, &death_chamber_lights[1]);
    light_set(2, &death_chamber_lights[0]);
    light_set(3, &death_chamber_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg25_light.bin");
    buf = object_builds_run((Uint8 *)world_build_land(
        &death_chamber_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    buf = wood_box_e_build(buf);
    buf = iron_box_e_build(buf);
    buf = chamber_bug_ornament_build(buf);
    buf = chamber_fire_mouth_build(buf);
    buf = chamber_fire_pot_build(buf);
    buf = chamber_lion_head_build(buf);
    buf = store_box_build(buf);
    buf = chamber_torch2_build(buf);
    buf = chamber_torch_build(buf);
    buf = chamber_treasure_build(buf);
    buf = painting_build(buf);
    buf = chamber_sandglass_door_build(buf);
    buf = chamber_scorpion_build(buf);
    buf = chamber_map_build(buf);
    chamber_door_light_build(buf);
    music_fade_in(0);
    music_play_or_defer("d_chambr.adx");
    path_create_manager(death_chamber_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
        footsteps_create(0, FOOTSTEP_SOUND, FOOTSTEP_SOUND, task);
    }
    kill_areas_start(death_chamber_kill_areas);
    module_motions_register();
    if (player_hooks.drop_rings == NULL) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (player_hooks.unk_04 == NULL) {
        player_hooks.unk_04 = dirt_create;
    }
    if (player_hooks.add_item_icon == NULL) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    if (player_hooks.create_piece == NULL) {
        player_hooks.create_piece = emerald_create;
    }
    if (player_hooks.show_piece_sign == NULL) {
        player_hooks.show_piece_sign = emerald_sign_create;
    }
    if (player_hooks.explode == NULL) {
        player_hooks.explode = explosion_create;
    }
    if (player_hooks.throw_skull == NULL) {
        player_hooks.throw_skull = skull_throw_up;
    }
    if (player_hooks.flash == NULL) {
        player_hooks.flash = flash_manager_create_sparks_depth;
    }
    if (player_hooks.ripple == NULL) {
        player_hooks.ripple = ripple_create;
    }
    if (player_hooks.splash == NULL) {
        player_hooks.splash = big_splash_create;
    }
    big_splash_load_texture();
    fog_read("stg25_fog.bin", &death_chamber_under_water_fog);
    fog_read("stg25_fogB.bin", &death_chamber_above_water_fog);
    njFogDisable();
    if (specks_task == NULL) {
        water_specks_create(0, SPECKS, &specks_task);
    }
    object_light_create();
    hunt_set_hints(death_chamber_hints);
    death_chamber_hint_file = hunt_load_hint_file("eh0025");
    emerald_set_paths(death_chamber_emerald_paths, EMERALD_PATHS);
    messenger_messages_load("mh0025");
    hint_box_hints_load("bh0025");
    smoke_task_create(0, NULL);
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    switch_clear_all();
    emerald_look.flags = 0;
    emerald_look.bob_height = PIECE_BOB_HEIGHT;
    emerald_look.bob_speed = PIECE_BOB_SPEED;
    /* @hack spelling: model, a local for each of the two models, where each
     * model's address stored in place would do */
    model = egg_key_model;
    emerald_look.models[0] = model;
    emerald_look.models[1] = model;
    emerald_look.models[2] = model;
    model = egg_key_glow_model;
    emerald_look.glow_models[0] = model;
    emerald_look.glow_models[1] = model;
    emerald_look.glow_models[2] = model;
    emerald_look.texlists[0] = &egg_key_texlist;
    emerald_look.texlists[1] = &egg_key_texlist;
    emerald_look.texlists[2] = &egg_key_texlist;
    emerald_look.spin = PIECE_SPIN;
    sound_load_stage("se_ac_dc.mlt", &death_chamber_sound_list);
    game_state.block_grid_loaded = TRUE;
    chamber_floor_tiles_create();
}

void death_chamber_exit(void)
{
    emerald_reset_look();
    hint_box_hints_free();
    messenger_messages_free();
    emerald_clear_paths();
    syFree(death_chamber_hint_file);
    death_chamber_hint_file = NULL;
    hunt_clear_hints();
    object_light_destroy();
    if (specks_task != NULL) {
        task_destroy(specks_task);
    }
    fog_stop();
    njFogDisable();
    if (player_hooks.splash == big_splash_create) {
        player_hooks.splash = NULL;
    }
    big_splash_release_texture();
    if (player_hooks.ripple == ripple_create) {
        player_hooks.ripple = NULL;
    }
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
    if (player_hooks.explode == explosion_create) {
        player_hooks.explode = NULL;
    }
    if (player_hooks.throw_skull == skull_throw_up) {
        player_hooks.throw_skull = NULL;
    }
    if (player_hooks.flash == flash_manager_create_sparks_depth) {
        player_hooks.flash = NULL;
    }
    particle_core_stop();
    texture_release_shared(death_chamber_pvms, death_chamber_texture_groups);
    texture_release_shared(death_chamber_background_pvms,
                           death_chamber_background_texture_groups);
    set_end();
    if (death_chamber_set != NULL) {
        loader_free(death_chamber_set);
        death_chamber_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void death_chamber_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    fog_start();
    /* @hack redundant: a switch of one empty case, where nothing would do */
    switch (work->mode) {
    case 0:
        break;
    }
}

/* each SPIN_SOUND plays for a key of its own, task's address plus 1 to 4.
 * one key would move a single slot from point to point */
static void background_step(struct task *task)
{
    /* @hack redundant: work, a local for task->work that nothing reads */
    struct task_work *work = task->work;
    int volume;

    if (game_state.land_block_mask[0] & CENTER_BLOCKS) {
        NJS_POINT3 center;
        NJS_POINT3 pos;
        NJS_POINT3 turned;
        Uint32 frame;

        frame = game_state.steps % death_chamber_center_motion->nbFrame;
        center.x = center.z = 0.0F;
        center.y = CENTER_Y;
        if (njDistanceP2P(&player_work[0]->pos, &center) < center_sound_range) {
            if (frame == CENTER_FRAME_1 || frame == CENTER_FRAME_2) {
                sound_play_at(CENTER_SOUND, NULL, CENTER_SOUND_PRIORITY,
                              center_volume, &center);
                if (njDistanceP2P(&player_work[0]->pos, &center) <
                    vibration_range) {
                    vibration_start(VIBRATION_PLAYER, vibration_power,
                                    vibration_freq, vibration_time);
                }
            }
            pos.x = SPIN_X;
            pos.y = SPIN_Y;
            pos.z = SPIN_Z;
            chamber_point_turn_y(pos.x, pos.z, spin_speed * game_state.steps,
                                 &turned);
            turned.y = pos.y;
            pos = turned;
            sound_play_at_for(SPIN_SOUND, (char *)task + 1, SPIN_SOUND_PRIORITY,
                              spin_volume, SPIN_SOUND_STEPS, &pos);
            pos.x = -turned.x;
            pos.z = -turned.z;
            sound_play_at_for(SPIN_SOUND, (char *)task + 2, SPIN_SOUND_PRIORITY,
                              spin_volume, SPIN_SOUND_STEPS, &pos);
            pos.x = -turned.z;
            pos.z = turned.x;
            sound_play_at_for(SPIN_SOUND, (char *)task + 3, SPIN_SOUND_PRIORITY,
                              spin_volume, SPIN_SOUND_STEPS, &pos);
            pos.x = turned.z;
            pos.z = -turned.x;
            sound_play_at_for(SPIN_SOUND, (char *)task + 4, SPIN_SOUND_PRIORITY,
                              spin_volume, SPIN_SOUND_STEPS, &pos);
        }
    }
    /* @hack spelling: a block of its own for pos, where pos declared with the
     * function's locals would do */
    {
        NJS_POINT3 pos;

        pos = player_work[0]->pos;
        /* @hack spelling: each bound before pos.x and pos.z in its test, where
         * pos.x and pos.z before their bounds would do */
        if (pos.y >= POOL_FLOOR && pos.y < pool_depth + POOL_FLOOR &&
            ((WEST_POOL_WEST_X < pos.x && pos.x < WEST_POOL_EAST_X &&
              HALL_POOLS_SOUTH_Z < pos.z && pos.z < HALL_POOLS_NORTH_Z) ||
             (EAST_POOL_WEST_X < pos.x && pos.x < EAST_POOL_EAST_X &&
              HALL_POOLS_SOUTH_Z < pos.z && pos.z < HALL_POOLS_NORTH_Z) ||
             (FAR_POOLS_WEST_X < pos.x && pos.x < FAR_POOLS_EAST_X &&
              NORTH_POOL_SOUTH_Z < pos.z && pos.z < NORTH_POOL_NORTH_Z) ||
             (FAR_POOLS_WEST_X < pos.x && pos.x < FAR_POOLS_EAST_X &&
              SOUTH_POOL_SOUTH_Z < pos.z && pos.z < SOUTH_POOL_NORTH_Z))) {
            volume = ((Float)(top_volume - floor_volume) / pool_depth *
                      (pos.y - POOL_FLOOR)) +
                     floor_volume;
            sound_play_for(WATER_SOUND, task, WATER_SOUND_PRIORITY, 0,
                           WATER_SOUND_STEPS);
            sound_set_volume(task, SOUND_ANY_ID, volume);
        }
    }
}

static void background_draw(struct task *task)
{
    NJS_POINT3 pos;
    NJS_POINT3 turned;
    /* @hack spelling: state, a local for game_state's address, where
     * game_state read in place would do */
    struct game_state *state = &game_state;
    Uint32 color;
    int step;

    step = state->steps % TINT_PULSE_STEPS;
    if (step < TINT_PULSE_RISE) {
        color = TINT_PULSE_RISE_ALPHA(step) | TINT_PULSE_COLORS;
    } else {
        color = TINT_PULSE_FALL_ALPHA(step) | TINT_PULSE_COLORS;
    }
    if (state->land_block_mask[0] & CENTER_BLOCKS) {
        njSetTexture(&death_chamber_center_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, CENTER_TOP_Y, 0.0F);
        model_draw_tinted(&death_chamber_center_top_model, color);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, CENTER_MIDDLE_Y, 0.0F);
        model_draw_tinted(&death_chamber_center_middle_model, color);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, 0.0F);
        njRotateY(NULL, EIGHTH_TURN);
        model_draw_tinted(&death_chamber_center_base_model, color);
        njPopMatrixEx();
        njPushMatrixEx();
        njSetTexture(&death_chamber_center_object_texlist);
        njCnkEasyDrawMotion(
            &death_chamber_center_object, death_chamber_center_motion,
            state->steps % death_chamber_center_motion->nbFrame);
        njPopMatrixEx();
        pos.x = SPIN_X;
        pos.y = SPIN_Y;
        pos.z = SPIN_Z;
        njPushMatrixEx();
        njRotateY(NULL, spin_speed * state->steps);
        njCnkEasyDrawModel(&death_chamber_spinner_model);
        njPopMatrixEx();
        chamber_point_turn_y(pos.x, pos.z, spin_speed * state->steps, &turned);
        turned.y = pos.y;
        pos = turned;
        chamber_biribiri_draw_at(&pos);
        pos.x = -turned.x;
        pos.z = -turned.z;
        chamber_biribiri_draw_at(&pos);
        pos.x = -turned.z;
        pos.z = turned.x;
        chamber_biribiri_draw_at(&pos);
        pos.x = turned.z;
        pos.z = -turned.x;
        chamber_biribiri_draw_at(&pos);
    }
    if ((state->land_block_mask[0] & WEST_FAR_BLOCKS) ||
        (state->land_block_mask[0] & HALL_BLOCKS)) {
        njPushMatrixEx();
        njTranslate(NULL, -WALL_X, WALL_Y, WALL_Z);
        njSetTexture(&death_chamber_wall_texlist);
        model_draw_tinted(&death_chamber_wall_model, color);
        njPopMatrixEx();
    }
    if ((state->land_block_mask[0] & EAST_FAR_BLOCKS) ||
        (state->land_block_mask[0] & HALL_BLOCKS)) {
        njPushMatrixEx();
        njTranslate(NULL, WALL_X, WALL_Y, WALL_Z);
        njRotateY(NULL, -QUARTER_TURN);
        njSetTexture(&death_chamber_wall_texlist);
        model_draw_tinted(&death_chamber_wall_model, color);
        njPopMatrixEx();
    }
    if (state->land_block_mask[0] & HALL_BLOCKS) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, HALL_PIECE_Y, HALL_PIECE_Z);
        /* @hack redundant: a texture list set that the next set replaces */
        njSetTexture(&death_chamber_hall_texlist);
        njSetTexture(&death_chamber_wall_texlist);
        model_draw_tinted(&death_chamber_hall_model, color);
        njPopMatrixEx();
    }
    if (state->land_block_mask[0] & NORTH_BLOCKS) {
        njPushMatrixEx();
        njTranslate(NULL, NORTH_PIECE_X, NORTH_PIECE_Y, NORTH_PIECE_Z);
        /* @hack redundant: a texture list set that the next set replaces */
        njSetTexture(&death_chamber_north_texlist);
        njSetTexture(&death_chamber_wall_texlist);
        model_draw_tinted(&death_chamber_north_model, color);
        njPopMatrixEx();
    }
    if (state->land_block_mask[0] & FAR_EAST_BLOCKS) {
        /* @hack redundant: a texture list set that a set after it replaces */
        njSetTexture(&death_chamber_wall_texlist);
        njPushMatrixEx();
        njTranslate(NULL, EAST_PIECE_X, EAST_PIECE_Y, 0.0F);
        njRotateY(NULL, QUARTER_TURN);
        njSetTexture(&death_chamber_hall_texlist);
        model_draw_tinted(&death_chamber_hall_model, color);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, EAST_PIECE_X, EAST_PIECE_Y, EAST_PIECE_SIDE_Z);
        njSetTexture(&death_chamber_east_north_texlist);
        model_draw_tinted(&death_chamber_east_north_model, color);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, EAST_PIECE_X, EAST_PIECE_Y, -EAST_PIECE_SIDE_Z);
        njSetTexture(&death_chamber_east_south_texlist);
        model_draw_tinted(&death_chamber_east_south_model, color);
        njPopMatrixEx();
    }
    /* @hack redundant: a block no path reaches, of four pushes of the
     * matrix */
    if (0) {
        njPushMatrixEx();
        njPushMatrixEx();
        njPushMatrixEx();
        njPushMatrixEx();
    }
    if (camera_views[camera_current_index]->pos.y < chamber_water_level()) {
        fog_set(&death_chamber_under_water_fog);
        if (specks_task != NULL) {
            specks_task->work->mode = MODE_SPECKS_SHOWN;
        }
    } else {
        fog_set(&death_chamber_above_water_fog);
        if (specks_task != NULL) {
            specks_task->work->mode = MODE_SPECKS_HIDDEN;
        }
        water_draw();
    }
}

/* give camera 0 the land blocks of the place where the current camera
 * stands */
static void land_clip_step(struct task *task)
{
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    NJS_POINT3 pos = camera_views[camera_current_index]->pos;
    Sint32 *mask = &game_state.land_block_mask[0];

    *mask = 0;
#else
    /* @hack type: state points at dat_8c0db20c, game_state as a struct
     * game_state_point, whose land block masks are fields of their own.
     * game_state.land_block_mask would do */
    struct game_state_point *state;
    Sint32 *mask;
    NJS_POINT3 pos;

    pos = camera_views[camera_current_index]->pos;
    state = &dat_8c0db20c;
    /* @hack redundant: the mask's address taken before the clear, and again
     * after it */
    mask = &state->land_block_mask_0;
    state->land_block_mask_0 = 0;
    mask = &state->land_block_mask_0;
#endif
    if (pos.z >= NORTH_Z) {
        *mask = NORTH_BLOCKS;
    } else if (pos.x >= FAR_EAST_X) {
        *mask = FAR_EAST_BLOCKS;
    } else if (pos.x >= SIDE_X && pos.z < -SIDE_Z) {
        if (pos.z >= SOUTH_FAR_Z) {
            *mask = SOUTH_EAST_NEAR_BLOCKS;
        } else {
            *mask = SOUTH_EAST_FAR_BLOCKS;
        }
    } else if (pos.x >= -HALL_HALF_WIDTH && pos.x < HALL_HALF_WIDTH &&
               pos.z >= HALL_SOUTH_Z && pos.z < HALL_NORTH_Z) {
        *mask = HALL_BLOCKS;
    } else if (pos.x >= SIDE_X && pos.z >= SIDE_Z) {
        if (pos.z < SIDE_FAR_Z) {
            *mask = EAST_NEAR_BLOCKS;
        } else {
            *mask = EAST_FAR_BLOCKS;
        }
    } else if (pos.x < WEST_X && pos.z >= WEST_SOUTH_Z) {
        if (pos.z < SIDE_FAR_Z) {
            *mask = WEST_NEAR_BLOCKS;
        } else {
            *mask = WEST_FAR_BLOCKS;
        }
    } else if (pos.x >= CENTER_WEST_X && pos.z >= CENTER_SOUTH_Z &&
               pos.y >= CENTER_LOW_Y) {
        if (pos.z >= CENTER_SPLIT_Z) {
            *mask = CENTER_NORTH_BLOCKS;
        } else {
            *mask = CENTER_SOUTH_BLOCKS;
        }
    } else if (pos.z < SOUTH_FAR_Z) {
        *mask = SOUTH_WEST_BLOCKS;
    } else {
        *mask = REST_BLOCKS;
    }
}

static void water_draw(void)
{
    if (game_state.land_block_mask[0] & (REST_BLOCKS | CENTER_SOUTH_BLOCKS)) {
        /* @hack spelling: texture's address and value worked out in steps,
         * where one assignment would do */
        Sint16 *texture = death_chamber_center_water->plist;
        int tid;

        texture = texture + WATER_TID;
        tid = *texture;
        tid = tid & NJD_FFM_MASK;
        tid = tid | (game_state.steps % WATER_FRAMES);
        *texture = tid;
        /* @hack redundant: mentions of the three matrix calls before the
         * calls */
        (void)njTranslate;
        (void)njPushMatrixEx;
        (void)njPopMatrixEx;
        njPushMatrixEx();
        njTranslate(NULL, CENTER_WATER_X, 0.0F, CENTER_WATER_Z);
        njSetTexture(&death_chamber_water_texlist);
        njCnkEasyDrawModel(death_chamber_center_water);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & SOUTH_WEST_BLOCKS) {
        /* @hack spelling: texture's address and value worked out in steps,
         * where one assignment would do */
        Sint16 *texture = death_chamber_south_west_water->plist;
        int tid;

        texture = texture + WATER_TID;
        tid = *texture;
        tid = tid & NJD_FFM_MASK;
        tid = tid | (game_state.steps % WATER_FRAMES);
        *texture = tid;
        njPushMatrixEx();
        njTranslate(NULL, SOUTH_WEST_WATER_X, 0.0F, SOUTH_WEST_WATER_Z);
        njSetTexture(&death_chamber_water_texlist);
        njCnkEasyDrawModel(death_chamber_south_west_water);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & SOUTH_EAST_FAR_BLOCKS) {
        /* @hack spelling: texture's address and value worked out in steps,
         * where one assignment would do */
        Sint16 *texture = death_chamber_south_east_water->plist;
        int tid;

        texture = texture + WATER_TID;
        tid = *texture;
        tid = tid & NJD_FFM_MASK;
        tid = tid | (game_state.steps % WATER_FRAMES);
        *texture = tid;
        njPushMatrixEx();
        njTranslate(NULL, SOUTH_EAST_WATER_X, 0.0F, SOUTH_EAST_WATER_Z);
        njSetTexture(&death_chamber_water_texlist);
        njCnkEasyDrawModel(death_chamber_south_east_water);
        njPopMatrixEx();
    }
}
