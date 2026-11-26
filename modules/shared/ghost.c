/* the ghost, an enemy that a stage's object list names "E GHOST". this source
 * also holds the path ghost, which rides a path. nothing in any image reaches
 * the path ghost's step */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "shared/enemy_route.h"
#include "shared/ghost.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "shared/ghost/text.inc"

/* a ghost's modes, in its work's mode */
enum ghost_mode {
    MODE_INIT, /* until ghost_init sets the ghost up */
    MODE_UNUSED, /* stepped as MODE_WANDER, though nothing sets it */
    MODE_CHASE, /* a GHOST_RUSH after the nearest player */
    MODE_CIRCLE, /* a GHOST_GO_AROUND, about home */
    MODE_HIDE, /* a GHOST_HIDE, which isn't drawn */
    MODE_SCARE, /* flying at the player it scared, and fading out */
    MODE_WANDER, /* a GHOST_RUSH or a GHOST_CAPTURE until it finds a player */
    MODE_PURSUE, /* a GHOST_CAPTURE after the nearest player */
    MODE_HOLD, /* a GHOST_CAPTURE holding a player */
    MODE_RELEASE, /* a GHOST_CAPTURE that let go, fading out */
    MODE_FLOAT, /* a GHOST_FLOAT */
    MODE_BIG, /* a GHOST_BIG before its first hit, as big_ghost_object */
    MODE_MEDIUM, /* a GHOST_BIG after its first hit, as medium_ghost_object */
    MODE_SHRUNK, /* a GHOST_BIG after its second hit, as ghost_object */
    MODE_MAKIKO, /* a GHOST_MAKIKO before its hit */
    MODE_GONE, /* a GHOST_GO_AROUND faded out, until it leaves range */
    MODE_DEFEATED, /* fading out and stretching up */
    MODE_DEAD
};

/* a path ghost's modes, in its work's mode */
enum path_ghost_mode {
    MODE_PATH_INIT,
    MODE_PATH_WAIT, /* not drawn, until a player comes in range */
    MODE_PATH_RIDE, /* along the path */
    MODE_PATH_FLY_ON, /* off the path's end, in a straight line */
    MODE_PATH_HOVER, /* at the path's end, under PATH_GHOST_STOP */
    MODE_PATH_DEFEATED, /* fading out and stretching up */
    MODE_PATH_GONE
};

/* a ghost's poses, which it keeps in its enemy_work's motion.next in place
 * of an action's index. ghost_draw picks the model and the motion by the pose
 * and the mode */
#define POSE_STILL 0
#define POSE_SWAY 1
#define POSE_CHASE 2
#define POSE_SCARE 3 /* scare_motion, played once and held */
#define POSE_CATCH 4 /* catch_motion forward, then POSE_HOLD */
#define POSE_HOLD 5 /* hold_motion, looped */
#define POSE_RELEASE 6 /* catch_motion backward */

/* the bits a ghost keeps in its work's flags */
#define GHOST_AWAY 0x1000 /* more than scl.z + HOME_RANGE from home */
#define GHOST_ALERT 0x2000 /* it found a player */

/* bits 12 to 15 of the set entry's ang.x are a path ghost's PATH_INDEX. bits 4
 * to 11 are flags, which the ghost keeps in its work's tick */
#define PATH_INDEX_BITS 0xf000
#define PATH_INDEX_SHIFT 12
#define PATH_FLAGS_BITS 0xff0
#define PATH_FLAGS_SHIFT 4
#define PATH_GHOST_STOP 0x1 /* hover at the path's end, not fly on */
#define PATH_GHOST_MIRROR 0x10 /* ride the path with its x negated */

/* what a ghost keeps in the fields of its enemy_work that enemy.h leaves to
 * each kind */
/* what ghost_move adds to spd.x and spd.z at each step */
#define ACCEL(enemy) ((enemy)->unk_00c)
/* a GHOST_GO_AROUND's bearing from home */
#define CIRCLE_BEARING(enemy) ((enemy)->unk_01c)
/* how far CIRCLE_BEARING turns in a step. it eases toward the enemy_work's
 * turn_speed, which each leg of the circle sets */
#define CIRCLE_TURN(enemy) ((enemy)->unk_028)
/* the steps left in a GHOST_GO_AROUND's leg, after the step that starts it */
#define LEG_STEPS(enemy) ((enemy)->unk_048)
/* a GHOST_BIG's count down from GUARD_TIME, after each hit that shrinks it.
 * while the count is above 0, a hit neither shrinks nor beats the ghost */
#define GUARD_STEPS(enemy) ((enemy)->unk_048)
/* the eye waits BLINK_WAIT steps, then blinks for BLINK_STEPS steps. both
 * counts then start again */
#define BLINK_STEPS(enemy) ((enemy)->unk_04e)
#define BLINK_WAIT(enemy) ((enemy)->unk_050)
/* how far the ghost has faded, from 0, which is opaque, down to UNSEEN. the
 * draw passes FADE to constant_material_set as an alpha. under
 * NJD_CONTROL_3D_OFFSET_MATERIAL, a model's alpha of 1 becomes 1 + FADE. the
 * draw leaves that bit as it finds it */
#define FADE(enemy) ((enemy)->unk_0ac.a)
#define UNSEEN (-1.0F)

/* what a path ghost keeps in its enemy_work */
#define PATH_TURN(enemy) ((enemy)->unk_01c) /* the path's turn about y */
#define BOB_PHASE(enemy) ((enemy)->unk_024) /* the phase of its bob */
/* the index in ghost_paths of the path the ghost rides */
#define PATH_INDEX(enemy) ((enemy)->unk_046)
/* how far along the path the ghost is */
#define PATH_DISTANCE(enemy) ((enemy)->unk_094)
/* how near a player must come to the wake point, home plus offset, to wake
 * the ghost */
#define WAKE_RANGE(enemy) ((enemy)->unk_098)
/* how far along the path the ghost moves in a step */
#define PATH_SPEED(enemy) ((enemy)->unk_09c)

/* a path ghost keeps its parts in its task's unk_3c, since its task's extra
 * holds its enemy_route */
#define PATH_PARTS(task) ((task)->unk_3c.p)

/* a ghost's range from home is scl.z + HOME_RANGE. it finds a player within
 * scl.y + SIGHT_RANGE: ghost_init squares that sum for both sight_distance2
 * and near_distance2. since a player in sight is then near as well,
 * SIGHT_ANGLE never decides what enemy_find_player finds */
#define HOME_RANGE 100.0F
#define SIGHT_RANGE 50.0F
#define SIGHT_ANGLE 0x71c7 /* 160 degrees */
/* the turn_speed ghost_init gives a ghost: the most ghost_turn turns ang.y in
 * a step. ghost_circle overwrites a GHOST_GO_AROUND's with the turn of
 * CIRCLE_BEARING that each leg takes */
#define TURN_SPEED 0x300
/* the ghost's enemy_work's bottom: where its bottom lies in y from the probe's
 * point */
#define BOTTOM (-6.0F)

/* the indexes in ghost_shapes of each kind's shapes. only TOUCH_SHAPE has no
 * attack level */
#define TOUCH_SHAPE 1 /* a GHOST_GO_AROUND's and a GHOST_CAPTURE's */
#define BIG_SHAPES 2 /* a GHOST_BIG's three, one for each size */
#define GHOST_SHAPE 4 /* a GHOST_RUSH's and a GHOST_FLOAT's */
#define MAKIKO_SHAPE 5
/* a GHOST_BIG's SIZES shapes in its collision record, one for each size. at
 * most one of them is on at a time */
#define SIZE_BIG 0
#define SIZE_MEDIUM 1
#define SIZE_SHRUNK 2
#define SIZES 3
/* the reach ghost_init gives a ghost's collision record, in place of the one
 * collision_init fit to its shape. a GHOST_GO_AROUND takes GO_AROUND_RADIUS,
 * and a GHOST_RUSH, a GHOST_CAPTURE or a GHOST_FLOAT takes GHOST_RADIUS */
#define GO_AROUND_RADIUS 6.0F
#define GHOST_RADIUS 8.0F

/* FADE starts at START_FADE in a GHOST_GO_AROUND or a GHOST_CAPTURE, and at 0
 * in the other kinds. every fade but a scare's moves FADE by FADE_STEP a step.
 * a beaten ghost's scl.y grows by STRETCH_STEP a step */
#define START_FADE (-0.25F)
#define FADE_STEP 0.02F
#define STRETCH_STEP 0.03F

/* ghost_move holds spd.x from SPEED_MIN to SPEED_MAX. MODE_PURSUE lets it
 * reach PURSUE_SPEED_MAX, and MODE_BIG holds it at BIG_SPEED */
#define SPEED_MIN 0.2F
#define SPEED_MAX 1.0F
#define BIG_SPEED 0.1F
#define PURSUE_SPEED_MAX 2.5F
/* a ghost sinks while its bottom lies above the floor. its spd.y then falls
 * by SINK_STEP a step, down to SINK_MAX. otherwise FALL_DAMPING scales spd.y
 * each step. a spd.y it scales to within FALL_STOP of 0 becomes 0 */
#define SINK_STEP 0.05F
#define SINK_MAX (-2.0F)
#define FALL_DAMPING 0.95F
#define FALL_STOP 0.1F
/* a step that turns a wandering or chasing ghost more than TURN_SLOW scales
 * its spd.x by COAST_DAMPING. a ghost whose spd.x is SPEED_MIN or less isn't
 * slowed. a step that turns a wandering ghost less than TURN_STILL gives it
 * POSE_SWAY, and any other POSE_STILL. the ghost moves at both. a scared
 * ghost's coast scales spd.x by COAST_DAMPING at every step */
#define TURN_SLOW 0x80
#define TURN_STILL 0x400
#define COAST_DAMPING 0.9F
/* MODE_CHASE, MODE_PURSUE and MODE_SHRUNK set ACCEL.x to CHASE_ACCEL.
 * MODE_CHASE and MODE_PURSUE give up once the timer passes CHASE_STEPS */
#define CHASE_ACCEL 0.1F
#define CHASE_STEPS 600

/* a GHOST_GO_AROUND circles home in legs. a leg goes round either way at
 * CIRCLE_PACE a step, or rests, each a third of the time. its LEG_STEPS starts
 * at random in [LEG_MIN, LEG_MIN + LEG_SPREAD). CIRCLE_TURN eases toward the
 * leg's turn by CIRCLE_EASE a step. the heading turns by at most HEADING_TURN
 * a step */
#define CIRCLE_PACE 0.3F
#define CIRCLE_EASE 64
#define HEADING_TURN 0x400
#define LEG_MIN 300.0F
#define LEG_SPREAD 10.0F

/* ghost_draw shows an eye by BLINK_STEPS in POSE_STILL and POSE_SWAY. the
 * count runs down through phases of BLINK_PHASE counts, a power of 2. a phase
 * shows obake_eye03 while the count mod BLINK_PHASE lies above EYE03_ABOVE.
 * it otherwise shows obake_eye02 in a phase with the bit BLINK_PHASE set, and
 * obake_eye01 in one without. the other poses show obake_eye01 */
#define BLINK_PHASE 8
#define EYE03_ABOVE 4
/* the least and the spread RANDOM_FROM takes for BLINK_STEPS and BLINK_WAIT, in
 * steps. BLINK_STEPS's start then drops to a whole number of phases */
#define BLINK_MIN 20.0F
#define BLINK_SPREAD 40.0F
#define BLINK_WAIT_MIN 4.0F
#define BLINK_WAIT_SPREAD 20.0F
#define BIG_BLINK 16 /* BLINK_STEPS's start in MODE_BIG */
#define BIG_BLINK_WAIT_MIN 300.0F
#define BIG_BLINK_WAIT_SPREAD 120.0F
/* how far a motion's frame moves in a step */
#define FRAME_STEP 0.1F
#define RELEASE_FRAME_STEP 0.05F /* back through catch_motion */
#define CHASE_FRAME_STEP 0.2F /* through medium_motion and chase_motion */

/* a scare sends the ghost toward the player at SCARE_SPEED. the ghost coasts,
 * and fades by SCARE_FADE_STEP a step after its first SCARE_COAST_STEPS */
#define SCARE_SPEED 2.0F
#define SCARE_COAST_STEPS 20
#define SCARE_FADE_STEP 0.04F
/* the animation PLAYER_REQUEST_SCARE asks the scared player to play */
#define SCARE_ANIMATION 80
/* the shake a scare gives the nearest player's pad */
#define SCARE_POWER 7
#define SCARE_FREQ 44
#define SCARE_LENGTH 64
/* a GHOST_CAPTURE fades to HOLD_FADE as it holds a player */
#define HOLD_FADE (-0.2F)
/* the slot of the player a GHOST_CAPTURE holds */
#define HELD_PLAYER(work) ((work)->tick)
/* a hold shakes the held player's pad by HOLD_POWER, HOLD_FREQ and
 * HOLD_LENGTH. it shakes at each step whose timer is a multiple of
 * HOLD_SHAKE_PERIOD. the period must be a power of 2 */
