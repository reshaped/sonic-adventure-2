/* the chaos enemies, which a stage's object list names "E CHAOS" and
 * "E PATHCHAOS". an E CHAOS stands or floats near home, in one of the kinds
 * enemy_chaos.h names. an E PATHCHAOS flies one of the stage's paths */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/enemy_chaos.h"
#include "shared/chaos_art/assets.h"
#include "shared/chaos_art/texture_words.h"
#include "shared/enemy_laser.h"
#include "shared/enemy_route.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* an E CHAOS's work->mode */
enum chaos_mode {
    MODE_INIT,
    MODE_WAIT, /* a standing kind, while no player is near */
    MODE_WANDER, /* a player is near, and the chaos turns between reaches */
    MODE_REACH, /* the arms reach out, and may fire */
    MODE_FLOAT, /* CHAOS_FLOAT or a shedding kind, while no player is near */
    MODE_FLOAT_WANDER, /* MODE_WANDER, for CHAOS_FLOAT */
    MODE_FLOAT_REACH, /* MODE_REACH, for CHAOS_FLOAT */
    MODE_SPIN, /* a round kind, once a player is near */
    MODE_SHED, /* a shedding kind sheds its pieces, or stands spent */
    MODE_GUARD, /* CHAOS_GUARD in its shell, or on its way in or out */
    MODE_DEFEATED, /* a hit has beaten the chaos, which its next step kills */
    MODE_DEAD
};

/* an E PATHCHAOS's work->mode */
enum path_chaos_mode {
    MODE_PATH_INIT,
    MODE_PATH_WAIT, /* undrawn and touching nothing, until a player wakes it */
    MODE_PATH_RIDE, /* along the path */
    MODE_PATH_FLY_ON, /* past the path's end, in a straight line */
    MODE_PATH_HOVER, /* at the path's end, under PATH_CHAOS_STOP */
    MODE_PATH_HIT, /* a hit has beaten the chaos, which its next step kills */
    MODE_PATH_GONE
};

/* the indexes of chaos_actions. a firing reach fires as its action ends. no
 * step asks for chaos_actions[4] */
#define ACTION_IDLE 0
#define ACTION_REACH 1 /* a reach that doesn't fire */
#define ACTION_REACH_FIRE_UP 2
#define ACTION_REACH_FIRE_DOWN 3
#define ACTION_FLOAT 5
#define ACTION_FLOAT_REACH 6 /* a reach that doesn't fire */
#define ACTION_FLOAT_REACH_FIRE 7
#define ACTION_SPIN 8
#define ACTION_SHED_START 9
#define ACTION_SHED 10
#define ACTION_SHED_END 11
#define ACTION_SPENT 12 /* a shedding kind with no pieces left */
#define ACTION_GUARD 13 /* CHAOS_GUARD's open body, before ACTION_SHUTTING */
#define ACTION_SHUTTING 14
#define ACTION_SHUT 15
#define ACTION_OPENING 16

/* an E CHAOS's bits in work->flags */
/* a player was near at the last look, by chaos_mark_player_near_home or
 * chaos_mark_player_near_offset */
#define CHAOS_PLAYER_NEAR 0x2000
/* chaos_turn heads the chaos home while CHAOS_HOMEWARD is set. no code
 * sets it */
#define CHAOS_HOMEWARD 0x1000

/* the wake range is scl.z + RANGE, and near_distance2 holds it squared */
#define RANGE 60.0F
#define SIGHT_ANGLE ((Angle)(160 * ANGLE_PER_DEGREE))
#define TURN_SPEED 0x200
#define SHADOW_SIZE 6.0F
#define SHADOW_RATIO 1.0F

/* an E CHAOS's own fields of its enemy_work */
/* the steps between reaches. a round kind's are the steps from a shot to
 * the start of its next charge */
#define INTERVAL(enemy) ((enemy)->unk_048)
#define SHED_COUNT(enemy) ((enemy)->unk_048) /* the pieces a shed makes */
#define LASER_LENGTH(enemy) ((enemy)->unk_047)
#define LASER_SPEED(enemy) ((enemy)->unk_094)
#define SPIN_SPEED(enemy) ((enemy)->unk_098) /* the spin, in turns a step */
/* a round kind's charge toward its next shot. its muzzle flashes play the
 * charge as their frame */
#define CHARGE(enemy) ((enemy)->unk_09c)
#define BOB_PHASE(enemy) ((enemy)->unk_018) /* the bob's phase offset */

/* an E PATHCHAOS's own fields of its enemy_work */
/* the distances along the path at which it starts and stops firing */
#define FIRE_START(enemy) ((enemy)->unk_00c.x)
#define FIRE_END(enemy) ((enemy)->unk_00c.y)
#define PATH_TURN(enemy) ((enemy)->unk_01c) /* the path's turn about y */
#define PATH_BOB_PHASE(enemy) ((enemy)->unk_024)
#define PATH_INDEX(enemy) ((enemy)->unk_046) /* in path_chaos_paths */
#define RELOAD(enemy) ((enemy)->unk_048) /* the steps left before it fires */
#define TEX_FRAME(enemy) ((enemy)->unk_04e) /* as TEXTURE_FIRST says */
#define DISTANCE(enemy) ((enemy)->unk_094) /* how far along the path it is */
/* how near a player must come to the wake point, home plus offset */
#define WAKE_RANGE(enemy) ((enemy)->unk_098)
/* the speed along the path, in distance a step */
#define SPEED(enemy) ((enemy)->unk_09c)

/* a body's frame counts steps from 0 to FRAME_LAST, and then starts over.
 * the body draws texture TEXTURE_FIRST + frame / 2 of chaos_texlist, one of
 * FRAME_TEXTURES */
#define TEXTURE_FIRST 4
#define FRAME_LAST 12
#define FRAME_TEXTURES ((FRAME_LAST / 2) + 1)
/* a frame writes the texture's index into a texture word's
 * TEXID_INDEX_BITS. the write also clears the word's NJD_FSS, which
 * super-samples the texture, and keeps its filter mode, NJD_FFM_MASK */
#define TEXID_INDEX_BITS 0x1fff
#define TEXID_BITS (NJD_FSS | TEXID_INDEX_BITS)

/* an arm is a chain of ARM_JOINTS nodes, node 0 at its root. arms_pose
 * turns and scales the first ARM_NODES. a collision shape lies along each of
 * those, out to the next node's origin */
#define ARM_NODES 8
#define ARM_JOINTS 9
/* a node past the root stands NODE_SPAN from the one before while its length
 * lies below 1. it stands its length times NODE_SPAN from it otherwise */
#define NODE_SPAN 5.0F
/* the first BODY_SHAPES shapes of a chaos's collision record are its
 * body's. a kind with arms has ARM_NODES more for each arm, and a record of
 * radius ARMED_RADIUS */
#define BODY_SHAPES 2
#define ARMED_SHAPES (BODY_SHAPES + (2 * ARM_NODES))
#define ARMED_RADIUS 400.0F
/* the body's lower shape. CHAOS_FLOAT, and a chaos in MODE_GUARD, throw
 * back a player who touches it */
#define LOWER_SHAPE 1
/* a step grows an arm's root by ROOT_GROW, up to 1, its model's own length.
 * a node past the root grows by NODE_GROW of the arm's reach, up to the
 * reach. a step shrinks a node by ARM_SHRINK of the reach */
#define ROOT_GROW 0.1F
#define NODE_GROW 0.2F
#define ARM_SHRINK 0.4F
/* an arm aims where the nearest player will be in LEAD_STEPS steps, at the
 * player's speed */
#define LEAD_STEPS 12.0F
/* a step bends an arm's joint toward its aim by ARM_TURN, to ARM_BEND at
 * most either way. node 1 also lifts, to ARM_LIFT at most */
#define ARM_TURN 0x800
#define ARM_BEND 0x1800
#define ARM_LIFT 0x2000
/* an arm's reach is REACH_SCALE times the nearest player's distance, held
 * from REACH_MIN to REACH_MAX. each node past the root grows to that length,
 * in its model's own lengths */
#define REACH_SCALE 0.060000002F
#define REACH_MIN 1.0F
#define REACH_MAX 6.0F
/* a reach lasts REACH_STEPS steps before the arms pull back. its arms aim
 * through its first AIM_STEPS steps */
#define REACH_STEPS 70
#define AIM_STEPS 45
/* a reach fires only where the nearest player is within FIRE_RANGE as it
 * starts. it fires down at a player whose flags hold a bit of
 * FIRE_DOWN_FLAGS, and up at others */
#define FIRE_RANGE 100.0F
#define FIRE_DOWN_FLAGS (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)
/* within its first SWITCH_STEPS steps, a reach that doesn't fire becomes one
 * that fires up. the nearest player must then hold neither bit of
 * FIRE_DOWN_FLAGS. a Knuckles or a Rouge must not be in
 * PLAYER_MODE_HUNTER_GLIDE */
#define SWITCH_STEPS 40
/* a firing reach draws a flash at each muzzle through its action's first
 * FLASH_FRAMES frames. a flash's frame is FLASH_RATE times the action's. the
 * flashes stand at FLASH_A and FLASH_B in the muzzle node's axes, and both
 * turn by FLASH_TURN */
#define FLASH_FRAMES 3.0F
#define FLASH_RATE 3.0F
#define FLASH_A_X (-3.852F)
#define FLASH_A_Y 1.6037F
#define FLASH_A_Z (-1.8111F)
#define FLASH_B_X (-3.842F)
#define FLASH_B_Y 1.6178F
#define FLASH_B_Z 1.7419F
#define FLASH_TURN_X 1
#define FLASH_TURN_Y 50
#define FLASH_TURN_Z 0x27b2
/* a muzzle node's muzzles stand at MUZZLE_X in x of its axes. muzzle a lies
 * at -MUZZLE_Z in z, and muzzle b at MUZZLE_Z */
#define MUZZLE_X (-6.0F)
#define MUZZLE_Z 1.8F

/* while a walker's lock holds CHAOS_GUARD, it leaves its shell only after
 * SHELL_STEPS steps near a player. it goes back in after WANDER_STEPS steps
 * of wandering */
#define SHELL_STEPS 180
#define WANDER_STEPS 20
/* the center of the body's first shape stands BODY_HEIGHT above pos. the
 * muzzles start at that height, until a draw records them */
#define BODY_HEIGHT 30.0F
/* while CHAOS_GUARD's shell shuts or opens, each frame of its motion sinks
 * the body's first shape by SHELL_SINK. a shut shell holds the shape
 * SHELL_FRAMES * SHELL_SINK low. the shape guards at SHELL_GUARD while the
 * shell is shut. a moving shell's shape guards at 0 before its motion's
 * frame 1, and at SHELL_GUARD from there. an open shell's shape guards at
 * 0 */
#define SHELL_SINK 1.3F
#define SHELL_FRAMES 12.0F
#define SHELL_GUARD 2

/* a wandering chaos picks a new aim near home after TURN_STEPS steps, plus
 * up to TURN_SPREAD at random. the aim lies in a square about home whose
 * side is HOME_SPREAD times the wake range */
#define TURN_STEPS 60.0F
#define TURN_SPREAD 180.0F
#define HOME_SPREAD 0.2F

/* the points score_add_enemy adds for a beaten chaos: SCORE, or SCORE_KNOCK
 * where player_touch_attacks returns TRUE */
#define SCORE 200
#define SCORE_KNOCK 400
/* a beaten chaos's ball, under ENEMY_IN_MAGIC_HAND, grows to BALL_SCALE. it
 * draws the chaos's model off its center by BALL_OFFSET_Y in y */
#define BALL_SCALE 0.09F
#define BALL_OFFSET_Y (-18.0F)
/* a chaos throws a player back at THROW_SPEED along the player's own x, and
 * THROW_LIFT up */
#define THROW_SPEED (-2.5F)
#define THROW_LIFT 1.0F

/* the steps of chaos_wait, chaos_guard, chaos_spin, chaos_reach and the
 * floating modes play SOUND_STEP for RENEWED_DURATION. a step that finds the
 * sound playing renews it. every E CHAOS keys the sound to chaos_step, so
 * all of them share it */
#define SOUND_STEP 0x4024
/* chaos_shed plays SOUND_SHEDDING the same way, spent or not, keyed to the
 * chaos's work */
#define SOUND_SHEDDING 0x4025
#define SOUND_BOUNCE 0x4017 /* as a chaos throws a player back */
#define SOUND_SHED 0x4018 /* as a shed starts */
#define SOUND_BURST 0x4019 /* as a piece bursts */
/* as a hit or its parent's defeat ends a piece */
#define SOUND_PIECE_HIT 0x8015
#define STEP_VOLUME 60
#define PIECE_VOLUME 80
/* the sounds' durations, in steps */
#define RENEWED_DURATION 4
#define PIECE_HIT_DURATION 120
/* a throw shakes the player's pad. a shed's start gives player 0's pad a
 * fading shake, in a one-player game */
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 7
#define SHED_VIBRATION_POWER 3
#define SHED_VIBRATION_FREQ 20
#define SHED_VIBRATION_LENGTH 128

/* a shedding kind sheds SHED_SCALE times scl.y's fraction in pieces. its
 * action becomes ACTION_SHED_END once SHED_END_LEFT pieces or fewer are left
 * to shed */
#define SHED_SCALE 100.0F
#define SHED_END_LEFT 10
/* the box a shedding kind's pieces stay in, in x and z. the signed bits of
 * ang.x or ang.z above its LOW_BYTE place the box's center from pos. its
 * LOW_BYTE gives the box's half width. each counts BOX_UNIT */
#define BOX_UNIT 2.0F
/* a shedding kind stands h above home. it wakes only for a player within
 * WAKE_HEIGHTS times h of it, in y */
#define WAKE_HEIGHTS 2.0F
/* farther in y than any player, where the search for the nearest starts */
#define FAR_AWAY 10000000.0F
/* the placement's state once its chaos has shed */
#define STATE_SHED 1
/* each part of a shed piece's spd starts from SHED_SPEED_MIN up to
 * SHED_SPEED_MIN + 1. the spd then turns to the piece's heading */
#define SHED_SPEED_MIN 0.5F
/* a step takes FALL_GRAVITY from a falling piece's spd.y, down to
 * -FALL_FASTEST */
#define FALL_GRAVITY 0.08F
#define FALL_FASTEST 2.0F
/* a piece lands PIECE_HEIGHT above its parent's home, and its model draws
 * PIECE_HEIGHT below its pos. its collision record's radius is
 * PIECE_RADIUS */
#define PIECE_HEIGHT 6.0F
#define PIECE_RADIUS 6.0F
/* the points a player scores for ending a piece */
#define PIECE_SCORE 10
/* a crawl's phase grows by CRAWL_RATE a step, and the crawl ends as it reaches
 * CRAWL_END. while the phase lies below CRAWL_STOP, the piece moves at its spd.
 * from CRAWL_STOP on, the piece stands and turns toward aim_y by CRAWL_TURN a
 * step. its spd turns with it, at CRAWL_SPEED. the piece's pitch is 0 while the
 * phase is CRAWL_TILT or below. the pitch then falls by TILT_RATE a step until
 * the phase passes CRAWL_STOP. it rises by twice that after */
#define CRAWL_SPEED 0.35F
#define CRAWL_RATE 0.2F
#define CRAWL_TILT 2.0F
#define CRAWL_STOP 6.0F
#define CRAWL_END 8.0F
#define CRAWL_TURN 0x300
#define TILT_RATE 0x80
/* at a crawl's end, a piece within FLASH_RANGE of the nearest player starts to
 * flash. any other aims its next crawl at that player */
#define FLASH_RANGE 20.0F
/* a piece flashes for FLASH_STEPS steps, then bursts. its burst hurts within
 * BURST_RADIUS for BURST_STEPS steps */
#define FLASH_STEPS 120
#define BURST_STEPS 10
#define BURST_RADIUS 15.0F
/* a flashing piece counts its steps left in its work's timer. LIGHT_BELOW holds
 * a threshold, FLASH_FIRST at first. a step that finds the count below it
 * lights the piece for that step, and halves it. the lights thus come faster
 * toward the burst */
#define FLASH_FIRST 63
#define LIGHT_BELOW(work) ((work)->submode)
/* 1 for the step the piece lights up, and 0 otherwise */
#define PIECE_LIT(work) ((work)->tick)

/* a round kind fires once its CHARGE passes CHARGE_FULL. it charges by
 * CHARGE_STEP a step, once INTERVAL steps have passed since its last shot */
#define CHARGE_STEP 0.5F
#define CHARGE_FULL 9.0F
/* a round kind's motion turns its body once in SPIN_FRAMES frames */
#define SPIN_FRAMES 16.0F

/* each step, a floating chaos's bob turns by BOB_SPEED. it bobs BOB_HEIGHT
 * above and below its height */
#define BOB_SPEED 384
#define BOB_HEIGHT 3.0F

/* an E PATHCHAOS's bob phase turns by PATH_BOB_TURN a step. riding, the chaos
 * stands PATH_BOB_RIDE times the phase's sine above the path. hovering, the
 * chaos rises by PATH_BOB_HOVER times the phase's cosine a step. its ang.x and
 * ang.z near 0 by LEVEL_EASE a step */
#define PATH_BOB_TURN 0x400
#define PATH_BOB_RIDE 3.0F
#define PATH_BOB_HOVER 0.2F
#define LEVEL_EASE 0x100
#define PATH_RELOAD 40 /* the steps between an E PATHCHAOS's shots */
#define PATH_LASER_LENGTH 32
/* an E PATHCHAOS's muzzles stand at PATH_MUZZLE_X and PATH_MUZZLE_Y in its
 * own axes. they lie PATH_MUZZLE_Z to either side in z */
#define PATH_MUZZLE_X 4.0F
#define PATH_MUZZLE_Y 24.0F
#define PATH_MUZZLE_Z 1.5F
#define PATH_TURN_SPEED 0x400
#define PATH_SHADOW_SIZE 12.0F
/* the scales of the values an E PATHCHAOS reads from scl's fractions, as
 * enemy_chaos.h gives them. WAKE_RANGE also adds WAKE_MIN */
