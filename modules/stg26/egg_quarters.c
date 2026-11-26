/* Egg Quarters, the stage of STG26: its init, exit and step, and its
 * background */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/color.h"
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
#include "stg26/egg_quarters.h"
#include "stg26/object_archives.h"
#include "stg26/guard.h"
#include "stg26/guard_eye.h"
#include "stg26/hiding_spot.h"
#include "stg26/hunt_routes.h"
#include "stg26/prop_builds.h"
#include "stg26/quarters_area.h"
#include "stg26/quarters_sounds.h"
#include "stg26/quarters_util.h"
#include "stg26/room_scenery.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

#define BACK_COLOR 0
/* the task levels of the land blocks' task, of the background's and of the
 * hiding spots' managers */
#define CLIP_LEVEL 0
#define BACKGROUND_LEVEL 1
#define HIDING_LEVEL 1
/* room for the set file's entries */
#define SET_ENTRIES 2048
#define EMERALD_PATHS 16
#define SPECKS 1270
#define PILLARS 4
#define PILLAR_TRAVEL 30.0F
#define WAVE_DEPTH 0.2F

#define PIECE_SPIN 0xc0
#define PIECE_BOB_HEIGHT 3.0F
#define PIECE_BOB_SPEED 0x180

/* a play of the pools' sound, QUARTERS_WATER, lasts WATER_SOUND_STEPS + 1
 * updates, as sound_play_for says */
#define WATER_SOUND_PRIORITY 1
#define WATER_SOUND_STEPS 10
/* the pools lie in two pairs, one pool west of x 0 and one east of it. each
 * east pool's bounds in x are its west pool's, negated. the north pair lies
 * from NORTH_POOLS_FLOOR up, and the south pair from SOUTH_POOLS_FLOOR up */
#define NORTH_POOLS_FLOOR (-30.1F)
#define NORTH_POOLS_INNER_X 300.0F
#define NORTH_POOLS_OUTER_X 590.0F
#define NORTH_POOLS_SOUTH_Z 930.0F
#define NORTH_POOLS_NORTH_Z 1220.0F
#define SOUTH_POOLS_FLOOR 19.9F
#define SOUTH_POOLS_INNER_X 180.0F
#define SOUTH_POOLS_OUTER_X 340.0F
#define SOUTH_POOLS_SOUTH_Z 180.0F
#define SOUTH_POOLS_NORTH_Z 340.0F

/* where the bands and the water draw */
#define NORTH_BAND_Y 120.0F
#define NORTH_BAND_Z 1295.0F
#define SIDE_BAND_X 470.0F
#define SIDE_BAND_Y (-30.0F)
#define SIDE_BAND_Z 1100.0F
#define WATER_Z 250.0F
/* a water surface shows its texture list's WATER_FRAMES textures in turn,
 * one a step. WATER_TID indexes the halfword of the surface's polygon list
 * that holds its texture's id */
#define WATER_FRAMES 14
#define WATER_TID 7

static void background_step(struct task *task);
static void draw_modifiers(NJS_CNK_OBJECT *group);
static void background_draw(struct task *task);
static void land_clip_step(struct task *task);
static void water_draw(void);
static void stage_idle(struct task *task);