#define HOLD_SHAKE_PERIOD 128
#define HOLD_POWER 2
#define HOLD_FREQ 20
#define HOLD_LENGTH 48
/* ghost_stick_to_player puts a GHOST_CAPTURE at HOLD_OFFSET_X and
 * HOLD_OFFSET_Y from the center of the held player's first collision shape.
 * a mech takes HOLD_OFFSET_Y_MECH in place of HOLD_OFFSET_Y. HOLD_SCALE and
 * HOLD_SCALE_MECH go to scl.x, which the ghost never reads after its setup */
#define HOLD_OFFSET_X (-6.0253F)
#define HOLD_OFFSET_Y (-3.5609999F)
#define HOLD_OFFSET_Y_MECH (-13.561F)
#define HOLD_SCALE 2.1F
#define HOLD_SCALE_MECH 7.0F
#define GUARD_TIME 60
/* a shrink bursts puffs of dust from a sphere about the ghost's pos. the
 * first makes BIG_DUST_PUFFS from a sphere of radius BIG_DUST_RADIUS. each
 * flies at BIG_DUST_SPEED, with a size of BIG_DUST_SIZE. the second makes
 * MEDIUM_DUST_PUFFS by the MEDIUM_DUST_ values */
#define BIG_DUST_PUFFS 12
#define BIG_DUST_RADIUS 20.0F
#define BIG_DUST_SPEED 1.4F
#define BIG_DUST_SIZE 10.0F
#define MEDIUM_DUST_PUFFS 6
#define MEDIUM_DUST_RADIUS 5.0F
#define MEDIUM_DUST_SPEED 0.9F
#define MEDIUM_DUST_SIZE 5.0F

/* the points a beaten GHOST_RUSH or GHOST_BIG adds to the score. each adds
 * its _KNOCK points where player_touch_attacks returns TRUE */
#define RUSH_SCORE 100
#define RUSH_SCORE_KNOCK 200
#define BIG_SCORE 300
#define BIG_SCORE_KNOCK 600

#define SOUND_SCARE 0x4014
#define SOUND_RELEASE 0x4015
#define SOUND_CATCH 0x4016
/* a swaying GHOST_GO_AROUND plays SOUND_SWAY whenever a jiggle's phase passes
 * FULL_TURN, and wraps the phase to below FULL_TURN */
#define SOUND_SWAY 0x4023
#define SWAY_PRIORITY 1
#define SWAY_VOLUME 20
#define SWAY_DURATION 15
/* the sound or voice a scare plays for each look of the scared player */
#define SOUND_SCARED_SONIC 0x300d
#define SOUND_SCARED_KNUCKLES 0x300c
#define SOUND_SCARED_ROUGE 0x301d
#define VOICE_SCARED_SHADOW 0x622
#define VOICE_SCARED_TAILS_MECH 0x691
#define VOICE_SCARED_EGGMAN_MECH 0x690

/* a path ghost's wake point lies at home plus scl, fractions and all. each of
 * scl's fractions, without its sign, also gives a setting at these scales */
/* WAKE_RANGE is scl.x's times WAKE_SCALE, plus WAKE_MIN */
#define WAKE_SCALE 400.0F
#define WAKE_MIN 10.0F
/* the index of the point the ghost starts at is scl.y's times START_SCALE */
#define START_SCALE 100.0F
/* PATH_SPEED is scl.z's times SPEED_SCALE */
#define SPEED_SCALE 10.0F
#define PATH_ACTION 0 /* path_ghost_actions' one action */
/* a path ghost's turn_speed, shadow_size and shadow_ratio */
#define PATH_TURN_SPEED 0x400
#define PATH_SHADOW_SIZE 12.0F
#define PATH_SHADOW_RATIO 1.0F
/* a path ghost whose PATH_INDEX is LEVEL_PATH never tilts with the path */
#define LEVEL_PATH 4
/* a path ghost under PATH_GHOST_STOP changes its step within STOP_DISTANCE of
 * the path's end. the step there is PATH_SPEED times STOP_EASE times the
 * distance left, but no less than STOP_MIN_SPEED */
#define STOP_DISTANCE 50.0F
#define STOP_EASE 0.2F
#define STOP_MIN_SPEED 0.3F
/* a riding path ghost bobs up to RIDE_BOB_HEIGHT above and below its path. a
 * hovering one rises or sinks up to HOVER_BOB_RISE a step. a hovering one's
 * ang.x and ang.z turn back toward 0 by LEVEL_EASE a step */
#define RIDE_BOB_HEIGHT 3.0F
#define HOVER_BOB_RISE 0.2F
#define BOB_TURN 0x400 /* how far BOB_PHASE turns a step */
#define LEVEL_EASE 0x100
/* the ball a beaten path ghost leaves grows toward BALL_SCALE. it draws
 * ghost_object at (0, BALL_OFFSET_Y, 0) from its center */
#define BALL_OFFSET_Y (-18.0F)
#define BALL_SCALE 0.09F
/* the points a beaten path ghost adds to the score: PATH_SCORE_KNOCK where
 * player_touch_attacks returns TRUE, else PATH_SCORE */
#define PATH_SCORE 200
#define PATH_SCORE_KNOCK 400

/* the nodes of a ghost's model: the root, its child and that child's sibling.
 * big_ghost_object has only the root */
#define GHOST_NODES 3
/* the parts of a ghost: for each node, a motion record and a jiggle. a
 * jiggle keeps a copy of its node, whose points it moves at each step. a
 * record's object is its jiggle's copy. the ghost uses only next and object of
 * each record */
struct ghost_parts {
    struct enemy_motion motion[GHOST_NODES];
    struct jiggle *jiggle[GHOST_NODES];
};

static void ghost_draw(struct task *task);
static void ghost_destroy(struct task *task);
static void path_ghost_destroy(struct task *task);
static void path_ghost_draw(struct task *task);

/* small_ghost_object, a GHOST_GO_AROUND's model */
static Sint16 small_ghost_node_2_polygons[49] = {
#include "shared/ghost/small_ghost_node_2_polygons.inc"
};
static Sint32 small_ghost_node_2_vertices[75] = {
#include "shared/ghost/small_ghost_node_2_vertices.inc"
};
static NJS_CNK_MODEL small_ghost_node_2_model = {
#include "shared/ghost/small_ghost_node_2_model.inc"
};
static NJS_CNK_OBJECT small_ghost_node_2_object = {
#include "shared/ghost/small_ghost_node_2_object.inc"
};
static Sint16 small_ghost_node_1_polygons[49] = {
#include "shared/ghost/small_ghost_node_1_polygons.inc"
};
static Sint32 small_ghost_node_1_vertices[75] = {
#include "shared/ghost/small_ghost_node_1_vertices.inc"
};
static NJS_CNK_MODEL small_ghost_node_1_model = {
#include "shared/ghost/small_ghost_node_1_model.inc"
};
static NJS_CNK_OBJECT small_ghost_node_1_object = {
#include "shared/ghost/small_ghost_node_1_object.inc"
};
static Sint16 small_ghost_polygons[501] = {
#include "shared/ghost/small_ghost_polygons.inc"
};
static Sint32 small_ghost_vertices[339] = {
#include "shared/ghost/small_ghost_vertices.inc"
};
static NJS_CNK_MODEL small_ghost_model = {
#include "shared/ghost/small_ghost_model.inc"
};
static NJS_CNK_OBJECT small_ghost_object = {
#include "shared/ghost/small_ghost_object.inc"
};
static NJS_TEXNAME dat_ghost_0_texname[3] = {
#include "shared/ghost/dat_ghost_0_texname.inc"
};
/* The original member retains four unused texture lists beside its models. */
static NJS_TEXLIST dat_ghost_0_texlist = {
#include "shared/ghost/dat_ghost_0_texlist.inc"
};
/* big_ghost_object, a GHOST_BIG's model before its first hit */
static Sint16 big_ghost_polygons[1175] = {
#include "shared/ghost/big_ghost_polygons.inc"
};
static Sint32 big_ghost_vertices[843] = {
#include "shared/ghost/big_ghost_vertices.inc"
};
static NJS_CNK_MODEL big_ghost_model = {
#include "shared/ghost/big_ghost_model.inc"
};
static NJS_CNK_OBJECT big_ghost_object = {
#include "shared/ghost/big_ghost_object.inc"
};
/* the texture lists ghost_init and path_ghost_init load from E_GHOSTEX,
 * E_GHOSTEX2 and E_GHOSTEX3. they differ only in their third name, the eye's */
static NJS_TEXNAME ghost_texname[3] = {
#include "shared/ghost/ghost_texname.inc"
};
static NJS_TEXLIST ghost_texlist = {
#include "shared/ghost/ghost_texlist.inc"
};
static NJS_TEXNAME ghost_2_texname[3] = {
#include "shared/ghost/ghost_2_texname.inc"
};
static NJS_TEXLIST ghost_2_texlist = {
#include "shared/ghost/ghost_2_texlist.inc"
};
static NJS_TEXNAME ghost_3_texname[3] = {
#include "shared/ghost/ghost_3_texname.inc"
};
static NJS_TEXLIST ghost_3_texlist = {
#include "shared/ghost/ghost_3_texlist.inc"
};
static NJS_TEXNAME dat_ghost_1_texname[2] = {
#include "shared/ghost/dat_ghost_1_texname.inc"
};
static NJS_TEXLIST dat_ghost_1_texlist = {
#include "shared/ghost/dat_ghost_1_texlist.inc"
};
/* medium_ghost_object, a GHOST_BIG's model after its first hit */
static Sint16 medium_ghost_node_2_polygons[73] = {
#include "shared/ghost/medium_ghost_node_2_polygons.inc"
};
static Sint32 medium_ghost_node_2_vertices[93] = {
#include "shared/ghost/medium_ghost_node_2_vertices.inc"
};
static NJS_CNK_MODEL medium_ghost_node_2_model = {
#include "shared/ghost/medium_ghost_node_2_model.inc"
};
static NJS_CNK_OBJECT medium_ghost_node_2_object = {
#include "shared/ghost/medium_ghost_node_2_object.inc"
};
static Sint16 medium_ghost_node_1_polygons[73] = {
#include "shared/ghost/medium_ghost_node_1_polygons.inc"
};
static Sint32 medium_ghost_node_1_vertices[93] = {
#include "shared/ghost/medium_ghost_node_1_vertices.inc"
};
static NJS_CNK_MODEL medium_ghost_node_1_model = {
#include "shared/ghost/medium_ghost_node_1_model.inc"
};
static NJS_CNK_OBJECT medium_ghost_node_1_object = {
#include "shared/ghost/medium_ghost_node_1_object.inc"
};
static Sint16 medium_ghost_polygons[903] = {
#include "shared/ghost/medium_ghost_polygons.inc"
};
static Sint32 medium_ghost_vertices[597] = {
#include "shared/ghost/medium_ghost_vertices.inc"
};
static NJS_CNK_MODEL medium_ghost_model = {
#include "shared/ghost/medium_ghost_model.inc"
};
static NJS_CNK_OBJECT medium_ghost_object = {
#include "shared/ghost/medium_ghost_object.inc"
};
static NJS_TEXNAME dat_ghost_2_texname[2] = {
#include "shared/ghost/dat_ghost_2_texname.inc"
};
static NJS_TEXLIST dat_ghost_2_texlist = {
#include "shared/ghost/dat_ghost_2_texlist.inc"
};
/* ghost_object, the model of each kind without one of its own. a GHOST_BIG
 * takes it at its second hit */
static Sint16 ghost_node_2_polygons[79] = {
#include "shared/ghost/ghost_node_2_polygons.inc"
};
static Sint32 ghost_node_2_vertices[93] = {
#include "shared/ghost/ghost_node_2_vertices.inc"
};
static NJS_CNK_MODEL ghost_node_2_model = {
#include "shared/ghost/ghost_node_2_model.inc"
};
static NJS_CNK_OBJECT ghost_node_2_object = {
#include "shared/ghost/ghost_node_2_object.inc"
};
static Sint16 ghost_node_1_polygons[79] = {
#include "shared/ghost/ghost_node_1_polygons.inc"
};
static Sint32 ghost_node_1_vertices[93] = {
#include "shared/ghost/ghost_node_1_vertices.inc"
};
static NJS_CNK_MODEL ghost_node_1_model = {
#include "shared/ghost/ghost_node_1_model.inc"
};
static NJS_CNK_OBJECT ghost_node_1_object = {
#include "shared/ghost/ghost_node_1_object.inc"
};
static Sint16 ghost_polygons[873] = {
#include "shared/ghost/ghost_polygons.inc"
};
static Sint32 ghost_vertices[609] = {
#include "shared/ghost/ghost_vertices.inc"
};
static NJS_CNK_MODEL ghost_model = {
#include "shared/ghost/ghost_model.inc"
};
static NJS_CNK_OBJECT ghost_object = {
#include "shared/ghost/ghost_object.inc"
};
static NJS_TEXNAME dat_ghost_3_texname[2] = {
#include "shared/ghost/dat_ghost_3_texname.inc"
};
static NJS_TEXLIST dat_ghost_3_texlist = {
#include "shared/ghost/dat_ghost_3_texlist.inc"
};
/* hide_ghost_object, which every ghost draws in POSE_SCARE. a GHOST_HIDE's
 * jiggles move a copy of it, which nothing draws */
