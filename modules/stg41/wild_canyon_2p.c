/* Wild Canyon's two-player stage, stg41: the stage's init, exit and step,
 * and its three tasks. BgClip picks the land blocks each camera draws. BgExec
 * plays the ambient sounds. CalcLightForPlayer lights each player in the
 * canyon's low half */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/common_object.h"
#include "shared/dirt.h"
#include "shared/dust.h"
#include "shared/emerald.h"
#include "shared/flash.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/ring.h"
#include "shared/skull.h"
#include "shared/switch.h"
#include "sound/adx.h"
#include "shared/wild_canyon_sounds.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "stg41/wild_canyon_2p.h"
#include "stg41/object_archives.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/texture.h"

/* land_clip_step keeps each player's pos and whether it is low for its next
 * step. player_light_step keeps each player's height for its next step */
static NJS_POINT3 land_clip_last_pos[2];
static Bool land_clip_low[2];
static Float light_last_heights[2];

#define BACK_COLOR 0x3232aa
/* task_create's levels for BgClip and for the stage's other two tasks */
#define CLIP_LEVEL 0
#define TASK_LEVEL 1
/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 2048
#define EMERALD_PATHS 12
/* the index in lights of the light player 0 takes below HALF_LINE. player 1
 * takes the next */
#define PLAYER_LIGHT 10

/* the canyon's axis, the vertical line at x AXIS_X and z AXIS_Z */
#define AXIS_X 200.0F
#define AXIS_Z 0.0F

/* the ambient sounds of the stage's sound list. FOLLOW_SOUND plays on the
 * axis, at the height of the player nearer to the axis. where the two are as
 * near, it takes player 1's. the others play by player 0's half of the
 * canyon. in the upper half, UPPER_SOUND plays on the axis at
 * UPPER_SOUND_HEIGHT. in the lower, LOWER_SOUND and FLOOR_SOUND play there at
 * their own SOUND_HEIGHTs. FADING_SOUND and the half's bed sound play without
 * a place. FADING_SOUND's volume goes by the current camera's distance from
 * the axis at the half's FADE_HEIGHT */
#define FOLLOW_SOUND 0
#define UPPER_SOUND 1
#define LOWER_SOUND 2
#define FLOOR_SOUND 3
#define FADING_SOUND 4
#define UPPER_BED_SOUND 9
#define LOWER_BED_SOUND 10
#define UPPER_FADE_HEIGHT 675.0F
#define UPPER_SOUND_HEIGHT 650.0F
#define LOWER_FADE_HEIGHT 75.0F
#define LOWER_SOUND_HEIGHT 50.0F
#define FLOOR_SOUND_HEIGHT 0.0F
#define AMBIENCE_PRIORITY 1
/* the volume a sound without a place starts at, before sound_set_volume gives
 * it its own */
#define START_VOLUME 0
/* each step plays the sounds for AMBIENCE_DURATION + 1 sound updates */
#define AMBIENCE_DURATION 10
/* the sounds' keys, as byte offsets from the task's address. FADING_SOUND
 * takes a key of its own in each half of the canyon. the bed sounds share
 * one */
#define UPPER_FADING_KEY 0
#define UPPER_KEY 1
#define LOWER_FADING_KEY 2
#define LOWER_KEY 3
#define FLOOR_KEY 4
#define FOLLOW_KEY 5
#define BED_KEY 6

/* the land blocks a camera draws, as masks for land_block_mask, by its
 * player's x. LOW_SPLIT_X splits the low blocks, and HIGH_WEST_X and
 * HIGH_EAST_X the high ones. west is toward -x */
#define LOW_SPLIT_X 580.0F
#define HIGH_WEST_X (-275.0F)
#define HIGH_EAST_X 900.0F
#define LOW_WEST_BLOCKS 0x1e
#define LOW_EAST_BLOCKS 0x100000
#define HIGH_WEST_BLOCKS 0xc00
#define HIGH_BLOCKS 0x1400
#define HIGH_EAST_BLOCKS 0x1c00

static void ambience_step(struct task *task);
static void ambience_draw(struct task *task);
static void land_clip_step(struct task *task);
static void player_light_step(struct task *task);