#define WAKE_SCALE 400.0F
#define WAKE_MIN 10.0F
#define START_SCALE 100.0F
#define SPEED_SCALE 10.0F
/* an E PATHCHAOS keeps its lasers' speed in its work's submode */
#define PATH_LASER_SPEED(work) ((work)->submode)
/* under PATH_CHAOS_STOP, an E PATHCHAOS slows within STOP_DISTANCE of the
 * path's end. its speed there is its SPEED times the distance left times
 * STOP_EASE, and STOP_MIN_SPEED at least */
#define STOP_DISTANCE 50.0F
#define STOP_EASE 0.2F
#define STOP_MIN_SPEED 0.3F
/* an E PATHCHAOS on the path of index LEVEL_PATH stays level. on the others it
 * leans as the path does */
#define LEVEL_PATH 4

/* a shot's heading lies within AIM_YAW of the chaos's. a reach's shot up
 * pitches within AIM_SPREAD of AIM_UP, and its shot down within AIM_SPREAD of
 * AIM_DOWN. an E PATHCHAOS's shot pitches within AIM_PITCH of level */
#define AIM_YAW 0x800
#define AIM_UP 0x1800
#define AIM_DOWN (FULL_TURN - 0x2800)
#define AIM_SPREAD 0x1800
#define AIM_PITCH 0x1800

/* the bytes of a set entry's angle: its low byte, and its high byte, which
 * HIGH_BYTE_SHIFT brings down */
#define LOW_BYTE 0xff
#define HIGH_BYTE 0xff00
#define HIGH_BYTE_SHIFT 8
/* a round kind's spin counts ang.x's high byte in SPIN_UNITs, each a 256th
 * of a turn a step */
#define SPIN_UNIT (1.0F / 256.0F)
/* the fields of an E PATHCHAOS's ang.x, as enemy_chaos.h gives them */
#define PATH_LASER_SPEED_BITS 0xf
#define PATH_FLAGS_BITS 0xff0
#define PATH_FLAGS_SHIFT 4
#define PATH_INDEX_BITS 0xf000
#define PATH_INDEX_SHIFT 12
#define PATH_ACTION 0 /* path_chaos_actions' one action */
/* a lit piece draws under a light of LIT_RED, LIT_GREEN and LIT_BLUE */
#define LIT_RED 2.0F
#define LIT_GREEN 0.3F
#define LIT_BLUE 0.3F
#define TEXTURE_LEVEL 0 /* the level of the textures' and the builds' tasks */

/* an E CHAOS's own record, in its task's extra */
struct chaos_state {
    Sint8 tex_frame; /* the body's frame, as TEXTURE_FIRST says */
    Uint8 unk_01;
    Sint16 turn_timer; /* the steps before chaos_turn picks a new aim */
    /* the box across x and z that a shedding kind's pieces stay in */
    Float x_max;
    Float x_min;
    Float z_max;
    Float z_min;
    /* where the two lasers start, in the world, as the last draw left them */
    NJS_POINT3 muzzle_a;
    NJS_POINT3 muzzle_b;
    Uint8 unk_2c[0x18];
    /* the length each node past an arm's root grows to, as REACH_SCALE
     * says */
    Float reach_a;
    Float reach_b;
    /* each node's length along x: 0 for none, and 1 for its model's own */
    Float len_a[ARM_NODES];
    Float len_b[ARM_NODES];
    struct angles ang_a[ARM_NODES];
    struct angles ang_b[ARM_NODES];
    /* each node's origin in the world, as the last draw left it */
    NJS_POINT3 joint_a[ARM_JOINTS];
    NJS_POINT3 joint_b[ARM_JOINTS];
    /* each arm's root, in the model the chaos draws */
    NJS_CNK_OBJECT *nodes_a;
    NJS_CNK_OBJECT *nodes_b;
};

/* a shed piece's own record, in its task's extra */
struct piece {
    Sint16 frame; /* as TEXTURE_FIRST says */
    Uint8 unk_02[2];
    /* a copy of piece_a_object. a piece's draw reads only its pos and ang */
    NJS_CNK_OBJECT object;
    NJS_VECTOR spd;
    Angle aim_y; /* the heading a crawling piece turns toward */
    Float crawl_phase; /* as CRAWL_RATE says */
};

#define PATH_CHAOS_PATHS ARRAY_COUNT(path_chaos_paths)

/* draw_inverse inverts the matrix chaos_draw starts from. it takes a drawn
 * point back into the world. drawn_state is the drawn chaos's state.
 * chaos_record_nodes reads both, and keeps a muzzle node's matrix in
 * muzzle_matrix, where the muzzle flashes draw */
static NJS_MATRIX draw_inverse;
static NJS_MATRIX muzzle_matrix;
static struct chaos_state *drawn_state;

/* the task that keeps chaos_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *chaos_textures = NULL;
/* the task that keeps the pieces' builds, or null while none does. a build
 * is a piece's model compiled with one of its FRAME_TEXTURES textures. the
 * task's extra holds piece_a_object's builds, each unk_3c bytes long. its
 * motion holds piece_b_object's, each as long as force says */
static struct task *piece_builds = NULL;

/* the second names of four functions, whose addresses the image holds in
 * registers, as TOOLCHAIN.md's "Second names" says. the source names each
 * function only by its second name. SHC leaves out a static function its
 * source never names, so each of the four is external */
#ifdef NONMATCHING
/* this side binds each second name to its function. its objects don't define
 * the second names, which only symbols.txt places */
#define piece_builds_destroy_outside piece_builds_destroy
#define builds_step_outside builds_step
#define arms_pose_outside arms_pose
#define chaos_record_nodes_outside chaos_record_nodes
#else
void piece_builds_destroy_outside(struct task *task);
void builds_step_outside(struct task *task);
void arms_pose_outside(struct chaos_state *state);
void chaos_record_nodes_outside(NJS_CNK_OBJECT *object);
#endif

static void chaos_textures_destroy(struct task *task)
{
    njReleaseTexture(&chaos_texlist);
    chaos_textures = NULL;
}