static struct kill_area quarters_kill_areas[2] = {
#include "stg26/egg_quarters/quarters_kill_areas.inc"
};
static struct light stage_lights[2] = {
    {{0.2F, -1.0F, 0.3F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.2F, -1.0F, 0.3F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* each pillar's motion. a step moves a pillar's height by its speed, and its
 * angle by its turn. a wave of depth WAVE_DEPTH, at the pillar's frequency,
 * swells and shrinks both. no code reads the angles */
/* @hack spelling: each frequency and turn has a name of its own, where a row
 * for each pillar would do */
static Angle pillar_0_frequency = 256;
static Angle pillar_0_turn = 896;
static Angle pillar_1_frequency = 384;
static Angle pillar_1_turn = 768;
static Angle pillar_2_frequency = 128;
static Angle pillar_2_turn = 960;
static Angle pillar_3_frequency = 192;
static Angle pillar_3_turn = 832;
/* @hack spelling: each speed has a name of its own, where an array would
 * do */
static Float pillar_0_speed = 1.0F;
static Float pillar_1_speed = 1.1F;
static Float pillar_2_speed = 0.9F;
static Float pillar_3_speed = 0.8F;
/* the set file egg_quarters_init read, until egg_quarters_exit frees it */
static struct set_head *quarters_set = NULL;
/* the water's specks, while their task lives */
static struct task *specks_task = NULL;
/* the pools' sound plays while player 0 lies in one of them, at most
 * pool_depth above its floor. its volume runs from floor_volume at the floor
 * to top_volume pool_depth above it */
static Sint32 floor_volume = -15;
static Sint32 top_volume = -100;
static Float pool_depth = 75.0F;
/* a light that no code reads */
static struct light dat_8c55814c = {
    {0.3F, 0.1F, 1.0F}, 0.9F, 0.3F, 0.9F, 0.7F, 1.0F};

static struct fog_setting underwater_fog;
static struct fog_setting clear_fog;
static Angle pillar_angles[PILLARS];
static Bool pillar_descends[PILLARS];
static Float pillar_heights[PILLARS];

void egg_quarters_init(void)
{
    struct task *task;
    Uint8 *buf;

    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&quarters_land);
    texture_load_shared(quarters_pvms, quarters_texture_groups);
    texture_await_shared(quarters_pvms, quarters_texture_groups);
    texture_load_shared(quarters_background_pvms, quarters_background_groups);
    texture_await_shared(quarters_background_pvms, quarters_background_groups);
    quarters_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&egg_quarters_objects, quarters_set);
    loader_read_camera_files();
    particle_core_start();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &stage_lights[0]);
    light_set(1, &stage_lights[1]);
    light_set(2, &stage_lights[0]);
    light_set(3, &stage_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    lights_load("stg26_light.bin");
    buf = object_builds_run((Uint8 *)world_build_land(
        &quarters_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    buf = wood_box_e_build(buf);
    buf = iron_box_e_build(buf);
    buf = quarters_fire_mouth_build(buf);
    buf = quarters_fire_pot_build(buf);
    buf = quarters_machinery_build(buf);
    buf = quarters_coffin_build(buf);
    buf = guard_build(buf);
    buf = guard_eye_build(buf);
    buf = quarters_curtain_build(buf);
    buf = quarters_propeller_build(buf);
    buf = quarters_propeller2_build(buf);
    buf = quarters_wall_torch_build(buf);
    buf = quarters_torch_build(buf);
    buf = quarters_treasure_build(buf);
    buf = painting_build(buf);
    buf = store_box_build(buf);
    quarters_snake_pot_build(buf);
    music_fade_in(0);
    music_play_or_defer("e_quart.adx");
    path_create_manager(quarters_paths);
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task != NULL) {
        task->draw = background_draw;
        footsteps_create(0, QUARTERS_FOOTSTEP, QUARTERS_FOOTSTEP, task);
    }
    /* @hack spelling: the four assignments of each array chained, where four
     * statements would do */
    pillar_descends[0] = pillar_descends[1] = pillar_descends[2] =
        pillar_descends[3] = FALSE;
    pillar_heights[0] = pillar_heights[1] = pillar_heights[2] =
        pillar_heights[3] = 0.0F;
    kill_areas_start(quarters_kill_areas);
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
    if (player_hooks.in_shadow == NULL) {
        player_hooks.in_shadow = guard_player_in_shadow;
    }
    fog_read("stg26_fog.bin", &underwater_fog);
    fog_read("stg26_fogB.bin", &guard_radar_fog);
    fog_read("stg26_fogC.bin", &clear_fog);
    njFogDisable();
    if (specks_task == NULL) {
        water_specks_create(0, SPECKS, &specks_task);
    }
    object_light_create();
    hunt_set_hints(quarters_hints);
    quarters_hint_file = hunt_load_hint_file("eh0026");
    emerald_set_paths(quarters_piece_paths, EMERALD_PATHS);
    messenger_messages_load("mh0026");
    hint_box_hints_load("bh0026");
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
    emerald_look.models[0] = egg_key_model;
    emerald_look.models[1] = egg_key_model;
    emerald_look.models[2] = egg_key_model;
    emerald_look.glow_models[0] = egg_key_glow_model;
    emerald_look.glow_models[1] = egg_key_glow_model;
    emerald_look.glow_models[2] = egg_key_glow_model;
    emerald_look.texlists[0] = &egg_key_texlist;
    emerald_look.texlists[1] = &egg_key_texlist;
    emerald_look.texlists[2] = &egg_key_texlist;
    emerald_look.spin = PIECE_SPIN;
    sound_load_stage("se_ac_eq.mlt", &quarters_sounds);
    game_state.block_grid_loaded = TRUE;
    task_create(TASK_WORK, HIDING_LEVEL, hiding_light_manager_init,
                "ObjectHideLightManager");
    task_create(TASK_WORK, HIDING_LEVEL, hiding_collision_manager_init,
                "ObjectHideColiManager");
    guard_radar_create();
}

void egg_quarters_exit(void)
{
    emerald_reset_look();
    hint_box_hints_free();
    messenger_messages_free();
    emerald_clear_paths();
    syFree(quarters_hint_file);
    quarters_hint_file = NULL;
    hunt_clear_hints();
    object_light_destroy();
    if (specks_task != NULL) {
        task_destroy(specks_task);
    }
    fog_stop();
    njFogDisable();
    if (player_hooks.in_shadow == guard_player_in_shadow) {
        player_hooks.in_shadow = NULL;
    }
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
    texture_release_shared(quarters_pvms, quarters_texture_groups);
    texture_release_shared(quarters_background_pvms,
                           quarters_background_groups);
    set_end();
    if (quarters_set != NULL) {
        loader_free(quarters_set);
        quarters_set = NULL;
    }
    world_land_stop();
    module_motions_unregister();
}

void egg_quarters_step(struct task *task)
{
    fog_start();
    guards_start();
    task->step = stage_idle;
}

static void stage_idle(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a switch of one empty case, where nothing would do */
    switch (work->mode) {
    case 0:
        break;
    }
}

/* pillar i climbs to PILLAR_TRAVEL, then sinks back to 0, over and over */
/* @hack spelling: phase set inside the switch's controlling expression,
 * where a statement before it would do */
#define MOVE_PILLAR(i, frequency, speed)                                       \
    switch (phase = (frequency) * frame, pillar_descends[i]) {                 \
    case FALSE:                                                                \
        pillar_heights[i] += (1.0F + (WAVE_DEPTH * cosine(phase))) * (speed);  \
        if (pillar_heights[i] >= PILLAR_TRAVEL) {                              \
            pillar_heights[i] = PILLAR_TRAVEL;                                 \
            pillar_descends[i] = TRUE;                                         \
        }                                                                      \
        break;                                                                 \
    case TRUE:                                                                 \
        pillar_heights[i] -= (1.0F + (WAVE_DEPTH * cosine(phase))) * (speed);  \
        if (pillar_heights[i] <= 0.0F) {                                       \
            pillar_heights[i] = 0.0F;                                          \
            pillar_descends[i] = FALSE;                                        \
        }                                                                      \
        break;                                                                 \
    }

static void background_step(struct task *task)
{
    int volume;
    Angle phase;
    Uint32 frame = game_state.steps;
    NJS_POINT3 pos;
    /* @hack spelling: pillar_angles written through angles, where the array
     * itself would do */
    Angle *angles = pillar_angles;
    /* @hack spelling: njCos called through cosine, a local set within the
     * first call, where calls of njCos would do */
    Float (*cosine)(Angle);

    angles[0] +=
        (1.0F + (WAVE_DEPTH *
                 (Float)(cosine = njCos, cosine(pillar_0_frequency * frame)))) *
        pillar_0_turn;
    angles[1] += (1.0F + (WAVE_DEPTH * cosine(pillar_1_frequency * frame))) *
                 pillar_1_turn;
    angles[2] += (1.0F + (WAVE_DEPTH * cosine(pillar_2_frequency * frame))) *
                 pillar_2_turn;
    angles[3] += (1.0F + (WAVE_DEPTH * cosine(pillar_3_frequency * frame))) *
                 pillar_3_turn;
    MOVE_PILLAR(0, pillar_0_frequency, pillar_0_speed);
    MOVE_PILLAR(1, pillar_1_frequency, pillar_1_speed);
    MOVE_PILLAR(2, pillar_2_frequency, pillar_2_speed);
    MOVE_PILLAR(3, pillar_3_frequency, pillar_3_speed);

    pos = player_work[0]->pos;
    /* @hack spelling: each low bound before pos.x and pos.z in its test, where
     * pos.x and pos.z before their bounds would do */
    if (pos.y >= NORTH_POOLS_FLOOR && pos.y < NORTH_POOLS_FLOOR + pool_depth &&
        ((-NORTH_POOLS_OUTER_X < pos.x && pos.x < -NORTH_POOLS_INNER_X &&
          NORTH_POOLS_SOUTH_Z < pos.z && pos.z < NORTH_POOLS_NORTH_Z) ||
         (NORTH_POOLS_INNER_X < pos.x && pos.x < NORTH_POOLS_OUTER_X &&
          NORTH_POOLS_SOUTH_Z < pos.z && pos.z < NORTH_POOLS_NORTH_Z))) {
        volume = ((top_volume - floor_volume) / pool_depth *
                  (pos.y - NORTH_POOLS_FLOOR)) +
                 floor_volume;
        sound_play_for(QUARTERS_WATER, task, WATER_SOUND_PRIORITY, 0,
                       WATER_SOUND_STEPS);
        sound_set_volume(task, SOUND_ANY_ID, volume);
    }
    if (pos.y >= SOUTH_POOLS_FLOOR && pos.y < SOUTH_POOLS_FLOOR + pool_depth &&
        ((-SOUTH_POOLS_OUTER_X < pos.x && pos.x < -SOUTH_POOLS_INNER_X &&
          SOUTH_POOLS_SOUTH_Z < pos.z && pos.z < SOUTH_POOLS_NORTH_Z) ||
         (SOUTH_POOLS_INNER_X < pos.x && pos.x < SOUTH_POOLS_OUTER_X &&
          SOUTH_POOLS_SOUTH_Z < pos.z && pos.z < SOUTH_POOLS_NORTH_Z))) {
        volume = ((top_volume - floor_volume) / pool_depth *
                  (pos.y - SOUTH_POOLS_FLOOR)) +
                 floor_volume;
        sound_play_for(QUARTERS_WATER, task, WATER_SOUND_PRIORITY, 0,
                       WATER_SOUND_STEPS);
        sound_set_volume(task, SOUND_ANY_ID, volume);
    }
}

static void draw_modifiers(NJS_CNK_OBJECT *group)
{
    NJS_CNK_OBJECT *child = group->child;

    while (child) {
        njPushMatrixEx();
        njTranslate(NULL, child->pos[0], child->pos[1], child->pos[2]);
        njRotateXYZ(NULL, child->ang[0], child->ang[1], child->ang[2]);
        njCnkModDrawModel(child->model);
        njPopMatrixEx();
        child = child->sibling;
    }
}

#define NORTH_BAND_BLOCKS                                                      \
    (QUARTERS_NORTH | QUARTERS_NORTHWEST | QUARTERS_NORTHEAST)
#define WEST_BAND_BLOCKS                                                       \
    (QUARTERS_WEST | QUARTERS_NORTHWEST | QUARTERS_INNER_NORTHWEST)
#define EAST_BAND_BLOCKS                                                       \
    (QUARTERS_EAST | QUARTERS_NORTHEAST | QUARTERS_INNER_NORTHEAST)
#define DRAW_PILLAR(i, texlist, model)                                         \
    height = pillar_heights[i];                                                \
    njPushMatrixEx();                                                          \
    njTranslate(NULL, 0.0F, height, 0.0F);                                     \
    njSetTexture(&(texlist));                                                  \
    njCnkEasyDrawModel(&(model));                                              \
    njPopMatrixEx()

static void background_draw(struct task *task)
{
    int phase;
    Uint32 color;
    Float height;
    Float surface;
    Float side_y;
    Float side_z;
    Uint32 white;
    Sint32 blocks;

    phase = game_state.steps % TINT_PULSE_STEPS;
    /* @hack spelling: white, a local for TINT_PULSE_COLORS, where the
     * constant written in place would do */
    white = TINT_PULSE_COLORS;
    /* @hack type: each level converted through int, where a conversion to
     * Uint32 would do */
    color = phase < TINT_PULSE_RISE
                ? ((Uint32)(int)(phase * TINT_PULSE_RISE_RATE) << ALPHA_SHIFT) |
                      white
                : ((Uint32)(int)(((TINT_PULSE_STEPS - 1) - phase) *
                                 TINT_PULSE_FALL_RATE)
                   << ALPHA_SHIFT) |
                      white;
    if (game_state.land_block_mask[0] & NORTH_BAND_BLOCKS) {
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, NORTH_BAND_Y, NORTH_BAND_Z);
        njRotateY(NULL, HALF_TURN);
        njSetTexture(&quarters_band_texlist);
        model_draw_tinted(&quarters_north_band, color);
        njPopMatrixEx();
    }
    side_y = SIDE_BAND_Y;
    side_z = SIDE_BAND_Z;
    /* @hack redundant: two increments of side_z, in a block no path
     * reaches */
    if (0) {
        side_z++;
        side_z++;
    }
    if (game_state.land_block_mask[0] & WEST_BAND_BLOCKS) {
        njPushMatrixEx();
        njTranslate(NULL, -SIDE_BAND_X, side_y, side_z);
        njRotateY(NULL, QUARTER_TURN);
        njSetTexture(&quarters_band_texlist);
        model_draw_tinted(&quarters_side_band, color);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & EAST_BAND_BLOCKS) {
        njPushMatrixEx();
        njTranslate(NULL, SIDE_BAND_X, side_y, side_z);
        njRotateY(NULL, HALF_TURN);
        njSetTexture(&quarters_band_texlist);
        model_draw_tinted(&quarters_side_band, color);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] & QUARTERS_EAST) {
        DRAW_PILLAR(0, quarters_pillar_0_texlist, quarters_pillar_0_model);
        DRAW_PILLAR(1, quarters_pillar_1_texlist, quarters_pillar_1_model);
        DRAW_PILLAR(2, quarters_pillar_2_texlist, quarters_pillar_2_model);
        DRAW_PILLAR(3, quarters_pillar_3_texlist, quarters_pillar_3_model);
    }
    /* @hack redundant: a read of specks_task that nothing uses */
    (void)specks_task;
    surface = quarters_water_level();
    if (surface > camera_views[camera_current_index]->pos.y ||
        quarters_view_dips_below(surface)) {
        fog_set(&underwater_fog);
        if (specks_task != NULL) {
            specks_task->work->mode = MODE_SPECKS_SHOWN;
        }
    } else {
        if (guard_radar_on) {
            fog_set(&guard_radar_fog);
        } else {
            fog_set(&clear_fog);
        }
        if (specks_task != NULL) {
            specks_task->work->mode = MODE_SPECKS_HIDDEN;
        }
        water_draw();
    }
    /* @hack spelling: the modifiers' tests read blocks, a copy of the mask,
     * where the mask itself would do */
    blocks = game_state.land_block_mask[0];
    if (blocks & QUARTERS_SOUTH) {
        draw_modifiers(&quarters_south_modifiers);
    } else if (blocks & QUARTERS_EAST) {
        draw_modifiers(&quarters_east_modifiers);
    } else if (blocks & QUARTERS_WEST) {
        draw_modifiers(&quarters_west_modifiers);
    }
}

static void land_clip_step(struct task *task)
{
    Sint32 *mask;
    NJS_POINT3 pos;

    pos = camera_views[camera_current_index]->pos;
    /* @hack redundant: the mask cleared before its replacement */
    game_state.land_block_mask[0] = 0;
    /* @hack spelling: the mask written through mask, where an assignment to
     * the field would do */
    mask = &game_state.land_block_mask[0];
    *mask = quarters_area_at(&pos);
}

static void water_draw(void)
{
    if (game_state.land_block_mask[0] & (QUARTERS_CENTER | QUARTERS_SOUTH)) {
        Sint16 *list = quarters_water_model->plist + WATER_TID;
        int tid;

        /* @hack spelling: tid, a local for the halfword's new value, built in
         * three statements, where one assignment to the halfword would do */
        tid = *list;
        tid &= NJD_FFM_MASK;
        tid |= game_state.steps % WATER_FRAMES;
        *list = tid;
        njSetTexture(&quarters_water_texlist);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, WATER_Z);
        njCnkEasyDrawModel(quarters_water_model);
        njPopMatrixEx();
    }
}