#include "shared/wild_canyon_emerald_routes.h"

/* the hints a search box gives for each place. each is the index of a
 * message in the hint file */
static struct hunt_hint wild_canyon_2p_hints[149] = {
#include "stg41/wild_canyon_2p/wild_canyon_2p_hints.inc"
};
/* a table of hints that no code reads */
static struct hunt_hint dat_8c5470dc[4] = {
    {EMERALD_KIND_A_PATHMOVE, 1, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
    {HUNT_HINT_END, 0, {0, 0, 0}},
    {EMERALD_KIND_B_NORMAL, 0, {0, 0, 0}},
};
static Sint32 *wild_canyon_2p_hint_file = NULL;
static struct path *wild_canyon_2p_paths[] = {NULL};
static struct path *wild_canyon_2p_emerald_paths[EMERALD_PATHS + 1] = {
#include "stg41/wild_canyon_2p/wild_canyon_2p_emerald_paths.inc"
};
/* two shapes that no code reads */
/* clang-format off */
static struct collision_shape dat_8c547138 = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000,
    {200.0F, 555.0F, 280.0F}, 400.0F, 5.0F, 200.0F, 0, {0, 0, 0}};
static struct collision_shape dat_8c547168 = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000,
    {200.0F, 555.0F, -280.0F}, 400.0F, 5.0F, 200.0F, 0, {0, 0, 0}};
/* clang-format on */
#include "shared/wild_canyon_kill_plane.h"

static struct kill_area wild_canyon_2p_kill_areas[2] = {
#include "stg41/wild_canyon_2p/wild_canyon_2p_kill_areas.inc"
};

static struct light stage_lights[2] = {
    {{0.2F, -1.0F, 0.3F}, 1.0F, 0.3F, 1.0F, 1.0F, 1.0F},
    {{0.2F, -1.0F, 0.3F}, 0.5F, 0.3F, 1.0F, 1.0F, 1.0F},
};
/* the set file's block, or null */
static struct set_head *wild_canyon_2p_set = NULL;
static Sint32 dat_8c5472e0 = 5;
static Sint32 dat_8c5472e4 = 2;
/* the ambient sounds' volumes. FADING_SOUND takes the near volume of player
 * 0's half within fade_near. from fade_far on, it takes the half's far
 * volume. between them, its volume falls evenly */
static Sint32 upper_volume = 70;
static Sint32 lower_volume = 70;
static Sint32 upper_near_volume = 125;
static Sint32 upper_far_volume = 0;
static Sint32 lower_near_volume = 125;
static Sint32 lower_far_volume = 0;
static Float fade_far = 400.0F;
static Float fade_near = 75.0F;
static Sint32 floor_volume = 75;
static Sint32 follow_volume = 70;
static Sint32 upper_bed_volume = 70;
static Sint32 lower_bed_volume = 70;
static Float high_height = 600.0F;
static Float low_height = 570.0F;
/* each player's light below HALF_LINE. its color, ambient and intensity
 * take the near values at the axis. they blend evenly into the far values,
 * which they take from light_radius on */
static Float near_r = 0.85F;
static Float near_g = 0.85F;
static Float near_b = 1.0F;
static Float near_ambient = 0.1F;
static Float near_intensity = 1.0F;
static Float far_r = 0.4F;
static Float far_g = 0.4F;
static Float far_b = 0.5F;
static Float far_ambient = 0.45F;
static Float far_intensity = 1.0F;
static Float light_radius = 300.0F;
static struct light player_lights[2] = {
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.437F, 139.0F, 97.0F, 98.0F},
    {{0.3F, 0.1F, 1.0F}, 1.0F, 0.437F, 98.0F, 97.0F, 139.0F},
};

