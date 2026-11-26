/* the second fight of Sonic and Shadow, stg42: the stage's init, exit and
 * step, and its background. here too is ControlShadow, the task that steers
 * player 1, the rival, against player 0. the fight runs along x without
 * end */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/bossinit.h"
#include "camera/camera.h"
#include "camera/ss.h"
#include "camera/stack.h"
#include "collision/world.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "effect/sonic_effect.h"
#include "effect/texlist.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "player/sonic.h"
#include "shared/big_the_cat.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/cage.h"
#include "shared/collision_object.h"
#include "shared/common_object.h"
#include "shared/effect_object.h"
#include "shared/flame.h"
#include "shared/fog.h"
#include "shared/hint_box.h"
#include "shared/iron_box.h"
#include "shared/item_box.h"
#include "shared/item_box_air.h"
#include "shared/item_icon.h"
#include "shared/light_switch.h"
#include "shared/messenger.h"
#include "shared/modifier_volume.h"
#include "shared/object_builds.h"
#include "shared/object_light.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "shared/ring_sparkle.h"
#include "shared/search_box.h"
#include "shared/shadow_fight.h"
#include "shared/skull.h"
#include "shared/stop.h"
#include "shared/white_breath.h"
#include "shared/wood_box.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "shared/stage_texture_groups.h"
#include "stg42/fireball.h"
#include "stg42/foothold.h"
#include "stg42/placement_wrap.h"
#include "stg42/versus_sonic_2.h"
#include "stg42/versus_sonic_2_sounds.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "stg42/versus_sonic_2/text.inc"

/* the names the background's texture lists point to. each but the last is
 * padded with NULs to a multiple of 4 bytes */
struct arena_texture_names {
    char earth512_2[12];
    char et64_3[8];
    char space128_17[12];
    char space128_2[12];
    char space128_3[12];
    char space128_4[12];
    char space128_7[12];
    char space128_9[12];
    char space256_1[12];
    char space256_7[12];
    char space32_4[12];
    char space32_7[12];
    char space512_1[12];
    char space512_2[12];
    char space64_18[12];
    char space64_19[12];
    char space64_20[12];
    char space64_9[12];
    char spacenbg128[12];
    char spacenbg64[12];
    char y_co8_11[9];
};
static const struct arena_texture_names arena_texture_names;
#include "stg42/arena_art.h"

static struct fog_setting versus_sonic_2_fog;

/* the set file's block holds at least SET_ENTRIES entries, its head counted
 * as one */
#define SET_ENTRIES 512
#define BACKGROUND_LEVEL 1
#define BACK_COLOR 0xff001020
/* Ninja's near and far clip while the stage runs, and the far clip
 * versus_sonic_2_exit sets */
#define CLIP_NEAR (-3.0F)
#define CLIP_FAR (-20000.0F)
#define EXIT_CLIP_FAR (-3000.0F)
/* the stage's step makes player 0 fall out below this y */
#define FALL_DEPTH (-100.0F)

/* the draw lays FLOOR_TILES tiles of the floor along x. the first lies
 * FLOOR_TILES_BEHIND tiles before the one under the current camera */
#define FLOOR_TILES 6
#define FLOOR_TILES_BEHIND 3.0F
/* the turn about x that the turning models add to their spin */
#define TURNING_TILT 0x3000

/* ControlShadow's record in the second fight. rival_move_chase moves chase,
 * which the intro's last camera looks toward */
struct shadow_chase {
    struct shadow_control control;
    NJS_POINT3 chase;
};

enum rival_shadow_mode {
    /* wait for STAGE_PLAY, then start the intro */
    MODE_WAIT,
    /* the intro, for wait steps. its end summons the messenger after a
     * retry, as shadow_fight.h says, and starts MODE_RACE */
    MODE_INTRO,
    /* steer at player 0 while behind it, and otherwise run along x */
    MODE_CHASE,
    /* jump, and hold the jump until player 1 lands or tops out */
    MODE_JUMP,
    MODE_JUMP_TOP, /* press jump again at the top of the jump */
    /* race along x for wait steps, keeping pace with player 0 */
    MODE_RACE,
    /* jump along x, at player 0's attack or over a gap */
    MODE_JUMP_AT,
    MODE_ATTACK, /* wait to land */
    MODE_SPECIAL, /* wait out a press of the action button, for wait steps */
    /* player 1 fell, or fell behind: it comes back ahead of player 0 */
    MODE_FALLEN,
    /* hold a ring level, and steer at player 0 while player 0's last_flags
     * show it off the ground */
    MODE_RING_LEVEL,
    /* set in STAGE_FADE_IN, until the same step's end sets MODE_WAIT */
    MODE_START_RUN,
    /* MODE_CHASE, after a jump that landed before its top */
    MODE_CHASE_AGAIN,
    /* a player has missed: no mode steers player 1 */
    MODE_STOPPED
};

/* ControlShadow's task level */
#define CONTROL_LEVEL 0
/* player 1's gauge at the fight's start, which each fade in restores */
#define RIVAL_GAUGE 5.0F
/* player 1's voices, by its kind. it speaks at INTRO_LINE_STEP of the intro,
 * as it falls behind and as it takes a ring level. a Sonic speaks again at
 * FALLEN_LINE_STEP of MODE_FALLEN */
#define SONIC_INTRO_VOICE 2247
#define SHADOW_INTRO_VOICE 2224
#define SONIC_FALL_VOICE 2254
#define SHADOW_FALL_VOICE 2230
#define SONIC_LEVEL_VOICE 2250
#define SHADOW_LEVEL_VOICE 2216
#define SONIC_FALLEN_VOICE 2255
#define FALLEN_LINE_STEP 60
#define VOICE_PRIORITY 3
#define INTRO_LINE_STEP 120
/* a line that is no message or voice */
#define NO_LINE (-1)

/* the intro's BOSSINIT camera looks at a point INTRO_LOOK_X past player 1
 * along x, at player 1's z. the point's y is INTRO_LOOK_HEIGHT above the
 * arena's center. a player 1 whose character is Sonic takes SONIC_INTRO_ANG
 * and SONIC_INTRO_TURN, and any other INTRO_ANG and INTRO_TURN */
#define INTRO_SPEED 1.0F
#define SONIC_INTRO_ANG 0xf000
#define SONIC_INTRO_TURN QUARTER_TURN
#define INTRO_ANG QUARTER_TURN
#define INTRO_TURN HALF_TURN
#define INTRO_START_DIST 180.0F
#define INTRO_DIST 17.0F
#define INTRO_LOOK_HEIGHT 7.0F
#define INTRO_LOOK_X 12.0F
/* through the intro, each player looks at a point INTRO_EYE_HEIGHT above the
 * other. each step renews the look for LOOK_STEPS */
#define INTRO_EYE_HEIGHT 8.0F
#define LOOK_STEPS 3
/* the intro's last INTRO_SS_STEPS steps start an SS camera. it looks
 * INTRO_SS_PART of the way from player 0 to the chase point */
#define INTRO_SS_STEPS 20
#define INTRO_SS_PART 0.5F
/* the index of camera 0's stack that the intro's mode is taken to hold */
#define INTRO_CAMERA_INDEX 13

/* player 1 falls behind, and MODE_FALLEN starts, below FALL_BEHIND_DEPTH or
 * more than FALL_BEHIND_X behind player 0 along x */
#define FALL_BEHIND_DEPTH (-50.0F)
#define FALL_BEHIND_X 150.0F
/* MODE_FALLEN keeps player 1 FALLEN_AHEAD past player 0, down at
 * FALLEN_DEPTH, until FALLEN_RETURN_STEP. a flash then shows where it comes
 * back, and at FALLEN_STAND_STEP it stands there */
#define FALLEN_RETURN_STEP 300
#define FALLEN_STAND_STEP 330
#define FALLEN_AHEAD 450.0F
#define FALLEN_DEPTH (-800.0F)
#define FALLEN_FLASH_HEIGHT 15.0F
#define FALLEN_STAND_HEIGHT 20.0F
#define FLASH_SOUND 8