static Sint16 hide_ghost_node_2_polygons[75] = {
#include "shared/ghost/hide_ghost_node_2_polygons.inc"
};
static Sint32 hide_ghost_node_2_vertices[93] = {
#include "shared/ghost/hide_ghost_node_2_vertices.inc"
};
static NJS_CNK_MODEL hide_ghost_node_2_model = {
#include "shared/ghost/hide_ghost_node_2_model.inc"
};
static NJS_CNK_OBJECT hide_ghost_node_2_object = {
#include "shared/ghost/hide_ghost_node_2_object.inc"
};
static Sint16 hide_ghost_node_1_polygons[75] = {
#include "shared/ghost/hide_ghost_node_1_polygons.inc"
};
static Sint32 hide_ghost_node_1_vertices[93] = {
#include "shared/ghost/hide_ghost_node_1_vertices.inc"
};
static NJS_CNK_MODEL hide_ghost_node_1_model = {
#include "shared/ghost/hide_ghost_node_1_model.inc"
};
static NJS_CNK_OBJECT hide_ghost_node_1_object = {
#include "shared/ghost/hide_ghost_node_1_object.inc"
};
static Sint16 hide_ghost_polygons[887] = {
#include "shared/ghost/hide_ghost_polygons.inc"
};
static Sint32 hide_ghost_vertices[597] = {
#include "shared/ghost/hide_ghost_vertices.inc"
};
static NJS_CNK_MODEL hide_ghost_model = {
#include "shared/ghost/hide_ghost_model.inc"
};
static NJS_CNK_OBJECT hide_ghost_object = {
#include "shared/ghost/hide_ghost_object.inc"
};
/* catch_ghost_object, which POSE_CATCH and POSE_RELEASE draw */
static Sint16 catch_ghost_node_2_polygons[73] = {
#include "shared/ghost/catch_ghost_node_2_polygons.inc"
};
static Sint32 catch_ghost_node_2_vertices[93] = {
#include "shared/ghost/catch_ghost_node_2_vertices.inc"
};
static NJS_CNK_MODEL catch_ghost_node_2_model = {
#include "shared/ghost/catch_ghost_node_2_model.inc"
};
static NJS_CNK_OBJECT catch_ghost_node_2_object = {
#include "shared/ghost/catch_ghost_node_2_object.inc"
};
static Sint16 catch_ghost_node_1_polygons[73] = {
#include "shared/ghost/catch_ghost_node_1_polygons.inc"
};
static Sint32 catch_ghost_node_1_vertices[93] = {
#include "shared/ghost/catch_ghost_node_1_vertices.inc"
};
static NJS_CNK_MODEL catch_ghost_node_1_model = {
#include "shared/ghost/catch_ghost_node_1_model.inc"
};
static NJS_CNK_OBJECT catch_ghost_node_1_object = {
#include "shared/ghost/catch_ghost_node_1_object.inc"
};
static Sint16 catch_ghost_polygons[881] = {
#include "shared/ghost/catch_ghost_polygons.inc"
};
static Sint32 catch_ghost_vertices[597] = {
#include "shared/ghost/catch_ghost_vertices.inc"
};
static NJS_CNK_MODEL catch_ghost_model = {
#include "shared/ghost/catch_ghost_model.inc"
};
static NJS_CNK_OBJECT catch_ghost_object = {
#include "shared/ghost/catch_ghost_object.inc"
};
/* hold_ghost_object, which POSE_HOLD draws */
static Sint16 hold_ghost_node_2_polygons[73] = {
#include "shared/ghost/hold_ghost_node_2_polygons.inc"
};
static Sint32 hold_ghost_node_2_vertices[93] = {
#include "shared/ghost/hold_ghost_node_2_vertices.inc"
};
static NJS_CNK_MODEL hold_ghost_node_2_model = {
#include "shared/ghost/hold_ghost_node_2_model.inc"
};
static NJS_CNK_OBJECT hold_ghost_node_2_object = {
#include "shared/ghost/hold_ghost_node_2_object.inc"
};
static Sint16 hold_ghost_node_1_polygons[73] = {
#include "shared/ghost/hold_ghost_node_1_polygons.inc"
};
static Sint32 hold_ghost_node_1_vertices[93] = {
#include "shared/ghost/hold_ghost_node_1_vertices.inc"
};
static NJS_CNK_MODEL hold_ghost_node_1_model = {
#include "shared/ghost/hold_ghost_node_1_model.inc"
};
static NJS_CNK_OBJECT hold_ghost_node_1_object = {
#include "shared/ghost/hold_ghost_node_1_object.inc"
};
static Sint16 hold_ghost_polygons[895] = {
#include "shared/ghost/hold_ghost_polygons.inc"
};
static Sint32 hold_ghost_vertices[597] = {
#include "shared/ghost/hold_ghost_vertices.inc"
};
static NJS_CNK_MODEL hold_ghost_model = {
#include "shared/ghost/hold_ghost_model.inc"
};
static NJS_CNK_OBJECT hold_ghost_object = {
#include "shared/ghost/hold_ghost_object.inc"
};
/* medium_motion moves the jiggles' copy of medium_ghost_object in MODE_MEDIUM.
 * chase_motion moves their copy of ghost_object in MODE_SHRUNK and MODE_CHASE.
 * the two motions hold the same keys */
static NJS_MKEY_A medium_node_1_ang_keys[2] = {
#include "shared/ghost/medium_node_1_ang_keys.inc"
};
static NJS_MKEY_A medium_node_2_ang_keys[2] = {
#include "shared/ghost/medium_node_2_ang_keys.inc"
};
static NJS_MDATA2 medium_motion_data[3] = {
#include "shared/ghost/medium_motion_data.inc"
};
static NJS_MOTION medium_motion = {
#include "shared/ghost/medium_motion.inc"
};
static NJS_MKEY_A chase_node_1_ang_keys[2] = {
#include "shared/ghost/chase_node_1_ang_keys.inc"
};
static NJS_MKEY_A chase_node_2_ang_keys[2] = {
#include "shared/ghost/chase_node_2_ang_keys.inc"
};
static NJS_MDATA2 chase_motion_data[3] = {
#include "shared/ghost/chase_motion_data.inc"
};
static NJS_MOTION chase_motion = {
#include "shared/ghost/chase_motion.inc"
};
/* scare_motion moves hide_ghost_object in POSE_SCARE */
static NJS_MKEY_A scare_node_0_ang_keys[3] = {
#include "shared/ghost/scare_node_0_ang_keys.inc"
};
static NJS_MKEY_F scare_node_0_scl_keys[3] = {
#include "shared/ghost/scare_node_0_scl_keys.inc"
};
static NJS_MKEY_A scare_node_1_ang_keys[3] = {
#include "shared/ghost/scare_node_1_ang_keys.inc"
};
static NJS_MKEY_F scare_node_1_scl_keys[3] = {
#include "shared/ghost/scare_node_1_scl_keys.inc"
};
static NJS_MKEY_A scare_node_2_ang_keys[3] = {
#include "shared/ghost/scare_node_2_ang_keys.inc"
};
static NJS_MKEY_F scare_node_2_scl_keys[3] = {
#include "shared/ghost/scare_node_2_scl_keys.inc"
};
static NJS_MDATA3 scare_motion_data[3] = {
#include "shared/ghost/scare_motion_data.inc"
};
static NJS_MOTION scare_motion = {
#include "shared/ghost/scare_motion.inc"
};
/* catch_motion and catch_shape move catch_ghost_object in POSE_CATCH and
 * POSE_RELEASE */
static NJS_MKEY_A catch_node_0_ang_keys[4] = {
#include "shared/ghost/catch_node_0_ang_keys.inc"
};
static NJS_MKEY_F catch_node_0_scl_keys[4] = {
#include "shared/ghost/catch_node_0_scl_keys.inc"
};
static NJS_MKEY_A catch_node_1_ang_keys[4] = {
#include "shared/ghost/catch_node_1_ang_keys.inc"
};
static NJS_MKEY_F catch_node_1_scl_keys[4] = {
#include "shared/ghost/catch_node_1_scl_keys.inc"
};
static NJS_MKEY_A catch_node_2_ang_keys[4] = {
#include "shared/ghost/catch_node_2_ang_keys.inc"
};
static NJS_MKEY_F catch_node_2_scl_keys[4] = {
#include "shared/ghost/catch_node_2_scl_keys.inc"
};
static NJS_MDATA3 catch_motion_data[3] = {
#include "shared/ghost/catch_motion_data.inc"
};
/* @hack type: catch_motion an array of one, where a motion would do */
static NJS_MOTION catch_motion[1] = {
#include "shared/ghost/catch_motion.inc"
};
static NJS_POINT3 catch_shape_points_0[99] = {
#include "shared/ghost/catch_shape_points_0.inc"
};
static NJS_POINT3 catch_shape_points_1[99] = {
#include "shared/ghost/catch_shape_points_1.inc"
};
static NJS_POINT3 catch_shape_points_2[99] = {
#include "shared/ghost/catch_shape_points_2.inc"
};
static NJS_MKEY_P catch_shape_keys[3] = {
#include "shared/ghost/catch_shape_keys.inc"
};
static NJS_MDATA2 catch_shape_data[3] = {
#include "shared/ghost/catch_shape_data.inc"
};
static NJS_MOTION catch_shape = {
#include "shared/ghost/catch_shape.inc"
};
/* hold_motion moves hold_ghost_object in POSE_HOLD */
static NJS_MKEY_A hold_node_0_ang_keys[3] = {
#include "shared/ghost/hold_node_0_ang_keys.inc"
};
static NJS_MDATA2 hold_motion_data[3] = {
#include "shared/ghost/hold_motion_data.inc"
};
static NJS_MOTION hold_motion = {
#include "shared/ghost/hold_motion.inc"
};
/* the ghosts' collision shapes, at the indexes TOUCH_SHAPE to MAKIKO_SHAPE.
 * no kind takes ghost_shapes[0] */
static struct collision_shape ghost_shapes[6] = {
#include "shared/ghost/ghost_shapes.inc"
};
/* each model's nodes, in the order of a ghost's parts */
static NJS_CNK_OBJECT *big_ghost_nodes[1] = {
#include "shared/ghost/big_ghost_nodes.inc"
};
static NJS_CNK_OBJECT *small_ghost_nodes[GHOST_NODES] = {
#include "shared/ghost/small_ghost_nodes.inc"
};
static NJS_CNK_OBJECT *medium_ghost_nodes[GHOST_NODES] = {
#include "shared/ghost/medium_ghost_nodes.inc"
};
static NJS_CNK_OBJECT *ghost_nodes[GHOST_NODES] = {
#include "shared/ghost/ghost_nodes.inc"
};
static NJS_CNK_OBJECT *hide_ghost_nodes[GHOST_NODES] = {
#include "shared/ghost/hide_ghost_nodes.inc"
};
/* the task that keeps the three texture lists loaded, or null while none
 * runs. its destructor releases them */