void wild_canyon_2p_init(void)
{
    struct task *task;

    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    task_create(TASK_WORK, CLIP_LEVEL, land_clip_step, "BgClip");
    world_land_start(&wild_canyon_2p_land);
    texture_load_shared(wild_canyon_2p_pvms, wild_canyon_2p_texture_groups);
    texture_await_shared(wild_canyon_2p_pvms, wild_canyon_2p_texture_groups);
    wild_canyon_2p_set = set_read_stage(NULL, SET_ENTRIES);
    set_start(&wild_canyon_2p_objects, wild_canyon_2p_set);
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
    task_create(TASK_WORK, TASK_LEVEL, player_light_step, "CalcLightForPlayer");
    lights_load("stg41_light.bin");
    object_builds_run((Uint8 *)world_build_land(
        &wild_canyon_2p_land, (Uint32)STAGE_MODEL_BUILD_AREA));
    music_fade_in(0);
    music_play_or_defer("w_canyon.adx");
    path_create_manager(wild_canyon_2p_paths);
    task = task_create(TASK_WORK, TASK_LEVEL, ambience_step, "BgExec");
    if (task != NULL) {
        task->draw = ambience_draw;
    }
    kill_areas_start(wild_canyon_2p_kill_areas);
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
    njFogDisable();
    object_light_create();
    hunt_set_hints(wild_canyon_2p_hints);
    wild_canyon_2p_hint_file = hunt_load_hint_file("eh0041");
    emerald_set_paths(wild_canyon_2p_emerald_paths, EMERALD_PATHS);
    messenger_messages_load("mh0041");
    hint_box_hints_load("bh0041");
    smoke_task_create(0, NULL);
    if (game_state.two_players != 0) {
        smoke_task_create(1, NULL);
    }
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    switch_clear_all();
    sound_load_stage("se_ac_wc.mlt", &wild_canyon_sound_list);
    game_state.block_grid_loaded = TRUE;
}

void wild_canyon_2p_exit(void)
{
    hint_box_hints_free();
    messenger_messages_free();
    emerald_clear_paths();
    syFree(wild_canyon_2p_hint_file);
    wild_canyon_2p_hint_file = NULL;
    hunt_clear_hints();
    object_light_destroy();
    fog_stop();
    njFogDisable();
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
    texture_release_shared(wild_canyon_2p_pvms, wild_canyon_2p_texture_groups);
    set_end();
    if (wild_canyon_2p_set != NULL) {
        loader_free(wild_canyon_2p_set);
        wild_canyon_2p_set = NULL;
    }
    world_land_stop();
}

void wild_canyon_2p_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    /* @hack redundant: a switch on the work's mode with one empty case */
    switch (work->mode) {
    case 0:
        break;
    }
}