static void textures_step(struct task *task)
{
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* piece_builds keeps a size in force, not a record. clearing force keeps
 * task_destroy from freeing it */
/* @hack type: external linkage, where static would do */
void piece_builds_destroy(struct task *task)
{
    piece_builds = NULL;
    /* @hack redundant: the test of force before it is cleared, where clearing
     * it alone would do */
    if (task->force.p != NULL) {
        task->force.p = NULL;
    }
}

/* @hack type: external linkage, where static would do */
void builds_step(struct task *task)
{
}
/* NOLINTEND(misc-use-internal-linkage) */

/* fold both arms to no length, and set each arm's reach as REACH_SCALE
 * says */
static void arms_reset(struct task_work *work, struct chaos_state *state)
{
    NJS_CNK_OBJECT *node;
    NJS_POINT3 pos;
    int player;
    int i;

    player = player_nearest(&work->pos);
    node = state->nodes_a;
    for (i = 0; i < ARM_NODES; i++) {
        /* @hack spelling: the loop's break in an else, where a break on a
         * null node first would do */
        if (node != NULL) {
            if (i) {
                state->len_a[i] = 0.0F;
                node->pos[0] = 0.0F;
                node->scl[0] = 0.0F;
                state->ang_a[i].x = 0;
                node->ang[0] = 0;
                state->ang_a[i].y = 0;
                node->ang[1] = 0;
                state->ang_a[i].z = 0;
                node->ang[2] = 0;
            } else {
                state->len_a[i] = 0.0F;
                node->scl[0] = 0.0F;
                state->ang_a[i].x = node->ang[0];
                state->ang_a[i].y = node->ang[1];
                state->ang_a[i].z = node->ang[2];
            }
            node->evalflags &= ~(NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
            node = node->child;
        } else {
            break;
        }
    }
    if (player == 0 || player == 1) {
        pos = player_work[player]->collision->shapes->center;
        state->reach_a = njDistanceP2P(&work->pos, &pos) * REACH_SCALE;
    } else {
        state->reach_a = REACH_MAX;
    }
    if (state->reach_a > REACH_MAX) {
        state->reach_a = REACH_MAX;
    } else if (state->reach_a < REACH_MIN) {
        state->reach_a = REACH_MIN;
    }

    node = state->nodes_b;
    for (i = 0; i < ARM_NODES; i++) {
        /* @hack spelling: the loop's break in an else, where a break on a
         * null node first would do */
        if (node != NULL) {
            if (i) {
                state->len_b[i] = 0.0F;
                node->pos[0] = 0.0F;
                node->scl[0] = 0.0F;
                /* @hack spelling: one assignment to the node's angle and to
                 * the arm's, where a statement for each would do */
                node->ang[0] = state->ang_b[i].x = 0;
                /* @hack spelling: one assignment to the node's angle and to
                 * the arm's, where a statement for each would do */
                node->ang[1] = state->ang_b[i].y = 0;
                /* @hack spelling: one assignment to the node's angle and to
                 * the arm's, where a statement for each would do */
                node->ang[2] = state->ang_b[i].z = 0;
            } else {
                state->len_b[i] = 0.0F;
                node->scl[0] = 0.0F;
                state->ang_b[i].x = node->ang[0];
                state->ang_b[i].y = node->ang[1];
                state->ang_b[i].z = node->ang[2];
            }
            node->evalflags &= ~(NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
            node = node->child;
        } else {
            break;
        }
    }
    if (player == 0 || player == 1) {
        pos = player_work[player]->collision->shapes->center;
        state->reach_b = njDistanceP2P(&work->pos, &pos) * REACH_SCALE;
    } else {
        state->reach_b = REACH_MAX;
    }
    if (state->reach_b > REACH_MAX) {
        state->reach_b = REACH_MAX;
    } else if (state->reach_b < REACH_MIN) {
        state->reach_b = REACH_MIN;
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set the arms' nodes from their lengths and angles. a root's length scales
 * it along x. a node past the root scales along x by its length, up to 1.
 * past 1, its scale stays 1, and it stands its length times NODE_SPAN from
 * the node before */
/* @hack type: external linkage, where static would do */
void arms_pose(struct chaos_state *state)
{
    NJS_CNK_OBJECT *node;
    int i;

    node = state->nodes_a;
    for (i = 0; i < ARM_NODES; i++) {
        /* @hack spelling: the loop's break in an else, where a break on a
         * null node first would do */
        if (node != NULL) {
            if (i) {
                if (state->len_a[i] < 1.0F) {
                    node->pos[0] = NODE_SPAN;
                    node->scl[0] = state->len_a[i];
                } else {
                    node->pos[0] = state->len_a[i] * NODE_SPAN;
                    node->scl[0] = 1.0F;
                }
            } else {
                node->scl[0] = state->len_a[i];
            }
            node->ang[0] = state->ang_a[i].x;
            node->ang[1] = state->ang_a[i].y;
            node->ang[2] = state->ang_a[i].z;
            node = node->child;
        } else {
            break;
        }
    }

    node = state->nodes_b;
    for (i = 0; i < ARM_NODES; i++) {
        /* @hack spelling: the loop's break in an else, where a break on a
         * null node first would do */
        if (node != NULL) {
            if (i) {
                if (state->len_b[i] < 1.0F) {
                    node->pos[0] = NODE_SPAN;
                    node->scl[0] = state->len_b[i];
                } else {
                    node->pos[0] = state->len_b[i] * NODE_SPAN;
                    node->scl[0] = 1.0F;
                }
            } else {
                node->scl[0] = state->len_b[i];
            }
            node->ang[0] = state->ang_b[i].x;
            node->ang[1] = state->ang_b[i].y;
            node->ang[2] = state->ang_b[i].z;
            node = node->child;
        } else {
            break;
        }
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* turn on the arms' collision shapes. lay each along its node, between the
 * joints the last draw recorded */
static void arms_fit_shapes(struct task_work *work, struct chaos_state *state)
{
    struct collision_shape *shape;
    /* @hack spelling: len, a pointer that walks the arm's lengths, where
     * state->len_a[i - 1] read in place would do */
    Float *len;
    NJS_VECTOR span;
    Float length2;
    /* @hack spelling: size, a local for the shape's size_y, where the
     * product stored in place would do */
    Float size;
    int i;

    if (work->collision == NULL) {
        return;
    }
    shape = &work->collision->shapes[BODY_SHAPES];
    len = &state->len_a[0];
    for (i = 1; i < ARM_JOINTS; i++) {
        shape->center.x =
            (state->joint_a[i].x + state->joint_a[i - 1].x) * 0.5F;
        shape->center.y =
            (state->joint_a[i].y + state->joint_a[i - 1].y) * 0.5F;
        shape->center.z =
            (state->joint_a[i].z + state->joint_a[i - 1].z) * 0.5F;
        size = *len++ * 2.0F;
        shape->size_y = size;
        span = state->joint_a[i];
        njSubVector(&span, &state->joint_a[i - 1]);
        shape->ang.y = (Angle)(atan2f(span.x, span.z) * ANGLE_PER_RADIAN);
        /* @hack spelling: the assignment inside the test, where a
         * statement of its own would do */
        if ((length2 = njScalor2(&span)) == 0.0F) {
            shape->ang.x = QUARTER_TURN;
        } else {
            shape->ang.x =
                QUARTER_TURN - (Angle)(asinf(span.y * njInvertSqrt(length2)) *
                                       ANGLE_PER_RADIAN);
        }
        shape->ang.z = 0;
        shape->flags &= ~SHAPE_OFF;
        shape++;
    }
    len = &state->len_b[0];
    for (i = 1; i < ARM_JOINTS; i++) {
        shape->center.x =
            (state->joint_b[i].x + state->joint_b[i - 1].x) * 0.5F;
        shape->center.y =
            (state->joint_b[i].y + state->joint_b[i - 1].y) * 0.5F;
        shape->center.z =
            (state->joint_b[i].z + state->joint_b[i - 1].z) * 0.5F;
        size = *len++ * 2.0F;
        shape->size_y = size;
        span = state->joint_b[i];
        njSubVector(&span, &state->joint_b[i - 1]);
        shape->ang.y = (Angle)(atan2f(span.x, span.z) * ANGLE_PER_RADIAN);
        /* @hack spelling: the assignment inside the test, where a
         * statement of its own would do */
        if ((length2 = njScalor2(&span)) == 0.0F) {
            shape->ang.x = QUARTER_TURN;
        } else {
            shape->ang.x =
                QUARTER_TURN - (Angle)(asinf(span.y * njInvertSqrt(length2)) *
                                       ANGLE_PER_RADIAN);
        }
        shape->ang.z = 0;
        shape->flags &= ~SHAPE_OFF;
        shape++;
    }
}

static void arms_shapes_off(struct task_work *work, struct chaos_state *state)
{
    struct collision_shape *shape;
    int i;

    if (work->collision != NULL) {
        shape = &work->collision->shapes[BODY_SHAPES];
        for (i = 0; i < ARM_NODES; i++) {
            shape->flags |= SHAPE_OFF;
            shape++;
        }
        for (i = 0; i < ARM_NODES; i++) {
            shape->flags |= SHAPE_OFF;
            shape++;
        }
    }
}

/* grow each arm a step from its root out, as ROOT_GROW says. a node grows only
 * once the node before it has its full length */
static void arms_extend(struct chaos_state *state)
{
    Float len;
    Float step;
    int i;

    len = 1.0F;
    step = ROOT_GROW;
    for (i = 0; i < ARM_NODES; i++) {
        state->len_a[i] = float_approach(state->len_a[i], len, step);
        if (state->len_a[i] < len) {
            break;
        }
        len = state->reach_a;
        step = state->reach_a * NODE_GROW;
    }
    len = 1.0F;
    step = ROOT_GROW;
    for (i = 0; i < ARM_NODES; i++) {
        state->len_b[i] = float_approach(state->len_b[i], len, step);
        if (state->len_b[i] < len) {
            break;
        }
        len = state->reach_b;
        step = state->reach_b * NODE_GROW;
    }
}

/* shrink each arm a step from its tip in, as ARM_SHRINK says. a node
 * shrinks only once the node past it has no length */
static void arms_retract(struct chaos_state *state)
{
    int i;

    for (i = ARM_NODES - 1; i >= 0; i--) {
        state->len_a[i] =
            float_approach(state->len_a[i], 0.0F, state->reach_a * ARM_SHRINK);
        if (state->len_a[i] > 0.0F) {
            break;
        }
    }
    for (i = ARM_NODES - 1; i >= 0; i--) {
        state->len_b[i] =
            float_approach(state->len_b[i], 0.0F, state->reach_b * ARM_SHRINK);
        if (state->len_b[i] > 0.0F) {
            break;
        }
    }
}

/* LENGTH_A(state, i) and LENGTH_B(state, i): the length of node i of state's
 * arm a and arm b. at ARM_NODES, one past the last node, each reads the word
 * after the arm's lengths, as the tags in arms_aim say */
#ifdef NONMATCHING
/* this side keeps those tags' results by defined means. at ARM_NODES,
 * LENGTH_A reads len_b[0], and LENGTH_B the bits of ang_a[0].x as a Float */
#define LENGTH_A(state, i)                                                     \
    ((i) < ARM_NODES ? (state)->len_a[i] : (state)->len_b[0])
#define LENGTH_B(state, i)                                                     \
    ((i) < ARM_NODES ? (state)->len_b[i] : bits_as_float((state)->ang_a[0].x))

/* the Float whose bits word holds */
static Float bits_as_float(Sint32 word)
{
    union {
        Sint32 word;
        Float value;
    } bits;

    bits.word = word;
    return bits.value;
}
#else
#define LENGTH_A(state, i) ((state)->len_a[i])
#define LENGTH_B(state, i) ((state)->len_b[i])
#endif

/* bend each arm a step toward its aim, as LEAD_STEPS and ARM_TURN say. of
 * the nodes past the root, only the one inside the outermost grown node
 * bends, about z. where that is node 1, it also lifts, about y.
 * CHAOS_NORMAL_REVERSE, drawn upside down, bends the other way */
static void arms_aim(struct task_work *work, struct enemy_work *enemy,
                     struct chaos_state *state)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Angle ang;
    /* @hack spelling: turn, a copy of the bend's aim, where the aim passed
     * in place would do */
    Angle turn;
    Angle joint_ang;
    Angle player_ang;
    /* @hack spelling: scale, a local for LEAD_STEPS, where LEAD_STEPS written
     * in place would do */
    Float scale;
    Float dx;
    Float dz;
    Float inv;
    int player;
    /* @hack type: i, a Sint32, where int would do */
    Sint32 i;

    player = player_nearest(&work->pos);
    if (player != 0 && player != 1) {
        return;
    }
    pos = player_work[player]->collision->shapes->center;
    scale = LEAD_STEPS;
    spd = player_motion[player]->spd;
    spd.x *= scale;
    spd.y *= scale;
    spd.z *= scale;
    njAddVector(&pos, &spd);

    /* @bug memory: the loop reads len_a[8], one past the array, at i = 6 and
     * 7. that word is len_b[0], so node 6 bends only while arm b's root has
     * no length (FMOV.S @R2,FR2 at 0xe8e and FMOV.S @R3,FR2 at 0xeae of the
     * object's code) */
    for (i = 1; i < ARM_NODES; i++) {
        /* @hack spelling: the loop's break in an else, where a break on a
         * node of no length first would do */
        if (LENGTH_A(state, i + 1) > 0.0F) {
            if (i != ARM_NODES - 1 && !(LENGTH_A(state, i + 2) > 0.0F)) {
                joint_ang =
                    (Angle)(atan2f(
                                state->joint_a[i + 1].x - state->joint_a[i].x,
                                state->joint_a[i + 1].z - state->joint_a[i].z) *
                            ANGLE_PER_RADIAN);
                player_ang = (Angle)(atan2f(pos.x - state->joint_a[i].x,
                                            pos.z - state->joint_a[i].z) *
                                     ANGLE_PER_RADIAN);
                ang = joint_ang - player_ang;
                if (work->submode == CHAOS_NORMAL_REVERSE) {
                    ang = -ang;
                }
                turn = ang;
                state->ang_a[i].z =
                    angle_approach(state->ang_a[i].z, turn, ARM_TURN);
                if (angle_difference(0, state->ang_a[i].z) > ARM_BEND) {
                    state->ang_a[i].z =
                        angle_approach(0, state->ang_a[i].z, ARM_BEND);
                    /* @hack redundant: a continue where nothing follows in
                     * the loop's body */
                    continue;
                } else if (angle_difference(0, state->ang_a[i].z) < -ARM_BEND) {
                    state->ang_a[i].z =
                        angle_approach(0, state->ang_a[i].z, -ARM_BEND);
                    /* @hack redundant: a continue where nothing follows in
                     * the loop's body */
                    continue;
                }
            }
        } else {
            break;
        }
    }
    if (state->len_a[2] > 0.0F && !(state->len_a[3] > 0.0F)) {
        dx = pos.x - state->joint_a[1].x;
        dz = pos.z - state->joint_a[1].z;
        inv = njInvertSqrt((dx * dx) + (dz * dz));
        if (work->submode == CHAOS_NORMAL_REVERSE) {
            /* @hack spelling: inv times -1, where its negation would do */
            inv *= -1.0F;
        }
        ang = (Angle)(asinf((pos.y - state->joint_a[1].y) * inv) *
                      ANGLE_PER_RADIAN);
        state->ang_a[1].y = angle_approach(state->ang_a[1].y, ang, ARM_TURN);
        if (angle_difference(0, state->ang_a[1].y) > ARM_LIFT) {
            state->ang_a[1].y = angle_approach(0, state->ang_a[1].y, ARM_LIFT);
        } else if (angle_difference(0, state->ang_a[1].y) < -ARM_LIFT) {
            state->ang_a[1].y = angle_approach(0, state->ang_a[1].y, -ARM_LIFT);
        }
    }

    /* @bug memory: the loop reads len_b[8], one past the array, at i = 6 and
     * 7. that word is ang_a[0].x, an Angle, read as a Float (FMOV.S @R3,FR2
     * at 0x10ee and 0x110e of the object's code) */
    for (i = 1; i < ARM_NODES; i++) {
        /* @hack spelling: the loop's break in an else, where a break on a
         * node of no length first would do */
        if (LENGTH_B(state, i + 1) > 0.0F) {
            if (i != ARM_NODES - 1 && !(LENGTH_B(state, i + 2) > 0.0F)) {
                joint_ang =
                    (Angle)(atan2f(
                                state->joint_b[i + 1].x - state->joint_b[i].x,
                                state->joint_b[i + 1].z - state->joint_b[i].z) *
                            ANGLE_PER_RADIAN);
                player_ang = (Angle)(atan2f(pos.x - state->joint_b[i].x,
                                            pos.z - state->joint_b[i].z) *
                                     ANGLE_PER_RADIAN);
                ang = joint_ang - player_ang;
                if (work->submode == CHAOS_NORMAL_REVERSE) {
                    ang = -ang;
                }
                turn = -ang;
                state->ang_b[i].z =
                    angle_approach(state->ang_b[i].z, turn, ARM_TURN);
                if (angle_difference(0, state->ang_b[i].z) > ARM_BEND) {
                    state->ang_b[i].z =
                        angle_approach(0, state->ang_b[i].z, ARM_BEND);
                    /* @hack redundant: a continue where nothing follows in
                     * the loop's body */
                    continue;
                } else if (angle_difference(0, state->ang_b[i].z) < -ARM_BEND) {
                    state->ang_b[i].z =
                        angle_approach(0, state->ang_b[i].z, -ARM_BEND);
                    /* @hack redundant: a continue where nothing follows in
                     * the loop's body */
                    continue;
                }
            }
        } else {
            break;
        }
    }
    if (state->len_b[2] > 0.0F && !(state->len_b[3] > 0.0F)) {
        dx = pos.x - state->joint_b[1].x;
        dz = pos.z - state->joint_b[1].z;
        inv = njInvertSqrt((dx * dx) + (dz * dz));
        if (work->submode == CHAOS_NORMAL_REVERSE) {
            /* @hack spelling: inv times -1, where its negation would do */
            inv *= -1.0F;
        }
        ang = -(Angle)(asinf((pos.y - state->joint_b[1].y) * inv) *
                       ANGLE_PER_RADIAN);
        state->ang_b[1].y = angle_approach(state->ang_b[1].y, ang, ARM_TURN);
        if (angle_difference(0, state->ang_b[1].y) > ARM_LIFT) {
            state->ang_b[1].y = angle_approach(0, state->ang_b[1].y, ARM_LIFT);
        } else if (angle_difference(0, state->ang_b[1].y) < -ARM_LIFT) {
            state->ang_b[1].y = angle_approach(0, state->ang_b[1].y, -ARM_LIFT);
        }
    }
}

static void chaos_point_to_world(struct task_work *work, NJS_POINT3 *point);
static void chaos_destroy(struct task *task);
static void chaos_draw(struct task *task);

static void chaos_init(struct task *task, struct task_work *work)
{
    struct chaos_state *state;
    struct enemy_work *enemy;
    Uint8 *build;
    Uint32 size;
    int i;
    NJS_VECTOR muzzle;
    /* @hack spelling: texid_a and texid_b, locals for the strips' texture
     * words, where the words written in place would do */
    Sint16 *texid_a;
    Sint16 *texid_b;
    Sint16 texid;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((state = syCalloc(1, sizeof(struct chaos_state))) == NULL) {
        return;
    }
    task->extra.p = state;
    enemy = enemy_work_alloc_grouped(task);
    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= CHAOS_KINDS || work->submode < 0) {
        work->submode = CHAOS_NORMAL;
    }
    switch (work->submode) {
    case CHAOS_NORMAL:
    case CHAOS_GUARD:
        collision_init(task, chaos_shapes, ARMED_SHAPES, COLLISION_ENEMY);
        collision_set_radius(task, ARMED_RADIUS);
        INTERVAL(enemy) = work->ang.z;
        break;
    case CHAOS_NORMAL_REVERSE:
        collision_init(task, chaos_shapes_reverse, ARMED_SHAPES,
                       COLLISION_ENEMY);
        collision_set_radius(task, ARMED_RADIUS);
        INTERVAL(enemy) = work->ang.z;
        break;
    case CHAOS_ROUND:
    case CHAOS_ROUND_PARALLEL:
        collision_init(task, chaos_shapes, BODY_SHAPES, COLLISION_ENEMY);
        INTERVAL(enemy) = work->ang.z;
        SPIN_SPEED(enemy) =
            ((work->ang.x & HIGH_BYTE) >> HIGH_BYTE_SHIFT) * SPIN_UNIT;
        break;
    case CHAOS_ROUND_REVERSE:
    case CHAOS_ROUND_REVERSE_PARALLEL:
        collision_init(task, chaos_shapes_round_reverse, BODY_SHAPES,
                       COLLISION_ENEMY);
        INTERVAL(enemy) = work->ang.z;
        SPIN_SPEED(enemy) =
            ((work->ang.x & HIGH_BYTE) >> HIGH_BYTE_SHIFT) * SPIN_UNIT;
        break;
    case CHAOS_FLOAT:
        collision_init(task, chaos_shapes_float, ARMED_SHAPES, COLLISION_ENEMY);
        collision_set_radius(task, ARMED_RADIUS);
        INTERVAL(enemy) = work->ang.z;
        BOB_PHASE(enemy) = work->ang.x & HIGH_BYTE;
        break;
    case CHAOS_SHEDDING:
    case CHAOS_SHEDDING_RED:
        collision_init(task, chaos_shapes_float, BODY_SHAPES, COLLISION_ENEMY);
        SHED_COUNT(enemy) = njFraction(work->scl.y) * SHED_SCALE;
        state->x_max = work->pos.x +
                       (BOX_UNIT * (work->ang.x >> HIGH_BYTE_SHIFT)) +
                       (BOX_UNIT * (work->ang.x & LOW_BYTE));
        state->x_min = work->pos.x +
                       (BOX_UNIT * (work->ang.x >> HIGH_BYTE_SHIFT)) -
                       ((work->ang.x & LOW_BYTE) * BOX_UNIT);
        state->z_max = work->pos.z +
                       (BOX_UNIT * (work->ang.z >> HIGH_BYTE_SHIFT)) +
                       (BOX_UNIT * (work->ang.z & LOW_BYTE));
        state->z_min = work->pos.z +
                       (BOX_UNIT * (work->ang.z >> HIGH_BYTE_SHIFT)) -
                       ((work->ang.z & LOW_BYTE) * BOX_UNIT);
        break;
    }
    /* @bug memory: chaos_init ignores collision_init's result. where the
     * heap has no room for the record, later steps read it through null.
     * only arms_fit_shapes and arms_shapes_off test it, at 0x40a and 0xbea of
     * the object's code. chaos_turn reads through null at 0x1b32,
     * chaos_check_hit at 0x1d54, chaos_guard at 0x25e8 and chaos_float at
     * 0x2964 */
#ifdef NONMATCHING
    /* this side repairs the tag above: without a record, the chaos ends on its
     * next step, and its placement makes it again while in range */
    if (work->collision == NULL) {
        task->destructor = chaos_destroy;
        task_kill(task);
        return;
    }
#endif
    LASER_LENGTH(enemy) = work->ang.x;
    LASER_SPEED(enemy) = work->scl.y;
    switch (work->submode) {
    case CHAOS_NORMAL:
    case CHAOS_ROUND:
    case CHAOS_ROUND_REVERSE:
    case CHAOS_ROUND_PARALLEL:
    case CHAOS_ROUND_REVERSE_PARALLEL:
    case CHAOS_NORMAL_REVERSE:
        enemy->motion.object = normal_object;
        state->nodes_a = normal_arm_a.child;
        state->nodes_b = normal_arm_b.child;
        enemy->motion.next = ACTION_IDLE;
        work->mode = MODE_WAIT;
        break;
    case CHAOS_FLOAT:
        enemy->motion.object = float_object;
        state->nodes_a = float_arm_a->child;
        state->nodes_b = float_arm_b->child;
        enemy->motion.next = ACTION_FLOAT;
        work->mode = MODE_FLOAT;
        break;
    case CHAOS_SHEDDING:
    case CHAOS_SHEDDING_RED:
        if (placement_get_state(task) == 0) {
            enemy->motion.object = float_object;
            enemy->motion.next = ACTION_FLOAT;
            work->mode = MODE_FLOAT;
        } else {
            enemy->motion.object = &spent_object;
            enemy->motion.next = ACTION_SPENT;
            work->mode = MODE_SHED;
            SHED_COUNT(enemy) = 0;
        }
        break;
    case CHAOS_GUARD:
        enemy->motion.object = normal_object;
        state->nodes_a = normal_arm_a.child;
        state->nodes_b = normal_arm_b.child;
        enemy->motion.next = ACTION_SHUT;
        work->mode = MODE_GUARD;
        break;
    }
    work->ang.x = 0;
    work->ang.z = 0;
    switch (work->submode) {
    case CHAOS_ROUND:
    case CHAOS_ROUND_PARALLEL:
    case CHAOS_SHEDDING:
    case CHAOS_SHEDDING_RED:
        enemy_set_offset(work, enemy);
        break;
    case CHAOS_ROUND_REVERSE:
    case CHAOS_ROUND_REVERSE_PARALLEL:
        enemy_set_offset_behind(work, enemy);
        break;
    }
    enemy->sight_distance2 = RANGE * RANGE;
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->turn_speed = TURN_SPEED;
    enemy_set_home(work, enemy);
    if (work->submode == CHAOS_SHEDDING ||
        work->submode == CHAOS_SHEDDING_RED) {
        work->pos.y += njRoundOff(work->scl.y);
    } else {
        work->scl.y = 0.0F;
    }
    enemy->motion.table = chaos_actions;
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    muzzle.x = 0.0F;
    muzzle.y = BODY_HEIGHT;
    muzzle.z = 0.0F;
    chaos_point_to_world(work, &muzzle);
    state->muzzle_a = muzzle;
    state->muzzle_b = muzzle;
    enemy->flags |= ENEMY_BURST;
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy_laser_load();
    switch (work->submode) {
    case CHAOS_NORMAL:
    case CHAOS_NORMAL_REVERSE:
    case CHAOS_FLOAT:
    case CHAOS_GUARD:
        arms_reset(work, state);
        break;
    case CHAOS_SHEDDING:
    case CHAOS_SHEDDING_RED:
        blast_load_texture();
        break;
    }
    task->draw = chaos_draw;
    task->destructor = chaos_destroy;
    if (chaos_textures == NULL) {
        texture_load_pvm("E_CHAOSTEX", &chaos_texlist);
        chaos_textures = task_create(0, TEXTURE_LEVEL, textures_step, "ManTex");
        chaos_textures->destructor = chaos_textures_destroy;
    }
    if (piece_builds != NULL) {
        return;
    }
    texid_b = PIECE_B_WATER_TEXTURE;
    texid_a = PIECE_A_WATER_TEXTURE;
    /* @hack spelling: the builds made in a do-while(0) left by a break, where
     * an early return would do */
    do {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        /* @hack spelling: the step named through its second name, where its
         * own name would do */
        if ((piece_builds = task_create(0, TEXTURE_LEVEL, builds_step_outside,
                                        "ManMdl")) == NULL) {
            break;
        }
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&chaos_texlist);
        light_apply(LIGHT_EFFECT);
        njSetDirectClip();
        size = njCnkDirectObjectCompileSize(&piece_a_object);
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((piece_builds->extra.p = syCalloc(FRAME_TEXTURES, size)) == NULL) {
            task_kill(piece_builds);
            piece_builds = NULL;
        } else {
            build = piece_builds->extra.p;
            piece_builds->unk_3c.l = size;
            /* @hack spelling: the destructor named through its second name,
             * where its own name would do */
            piece_builds->destructor = piece_builds_destroy_outside;
            /* @hack spelling: i = i + 1, where i++ would do */
            for (i = TEXTURE_FIRST; i < TEXTURE_FIRST + FRAME_TEXTURES;
                 i = i + 1) {
                /* @hack spelling: the texture word through texid, in three
                 * statements, where one would do */
                texid = *texid_a;
                texid = texid & ~TEXID_BITS;
                *texid_a = texid | i;
                njCnkDirectObjectCompile(&piece_a_object, build);
                /* @hack spelling: build = build + size, where build += size
                 * would do */
                build = build + size;
            }
        }
        size = njCnkDirectObjectCompileSize(&piece_b_object);
        /* @bug memory: with no room for piece_a_object's builds, piece_builds
         * is null here. the allocation of piece_b_object's builds then stores
         * its result through null (MOV.L R0,@R3 at 0x194c of the object's
         * code) */
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
#ifdef NONMATCHING
        /* this side repairs the tag above: without the builds task, it makes
         * no builds of piece_b_object */
        if (piece_builds != NULL &&
            (piece_builds->motion.p = syCalloc(FRAME_TEXTURES, size)) != NULL) {
#else
        if ((piece_builds->motion.p = syCalloc(FRAME_TEXTURES, size)) != NULL) {
#endif
            build = piece_builds->motion.p;
            piece_builds->force.l = size;
            /* @hack spelling: i = i + 1, where i++ would do */
            for (i = TEXTURE_FIRST; i < TEXTURE_FIRST + FRAME_TEXTURES;
                 i = i + 1) {
                /* @hack spelling: the texture word through texid, in three
                 * statements, where one would do */
                texid = *texid_b;
                texid = texid & ~TEXID_BITS;
                *texid_b = texid | i;
                njCnkDirectObjectCompile(&piece_b_object, build);
                /* @hack spelling: build = build + size, where build += size
                 * would do */
                build = build + size;
            }
        }
        draw_control_clear(DRAW_CONTROL_SHADOW);
        light_apply(light_after_draw);
    } while (0);
}

/* aim the chaos at a random point near home, as HOME_SPREAD says */
static void chaos_aim_near_home(struct task_work *work,
                                struct enemy_work *enemy)
{
    Float range;

    range = njSqrt(enemy->near_distance2);
    enemy->target.x =
        enemy->home.x + (range * ((rand() * RAND_SCALE) - 0.5F) * HOME_SPREAD);
    enemy->target.y = enemy->home.y;
    enemy->target.z =
        enemy->home.z + (range * ((rand() * RAND_SCALE) - 0.5F) * HOME_SPREAD);
    enemy->aim_y = (Angle)(atan2f(enemy->target.x - work->pos.x,
                                  enemy->target.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
}

/* turn the chaos a step. under CHAOS_HOMEWARD it turns toward home, and while a
 * player is near, toward the nearest player. it turns toward its aim near home
 * otherwise. it picks a new aim as TURN_STEPS says, or on touching anything but
 * a player */
static void chaos_turn(struct task_work *work, struct enemy_work *enemy,
                       struct chaos_state *state)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & CHAOS_HOMEWARD) {
        enemy_turn_to_home(work, enemy);
    } else if (work->flags & CHAOS_PLAYER_NEAR) {
        player = player_nearest(&work->pos);
        if (player != 0 && player != 1) {
            return;
        }
        enemy_turn_to_player(work, enemy, player);
    } else if (state->turn_timer-- < 0 ||
               (work->collision->flags & COLLISION_TOUCHED &&
                work->collision->hit->kind != COLLISION_PLAYER)) {
        chaos_aim_near_home(work, enemy);
        state->turn_timer = (rand() * (TURN_SPREAD * RAND_SCALE)) + TURN_STEPS;
    } else {
        work->ang.y =
            angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
    }
}

/* set CHAOS_PLAYER_NEAR where enemy_find_player_near_offset finds a player,
 * and clear it otherwise. a shedding kind also needs some player near in y, as
 * WAKE_HEIGHTS says. while game_state.time_stopped is set, the flag stays */
static void chaos_mark_player_near_offset(struct task_work *work,
                                          struct enemy_work *enemy)
{
    /* @hack spelling: slot, a pointer that walks player_work and is read in
     * the test, where an index would do */
    struct task_work **slot;
    struct task_work *player;
    Float height;
    Float nearest;

    if (game_state.time_stopped) {
        return;
    }
    if (work->submode == CHAOS_SHEDDING ||
        work->submode == CHAOS_SHEDDING_RED) {
        nearest = FAR_AWAY;
        for (slot = &player_work[0]; slot < &player_work[2];) {
            if ((player = *slot++) != NULL) {
                height = njAbs(work->pos.y - player->pos.y);
                if (height < nearest) {
                    nearest = height;
                }
            }
        }
        if (nearest > njAbs(work->pos.y - enemy->home.y) * WAKE_HEIGHTS) {
            work->flags &= ~CHAOS_PLAYER_NEAR;
            return;
        }
    }
    if (enemy_find_player_near_offset(work, enemy)) {
        work->flags |= CHAOS_PLAYER_NEAR;
    } else {
        work->flags &= ~CHAOS_PLAYER_NEAR;
    }
}

/* beat the chaos where enemy_take_hit finds a hit. make it a ball where a
 * magic hand took it. while a walker's lock holds CHAOS_GUARD in its shell, a
 * hit beats it only where collision_hurt returns TRUE. a hit that doesn't ends
 * the call. CHAOS_FLOAT, and a chaos in MODE_GUARD, then throw back a player
 * that touches LOWER_SHAPE. while game_state.time_stopped is set, a hit beats
 * any chaos, and makes no ball */
static void chaos_check_hit(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    int score;
    int player;
    struct task *toucher;

    if (enemy_take_hit(work, enemy)) {
        /* @hack spelling: the empty then-arm and its else, where the test
         * negated would do */
        if (game_state.time_stopped) {
        } else {
            if (enemy->flags & ENEMY_IN_MAGIC_HAND) {

                /* @hack spelling: offset and scl declared in the block that
                 * uses them, where the function's head would do */
                NJS_VECTOR offset;
                NJS_VECTOR scl;

                /* @hack spelling: player set to shot_by, where shot_by
                 * passed at each use would do */
                player = enemy->shot_by;
                offset.x = 0.0F;
                offset.y = BALL_OFFSET_Y;
                offset.z = 0.0F;
                /* @hack spelling: one assignment to the three parts, where a
                 * statement for each would do */
                scl.x = scl.y = scl.z = BALL_SCALE;
                switch (work->submode) {
                case CHAOS_NORMAL:
                case CHAOS_ROUND:
                case CHAOS_ROUND_REVERSE:
                case CHAOS_ROUND_PARALLEL:
                case CHAOS_ROUND_REVERSE_PARALLEL:
                case CHAOS_NORMAL_REVERSE:
                    enemy_ball_create(player, normal_object, &chaos_texlist,
                                      &scl, &offset);
                    break;
                case CHAOS_FLOAT:
                case CHAOS_SHEDDING:
                case CHAOS_SHEDDING_RED:
                    enemy_ball_create(player, float_object, &chaos_texlist,
                                      &scl, &offset);
                    break;
                case CHAOS_GUARD:
                    enemy_ball_create(player, &guard_object, &chaos_texlist,
                                      &scl, &offset);
                    break;
                }
            } else if (work->id == WORK_ID_LOCKON_GUARD &&
                       enemy->lock_task != NULL && !collision_hurt(task)) {
                work->flags &= ~WORK_HURT;
                enemy->flags &= ~ENEMY_HIT;
                return;
            }
        }
        work->mode = MODE_DEFEATED;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        enemy_break(work, chaos_pieces, &chaos_texlist);
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
    }
    if (work->submode == CHAOS_FLOAT || work->mode == MODE_GUARD) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((toucher = collision_next_player_by_shape(task, LOWER_SHAPE)) !=
            NULL) {

            /* @hack spelling: spd and ang declared in the block that uses
             * them, where the function's head would do */
            NJS_VECTOR spd;
            struct angles ang;

            player = player_slot_of(toucher);
            spd = work->pos;
            njSubVector(&spd, &player_work[player]->pos);
            ang.x = 0;
            ang.y = (Angle)(atan2f(spd.x, spd.z) * ANGLE_PER_RADIAN);
            ang.z = 0;
            spd.x = THROW_SPEED;
            spd.y = THROW_LIFT;
            spd.z = 0.0F;
            player_launch_speed(player, &spd, &ang, 0);
            sound_play_at(SOUND_BOUNCE, NULL, 0, SOUND_LOUDEST, &work->pos);
            vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
        }
    }
}

static void chaos_mark_player_near_home(struct task_work *work,
                                        struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= CHAOS_PLAYER_NEAR;
    } else {
        work->flags &= ~CHAOS_PLAYER_NEAR;
    }
}

/* set a floating chaos's height: home's, plus scl.y rounded to a whole
 * number, plus its bob. the bob's phase runs with game_state.steps */
static void chaos_bob(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: steps, a local for game_state.steps, where the field
     * read in place would do */
    Uint32 steps;
    Float sine;

    steps = game_state.steps;
    sine = njSin((steps * BOB_SPEED) + BOB_PHASE(enemy));
    work->pos.y = enemy->home.y + njRoundOff(work->scl.y) + (sine * BOB_HEIGHT);
}

/* move point from the chaos's own axes, turned by its heading, into the
 * world's */
static void chaos_point_to_world(struct task_work *work, NJS_POINT3 *point)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, point, point);
    njPopMatrixEx();
}

/* turn dir, a shot's velocity along x, to aim from pos at the nearest
 * player's first collision shape. AIM_UP says how far it turns. where the
 * shape's center lies at pos, the shot goes level along the heading */
/* @hack redundant: state, a parameter nothing reads */
static void chaos_aim_up(struct task_work *work, struct chaos_state *state,
                         NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    Float distance;
    /* @hack spelling: rise, a local for the target's height over pos, where
     * the difference written in place would do */
    Float rise;
    Angle ang_y;
    Angle ang_z;
    Angle turn_z;
    Angle turn_y;
    NJS_POINT3 target;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    rise = target.y - pos->y;
    if (distance != 0.0F) {
        ang_z = (Angle)(asinf(rise / distance) * ANGLE_PER_RADIAN);
        ang_y = (Angle)(atan2f(pos->z - target.z, target.x - pos->x) *
                        ANGLE_PER_RADIAN);
    } else {
        ang_y = work->ang.y;
        ang_z = 0;
    }
    turn_z = angle_approach(AIM_UP, ang_z, AIM_SPREAD);
    turn_y = angle_approach(work->ang.y, ang_y, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, turn_y);
    njRotateZ(NULL, turn_z);
    njCalcVector(NULL, dir, dir);
    njPopMatrixEx();
}

/* aim a shot as chaos_aim_up does, but as AIM_DOWN says. where the shape's
 * center lies at pos, the shot goes along the heading. it pitches as near
 * level as AIM_SPREAD lets it */
/* @hack redundant: state, a parameter nothing reads */
static void chaos_aim_down(struct task_work *work, struct chaos_state *state,
                           NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    Float distance;
    /* @hack spelling: rise, a local for the target's height over pos, where
     * the difference written in place would do */
    Float rise;
    Angle ang_z;
    Angle ang_y;
    Angle turn_z;
    Angle turn_y;
    NJS_POINT3 target;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    rise = target.y - pos->y;
    if (distance != 0.0F) {
        ang_z = (Angle)(asinf(rise / distance) * ANGLE_PER_RADIAN);
        ang_y = (Angle)(atan2f(pos->z - target.z, target.x - pos->x) *
                        ANGLE_PER_RADIAN);
    } else {
        ang_y = work->ang.y;
        ang_z = 0;
    }
    turn_z = angle_approach(AIM_DOWN, ang_z, AIM_SPREAD);
    turn_y = angle_approach(work->ang.y, ang_y, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, turn_y);
    njRotateZ(NULL, turn_z);
    njCalcVector(NULL, dir, dir);
    njPopMatrixEx();
}

/* aim a round kind's shot the way its body faces, which turns with its
 * motion's frame. a round reverse kind faces half a turn away, and turns the
 * other way. CHAOS_ROUND and CHAOS_ROUND_REVERSE pitch the shot toward the
 * nearest player, by EIGHTH_TURN at most. the parallel kinds fire level */
static void chaos_aim_spin(struct task_work *work, struct enemy_work *enemy,
                           NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    Float distance;
    /* @hack spelling: rise, a local for the target's height over pos, where
     * the difference written in place would do */
    Float rise;
    Angle ang_z;
    Angle ang_y;
    Angle turn_z;
    NJS_POINT3 target;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    rise = target.y - pos->y;
    if (distance != 0.0F && (work->submode == CHAOS_ROUND ||
                             work->submode == CHAOS_ROUND_REVERSE)) {
        ang_z = (Angle)(asinf(rise / distance) * ANGLE_PER_RADIAN);
    } else {
        ang_z = 0;
    }
    if (work->submode == CHAOS_ROUND_REVERSE ||
        work->submode == CHAOS_ROUND_REVERSE_PARALLEL) {
        ang_y = (work->ang.y + HALF_TURN) -
                (enemy->motion.frame * (FULL_TURN / SPIN_FRAMES));
    } else {
        ang_y = work->ang.y + (enemy->motion.frame * (FULL_TURN / SPIN_FRAMES));
    }
    turn_z = angle_approach(0, ang_z, EIGHTH_TURN);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, ang_y);
    njRotateZ(NULL, turn_z);
    njCalcVector(NULL, dir, dir);
    njPopMatrixEx();
}