static struct task *ghost_holder = NULL;
/* the kinds' titles, by ghost_kind */
/* The original SET readout retains the titles of all seven kinds. */
static char *ghost_titles[GHOST_KINDS] = {
#include "shared/ghost/ghost_titles.inc"
};
/* the paths a path ghost rides */
static struct path_point ghost_path_0_points[17] = {
#include "shared/ghost/ghost_path_0_points.inc"
};
static struct path ghost_path_0 = {
#include "shared/ghost/ghost_path_0.inc"
};
static struct path_point ghost_path_1_points[13] = {
#include "shared/ghost/ghost_path_1_points.inc"
};
static struct path ghost_path_1 = {
#include "shared/ghost/ghost_path_1.inc"
};
static struct path_point ghost_path_2_points[27] = {
#include "shared/ghost/ghost_path_2_points.inc"
};
static struct path ghost_path_2 = {
#include "shared/ghost/ghost_path_2.inc"
};
static struct path_point ghost_path_3_points[16] = {
#include "shared/ghost/ghost_path_3_points.inc"
};
static struct path ghost_path_3 = {
#include "shared/ghost/ghost_path_3.inc"
};
static struct path_point ghost_path_4_points[67] = {
#include "shared/ghost/ghost_path_4_points.inc"
};
static struct path ghost_path_4 = {
#include "shared/ghost/ghost_path_4.inc"
};
static struct path_point ghost_path_5_points[32] = {
#include "shared/ghost/ghost_path_5_points.inc"
};
static struct path ghost_path_5 = {
#include "shared/ghost/ghost_path_5.inc"
};
static struct path_point ghost_path_6_points[4] = {
#include "shared/ghost/ghost_path_6_points.inc"
};
static struct path ghost_path_6 = {
#include "shared/ghost/ghost_path_6.inc"
};
static struct path_point ghost_path_7_points[11] = {
#include "shared/ghost/ghost_path_7_points.inc"
};
static struct path ghost_path_7 = {
#include "shared/ghost/ghost_path_7.inc"
};
static struct path_point ghost_path_8_points[25] = {
#include "shared/ghost/ghost_path_8_points.inc"
};
static struct path ghost_path_8 = {
#include "shared/ghost/ghost_path_8.inc"
};
static struct path_point ghost_path_9_points[17] = {
#include "shared/ghost/ghost_path_9_points.inc"
};
static struct path ghost_path_9 = {
#include "shared/ghost/ghost_path_9.inc"
};
static struct path_point ghost_path_10_points[11] = {
#include "shared/ghost/ghost_path_10_points.inc"
};
static struct path ghost_path_10 = {
#include "shared/ghost/ghost_path_10.inc"
};
static struct path_point ghost_path_11_points[27] = {
#include "shared/ghost/ghost_path_11_points.inc"
};
static struct path ghost_path_11 = {
#include "shared/ghost/ghost_path_11.inc"
};
static struct collision_shape path_ghost_shapes[1] = {
#include "shared/ghost/path_ghost_shapes.inc"
};
KEPT static struct path *ghost_paths[12] = {
#include "shared/ghost/ghost_paths.inc"
};
#define GHOST_PATHS ARRAY_COUNT(ghost_paths)
/* a path ghost's one action, which moves nothing */
static struct enemy_action path_ghost_actions[1] = {
#include "shared/ghost/path_ghost_actions.inc"
};
/* the triangle path_ghost_draw_route draws at each point of a path */
/* The original route readout retains its vertex triangle. */
static NJS_POLYGON_VTX route_triangle[3] = {
#include "shared/ghost/route_triangle.inc"
};

static void ghost_holder_destroy(struct task *task)
{
    njReleaseTexture(&ghost_texlist);
    njReleaseTexture(&ghost_2_texlist);
    njReleaseTexture(&ghost_3_texlist);
    ghost_holder = NULL;
}

static void ghost_holder_step(struct task *task)
{
}

static void jiggle_set_small(struct jiggle *jiggle)
{
    jiggle->kind = JIGGLE_WAVE;
    jiggle->param = 0.9F;
    jiggle->by_kind.angle = 0x4000;
    jiggle->phase_step = 0x1000;
}

static void jiggle_set_big(struct jiggle *jiggle)
{
    jiggle->kind = JIGGLE_WAVE_TWIN;
    jiggle->param = 0.95F;
    jiggle->by_kind.angle = 0x6000;
    jiggle->phase_step = 0x280;
}

static void jiggle_set_medium(struct jiggle *jiggle)
{
    jiggle->kind = JIGGLE_WAVE_CENTERED;
    jiggle->param = 0.98F;
    jiggle->by_kind.angle = 0x3000;
    jiggle->phase_step = 0x1400;
}

static void jiggle_set_ghost(struct jiggle *jiggle)
{
    jiggle->kind = JIGGLE_WAVE_CENTERED;
    jiggle->param = 0.98F;
    jiggle->by_kind.angle = 0x3000;
    jiggle->phase_step = 0x1400;
}

static void jiggle_set_makiko(struct jiggle *jiggle)
{
    jiggle->kind = JIGGLE_WAVE_CENTERED;
    jiggle->param = 0.9F;
    jiggle->by_kind.angle = 0x3000;
    jiggle->phase_step = 0xbcc;
}

static void ghost_init(struct task *task, struct task_work *work)
{
    struct ghost_parts *parts;
    struct enemy_work *enemy;
    struct jiggle **jiggle_at;
    struct jiggle *jiggle;
    struct enemy_motion *part;
    NJS_CNK_OBJECT **nodes;

    if ((parts = syCalloc(1, sizeof(struct ghost_parts))) == NULL) {
        return;
    }
    task->extra.p = parts;
    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= GHOST_KINDS || work->submode < 0) {
        work->submode = GHOST_GO_AROUND;
    }
    switch (work->submode) {
    case GHOST_HIDE:
    case GHOST_GO_AROUND:
    case GHOST_CAPTURE:
    case GHOST_FLOAT:
    case GHOST_MAKIKO:
        enemy = enemy_work_alloc(task);
        break;
    case GHOST_RUSH:
    case GHOST_BIG:
        enemy = enemy_work_alloc_grouped(task);
        break;
    }
    switch (work->submode) {
    case GHOST_HIDE:
        FADE(enemy) = 0.0F;
        break;
    case GHOST_GO_AROUND:
        collision_init(task, &ghost_shapes[TOUCH_SHAPE], 1, COLLISION_ITEM);
        if (work->collision) {
            work->collision->radius = GO_AROUND_RADIUS;
        }
        FADE(enemy) = START_FADE;
        break;
    case GHOST_CAPTURE:
        collision_init(task, &ghost_shapes[TOUCH_SHAPE], 1, COLLISION_ITEM);
        if (work->collision) {
            work->collision->radius = GHOST_RADIUS;
        }
        FADE(enemy) = START_FADE;
        break;
    case GHOST_RUSH:
    case GHOST_FLOAT:
        collision_init(task, &ghost_shapes[GHOST_SHAPE], 1, COLLISION_ITEM);
        if (work->collision) {
            work->collision->radius = GHOST_RADIUS;
        }
        FADE(enemy) = 0.0F;
        break;
    case GHOST_MAKIKO:
        collision_init(task, &ghost_shapes[MAKIKO_SHAPE], 1, COLLISION_ENEMY);
        FADE(enemy) = 0.0F;
        break;
    case GHOST_BIG:
        collision_init(task, &ghost_shapes[BIG_SHAPES], SIZES, COLLISION_ITEM);
        /* @bug memory: reads the collision record through null where
         * collision_init had no room for it. the cases above test the record
         * before they set its radius (TST R2,R2 at 0x1d0 of the object's
         * code), and this case doesn't (MOV.L @(12,R3),R3 at 0x244). a
         * GHOST_CAPTURE's catch in ghost_check_hit reads it unchecked too
         * (MOV.W @(4,R13),R0 at 0x748). so does a GHOST_RUSH's or a
         * GHOST_BIG's fade in ghost_wander_step (MOV.L @(12,R3),R3 at
         * 0x1138). the give-up of a chase reads it in ghost_chase_step (OR
         * #16,R0 at 0x122c) and in ghost_pursue_step (OR #16,R0 at 0x1302) */
#ifdef NONMATCHING
        /* this side repairs the tag above. this case sets the sizes' shapes
         * only where the record exists. the test after the switch ends a
         * GHOST_RUSH or a GHOST_CAPTURE without one. any other kind reads its
         * record only after a hit. ghost_check_hit acts on a hit only where the
         * record exists */
        if (work->collision != NULL)
#endif
        {
            work->collision->shapes[SIZE_BIG].flags &= ~SHAPE_OFF;
            work->collision->shapes[SIZE_MEDIUM].flags |= SHAPE_OFF;
            work->collision->shapes[SIZE_SHRUNK].flags |= SHAPE_OFF;
        }
        FADE(enemy) = 0.0F;
        break;
    }
#ifdef NONMATCHING
    /* a GHOST_RUSH's fade and a GHOST_CAPTURE's catch read the record at each
     * step. this side ends either kind without a record. a placed ghost's
     * placement makes it again while in range */
    if (work->collision == NULL &&
        (work->submode == GHOST_RUSH || work->submode == GHOST_CAPTURE)) {
        task->destructor = ghost_destroy;
        task_kill(task);
        return;
    }
#endif
    part = parts->motion;
    enemy->motion.next = POSE_STILL;
#ifdef NONMATCHING
    /* the switch below sets nodes: it has a case for each kind the submode
     * can hold. GCC can't see that the submode holds no other */
    nodes = ghost_nodes;
#endif
    switch (work->submode) {
    case GHOST_HIDE:
        nodes = hide_ghost_nodes;
        break;
    case GHOST_GO_AROUND:
        nodes = small_ghost_nodes;
        break;
    case GHOST_BIG:
        nodes = big_ghost_nodes;
        break;
    case GHOST_RUSH:
    case GHOST_FLOAT:
    case GHOST_CAPTURE:
    case GHOST_MAKIKO:
        nodes = ghost_nodes;
        break;
    }
    for (jiggle_at = parts->jiggle; jiggle_at < &parts->jiggle[GHOST_NODES];
         jiggle_at++) {
        /* @hack spelling: one assignment to the jiggle and its slot, where a
         * statement for each would do */
        *jiggle_at = jiggle = jiggle_create(*nodes++);
        /* a GHOST_BIG's model has one node */
        if (work->submode == GHOST_BIG) {
            jiggle_set_big(jiggle);
            part->object = jiggle->copy;
            break;
        }
        if (work->submode == GHOST_GO_AROUND) {
            jiggle_set_small(jiggle);
        } else if (work->submode == GHOST_MAKIKO) {
            jiggle_set_makiko(jiggle);
        } else {
            jiggle_set_ghost(jiggle);
        }
        part->object = jiggle->copy;
        part++;
    }
    if (work->submode != GHOST_BIG) {
        parts->jiggle[0]->copy->child = parts->jiggle[1]->copy;
        parts->jiggle[1]->copy->sibling = parts->jiggle[2]->copy;
    }
    /* @hack spelling: one assignment to both angles, where a statement for
     * each would do */
    work->ang.z = work->ang.x = 0;
    enemy->sight_distance2 =
        (work->scl.y + SIGHT_RANGE) * (work->scl.y + SIGHT_RANGE);
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 =
        (work->scl.y + SIGHT_RANGE) * (work->scl.y + SIGHT_RANGE);
    enemy->spd.x = SPEED_MAX;
    enemy->turn_speed = TURN_SPEED;
    work->scl.x = 1.0F;
    work->scl.y = 1.0F;
    enemy_set_home(work, enemy);
    if (work->submode == GHOST_GO_AROUND) {
        work->mode = MODE_CIRCLE;
    } else if (work->submode == GHOST_FLOAT) {
        work->mode = MODE_FLOAT;
    } else if (work->submode == GHOST_HIDE) {
        work->mode = MODE_HIDE;
    } else if (work->submode == GHOST_RUSH) {
        work->mode = MODE_WANDER;
        enemy->motion.next = POSE_CHASE;
    } else if (work->submode == GHOST_BIG) {
        work->mode = MODE_BIG;
    } else if (work->submode == GHOST_MAKIKO) {
        work->mode = MODE_MAKIKO;
    } else {
        work->mode = MODE_WANDER;
    }
    enemy->bottom = BOTTOM;
    task->draw = ghost_draw;
    task->destructor = ghost_destroy;
    /* @hack redundant: the test of ghost_texlist's address, which is never
     * null, where the test of ghost_holder alone would do */
    /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
    if (&ghost_texlist && ghost_holder == NULL) {
        texture_load_pvm(GHOST_TEXTURE_ARCHIVE, &ghost_texlist);
        texture_load_pvm(GHOST_TEXTURE_ARCHIVE_2, &ghost_2_texlist);
        texture_load_pvm(GHOST_TEXTURE_ARCHIVE_3, &ghost_3_texlist);
        ghost_holder =
            task_create(0, 0, ghost_holder_step, GHOST_TEXTURE_TASK_NAME);
        ghost_holder->destructor = ghost_holder_destroy;
    }
}

static void ghost_turn(struct task_work *work, struct enemy_work *enemy)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & GHOST_AWAY) {
        enemy_turn_to_home(work, enemy);
    } else if (work->flags & GHOST_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    }
}

static void ghost_mark_away(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: distance2, a local for enemy_home_distance2's result,
     * where the result compared in place would do */
    Float distance2;

    if (game_state.time_stopped == 0) {
        distance2 = enemy_home_distance2(work, enemy);
        if (distance2 > powf(work->scl.z + HOME_RANGE, 2.0F)) {
            work->flags |= GHOST_AWAY;
        } else {
            work->flags &= ~GHOST_AWAY;
        }
    }
}

static void ghost_defeated_step(struct task *task, struct task_work *work,
                                struct enemy_work *enemy)
{
    if (FADE(enemy) > UNSEEN) {
        FADE(enemy) -= FADE_STEP;
        work->scl.y += STRETCH_STEP;
    } else {
        work->mode = MODE_DEAD;
        if (task->placement) {
            placement_stop(task);
        }
    }
}

/* put a GHOST_CAPTURE on the player it holds. for a character the switch
 * leaves out, such as PLAYER_EGGMAN, offset and scale stay unset */
static void ghost_stick_to_player(int player, struct task_work *work)
{
    NJS_POINT3 offset;
    Float scale;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&player_work[player]->collision->shapes->center);
#ifdef NONMATCHING
    /* under NONMATCHING, a character the switch leaves out takes the offset
     * and scale of one on foot. the original instead reads FR4 as
     * njTranslateEx left it, and stack words no path writes. C can't
     * reproduce either */
    scale = HOLD_SCALE;
    offset.x = HOLD_OFFSET_X;
    offset.y = HOLD_OFFSET_Y;
    offset.z = 0.0F;