/* the speed both players take forward through STAGE_FADE_IN, MODE_WAIT and
 * MODE_INTRO, with both sticks pushed along x */
#define START_RUN_SPEED 6.0F

/* a chase steers player 1 at player 0 from more than CHASE_BEHIND behind
 * along x. otherwise it steers player 1 along x. while both players run
 * past their sprint_speed, a step multiplies player 1's speed. from behind,
 * the speed takes CHASE_BOOST. from more than CHASE_AHEAD ahead, it takes
 * HOMING_RATE in player 1's own homing attack. otherwise it takes the rate of
 * player 1's gauge. a chase turns to a race after CHASE_STEPS */
#define CHASE_BEHIND 10.0F
#define CHASE_AHEAD 60.0F
#define CHASE_BOOST 1.005F
#define HOMING_RATE 1.006F
#define CHASE_STEPS 60
/* a race runs player 1 along x. while both players run past their
 * sprint_speed, a step multiplies player 1's speed. less than RACE_LEAD ahead
 * of player 0, the speed takes RACE_BOOST. further ahead, it takes the rate of
 * player 1's gauge. RACE_AHEAD changes nothing: where its test fails, a sprint
 * test has failed too */
#define RACE_LEAD 45.0F
#define RACE_AHEAD 25.0F
#define RACE_BOOST 1.007F
/* the rates by player 1's gauge: RATE_LOW below GAUGE_LOW, RATE_MID below
 * GAUGE_MID, RATE_HIGH below GAUGE_HIGH, and RATE_FULL from there up */
#define GAUGE_LOW 2.0F
#define GAUGE_MID 3.0F
#define GAUGE_HIGH 4.0F
#define RATE_LOW 0.999F
#define RATE_MID 0.985F
#define RATE_HIGH 0.972F
#define RATE_FULL 0.96F
/* a race takes a ring level where player 1 leads by enough. its lead must
 * pass LEVEL_AHEAD, less LEVEL_PER_GAUGE for each unit of its gauge. the
 * level is RING_LEVEL_WIND_CUTTER, and its timer starts at LEVEL_STEPS */
#define LEVEL_AHEAD 340.0F
#define LEVEL_PER_GAUGE 20.0F
#define LEVEL_STEPS 60
/* MODE_RING_LEVEL turns to MODE_SPECIAL once the level's timer runs out or
 * LEVEL_LONGEST_STEPS pass. the turn waits while player 0 has missed, or while
 * kill_areas_task is null or has a child. the mode steers at a stroke of
 * LEVEL_STROKE */
#define LEVEL_LONGEST_STEPS 120
#define LEVEL_STROKE 0.2F
/* a racing player 1 jumps along x over a gap between the generator's
 * footholds. it jumps below GAP_DEPTH. its x less GAP_START must then lie
 * GAP_EDGE or more of the way along a FOOTHOLD_SPACING */
#define GAP_DEPTH (-4.0F)
#define GAP_START 150.0F
#define GAP_EDGE 0.65F
/* the steps a race lasts: its base, and up to its MORE steps more at random.
 * the first race follows the intro, and the others a chase or a jump at
 * player 0. MODE_SPECIAL lasts SPECIAL_STEPS */
#define RACE_FIRST_STEPS 180.0F
#define RACE_FIRST_MORE_STEPS 120.0F
#define RACE_STEPS 60.0F
#define RACE_MORE_STEPS 120.0F
#define RACE_AFTER_JUMP_STEPS 120.0F
#define RACE_AFTER_JUMP_MORE_STEPS 180.0F
#define SPECIAL_STEPS 180
/* a jump tops out where player 1 has no speed in y. player 1 must then lie
 * more than JUMP_HEIGHT above the ground below player 0. MODE_JUMP_TOP presses
 * jump again once player 1, that high, rises at JUMP_TOP_RISE or less. a jump
 * at player 0's attack or over a gap tops out too once player 1 falls. it does
 * as well above y JUMP_AT_HEIGHT while rival_sees_attack holds. no jump counts
 * as landed in its first JUMP_SURE_STEPS steps */
#define JUMP_HEIGHT 30.0F
#define JUMP_TOP_RISE 0.3F
#define JUMP_AT_HEIGHT 25.0F
#define JUMP_SURE_STEPS 2
/* player 1 jumps at player 0's attack in a chase while its gauge is no more
 * than CHASE_ATTACK_GAUGE. in a race the gauge must be no more than
 * RACE_ATTACK_GAUGE */
#define CHASE_ATTACK_GAUGE 3.0F
#define RACE_ATTACK_GAUGE 2.0F
#define HOMING_GAUGE 2.0F
#define ATTACK_REACH 19.0F
/* while the players lie more than CHASE_APART apart, the chase point heads
 * for CHASE_LEAD past player 0's x. it heads for y 0 and z 0 too. it moves by
 * CHASE_STEP_X, CHASE_STEP_Y and CHASE_STEP_Z a step. otherwise it is player
 * 1's pos */
#define CHASE_APART 80.0F
#define CHASE_LEAD 80.0F
#define CHASE_STEP_X 8.0F
#define CHASE_STEP_Y 3.0F
#define CHASE_STEP_Z 2.0F

/* data that calls above its definitions use. SHC places each datum where it
 * is defined, so each definition stays where the image's order puts it */
static NJS_TEXLIST **background_texture_groups[2];
static struct texture_pvm background_pvms[2];
static struct kill_area versus_sonic_2_kill_areas[1];
static struct set_head *versus_sonic_2_set;
static struct texture_pvm object_pvms[5];
static NJS_TEXLIST **object_texture_groups[15];
static NJS_TEXLIST title_sonic_texlist;
static NJS_TEXLIST title_shadow_texlist;
static NJS_POINT3 arena_center[1];
static Float arena_half_width;
static struct boss_title title_sonic;
static struct boss_title title_shadow;

static void background_step(struct task *task);
static void background_draw(struct task *task);
static void rival_shadow_create(void);

void versus_sonic_2_init(void)
{
    struct task *task;
    Uint8 *buf;

    if (stage_character(0) == PLAYER_SONIC) {
        shadow_create(1);
    } else if (stage_character(0) == PLAYER_SHADOW) {
        sonic_create(1);
    }
    if (player_tasks[1]) {
        stage_place_player(1);
        player_work[1]->id = WORK_ID_RIVAL;
        pad_disable_player(1);
        rival_shadow_create();
    }
    texture_load_shared(object_pvms, object_texture_groups);
    texture_await_shared(object_pvms, object_texture_groups);
    texture_load_shared(background_pvms, background_texture_groups);
    texture_await_shared(background_pvms, background_texture_groups);
    versus_sonic_2_set =
        set_read("set0042_s.bin", "set0042_u.bin", SET_ENTRIES);
    set_start(&versus_sonic_2_objects, versus_sonic_2_set);
    loader_read_camera_files();
    particle_core_start();
    task = task_create(TASK_WORK, BACKGROUND_LEVEL, background_step, "BgExec");
    if (task) {
        task->draw = background_draw;
    }
    lights_load("stg42_light.bin");
    fog_start_file("stg42_fog.bin", &versus_sonic_2_fog);
    njFogEnable();
    messenger_messages_load("mh0042");
    njSetBackColor(BACK_COLOR, BACK_COLOR, BACK_COLOR);
    /* @hack spelling: buf, a local for the build area, where the area in
     * place would do */
    buf = STAGE_MODEL_BUILD_AREA;
    njSetDirectClip();
    object_builds_run(buf);
    kill_areas_start(versus_sonic_2_kill_areas);
    if (!player_hooks.drop_rings) {
        player_hooks.drop_rings = ring_scatter;
    }
    if (!player_hooks.add_item_icon) {
        player_hooks.add_item_icon = item_icon_row_add;
    }
    stage_hooks.open_message = message_window_open_unowned;
    stage_hooks.trail_alloc = trail_alloc;
    stage_hooks.trail_draw = trail_draw;
    stage_hooks.trail_free = trail_free;
    stage_hooks.action_icon = NULL;
    stage_hooks.number_cell = NULL;
    stage_hooks.chao_area = NULL;
    stage_hooks.save = NULL;
    sound_load_stage("se_vs_shadow.mlt", &versus_sonic_2_sound_list);
    music_fade_in(0);
    music_play_or_defer("boss_05.ADX");
    njClipZ(CLIP_NEAR, CLIP_FAR);
}