/* fire a laser from each muzzle. a round kind aims as it spins. the others
 * fire down in ACTION_REACH_FIRE_DOWN, and up in any other action.
 * CHAOS_NORMAL_REVERSE, drawn upside down, swaps the two */
static void chaos_fire(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    struct chaos_state *state;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    state = task->extra.p;
    pos = state->muzzle_a;
    dir.x = LASER_SPEED(enemy);
    dir.y = 0.0F;
    dir.z = 0.0F;
    if (work->mode == MODE_SPIN) {
        chaos_aim_spin(work, enemy, &pos, &dir);
    } else if ((enemy->motion.action != ACTION_REACH_FIRE_DOWN &&
                work->submode != CHAOS_NORMAL_REVERSE) ||
               (enemy->motion.action == ACTION_REACH_FIRE_DOWN &&
                work->submode == CHAOS_NORMAL_REVERSE)) {
        chaos_aim_up(work, state, &pos, &dir);
    } else {
        chaos_aim_down(work, state, &pos, &dir);
    }
    enemy_laser_fire_charged(task, LASER_LENGTH(enemy), &dir, &pos);
    pos = state->muzzle_b;
    dir.x = LASER_SPEED(enemy);
    dir.y = 0.0F;
    dir.z = 0.0F;
    if (work->mode == MODE_SPIN) {
        chaos_aim_spin(work, enemy, &pos, &dir);
    } else if ((enemy->motion.action != ACTION_REACH_FIRE_DOWN &&
                work->submode != CHAOS_NORMAL_REVERSE) ||
               (enemy->motion.action == ACTION_REACH_FIRE_DOWN &&
                work->submode == CHAOS_NORMAL_REVERSE)) {
        chaos_aim_up(work, state, &pos, &dir);
    } else {
        chaos_aim_down(work, state, &pos, &dir);
    }
    enemy_laser_fire_charged(task, LASER_LENGTH(enemy), &dir, &pos);
}

static void chaos_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* the step of MODE_WAIT. once a player is near, the normal kinds and
 * CHAOS_GUARD go to MODE_WANDER, whose first step reaches. the round kinds
 * spin. while no player is near, CHAOS_GUARD goes into its shell */