#endif
    switch (player_records[player]->character) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_TAILS:
    case PLAYER_ROUGE:
        scale = HOLD_SCALE;
        offset.x = HOLD_OFFSET_X;
        offset.y = HOLD_OFFSET_Y;
        offset.z = 0.0F;
        break;
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        scale = HOLD_SCALE_MECH;
        offset.x = HOLD_OFFSET_X;
        offset.y = HOLD_OFFSET_Y_MECH;
        offset.z = 0.0F;
        break;
    }
    work->scl.x = scale;
    work->ang = player_work[player]->ang;
    work->ang.y = -work->ang.y;
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &work->pos);
    njPopMatrixEx();
}

/* a GHOST_CAPTURE that touches a player catches it. otherwise a hit beats the
 * ghost, or shrinks a GHOST_BIG */
static void ghost_check_hit(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    struct collision_work *collision;
    int player;
    int i;
    int score;

    collision = work->collision;
    if (work->submode == GHOST_CAPTURE &&
        (collision->flags & COLLISION_TOUCHED)) {
        player = player_slot_of(collision->hit->owner);
        if (player != -1) {
            player_request(player, PLAYER_REQUEST_CAPTURE, 0);
            work->mode = MODE_HOLD;
            work->timer = 0;
            enemy->motion.next = POSE_CATCH;
            collision->shapes->flags |= SHAPE_OFF;
            HELD_PLAYER(work) = player;
            ghost_stick_to_player(player, work);
            sound_play_at(SOUND_CATCH, NULL, 0, SOUND_LOUDEST, &work->pos);
            return;
        }
    } else if (enemy_take_hit(work, enemy)) {
        /* the record is null for a GHOST_HIDE, and where collision_init had
         * no room for it */
        /* @hack spelling: work->collision read again in this test and the arms
         * below, where collision would do */
        if (work->collision == NULL) {
            return;
        }
        if (work->mode == MODE_BIG) {
            work->mode = MODE_MEDIUM;
            work->collision->shapes[SIZE_BIG].flags |= SHAPE_OFF;
            work->timer = 0;
            GUARD_STEPS(enemy) = GUARD_TIME;
            work->flags &= ~WORK_HURT;
            /* @hack spelling: a while that counts i down, where a for that
             * counts up would do */
            i = BIG_DUST_PUFFS;
            while (i > 0) {
                dust_short_sphere(&work->pos, BIG_DUST_RADIUS, BIG_DUST_SPEED,
                                  BIG_DUST_SIZE);
                i--;
            }
        } else if (work->mode == MODE_MEDIUM) {
            work->flags &= ~WORK_HURT;
            if (GUARD_STEPS(enemy) <= 0) {
                work->mode = MODE_SHRUNK;
                work->collision->shapes[SIZE_MEDIUM].flags |= SHAPE_OFF;
                work->timer = 0;
                GUARD_STEPS(enemy) = GUARD_TIME;
                work->flags &= ~WORK_HURT;
                /* @hack spelling: a while that counts i down, where a for that
                 * counts up would do */
                i = MEDIUM_DUST_PUFFS;
                while (i > 0) {
                    dust_short_sphere(&work->pos, MEDIUM_DUST_RADIUS,
                                      MEDIUM_DUST_SPEED, MEDIUM_DUST_SIZE);
                    i--;
                }
            }
        } else if (work->mode == MODE_SHRUNK) {
            if (GUARD_STEPS(enemy) <= 0) {
                work->mode = MODE_DEFEATED;
                work->collision->shapes[SIZE_SHRUNK].flags |= SHAPE_OFF;
                work->timer = 0;
                enemy_set_home(work, enemy);
                score = BIG_SCORE;
                if (player_touch_attacks(task)) {
                    score = BIG_SCORE_KNOCK;
                }
                score_add_enemy(score);
                enemy_defeat(work, enemy);
            }
        } else if (work->submode != GHOST_BIG || GUARD_STEPS(enemy) <= 0) {
            work->mode = MODE_DEFEATED;
            work->collision->shapes[0].flags |= SHAPE_OFF;
            work->timer = 0;
            enemy_set_home(work, enemy);
            if (work->submode == GHOST_RUSH) {
                score = RUSH_SCORE;
                if (player_touch_attacks(task)) {
                    score = RUSH_SCORE_KNOCK;
                }
                score_add_enemy(score);
            }
            enemy_defeat(work, enemy);
        }
    }
    if (work->submode == GHOST_BIG && GUARD_STEPS(enemy) > 0) {
        GUARD_STEPS(enemy)--;
    }
}

static void ghost_mark_player_found(struct task_work *work,
                                    struct enemy_work *enemy)
{
    int player;

    switch (work->submode) {
    case GHOST_GO_AROUND:
    case GHOST_HIDE:
        player = enemy_find_player(work, enemy);
        break;
    default:
        player = enemy_find_player_near(work, enemy);
        break;
    }
    if (player) {
        work->flags |= GHOST_ALERT;
    } else {
        work->flags &= ~GHOST_ALERT;
    }
}

/* move the ghost a step by spd. spd.x lies along ang.y, and spd.z across it */
static void ghost_move(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    enemy->spd.x += ACCEL(enemy).x;
    enemy->spd.z += ACCEL(enemy).z;
    if (work->mode == MODE_BIG) {
        enemy->spd.x = BIG_SPEED;
    } else if (enemy->spd.x > SPEED_MAX && work->mode == MODE_PURSUE) {
        if (enemy->spd.x > PURSUE_SPEED_MAX) {
            enemy->spd.x = PURSUE_SPEED_MAX;
        }
    } else if (enemy->spd.x > SPEED_MAX) {
        enemy->spd.x = SPEED_MAX;
    } else if (enemy->spd.x < SPEED_MIN) {
        enemy->spd.x = SPEED_MIN;
    }
    work->pos.x += (enemy->spd.x * njCos(work->ang.y)) +
                   (enemy->spd.z * njSin(work->ang.y));
    work->pos.y += enemy->spd.y;
    work->pos.z += (-enemy->spd.x * njSin(work->ang.y)) +
                   (enemy->spd.z * njCos(work->ang.y));
    enemy_meet_stage(work, enemy);
    if (enemy->probe.pos.y + enemy->bottom >
        enemy->probe.sides[SURFACE_NEG_Y].pos) {
        enemy->spd.y -= SINK_STEP;
        if (enemy->spd.y < SINK_MAX) {
            enemy->spd.y = SINK_MAX;
        }
    } else {
        enemy->spd.y *= FALL_DAMPING;
        if (njAbs(enemy->spd.y) < FALL_STOP) {
            enemy->spd.y = 0.0F;
        }
    }
}

/* move a GHOST_GO_AROUND a step round its circle about home. a new leg starts
 * once LEG_STEPS runs out. the ghost faces the way it goes round, or faces
 * home while it rests */
static void ghost_circle(struct task_work *work, struct enemy_work *enemy)
{
    Float radius;
    Float circumference;
    Angle heading;

    if (game_state.time_stopped) {
        return;
    }
    radius = work->scl.z + HOME_RANGE;
    if (LEG_STEPS(enemy) == 0) {
        LEG_STEPS(enemy) = RANDOM_FROM(LEG_MIN, LEG_SPREAD);
        circumference = radius * (2.0F * NJD_PI);
        switch ((int)njRoundOff(rand() * (3.0F * RAND_SCALE))) {
        case 0:
            enemy->turn_speed = (CIRCLE_PACE * FULL_TURN) / circumference;
            enemy->motion.next = POSE_SWAY;
            break;
        case 1:
            enemy->turn_speed = -(CIRCLE_PACE * FULL_TURN) / circumference;
            enemy->motion.next = POSE_SWAY;
            break;
        case 2:
        /* @hack redundant: case 3, which njRoundOff of a value below 3 never
         * gives */
        case 3:
            enemy->turn_speed = 0;
            enemy->motion.next = POSE_STILL;
            break;
        }
    } else {
        LEG_STEPS(enemy)--;
    }
    CIRCLE_TURN(enemy) =
        angle_approach(CIRCLE_TURN(enemy), enemy->turn_speed, CIRCLE_EASE);
    heading = CIRCLE_BEARING(enemy);
    if (angle_difference(0, enemy->turn_speed) > 0) {
        heading += QUARTER_TURN;
    } else if (angle_difference(0, enemy->turn_speed) < 0) {
        heading += HALF_TURN + QUARTER_TURN;
        /* @hack spelling: a third test of the turn's sign, which always holds
         * where it is reached, where an else would do */
    } else if (angle_difference(0, enemy->turn_speed) == 0) {
        heading += HALF_TURN;
    }
    work->ang.y = angle_approach(work->ang.y, heading, HEADING_TURN);
    CIRCLE_BEARING(enemy) += CIRCLE_TURN(enemy);
    work->pos.x = (radius * njCos(CIRCLE_BEARING(enemy))) + enemy->home.x;
    work->pos.z = (-radius * njSin(CIRCLE_BEARING(enemy))) + enemy->home.z;
}

static void ghost_coast(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    enemy->spd.x *= COAST_DAMPING;
    work->pos.x += enemy->spd.x * njCos(work->ang.y);
    work->pos.z += -enemy->spd.x * njSin(work->ang.y);
}

static void ghost_destroy(struct task *task)
{
    struct ghost_parts *parts;
    int i;

    parts = task->extra.p;
    enemy_destroy(task);
    for (i = 0; i < GHOST_NODES; i++) {
        if (parts->jiggle[i]) {
            jiggle_free(parts->jiggle[i]);
            parts->jiggle[i] = NULL;
        }
    }
}

/* scare the nearest player, and fly at it. the player's look, not its
 * character, picks its sound or voice. a look the switch leaves out, such as
 * PLAYER_KIND_AMY, plays neither */
static void ghost_scare(struct task_work *work, struct enemy_work *enemy)
{
    struct task_work *target;
    int player;
    /* @hack spelling: mode, a local for the player's mode, where the mode read
     * at its test would do */
    int mode;
    int sound;
    int voice;

    work->mode = MODE_SCARE;
    work->timer = 0;
    enemy->motion.next = POSE_SCARE;
    player = player_nearest(&work->pos);
    target = player_work[player];
    work->ang.y = (Angle)(atan2f(target->pos.x - work->pos.x,
                                 target->pos.z - work->pos.z) *
                          ANGLE_PER_RADIAN) -
                  QUARTER_TURN;
    enemy->spd.x = SCARE_SPEED;
    mode = target->mode;
    sound_play_at(SOUND_SCARE, NULL, 0, SOUND_LOUDEST, &work->pos);
    if ((target->flags & PLAYER_WORK_GROUNDED) && mode < PLAYER_MODE_UNK_3B) {
        voice = -1;
        sound = -1;
        player_request(player, PLAYER_REQUEST_SCARE, SCARE_ANIMATION);
        switch (player_records[player]->kind) {
        case PLAYER_SONIC:
            sound = SOUND_SCARED_SONIC;
            break;
        case PLAYER_KNUCKLES:
            sound = SOUND_SCARED_KNUCKLES;
            break;
        case PLAYER_ROUGE:
            sound = SOUND_SCARED_ROUGE;
            break;
        case PLAYER_SHADOW:
            voice = VOICE_SCARED_SHADOW;
            break;
        case PLAYER_TAILS_MECH:
            voice = VOICE_SCARED_TAILS_MECH;
            break;
        case PLAYER_EGGMAN_MECH:
            voice = VOICE_SCARED_EGGMAN_MECH;
            break;
        }
        if (sound != -1) {
            /* @hack spelling: player_work read again for the player's pos,
             * where target would do */
            sound_play_at(sound, NULL, 0, SOUND_LOUDEST,
                          &player_work[player]->pos);
        }
        if (voice != -1) {
            voice_play_priority_2(voice);
        }
    }
    vibration_start_fading(player, SCARE_POWER, SCARE_FREQ, SCARE_LENGTH);
}