static const struct arena_texture_names arena_texture_names = {
#include "stg42/versus_sonic_2/arena_texture_names.inc"
};

void versus_sonic_2_exit(void)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. they are the strings of
     * the stage's tables, as those tables name them, which SHC would place
     * after the code's own */
    if (0) {
        (void)BACKGROUND_ARCHIVE_NAME;
        (void)OBJTEX_COMMON_BOMBFIRE;
        (void)OBJTEX_COMMON_ITEMP_10RING;
        (void)OBJTEX_COMMON_ITEMP_1UP;
        (void)OBJTEX_COMMON_ITEMP_1UP2;
        (void)OBJTEX_COMMON_ITEMP_1UP3;
        (void)OBJTEX_COMMON_ITEMP_1UP4;
        (void)OBJTEX_COMMON_ITEMP_1UP5;
        (void)OBJTEX_COMMON_ITEMP_1UP6;
        (void)OBJTEX_COMMON_ITEMP_20RING;
        (void)OBJTEX_COMMON_ITEMP_5RING;
        (void)OBJTEX_COMMON_ITEMP_BARRIER;
        (void)OBJTEX_COMMON_ITEMP_BOMB;
        (void)OBJTEX_COMMON_ITEMP_LIFE;
        (void)OBJTEX_COMMON_ITEMP_MAGNET;
        (void)OBJTEX_COMMON_ITEMP_RANDOMRING;
        (void)OBJTEX_COMMON_ITEMP_SPEED;
        (void)OBJTEX_COMMON_ITEMP_SUPER;
        (void)OBJTEX_COMMON_KYOTU32_MARUKAGE;
        (void)OBJTEX_COMMON_KYOTU64_MARUKAGE;
        (void)OBJTEX_COMMON_SIKAKE_01_128;
        (void)OBJTEX_COMMON_SIKAKE_02_128;
        (void)OBJTEX_COMMON_SIKAKE_03_64;
        (void)OBJTEX_COMMON_SIKAKE_04_64;
        (void)OBJTEX_COMMON_SIKAKE_05_64;
        (void)OBJTEX_COMMON_SIKAKE_06_64;
        (void)OBJTEX_COMMON_SIKAKE_07_64;
        (void)OBJTEX_COMMON_SIKAKE_08_64;
        (void)OBJTEX_COMMON_SIKAKE_09_32;
        (void)OBJTEX_COMMON_SIKAKE_10_128;
        (void)OBJTEX_COMMON_SIKAKE_11_64;
        (void)OBJTEX_COMMON_SIKAKE_12_64;
        (void)OBJTEX_COMMON_SIKAKE_13_64;
        (void)OBJTEX_COMMON_SIKAKE_14_32;
        (void)OBJTEX_COMMON_SIKAKE_15_64;
        (void)OBJTEX_COMMON_SIKAKE_16_64;
        (void)OBJTEX_COMMON_SIKAKE_17_64;
        (void)OBJTEX_COMMON_SIKAKE_18_128;
        (void)OBJTEX_COMMON_SIKAKE_19_128;
        (void)OBJTEX_COMMON_SIKAKE_20_128;
        (void)OBJTEX_COMMON_SIKAKE_21_128;
        (void)OBJTEX_COMMON_SIKAKE_22_64;
        (void)OBJTEX_COMMON_SIKAKE_23_64;
        (void)OBJTEX_COMMON_SIKAKE_24_64;
        (void)OBJTEX_COMMON_SIKAKE_25_64;
        (void)OBJTEX_COMMON_SIKAKE_26_64;
        (void)OBJTEX_COMMON_SIKAKE_27_64;
        (void)OBJTEX_COMMON_SIKAKE_28_64;
        (void)OBJTEX_COMMON_SIKAKE_29_64;
        (void)OBJTEX_COMMON_SIKAKE_30_64;
        (void)OBJTEX_COMMON_SIKAKE_31_128;
        (void)OBJTEX_COMMON_SIKAKE_32_64;
        (void)OBJTEX_COMMON_SIKAKE_33_128;
        (void)OBJTEX_COMMON_SIKAKE_34_64;
        (void)OBJTEX_COMMON_SIKAKE_35_32;
        (void)OBJTEX_COMMON_SIKAKE_36_32;
        (void)OBJTEX_COMMON_SIKAKE_37_32;
        (void)OBJTEX_COMMON_SIKAKE_38_128;
        (void)OBJTEX_COMMON_SIKAKE_39_128;
        (void)OBJTEX_COMMON_SIKAKE_40_64;
        (void)OBJTEX_COMMON_SIKAKE_41_32;
        (void)OBJTEX_COMMON_SIKAKE_42_32;
        (void)OBJTEX_COMMON_SIKAKE_43_32;
        (void)OBJTEX_COMMON_SIKAKE_44_32;
        (void)OBJTEX_COMMON_SIKAKE_45_64;
        (void)OBJTEX_COMMON_SIKAKE_46_128;
        (void)OBJTEX_COMMON_SIKAKE_47_128;
        (void)OBJTEX_COMMON_SIKAKE_48_64;
        (void)OBJTEX_COMMON_SIKAKE_49_64;
        (void)OBJTEX_COMMON_SIKAKE_50_64;
        (void)OBJTEX_COMMON_SIKAKE_51_64;
        (void)OBJTEX_COMMON_SIKAKE_52_32;
        (void)OBJTEX_COMMON_THHIKARI01;
        (void)OBJTEX_COMMON_THHIKARI02;
        (void)OBJTEX_COMMON_THHIKARI03;
        (void)OBJTEX_COMMON_THHIKARI04;
        (void)OBJTEX_COMMON_THHIKARI05;
        (void)OBJTEX_COMMON_THHIKARI06;
        (void)OBJTEX_COMMON_THHIKARI07;
        (void)OBJTEX_COMMON_THHIKARI08;
        (void)OBJTEX_COMMON_THTSUBU01;
        (void)OBJTEX_COMMON_THTSUBU02;
        (void)OBJTEX_COMMON_THTSUBU03;
        (void)OBJTEX_COMMON_THTSUBU04;
        (void)OBJTEX_COMMON_THTSUBU05;
        (void)OBJTEX_COMMON_THTSUBU06;
        (void)OBJTEX_COMMON_THTSUBU07;
        (void)OBJTEX_COMMON_THTSUBU08;
        (void)EFFTEX_COMMON_BARRIA_N01;
        (void)EFFTEX_COMMON_BARRIA_N02;
        (void)EFFTEX_COMMON_BARRIA_N03;
        (void)EFFTEX_COMMON_BARRIA_N04;
        (void)EFFTEX_COMMON_BARRIA_N05;
        (void)EFFTEX_COMMON_BARRIA_N06;
        (void)EFFTEX_COMMON_BARRIA_N07;
        (void)EFFTEX_COMMON_BARRIA_N08;
        (void)EFFTEX_COMMON_BARRIA_T01;
        (void)EFFTEX_COMMON_BARRIA_T02;
        (void)EFFTEX_COMMON_BARRIA_T03;
        (void)EFFTEX_COMMON_BARRIA_T04;
        (void)EFFTEX_COMMON_BARRIA_T05;
        (void)EFFTEX_COMMON_BARRIA_T06;
        (void)EFFTEX_COMMON_BARRIA_T07;
        (void)EFFTEX_COMMON_BARRIA_T08;
        (void)EFFTEX_COMMON_BARRIA_T09;
        (void)EFFTEX_COMMON_BARRIA_T10;
        (void)EFFTEX_COMMON_BARRIA_T11;
        (void)EFFTEX_COMMON_BOMB01;
        (void)EFFTEX_COMMON_BOMB02;
        (void)EFFTEX_COMMON_BOMB03;
        (void)EFFTEX_COMMON_BOMB04;
        (void)EFFTEX_COMMON_BOMB05;
        (void)EFFTEX_COMMON_BOMB06;
        (void)EFFTEX_COMMON_BOMB07;
        (void)EFFTEX_COMMON_BOMB08;
        (void)EFFTEX_COMMON_BOMB09;
        (void)EFFTEX_COMMON_BOMB10;
        (void)EFFTEX_COMMON_BOMB11;
        (void)EFFTEX_COMMON_BOMB12;
        (void)EFFTEX_COMMON_BOMB13;
        (void)EFFTEX_COMMON_BOMB14;
        (void)EFFTEX_COMMON_BOMB15;
        (void)EFFTEX_COMMON_BOMB16;
        (void)EFFTEX_COMMON_BUBBLE;
        (void)EFFTEX_COMMON_DUST01;
        (void)EFFTEX_COMMON_DUST02;
        (void)EFFTEX_COMMON_DUST03;
        (void)EFFTEX_COMMON_DUST04;
        (void)EFFTEX_COMMON_DUST05;
        (void)EFFTEX_COMMON_DUST06;
        (void)EFFTEX_COMMON_DUST07;
        (void)EFFTEX_COMMON_DUST08;
        (void)EFFTEX_COMMON_EXP01;
        (void)EFFTEX_COMMON_EXP02;
        (void)EFFTEX_COMMON_EXP03;
        (void)EFFTEX_COMMON_EXP04;
        (void)EFFTEX_COMMON_EXP05;
        (void)EFFTEX_COMMON_EXP06;
        (void)EFFTEX_COMMON_EXP07;
        (void)EFFTEX_COMMON_EXP08;
        (void)EFFTEX_COMMON_FLAME01;
        (void)EFFTEX_COMMON_FLAME02;
        (void)EFFTEX_COMMON_FLAME03;
        (void)EFFTEX_COMMON_FLAME04;
        (void)EFFTEX_COMMON_FLAME05;
        (void)EFFTEX_COMMON_FLAME06;
        (void)EFFTEX_COMMON_FLAME07;
        (void)EFFTEX_COMMON_FLAME08;
        (void)EFFTEX_COMMON_FLAME09;
        (void)EFFTEX_COMMON_FLAME10;
        (void)EFFTEX_COMMON_KIRAN01;
        (void)EFFTEX_COMMON_KIRAN02;
        (void)EFFTEX_COMMON_KIRAN03;
        (void)EFFTEX_COMMON_KIRAN04;
        (void)EFFTEX_COMMON_LENS01;
        (void)EFFTEX_COMMON_LENS02;
        (void)EFFTEX_COMMON_LENS03;
        (void)EFFTEX_COMMON_LENS04;
        (void)EFFTEX_COMMON_LENS05;
        (void)EFFTEX_COMMON_LENS06;
        (void)EFFTEX_COMMON_LENS07;
        (void)EFFTEX_COMMON_MUTEKI01;
        (void)EFFTEX_COMMON_MUTEKI02;
        (void)EFFTEX_COMMON_MUTEKI03;
        (void)EFFTEX_COMMON_MUTEKI04;
        (void)EFFTEX_COMMON_MUTEKI05;
        (void)EFFTEX_COMMON_MUTEKI06;
        (void)EFFTEX_COMMON_MUTEKI07;
        (void)EFFTEX_COMMON_MUTEKI08;
        (void)EFFTEX_COMMON_MUTEKIBALL;
        (void)EFFTEX_COMMON_P_EXP;
        (void)EFFTEX_COMMON_PAN_JETB01;
        (void)EFFTEX_COMMON_PAN_JETB02;
        (void)EFFTEX_COMMON_PAN_JETB03;
        (void)EFFTEX_COMMON_PANLIGHT;
        (void)EFFTEX_COMMON_RING01;
        (void)EFFTEX_COMMON_RING02;
        (void)EFFTEX_COMMON_RING03;
        (void)EFFTEX_COMMON_RING04;
        (void)EFFTEX_COMMON_RING05;
        (void)EFFTEX_COMMON_RING06;
        (void)EFFTEX_COMMON_RING07;
        (void)EFFTEX_COMMON_RING08;
        (void)EFFTEX_COMMON_ROKETTHRUST01;
        (void)EFFTEX_COMMON_ROKETTHRUST02;
        (void)EFFTEX_COMMON_ROKETTHRUST03;
        (void)EFFTEX_COMMON_ROKETTHRUST04;
        (void)EFFTEX_COMMON_ROKETTHRUST05;
        (void)EFFTEX_COMMON_ROKETTHRUST06;
        (void)EFFTEX_COMMON_ROKETTHRUST07;
        (void)EFFTEX_COMMON_ROKETTHRUST08;
        (void)EFFTEX_COMMON_SNOWP01;
        (void)EFFTEX_COMMON_SNOWP02;
        (void)EFFTEX_COMMON_SNOWP03;
        (void)EFFTEX_COMMON_SNOWP04;
        (void)EFFTEX_COMMON_SNOWP05;
        (void)EFFTEX_COMMON_SNOWP06;
        (void)EFFTEX_COMMON_SNOWP07;
        (void)EFFTEX_COMMON_SNOWP08;
        (void)EFFTEX_COMMON_SPLASH_A01;
        (void)EFFTEX_COMMON_SPLASH_A02;
        (void)EFFTEX_COMMON_SPLASH_A03;
        (void)EFFTEX_COMMON_SPLASH_A04;
        (void)EFFTEX_COMMON_SPLASH_A05;
        (void)EFFTEX_COMMON_SPLASH_A06;
        (void)EFFTEX_COMMON_SPLASH_A07;
        (void)EFFTEX_COMMON_SPLASH_A08;
        (void)EFFTEX_COMMON_TSUCHI_YAMA;
        (void)EFFTEX_COMMON_WATERCOLUMN01;
        (void)STAGE_TEXTURE_MIU128_FR001;
        (void)STAGE_TEXTURE_MIU128_FR002;
        (void)STAGE_TEXTURE_MIU128_MEH001;
        (void)STAGE_TEXTURE_MIU256_FR001;
        (void)STAGE_TEXTURE_MIU64_MEH001;
        (void)STAGE_TEXTURE_SPACE128_5;
        (void)STAGE_TEXTURE_SPACE128_9;
        (void)STAGE_TEXTURE_SPACE32_2;
        (void)STAGE_TEXTURE_SPACE32_5;
        (void)STAGE_TEXTURE_SPACE32_6;
        (void)STAGE_TEXTURE_SPACE512_2;
        (void)STAGE_TEXTURE_VS256_1;
        (void)STAGE_OBJECT_ARCHIVE;
        (void)COMMON_EFFECT_ARCHIVE;
        (void)COMMON_OBJECT_ARCHIVE;
        (void)BOSS_GAUGE_ARCHIVE;
        (void)OBJECT_NAME_LIGHT_SW;
        (void)OBJECT_NAME_MODMOD;
        (void)OBJECT_NAME_RING;
        (void)OBJECT_NAME_RINGLINE;
        (void)OBJECT_NAME_RINGCIRC;
        (void)OBJECT_NAME_ITEMBOX;
        (void)OBJECT_NAME_ITEMBOXAIR;
        (void)OBJECT_NAME_ORI;
        (void)OBJECT_NAME_CONTWOOD;
        (void)OBJECT_NAME_CONTIRON;
        (void)OBJECT_NAME_SKULL;
        (void)OBJECT_NAME_UDREEL;
        (void)OBJECT_NAME_STOPLSD;
        (void)OBJECT_NAME_PSKULL;
        (void)OBJECT_NAME_SCHBOX;
        (void)OBJECT_NAME_HINTBOX;
        (void)OBJECT_NAME_EFFOBJ0;
        (void)OBJECT_NAME_SPHERE;
        (void)OBJECT_NAME_CCYL;
        (void)OBJECT_NAME_CCUBE;
        (void)OBJECT_NAME_CWALL;
        (void)OBJECT_NAME_CCIRCLE;
        (void)OBJECT_NAME_MSGER;
        (void)OBJECT_NAME_BIG_THE_CAT;
        (void)OBJECT_NAME_DODAI;
        (void)OBJECT_NAME_DODAIGEN;
        (void)OBJECT_NAME_FIREBALL;
    }
    messenger_messages_free();
    if (player_hooks.drop_rings == ring_scatter) {
        player_hooks.drop_rings = NULL;
    }
    if (player_hooks.add_item_icon == item_icon_row_add) {
        player_hooks.add_item_icon = NULL;
    }
    fog_stop();
    njFogDisable();
    particle_core_stop();
    texture_release_shared(object_pvms, object_texture_groups);
    texture_release_shared(background_pvms, background_texture_groups);
    set_end();
    if (versus_sonic_2_set) {
        loader_free(versus_sonic_2_set);
        versus_sonic_2_set = NULL;
    }
    world_land_stop();
    njClipZ(CLIP_NEAR, EXIT_CLIP_FAR);
}