static void chaos_wait(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    if (work->submode == CHAOS_NORMAL ||
        work->submode == CHAOS_NORMAL_REVERSE || work->submode == CHAOS_GUARD) {
        chaos_mark_player_near_home(work, enemy);
    } else {
        chaos_mark_player_near_offset(work, enemy);
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & CHAOS_PLAYER_NEAR) {
        switch (work->submode) {
        case CHAOS_NORMAL:
        case CHAOS_NORMAL_REVERSE:
        case CHAOS_GUARD:
            work->mode = MODE_WANDER;
            break;
        case CHAOS_ROUND:
        case CHAOS_ROUND_REVERSE:
        case CHAOS_ROUND_PARALLEL:
        case CHAOS_ROUND_REVERSE_PARALLEL:
            work->mode = MODE_SPIN;
            enemy->motion.next = ACTION_SPIN;
            break;
        }
        work->timer = INTERVAL(enemy);
    } else if (work->submode == CHAOS_GUARD) {
        work->mode = MODE_GUARD;
        enemy->motion.next = ACTION_GUARD;
    }
    chaos_check_hit(task, work, enemy);
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

/* the step of MODE_GUARD. the shell opens while a player is near, as
 * SHELL_STEPS says, and shuts otherwise. once the shell is open, the chaos
 * goes to MODE_WANDER, whose first step reaches */
static void chaos_guard(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    struct collision_shape *shape;

    chaos_mark_player_near_home(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & CHAOS_PLAYER_NEAR &&
        (enemy->lock_task == NULL || work->timer > SHELL_STEPS)) {
        if (enemy->motion.action == ACTION_OPENING) {
            if (enemy->motion.flags & ENEMY_MOTION_ENDED) {
                work->mode = MODE_WANDER;
                enemy->motion.next = ACTION_IDLE;
                work->timer = INTERVAL(enemy);
            }
        } else {
            enemy->motion.next = ACTION_OPENING;
        }
    } else {
        if (enemy->motion.action == ACTION_GUARD) {
            if (enemy->motion.flags & ENEMY_MOTION_ENDED) {
                enemy->motion.next = ACTION_SHUTTING;
            }
        } else if (enemy->motion.action != ACTION_SHUT &&
                   enemy->motion.action != ACTION_SHUTTING) {
            enemy->motion.next = ACTION_SHUTTING;
        }
        if (!(work->flags & CHAOS_PLAYER_NEAR)) {
            work->timer = 0;
        }
    }
    shape = work->collision->shapes;
    switch (enemy->motion.next) {
    case ACTION_SHUTTING:
    case ACTION_OPENING:
        if (enemy->motion.frame < 1.0F) {
            shape->levels =
                (shape->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(0);
        } else {
            shape->levels =
                (shape->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(SHELL_GUARD);
        }
        shape->center.y = BODY_HEIGHT - (enemy->motion.frame * SHELL_SINK);
        work->id = WORK_ID_LOCKON_GUARD;
        break;
    case ACTION_SHUT:
        shape->levels =
            (shape->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(SHELL_GUARD);
        shape->center.y = BODY_HEIGHT - (SHELL_FRAMES * SHELL_SINK);
        work->id = WORK_ID_LOCKON_GUARD;
        break;
    case ACTION_GUARD:
    default:
        shape->levels = (shape->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(0);
        shape->center.y = BODY_HEIGHT;
        work->id = WORK_ID_NONE;
        break;
    }
    chaos_check_hit(task, work, enemy);
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

/* the step of MODE_SPIN. the chaos charges and fires as CHARGE_FULL says.
 * between shots, it goes back to MODE_WAIT where its body ends a turn with
 * no player near */
static void chaos_spin(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    /* @hack redundant: state, a local read from task and not used */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct chaos_state *state;
    int wrapped;

    state = task->extra.p;
    wrapped = 0;
    chaos_mark_player_near_offset(work, enemy);
    chaos_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED || game_state.time_stopped) {
        return;
    }
    enemy->motion.frame += SPIN_SPEED(enemy) * SPIN_FRAMES;
    while (enemy->motion.frame < 0.0F) {
        enemy->motion.frame += SPIN_FRAMES;
        wrapped = 1;
    }
    while (enemy->motion.frame > SPIN_FRAMES) {
        enemy->motion.frame -= SPIN_FRAMES;
        wrapped = 1;
    }
    if (work->timer > INTERVAL(enemy)) {
        if (CHARGE(enemy) == 0.0F) {
            CHARGE(enemy) = CHARGE_STEP;
        } else {
            CHARGE(enemy) += CHARGE_STEP;
            if (CHARGE(enemy) > CHARGE_FULL) {
                chaos_fire(task, work, enemy);
                work->timer = 0;
                CHARGE(enemy) = 0.0F;
            }
        }
    } else {
        work->timer++;
        if (!(work->flags & CHAOS_PLAYER_NEAR) && wrapped) {
            work->mode = MODE_WAIT;
            work->timer = 0;
            enemy->motion.next = ACTION_IDLE;
        }
    }
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

/* the step of MODE_FLOAT_WANDER: chaos_wander's, for CHAOS_FLOAT, which
 * bobs as it goes. its firing reach fires up at any player */
static void chaos_float_wander(struct task *task, struct task_work *work,
                               struct enemy_work *enemy)
{
    struct chaos_state *state;
    int player;

    state = task->extra.p;
    if (work->submode == CHAOS_FLOAT) {
        chaos_mark_player_near_home(work, enemy);
    } else {
        chaos_mark_player_near_offset(work, enemy);
    }
    chaos_turn(work, enemy, state);
    chaos_bob(work, enemy);
    chaos_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        arms_shapes_off(work, state);
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > INTERVAL(enemy)) {
        work->mode = MODE_FLOAT_REACH;
        work->timer = 0;
        arms_reset(work, state);
        player = player_nearest(&work->pos);
        /* @bug logic: player is never -1, so this test always holds
         * (CMP/EQ #-1,R0 at 0x2874 of the object's code) */
        if (player != -1 &&
            njDistanceP2P(&work->pos, &player_work[player]->pos) < FIRE_RANGE) {
            /* @hack redundant: a test whose two arms do the same */
            if (player_work[player]->flags & FIRE_DOWN_FLAGS) {
                enemy->motion.next = ACTION_FLOAT_REACH_FIRE;
            } else {
                enemy->motion.next = ACTION_FLOAT_REACH_FIRE;
            }
        } else {
            enemy->motion.next = ACTION_FLOAT_REACH;
        }
    }
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

/* the step of MODE_FLOAT. once a player is near, CHAOS_FLOAT wanders.
 * a shedding kind starts to shed, and its placement remembers the shed */
static void chaos_float(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    if (work->submode == CHAOS_FLOAT) {
        chaos_mark_player_near_home(work, enemy);
    } else {
        chaos_mark_player_near_offset(work, enemy);
    }
    chaos_bob(work, enemy);
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->flags & CHAOS_PLAYER_NEAR) {
        switch (work->submode) {
        case CHAOS_FLOAT:
            work->mode = MODE_FLOAT_WANDER;
            work->timer = INTERVAL(enemy);
            break;
        case CHAOS_SHEDDING:
        case CHAOS_SHEDDING_RED:
            enemy->motion.object = &shed_object;
            enemy->motion.next = ACTION_SHED_START;
            work->mode = MODE_SHED;
            placement_set_state(task, STATE_SHED);
            work->timer = 0;
            work->collision->shapes[LOWER_SHAPE].flags |= SHAPE_OFF;
            sound_play_at(SOUND_SHED, NULL, 0, SOUND_LOUDEST, &work->pos);
            if (!game_state.two_players) {
                vibration_start_fading(0, SHED_VIBRATION_POWER,
                                       SHED_VIBRATION_FREQ,
                                       SHED_VIBRATION_LENGTH);
            }
            break;
        }
    }
    chaos_check_hit(task, work, enemy);
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

/* the step of MODE_FLOAT_REACH: chaos_reach's, for CHAOS_FLOAT, which bobs
 * as it goes */
static void chaos_float_reach(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    struct chaos_state *state;
    int player;

    state = task->extra.p;
    if (work->submode == CHAOS_FLOAT) {
        chaos_mark_player_near_home(work, enemy);
    } else {
        chaos_mark_player_near_offset(work, enemy);
    }
    chaos_bob(work, enemy);
    chaos_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        arms_shapes_off(work, state);
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > REACH_STEPS) {
        arms_retract(state);
        arms_fit_shapes(work, state);
        if (state->len_a[0] <= 0.0F) {
            if (!(work->flags & CHAOS_PLAYER_NEAR)) {
                work->mode = MODE_FLOAT;
            } else {
                work->mode = MODE_FLOAT_WANDER;
            }
            work->timer = 0;
            arms_shapes_off(work, state);
            enemy->motion.next = ACTION_FLOAT;
        }
    } else {
        arms_extend(state);
        if (work->timer < AIM_STEPS) {
            arms_aim(work, enemy, state);
        }
        arms_fit_shapes(work, state);
    }
    if (work->timer < SWITCH_STEPS &&
        enemy->motion.action == ACTION_FLOAT_REACH) {
        player = player_nearest(&work->pos);
        /* @bug logic: player is never -1, so this test always holds
         * (CMP/EQ #-1,R0 at 0x2aec of the object's code) */
        if (player != -1 && !(player_work[player]->flags & FIRE_DOWN_FLAGS) &&
            ((player_records[player]->character != PLAYER_KNUCKLES &&
              player_records[player]->character != PLAYER_ROUGE) ||
             player_work[player]->mode != PLAYER_MODE_HUNTER_GLIDE)) {
            enemy->motion.next = ACTION_FLOAT_REACH_FIRE;
        }
    }
    if (enemy->motion.action == ACTION_FLOAT_REACH_FIRE &&
        enemy->motion.flags & ENEMY_MOTION_ENDING) {
        chaos_fire(task, work, enemy);
    }
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

/* the step of MODE_WANDER. once its timer passes INTERVAL, the chaos
 * reaches, and fires as FIRE_RANGE says. CHAOS_GUARD goes back in its shell
 * as WANDER_STEPS says */
static void chaos_wander(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    struct chaos_state *state;
    int player;

    state = task->extra.p;
    chaos_mark_player_near_home(work, enemy);
    chaos_turn(work, enemy, state);
    chaos_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        arms_shapes_off(work, state);
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > INTERVAL(enemy)) {
        work->mode = MODE_REACH;
        work->timer = 0;
        arms_reset(work, state);
        player = player_nearest(&work->pos);
        /* @bug logic: player is never -1, so this test always holds
         * (CMP/EQ #-1,R0 at 0x2c3c of the object's code) */
        if (player != -1 &&
            njDistanceP2P(&work->pos, &player_work[player]->pos) < FIRE_RANGE) {
            if (player_work[player]->flags & FIRE_DOWN_FLAGS) {
                enemy->motion.next = ACTION_REACH_FIRE_DOWN;
            } else {
                enemy->motion.next = ACTION_REACH_FIRE_UP;
            }
        } else {
            enemy->motion.next = ACTION_REACH;
        }
        /* @hack redundant: a return where nothing follows the if */
        return;
    } else if (enemy->lock_task != NULL && work->submode == CHAOS_GUARD &&
               work->timer > WANDER_STEPS) {
        work->mode = MODE_GUARD;
        enemy->motion.next = ACTION_GUARD;
    }
}

/* the step of MODE_REACH. once the arms are back in, the chaos wanders where a
 * player is near, and waits otherwise */
static void chaos_reach(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    struct chaos_state *state;
    int player;

    state = task->extra.p;
    /* @hack spelling: the call in a do-while(0) left by a break, where the
     * call alone would do */
    do {
        chaos_mark_player_near_home(work, enemy);
        break;
    } while (0);
    chaos_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        arms_shapes_off(work, state);
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > REACH_STEPS) {
        arms_retract(state);
        arms_fit_shapes(work, state);
        if (state->len_a[0] <= 0.0F) {
            if (!(work->flags & CHAOS_PLAYER_NEAR)) {
                work->mode = MODE_WAIT;
            } else {
                work->mode = MODE_WANDER;
            }
            work->timer = 0;
            arms_shapes_off(work, state);
            enemy->motion.next = ACTION_IDLE;
        }
    } else {
        arms_extend(state);
        if (work->timer < AIM_STEPS) {
            arms_aim(work, enemy, state);
        }
        arms_fit_shapes(work, state);
    }
    if (work->timer < SWITCH_STEPS && enemy->motion.action == ACTION_REACH) {
        player = player_nearest(&work->pos);
        /* @bug logic: player is never -1, so this test always holds
         * (CMP/EQ #-1,R0 at 0x2dde of the object's code) */
        if (player != -1 && !(player_work[player]->flags & FIRE_DOWN_FLAGS) &&
            ((player_records[player]->character != PLAYER_KNUCKLES &&
              player_records[player]->character != PLAYER_ROUGE) ||
             player_work[player]->mode != PLAYER_MODE_HUNTER_GLIDE)) {
            enemy->motion.next = ACTION_REACH_FIRE_UP;
        }
    }
    if ((enemy->motion.action == ACTION_REACH_FIRE_UP ||
         enemy->motion.action == ACTION_REACH_FIRE_DOWN) &&
        enemy->motion.flags & ENEMY_MOTION_ENDING) {
        chaos_fire(task, work, enemy);
    }
    sound_play_at_for(SOUND_STEP, (void *)chaos_step, 1, STEP_VOLUME,
                      RENEWED_DURATION, &work->pos);
}

static void piece_create(struct task *parent);

/* the step of MODE_SHED. the chaos sheds a piece a step through ACTION_SHED
 * and ACTION_SHED_END, SHED_COUNT pieces in all */
static void chaos_shed(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    /* @hack redundant: state, a local read from task and not used */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct chaos_state *state;

    state = task->extra.p;
    chaos_bob(work, enemy);
    chaos_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED || game_state.time_stopped) {
        return;
    }
    if (enemy->motion.action == ACTION_SHED ||
        enemy->motion.action == ACTION_SHED_END) {
        if (work->timer < SHED_COUNT(enemy)) {
            piece_create(task);
            work->timer++;
        }
        if (work->timer + SHED_END_LEFT >= SHED_COUNT(enemy)) {
            enemy->motion.next = ACTION_SHED_END;
        }
    }
    sound_play_at_for(SOUND_SHEDDING, work, 1, SOUND_LOUDEST, RENEWED_DURATION,
                      &work->pos);
}

/* an empty function, which nothing calls. it is external, since SHC leaves
 * out a static function its source never names */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void fun_enemy_chaos_2f5c(struct task *task)
{
}

/* chaos_draw's motion callback, which the draw calls at each node. it
 * records each arm node's origin, and a muzzle node's two muzzles, in
 * drawn_state, in the world's axes. it keeps a muzzle node's matrix in
 * muzzle_matrix. each arm must hold ARM_JOINTS nodes at least, which nothing
 * checks */
/* @hack type: external linkage, where static would do */
void chaos_record_nodes(NJS_CNK_OBJECT *object)
{
    NJS_CNK_OBJECT *node;
    int i;
    NJS_POINT3 point;

    /* @hack spelling: one assignment to the three parts, where a statement
     * for each would do */
    point.x = point.y = point.z = 0.0F;
    njCalcPoint(NULL, &point, &point);
    node = drawn_state->nodes_a;
    for (i = 0; i < ARM_JOINTS; i++) {
        if (object == node) {
            njCalcPoint(&draw_inverse, &point, &drawn_state->joint_a[i]);
            return;
        }
        node = node->child;
    }
    node = drawn_state->nodes_b;
    for (i = 0; i < ARM_JOINTS; i++) {
        if (object == node) {
            njCalcPoint(&draw_inverse, &point, &drawn_state->joint_b[i]);
            return;
        }
        node = node->child;
    }
    if (object == &normal_reach_muzzle_node ||
        object == &float_reach_muzzle_node || object == &normal_muzzle_node) {
        njGetMatrix(&muzzle_matrix);
        point.x = MUZZLE_X;
        point.y = 0.0F;
        point.z = -MUZZLE_Z;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&draw_inverse, &point, &drawn_state->muzzle_a);
        point.x = MUZZLE_X;
        point.y = 0.0F;
        point.z = MUZZLE_Z;
        njCalcPoint(NULL, &point, &point);
        njCalcPoint(&draw_inverse, &point, &drawn_state->muzzle_b);
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* draw the chaos's body for its mode and kind. the draws of a reach and of a
 * spin record the joints and the muzzles through chaos_record_nodes */
static void chaos_draw(struct task *task)
{
    struct task_work *work;
    struct enemy_work *enemy;
    struct chaos_state *state;
    NJS_VECTOR vector;
    NJS_POINT3 point;
    Float flash_frame;
    /* @hack spelling: scl, a local for FLASH_RATE, where FLASH_RATE written
     * in place would do */
    Float scl;
    Float saved_scale;
    /* @hack spelling: left_x, left_z and right_z, locals for FLASH_A_X,
     * FLASH_A_Z and FLASH_B_Z, where the names written in place would do */
    Float left_x;
    Float left_z;
    Float right_z;
    /* @hack spelling: object, a local for guard_object's address, where the
     * address written in place would do */
    NJS_CNK_OBJECT *object;
    /* @hack spelling: texture, a local for a reach body's texture word, where
     * the word read in place would do */
    Sint16 *texture;
    int kind;
    int i;
    Sint16 texture_flags;
    /* @hack spelling: print_hex, a local for njPrintH, where njPrintH called
     * in place would do */
    void (*print_hex)(Int, Uint32, Int);

    work = task->work;
    enemy = task->motion.p;
    state = task->extra.p;
    light_apply(LIGHT_EFFECT);
    scl = FLASH_RATE;
    /* @hack redundant: a block no path reaches, where none would do. SHC
     * still writes its switch's cases into the image, and the block shapes
     * the code of its else. the calls before the switch and the case labels
     * leave no code, so those written here are stand-ins */
    if (0) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + HALF_TURN);
        njSetTexture(&chaos_texlist);
        njTranslateEx(&work->pos);
        /* @hack type: normal_object and float_object cast from their
         * addresses, where the arrays alone would do */
        switch (work->submode) {
        case CHAOS_NORMAL_REVERSE:
            njRotateZ(NULL, HALF_TURN);
            njRotateY(NULL, HALF_TURN);
            njCnkSimpleDrawMotion((NJS_CNK_OBJECT *)&normal_object,
                                  &normal_motion, enemy->motion.frame);
            break;
        case CHAOS_ROUND_REVERSE:
        case CHAOS_ROUND_REVERSE_PARALLEL:
            njRotateZ(NULL, HALF_TURN);
            njCnkSimpleDrawMotion((NJS_CNK_OBJECT *)&normal_object,
                                  &normal_motion, enemy->motion.frame);
            break;
        case CHAOS_NORMAL:
        case CHAOS_ROUND:
        case CHAOS_ROUND_PARALLEL:
            njCnkSimpleDrawMotion((NJS_CNK_OBJECT *)&normal_object,
                                  &normal_motion, enemy->motion.frame);
            break;
        case CHAOS_FLOAT:
            njCnkSimpleDrawMotion((NJS_CNK_OBJECT *)&float_object,
                                  &float_motion, enemy->motion.frame);
            break;
        case CHAOS_SHEDDING:
        case CHAOS_SHEDDING_RED:
            njTranslate(NULL, 0.0F, njRoundOff(work->scl.y), 0.0F);
            njCnkSimpleDrawMotion((NJS_CNK_OBJECT *)&float_object,
                                  &float_motion, enemy->motion.frame);
            break;
        case CHAOS_GUARD:
            njCnkSimpleDrawShapeMotion(&guard_object, &guard_motion,
                                       &guard_shut_shape, enemy->motion.frame);
            break;
        }
        njPopMatrixEx();
    } else {
        right_z = FLASH_B_Z;
        left_z = FLASH_A_Z;
        left_x = FLASH_A_X;
        if (work->mode == MODE_REACH) {
            texture = &NORMAL_REACH_TEXID;
            texture_flags = texture[0];
            /* @hack redundant: a test no path reaches, of the word just
             * read */
            if (0) {
                if (texture_flags) {
                }
            }
            texture_flags = (texture_flags & ~TEXID_BITS) |
                            ((state->tex_frame >> 1) + TEXTURE_FIRST);
            texture[0] = texture_flags;

            /* @hack spelling: arms_pose named through its second name, where
             * its own name would do */
            arms_pose_outside(state);
            drawn_state = task->extra.p;
            /* @hack spelling: chaos_record_nodes named through its second
             * name, where its own name would do */
            njCnkSetMotionCallback(chaos_record_nodes_outside);
            njGetMatrix(&draw_inverse);
            njInvertMatrix(&draw_inverse);
            njSetTexture(&chaos_texlist);
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            if (work->submode == CHAOS_NORMAL_REVERSE) {
                njRotateZ(NULL, HALF_TURN);
                njRotateY(NULL, HALF_TURN);
            }
            njCnkSimpleDrawMotion(
                &normal_reach_object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
            njCnkSetMotionCallback(NULL);
            if (enemy->motion.action == ACTION_REACH_FIRE_UP ||
                enemy->motion.action == ACTION_REACH_FIRE_DOWN) {
                if (enemy->motion.frame >= 0.0F &&
                    enemy->motion.frame < FLASH_FRAMES) {
                    /* @hack redundant: the frame less 0.0F, where the frame
                     * alone would do */
                    flash_frame = (enemy->motion.frame - 0.0F) * scl;
                    /* @hack redundant: a store no path reaches */
                    if (0) {
                        flash_frame = enemy->motion.frame;
                    }
                    njSetMatrix(NULL, &muzzle_matrix);
                    vector.x = left_x;
                    vector.y = FLASH_A_Y;
                    vector.z = left_z;
                    njTranslateEx(&vector);
                    njRotateXYZ(NULL, FLASH_TURN_X, FLASH_TURN_Y, FLASH_TURN_Z);
                    njCnkSimpleDrawShapeMotion(&flash_a_object, &flash_a_motion,
                                               &flash_a_shape, flash_frame);
                    njSetMatrix(NULL, &muzzle_matrix);
                    vector.x = FLASH_B_X;
                    vector.y = FLASH_B_Y;
                    vector.z = right_z;
                    njTranslateEx(&vector);
                    njRotateXYZ(NULL, FLASH_TURN_X, FLASH_TURN_Y, FLASH_TURN_Z);
                    njCnkSimpleDrawShapeMotion(&flash_b_object, &flash_b_motion,
                                               &flash_b_shape, flash_frame);
                }
            }
            njPopMatrixEx();
            /* @hack redundant: a loop no path reaches, where none would do.
             * SHC still writes the loop's body into the image, and the loop
             * shapes the code around it. its start leaves no code, so the
             * start written here is a stand-in */
            if (0) {
                for (i = 0; i < ARM_JOINTS; i++) {
                    debug_draw_model_unlit(&state->joint_a[i], NULL, &vector,
                                           DEBUG_MODEL_CYLINDER);
                }
            }
        } else if (work->mode == MODE_SPIN) {
            texture_flags = NORMAL_TEXID[0];
            texture_flags = (texture_flags & ~TEXID_BITS) |
                            ((state->tex_frame >> 1) + TEXTURE_FIRST);
            NORMAL_TEXID[0] = texture_flags;

            drawn_state = task->extra.p;
            /* @hack spelling: chaos_record_nodes named through its second
             * name, where its own name would do */
            njCnkSetMotionCallback(chaos_record_nodes_outside);
            njGetMatrix(&draw_inverse);
            njInvertMatrix(&draw_inverse);
            njSetTexture(&chaos_texlist);
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            switch (work->submode) {
            case CHAOS_ROUND_REVERSE:
            case CHAOS_ROUND_REVERSE_PARALLEL:
                njRotateZ(NULL, HALF_TURN);
                break;
            }
            njCnkSimpleDrawMotion(
                normal_object, enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
            njCnkSetMotionCallback(NULL);
            if (CHARGE(enemy) > 0.0F) {
                flash_frame = CHARGE(enemy);
                njSetMatrix(NULL, &muzzle_matrix);
                vector.x = left_x;
                vector.y = FLASH_A_Y;
                vector.z = left_z;
                njTranslateEx(&vector);
                njRotateXYZ(NULL, FLASH_TURN_X, FLASH_TURN_Y, FLASH_TURN_Z);
                njCnkSimpleDrawShapeMotion(&flash_a_object, &flash_a_motion,
                                           &flash_a_shape, flash_frame);
                njSetMatrix(NULL, &muzzle_matrix);
                vector.x = FLASH_B_X;
                vector.y = FLASH_B_Y;
                vector.z = right_z;
                njTranslateEx(&vector);
                njRotateXYZ(NULL, FLASH_TURN_X, FLASH_TURN_Y, FLASH_TURN_Z);
                njCnkSimpleDrawShapeMotion(&flash_b_object, &flash_b_motion,
                                           &flash_b_shape, flash_frame);
            }
            njPopMatrixEx();
        } else if (work->submode == CHAOS_SHEDDING ||
                   work->submode == CHAOS_SHEDDING_RED) {
            if (enemy->motion.action == ACTION_FLOAT) {
                texture_flags = FLOAT_TEXID[0];
                texture_flags = (texture_flags & ~TEXID_BITS) |
                                ((state->tex_frame >> 1) + TEXTURE_FIRST);
                FLOAT_TEXID[0] = texture_flags;

            } else if (enemy->motion.action != ACTION_SPENT) {
                texture_flags = SHED_TEXID[0];
                texture_flags = (texture_flags & ~TEXID_BITS) |
                                ((state->tex_frame >> 1) + TEXTURE_FIRST);
                SHED_TEXID[0] = texture_flags;
            }
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            njSetTexture(&chaos_texlist);
            if (enemy->motion.action == ACTION_FLOAT) {
                njCnkSimpleDrawShapeMotion(
                    enemy->motion.object,
                    enemy->motion.table[enemy->motion.action].motion,
                    &float_shape, enemy->motion.frame);
            } else if (enemy->motion.action == ACTION_SHED_START ||
                       enemy->motion.action == ACTION_SHED) {
                njCnkSimpleDrawShapeMotion(
                    enemy->motion.object,
                    enemy->motion.table[enemy->motion.action].motion,
                    &shed_shape, enemy->motion.frame);
            } else if (enemy->motion.action == ACTION_SHED_END) {
                njCnkSimpleDrawShapeMotion(
                    enemy->motion.object,
                    enemy->motion.table[enemy->motion.action].motion,
                    &shed_end_shape, enemy->motion.frame);
            } else if (enemy->motion.action == ACTION_SPENT) {
                njCnkSimpleDrawMotion(&spent_object, &spent_motion,
                                      enemy->motion.frame);
            }
            njPopMatrixEx();
        } else if (work->mode == MODE_FLOAT ||
                   work->mode == MODE_FLOAT_WANDER) {
            texture_flags = FLOAT_TEXID[0];
            texture_flags = (texture_flags & ~TEXID_BITS) |
                            ((state->tex_frame >> 1) + TEXTURE_FIRST);
            FLOAT_TEXID[0] = texture_flags;

            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            njSetTexture(&chaos_texlist);
            if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
                njCnkSimpleDrawShapeMotion(
                    enemy->motion.object,
                    enemy->motion.table[enemy->motion.action].motion,
                    &float_shape, enemy->motion.frame);
            } else {
                njCnkSimpleDrawMotionLink(enemy->motion.object,
                                          &enemy->motion.link,
                                          enemy->motion.frame);
            }
            njPopMatrixEx();
        } else if (work->mode == MODE_FLOAT_REACH) {
            texture = &FLOAT_REACH_TEXID;
            texture_flags = texture[0];
            /* @hack redundant: a test no path reaches, of the word just
             * read */
            if (0) {
                if (texture_flags) {
                }
            }
            texture_flags = (texture_flags & ~TEXID_BITS) |
                            ((state->tex_frame >> 1) + TEXTURE_FIRST);
            texture[0] = texture_flags;

            /* @hack spelling: arms_pose named through its second name, where
             * its own name would do */
            arms_pose_outside(state);
            drawn_state = task->extra.p;
            /* @hack spelling: chaos_record_nodes named through its second
             * name, where its own name would do */
            njCnkSetMotionCallback(chaos_record_nodes_outside);
            njGetMatrix(&draw_inverse);
            njInvertMatrix(&draw_inverse);
            njSetTexture(&chaos_texlist);
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            /* @hack redundant: an empty block no path reaches */
            if (0) {
            }
            njCnkSimpleDrawMotion(
                &float_reach_object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
            njCnkSetMotionCallback(NULL);
            if (enemy->motion.action == ACTION_FLOAT_REACH_FIRE) {
                if (enemy->motion.frame >= 0.0F &&
                    enemy->motion.frame < FLASH_FRAMES) {
                    /* @hack redundant: the frame less 0.0F, where the frame
                     * alone would do */
                    flash_frame = (enemy->motion.frame - 0.0F) * scl;
                    njSetMatrix(NULL, &muzzle_matrix);
                    vector.x = left_x;
                    vector.y = FLASH_A_Y;
                    vector.z = left_z;
                    njTranslateEx(&vector);
                    njRotateXYZ(NULL, FLASH_TURN_X, FLASH_TURN_Y, FLASH_TURN_Z);
                    njCnkSimpleDrawShapeMotion(&flash_a_object, &flash_a_motion,
                                               &flash_a_shape, flash_frame);
                    njSetMatrix(NULL, &muzzle_matrix);
                    vector.x = FLASH_B_X;
                    vector.y = FLASH_B_Y;
                    vector.z = right_z;
                    njTranslateEx(&vector);
                    njRotateXYZ(NULL, FLASH_TURN_X, FLASH_TURN_Y, FLASH_TURN_Z);
                    njCnkSimpleDrawShapeMotion(&flash_b_object, &flash_b_motion,
                                               &flash_b_shape, flash_frame);
                }
            }
            njPopMatrixEx();
        } else if (enemy->motion.action == ACTION_SHUT ||
                   enemy->motion.action == ACTION_SHUTTING ||
                   enemy->motion.action == ACTION_OPENING) {
            texture_flags = GUARD_TEXID[0];
            /* @hack redundant: tests no path reaches, of the word just
             * read */
            if (0) {
                if (texture_flags) {
                }
                if (texture_flags) {
                }
                if (texture_flags) {
                }
            }
            texture_flags = (texture_flags & ~TEXID_BITS) |
                            ((state->tex_frame >> 1) + TEXTURE_FIRST);
            GUARD_TEXID[0] = texture_flags;

            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            njSetTexture(&chaos_texlist);
            object = &guard_object;
            switch (enemy->motion.action) {
            case ACTION_SHUT:
                njCnkSimpleDrawShapeMotion(
                    object, enemy->motion.table[enemy->motion.action].motion,
                    &guard_shut_shape, enemy->motion.frame);
                break;
            case ACTION_SHUTTING:
            case ACTION_OPENING:
                njCnkSimpleDrawShapeMotion(
                    object, enemy->motion.table[enemy->motion.action].motion,
                    &guard_shape, enemy->motion.frame);
                break;
            }
            njPopMatrixEx();
        } else {
            texture_flags = NORMAL_TEXID[0];
            texture_flags = (texture_flags & ~TEXID_BITS) |
                            ((state->tex_frame >> 1) + TEXTURE_FIRST);
            NORMAL_TEXID[0] = texture_flags;

            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y + HALF_TURN);
            njSetTexture(&chaos_texlist);
            switch (work->submode) {
            case CHAOS_NORMAL_REVERSE:
                njRotateZ(NULL, HALF_TURN);
                njRotateY(NULL, HALF_TURN);
                break;
            case CHAOS_ROUND_REVERSE:
            case CHAOS_ROUND_REVERSE_PARALLEL:
                njRotateZ(NULL, HALF_TURN);
                break;
            }
            if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
                njCnkSimpleDrawMotion(
                    enemy->motion.object,
                    enemy->motion.table[enemy->motion.action].motion,
                    enemy->motion.frame);
            } else {
                njCnkSimpleDrawMotionLink(enemy->motion.object,
                                          &enemy->motion.link,
                                          enemy->motion.frame);
            }
            njPopMatrixEx();
        }
    }
    /* @hack redundant: a block no path reaches, where none would do. it
     * leaves its strings in the object, and shapes the code SHC writes before
     * it. SHC still writes its cases and the second switch's dispatch into
     * the image. the code before the first switch, that switch's labels and
     * the prints' callees leave no code, so those written here are
     * stand-ins */
    if (0) {
        kind = work->submode;
        print_hex = njPrintH;
        switch (kind) {
        case CHAOS_NORMAL:
        case CHAOS_FLOAT:
        case CHAOS_NORMAL_REVERSE:
        case CHAOS_GUARD:
            njPrintC(NJM_LOCATION(20, 15), "PHASE ");
            print_hex(NJM_LOCATION(26, 15), work->ang.x & HIGH_BYTE, 4);
            njPrintC(NJM_LOCATION(17, 16), "-> LASER LENGTH ");
            njPrintD(NJM_LOCATION(33, 16), (Uint8)work->ang.x, 3);
            njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
            njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
            njPrintC(NJM_LOCATION(34, 18), " FRAMES");
            njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 8)");
            njPrintC(NJM_LOCATION(17, 20), "-> LASER SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> RANGE       ");
            break;
        case CHAOS_ROUND:
        case CHAOS_ROUND_REVERSE:
        case CHAOS_ROUND_PARALLEL:
        case CHAOS_ROUND_REVERSE_PARALLEL:
            njPrintC(NJM_LOCATION(20, 15), "ROT SPEED ");
            print_hex(NJM_LOCATION(30, 15), work->ang.x & HIGH_BYTE, 4);
            njPrintC(NJM_LOCATION(17, 16), "-> LASER LENGTH ");
            njPrintD(NJM_LOCATION(33, 16), (Uint8)work->ang.x, 3);
            njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
            njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
            njPrintC(NJM_LOCATION(34, 18), " FRAMES");
            njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 8)");
            njPrintC(NJM_LOCATION(17, 20), "-> LASER SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> RANGE/SCOPE");
            break;
        case CHAOS_SHEDDING:
        case CHAOS_SHEDDING_RED:
            njPrintC(NJM_LOCATION(17, 16), "-> AREA X DIFF/X SIZE ");
            njPrintC(NJM_LOCATION(17, 18), "-> AREA Z DIFF/Z SIZE ");
            njPrintC(NJM_LOCATION(17, 19), "-> TYPE( 0 - 8)");
            njPrintC(NJM_LOCATION(17, 20), "-> Y HEIGHT/");
            njPrintD(NJM_LOCATION(29, 20),
                     (int)(njFraction(work->scl.y) * SHED_SCALE), 2);
            njPrintC(NJM_LOCATION(31, 20), "PEACES");
            njPrintC(NJM_LOCATION(17, 21), "-> RANGE/SCOPE");
            break;
        }
        njPrintC(NJM_LOCATION(17, 17), "-> ANG Y ");
        print_hex(NJM_LOCATION(26, 17), work->ang.y & HIGH_BYTE, 4);
        njPrintC(NJM_LOCATION(30, 17), " EME ");
        njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
        switch (kind) {
        case CHAOS_NORMAL:
        case CHAOS_FLOAT:
        case CHAOS_NORMAL_REVERSE:
        case CHAOS_GUARD:
            vector.z = vector.x = work->scl.z + RANGE;
            vector.y = 1.0F;
            debug_draw_model_unlit(&work->pos, NULL, &vector,
                                   DEBUG_MODEL_CYLINDER);
            break;
        case CHAOS_ROUND:
        case CHAOS_ROUND_REVERSE:
        case CHAOS_ROUND_PARALLEL:
        case CHAOS_ROUND_REVERSE_PARALLEL:
            scl = work->scl.z;
            if (kind != CHAOS_ROUND_REVERSE &&
                kind != CHAOS_ROUND_REVERSE_PARALLEL) {
                enemy_set_offset(work, enemy);
            } else {
                enemy_set_offset_behind(work, enemy);
            }
            vector.z = vector.x = njRoundOff(work->scl.z) + RANGE;
            vector.y = 1.0F;
            point = work->pos;
            njAddVector(&point, &enemy->offset);
            debug_draw_model_unlit(&point, NULL, &vector, DEBUG_MODEL_CYLINDER);
            work->scl.z = scl;
            break;
        case CHAOS_SHEDDING:
        case CHAOS_SHEDDING_RED:
            saved_scale = work->scl.z;
            enemy_set_offset(work, enemy);
            vector.z = vector.x = njRoundOff(work->scl.z) + RANGE;
            vector.y = scl;
            point = work->pos;
            njAddVector(&point, &enemy->offset);
            debug_draw_model_unlit(&point, NULL, &vector, DEBUG_MODEL_CYLINDER);
            work->scl.z = saved_scale;
            point.x =
                work->pos.x + ((work->ang.x >> HIGH_BYTE_SHIFT) * BOX_UNIT);
            point.y = work->pos.y;
            point.z =
                work->pos.z + ((work->ang.z >> HIGH_BYTE_SHIFT) * BOX_UNIT);
            vector.x = (5.0F + ((Uint8)work->ang.x * BOX_UNIT)) * 2.0F;
            vector.y = 2.0F;
            vector.z = (5.0F + ((Uint8)work->ang.z * BOX_UNIT)) * 2.0F;
            debug_draw_model_unlit(&point, NULL, &vector, DEBUG_MODEL_BOX);
            break;
        }
    }
    light_apply(light_after_draw);
}