static void ghost_hide_step(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    if (enemy_find_player(work, enemy)) {
        work->flags |= GHOST_ALERT;
        ghost_scare(work, enemy);
    } else {
        work->flags &= ~GHOST_ALERT;
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    ghost_check_hit(task, work, enemy);
    /* @hack redundant: a return at the end, where falling off it would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* the step of MODE_WANDER, MODE_UNUSED, MODE_BIG and MODE_MEDIUM. the ghost
 * moves only while its FADE is at least 0 */
static void ghost_wander_step(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    Angle last_y;

    ghost_mark_away(work, enemy);
    ghost_mark_player_found(work, enemy);
    if (work->flags & GHOST_ALERT) {
        if (work->submode == GHOST_RUSH) {
            work->mode = MODE_CHASE;
            enemy->motion.next = POSE_CHASE;
            work->timer = 0;
        } else if (work->submode == GHOST_CAPTURE) {
            work->mode = MODE_PURSUE;
            enemy->motion.next = POSE_CHASE;
            work->timer = 0;
        }
    }
    if (FADE(enemy) >= 0.0F) {
        last_y = work->ang.y;
        ghost_turn(work, enemy);
        if (angle_distance(last_y, work->ang.y) > TURN_SLOW &&
            enemy->spd.x > SPEED_MIN) {
            enemy->spd.x *= COAST_DAMPING;
        }
        ghost_move(work, enemy);
        if (angle_distance(last_y, work->ang.y) < TURN_STILL) {
            enemy->motion.next = POSE_SWAY;
        } else {
            enemy->motion.next = POSE_STILL;
        }
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    /* @bug logic: a GHOST_CAPTURE that has let go of a player, or given up a
     * chase, stays faded out with its shape off, and never catches again. the
     * catch sets SHAPE_OFF (OR R11,R2 at 0x78c of the object's code), and so
     * does the give-up (OR #16,R0 at 0x1302). only this fade-in clears it,
     * and it skips a GHOST_CAPTURE (CMP/EQ #3,R0 at 0x1100) */
    if (work->submode != GHOST_CAPTURE) {
        if (FADE(enemy) < 0.0F) {
            FADE(enemy) += FADE_STEP;
            work->collision->shapes->flags |= SHAPE_OFF;
        } else if (work->submode != GHOST_BIG) {
            FADE(enemy) = 0.0F;
            work->collision->shapes->flags &= ~SHAPE_OFF;
        }
    }
    ghost_check_hit(task, work, enemy);
    /* @hack redundant: a return at the end, where falling off it would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* the step of MODE_CHASE and MODE_SHRUNK */
static void ghost_chase_step(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    Angle last_y;

    work->flags &= ~GHOST_AWAY;
    work->flags |= GHOST_ALERT;
    last_y = work->ang.y;
    ghost_turn(work, enemy);
    if (angle_distance(last_y, work->ang.y) > TURN_SLOW &&
        enemy->spd.x > SPEED_MIN) {
        enemy->spd.x *= COAST_DAMPING;
    }
    ACCEL(enemy).x = CHASE_ACCEL;
    ghost_move(work, enemy);
    if (enemy->spd.x >= SPEED_MIN) {
        enemy->motion.next = POSE_SWAY;
    } else {
        enemy->motion.next = POSE_STILL;
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    if (work->timer > CHASE_STEPS && work->mode == MODE_CHASE) {
        work->collision->shapes->flags |= SHAPE_OFF;
        FADE(enemy) -= FADE_STEP;
        if (FADE(enemy) <= UNSEEN) {
            work->pos = enemy->home;
            work->mode = MODE_WANDER;
            work->timer = 0;
            enemy->motion.next = POSE_STILL;
            ACCEL(enemy).x = 0.0F;
        }
    }
    ghost_check_hit(task, work, enemy);
}

/* the step of MODE_PURSUE. unlike ghost_chase_step, it leaves the pose at
 * POSE_CHASE */
static void ghost_pursue_step(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    Angle last_y;

    work->flags &= ~GHOST_AWAY;
    work->flags |= GHOST_ALERT;
    last_y = work->ang.y;
    ghost_turn(work, enemy);
    if (angle_distance(last_y, work->ang.y) > TURN_SLOW &&
        enemy->spd.x > SPEED_MIN) {
        enemy->spd.x *= COAST_DAMPING;
    }
    ACCEL(enemy).x = CHASE_ACCEL;
    ghost_move(work, enemy);
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    if (work->timer > CHASE_STEPS) {
        work->collision->shapes->flags |= SHAPE_OFF;
        FADE(enemy) -= FADE_STEP;
        if (FADE(enemy) <= UNSEEN) {
            work->pos = enemy->home;
            work->mode = MODE_WANDER;
            work->timer = 0;
            enemy->motion.next = POSE_STILL;
        }
    }
    ghost_check_hit(task, work, enemy);
}

static void ghost_circle_step(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    ghost_circle(work, enemy);
    ghost_mark_player_found(work, enemy);
    if (work->flags & GHOST_ALERT) {
        ghost_scare(work, enemy);
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    ghost_check_hit(task, work, enemy);
    /* @hack redundant: a return at the end, where falling off it would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* once a scare has faded the ghost out, a GHOST_HIDE ends through
 * MODE_DEFEATED, which stops its placement. a GHOST_MAKIKO goes straight to
 * MODE_DEAD, which leaves its placement active */
static void ghost_scare_step(struct task_work *work, struct enemy_work *enemy)
{
    ghost_coast(work, enemy);
    if (game_state.time_stopped) {
        return;
    }
    if (work->timer++ >= SCARE_COAST_STEPS) {
        if (FADE(enemy) <= UNSEEN) {
            if (work->submode == GHOST_MAKIKO) {
                work->mode = MODE_DEAD;
            } else if (work->submode == GHOST_HIDE) {
                work->mode = MODE_DEFEATED;
            } else {
                /* @bug logic: a GHOST_GO_AROUND waits faded out in MODE_GONE
                 * with its shape on (MOV #15,R0 at 0x140e of the object's
                 * code), so it still pushes players. a chase that gives up
                 * turns the shape off as the ghost fades (OR #16,R0 at
                 * 0x122c) */
                work->mode = MODE_GONE;
            }
            work->timer = 0;
        } else {
            FADE(enemy) -= SCARE_FADE_STEP;
        }
    }
}

static void ghost_hold_step(struct task_work *work, struct enemy_work *enemy)
{
    if (player_work[HELD_PLAYER(work)]->mode != PLAYER_MODE_CAUGHT) {
        work->mode = MODE_RELEASE;
        work->timer = 0;
        ACCEL(enemy).x = 0.0F;
        enemy->motion.next = POSE_RELEASE;
        enemy->motion.frame = catch_motion->nbFrame - 1.0F;
        sound_play_at(SOUND_RELEASE, NULL, 0, SOUND_LOUDEST, &work->pos);
    } else {
        ghost_stick_to_player(HELD_PLAYER(work), work);
    }
    /* @bug logic: shakes the pad at every step of a hold, not once every
     * HOLD_SHAKE_PERIOD steps. the test reads the timer, which the catch set
     * to 0, and nothing in MODE_HOLD advances it (TST #127,R0 at 0x14cc of
     * the object's code) */
    if ((work->timer & (HOLD_SHAKE_PERIOD - 1)) == 0) {
        vibration_start_fading(HELD_PLAYER(work), HOLD_POWER, HOLD_FREQ,
                               HOLD_LENGTH);
    }
    if (FADE(enemy) > HOLD_FADE) {
        FADE(enemy) -= FADE_STEP;
    } else {
        FADE(enemy) = HOLD_FADE;
    }
}

static void ghost_release_step(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    if (FADE(enemy) <= UNSEEN) {
        work->pos = enemy->home;
        work->mode = MODE_WANDER;
        work->timer = 0;
        enemy->motion.next = POSE_STILL;
        ACCEL(enemy).x = 0.0F;
    } else {
        FADE(enemy) -= FADE_STEP;
    }
    /* @hack redundant: a return at the end, where falling off it would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void ghost_float_step(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    ghost_mark_player_found(work, enemy);
    ghost_turn(work, enemy);
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    ghost_check_hit(task, work, enemy);
    /* @hack redundant: a return at the end, where falling off it would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* a hit beats a GHOST_MAKIKO, which then scares the nearest player */
static void ghost_makiko_step(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    ghost_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        ghost_scare(work, enemy);
    }
}

/* draw the ghost at its FADE, except in MODE_HIDE */
static void ghost_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct ghost_parts *parts = task->extra.p;
    struct angles ang;
    NJS_VECTOR scl;

    light_apply(LIGHT_EFFECT);
    /* @hack redundant: a block no path reaches. the image holds none of its
     * code, but the block gives the code after it its registers */
    if (0) {
        njSetTexture(&ghost_texlist);
        njCnkSimpleDrawObject(parts->motion[0].object);
    }
    /* @hack spelling: a while whose body jumps past the draw, where an if
     * around the draw would do */
    while (work->mode == MODE_HIDE) {
        goto lights;
    }
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_attr_save();
    draw_attr_set(0, NJD_FST_UA);
    draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
    constant_material_set(FADE(enemy), 0.0F, 0.0F, 0.0F);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    if (work->scl.y != 1.0F) {
        njScale(NULL, 1.0F, work->scl.y, 1.0F);
    }
    /* @hack redundant: the test of ghost_texlist's address, which is never
     * null, where no test would do */
    /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
    if (&ghost_texlist) {
        if (enemy->motion.next == POSE_STILL ||
            enemy->motion.next == POSE_SWAY) {
            if ((BLINK_STEPS(enemy) & (BLINK_PHASE - 1)) > EYE03_ABOVE) {
                njSetTexture(&ghost_3_texlist);
            } else if (BLINK_STEPS(enemy) & BLINK_PHASE) {
                njSetTexture(&ghost_2_texlist);
            } else {
                njSetTexture(&ghost_texlist);
            }
        } else {
            njSetTexture(&ghost_texlist);
        }
    }
    if (enemy->motion.next == POSE_CATCH ||
        enemy->motion.next == POSE_RELEASE) {
        njCnkSimpleDrawShapeMotion(&catch_ghost_object, catch_motion,
                                   &catch_shape, enemy->motion.frame);
    } else if (enemy->motion.next == POSE_HOLD) {
        njCnkSimpleDrawMotion(&hold_ghost_object, &hold_motion,
                              enemy->motion.frame);
    } else if (enemy->motion.next == POSE_SCARE) {
        njCnkSimpleDrawMotion(&hide_ghost_object, &scare_motion,
                              enemy->motion.frame);
    } else if (work->mode == MODE_MEDIUM) {
        njCnkSimpleDrawMotion(parts->motion[0].object, &medium_motion,
                              enemy->motion.frame);
    } else if (work->mode == MODE_SHRUNK || work->mode == MODE_CHASE) {
        njCnkSimpleDrawMotion(parts->motion[0].object, &chase_motion,
                              enemy->motion.frame);
    } else {
        njCnkSimpleDrawObject(parts->motion[0].object);
    }
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
lights:
    light_apply(light_after_draw);
    /* @hack redundant: a readout of the ghost's settings, which no path
     * reaches. the image holds the code of its switch's cases and of the
     * draws after them, and the strings of the whole block. it holds none of
     * the code of the statements before the switch, which give the kept code
     * its registers and the strings their order */
    if (0) {
        int kind = work->submode;

        (void)GHOST_DEBUG_YAW_AND_EMERALD_ID;
        if (kind >= GHOST_KINDS || kind < 0) {
            kind = GHOST_GO_AROUND;
        }
        njPrintC(0, GHOST_DEBUG_KIND);
        switch (kind) {
        case GHOST_GO_AROUND:
        case GHOST_RUSH:
        case GHOST_CAPTURE:
        case GHOST_BIG:
            njPrintC(NJM_LOCATION(17, 17), GHOST_DEBUG_YAW_AND_EMERALD_ID);
            njPrintD(NJM_LOCATION(32, 17), (Uint8)work->ang.y, 3);
            njPrintC(NJM_LOCATION(17, 18), GHOST_DEBUG_KIND);
            njPrintC(NJM_LOCATION(17, 20), GHOST_DEBUG_VIEW_RANGE);
            njPrintC(NJM_LOCATION(17, 21), GHOST_DEBUG_MOVE_RANGE);
            break;
        case GHOST_HIDE:
        case GHOST_FLOAT:
            njPrintC(NJM_LOCATION(17, 18), GHOST_DEBUG_KIND);
            njPrintC(NJM_LOCATION(17, 20), GHOST_DEBUG_VIEW_RANGE);
            break;
        case GHOST_MAKIKO:
            njPrintC(NJM_LOCATION(17, 18), GHOST_DEBUG_KIND);
            break;
        }
        if (kind != GHOST_MAKIKO) {
            ang.x = ang.y = ang.z = 0;
            scl.z = scl.x = work->scl.y + SIGHT_RANGE;
            scl.y = 20.0F;
            debug_draw_model_unlit(&work->pos, &ang, &scl,
                                   DEBUG_MODEL_CYLINDER);
        }
        if (kind != GHOST_HIDE && kind != GHOST_FLOAT && kind != GHOST_MAKIKO) {
            ang.x = ang.y = ang.z = 0;
            scl.z = scl.x = work->scl.z + HOME_RANGE;
            scl.y = 1.0F;
            debug_draw_model_unlit(&work->pos, &ang, &scl,
                                   DEBUG_MODEL_CYLINDER);
        }
    }
}

/* parts_build_medium gives the parts new jiggles of medium_ghost_object's
 * nodes, and parts_build_ghost of ghost_object's. each overwrites the parts'
 * jiggles, which the caller must free first */
static void parts_build_medium(struct ghost_parts *parts)
{
    struct jiggle *jiggle;
    struct enemy_motion *part;
    NJS_CNK_OBJECT **nodes;
    int i;

    part = parts->motion;
    nodes = medium_ghost_nodes;
    for (i = 0; i < GHOST_NODES; i++) {
        /* @hack spelling: one assignment to the jiggle and its slot, where a
         * statement for each would do */
        parts->jiggle[i] = jiggle = jiggle_create(*nodes++);
        jiggle_set_medium(jiggle);
        part->object = jiggle->copy;
        part++;
    }
    parts->jiggle[0]->copy->child = parts->jiggle[1]->copy;
    parts->jiggle[1]->copy->sibling = parts->jiggle[2]->copy;
}

static void parts_build_ghost(struct ghost_parts *parts)
{
    struct jiggle *jiggle;
    struct enemy_motion *part;
    NJS_CNK_OBJECT **nodes;
    int i;

    part = parts->motion;
    nodes = ghost_nodes;
    for (i = 0; i < GHOST_NODES; i++) {
        /* @hack spelling: one assignment to the jiggle and its slot, where a
         * statement for each would do */
        parts->jiggle[i] = jiggle = jiggle_create(*nodes++);
        jiggle_set_ghost(jiggle);
        part->object = jiggle->copy;
        part++;
    }
    parts->jiggle[0]->copy->child = parts->jiggle[1]->copy;
    parts->jiggle[1]->copy->sibling = parts->jiggle[2]->copy;
}

void ghost_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct ghost_parts *parts = task->extra.p;
    struct enemy_motion *part = parts->motion;
    int i;
    struct jiggle **kind_at;
    struct jiggle **step_at;
    struct jiggle **phase_at;

    if (work->mode != MODE_INIT && task->parent == NULL &&
        placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        ghost_init(task, work);
        return;
    case MODE_UNUSED:
    case MODE_WANDER:
        ghost_wander_step(task, work, enemy);
        break;
    case MODE_CHASE:
        ghost_chase_step(task, work, enemy);
        break;
    case MODE_CIRCLE:
        ghost_circle_step(task, work, enemy);
        break;
    case MODE_HIDE:
        ghost_hide_step(task, work, enemy);
        break;
    case MODE_SCARE:
        ghost_scare_step(work, enemy);
        break;
    case MODE_PURSUE:
        ghost_pursue_step(task, work, enemy);
        break;
    case MODE_HOLD:
        ghost_hold_step(work, enemy);
        break;
    case MODE_RELEASE:
        ghost_release_step(work, enemy);
        break;
    case MODE_FLOAT:
        ghost_float_step(task, work, enemy);
        break;
    case MODE_BIG:
        ghost_wander_step(task, work, enemy);
        if (work->mode == MODE_MEDIUM) {
            for (i = 0; i < GHOST_NODES; i++) {
                if (parts->jiggle[i]) {
                    jiggle_free(parts->jiggle[i]);
                    parts->jiggle[i] = NULL;
                }
            }
            parts_build_medium(parts);
            work->collision->shapes[SIZE_BIG].flags |= SHAPE_OFF;
            work->collision->shapes[SIZE_MEDIUM].flags &= ~SHAPE_OFF;
            work->collision->shapes[SIZE_SHRUNK].flags |= SHAPE_OFF;
        }
        break;
    case MODE_MEDIUM:
        ghost_wander_step(task, work, enemy);
        if (work->mode == MODE_SHRUNK) {
            for (i = 0; i < GHOST_NODES; i++) {
                if (parts->jiggle[i]) {
                    jiggle_free(parts->jiggle[i]);
                    parts->jiggle[i] = NULL;
                }
            }
            parts_build_ghost(parts);
            work->collision->shapes[SIZE_BIG].flags |= SHAPE_OFF;
            work->collision->shapes[SIZE_MEDIUM].flags |= SHAPE_OFF;
            work->collision->shapes[SIZE_SHRUNK].flags &= ~SHAPE_OFF;
        }
        break;
    case MODE_SHRUNK:
        ghost_chase_step(task, work, enemy);
        break;
    case MODE_MAKIKO:
        ghost_makiko_step(task, work, enemy);
        break;
    case MODE_GONE:
        break;
    case MODE_DEFEATED:
        ghost_defeated_step(task, work, enemy);
        break;
    case MODE_DEAD:
        task_kill(task);
        /* @hack spelling: a goto to the end, where a return would do */
        goto end;
    }
    /* @hack spelling: the empty then-arm and its else, where the test negated
     * would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        /* @hack spelling: kind_at, step_at and phase_at, walks over the
         * jiggles beside i, where the jiggles indexed by i would do */
        /* @hack spelling: i set before the for, where its first clause would
         * do */
        i = 0;
        step_at = parts->jiggle;
        kind_at = parts->jiggle;
        phase_at = parts->jiggle;
        /* @hack spelling: kind_at and phase_at stepped by an assignment, where
         * ++ would do */
        for (; i < GHOST_NODES; i++, part++, step_at++,
                                kind_at = kind_at + 1,
                                phase_at = phase_at + 1) {
            part->next = enemy->motion.next;
            if (parts->jiggle[i]) {
                if (work->submode == GHOST_BIG) {
                    if (part->next == POSE_SWAY) {
                        parts->jiggle[i]->kind = JIGGLE_WAVE_SWAY_WIDE;
                    } else {
                        parts->jiggle[i]->kind = JIGGLE_WAVE_TWIN;
                    }
                } else if (work->submode == GHOST_GO_AROUND) {
                    if (part->next == POSE_SWAY) {
                        parts->jiggle[i]->kind = JIGGLE_WAVE_SWAY;
                        if (parts->jiggle[i]->phase > FULL_TURN) {
                            sound_play_at_for(SOUND_SWAY, work, SWAY_PRIORITY,
                                              SWAY_VOLUME, SWAY_DURATION,
                                              &work->pos);
                            (*phase_at)->phase &= FULL_TURN - 1;
                        }
                    } else {
                        (*kind_at)->kind = JIGGLE_WAVE;
                    }
                }
                jiggle_step(*step_at);
            }
        }
        if (work->mode == MODE_BIG) {
            if (BLINK_STEPS(enemy) <= 0 && BLINK_WAIT(enemy) <= 0) {
                BLINK_STEPS(enemy) = BIG_BLINK;
                BLINK_WAIT(enemy) =
                    RANDOM_FROM(BIG_BLINK_WAIT_MIN, BIG_BLINK_WAIT_SPREAD);
            } else if (BLINK_WAIT(enemy) != 0) {
                BLINK_WAIT(enemy)--;
            } else {
                BLINK_STEPS(enemy)--;
            }
        } else if (BLINK_STEPS(enemy) <= 0 && BLINK_WAIT(enemy) <= 0) {
            BLINK_STEPS(enemy) =
                (int)RANDOM_FROM(BLINK_MIN, BLINK_SPREAD) & ~(BLINK_PHASE - 1);
            BLINK_WAIT(enemy) = RANDOM_FROM(BLINK_WAIT_MIN, BLINK_WAIT_SPREAD);
        } else if (BLINK_WAIT(enemy) != 0) {
            BLINK_WAIT(enemy)--;
        } else {
            BLINK_STEPS(enemy)--;
        }
        if (enemy->motion.next == POSE_SCARE) {
            enemy->motion.frame += FRAME_STEP;
            /* @hack spelling: a while whose body ends the loop, where an if
             * would do */
            while (enemy->motion.frame > scare_motion.nbFrame - 1.0F) {
                enemy->motion.frame = scare_motion.nbFrame - 1.0F;
            }
        } else if (enemy->motion.next == POSE_CATCH) {
            enemy->motion.frame += FRAME_STEP;
            if (enemy->motion.frame > catch_motion->nbFrame - 1.0F) {
                enemy->motion.frame = 0.0F;
                enemy->motion.next = POSE_HOLD;
            }
        } else if (enemy->motion.next == POSE_HOLD) {
            enemy->motion.frame += FRAME_STEP;
            while (enemy->motion.frame > hold_motion.nbFrame) {
                enemy->motion.frame -= hold_motion.nbFrame;
            }
        } else if (enemy->motion.next == POSE_RELEASE) {
            enemy->motion.frame -= RELEASE_FRAME_STEP;
            if (enemy->motion.frame < 0.0F) {
                enemy->motion.frame = 0.0F;
            }
        } else if (work->mode == MODE_MEDIUM) {
            enemy->motion.frame += CHASE_FRAME_STEP;
            while (enemy->motion.frame >= medium_motion.nbFrame) {
                enemy->motion.frame -= medium_motion.nbFrame;
            }
        } else if (work->mode == MODE_SHRUNK || work->mode == MODE_CHASE) {
            enemy->motion.frame += CHASE_FRAME_STEP;
            while (enemy->motion.frame >= chase_motion.nbFrame) {
                enemy->motion.frame -= chase_motion.nbFrame;
            }
        }
    }
    enemy->flags &= ~ENEMY_LEDGE_HOLD_OFF;
    /* @hack redundant: an if (1) whose else never runs, where its then-arm
     * alone would do. the else repeats the switch's MODE_INIT case */
    if (1) {
        collision_register(task);
        return;
    } else {
        switch (work->mode) {
        case MODE_INIT:
            ghost_init(task, work);
            return;
        }
    }
end:;
}

static void path_ghost_set_route(struct task_work *work,
                                 struct enemy_work *enemy,
                                 struct enemy_route *route)
{
    /* @hack spelling: point, a local for the point's index, where the index
     * passed in place would do */
    unsigned int point;

    PATH_INDEX(enemy) = (work->ang.x & PATH_INDEX_BITS) >> PATH_INDEX_SHIFT;
    if ((Uint32)PATH_INDEX(enemy) >= GHOST_PATHS || PATH_INDEX(enemy) < 0) {
        PATH_INDEX(enemy) = 0;
    }
    route->path = ghost_paths[PATH_INDEX(enemy)];
    work->tick = (work->ang.x & PATH_FLAGS_BITS) >> PATH_FLAGS_SHIFT;
    point = njAbs(njFraction(work->scl.y) * START_SCALE);
    if (path_point_distance(route->path, point, &PATH_DISTANCE(enemy)) == 0) {
        PATH_DISTANCE(enemy) = 0.0F;
    }
}

static void jiggle_set_path(struct jiggle *jiggle)
{
    jiggle->kind = JIGGLE_WAVE_CENTERED;
    jiggle->param = 0.9F;
    jiggle->by_kind.angle = 0x3000;
    jiggle->phase_step = 0xbcc;
}

static void path_ghost_init(struct task *task, struct task_work *work)
{
    struct ghost_parts *parts;
    struct enemy_work *enemy;
    struct enemy_route *route;
    int i;
    struct jiggle *jiggle;
    struct enemy_motion *part;
    NJS_CNK_OBJECT **nodes;

    if ((parts = syCalloc(1, sizeof(struct ghost_parts))) == NULL) {
        return;
    }
    PATH_PARTS(task) = parts;
    enemy = enemy_work_alloc(task);
    route = syCalloc(1, sizeof(struct enemy_route));
    task->extra.p = route;
    path_ghost_set_route(work, enemy, route);
    collision_init(task, path_ghost_shapes, 1, COLLISION_ENEMY);
    FADE(enemy) = 0.0F;
    part = parts->motion;
    nodes = ghost_nodes;
    for (i = 0; i < GHOST_NODES; i++) {
        jiggle = jiggle_create(*nodes++);
        parts->jiggle[i] = jiggle;
        jiggle_set_path(jiggle);
        part->object = jiggle->copy;
        part++;
    }
    parts->jiggle[0]->copy->child = parts->jiggle[1]->copy;
    parts->jiggle[1]->copy->sibling = parts->jiggle[2]->copy;
    enemy->offset = work->scl;
    WAKE_RANGE(enemy) = njAbs(njFraction(work->scl.x) * WAKE_SCALE) + WAKE_MIN;
    PATH_TURN(enemy) = work->ang.y & PATH_TURN_BITS;
    enemy->turn_speed = PATH_TURN_SPEED;
    PATH_SPEED(enemy) = njAbs(njFraction(work->scl.z) * SPEED_SCALE);
    /* @hack spelling: one assignment to both angles, where a statement for
     * each would do */
    work->ang.x = work->ang.z = 0;
    work->scl.x = 1.0F;
    work->scl.y = 1.0F;
    enemy_set_home(work, enemy);
    work->mode = MODE_PATH_WAIT;
    enemy->motion.table = path_ghost_actions;
    enemy->motion.object = &ghost_object;
    enemy->motion.next = PATH_ACTION;
    enemy->shadow_size = PATH_SHADOW_SIZE;
    enemy->shadow_ratio = PATH_SHADOW_RATIO;
    enemy_motion_step(&enemy->motion);
    task->draw = path_ghost_draw;
    task->destructor = path_ghost_destroy;
    /* @hack redundant: the test of ghost_texlist's address, which is never
     * null, where the test of ghost_holder alone would do */
    /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
    if (&ghost_texlist && ghost_holder == NULL) {
        texture_load_pvm(GHOST_TEXTURE_ARCHIVE, &ghost_texlist);
        texture_load_pvm(GHOST_TEXTURE_ARCHIVE_2, &ghost_2_texlist);
        texture_load_pvm(GHOST_TEXTURE_ARCHIVE_3, &ghost_3_texlist);
        ghost_holder =
            task_create(0, 0, ghost_holder_step, GHOST_TEXTURE_TASK_NAME);
        ghost_holder->destructor = ghost_holder_destroy;
    }
}

static void path_ghost_defeated_step(struct task_work *work,
                                     struct enemy_work *enemy)
{
    if (FADE(enemy) > UNSEEN) {
        FADE(enemy) -= FADE_STEP;
        work->scl.y += STRETCH_STEP;
    } else {
        work->mode = MODE_PATH_GONE;
    }
}

/* a hit beats the path ghost. under ENEMY_IN_MAGIC_HAND, unless
 * game_state.time_stopped is set, it also leaves a ball holding ghost_object
 * for the player in shot_by */
static void path_ghost_check_hit(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    /* @hack spelling: player set to shot_by, where shot_by passed at its use
     * would do */
    int player;
    int score;
    NJS_VECTOR offset;
    NJS_VECTOR scl;

    if (enemy_take_hit(work, enemy) == 0) {
        return;
    }
    /* @hack spelling: the empty then-arm and its else, where the test negated
     * would do */
    if (game_state.time_stopped) {
    } else if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
        player = enemy->shot_by;
        offset.x = 0.0F;
        offset.y = BALL_OFFSET_Y;
        offset.z = 0.0F;
        /* @hack spelling: one assignment to the three parts, where a statement
         * for each would do */
        scl.x = scl.y = scl.z = BALL_SCALE;
        enemy_ball_create(player, &ghost_object, &ghost_texlist, &scl, &offset);
    }
    work->mode = MODE_PATH_DEFEATED;
    work->collision->shapes[0].flags |= SHAPE_OFF;
    work->timer = 0;
    enemy_set_home(work, enemy);
    score = PATH_SCORE;
    if (player_touch_attacks(task)) {
        score = PATH_SCORE_KNOCK;
    }
    score_add_enemy(score);
    enemy_defeat(work, enemy);
}

/* move the path ghost a step along its path, or on past its end. once it
 * reaches the path's last segment, the ghost leaves the path. it then hovers
 * under PATH_GHOST_STOP, and otherwise flies on along spd. it faces the way
 * it moves */
static void path_ghost_move(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    struct enemy_route *route;
    struct path *path;
    Float speed;
    unsigned int index;
    struct path_sample sample;
    /* @hack redundant: unused, 48 bytes that nothing reads, which size the
     * frame */
    NJS_VECTOR unused[4];
    NJS_VECTOR dir;
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
        speed = PATH_SPEED(enemy);
        if (work->tick & PATH_GHOST_STOP) {
            if (PATH_DISTANCE(enemy) >= path->length - STOP_DISTANCE) {
                speed =
                    speed * (path->length - PATH_DISTANCE(enemy)) * STOP_EASE;
                if (speed < STOP_MIN_SPEED) {
                    speed = STOP_MIN_SPEED;
                }
            }
        }
        PATH_DISTANCE(enemy) += speed;
        sample.distance = PATH_DISTANCE(enemy);
        /* @hack spelling: the empty then-arm and its else, where the test
         * negated would do */
        if (path_segment_at(path, PATH_DISTANCE(enemy), &index) != 0 &&
            index < (unsigned int)(path->count - 2) &&
            path_get_sample(path, &sample) != 0) {
        } else {
            work->mode = (work->tick & PATH_GHOST_STOP) ? MODE_PATH_HOVER
                                                        : MODE_PATH_FLY_ON;
            add(&work->pos, &enemy->spd);
            return;
        }
        if (work->tick & PATH_GHOST_MIRROR) {
            sample.pos.x = -sample.pos.x;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&enemy->home);
        njRotateY(NULL, PATH_TURN(enemy));
        njCalcPoint(NULL, &sample.pos, &work->pos);
        njPopMatrixEx();
        bob_sin = njSin(BOB_PHASE(enemy));
        BOB_PHASE(enemy) += BOB_TURN;
        work->pos.y += bob_sin * RIDE_BOB_HEIGHT;
        enemy->spd = work->pos;
        njSubVector(&enemy->spd, &enemy->last_pos);
        /* set ang.x and ang.z so that the ghost's up is the path's up at the
         * sample */
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
        dir = enemy->spd;
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

static void path_ghost_destroy(struct task *task)
{
    struct ghost_parts *parts;
    int i;

    parts = PATH_PARTS(task);
    enemy_destroy(task);
    for (i = 0; i < GHOST_NODES; i++) {
        if (parts->jiggle[i]) {
            jiggle_free(parts->jiggle[i]);
            parts->jiggle[i] = NULL;
        }
    }
}

/* the step of MODE_PATH_RIDE and MODE_PATH_FLY_ON */
static void path_ghost_ride_step(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    path_ghost_move(task, work, enemy);
    path_ghost_check_hit(task, work, enemy);
    if (work->mode != MODE_PATH_DEFEATED && !game_state.time_stopped) {
        work->timer++;
    }
}

/* hover, facing the nearest player */
static void path_ghost_hover(struct task_work *work, struct enemy_work *enemy)
{
    NJS_VECTOR dir;
    int player;
    /* @hack spelling: bob_cos, a local for the bob phase's cosine, where njCos
     * read in place, before the phase turns, would do */
    Float bob_cos;

    bob_cos = njCos(BOB_PHASE(enemy));
    BOB_PHASE(enemy) += BOB_TURN;
    work->pos.y += bob_cos * HOVER_BOB_RISE;
    player = player_nearest(&work->pos);
    work->ang.x = angle_approach(work->ang.x, 0, LEVEL_EASE);
    work->ang.z = angle_approach(work->ang.z, 0, LEVEL_EASE);
    /* @hack redundant: the test of player_nearest's result, which is never
     * -1, where the then-arm alone would do */
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

static void path_ghost_hover_step(struct task *task, struct task_work *work,
                                  struct enemy_work *enemy)
{
    path_ghost_hover(work, enemy);
    path_ghost_check_hit(task, work, enemy);
    if (work->mode != MODE_PATH_DEFEATED && !game_state.time_stopped) {
        work->timer++;
    }
}

static void path_ghost_wait_step(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    int i;
    struct task_work *player;

    pos = enemy->home;
    njAddVector(&pos, &enemy->offset);
    /* placement_stop and path_ghost_move run once for each player within
     * WAKE_RANGE of the wake point. a step with both players there moves the
     * ghost twice */
    for (i = 0; i < GAME_PLAYERS; i++) {
        player = player_work[i];
        if (player && njDistanceP2P(&player->pos, &pos) < WAKE_RANGE(enemy)) {
            work->mode = MODE_PATH_RIDE;
            placement_stop(task);
            path_ghost_move(task, work, enemy);
        }
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void fun_ghost_2c20(struct task *task, struct task_work *work,
                    struct enemy_work *enemy)
{
    struct enemy_route *route = task->extra.p;

    /* @hack redundant: calls no path reaches. the image holds none of their
     * code, but holds the load of route and the saves of the registers that
     * hold work and enemy */
    if (0) {
        path_ghost_set_route(work, enemy, route);
        path_ghost_hover(work, enemy);
        path_ghost_hover(work, enemy);
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* draw the path ghost's path as a triangle at each point but the last. the
 * triangles pulse and their colors cycle as route->frames counts up. only a
 * block of path_ghost_draw that no path reaches calls it */
static void path_ghost_draw_route(struct task *task)
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
    route_triangle[0].col = path_color;
    route_triangle[1].col = path_color;
    route_triangle[2].col = tip_color;
    path = route->path;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y & PATH_TURN_BITS);
    njDrawPolygon3DExStart(0);
    for (i = 0, point = path->points; i < path->count - 1; i++, point++) {
        normal.x = -njSin(point->slope_z) * njCos(point->slope_x);
        normal.y = njCos(point->slope_z) * njCos(point->slope_x);
        normal.z = njSin(point->slope_x);
        length =
            TIP_LENGTH +
            (TIP_GROWTH * (Float)((route->frames & TIP_FRAMES) >> TIP_SHIFT));
        route_triangle[0].x = point->pos.x;
        route_triangle[0].y = point->pos.y;
        route_triangle[0].z = point->pos.z;
        route_triangle[1].x = point[1].pos.x;
        route_triangle[1].y = point[1].pos.y;
        route_triangle[1].z = point[1].pos.z;
        route_triangle[2].x = point->pos.x + (length * normal.x);
        route_triangle[2].y = point->pos.y + (length * normal.y);
        route_triangle[2].z = point->pos.z + (length * normal.z);
        if (work->tick & PATH_GHOST_MIRROR) {
            route_triangle[0].x = -route_triangle[0].x;
            route_triangle[1].x = -route_triangle[1].x;
            route_triangle[2].x = -route_triangle[2].x;
        }
        njDrawPolygon3DExSetData(route_triangle, 3);
    }
    njDrawPolygon3DExEnd();
    njPopMatrixEx();
}

/* draw the path ghost at its FADE, except in MODE_PATH_WAIT */
static void path_ghost_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct ghost_parts *parts = PATH_PARTS(task);
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
    Float length;

    light_apply(LIGHT_EFFECT);
    if (work->mode != MODE_PATH_WAIT) {
        draw_control_save();
        draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        draw_attr_save();
        draw_attr_set(0, NJD_FST_UA);
        draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
        constant_material_set(FADE(enemy), 0.0F, 0.0F, 0.0F);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njSetTexture(&ghost_texlist);
        if (work->scl.y != 1.0F) {
            njScale(NULL, 1.0F, work->scl.y, 1.0F);
        }
        njCnkSimpleDrawObject(parts->motion[0].object);
        njPopMatrixEx();
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
        /* @hack spelling: an empty else, where the if alone would do */
    } else {
    }
    light_apply(light_after_draw);
    /* @hack redundant: a block no path reaches. the image holds its strings
     * and its locals' room in the frame, but none of its code. its calls give
     * the code before the block its registers */
    if (0) {
        (void)GHOST_PATH_TITLE;
        (void)GHOST_PATH_DEBUG_YAW;
        (void)GHOST_PATH_DEBUG_ID;
        (void)GHOST_PATH_DEBUG_NUMBER;
        (void)GHOST_PATH_DEBUG_REVERSE;
        (void)GHOST_PATH_DEBUG_STOP;
        (void)GHOST_PATH_DEBUG_SPACE;
        (void)GHOST_PATH_DEBUG_SEPARATOR;
        (void)GHOST_PATH_DEBUG_CLOSE;
        (void)GHOST_PATH_DEBUG_X_AND_RADIUS;
        (void)GHOST_PATH_DEBUG_Y_AND_START;
        (void)GHOST_PATH_DEBUG_Z_AND_SPEED;
        parts_build_medium(parts);
        parts_build_ghost(parts);
        parts_build_medium(parts);
        parts_build_ghost(parts);
        pos = enemy->home;
        njAddVector(&pos, &enemy->offset);
        ang.x = ang.y = ang.z = 0;
        scl.x = scl.z = WAKE_RANGE(enemy);
        scl.y = 1.0F;
        debug_draw_model_unlit(&pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
        /* @hack spelling: task->extra cast at its use, where a local route
         * would do */
        path_point_distance(((struct enemy_route *)task->extra.p)->path, 0,
                            &length);
        path_ghost_draw_route(task);
    }
}

/* the step of the path ghost, which nothing in any image reaches */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void path_ghost_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct ghost_parts *parts = PATH_PARTS(task);
    int i;

    /* @hack redundant: two calls no path reaches. the image holds none of
     * their code, but the calls give parts the register it takes */
    if (0) {
        parts_build_medium(parts);
        parts_build_medium(parts);
    }
    if (work->mode != MODE_PATH_INIT && work->mode != MODE_PATH_RIDE &&
        placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_PATH_INIT:
        /* @hack spelling: the init's call after the switch, reached by a goto,
         * where the call and a return in its case would do */
        goto init;
    case MODE_PATH_RIDE:
    case MODE_PATH_FLY_ON:
        path_ghost_ride_step(task, work, enemy);
        break;
    case MODE_PATH_HOVER:
        path_ghost_hover_step(task, work, enemy);
        break;
    case MODE_PATH_DEFEATED:
        path_ghost_defeated_step(work, enemy);
        break;
    case MODE_PATH_GONE:
        placement_kill(task);
        /* @hack spelling: a goto to the end, where a return would do */
        goto end;
    case MODE_PATH_WAIT:
        path_ghost_wait_step(task, work, enemy);
        return;
    }
    /* @hack spelling: the empty then-arm and its else, where the test negated
     * would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        for (i = 0; i < GHOST_NODES; i++) {
            if (parts->jiggle[i]) {
                jiggle_step(parts->jiggle[i]);
            }
        }
    }
    if (work->mode != MODE_PATH_DEFEATED) {
        collision_register(task);
    }
    return;
init:
    path_ghost_init(task, work);
    return;
end:;
}
/* NOLINTEND(misc-use-internal-linkage) */