/* the background's texture lists, which take their textures by name from
 * the PVM bgtex42 */
static NJS_TEXLIST *background_group[6] = {
#include "stg42/versus_sonic_2/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "stg42/versus_sonic_2/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "stg42/versus_sonic_2/background_pvms.inc"
};
static Sint32 dat_8c5325f8 = 0;
/* the stage has no kill areas */
static struct kill_area versus_sonic_2_kill_areas[1] = {{0, NULL}};
/* the set file's block, or null */
static struct set_head *versus_sonic_2_set = NULL;
/* the angles the turning models turn each step of the game, about x and
 * about y */
static Angle turning_a_spin_x = 1;
static Angle turning_a_spin_y = 2;
static Angle turning_b_spin_x = 2;
static Angle turning_b_spin_y = 4;
/* the upper and around models' heights above the current camera, and their
 * scales across x and z. floor_height is the floor's y in the world */
static Float upper_height = 1500.0F;
static Float upper_scale = 1.6F;
static Float dat_8c532620 = 0.0F;
static Float dat_8c532624 = 1.0F;
static Float around_height = 0.0F;
static Float around_scale = 1.0F;
static Float floor_height = 0.0F;
static Float floor_scale = 1.0F;
static Float floor_spacing = 7000.0F;
static Float turning_scale = 2.0F;