void chaos_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct chaos_state *state = task->extra.p;

    if (work->mode != MODE_INIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        /* @hack spelling: the init's call after the switch, reached by a
         * goto, where the call and a return in its case would do */
        goto init;
    case MODE_WAIT:
        chaos_wait(task, work, enemy);
        break;
    case MODE_WANDER:
        chaos_wander(task, work, enemy);
        break;
    case MODE_REACH:
        chaos_reach(task, work, enemy);
        break;
    case MODE_FLOAT:
        chaos_float(task, work, enemy);
        break;
    case MODE_FLOAT_WANDER:
        chaos_float_wander(task, work, enemy);
        break;
    case MODE_FLOAT_REACH:
        chaos_float_reach(task, work, enemy);
        break;
    case MODE_SPIN:
        chaos_spin(task, work, enemy);
        break;
    case MODE_SHED:
        chaos_shed(task, work, enemy);
        break;
    case MODE_GUARD:
        chaos_guard(task, work, enemy);
        break;
    case MODE_DEFEATED:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        placement_kill(task);
        /* @hack spelling: a goto to the end, where a return would do */
        goto end;
    }
    /* @hack spelling: the empty then-arm and its else, where the test
     * negated would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
        /* @hack redundant: a test with an empty body */
        if (work->flags & CHAOS_PLAYER_NEAR) {
        }
        state->tex_frame++;
        if (state->tex_frame > FRAME_LAST) {
            state->tex_frame = 0;
        }
    }
    collision_register(task);
    return;
init:
    /* @hack redundant: a store no path reaches */
    if (0) {
        work->mode = work->mode;
    }
    chaos_init(task, work);
    return;
end:;
}

/* draw a piece from its build. a lit piece draws piece_model instead. so does
 * any piece while there are no builds, or while game_state.builds_drawn is 0.
 * the parent's first child sets the draw up, and pushes a matrix for all its
 * pieces. its last child, the first's prev, pops the matrix */
static void piece_draw(struct task *task)
{
    struct piece *piece = task->extra.p;

    if (task->parent->child == task) {
        light_apply(LIGHT_EFFECT);
        njSetTexture(&chaos_texlist);
        njSetDirectClip();
        njPushMatrixEx();
    }
    njTranslateEx((NJS_VECTOR *)piece->object.pos);
    njRotateY(NULL, piece->object.ang[1]);
    njRotateX(NULL, piece->object.ang[0]);
    if (PIECE_LIT(task->work) || piece_builds == NULL ||
        !game_state.builds_drawn) {
        /* @hack spelling: texid, a local for the texture word, where the
         * word rewritten in place would do */
        Sint16 texid = PIECE_TEXID;

        texid = (texid & ~TEXID_BITS) | ((piece->frame >> 1) + TEXTURE_FIRST);
        PIECE_TEXID = texid;
        njCnkSimpleDrawModel(&piece_model);
    } else {
        /* @hack spelling: build, a local for the build's address, where the
         * address passed in place would do */
        Uint8 *build = (Uint8 *)piece_builds->extra.p +
                       (piece_builds->unk_3c.l * (piece->frame >> 1));

        njDirectDrawObject((NJS_DIRECT_HEAD *)build);
    }
    matrix_load_parent();
    if (task->parent->child->prev == task) {
        light_apply(light_after_draw);
        njPopMatrixEx();
    } else if (PIECE_LIT(task->work)) {
        light_apply(LIGHT_EFFECT);
    }
}

static void piece_crawl_step(struct task *task);

/* draw a piece as piece_draw does, from piece_b_object's builds. a lit piece
 * draws in a red light */