static void ambience_step(struct task *task)
{
    NJS_POINT3 pos;
    /* @hack spelling: player, a local for player 1's work, where the work
     * read in place would do */
    struct task_work *player;
    Float dx;
    Float dz;
    /* @hack spelling: off_1, a local for player 1's squared distance from
     * the axis, where its sum in the test would do */
    Float off_1;
    Float off_0; /* player 0's squared distance from the axis */
    Float distance;
    int volume;

    pos.x = AXIS_X;
    pos.z = AXIS_Z;
    dx = player_work[0]->pos.x - pos.x;
    dz = player_work[0]->pos.z - pos.z;
    off_0 = (dx * dx) + (dz * dz);
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    player = player_work[1];
#else
    {
        /* @hack type: player 1's work read through dat_8c25127c, where
         * player_work[1] would do */
        extern struct task_work *dat_8c25127c[];

        player = dat_8c25127c[0];
    }
#endif
    dx = player->pos.x - pos.x;
    dz = player->pos.z - pos.z;
    /* @hack redundant: a block no path reaches, which reads pos.x three
     * times, fade_far and off_0 */
    if (0) {
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        if (pos.x && pos.x && pos.x && fade_far && off_0) {
        }
    }
    off_1 = (dx * dx) + (dz * dz);
    if (off_0 < off_1) {
        pos.y = player_work[0]->pos.y;
    } else {
        pos.y = player_work[1]->pos.y;
    }
    sound_play_at_for(FOLLOW_SOUND, (char *)task + FOLLOW_KEY,
                      AMBIENCE_PRIORITY, follow_volume, AMBIENCE_DURATION,
                      &pos);
    if (player_work[0]->pos.y > HALF_LINE) {
        pos.y = UPPER_FADE_HEIGHT;
        distance =
            njDistanceP2P(&camera_views[camera_current_index]->pos, &pos);
        if (distance <= fade_near) {
            volume = upper_near_volume;
        } else if (distance >= fade_far) {
            volume = upper_far_volume;
        } else {
            volume = ((fade_far - distance) / (fade_far - fade_near) *
                      (upper_near_volume - upper_far_volume)) +
                     upper_far_volume;
        }
        sound_play_for(FADING_SOUND, (char *)task + UPPER_FADING_KEY,
                       AMBIENCE_PRIORITY, START_VOLUME, AMBIENCE_DURATION);
        sound_set_volume((char *)task + UPPER_FADING_KEY, SOUND_ANY_ID, volume);
        pos.y = UPPER_SOUND_HEIGHT;
        sound_play_at_for(UPPER_SOUND, (char *)task + UPPER_KEY,
                          AMBIENCE_PRIORITY, upper_volume, AMBIENCE_DURATION,
                          &pos);
        sound_play_for(UPPER_BED_SOUND, (char *)task + BED_KEY,
                       AMBIENCE_PRIORITY, START_VOLUME, AMBIENCE_DURATION);
        sound_set_volume((char *)task + BED_KEY, SOUND_ANY_ID,
                         upper_bed_volume);
    } else {
        pos.y = LOWER_FADE_HEIGHT;
        distance =
            njDistanceP2P(&camera_views[camera_current_index]->pos, &pos);
        if (distance <= fade_near) {
            volume = lower_near_volume;
        } else if (distance >= fade_far) {
            volume = lower_far_volume;
        } else {
            volume = ((fade_far - distance) / (fade_far - fade_near) *
                      (lower_near_volume - lower_far_volume)) +
                     lower_far_volume;
        }
        /* @hack redundant: a block no path reaches, which reads volume */
        if (0) {
            if (volume) {
            }
        }
        sound_play_for(FADING_SOUND, (char *)task + LOWER_FADING_KEY,
                       AMBIENCE_PRIORITY, START_VOLUME, AMBIENCE_DURATION);
        sound_set_volume((char *)task + LOWER_FADING_KEY, SOUND_ANY_ID, volume);
        pos.y = LOWER_SOUND_HEIGHT;
        sound_play_at_for(LOWER_SOUND, (char *)task + LOWER_KEY,
                          AMBIENCE_PRIORITY, lower_volume, AMBIENCE_DURATION,
                          &pos);
        pos.y = FLOOR_SOUND_HEIGHT;
        sound_play_at_for(FLOOR_SOUND, (char *)task + FLOOR_KEY,
                          AMBIENCE_PRIORITY, floor_volume, AMBIENCE_DURATION,
                          &pos);
        sound_play_for(LOWER_BED_SOUND, (char *)task + BED_KEY,
                       AMBIENCE_PRIORITY, START_VOLUME, AMBIENCE_DURATION);
        sound_set_volume((char *)task + BED_KEY, SOUND_ANY_ID,
                         lower_bed_volume);
    }
}

static void ambience_draw(struct task *task)
{
}

/* set each camera's land_block_mask by its player's x and half of the
 * canyon. a player at or below low_height is low, and one at or above
 * high_height high. a player that comes into the band between them from
 * below turns high at once. one that comes from above turns low. so a camera
 * takes the blocks of the half its player heads for. any other player keeps
 * its half */