/* the objects' texture names, by PVM. the lists of object_texture_groups take
 * their textures by name from the PVMs of object_pvms */
#include "shared/common_pvm_textures.h"

static NJS_TEXNAME objtex_stg42_texnames[12] = {
#include "stg42/versus_sonic_2/objtex_stg42_texnames.inc"
};
static NJS_TEXLIST objtex_stg42_texlist = {
#include "stg42/versus_sonic_2/objtex_stg42_texlist.inc"
};
static NJS_TEXLIST *common_group[9] = {
#include "stg42/versus_sonic_2/common_group.inc"
};
static NJS_TEXLIST **object_texture_groups[15] = {
#include "stg42/versus_sonic_2/object_texture_groups.inc"
};
static struct texture_pvm object_pvms[5] = {
#include "stg42/versus_sonic_2/object_pvms.inc"
};
struct object_kind versus_sonic_2_kinds[27] = {
#include "stg42/versus_sonic_2/versus_sonic_2_kinds.inc"
};

void versus_sonic_2_step(struct task *task)
{
    if (kill_areas_task && !kill_areas_task->child) {
        if (player_work[0] && !(player_records[0]->flags & PLAYER_MISSED) &&
            player_work[1] && !(player_records[1]->flags & PLAYER_MISSED)) {
            if (player_work[0]->pos.y < FALL_DEPTH) {
                player_fall_out(0);
            }
        }
    }
    placements_wrap();
}

static void background_step(struct task *task)
{
}

/* draw the floor's tiles about the current camera, then the rest with the
 * fog off. the upper and around models lie at their heights above the
 * camera. the turning models lie at their pos from it */
static void background_draw(struct task *task)
{
    NJS_CNK_OBJECT *object;
    NJS_VECTOR pos;
    int i;
    void (*translate)(NJS_MATRIX *matrix, Float x, Float y, Float z);
    NJS_CNK_MODEL *model;

    njPushMatrixEx();
    njSetTexture(&floor_texlist);
    pos.x = (floorf(camera_views[camera_current_index]->pos.x / floor_spacing) *
             floor_spacing) -
            (floor_spacing * FLOOR_TILES_BEHIND);
    pos.y = floor_height;
    pos.z = 0.0F;
    for (i = 0; i < FLOOR_TILES; i++) {
        njTranslateEx(&pos);
        njScale(NULL, floor_scale, 1.0F, floor_scale);
        njCnkEasyDrawObject(&floor_object);
        matrix_load_parent();
        pos.x += floor_spacing;
    }
    njFogDisable();
    njTranslateEx(&camera_views[camera_current_index]->pos);
    njPushMatrixEx();
    njSetTexture(&upper_texlist);
    njTranslate(NULL, 0.0F, upper_height, 0.0F);
    njScale(NULL, upper_scale, 1.0F, upper_scale);
    njCnkEasyDrawObject(&upper_object);
    matrix_load_parent();
    njSetTexture(&around_texlist);
    njTranslate(NULL, 0.0F, around_height, 0.0F);
    njScale(NULL, around_scale, 1.0F, around_scale);
    njCnkEasyDrawObject(&around_object);
    matrix_load_parent();
    njSetTexture(&turning_a_texlist);
    /* @hack spelling: the assignment inside the argument, where a statement
     * of its own would do */
    njTranslateEx((NJS_VECTOR *)(object = &turning_a_object)->pos);
    njTranslate(NULL, object->model->center.x, object->model->center.y,
                object->model->center.z);
    njRotateX(NULL, (turning_a_spin_x * game_state.steps) + TURNING_TILT);
    njRotateY(NULL, turning_a_spin_y * game_state.steps);
    njScale(NULL, turning_scale, turning_scale, turning_scale);
    /* @hack spelling: translate, a local that calls njTranslate, where
     * njTranslate would do */
    translate = njTranslate;
    translate(NULL, -object->model->center.x, -object->model->center.y,
              -object->model->center.z);
    njCnkEasyDrawModel(object->model);
    matrix_load_parent();
    njSetTexture(&turning_b_texlist);
    object = &turning_b_object;
    njTranslateEx((NJS_VECTOR *)object->pos);
    translate(NULL, object->model->center.x, object->model->center.y,
              object->model->center.z);
    njRotateX(NULL, (turning_b_spin_x * game_state.steps) + TURNING_TILT);
    njRotateY(NULL, turning_b_spin_y * game_state.steps);
    njScale(NULL, turning_scale, turning_scale, turning_scale);
    translate(NULL, -object->model->center.x, -object->model->center.y,
              -object->model->center.z);
    /* @hack spelling: model, a local for the model, where object->model in
     * place would do */
    model = object->model;
    njCnkEasyDrawModel(model);
    njPopMatrix(2);
    njFogEnable();
}

/* keep player 1's z within arena_half_width of 0. it returns TRUE where it
 * set player 1's pos */