static void piece_draw_red(struct task *task)
{
    struct piece *piece = task->extra.p;
    Sint16 texid;
    /* @hack spelling: build, a local for the build's address, where the
     * address passed in place would do */
    NJS_DIRECT_HEAD *build;

    if (task->parent->child == task) {
        light_apply(LIGHT_EFFECT);
        njSetTexture(&chaos_texlist);
        njSetDirectClip();
        njPushMatrixEx();
    }
    if (PIECE_LIT(task->work)) {
        njCnkSetSimpleLightIntensity(1.0F, 1.0F);
        njCnkSetSimpleLightColor(LIT_RED, LIT_GREEN, LIT_BLUE);
    }
    njTranslateEx((NJS_VECTOR *)piece->object.pos);
    njRotateY(NULL, piece->object.ang[1]);
    njRotateX(NULL, piece->object.ang[0]);
    /* @bug memory: where chaos_init had no room for piece_b_object's builds,
     * piece_builds holds a null motion. this test doesn't read it, so the
     * draw reads through null (MOV.L @(36,R3),R4 at 0x424e of the object's
     * code) */
#ifdef NONMATCHING
    /* this side repairs the tag above: without piece_b_object's builds, the
     * piece draws as it does without the builds task */
    if (PIECE_LIT(task->work) || piece_builds == NULL ||
        piece_builds->motion.p == NULL || game_state.builds_drawn == 0) {
#else
    if (PIECE_LIT(task->work) || piece_builds == NULL ||
        game_state.builds_drawn == 0) {
#endif
        /* @hack spelling: the texture word through texid, in three
         * statements, where one would do */
        texid = PIECE_TEXID;
        texid = texid & ~TEXID_BITS;
        PIECE_TEXID = texid | ((piece->frame >> 1) + TEXTURE_FIRST);
        njCnkSimpleDrawModel(&piece_model);
    } else {
        build =
            (NJS_DIRECT_HEAD *)((char *)piece_builds->motion.p +
                                (piece_builds->force.l * (piece->frame >> 1)));
        njDirectDrawObject(build);
    }
    matrix_load_parent();
    if (task->parent->child->prev == task) {
        light_apply(light_after_draw);
        njPopMatrixEx();
    } else if (PIECE_LIT(task->work)) {
        light_apply(LIGHT_EFFECT);
    }
}

/* the step of a piece falling from where it was shed. it lands as
 * PIECE_HEIGHT says, and then crawls. it stays in its parent's box.
 *
 * a piece that is hurt, or whose parent is beaten, is killed, and leaves a
 * small blast. the player who touched it, or whose attack hit it, scores
 * PIECE_SCORE, and bounces off as enemy_bounce_player allows */
static void piece_fall_step(struct task *task)
{
    struct task_work *work = task->work;
    struct piece *piece = task->extra.p;
    /* @hack type: the parent read through task's address as bytes, where
     * task->parent would do */
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    struct enemy_work *parent_enemy = task->parent->motion.p;
#else
    struct enemy_work *parent_enemy =
        (*(struct task **)((char *)task + 8))->motion.p;
#endif
    struct chaos_state *area;
    struct task *toucher;
    int player;
    /* @hack spelling: spd_y, a local for the piece's fall, where piece->spd.y
     * changed in place would do */
    Float spd_y;
    /* @hack spelling: coord, a local for each part of pos, where the part
     * read in place would do */
    Float coord;
    /* @hack spelling: to and from, pointers that walk the vectors' parts,
     * where the parts written by name would do */
    Float *to;
    Float *from;

    if ((work->flags & WORK_HURT) ||
        task->parent->work->mode == MODE_DEFEATED) {
        task_kill(task);
        blast_create(BLAST_TINY, &work->pos);
        if (((toucher = collision_next_player(task)) != NULL ||
             (toucher = collision_next_attack(task)) != NULL) &&
            (player = player_slot_of(toucher)) != -1) {
            enemy_bounce_player(player);
            score_add(PIECE_SCORE);
        }
        if (task->parent->work->mode == MODE_DEFEATED) {
            sound_start_at_for(SOUND_PIECE_HIT, task->parent->work, 1,
                               PIECE_VOLUME, PIECE_HIT_DURATION, &work->pos);
        } else {
            sound_play_at(SOUND_PIECE_HIT, NULL, 0, PIECE_VOLUME, &work->pos);
        }
        return;
    }
    if (!game_state.time_stopped) {
        piece->frame++;
        if (piece->frame > FRAME_LAST) {
            piece->frame = 0;
        }
        from = &piece->spd.x;
        to = &work->pos.x;
        *to++ += *from++;
        *to++ += *from++;
        *to += *from;
        if (work->pos.y <= parent_enemy->home.y + PIECE_HEIGHT) {
            work->pos.y = parent_enemy->home.y + PIECE_HEIGHT;
            piece->spd.y = 0.0F;
            njUnitVector(&piece->spd);
            piece->spd.x *= CRAWL_SPEED;
            piece->spd.z *= CRAWL_SPEED;
            task->step = piece_crawl_step;
        } else {
            spd_y = piece->spd.y;
            spd_y -= FALL_GRAVITY;
            if (spd_y < -FALL_FASTEST) {
                spd_y = -FALL_FASTEST;
            }
            piece->spd.y = spd_y;
        }
        area = task->parent->extra.p;
        coord = work->pos.x;
        if (coord > area->x_max) {
            work->pos.x = area->x_max;
        } else if (coord < area->x_min) {
            work->pos.x = area->x_min;
        }
        coord = work->pos.z;
        if (coord > area->z_max) {
            work->pos.z = area->z_max;
        } else if (coord < area->z_min) {
            work->pos.z = area->z_min;
        }
        from = &work->pos.x;
        to = piece->object.pos;
        *to++ = *from++;
        *to++ = *from++ - PIECE_HEIGHT;
        *to = *from;
        piece->object.ang[1] = work->ang.y;
    }
    collision_register(task);
}

static void piece_flash_step(struct task *task);

/* the step of a crawling piece, as CRAWL_RATE and FLASH_RANGE say. it ends as
 * piece_fall_step says */
static void piece_crawl_step(struct task *task)
{
    struct task_work *work = task->work;
    struct piece *piece = task->extra.p;
    struct chaos_state *area;
    struct task *toucher;
    int player;
    int nearest;
    /* @hack spelling: ang, a local for the piece's pitch, where the pitch
     * changed in place would do */
    Angle ang;
    /* @hack spelling: coord, a local for each part of pos, where the part
     * read in place would do */
    Float coord;
    /* @hack spelling: pos and spd, pointers that walk the vectors' parts,
     * each step of pos a statement of its own, where the parts written by
     * name would do */
    Float *pos;
    Float *spd;
    /* @hack spelling: to and from, pointers that walk the vectors' parts,
     * where the parts written by name would do */
    Float *from;
    Float *to;

    if ((work->flags & WORK_HURT) ||
        task->parent->work->mode == MODE_DEFEATED) {
        task_kill(task);
        blast_create(BLAST_TINY_RAISED, &work->pos);
        if (((toucher = collision_next_player(task)) != NULL ||
             (toucher = collision_next_attack(task)) != NULL) &&
            (player = player_slot_of(toucher)) != -1) {
            enemy_bounce_player(player);
            score_add(PIECE_SCORE);
        }
        if (task->parent->work->mode == MODE_DEFEATED) {
            sound_start_at_for(SOUND_PIECE_HIT, task->parent->work, 1,
                               PIECE_VOLUME, PIECE_HIT_DURATION, &work->pos);
        } else {
            sound_play_at(SOUND_PIECE_HIT, NULL, 0, PIECE_VOLUME, &work->pos);
        }
        return;
    }
    if (!game_state.time_stopped) {
        piece->frame++;
        if (piece->frame > FRAME_LAST) {
            piece->frame = 0;
        }
        if (piece->crawl_phase < CRAWL_STOP) {
            spd = &piece->spd.x;
            pos = &work->pos.x;
            *pos += *spd++;
            pos++;
            *pos += *spd++;
            pos++;
            *pos += *spd;
        } else {
            work->ang.y = angle_approach(work->ang.y, piece->aim_y, CRAWL_TURN);
            piece->spd.x = njSin(work->ang.y) * CRAWL_SPEED;
            piece->spd.z = njCos(work->ang.y) * CRAWL_SPEED;
        }
        piece->crawl_phase += CRAWL_RATE;
        ang = piece->object.ang[0];
        if (piece->crawl_phase > CRAWL_STOP) {
            ang += 2 * TILT_RATE;
        } else if (piece->crawl_phase > CRAWL_TILT) {
            ang -= TILT_RATE;
        } else {
            ang = 0;
        }
        piece->object.ang[0] = ang;
        while (piece->crawl_phase >= CRAWL_END) {
            piece->crawl_phase -= CRAWL_END;
            nearest = player_nearest(&work->pos);
            if (njDistanceP2P(&work->pos, &player_work[nearest]->pos) <
                FLASH_RANGE) {
                task->step = piece_flash_step;
                LIGHT_BELOW(work) = FLASH_FIRST;
                work->timer = FLASH_STEPS;
                work->collision->shapes->push |=
                    PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES);
            } else {
                piece->aim_y =
                    atan2f(player_work[nearest]->pos.x - work->pos.x,
                           player_work[nearest]->pos.z - work->pos.z) *
                    ANGLE_PER_RADIAN;
            }
        }
        area = task->parent->extra.p;
        coord = work->pos.x;
        if (coord > area->x_max) {
            work->pos.x = area->x_max;
        } else if (coord < area->x_min) {
            work->pos.x = area->x_min;
        }
        coord = work->pos.z;
        if (coord > area->z_max) {
            work->pos.z = area->z_max;
        } else if (coord < area->z_min) {
            work->pos.z = area->z_min;
        }
        from = &work->pos.x;
        to = piece->object.pos;
        *to++ = *from++;
        *to++ = *from++ - PIECE_HEIGHT;
        *to = *from;
        piece->object.ang[1] = work->ang.y;
    }
    collision_register(task);
}

static void piece_burst_step(struct task *task);

/* the step of a flashing piece, which lights up as LIGHT_BELOW says and
 * bursts as FLASH_STEPS says. a burst piece no longer draws. a flashing piece
 * ends as piece_fall_step says */
static void piece_flash_step(struct task *task)
{
    struct task_work *work = task->work;
    struct piece *piece = task->extra.p;
    struct task *toucher;
    int player;

    if ((work->flags & WORK_HURT) ||
        task->parent->work->mode == MODE_DEFEATED) {
        task_kill(task);
        blast_create(BLAST_TINY_RAISED, &work->pos);
        if (((toucher = collision_next_player(task)) != NULL ||
             (toucher = collision_next_attack(task)) != NULL) &&
            (player = player_slot_of(toucher)) != -1) {
            enemy_bounce_player(player);
            score_add(PIECE_SCORE);
        }
        if (task->parent->work->mode == MODE_DEFEATED) {
            sound_start_at_for(SOUND_PIECE_HIT, task->parent->work, 1,
                               PIECE_VOLUME, PIECE_HIT_DURATION, &work->pos);
        } else {
            sound_play_at(SOUND_PIECE_HIT, NULL, 0, PIECE_VOLUME, &work->pos);
        }
        return;
    }
    if (!game_state.time_stopped) {
        if (work->timer-- <= 0) {
            blast_create(BLAST_MEDIUM_RAISED, &work->pos);
            task->step = piece_burst_step;
            task->draw = NULL;
            work->collision->shapes->levels |= SHAPE_HURTS_ENEMIES;
            work->collision->shapes->push &= ~PUSH_ENEMIES;
            work->collision->shapes->radius = BURST_RADIUS;
            work->collision->radius = BURST_RADIUS;
            work->timer = 0;
            sound_play_at(SOUND_BURST, NULL, 0, PIECE_VOLUME, &work->pos);
            return;
        }
        if (work->timer < LIGHT_BELOW(work)) {
            PIECE_LIT(work) = 1;
            LIGHT_BELOW(work) >>= 1;
        } else {
            PIECE_LIT(work) = 0;
        }
        piece->frame++;
        if (piece->frame > FRAME_LAST) {
            piece->frame = 0;
        }
    }
    collision_register(task);
}

/* the step of a burst piece, which ends on the step after its BURST_STEPS */
static void piece_burst_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!game_state.time_stopped && work->timer++ >= BURST_STEPS) {
        task_kill(task);
    } else {
        collision_register(task);
    }
}

/* shed a piece from parent's pos. the piece is parent's child, and flies up and
 * out on a random heading. where the heap has no room for the piece, nothing is
 * shed */
static void piece_create(struct task *parent)
{
    struct task *task;
    struct piece *piece;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create_child(TASK_WORK, piece_fall_step, parent)) ==
        NULL) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((piece = syCalloc(1, sizeof(struct piece))) == NULL) {
        task_kill(task);
        return;
    }
    task->extra.p = piece;
    piece->spd.x = (rand() * RAND_SCALE) + SHED_SPEED_MIN;
    piece->spd.y = (rand() * RAND_SCALE) + SHED_SPEED_MIN;
    piece->spd.z = (rand() * RAND_SCALE) + SHED_SPEED_MIN;
    task->work->pos = parent->work->pos;
    /* @hack spelling: one assignment to the aim and the heading, where a
     * statement for each would do */
    piece->aim_y = task->work->ang.y = RANDOM_TURN();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, task->work->ang.y);
    njCalcVector(NULL, &piece->spd, &piece->spd);
    njPopMatrixEx();
    piece->object = piece_a_object;
    piece->object.evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    collision_init(task, piece_shapes, 1, COLLISION_ENEMY);
    task->work->collision->radius = PIECE_RADIUS;
    if (parent->work->submode == CHAOS_SHEDDING_RED) {
        task->draw = piece_draw_red;
    } else {
        task->draw = piece_draw;
    }
}

static void path_chaos_destroy(struct task *task);
static void path_chaos_draw(struct task *task);

/* set route's path, and the work's tick, from ang.x, as enemy_chaos.h says.
 * FIRE_START and FIRE_END take the distances along the path of the points
 * ang.z names. DISTANCE takes that of the point scl.y names. a point past the
 * path's count gives 0 */
static void path_chaos_set_route(struct task_work *work,
                                 struct enemy_work *enemy,
                                 struct enemy_route *route)
{
    /* @hack spelling: point, a local for each point's index, where the index
     * passed in place would do */
    unsigned int point;

    PATH_INDEX(enemy) = (work->ang.x & PATH_INDEX_BITS) >> PATH_INDEX_SHIFT;
    if ((Uint32)PATH_INDEX(enemy) >= PATH_CHAOS_PATHS ||
        PATH_INDEX(enemy) < 0) {
        PATH_INDEX(enemy) = 0;
    }
    route->path = path_chaos_paths[PATH_INDEX(enemy)];
    work->tick = (work->ang.x & PATH_FLAGS_BITS) >> PATH_FLAGS_SHIFT;
    point = (work->ang.z & HIGH_BYTE) >> HIGH_BYTE_SHIFT;
    if (path_point_distance(route->path, point, &FIRE_START(enemy)) == 0) {
        FIRE_START(enemy) = 0.0F;
    }
    point = work->ang.z & LOW_BYTE;
    if (path_point_distance(route->path, point, &FIRE_END(enemy)) == 0) {
        FIRE_END(enemy) = 0.0F;
    }
    point = njAbs(njFraction(work->scl.y) * START_SCALE);
    if (path_point_distance(route->path, point, &DISTANCE(enemy)) == 0) {
        DISTANCE(enemy) = 0.0F;
    }
}

static void path_chaos_init(struct task *task, struct task_work *work)
{
    struct enemy_work *enemy;
    struct enemy_route *route;

    enemy = enemy_work_alloc(task);
    route = syCalloc(1, sizeof(struct enemy_route));
    task->extra.p = route;
    path_chaos_set_route(work, enemy, route);
    collision_init(task, path_chaos_shapes, BODY_SHAPES, COLLISION_ENEMY);
    enemy->offset = work->scl;
    WAKE_RANGE(enemy) = njAbs(njFraction(work->scl.x) * WAKE_SCALE) + WAKE_MIN;
    PATH_TURN(enemy) = work->ang.y & HIGH_BYTE;
    enemy->turn_speed = PATH_TURN_SPEED;
    LASER_LENGTH(enemy) = PATH_LASER_LENGTH;
    PATH_LASER_SPEED(work) = work->ang.x & PATH_LASER_SPEED_BITS;
    SPEED(enemy) = njAbs(njFraction(work->scl.z) * SPEED_SCALE);
    /* @hack spelling: one assignment to both angles, where a statement for
     * each, in either order, would do */
    work->ang.x = work->ang.z = 0;
    enemy_set_home(work, enemy);
    work->mode = MODE_PATH_WAIT;
    enemy->motion.table = path_chaos_actions;
    enemy->motion.object = float_object;
    enemy->motion.next = PATH_ACTION;
    enemy->shadow_size = PATH_SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    enemy_laser_load();
    enemy_motion_step(&enemy->motion);
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = path_chaos_draw;
    task->destructor = path_chaos_destroy;
    if (chaos_textures == NULL) {
        texture_load_pvm("E_CHAOSTEX", &chaos_texlist);
        chaos_textures = task_create(0, TEXTURE_LEVEL, textures_step, "ManTex");
        chaos_textures->destructor = chaos_textures_destroy;
    }
}

/* take a hit as chaos_check_hit takes one on CHAOS_FLOAT, with MODE_PATH_HIT
 * for MODE_DEFEATED. then throw back a player that touches LOWER_SHAPE, with
 * no sound or shake */