static void land_clip_step(struct task *task)
{
    int i;
    Sint32 *mask;
    struct task_work *work;

    /* @hack redundant: a block no path reaches */
    if (0) {
        if (task) {
        }
    }
    /* @hack type: the masks cleared through dat_8c0db20c, where
     * game_state.land_block_mask would do */
    STAGE_LAND_MASK(0) = 0;
    STAGE_LAND_MASK(1) = 0;
    for (i = 0; i <= 1; i++) {
        work = player_work[i];
        if (work == NULL) {
            continue;
        }
        /* @hack type: the mask's address picked by a test of i from
         * dat_8c0db20c, where &game_state.land_block_mask[i] would do */
        if (i == 0) {
            mask = &STAGE_LAND_MASK(0);
        } else {
            mask = &STAGE_LAND_MASK(1);
        }
        if (work->pos.y <= low_height) {
            land_clip_low[i] = TRUE;
        } else if (work->pos.y >= high_height) {
            land_clip_low[i] = FALSE;
        } else if (land_clip_last_pos[i].y < low_height) {
            land_clip_low[i] = FALSE;
        } else if (land_clip_last_pos[i].y > high_height) {
            land_clip_low[i] = TRUE;
        }
        if (land_clip_low[i]) {
            if (work->pos.x < LOW_SPLIT_X) {
                *mask = LOW_WEST_BLOCKS;
            } else {
                *mask = LOW_EAST_BLOCKS;
            }
        } else {
            if (work->pos.x < HIGH_WEST_X) {
                *mask = HIGH_WEST_BLOCKS;
            } else if (work->pos.x < HIGH_EAST_X) {
                *mask = HIGH_BLOCKS;
            } else {
                *mask = HIGH_EAST_BLOCKS;
            }
        }
        land_clip_last_pos[i] = work->pos;
    }
}

/* below HALF_LINE, light each player with a light of its own, PLAYER_LIGHT
 * plus the player's number. the light shines on the player from the axis's
 * point at HALF_LINE. it blends by the player's distance from the axis. back
 * at HALF_LINE or above, player 0 takes light 0 again. player 1 takes light
 * 1, where the init gave it light 0. player 1 counts only in a two-player
 * game */
static void player_light_step(struct task *task)
{
    int i;
    Float dx;
    Float dz;
    Float near;
    NJS_VECTOR dir;

    for (i = 0; i <= 1; i++) {
        if (i == 1 && game_state.two_players == 0) {
            continue;
        }
        if (player_work[i]->pos.y < HALF_LINE) {
            dx = AXIS_X - player_work[i]->pos.x;
            dz = AXIS_Z - player_work[i]->pos.z;
            near =
                (light_radius - njSqrt((dx * dx) + (dz * dz))) / light_radius;
            if (near < 0.0F) {
                near = 0.0F;
            }
            /* @bug logic: each player's light lags the player by a step, and
             * its first step below HALF_LINE is a hundred times too bright or
             * more. light_set copies player_lights[i] into the player's
             * light at 0x8c500970, before the blend's stores from 0x8c5009b6
             * on. so each step takes the blend of the step before. the first
             * takes player_lights' starting color, whose r, g and b run from
             * 97 to 139 where a blend's run from 0.4 to 1 */
            /* @hack spelling: the light's index picked by a test of i, where
             * PLAYER_LIGHT + i would do */
            if (i == 0) {
                light_set(PLAYER_LIGHT, &player_lights[i]);
            } else {
                light_set(PLAYER_LIGHT + 1, &player_lights[i]);
            }
            dir.x = player_work[i]->pos.x - AXIS_X;
            dir.y = player_work[i]->pos.y - HALF_LINE;
            dir.z = player_work[i]->pos.z - AXIS_Z;
            njUnitVector(&dir);
            player_lights[i].dir.x = dir.x;
            player_lights[i].dir.y = dir.y;
            player_lights[i].dir.z = dir.z;
            player_lights[i].r = (near_r * near) + (far_r * (1.0F - near));
            player_lights[i].g = (near_g * near) + (far_g * (1.0F - near));
            player_lights[i].b = (near_b * near) + (far_b * (1.0F - near));
            player_lights[i].ambient =
                (near_ambient * near) + (far_ambient * (1.0F - near));
            player_lights[i].intensity =
                (near_intensity * near) + (far_intensity * (1.0F - near));
            /* @hack spelling: the light's index picked by a test of i, where
             * PLAYER_LIGHT + i would do */
            if (i == 0) {
                light_select(PLAYER_LIGHT, i);
            } else {
                light_select(PLAYER_LIGHT + 1, i);
            }
        } else if (light_last_heights[i] < HALF_LINE) {
            /* @hack spelling: the call picked by a test of i, where
             * light_select(i, i) would do */
            if (i == 0) {
                light_select(0, 0);
            } else {
                light_select(1, 1);
            }
        }
        light_last_heights[i] = player_work[i]->pos.y;
    }
}