static Bool rival_keep_in_arena(struct shadow_chase *record,
                                struct task_work *rival)
{
    if (arena_half_width <= _builtin_fabsf(rival->pos.z)) {
        Float z = arena_half_width;

        if (rival->pos.z < 0.0F) {
            z = -z;
        }
        player_set_pos(1, rival->pos.x, rival->pos.y, z);
        return TRUE;
    }
    return FALSE;
}

static void rival_move_chase(struct shadow_chase *record,
                             struct task_work *player, struct task_work *rival)
{
    if (njDistanceP2P(&player->pos, &rival->pos) > CHASE_APART) {
        record->chase.x = float_approach(
            record->chase.x, player->pos.x + CHASE_LEAD, CHASE_STEP_X);
        record->chase.y = float_approach(record->chase.y, 0.0F, CHASE_STEP_Y);
        record->chase.z = float_approach(record->chase.z, 0.0F, CHASE_STEP_Z);
    } else {
        record->chase = rival->pos;
    }
}

/* TRUE where player 0 is in one of the modes below. player 1 must also face
 * it, or lie within ATTACK_REACH of it. a homing attack counts only while
 * player 1's gauge is no more than HOMING_GAUGE */
static Bool rival_sees_attack(struct shadow_chase *record,
                              struct task_work *player, struct task_work *rival,
                              struct player *rival_record)
{
    switch (player->mode) {
    case PLAYER_MODE_SONIC_HOMING:
        if (rival_record->gauge > HOMING_GAUGE) {
            return FALSE;
        }
        break;
    case PLAYER_MODE_STAND:
    case PLAYER_MODE_RUN:
    case PLAYER_MODE_SONIC_SPIN:
    case PLAYER_MODE_ROLL:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_1:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_2:
        break;
    default:
        return FALSE;
    }
    if (angle_distance(SHADOW_FIGHT_HEADING(rival, player), rival->ang.y) <
        SHADOW_FIGHT_FACING) {
        return TRUE;
    }
    if (njDistanceP2P(&player->pos, &rival->pos) < ATTACK_REACH) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 0 makes a homing attack. player 1 must also face it, or
 * run slower than its sprint_speed */
static Bool rival_sees_homing(struct shadow_chase *record,
                              struct task_work *player, struct task_work *rival,
                              struct player *rival_record)
{
    if (player->mode != PLAYER_MODE_SONIC_HOMING) {
        return FALSE;
    }
    if (angle_distance(SHADOW_FIGHT_HEADING(rival, player), rival->ang.y) <
        SHADOW_FIGHT_FACING) {
        return TRUE;
    }
    if (rival_record->spd.x < rival_record->consts.sprint_speed) {
        return TRUE;
    }
    return FALSE;
}

/* TRUE where player 0 makes a move of its combo. player 1 must also face it */
static Bool rival_sees_combo(struct shadow_chase *record,
                             struct task_work *player, struct task_work *rival)
{
    if (player->mode < PLAYER_MODE_SONIC_COMBO_1 ||
        player->mode > PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
        return FALSE;
    }
    if (angle_distance(SHADOW_FIGHT_HEADING(rival, player), rival->ang.y) <
        SHADOW_FIGHT_FACING) {
        return TRUE;
    }
    return FALSE;
}

static void rival_shadow_step(struct task *task)
{
    struct shadow_chase *record;
    struct task_work *player;
    struct task_work *rival;
    struct player *rival_record;
    struct player *player_record;
    Float rate;
    Angle heading;
    int line;
    int i;

    record = task->extra.p;
    if (game_state.mini_event_running == 1 ||
        (game_state.time_stopped & TIME_STOPPED_BIT(1)) ||
        game_state.cleared == 1) {
        return;
    }
    player = player_work[0];
    rival = player_work[1];
    player_record = player_records[0];
    rival_record = player_records[1];
    if ((player_record->flags & PLAYER_MISSED) ||
        (rival_record->flags & PLAYER_MISSED)) {
        record->control.mode = MODE_STOPPED;
        game_state.ring_levels[1][RING_LEVEL_WIND_CUTTER] = 0;
        game_state.ring_levels[1][RING_LEVEL_TIME_STOP] = 0;
    }
    if ((rival->pos.y < FALL_BEHIND_DEPTH ||
         player->pos.x - FALL_BEHIND_X > rival->pos.x) &&
        record->control.mode != MODE_FALLEN &&
        record->control.mode != MODE_STOPPED) {
        chaos_control_flash_create(1, &rival_record->unk_230);
        record->control.mode = MODE_FALLEN;
        record->control.timer = 0;
        player_stop(NULL, rival_record);
        for (i = 0; i < VOICES; i++) {
            if (voice_can_start(VOICE_PRIORITY, i) == TRUE) {
                voice_stop(i);
            }
        }
        if (rival_record->kind == PLAYER_SONIC) {
            voice_play(SONIC_FALL_VOICE, VOICE_PRIORITY);
        } else if (rival_record->kind == PLAYER_SHADOW) {
            voice_play(SHADOW_FALL_VOICE, VOICE_PRIORITY);
        }
    }
    if (stage_get_mode() == STAGE_FADE_IN ||
        record->control.mode == MODE_WAIT ||
        record->control.mode == MODE_INTRO) {
        pad_sticks[0].ang = 0;
        pad_sticks[0].stroke = 1.0F;
        pad_sticks[1].ang = 0;
        pad_sticks[1].stroke = 1.0F;
        if (record->control.mode != MODE_WAIT &&
            record->control.mode != MODE_INTRO) {
            record->control.mode = MODE_START_RUN;
        }
        player_set_local_speed(0, START_RUN_SPEED, 0.0F, 0.0F);
        player_set_local_speed(1, START_RUN_SPEED, 0.0F, 0.0F);
    } else if (record->control.mode == MODE_START_RUN) {
        record->control.mode = MODE_CHASE;
    }
    switch (record->control.mode) {
    case MODE_WAIT:
        if (stage_get_mode() == STAGE_PLAY) {
            struct camera_bossinit intro;

            pad_disable_player(PAD_BOTH_PLAYERS);
            pause_disable();
            record->control.mode = MODE_INTRO;
            record->control.wait = SHADOW_FIGHT_INTRO_STEPS;
            if (rival_record->kind == PLAYER_SONIC) {
                boss_title_create(NULL, &title_sonic);
            } else if (rival_record->kind == PLAYER_SHADOW) {
                boss_title_create(NULL, &title_shadow);
            }
            intro.speed = INTRO_SPEED;
            if (rival_record->character == PLAYER_SONIC) {
                intro.ang = SONIC_INTRO_ANG;
                intro.turn = SONIC_INTRO_TURN;
            } else {
                intro.ang = INTRO_ANG;
                intro.turn = INTRO_TURN;
            }
            record->control.look = rival->pos;
            record->control.look.y = arena_center->y + INTRO_LOOK_HEIGHT;
            record->control.look.x += INTRO_LOOK_X;
            intro.target = &record->control.look;
            intro.start_dist = INTRO_START_DIST;
            intro.dist = INTRO_DIST;
            camera_bossinit_start(0, &intro);
        }
        break;
    case MODE_INTRO:
        record->control.look = rival->pos;
        record->control.look.y = arena_center->y + INTRO_LOOK_HEIGHT;
        record->control.look.x += INTRO_LOOK_X;
        {
            NJS_POINT3 eye;

            /* @hack spelling: the chained assignment, where two would do */
            eye.x = eye.z = 0.0F;
            eye.y = INTRO_EYE_HEIGHT;
            player_look_at_task(1, player_tasks[0], &eye, LOOK_STEPS);
            player_look_at_task(0, player_tasks[1], &eye, LOOK_STEPS);
        }
        if (record->control.timer > record->control.wait) {
            if (record->control.intro_done == 0) {
                NJS_POINT3 pos;

                line = NO_LINE;
                pos = *arena_center;
                pos.y += SHADOW_FIGHT_MESSENGER_RISE;
                record->control.intro_done = 1;
                switch (stage_get_retries()) {
                case 0:
                    break;
                case 1:
                    line = 0;
                    break;
                case 2:
                    line = 1;
                    break;
                /* @hack redundant: case 3, which the default covers */
                case 3:
                default:
                    line = 2;
                    break;
                }
                if (line != NO_LINE) {
                    if (player_record->character == PLAYER_SONIC) {
                        line += SHADOW_FIGHT_LINES;
                    }
                    messenger_create(&pos, 0, 0, line, MESSENGER_SUMMON, NULL);
                }
            }
            pad_enable_player(0);
            pause_enable();
            record->control.timer = 0;
            record->control.mode = MODE_RACE;
            record->control.wait =
                (rand() * (RACE_FIRST_MORE_STEPS * RAND_SCALE)) +
                RACE_FIRST_STEPS;
        } else if (record->control.timer >
                   record->control.wait - INTRO_SS_STEPS) {
            struct camera_ss look;

            if (camera_top(0) == INTRO_CAMERA_INDEX) {
                camera_bossinit_end(0);
            }
            look.adjust = CAMERA_ADJUST_NONE;
            look.from = &player->pos;
            look.to = &record->chase;
            look.part = INTRO_SS_PART;
            camera_ss_start(0, &look);
        } else if (record->control.timer == INTRO_LINE_STEP) {
            line = NO_LINE;
            if (rival_record->kind == PLAYER_SONIC) {
                line = SONIC_INTRO_VOICE;
            } else if (rival_record->kind == PLAYER_SHADOW) {
                line = SHADOW_INTRO_VOICE;
            }
            if (line != NO_LINE) {
                for (i = 0; i < VOICES; i++) {
                    if (voice_can_start(VOICE_PRIORITY, i) == TRUE) {
                        voice_stop(i);
                    }
                }
                voice_play(line, VOICE_PRIORITY);
            }
        }
        break;
    case MODE_CHASE:
    case MODE_CHASE_AGAIN:
        if (player->pos.x - CHASE_BEHIND > rival->pos.x) {
            heading = SHADOW_FIGHT_HEADING(rival, player);
            pad_sticks[1].ang = heading;
            pad_sticks[1].stroke = 1.0F;
            if (player_record->spd.x > player_record->consts.sprint_speed &&
                rival_record->spd.x > rival_record->consts.sprint_speed) {
                player_set_local_speed(1, rival_record->spd.x * CHASE_BOOST,
                                       rival_record->spd.y,
                                       rival_record->spd.z);
            }
        } else {
            pad_sticks[1].ang = 0;
            pad_sticks[1].stroke = 1.0F;
            if (rival->pos.x > player->pos.x + CHASE_AHEAD &&
                player_record->spd.x > player_record->consts.sprint_speed &&
                rival_record->spd.x > rival_record->consts.sprint_speed) {
                if (rival->mode == PLAYER_MODE_SONIC_HOMING) {
                    rate = HOMING_RATE;
                } else if (rival_record->gauge < GAUGE_LOW) {
                    rate = RATE_LOW;
                } else if (rival_record->gauge < GAUGE_MID) {
                    rate = RATE_MID;
                } else if (rival_record->gauge < GAUGE_HIGH) {
                    rate = RATE_HIGH;
                } else {
                    rate = RATE_FULL;
                }
                player_set_local_speed(1, rival_record->spd.x * rate,
                                       rival_record->spd.y,
                                       rival_record->spd.z);
            }
        }
        if (rival_sees_homing(record, player, rival, rival_record) ||
            rival_sees_combo(record, player, rival)) {
            record->control.timer = 0;
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            record->control.mode = MODE_JUMP;
        } else if (rival_record->gauge <= CHASE_ATTACK_GAUGE &&
                   rival_sees_attack(record, player, rival, rival_record) ==
                       TRUE) {
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            record->control.mode = MODE_JUMP_AT;
            record->control.timer = 0;
        } else if (record->control.timer > CHASE_STEPS) {
            record->control.timer = 0;
            record->control.mode = MODE_RACE;
            record->control.wait =
                (rand() * (RACE_MORE_STEPS * RAND_SCALE)) + RACE_STEPS;
        }
        break;
    case MODE_JUMP:
        if (record->control.timer < JUMP_SURE_STEPS ||
            !(rival->flags & (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE))) {
            pad_jump_held[1] = 1;
            if (rival_record->spd.y == 0.0F &&
                rival->pos.y - player_record->ground.below_y > JUMP_HEIGHT) {
                record->control.mode = MODE_JUMP_TOP;
                record->control.timer = 0;
            }
        } else {
            record->control.mode = MODE_CHASE_AGAIN;
        }
        break;
    case MODE_JUMP_TOP:
        if (record->control.timer < JUMP_SURE_STEPS ||
            !(rival->flags & (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE))) {
            if (!(rival_record->spd.y > JUMP_TOP_RISE) &&
                rival->pos.y - player_record->ground.below_y > JUMP_HEIGHT) {
                record->control.mode = MODE_ATTACK;
                record->control.timer = 0;
                pad_jump_pressed[1] = 1;
                pad_jump_held[1] = 1;
            }
        } else {
            record->control.mode = MODE_CHASE_AGAIN;
        }
        break;
    case MODE_RACE:
        if (rival_sees_homing(record, player, rival, rival_record) ||
            rival_sees_combo(record, player, rival)) {
            record->control.timer = 0;
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            record->control.mode = MODE_JUMP;
            break;
        }
        if (record->control.timer > record->control.wait) {
            record->control.mode = MODE_CHASE;
            record->control.timer = 0;
            break;
        }
        if (rival->pos.y < GAP_DEPTH &&
            !(njFraction((rival->pos.x - GAP_START) *
                         (1.0F / FOOTHOLD_SPACING)) < GAP_EDGE)) {
            pad_sticks[1].ang = 0;
            pad_sticks[1].stroke = 1.0F;
            record->control.timer = 0;
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            record->control.mode = MODE_JUMP_AT;
            break;
        }
        if (rival->pos.x > player->pos.x -
                               (rival_record->gauge * LEVEL_PER_GAUGE) +
                               LEVEL_AHEAD &&
            !(player_record->flags & PLAYER_MISSED) && kill_areas_task &&
            !kill_areas_task->child) {
            record->control.timer = 0;
            record->control.mode = MODE_RING_LEVEL;
            game_state.ring_levels[1][RING_LEVEL_SPEED_UP] = 0;
            game_state.ring_levels[1][RING_LEVEL_WIND_CUTTER] = 1;
            game_state.ring_levels[1][RING_LEVEL_TIME_STOP] = 0;
            for (i = 0; i < VOICES; i++) {
                if (voice_can_start(VOICE_PRIORITY, i) == TRUE) {
                    voice_stop(i);
                }
            }
            switch (rival_record->kind) {
            case PLAYER_SONIC:
                voice_play(SONIC_LEVEL_VOICE, VOICE_PRIORITY);
                break;
            case PLAYER_SHADOW:
                voice_play(SHADOW_LEVEL_VOICE, VOICE_PRIORITY);
            }

            /* @hack type: game_state's ring_level_timer[1] written as
             * game_state_ring_level_timer_1, where
             * game_state.ring_level_timer[1] would do */
            game_state_ring_level_timer_1 = LEVEL_STEPS;
            break;
        }
        pad_sticks[1].ang = 0;
        pad_sticks[1].stroke = 1.0F;
        if (rival_record->gauge <= RACE_ATTACK_GAUGE &&
            rival_sees_attack(record, player, rival, rival_record) == TRUE) {
            pad_jump_pressed[1] = 1;
            pad_jump_held[1] = 1;
            record->control.mode = MODE_JUMP_AT;
            record->control.timer = 0;
        } else if (player->pos.x + RACE_LEAD > rival->pos.x &&
                   player_record->spd.x > player_record->consts.sprint_speed &&
                   rival_record->spd.x > rival_record->consts.sprint_speed) {
            player_set_local_speed(1, rival_record->spd.x * RACE_BOOST,
                                   rival_record->spd.y, rival_record->spd.z);
        } else if (rival->pos.x > player->pos.x + RACE_AHEAD &&
                   player_record->spd.x > player_record->consts.sprint_speed &&
                   rival_record->spd.x > rival_record->consts.sprint_speed) {
            if (rival_record->gauge < GAUGE_LOW) {
                rate = RATE_LOW;
            } else if (rival_record->gauge < GAUGE_MID) {
                rate = RATE_MID;
            } else if (rival_record->gauge < GAUGE_HIGH) {
                rate = RATE_HIGH;
            } else {
                rate = RATE_FULL;
            }
            player_set_local_speed(1, rival_record->spd.x * rate,
                                   rival_record->spd.y, rival_record->spd.z);
        } else if (player->pos.x - CHASE_BEHIND > rival->pos.x &&
                   player_record->spd.x > player_record->consts.sprint_speed &&
                   rival_record->spd.x > rival_record->consts.sprint_speed) {
            /* @bug logic: this branch, at 0x8c5013f8, never runs. player 1
             * more than CHASE_BEHIND behind player 0 passes the RACE_LEAD
             * test, at 0x8c5012d0. so it gets here only where a sprint test
             * failed, and it makes the same tests again */
            player_set_local_speed(1, rival_record->spd.x * CHASE_BOOST,
                                   rival_record->spd.y, rival_record->spd.z);
        }
        break;
    case MODE_JUMP_AT:
        pad_sticks[1].ang = 0;
        pad_sticks[1].stroke = 1.0F;
        if (record->control.timer < JUMP_SURE_STEPS ||
            !(rival->flags & (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE))) {
            pad_jump_held[1] = 1;
            if ((rival_record->spd.y == 0.0F &&
                 rival->pos.y - player_record->ground.below_y > JUMP_HEIGHT) ||
                /* @hack redundant: the subtraction of 0, where pos.y alone
                 * would do */
                (rival->pos.y - 0.0F > JUMP_AT_HEIGHT &&
                 rival_sees_attack(record, player, rival, rival_record) ==
                     TRUE) ||
                rival_record->spd.y < 0.0F) {
                record->control.mode = MODE_JUMP_TOP;
                record->control.timer = 0;
            }
        } else {
            record->control.mode = MODE_RACE;
            record->control.timer = 0;
            record->control.wait =
                (rand() * (RACE_AFTER_JUMP_MORE_STEPS * RAND_SCALE)) +
                RACE_AFTER_JUMP_STEPS;
        }
        break;
    case MODE_RING_LEVEL:
        if (!(player_record->last_flags &
              (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE))) {
            heading = SHADOW_FIGHT_HEADING(rival, player);
            pad_sticks[1].ang = heading;
            pad_sticks[1].stroke = LEVEL_STROKE;
        }
        if (rival->timer > 0) {
            record->control.mode = MODE_CHASE;
            record->control.timer = 0;
        } else if ((game_state.ring_level_timer[1] <= 0 ||
                    record->control.timer >= LEVEL_LONGEST_STEPS) &&
                   !(player_record->flags & PLAYER_MISSED) && kill_areas_task &&
                   !kill_areas_task->child) {
            record->control.timer = 0;
            record->control.mode = MODE_SPECIAL;
            pad_action_pressed[1] = 1;
            pad_action_held[1] = 1;
            record->control.wait = SPECIAL_STEPS;
        }
        break;
    case MODE_ATTACK:
        if (record->control.timer >= JUMP_SURE_STEPS &&
            (rival->flags & (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE))) {
            record->control.mode = MODE_CHASE;
            record->control.timer = 0;
        }
        break;
    case MODE_SPECIAL:
        if (record->control.timer > record->control.wait || rival->timer > 0) {
            record->control.mode = MODE_CHASE;
            record->control.timer = 0;
        }
        break;
    case MODE_FALLEN:
        if (record->control.timer == FALLEN_LINE_STEP) {
            if (rival_record->kind == PLAYER_SONIC) {
                for (i = 0; i < VOICES; i++) {
                    if (voice_can_start(VOICE_PRIORITY, i) == TRUE) {
                        voice_stop(i);
                    }
                }
                voice_play(SONIC_FALLEN_VOICE, VOICE_PRIORITY);
            }
        } else if (record->control.timer < FALLEN_RETURN_STEP) {
            NJS_POINT3 pos;

            pos = player->pos;
            pos.x += FALLEN_AHEAD;
            pos.y = FALLEN_DEPTH;
            pos.z = 0.0F;
            player_set_pos(1, pos.x, pos.y, pos.z);
        } else if (record->control.timer == FALLEN_RETURN_STEP) {
            NJS_POINT3 pos;

            pos = rival->pos;
            pos.y = FALLEN_FLASH_HEIGHT;
            pos.z = 0.0F;
            chaos_control_flash_create(1, &pos);
            sound_play(FLASH_SOUND, NULL, 0, 0);
        } else if (record->control.timer == FALLEN_STAND_STEP) {
            NJS_POINT3 pos;

            pos = rival->pos;
            pos.y = FALLEN_STAND_HEIGHT;
            pos.z = 0.0F;
            player_set_pos(1, pos.x, pos.y, pos.z);
            record->control.mode = MODE_CHASE;
            record->control.timer = 0;
        }
        break;
    }
    rival_keep_in_arena(record, rival);
    rival_move_chase(record, player, rival);
    if (stage_get_mode() == STAGE_FADE_IN || !game_state.steps) {
        rival_record->gauge = RIVAL_GAUGE;
        if (task->child) {
            task_destroy(task->child);
        }
        record->control.intro_done = 0;
        record->control.timer = 0;
        record->control.mode = MODE_WAIT;
    } else {
        record->control.timer++;
    }
    if (rival_record->gauge > 0.0F && !task->child) {
        boss_gauge_create(task, &player_records[1]->gauge);
    }
    if (!(rival_record->gauge > 0.0F)) {
        player_look_at_task(0, player_tasks[1], NULL, LOOK_STEPS);
    }
}

static void rival_shadow_destroy_sonic(struct task *task)
{
    njReleaseTexture(&title_sonic_texlist);
}

static void rival_shadow_destroy_shadow(struct task *task)
{
    njReleaseTexture(&title_shadow_texlist);
}

/* where the heap can't give ControlShadow's record, the task ends at its
 * first step */
static void rival_shadow_create(void)
{
    struct task *task;
    struct shadow_chase *record;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if (!(task = task_create(0, CONTROL_LEVEL, rival_shadow_step,
                             "ControlShadow"))) {
        return;
    }
    if (!(record = syCalloc(1, sizeof(struct shadow_chase)))) {
        task_kill(task);
        return;
    }
    task->extra.p = record;
    player_records[1]->gauge = RIVAL_GAUGE;
    boss_gauge_create(task, &player_records[1]->gauge);
    record->control.mode = MODE_WAIT;
    if (player_records[1]->kind == PLAYER_SONIC) {
        texture_load_pvm(SONIC_TITLE_ARCHIVE, &title_sonic_texlist);
        task->destructor = rival_shadow_destroy_sonic;
    } else if (player_records[1]->kind == PLAYER_SHADOW) {
        texture_load_pvm(SHADOW_TITLE_ARCHIVE, &title_shadow_texlist);
        task->destructor = rival_shadow_destroy_shadow;
    }
}

#include "shared/rival_title_textures.h"

/* the arena's center, which the intro's look point and the messenger rise
 * from */
/* @hack type: arena_center an array of one, where the point would do */
static NJS_POINT3 arena_center[1] = {{0.0F, 0.0F, 0.0F}};
static Float dat_8c533380 = 40.0F;
static Float arena_half_width = 50.0F;
#include "shared/rival_titles.h"