static void path_chaos_check_hit(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    struct task *toucher;
    int player;
    int score;

    if (enemy_take_hit(work, enemy)) {
        /* @hack spelling: the empty then-arm and its else, where the test
         * negated would do */
        if (game_state.time_stopped) {
        } else {
            if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
                /* @hack spelling: offset and scl declared in the block that
                 * uses them, where the function's head would do */
                NJS_VECTOR offset;
                NJS_VECTOR scl;

                /* @hack spelling: player set to shot_by, where shot_by
                 * passed at its use would do */
                player = enemy->shot_by;
                offset.x = 0.0F;
                offset.y = BALL_OFFSET_Y;
                offset.z = 0.0F;
                /* @hack spelling: one assignment to the three parts, where a
                 * statement for each would do */
                scl.x = scl.y = scl.z = BALL_SCALE;
                enemy_ball_create(player, float_object, &chaos_texlist, &scl,
                                  &offset);
            }
        }
        work->mode = MODE_PATH_HIT;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        enemy_break(work, chaos_pieces, &chaos_texlist);
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((toucher = collision_next_player_by_shape(task, LOWER_SHAPE)) != NULL) {
        /* @hack spelling: spd and ang declared in the block that uses them,
         * where the function's head would do */
        NJS_VECTOR spd;
        struct angles ang;

        player = player_slot_of(toucher);
        spd = work->pos;
        njSubVector(&spd, &player_work[player]->pos);
        ang.x = 0;
        ang.y = (Angle)(atan2f(spd.x, spd.z) * ANGLE_PER_RADIAN);
        ang.z = 0;
        spd.x = THROW_SPEED;
        spd.y = THROW_LIFT;
        spd.z = 0.0F;
        player_launch_speed(player, &spd, &ang, 0);
    }
}

/* move the chaos a step along its path, or on past its end. on the path, it
 * leans as the path does and faces the nearest player. at the path's last
 * segment the chaos leaves the path. it then hovers under PATH_CHAOS_STOP,
 * and flies on otherwise */
static void path_chaos_move(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    struct enemy_route *route;
    struct path *path;
    Float speed;
    unsigned int index;
    struct path_sample sample;
    /* @hack redundant: unused, an array nothing reads, which sizes the
     * frame */
    NJS_VECTOR unused[4];
    NJS_VECTOR dir;
    int player;
    /* @hack spelling: bob_sin, a local for the bob phase's sine, where njSin
     * read in place, before the phase turns, would do */
    Float bob_sin;
    /* @hack spelling: add, a local for njAddVector, where njAddVector called
     * in place would do */
    void (*add)(NJS_VECTOR *vd, NJS_VECTOR *vs);

    if (game_state.time_stopped) {
        return;
    }
    add = njAddVector;
    if (work->mode == MODE_PATH_RIDE) {
        route = task->extra.p;
        path = route->path;
        speed = SPEED(enemy);
        if (work->tick & PATH_CHAOS_STOP) {
            if (DISTANCE(enemy) >= path->length - STOP_DISTANCE) {
                speed = speed * (path->length - DISTANCE(enemy)) * STOP_EASE;
                if (speed < STOP_MIN_SPEED) {
                    speed = STOP_MIN_SPEED;
                }
            }
        }
        DISTANCE(enemy) += speed;
        sample.distance = DISTANCE(enemy);
        /* @hack spelling: the empty then-arm and its else, where the test
         * negated would do */
        if (path_segment_at(path, DISTANCE(enemy), &index) != 0 &&
            index < (unsigned int)(path->count - 2) &&
            path_get_sample(path, &sample) != 0) {
        } else {
            work->mode = (work->tick & PATH_CHAOS_STOP) ? MODE_PATH_HOVER
                                                        : MODE_PATH_FLY_ON;
            add(&work->pos, &enemy->spd);
            return;
        }
        if (work->tick & PATH_CHAOS_MIRROR) {
            sample.pos.x = -sample.pos.x;
        }
        /* the path's point at DISTANCE, in the world. the path starts at
         * home, turned about y by PATH_TURN */
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&enemy->home);
        njRotateY(NULL, PATH_TURN(enemy));
        njCalcPoint(NULL, &sample.pos, &work->pos);
        njPopMatrixEx();
        bob_sin = njSin(PATH_BOB_PHASE(enemy));
        PATH_BOB_PHASE(enemy) += PATH_BOB_TURN;
        work->pos.y += bob_sin * PATH_BOB_RIDE;
        enemy->spd = work->pos;
        njSubVector(&enemy->spd, &enemy->last_pos);
        /* lean the chaos as the path's up vector leans there, except on
         * LEVEL_PATH */
        dir.x = 0.0F;
        dir.y = 1.0F;
        dir.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, PATH_TURN(enemy));
        njRotateZ(NULL, sample.ang_z);
        njRotateX(NULL, sample.ang_x);
        njCalcVector(NULL, &dir, &dir);
        if (PATH_INDEX(enemy) == LEVEL_PATH) {
            work->ang.x = 0;
            work->ang.z = 0;
        } else {
            work->ang.x = (Angle)(asinf(dir.z) * ANGLE_PER_RADIAN);
            work->ang.z = -(Angle)(atan2f(dir.x, dir.y) * ANGLE_PER_RADIAN);
        }
        /* @bug logic: player_nearest's result is never -1, so the else never
         * runs (CMP/EQ #-1,R0 at 0x521c of the object's code) */
        if ((player = player_nearest(&work->pos)) != -1) {
            dir = player_work[player]->pos;
            njSubVector(&dir, &work->pos);
        } else {
            dir = enemy->spd;
        }
        njUnitMatrix(NULL);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njInvertMatrix(NULL);
        njCalcVector(NULL, &dir, &dir);
        work->ang.y = -(Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN);
        njPopMatrixEx();
    } else if (work->mode == MODE_PATH_FLY_ON) {
        add(&work->pos, &enemy->spd);
    }
}

static void path_chaos_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void path_chaos_point_to_world(struct task_work *work, NJS_POINT3 *point)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, point, point);
    njPopMatrixEx();
}

/* aim a shot as chaos_aim_up does, but within AIM_PITCH of level */
static void path_chaos_aim(struct task_work *work, NJS_POINT3 *pos,
                           NJS_VECTOR *dir)
{
    NJS_POINT3 target;
    Float distance;
    /* @hack spelling: rise, a local for the target's height over pos, where
     * the difference written in place would do */
    Float rise;
    Angle ang_y;
    Angle ang_z;
    Angle turn_z;
    Angle turn_y;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    rise = target.y - pos->y;
    if (distance != 0.0F) {
        ang_z = (Angle)(asinf(rise / distance) * ANGLE_PER_RADIAN);
        ang_y = (Angle)(atan2f(pos->z - target.z, target.x - pos->x) *
                        ANGLE_PER_RADIAN);
    } else {
        ang_y = work->ang.y;
        ang_z = 0;
    }
    turn_z = angle_approach(0, ang_z, AIM_PITCH);
    turn_y = angle_approach(work->ang.y, ang_y, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, turn_y);
    njRotateZ(NULL, turn_z);
    njCalcPoint(NULL, dir, dir);
    njPopMatrixEx();
}

/* fire a laser at the nearest player from each of the chaos's two muzzles */
static void path_chaos_fire(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = PATH_MUZZLE_X;
    pos.y = PATH_MUZZLE_Y;
    pos.z = PATH_MUZZLE_Z;
    path_chaos_point_to_world(work, &pos);
    dir.x = PATH_LASER_SPEED(work);
    dir.y = 0.0F;
    dir.z = 0.0F;
    path_chaos_aim(work, &pos, &dir);
    enemy_laser_fire_charged(task, LASER_LENGTH(enemy), &dir, &pos);
    pos.x = PATH_MUZZLE_X;
    pos.y = PATH_MUZZLE_Y;
    pos.z = -PATH_MUZZLE_Z;
    path_chaos_point_to_world(work, &pos);
    dir.x = PATH_LASER_SPEED(work);
    dir.y = 0.0F;
    dir.z = 0.0F;
    path_chaos_aim(work, &pos, &dir);
    enemy_laser_fire_charged(task, LASER_LENGTH(enemy), &dir, &pos);
}

/* the step of MODE_PATH_RIDE and MODE_PATH_FLY_ON. the chaos fires, with
 * PATH_RELOAD steps between shots, while its DISTANCE lies from FIRE_START to
 * FIRE_END. in MODE_PATH_FLY_ON, DISTANCE stays where the chaos left the
 * path */
static void path_chaos_ride_step(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    path_chaos_move(task, work, enemy);
    path_chaos_check_hit(task, work, enemy);
    if (work->mode != MODE_PATH_HIT) {
        if (game_state.time_stopped == 0) {
            work->timer++;
        }
        if (DISTANCE(enemy) >= FIRE_START(enemy) &&
            DISTANCE(enemy) <= FIRE_END(enemy)) {
            if (RELOAD(enemy) <= 0) {
                path_chaos_fire(task, work, enemy);
                RELOAD(enemy) = PATH_RELOAD;
            } else {
                RELOAD(enemy)--;
            }
        }
    }
}

/* bob the chaos up and down, ease it level, and head it toward the nearest
 * player */
static void path_chaos_hover(struct task_work *work, struct enemy_work *enemy)
{
    int player;
    NJS_VECTOR dir;
    /* @hack spelling: bob_cos, a local for the bob phase's cosine, where njCos
     * read in place, before the phase turns, would do */
    Float bob_cos;

    bob_cos = njCos(PATH_BOB_PHASE(enemy));
    PATH_BOB_PHASE(enemy) += PATH_BOB_TURN;
    work->pos.y += bob_cos * PATH_BOB_HOVER;
    player = player_nearest(&work->pos);
    work->ang.x = angle_approach(work->ang.x, 0, LEVEL_EASE);
    work->ang.z = angle_approach(work->ang.z, 0, LEVEL_EASE);
    /* @bug logic: player is never -1, so the else never runs (CMP/EQ
     * #-1,R0 at 0x565e of the object's code) */
    if (player != -1) {
        dir = player_work[player]->pos;
        njSubVector(&dir, &work->pos);
    } else {
        dir = enemy->spd;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njInvertMatrix(NULL);
    njCalcVector(NULL, &dir, &dir);
    work->ang.y = -(Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN);
    njPopMatrixEx();
}

/* the step of MODE_PATH_HOVER, in which the chaos fires with PATH_RELOAD steps
 * between shots */
static void path_chaos_hover_step(struct task *task, struct task_work *work,
                                  struct enemy_work *enemy)
{
    path_chaos_hover(work, enemy);
    path_chaos_check_hit(task, work, enemy);
    if (work->mode != MODE_PATH_HIT) {
        if (game_state.time_stopped == 0) {
            work->timer++;
        }
        if (RELOAD(enemy) <= 0) {
            path_chaos_fire(task, work, enemy);
            RELOAD(enemy) = PATH_RELOAD;
        } else {
            RELOAD(enemy)--;
        }
    }
}

/* the step of MODE_PATH_WAIT. a woken chaos stops its placement and takes its
 * first step along the path */
static void path_chaos_wait_step(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    struct task_work *player;
    int i;

    pos = enemy->home;
    njAddVector(&pos, &enemy->offset);
    /* @bug logic: the loop goes on after a wake, so a step with both players
     * in range moves the chaos twice (BSR path_chaos_move at 0x57f8 of the
     * object's code) */
    for (i = 0; i < 2; i++) {
        player = player_work[i];
        if (player != NULL &&
            WAKE_RANGE(enemy) > njDistanceP2P(&player->pos, &pos)) {
            work->mode = MODE_PATH_RIDE;
            placement_stop(task);
            path_chaos_move(task, work, enemy);
        }
    }
}

/* a function nothing calls. it is external, since SHC leaves out a static
 * function its source never names */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void fun_enemy_chaos_5814(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    struct enemy_route *route = task->extra.p;

    /* @hack redundant: calls no path reaches. the image keeps none of their
     * code, only the stack frame they give the function. the calls written
     * here are stand-ins for the code that gave it that frame */
    if (0) {
        path_chaos_set_route(work, enemy, route);
        path_chaos_hover(work, enemy);
        path_chaos_hover(work, enemy);
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* draw the chaos's path as a triangle at each point but the last. the
 * triangles pulse, and their colors cycle, as route->frames counts. the
 * function's only call lies behind a return in path_chaos_draw, so it never
 * runs */
static void path_chaos_draw_route(struct task *task)
{
    struct enemy_route *route = task->extra.p;
    struct task_work *work = task->work;
    struct path *path;
    struct path_point *point;
    Uint16 phase;
    Uint16 rest;
    Uint16 i;
    Uint32 path_color;
    Uint32 tip_color;
    NJS_VECTOR normal;
    Float length;

    route->frames++;
    phase = (Uint8)route->frames;
    rest = BLEND_SCALE - phase;
    path_color = BLEND_COLOR(0xff, 0xff, 0xff, 0x00, 0x00, 0xff, phase, rest);
    tip_color = BLEND_COLOR(0x00, 0xff, 0x00, 0xff, 0x00, 0xff, phase, rest);
    path_chaos_triangle[0].col = path_color;
    path_chaos_triangle[1].col = path_color;
    path_chaos_triangle[2].col = tip_color;
    path = route->path;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y & HIGH_BYTE);
    njDrawPolygon3DExStart(0);
    for (i = 0, point = path->points; i < path->count - 1; i++, point++) {
        normal.x = -njSin(point->slope_z) * njCos(point->slope_x);
        normal.y = njCos(point->slope_z) * njCos(point->slope_x);
        normal.z = njSin(point->slope_x);
        length =
            TIP_LENGTH +
            (TIP_GROWTH * (Float)((route->frames & TIP_FRAMES) >> TIP_SHIFT));
        path_chaos_triangle[0].x = point->pos.x;
        path_chaos_triangle[0].y = point->pos.y;
        path_chaos_triangle[0].z = point->pos.z;
        path_chaos_triangle[1].x = point[1].pos.x;
        path_chaos_triangle[1].y = point[1].pos.y;
        path_chaos_triangle[1].z = point[1].pos.z;
        path_chaos_triangle[2].x = point->pos.x + (length * normal.x);
        path_chaos_triangle[2].y = point->pos.y + (length * normal.y);
        path_chaos_triangle[2].z = point->pos.z + (length * normal.z);
        if (work->tick & PATH_CHAOS_MIRROR) {
            path_chaos_triangle[0].x = -path_chaos_triangle[0].x;
            path_chaos_triangle[1].x = -path_chaos_triangle[1].x;
            path_chaos_triangle[2].x = -path_chaos_triangle[2].x;
        }
        njDrawPolygon3DExSetData(path_chaos_triangle, 3);
    }
    njDrawPolygon3DExEnd();
    njPopMatrixEx();
}

/* draw the chaos, except in MODE_PATH_WAIT: its body, and under
 * PATH_CHAOS_SHADOW its shadow */
static void path_chaos_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_POINT3 point_a;
    NJS_POINT3 point_b;
    NJS_POINT3 point_c;
    Float length;
    Float size;
    /* @hack spelling: texid and id, locals for the texture word and its
     * value, where the word rewritten in place would do */
    Sint16 *texid;
    int id;

    light_apply(LIGHT_EFFECT);
    if (work->mode != MODE_PATH_WAIT) {
        texid = FLOAT_TEXID;
        id = *texid;
        *texid = (id & ~TEXID_BITS) | ((TEX_FRAME(enemy) >> 1) + TEXTURE_FIRST);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y + HALF_TURN);
        njSetTexture(&chaos_texlist);
        njCnkSimpleDrawShapeMotion(float_object, &path_chaos_motion,
                                   &path_chaos_shape, enemy->motion.frame);
        njPopMatrixEx();
        if (work->tick & PATH_CHAOS_SHADOW) {
            enemy_draw_shadow(work, enemy);
        }
        /* @hack spelling: an empty else, where the if alone would do */
    } else {
    }
    light_apply(light_after_draw);
    /* @hack redundant: a return and, behind it, a body no path reaches. the
     * body leaves its strings in the object and its frame in the code, but
     * none of its own code. past the strings, the body written here is a
     * stand-in */
    if (1) {
        return;
    }
    (void)"< ENEMY PATH CHAOS >";
    (void)" ID:";
    (void)"-> Path";
    (void)"/R/";
    (void)"Sdw";
    (void)"   ";
    (void)"Stp";
    (void)"/LSPD";
    (void)"-> ST(";
    (void)")ED(";
    (void)")/";
    (void)"-> SCOPE X/R";
    (void)"-> SCOPE Y/START PNT";
    (void)"-> SCOPE Z/MOVE SPEED";
    size = work->scl.x;
    njCalcPoint(NULL, &point_a, &point_b);
    njCalcVector(NULL, &point_b, &point_c);
    njUnitMatrix(NULL);
    njCalcPoint(NULL, &point_a, &point_b);
    njCalcVector(NULL, &point_b, &point_c);
    njUnitMatrix(NULL);
    njCalcPoint(NULL, &point_a, &point_b);
    njCalcVector(NULL, &point_b, &point_c);
    njUnitMatrix(NULL);
    njCalcPoint(NULL, &point_c, &point_a);
    njCalcPoint(NULL, &point_c, &point_a);
    njCalcPoint(NULL, &point_c, &point_a);
    njCalcPoint(NULL, &point_c, &point_a);
    path_point_distance(((struct enemy_route *)task->extra.p)->path, 0,
                        &length);
    path_chaos_draw_route(task);
    enemy_draw_shadow(work, enemy);
    point_b.x = size;
    point_c.x = size;
}

void path_chaos_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (work->mode != MODE_PATH_INIT && work->mode != MODE_PATH_RIDE &&
        work->mode != MODE_PATH_HIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_PATH_INIT:
        /* @hack spelling: the init's call after the switch, reached by a
         * goto, where the call and a return in its case would do */
        goto init;
    case MODE_PATH_RIDE:
    case MODE_PATH_FLY_ON:
        path_chaos_ride_step(task, work, enemy);
        break;
    case MODE_PATH_HOVER:
        path_chaos_hover_step(task, work, enemy);
        break;
    case MODE_PATH_HIT:
        work->mode = MODE_PATH_GONE;
        /* fall through */
    case MODE_PATH_GONE:
        placement_kill(task);
        /* @hack spelling: a goto to the end, where a return would do */
        goto end;
    case MODE_PATH_WAIT:
        path_chaos_wait_step(task, work, enemy);
        return;
    }
    /* @hack spelling: the empty then-arm and its else, where the test
     * negated would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
        TEX_FRAME(enemy)++;
        if (TEX_FRAME(enemy) > FRAME_LAST) {
            TEX_FRAME(enemy) = 0;
        }
    }
    if (work->mode != MODE_PATH_HIT) {
        collision_register(task);
    }
    return;
init:
    path_chaos_init(task, work);
    return;
end:;
}
