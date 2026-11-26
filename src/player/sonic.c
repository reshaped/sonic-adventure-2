/* Sonic and Shadow, and Amy and Metal Sonic in their places in a two-player
 * game. this holds their makers, and their task's step, draw and actions */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/motion.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/helpers.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "effect/bubbles.h"
#include "effect/chaos_spear.h"
#include "effect/ectoplasm.h"
#include "effect/powder.h"
#include "effect/sonic_effect.h"
#include "effect/speed_lines.h"
#include "effect/stars.h"
#include "effect/wind_cutter.h"
#include "enemy/enemy.h"
#include "event/mini_event.h"
#include "hud/action_window.h"
#include "hud/hud.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "player/sonic.h"
#include "player/trick_camera.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the size in bytes of a Sonic's record, which its task's extra points to.
 * its own fields are struct player's by_character.sonic */
#define SONIC_RECORD_SIZE 0x34c

/* the sounds of the task's moves, from the character's own bank. a sound
 * named for Sonic plays for Amy too, and one named for Shadow for Metal
 * Sonic */
#define SOUND_JUMP_SHADOW 0x2000
#define SOUND_JUMP_SONIC 0x2001
#define SOUND_BRAKE 0x2002
#define SOUND_HOMING 0x2007
#define SOUND_COMBO 0x2008
#define SOUND_COMBO_LAST 0x2009 /* the third move of either combo */
#define SOUND_BOUNCE 0x200a
#define SOUND_ROLL 0x200b
#define SOUND_TIME_STOP_SHADOW 0x200d
#define SOUND_SPIN 0x200e
#define SOUND_WIND_CUTTER 0x200f
#define SOUND_CHAOS_SPEARS 0x2010
#define SOUND_SPIN_SHOES 0x2011 /* a spin's charge with either shoes */
#define SOUND_LIGHT_DASH 0x2012 /* a light dash's, and a light attack's */
#define SOUND_TIME_STOP_SONIC 0x2013
/* the cries of a Sonic look and of a Shadow look, which Amy and Metal Sonic
 * don't cry. no cry plays on STAGE_GREEN_HILL */
#define CRY_JUMP_SONIC 0x3000 /* a held task's jump, and the combo's start */
#define CRY_PICK_UP_SONIC 0x3004
#define CRY_THROW_SONIC 0x3005
#define CRY_PUSH_START_SONIC 0x3006
#define CRY_CHARGED_SONIC 0x3007 /* a spin's charge of the light attack */
#define CRY_LIGHT_ATTACK_SONIC 0x3008
#define CRY_HOMING_SONIC 0x3009
#define CRY_COMBO_LAST_SONIC 0x300a
#define CRY_HURT_SONIC 0x300b
#define CRY_CRASH_SONIC 0x300c
#define CRY_JUMP_SHADOW 0x3012
#define CRY_PICK_UP_SHADOW 0x3016
#define CRY_THROW_SHADOW 0x3017
#define CRY_PUSH_START_SHADOW 0x3018
#define CRY_CHARGED_SHADOW 0x3019
#define CRY_LIGHT_ATTACK_SHADOW 0x301a
#define CRY_HOMING_SHADOW 0x301b
#define CRY_COMBO_LAST_SHADOW 0x301c
#define CRY_HURT_SHADOW 0x301d
#define CRY_CRASH_SHADOW 0x301e
/* the voices of a Sonic look, of a Shadow look and of Amy. Metal Sonic has
 * none. a trick off a soap switch voices its grade, and one off a ramp its
 * speed. Amy voices VOICE_TRICK_AMY for every trick */
#define VOICE_SPEED_UP_SHADOW 0x5f0
#define VOICE_SPEED_UP_SONIC 0x5f1
#define VOICE_WIND_CUTTER_SHADOW 0x5f2
#define VOICE_WIND_CUTTER_SONIC 0x5f3
#define VOICE_TIME_STOP_SHADOW 0x5f4
#define VOICE_TIME_STOP_SONIC 0x5f5
#define VOICE_TRICK_C_SHADOW 0x604
#define VOICE_TRICK_C_SONIC 0x605
#define VOICE_TRICK_B_SHADOW 0x606
#define VOICE_TRICK_B_SONIC 0x607
#define VOICE_TRICK_A_SHADOW 0x608
#define VOICE_TRICK_A_SONIC 0x609
#define VOICE_CAPTURE_SHADOW 0x618
#define VOICE_UNK_23_SHADOW 0x61a
#define VOICE_CAPTURE_SONIC 0x61b
#define VOICE_MISS_SHADOW 0x61c
#define VOICE_UNK_23_SONIC 0x61d
#define VOICE_MISS_SONIC 0x61f
#define VOICE_WIND_CUTTER_AMY_JAPANESE 0xa68
#define VOICE_CAPTURE_AMY 0xa6a /* PLAYER_REQUEST_UNK_23's too */
#define VOICE_MISS_AMY 0xa6b
#define VOICE_TIME_STOP_AMY 0xa70
#define VOICE_TRICK_AMY 0xa72 /* a held task's jump too */
#define VOICE_SPEED_UP_AMY 0xa73
#define VOICE_WIND_CUTTER_AMY 0xa75
/* the sounds of the stages' own banks, by the stage each plays on. _CITY is
 * STAGE_CITY_ESCAPE's, _HIGHWAY STAGE_RADICAL_HIGHWAY's and _HARBOR
 * STAGE_METAL_HARBOR's. SOUND_TRICK_ plays through a trick in the air.
 * SOUND_BOARD_CITY plays through the board's lean and its brake. SOUND_RIDE
 * plays through the ride beneath a rail, on STAGE_CRAZY_GADGET and
 * STAGE_RELAY_SONIC */
#define SOUND_TRICK_CITY 14
#define SOUND_TRICK_HIGHWAY 5
#define SOUND_BOARD_RUN_CITY 8
#define SOUND_BOARD_RUN_HARBOR 6
#define SOUND_BOARD_CITY 2
#define SOUND_BOARD_LAND_CITY 0
#define SOUND_RIDE 2
/* the rail's sounds: _FOREST on STAGE_GREEN_FOREST, _JUNGLE on
 * STAGE_WHITE_JUNGLE, and the others on any other stage. those two stages play
 * their SOUND_RAIL_ in SOUND_RAIL_HIGH's place. each call plays
 * SOUND_RAIL_HIGH for SOUND_RAIL_HIGH_STEPS */
#define SOUND_RAIL_LAND_FOREST 6
#define SOUND_RAIL_LAND_JUNGLE 2
#define SOUND_RAIL_LAND 0x1007
#define SOUND_RAIL_FOREST 7
#define SOUND_RAIL_JUNGLE 3
#define SOUND_RAIL 0x101b
#define SOUND_RAIL_HIGH 0x101c
#define SOUND_RAIL_HIGH_STEPS 10
/* the priority of each sound the task plays for a time, by sound_play_for or
 * sound_play_at_for. a sound that each step's call renews lasts
 * SOUND_DURATION */
#define SOUND_PRIORITY 1
#define SOUND_DURATION 2

/* a press of jump with the stick pushed anywhere but ahead leaps off a rail.
 * the leap turns toward the stick. the turn is leap_turn less
 * leap_turn_per_speed for each unit of the record's speed. the rise added to
 * spd.y is leap_rise less leap_rise_per_speed for each */
static Sint32 leap_turn = 0x3000;
static Sint32 leap_turn_per_speed = 800;
static Float leap_rise = 0.45F;
static Float leap_rise_per_speed = 0.026F;
Float light_dash_exit_speed = 2.0F;

/* the slots of model_table that the task names, Sonic's and then Shadow's,
 * which their model files fill. Amy's models take Sonic's slots, and Metal
 * Sonic's take Shadow's. _BODY is the near body, and _BODY_MIDDLE and
 * _BODY_FAR the bodies sonic_draw shows farther off. the slots after each body
 * are its nodes and the models its draw adds. a _SHADOW slot is a modifier
 * volume */
#define SONIC_BODY 0
/* the draw keeps its place in unk_230, and its x axis in unk_2cc */
#define SONIC_AT_230 1
#define SONIC_LEAN 2
/* the head, and the two eyes, which the look task turns */
#define SONIC_HEAD 3
#define SONIC_EYE_0 4
#define SONIC_EYE_1 5
/* the ball, whose child the draw keeps in unk_230 and unk_2cc too */
#define SONIC_BALL 6
#define SONIC_HEAD_SHADOW 7
#define SONIC_MOUTH 8
#define SONIC_FOOT_0 9
#define SONIC_FOOT_1 10
/* the shoes of UPGRADE_SONIC_SHOES, each drawn at its node */
#define SONIC_SHOE_NODE_0 11
#define SONIC_SHOE_NODE_1 12
#define SONIC_SHOE_0 13
#define SONIC_SHOE_1 14
#define SONIC_HAND_0 15
#define SONIC_HAND_1 16
#define SONIC_ARM_0 17
#define SONIC_ARM_1 18
/* the jiggle copies this slot's child. the draw swaps the child's model for
 * the copy's */
#define SONIC_JIGGLE 21
/* the nodes the upgrades draw at, then the upgrades. the flame ring is
 * UPGRADE_SONIC_FLAMERING's, the rubber unit UPGRADE_SONIC_RUBBERUNIT's and the
 * magic wrist UPGRADE_SONIC_MAGICWRIST's */
#define SONIC_WRIST 23
#define SONIC_FLAME_RING_NODE 24
#define SONIC_FLAME_RING 25
#define SONIC_RUBBER_UNIT 26
#define SONIC_MAGIC_WRIST 27
#define SONIC_LIGHT_DASH 30 /* the light dash's afterimage */
#define SONIC_BODY_MIDDLE 31
#define SONIC_BODY_FAR 48
/* COSTUME_PSO's node in the near, the middle and the far body, and the model
 * the draw adds there */
#define SONIC_PSO_NODE 391
#define SONIC_PSO_NODE_MIDDLE 392
#define SONIC_PSO_NODE_FAR 393
#define SONIC_PSO 394
/* Shadow's slots, as Sonic's are */
#define SHADOW_BODY 65
#define SHADOW_AT_230 66
#define SHADOW_LEAN 67
#define SHADOW_HEAD 68
#define SHADOW_EYE_0 69
#define SHADOW_EYE_1 70
#define SHADOW_BALL 71
#define SHADOW_HEAD_SHADOW 72
#define SHADOW_MOUTH 73
#define SHADOW_FOOT_0 74
#define SHADOW_FOOT_1 75
#define SHADOW_SHOE_NODE_0 76
#define SHADOW_SHOE_NODE_1 77
#define SHADOW_SHOE_0 78
#define SHADOW_SHOE_1 79
#define SHADOW_HAND_0 80
#define SHADOW_HAND_1 81
#define SHADOW_ARM_0 82
#define SHADOW_ARM_1 83
#define SHADOW_JIGGLE 86
#define SHADOW_WRIST 88
#define SHADOW_FLAME_RING_NODE 89
/* a running Shadow draws four parts more: at the feet, and at these two
 * nodes. each part is two models, which take turns by the step */
#define SHADOW_RUN_NODE_2 90
#define SHADOW_RUN_NODE_3 91
#define SHADOW_RUN_PART_1 92
#define SHADOW_RUN_PART_3 94
#define SHADOW_RUN_PART_0 96
#define SHADOW_RUN_PART_2 98
#define SHADOW_FLAME_RING 100
#define SHADOW_BODY_MIDDLE 104
#define SHADOW_BODY_FAR 123
/* a LESSER_ slot is a node's offset from the slot of its middle or far body.
 * those four bodies, Sonic's and Shadow's, lay out their nodes alike */
#define LESSER_AT_230 1
#define LESSER_LEAN 2
#define LESSER_HEAD 3
#define LESSER_MOUTH 6
#define LESSER_FOOT_0 7
#define LESSER_FOOT_1 8
#define LESSER_SHOE_NODE_0 9
#define LESSER_SHOE_NODE_1 10
#define LESSER_HAND_0 11
#define LESSER_HAND_1 12
#define LESSER_ARM_0 13
#define LESSER_ARM_1 14
#define LESSER_SONIC_WRIST 15
#define LESSER_SONIC_FLAME_RING_NODE 16
#define LESSER_SHADOW_RUN_NODE_2 15
#define LESSER_SHADOW_RUN_NODE_3 16
#define LESSER_SHADOW_WRIST 17
#define LESSER_SHADOW_FLAME_RING_NODE 18
/* whether object is a node of whichever body the draw shows. near is the
 * node's slot in the near body, and lesser its LESSER_ slot */
#define SONIC_NODE(object, near, lesser)                                       \
    ((object) == model_table[near].object ||                                   \
     (object) == model_table[SONIC_BODY_MIDDLE + (lesser)].object ||           \
     (object) == model_table[SONIC_BODY_FAR + (lesser)].object)
#define SHADOW_NODE(object, near, lesser)                                      \
    ((object) == model_table[near].object ||                                   \
     (object) == model_table[SHADOW_BODY_MIDDLE + (lesser)].object ||          \
     (object) == model_table[SHADOW_BODY_FAR + (lesser)].object)

/* the characters' animations, each record's anim.actions: Sonic's, Shadow's and
 * Amy's. Metal Sonic plays Shadow's. Shadow's from 136 to 150 take SONIC_BODY.
 * sonic_draw takes a motion through the table twice: entry i's motion m, then
 * entry m's motion. entry m has motion m wherever an entry names m, but for
 * two pairs. ANIM_BOARD_CRASH_POS and ANIM_BOARD_CRASH_NEG each name the
 * other's index, and so does the pair 15 entries on */
#define SONIC_ANIMS 185
#define AMY_ANIMS 198
static struct player_action sonic_animations[SONIC_ANIMS] = {
#include "player/sonic/sonic_animations.inc"
};
static struct player_action shadow_animations[SONIC_ANIMS] = {
#include "player/sonic/shadow_animations.inc"
};
static struct player_action amy_animations[AMY_ANIMS] = {
#include "player/sonic/amy_animations.inc"
};

/* the shapes of a Sonic's collision record. the first is the body, whose
 * radius each maker sets to the character's. BALL_SHAPE is the ball of each
 * combo's last move, and is on only in that move */
#define BALL_SHAPE 1
#define SONIC_SHAPES 2
static struct collision_shape sonic_shapes[SONIC_SHAPES] = {
#include "player/sonic/sonic_shapes.inc"
};

/* the nodes each maker gives player_look_start: the head, then the two
 * eyes. Metal Sonic's maker puts Shadow's in sonic_look_nodes */
#define LOOK_NODES 3
static NJS_CNK_OBJECT *sonic_look_nodes[LOOK_NODES];
static NJS_CNK_OBJECT *shadow_look_nodes[LOOK_NODES];
/* the record of the player being drawn. sonic_draw sets it, and the motion
 * callbacks write the record's places through it */
static struct player *drawn_record;
static Sint32 unused_word;
/* the inverse of the view as sonic_draw begins. it takes a place in the view
 * back to the world */
static NJS_MATRIX view_to_world;
static NJS_MATRIX unused_matrix;
/* the matrices the motion callbacks keep for the models sonic_draw adds. each
 * is the matrix of the node its name gives. the run matrices are Shadow's:
 * those of his feet, then of SHADOW_RUN_NODE_2 and SHADOW_RUN_NODE_3 */
static NJS_MATRIX wrist_matrix;
static NJS_MATRIX flame_ring_matrix;
static NJS_MATRIX shoe_matrix_0;
static NJS_MATRIX shoe_matrix_1;
static NJS_MATRIX costume_matrix;
static NJS_MATRIX run_matrix_0;
static NJS_MATRIX run_matrix_1;
static NJS_MATRIX run_matrix_2;
static NJS_MATRIX run_matrix_3;

static void amy_create(int slot);
static void metal_sonic_create(int slot);
static void sonic_step(struct task *task);
static void sonic_pick_mode(struct task_work *work,
                            struct player_motion *motion, struct player *record,
                            struct player *sonic);
static void sonic_destroy(struct task *task);
static void sonic_center_shapes(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record);
static void light_dash_afterimage(struct task_work *work, struct player *record,
                                  struct player *sonic);
static void sonic_draw(struct task *task);
static void sonic_shadows(struct task_work *work, struct player *record,
                          int current);
static void shadow_shadows(struct task_work *work, struct player *record,
                           int current);
static void sonic_free_move(struct task_work *work,
                            struct player_motion *motion, struct player *record,
                            struct player *sonic);
static void homing_start(struct task_work *work, struct player_motion *motion,
                         struct player *record, struct player *sonic);
static void homing_step(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic);

/* the param and the phase_step the makers give the jiggle. Metal Sonic's
 * maker makes none */
#define JIGGLE_PARAM 0.4F
#define JIGGLE_PHASE_STEP 0x400

/* the animations the task names, beside player/player.h's. Sonic's, Shadow's
 * and Amy's tables each hold them */
#define ANIM_SPIN 11 /* PLAYER_MODE_SONIC_SPIN's, paced by anim_speed */
#define ANIM_ROLL 12 /* PLAYER_MODE_ROLL's and the light attack's */
/* the bounce's, and the ball sonic_draw shows in a step of PLAYER_WORK_BALL
 * as BALL_FLICKER says */
#define ANIM_BALL 30
#define ANIM_LIGHT_DASH 87
/* the combo's moves, its wait and its step back, and a jump out of a move.
 * the second and third moves follow by their entries' next */
#define ANIM_COMBO_FIRST 91
#define ANIM_COMBO_WAIT 94
#define ANIM_COMBO_BACK 95
#define ANIM_COMBO_RUNNING 96
#define ANIM_COMBO_RUNNING_LAST 99
/* the homing attack's coast, and the bounces off its first two hits. a third
 * hit bounces off in ANIM_COMBO_JUMP */
#define ANIM_HOMING_DASH 100
#define ANIM_HOMING_HIT_1 101
#define ANIM_HOMING_HIT_2 102
#define ANIM_COMBO_JUMP 103
/* the rail's stances. the rail's start picks stance A or B at random, and
 * the action button crouches either. a _MINUS animation leans to the
 * player's -z and a _PLUS one to its z, by its frame. a lean too far
 * totters, and a totter ends in stance A */
#define ANIM_RAIL_A_MINUS 104
#define ANIM_RAIL_A_PLUS 105
#define ANIM_RAIL_A_CROUCH_MINUS 106
#define ANIM_RAIL_A_CROUCH_PLUS 107
#define ANIM_RAIL_B_MINUS 108
#define ANIM_RAIL_B_PLUS 109
#define ANIM_RAIL_B_CROUCH_MINUS 110
#define ANIM_RAIL_B_CROUCH_PLUS 111
#define ANIM_RAIL_TOTTER_MINUS 112
#define ANIM_RAIL_TOTTER_PLUS 113
/* the tricks off a soap switch, by grade, and the jump off a ramp */
#define ANIM_SOAP_A 114
#define ANIM_SOAP_B 115
#define ANIM_SOAP_C 116
#define ANIM_RAMP_JUMP 117
/* the poses of Sonic's and of Shadow's versus actions */
#define ANIM_SONIC_POSE 118
#define ANIM_SHADOW_POSE 164
/* the board's leans: a _NEG one while drive is below 0, and a _POS one above.
 * PLAYER_MODE_SONIC_BOARD_FAST plays the _FAST pair */
#define ANIM_BOARD_LEAN_NEG 121
#define ANIM_BOARD_LEAN_POS 122
#define ANIM_BOARD_FAST_LEAN_NEG 123
#define ANIM_BOARD_FAST_LEAN_POS 124
/* PLAYER_MODE_SONIC_BOARD_BRAKE's: _POS where the stick turns the heading by
 * a positive angle_difference, and _NEG otherwise */
#define ANIM_BOARD_BRAKE_NEG 125
#define ANIM_BOARD_BRAKE_POS 126
/* the board's jump, its fall's start, and the board's fall */
#define ANIM_BOARD_JUMP 127
#define ANIM_BOARD_JUMP_FALL 128
#define ANIM_BOARD_FALL 129
/* the board's crash into a wall. it is _NEG where vector_angle, from the
 * motion's spd to wall_normal about the ball's up, is past HALF_TURN. it is
 * _POS otherwise */
#define ANIM_BOARD_CRASH_POS 131
#define ANIM_BOARD_CRASH_NEG 132
/* the board's tricks off a ramp or a soap switch, by grade */
#define ANIM_BOARD_TRICK_A 133
#define ANIM_BOARD_TRICK_B 134
#define ANIM_BOARD_TRICK_C 135
/* an animation PLAYER_MODE_RUN takes as it takes ANIM_TURN: the run adds
 * trot_speed to spd.x once either ends */
#define ANIM_UNK_157 157
#define ANIM_RAIL_BENEATH 182 /* PLAYER_MODE_SONIC_RAIL_BENEATH's */
#define ANIM_HOLD_HANDLE 183 /* PLAYER_MODE_HOLD_HANDLE's */

void sonic_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *sonic;
    struct player *record;

    if (game_state.two_players != 0 && game_state.alt_character[slot] != 0) {
        amy_create(slot);
        return;
    }
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            sonic_step, "SonicTheHedgehog")) == NULL) {
        return;
    }
    if ((sonic = syCalloc(1, SONIC_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = sonic;
    work = task->work;
    /* @hack spelling: record, a second pointer to the record taken through
     * its first field, where sonic alone would do */
    record = (struct player *)&sonic->slot;
    record->slot = slot;
    /* @hack spelling: the record's character set in one chain with the
     * work's mode and submode, where a statement of its own would do */
    work->mode = work->submode = record->character = PLAYER_SONIC;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = sonic_animations;
    record->anim.next = ANIM_STAND;
    collision_init(task, sonic_shapes, SONIC_SHAPES, COLLISION_PLAYER);
    /* @bug memory: reads through a null collision record where collision_init
     * fails, which sonic_step's test of it allows, TST R2,R2 at 0x8c065c1e.
     * this read is at 0x8c0651f8, and the other makers' at 0x8c065544,
     * 0x8c06579e and 0x8c065ae6. sonic_step reads it at 0x8c065eb0,
     * sonic_pick_mode at 0x8c06ac22, sonic_center_shapes at 0x8c06d722 and
     * sonic_draw at 0x8c06e628 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where collision_init fails, the work
     * holds no collision record, and the create empties the slot and
     * destroys the task. so no step or draw reads a null collision record.
     * collision_init sets the record only where it succeeds, so the test
     * leaves its call outside the guard. the other makers' sides do the
     * same */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    if (game_state.two_players == 0 || game_state.costume[slot] == 0) {
        sonic->by_character.sonic.texlist = texture_load_pvm_alloc("SONICTEX");
        sonic->by_character.sonic.model_file = model_file_read("SONICMDL.PRS");
    } else if (game_state.costume[slot] == COSTUME_PSO) {
        sonic->by_character.sonic.texlist =
            texture_load_pvm_alloc("PSOSONICTEX");
        sonic->by_character.sonic.model_file =
            model_file_read("PSOSONICMDL.PRS");
        record->costume = COSTUME_PSO;
    } else if (game_state.costume[slot] == COSTUME_H) {
        sonic->by_character.sonic.texlist = texture_load_pvm_alloc("HSONICTEX");
        sonic->by_character.sonic.model_file = model_file_read("HSONICMDL.PRS");
        record->costume = COSTUME_H;
    } else {
        sonic->by_character.sonic.texlist = texture_load_pvm_alloc("XSONICTEX");
        sonic->by_character.sonic.model_file = model_file_read("XSONICMDL.PRS");
        record->costume = COSTUME_X;
    }
    life_icon_load(record->character);
    sonic->by_character.sonic.motion_file = motion_file_read("SONICMTN.PRS");
    player_anim_step(&record->anim);
    sonic_look_nodes[0] = model_table[SONIC_HEAD].object;
    sonic_look_nodes[1] = model_table[SONIC_EYE_0].object;
    sonic_look_nodes[2] = model_table[SONIC_EYE_1].object;
    player_look_start(slot, sonic_look_nodes);
    sonic->by_character.sonic.jiggle =
        jiggle_create(model_table[SONIC_JIGGLE].object->child);
    sonic->by_character.sonic.jiggle->kind = JIGGLE_SWING_X;
    sonic->by_character.sonic.jiggle->param = JIGGLE_PARAM;
    sonic->by_character.sonic.jiggle->by_kind.steps = 0;
    sonic->by_character.sonic.jiggle->phase_step = JIGGLE_PHASE_STEP;
    sonic->by_character.sonic.jiggle->shape_a = NULL;
    sonic->by_character.sonic.jiggle_pos = work->pos;
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = sonic_destroy;
    task->draw = sonic_draw;
    s_efftex_load();
    /* @hack spelling: the arms that store the same, each with a test of its
     * own, where one test of them together would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_SONIC_SHOES);
    } else if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_SONIC_SHOES);
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_1 && slot == 1) {
        record->upgrades = 0;
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_2 && slot == 1) {
        record->upgrades = 0;
    }
}

static void amy_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *sonic;
    struct player *record;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            sonic_step, "SonicTheHedgehog")) == NULL) {
        return;
    }
    if ((sonic = syCalloc(1, SONIC_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = sonic;
    work = task->work;
    /* @hack spelling: record, a second pointer to the record taken through
     * its first field, where sonic alone would do */
    record = (struct player *)&sonic->slot;
    record->slot = slot;
    record->character = PLAYER_SONIC;
    work->mode = work->submode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = amy_animations;
    record->anim.next = ANIM_STAND;
    record->kind = PLAYER_KIND_AMY;
    collision_init(task, sonic_shapes, SONIC_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in sonic_create, as sonic_create's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    sonic->by_character.sonic.texlist = texture_load_pvm_alloc("AMYTEX");
    sonic->by_character.sonic.model_file = model_file_read("AMYMDL.PRS");
    life_icon_load(record->kind);
    sonic->by_character.sonic.motion_file = motion_file_read("AMYMTN.PRS");
    player_anim_step(&record->anim);
    sonic_look_nodes[0] = model_table[SONIC_HEAD].object;
    sonic_look_nodes[1] = model_table[SONIC_EYE_0].object;
    sonic_look_nodes[2] = model_table[SONIC_EYE_1].object;
    player_look_start(slot, sonic_look_nodes);
    sonic->by_character.sonic.jiggle =
        jiggle_create(model_table[SONIC_JIGGLE].object->child);
    sonic->by_character.sonic.jiggle->kind = JIGGLE_SWING_X;
    sonic->by_character.sonic.jiggle->param = JIGGLE_PARAM;
    sonic->by_character.sonic.jiggle->by_kind.steps = 0;
    sonic->by_character.sonic.jiggle->phase_step = JIGGLE_PHASE_STEP;
    sonic->by_character.sonic.jiggle->shape_a = NULL;
    sonic->by_character.sonic.jiggle_pos = work->pos;
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = sonic_destroy;
    task->draw = sonic_draw;
    amy_efftex_load();
    /* @hack spelling: the arms that store the same, each with a test of its
     * own, where one test of them together would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_SONIC_SHOES);
    } else if (game_state.two_players != 0) {
        record->upgrades = 0;
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_1 && slot == 1) {
        record->upgrades = 0;
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_2 && slot == 1) {
        record->upgrades = 0;
    }
}

void shadow_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *sonic;
    struct player *record;

    if (game_state.two_players != 0 && game_state.alt_character[slot] != 0) {
        metal_sonic_create(slot);
        return;
    }
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            sonic_step, "SonicTheHedgehog")) == NULL) {
        return;
    }
    if ((sonic = syCalloc(1, SONIC_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = sonic;
    work = task->work;
    /* @hack spelling: record, a second pointer to the record taken through
     * its first field, where sonic alone would do */
    record = (struct player *)&sonic->slot;
    record->slot = slot;
    record->character = PLAYER_SHADOW;
    work->mode = work->submode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = shadow_animations;
    record->anim.next = ANIM_STAND;
    collision_init(task, sonic_shapes, SONIC_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in sonic_create, as sonic_create's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    if (game_state.two_players == 0 || game_state.costume[slot] == 0) {
        sonic->by_character.sonic.texlist = texture_load_pvm_alloc("TERIOSTEX");
        sonic->by_character.sonic.model_file = model_file_read("TERIOSMDL.PRS");
    } else if (game_state.costume[slot] == COSTUME_PSO) {
        sonic->by_character.sonic.texlist =
            texture_load_pvm_alloc("PSOSHADOWTEX");
        sonic->by_character.sonic.model_file =
            model_file_read("PSOSHADOWMDL.PRS");
        record->costume = COSTUME_PSO;
    } else if (game_state.costume[slot] == COSTUME_H) {
        sonic->by_character.sonic.texlist =
            texture_load_pvm_alloc("HSHADOWTEX");
        sonic->by_character.sonic.model_file =
            model_file_read("HSHADOWMDL.PRS");
        record->costume = COSTUME_H;
    } else {
        sonic->by_character.sonic.texlist =
            texture_load_pvm_alloc("XSHADOWTEX");
        sonic->by_character.sonic.model_file =
            model_file_read("XSHADOWMDL.PRS");
        record->costume = COSTUME_X;
    }
    life_icon_load(record->character);
    sonic->by_character.sonic.motion_file = motion_file_read("TERIOSMTN.PRS");
    player_anim_step(&record->anim);
    shadow_look_nodes[0] = model_table[SHADOW_HEAD].object;
    shadow_look_nodes[1] = model_table[SHADOW_EYE_0].object;
    shadow_look_nodes[2] = model_table[SHADOW_EYE_1].object;
    player_look_start(slot, shadow_look_nodes);
    sonic->by_character.sonic.jiggle =
        jiggle_create(model_table[SHADOW_JIGGLE].object->child);
    sonic->by_character.sonic.jiggle->kind = JIGGLE_SWING_XZ;
    sonic->by_character.sonic.jiggle->param = JIGGLE_PARAM;
    sonic->by_character.sonic.jiggle->by_kind.steps = 0;
    sonic->by_character.sonic.jiggle->phase_step = JIGGLE_PHASE_STEP;
    sonic->by_character.sonic.jiggle->shape_a = NULL;
    sonic->by_character.sonic.jiggle_pos = work->pos;
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = sonic_destroy;
    task->draw = sonic_draw;
    sh_efftex_load();
    /* @hack spelling: the arms that store the same, each with a test of its
     * own, where one test of them together would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_SHADOW_SHOES);
    } else if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_SHADOW_SHOES);
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_1 && slot == 1) {
        record->upgrades = 0;
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_2 && slot == 1) {
        record->upgrades = 0;
    }
}

static void metal_sonic_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *sonic;
    struct player *record;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            sonic_step, "SonicTheHedgehog")) == NULL) {
        return;
    }
    if ((sonic = syCalloc(1, SONIC_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = sonic;
    work = task->work;
    /* @hack spelling: record, a second pointer to the record taken through
     * its first field, where sonic alone would do */
    record = (struct player *)&sonic->slot;
    record->slot = slot;
    record->character = PLAYER_SHADOW;
    work->mode = work->submode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = shadow_animations;
    record->anim.next = ANIM_STAND;
    record->kind = PLAYER_KIND_METALSONIC;
    collision_init(task, sonic_shapes, SONIC_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in sonic_create, as sonic_create's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    sonic->by_character.sonic.texlist = texture_load_pvm_alloc("METALSONICTEX");
    life_icon_load(record->kind);
    sonic->by_character.sonic.model_file = model_file_read("METALSONICMDL.PRS");
    sonic->by_character.sonic.motion_file =
        motion_file_read("METALSONICMTN.PRS");
    player_anim_step(&record->anim);
    sonic_look_nodes[0] = model_table[SHADOW_HEAD].object;
    sonic_look_nodes[1] = model_table[SHADOW_EYE_0].object;
    sonic_look_nodes[2] = model_table[SHADOW_EYE_1].object;
    player_look_start(slot, sonic_look_nodes);
    action_window_start(record->slot);
    task->destructor = sonic_destroy;
    task->draw = sonic_draw;
    sh_efftex_load();
    /* @hack spelling: the arms that store the same, each with a test of its
     * own, where one test of them together would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_SHADOW_SHOES);
    } else if (game_state.two_players != 0) {
        record->upgrades = 0;
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_1 && slot == 1) {
        record->upgrades = 0;
    } else if (stage_current_id() == STAGE_VERSUS_SONIC_2 && slot == 1) {
        record->upgrades = 0;
    }
}

/* the share of Sonic's height, radius, snap and center_height that a curled
 * player takes. a player curls in a combo's move, or while hold_steps is 2 or
 * more. sonic_step reads PLAYER_SONIC's row of character_constants for every
 * character. PLAYER_SHADOW's row holds the same values */
#define CURLED_SIZE 0.4F
/* each step the player's time runs, flame_ring_glow advances by
 * FLAME_RING_GLOW_STEP and bounce_glow by BOUNCE_GLOW_STEP. sonic_draw lights
 * the flame ring and the rubber unit by their cosines */
#define FLAME_RING_GLOW_STEP 0x400
#define BOUNCE_GLOW_STEP 0x2000
/* Metal Sonic's jets. each step of his run adds METAL_JETS_PER_STEP to jets. at
 * running_speed or more, with the stick past METAL_JET_STICK, it makes a jet.
 * that jet is JET_FAST past METAL_JET_FAST_STICK, and JET_SLOW otherwise. each
 * step that jets is above 0 makes a JET_SCALED jet, scaled METAL_JET_SIZE
 * for each one. it then takes one off. jets is held to METAL_JETS_MAX first */
#define METAL_JETS_PER_STEP 5
#define METAL_JET_STICK 0.1F
#define METAL_JET_FAST_STICK 0.9F
#define METAL_JET_SIZE 0.125F
#define METAL_JETS_MAX 8
/* along a rail at RAIL_POWDER_SPEED or more, each step throws powder from the
 * feet, unk_200 and unk_20c. it flies at RAIL_POWDER_SHARE of the motion's
 * spd */
#define RAIL_POWDER_SPEED 3.68F
#define RAIL_POWDER_SHARE 0.9F
/* the homing attack can meet a wall turned more than HOMING_WALL_TURN from
 * the wall of the step before. it then stops along the player's x and z. on the
 * ground it stops along y too. a wall_normal shorter than WALL_NORMAL_MIN is
 * no wall */
#define HOMING_WALL_TURN 0x3c00
#define WALL_NORMAL_MIN 0.01F
/* the top speed player_accel_board takes: the board's, and
 * PLAYER_MODE_SONIC_BOARD_FAST's */
#define BOARD_TOP_SPEED 7.82F
#define BOARD_FAST_TOP_SPEED 13.8F
/* the board's run sounds above BOARD_RUN_SOUND_SPEED along x. its volume is
 * BOARD_RUN_VOLUME_BASE plus BOARD_RUN_VOLUME_PER_SPEED for each unit of that
 * speed. the part past the base is BOARD_RUN_VOLUME_RISE_MAX at most. its
 * pitch is BOARD_RUN_PITCH_PER_SPEED for each unit, up to BOARD_RUN_PITCH_MAX,
 * less BOARD_RUN_PITCH_BASE */
#define BOARD_RUN_SOUND_SPEED 0.2F
#define BOARD_RUN_VOLUME_BASE 40
#define BOARD_RUN_VOLUME_PER_SPEED 5.0F
#define BOARD_RUN_VOLUME_RISE_MAX 80
#define BOARD_RUN_PITCH_PER_SPEED 256.0F
#define BOARD_RUN_PITCH_MAX 3152
#define BOARD_RUN_PITCH_BASE 4096
/* the board's sparks, and its brake's spray, fly from ground without a flag
 * of SPARKLESS_GROUND. each flies at SPARK_SPEED_SHARE of the motion's spd,
 * less SPARK_FALL_SHARE of world_gravity. the sparks fly while the record's
 * spd is longer than BOARD_SPARKS_SPEED */
#define SPARKLESS_GROUND (WORLD_FLAT | WORLD_DUSTY | GROUND_SLIPPERY)
#define SPARK_SPEED_SHARE 0.7F
#define SPARK_FALL_SHARE 0.4F
#define BOARD_SPARKS_SPEED 2.0F
/* PLAYER_MODE_SONIC_BOARD_BRAKE sprays each step above BOARD_SPARKS_SPEED,
 * and each fourth step above BRAKE_SPRAY_SPEED_MIN. a spray throws
 * BRAKE_SPECKS_PER_SPEED specks for each unit of the speed before the step.
 * each lies a random distance up to BRAKE_SPRAY_SPREAD along the player's z.
 * on STAGE_CITY_ESCAPE each spray plays SOUND_BOARD_CITY for
 * BRAKE_SOUND_STEPS. its volume is BRAKE_VOLUME_BASE plus
 * BRAKE_VOLUME_PER_SPEED for each unit of speed */
#define BRAKE_SPRAY_SPEED_MIN 0.2F
#define BRAKE_SPECKS_PER_SPEED 14.0F
#define BRAKE_SPRAY_SPREAD 6.0F
#define BRAKE_SOUND_STEPS 5
#define BRAKE_VOLUME_BASE 60.0F
#define BRAKE_VOLUME_PER_SPEED 4.0F
/* PLAYER_MODE_UNK_38, which PLAYER_REQUEST_SINK starts, holds spd.x within
 * SINK_SPEED_MAX either way. a jump out of it rises at SINK_JUMP_SHARE of a
 * jump's spd.y. that rise loses SINK_JUMP_FADE of itself for each step the
 * mode has run. after SINK_JUMP_STEPS it loses SINK_JUMP_LATE_SHARE */
#define SINK_SPEED_MAX 0.5F
#define SINK_JUMP_SHARE 0.8F
#define SINK_JUMP_FADE 0.005F
#define SINK_JUMP_LATE_SHARE 0.5F
#define SINK_JUMP_STEPS 100
/* the jiggle follows the head's place, unk_218. each step it takes a share of
 * how far that place moved. the share is SONIC_JIGGLE_SHARE for Sonic, and
 * SHADOW_JIGGLE_SHARE for Shadow. a share larger than the jiggle's param
 * becomes its param. a share past PLAYER_SWING_KICK sets the phase to
 * QUARTER_TURN. a share of SONIC_JIGGLE_KICK or more instead advances the phase
 * by SONIC_KICK_PHASE plus up to SONIC_KICK_SPREAD. for Shadow those are
 * SHADOW_JIGGLE_KICK, SHADOW_KICK_PHASE and SHADOW_KICK_SPREAD */
#define SONIC_JIGGLE_SHARE 0.25F
#define SHADOW_JIGGLE_SHARE 0.2F
#define SONIC_JIGGLE_KICK 0.8F
#define SONIC_KICK_PHASE 8192.0F
#define SONIC_KICK_SPREAD 8192.0F
#define SHADOW_JIGGLE_KICK 0.64F
#define SHADOW_KICK_PHASE 6144.0F
#define SHADOW_KICK_SPREAD 4096.0F

/* a jump goes on to ANIM_JUMP_FALL once spd.y is 0 or less. from
 * ANIM_JUMP_RISE it waits until JUMP_RISE_SHARE of that animation's frames
 * have played */
#define JUMP_RISE_SHARE 0.8F
/* a leap off a rail holds the stick's lock at LEAP_STICK_LOCK steps at
 * least */
#define LEAP_STICK_LOCK 12
/* a jump off the ride beneath a rail starts at this spd.y */
#define RAIL_BENEATH_JUMP_SPEED (-1.5F)
/* a magic hand stands the player after MAGIC_HAND_STEPS steps */
#define MAGIC_HAND_STEPS 20
/* the technique score a trick's landing earns, by the trick's grade */
#define TRICK_SCORE_A 1000
#define TRICK_SCORE_B 500
#define TRICK_SCORE_C 200

/* a light dash starts at LIGHT_DASH_SPEED. it heads for the nearest ring
 * within LIGHT_DASH_RANGE and within LIGHT_DASH_CONE of the player's heading.
 * the player turns toward that ring by up to LIGHT_DASH_TURN a step. a step
 * moves the ring's distance, held from LIGHT_DASH_STEP_MIN to
 * LIGHT_DASH_STEP_MAX. within LIGHT_DASH_NEAR of the ring, the step keeps the
 * way the motion's spd points */
#define LIGHT_DASH_SPEED 16.0F
#define LIGHT_DASH_RANGE 32.0F
#define LIGHT_DASH_CONE 0x3000
#define LIGHT_DASH_TURN 0x1800
#define LIGHT_DASH_STEP_MIN 2.0F
#define LIGHT_DASH_STEP_MAX 16.0F
#define LIGHT_DASH_NEAR 1.0F
/* light_dash_steps starts at LIGHT_DASH_STEPS and counts down a step. while a
 * ring is in reach it stays at LIGHT_DASH_RING_STEPS or more. the dash ends
 * once it runs out. it ends too LIGHT_DASH_TIMEOUT steps after its start, or
 * after the last ring the player's collision hit. a ring hit counts in
 * light_dash_rings where it is the first. a later one counts where it lies more
 * than LIGHT_DASH_RINGS_APART from the last one counted */
#define LIGHT_DASH_STEPS 10
#define LIGHT_DASH_RING_STEPS 5
#define LIGHT_DASH_TIMEOUT 360
#define LIGHT_DASH_RINGS_APART 0.5F
/* a short dash has counted LIGHT_DASH_SHORT_RINGS rings or fewer. with no
 * ring in reach, a short dash looks out to LIGHT_DASH_FAR_RANGE. it runs out
 * where none is there either. a longer dash that ends with the stick pushed,
 * but not back, goes on at full_speed. any other end holds the record's spd to
 * LIGHT_DASH_END_SPEED. a request ends a dash with spd.x held to it. so does a
 * step that moves the player LIGHT_DASH_STUCK or less, which brakes */
#define LIGHT_DASH_SHORT_RINGS 1
#define LIGHT_DASH_FAR_RANGE 64.0F
#define LIGHT_DASH_END_SPEED 2.0F
#define LIGHT_DASH_STUCK 1.5F

/* the square of the light attack's reach, 400, for a player without a rival.
 * with a rival, an item or an enemy is in reach only nearer than the rival */
#define LIGHT_ATTACK_RANGE 160000.0F
/* a light attack rises at LIGHT_ATTACK_RISE. it flies straight on while
 * ball_steps is below LIGHT_ATTACK_STRAIGHT_STEPS. it then flies at
 * LIGHT_ATTACK_SPEED, along a blend of the motion's way and the way to the
 * target. the blend's two weights sum to LIGHT_ATTACK_WEIGHT. the target's
 * weight is LIGHT_ATTACK_PULL plus ball_steps' low byte over 255. the stick
 * moves LIGHT_ATTACK_STICK_PULL times its strength over to the target's weight.
 * without a target the player keeps LIGHT_ATTACK_SLOW of its spd.x a step */
#define LIGHT_ATTACK_RISE 16.0F
#define LIGHT_ATTACK_STRAIGHT_STEPS 2
#define LIGHT_ATTACK_SPEED 10.0F
#define LIGHT_ATTACK_WEIGHT 10.0F
#define LIGHT_ATTACK_PULL 2.0F
#define LIGHT_ATTACK_STICK_PULL 2.0F
#define LIGHT_ATTACK_SLOW 0.9F
/* sonic_pick_mode ends a light attack after LIGHT_ATTACK_TIMEOUT steps, which
 * steps never reaches */
#define LIGHT_ATTACK_TIMEOUT 360
/* the attack and the guard light_attack_start gives the body's shape.
 * sonic_step sets both again later in the step, by the mode. in the attack
 * they are attack 2 and guard 3. under PLAYER_INVINCIBLE or with
 * UPGRADE_UNK_15 both are 3 */
#define LIGHT_ATTACK_LEVEL 3

/* a ramp throws a jump off it, or a board's jump, along the ramp's own angles.
 * the throw is RAMP_THROW_UP up, and RAMP_THROW_AHEAD ahead below full_speed.
 * up to RAMP_THROW_AHEAD past full_speed, the throw ahead is spd.x less
 * full_speed. past that it is 0 */
#define RAMP_THROW_UP 0.7F
#define RAMP_THROW_AHEAD 2.4F
/* a board's jump off a ramp is a trick, graded by spd.x. it is an A past
 * BOARD_TRICK_A_SPEED, a B past BOARD_TRICK_B_SPEED and a C past
 * BOARD_TRICK_C_SPEED. a slower one is a plain jump */
#define BOARD_TRICK_A_SPEED 6.854F
#define BOARD_TRICK_B_SPEED 5.06F
#define BOARD_TRICK_C_SPEED 3.22F
/* a board's jump off a soap switch is a trick graded the same way. the switch
 * throws it SOAP_THROW_UP up and SOAP_THROW_AHEAD along the switch's -z */
#define SOAP_THROW_UP 1.7F
#define SOAP_THROW_AHEAD 3.0F
/* a board that lands at BOARD_LAND_SLOW along x or slower goes on at
 * BOARD_LAND_SPEED. on STAGE_CITY_ESCAPE a landing plays SOUND_BOARD_LAND_CITY
 * at volume 0, or after a plain jump at BOARD_JUMP_LAND_VOLUME */
#define BOARD_LAND_SLOW 0.3F
#define BOARD_LAND_SPEED 1.0F
#define BOARD_JUMP_LAND_VOLUME (-10)

/* a spin with either sparkle charges the light attack after
 * SPIN_CHARGE_STEPS, and any spin ends after SPIN_STEPS. its charge, held in
 * anim_speed, grows by SPIN_CHARGE_GAIN a step up to SPIN_CHARGE_MAX */
#define SPIN_CHARGE_STEPS 120
#define SPIN_STEPS 300
#define SPIN_CHARGE_GAIN 0.4F
#define SPIN_CHARGE_MAX 10.0F
/* ANIM_SPIN's anim_speed outside PLAYER_MODE_SONIC_SPIN.
 * PLAYER_REQUEST_SPIN's spin takes it. so does a spin, a roll or a path roll
 * that falls or slows */
#define SPIN_PACE 5.0F
/* the record's top_speed while the light attack is charged */
#define CHARGED_TOP_SPEED 0.5F

/* the combo. in its wait, or outside the combo, a hold of the action button
 * for more than COMBO_HOLD_STEPS steps spins. a hold let go after more than
 * COMBO_HOLD_MIN steps starts the combo. so does a ceiling other than water,
 * no more than COMBO_ROOM above a player on the ground. that player's up must
 * have a y above COMBO_LEVEL. under such a ceiling a move starts over,
 * which keeps the player curled. the first move's spd.x is COMBO_SPEED, and
 * each later move has its own */
#define COMBO_HOLD_STEPS 24
#define COMBO_HOLD_MIN 2
#define COMBO_ROOM 5.0F
#define COMBO_LEVEL 0.99F
#define COMBO_SPEED 1.875F
#define COMBO_2_SPEED 2.5F
#define COMBO_3_SPEED 2.5F
#define COMBO_RUNNING_2_SPEED 3.125F
#define COMBO_RUNNING_3_SPEED 3.0F
/* a running first move that ends within COMBO_RUNNING_REPEAT_STEPS starts
 * over, with its steps from COMBO_RUNNING_REPEAT_FROM */
#define COMBO_RUNNING_REPEAT_STEPS 41
#define COMBO_RUNNING_REPEAT_FROM 40
/* the spd.x of a step back out of a move */
#define COMBO_BACK_SPEED (-4.0F)
/* the vibration_start power and freq of the combo's start and its last
 * move */
#define COMBO_VIBRATION_POWER 6
#define COMBO_VIBRATION_FREQ 15

/* the square of a magic hand's reach, 400 */
#define MAGIC_HAND_RANGE 160000.0F

/* the spd.y of a bounce, and of a bounce out of a bounce. the first bounce up
 * off the ground leaves at BOUNCE_UP_SPEED, while steps is 0. each later one
 * leaves at BOUNCE_UP_AGAIN_SPEED */
#define BOUNCE_SPEED (-5.0F)
#define BOUNCE_AGAIN_SPEED (-7.0F)
#define BOUNCE_UP_SPEED 2.9F
#define BOUNCE_UP_AGAIN_SPEED 3.6F

/* a reach takes hold, or gives up, from this frame on */
#define PICK_UP_FRAME 2.0F
/* the chance that the first step of each loop of ANIM_PULL lifts the task
 * without the stick */
#define PULL_LIFT_CHANCE 0.1F

/* a throw at a target gives the thrown task THROW_SPEED. a throw ahead gives it
 * THROW_AHEAD_X and THROW_AHEAD_Y, in the player's own axes */
#define THROW_SPEED 3.0F
#define THROW_AHEAD_X 2.0F
#define THROW_AHEAD_Y 0.8F
/* the frames a throw lets go from: on the ground, in the air and at a touched
 * shape. a set down lets go up to SET_DOWN_FRAME */
#define THROW_FRAME 3.0F
#define AIR_THROW_FRAME 2.0F
#define THROW_AT_TOUCH_FRAME 7.0F
#define SET_DOWN_FRAME 2.0F
/* a held task lies between the hands, unk_1e8 and unk_1f4, HOLD_OUT along their
 * -y axes. through a throw it lies HOLD_REACH along unk_1e8's z axis, unk_26c,
 * and HOLD_OUT along its -y, unk_284 */
#define HOLD_REACH 2.5F
#define HOLD_OUT 1.2F

/* the task_create levels of the versus actions' tasks, and of the task that
 * holds the light attack's charge */
#define SPEED_UP_LEVEL 2
#define WIND_CUTTER_LEVEL 1
#define TIME_STOP_LEVEL 2
#define LIGHT_CHARGE_LEVEL 0

/* the versus speed-up lasts SPEED_UP_STEPS. it multiplies ground_accel by
 * SPEED_UP_ACCEL, and top_speed and still_grip by SPEED_UP_FACTOR. a running
 * player's spd.x takes SPEED_UP_RUN times itself at once */
#define SPEED_UP_STEPS 600
#define SPEED_UP_ACCEL 4.0F
#define SPEED_UP_FACTOR 2.5F
#define SPEED_UP_RUN 2.0F
/* how long the speed-up's ectoplasm plays, in steps */
#define SPEED_UP_EFFECT_STEPS 60

/* the wind cutter's task sends its blades at step WIND_CUTTER_SEND, and
 * hurts the other player at WIND_CUTTER_HURT. it ends after
 * WIND_CUTTER_STEPS, which its sound lasts. Sonic sends WIND_CUTTER_BLADES
 * blades, and Shadow CHAOS_SPEARS spears */
#define WIND_CUTTER_SEND 20
#define WIND_CUTTER_HURT 40
#define WIND_CUTTER_STEPS 60
#define WIND_CUTTER_BLADES 6
#define CHAOS_SPEARS 12

/* a time stop lasts TIME_STOP_STEPS. each step the stopped player presses
 * one of PLAYER_STRUGGLE_BUTTONS counts TIME_STOP_PRESS more */
#define TIME_STOP_STEPS 600
#define TIME_STOP_PRESS 5
/* the time stop's draw counts the seconds left down from TIME_STOP_SECONDS,
 * which is TIME_STOP_STEPS at STEPS_PER_SECOND. their number lies at
 * STOP_SECONDS_X_0 on player 0's half of the screen, and at STOP_SECONDS_X_1
 * on player 1's */
#define TIME_STOP_SECONDS 10.0F
#define STOP_SECONDS_X_0 128.0F
#define STOP_SECONDS_X_1 448.0F

/* the rail's pull along world_gravity, in units of the constants' gravity:
 * downhill or uphill, crouched or upright. its drag, resist.x times spd.x,
 * acts past RAIL_CROUCH_RESIST_SPEED in a crouch, and past RAIL_RESIST_SPEED
 * upright */
#define RAIL_CROUCH_GRAVITY_DOWNHILL 1.4F
#define RAIL_CROUCH_GRAVITY_UPHILL 0.8F
#define RAIL_CROUCH_RESIST_SPEED 15.64F
#define RAIL_GRAVITY_DOWNHILL 0.6F
#define RAIL_GRAVITY_UPHILL 1.0F
#define RAIL_RESIST_SPEED 6.44F
/* the rail sounds above RAIL_SOUND_SPEED along x. its volume is
 * RAIL_VOLUME_BASE plus RAIL_VOLUME_PER_SPEED for each unit of that speed. in
 * a crouch it is RAIL_CROUCH_VOLUME_BASE plus RAIL_CROUCH_VOLUME_PER_SPEED. the
 * part past the base is RAIL_VOLUME_RISE_MAX at most. its pitch is
 * RAIL_PITCH_PER_SPEED for each unit, up to RAIL_PITCH_MAX. past
 * RAIL_PITCH_HIGH, SOUND_RAIL_HIGH plays too, as loud as that part, or
 * RAIL_CROUCH_HIGH_VOLUME louder in a crouch */
#define RAIL_SOUND_SPEED 0.2F
#define RAIL_VOLUME_BASE 70
#define RAIL_VOLUME_PER_SPEED 3.75F
#define RAIL_CROUCH_VOLUME_BASE 80
#define RAIL_CROUCH_VOLUME_PER_SPEED 5.0F
#define RAIL_VOLUME_RISE_MAX 40
#define RAIL_PITCH_PER_SPEED 512.0F
#define RAIL_PITCH_MAX 0x800
#define RAIL_PITCH_HIGH 0x400
#define RAIL_CROUCH_HIGH_VOLUME 10
/* after RAIL_CURVE_STEPS on a rail, its curve pushes the player across it. it
 * does where the segment behind is shorter than RAIL_CURVE_REACH */
#define RAIL_CURVE_STEPS 2
#define RAIL_CURVE_REACH 100.0F
/* a crouched lean totters once its frame passes RAIL_CROUCH_TOTTER of its
 * stance's frames. an upright lean below RAIL_TOTTER_SPEED totters past
 * RAIL_TOTTER of them. a totter ends once the push across the rail lies within
 * RAIL_STEADY of 0. its stance then starts at RAIL_TOTTER_MIDDLE of its
 * frames. a lean
 * that totters for more than RAIL_TOTTER_STEPS steps falls off the rail */
#define RAIL_CROUCH_TOTTER 0.8F
#define RAIL_TOTTER 0.9F
#define RAIL_TOTTER_SPEED 1.0F
#define RAIL_TOTTER_MIDDLE 0.5F
#define RAIL_STEADY 0.6F
#define RAIL_TOTTER_STEPS 120
/* a lean's frame steps toward its aim by RAIL_LEAN_SHARE of the way there,
 * RAIL_LEAN_STEP_MIN at least */
#define RAIL_LEAN_SHARE 0.2F
#define RAIL_LEAN_STEP_MIN 0.2F
/* the rail's count of steps stays at SONIC_STEPS_MAX once a Sint16 runs out */
#define SONIC_STEPS_MAX 0x7fff

/* a trick off a soap switch takes its grade by the player's speed along x,
 * either way. C takes SOAP_C_SPEED, B SOAP_B_SPEED and A SOAP_A_SPEED. below
 * SOAP_C_SPEED it takes none. a grade then sets spd.x and spd.y to its own, the
 * same way along x. C leaves spd.x as it was */
#define SOAP_A_SPEED 9.2F
#define SOAP_A_SPEED_X 11.96F
#define SOAP_A_SPEED_Y 5.0F
#define SOAP_B_SPEED 6.44F
#define SOAP_B_SPEED_X 9.2F
#define SOAP_B_SPEED_Y 4.0F
#define SOAP_C_SPEED 3.68F
#define SOAP_C_SPEED_Y 3.0F
/* the speed of a trick's camera motion */
#define SOAP_CAMERA_SPEED 0.2F
/* a grade A trick reads a percent from the fraction of the switch's scl.z. a
 * grade B trick reads it from that of its scl.y. the percent is the fraction
 * times SOAP_PERCENT, rounded down. a percent of SOAP_BOOST_LOW or more counts
 * as itself less SOAP_WHOLE, and one below SOAP_BOOST_MIN as 0. spd.x then
 * gains SOAP_BOOST_FACTOR times that count, which runs from -49 to 50 */
#define SOAP_PERCENT 100.0F
#define SOAP_WHOLE 100.0F
#define SOAP_BOOST_LOW 51.0F
#define SOAP_BOOST_MIN 1.0F
#define SOAP_BOOST_FACTOR 0.2F

/* the homing attack takes a target within HOMING_RANGE, a squared distance, and
 * HOMING_CONE of the player's heading. UPGRADE_UNK_15 and STAGE_BOSS_GOLEM take
 * a near target at any heading */
#define HOMING_RANGE 10000.0F
#define HOMING_CONE 0x5000
/* the attack's speed, and its speed with UPGRADE_UNK_15, the one upgrade
 * that boss_last2 gives Super Sonic and Super Shadow. past
 * HOMING_TIRE_STEPS, each step takes a random share of it: HOMING_TIRED, plus
 * up to HOMING_TIRED_SPREAD */
#define HOMING_SPEED 5.0F
#define HOMING_SPEED_SUPER 8.0F
#define HOMING_TIRE_STEPS 180
#define HOMING_TIRED 0.7F
#define HOMING_TIRED_SPREAD 0.1F
/* the player turns toward its target by HOMING_TURN a step, or by
 * HOMING_TURN_SUPER with that upgrade. past HOMING_LOCK_STEPS, or on
 * STAGE_BOSS_GOLEM, it faces the target at once */
#define HOMING_TURN 0x800
#define HOMING_TURN_SUPER 0x1000
#define HOMING_LOCK_STEPS 120
/* an attack that has stopped steering keeps HOMING_COAST of its spd.x each
 * step */
#define HOMING_COAST 0.98F
/* a target above the player, against world_gravity, stops the steering on
 * the attack's first step. on STAGE_BOSS_GOLEM only one HOMING_GOLEM_RISE or
 * more above does */
#define HOMING_GOLEM_RISE 20.0F
/* a press of jump in the air starts the attack more than HOMING_HEIGHT from
 * the ground below. it starts it at any height where gravity lies off y, and
 * in a jump on STAGE_BOSS_GOLEM */
#define HOMING_HEIGHT 8.0F
/* an attack that steers but moves HOMING_STUCK or less in a step coasts. a
 * coast ends once homing reaches HOMING_COAST_STEPS, and any attack after
 * HOMING_TIMEOUT steps */
#define HOMING_STUCK 0.5F
#define HOMING_COAST_STEPS 15
#define HOMING_TIMEOUT 360
/* the attack's hits bounce the player off in ANIM_HOMING_HIT_1, then
 * ANIM_HOMING_HIT_2, then ANIM_COMBO_JUMP. homing_hits counts them, and starts
 * over after HOMING_HITS. a hit on a player turns the attacker to face its
 * way across the ground. its speed becomes HOMING_REBOUND_BACK times that way,
 * and HOMING_REBOUND_UP up */
#define HOMING_HITS 3
#define HOMING_REBOUND_BACK (-4.0F)
#define HOMING_REBOUND_UP 2.5F

/* NOLINTNEXTLINE(readability-function-size) */
static void sonic_step(struct task *task)
{
    struct player *sonic;
    struct task_work *work;
    struct player_motion *motion;
    struct player *record;

    sonic = task->extra.p;
    work = task->work;
    motion = task->motion.p;
    record = sonic;
    player_ground_step(task, work, motion, record);
    /* @hack redundant: an arm that never runs */
    if (0) {
        work->mode = PLAYER_MODE_STAND;
    } else if (!(game_state.time_stopped & TIME_STOPPED_BIT(record->slot))) {
        sonic_pick_mode(work, motion, record, sonic);
        sonic->by_character.sonic.flame_ring_glow += FLAME_RING_GLOW_STEP;
        sonic->by_character.sonic.bounce_glow += BOUNCE_GLOW_STEP;
    }
    if (work->collision != NULL) {
        int attack;
        int guard;
        struct collision_shape *shape;

        shape = work->collision->shapes;
        shape->flags &= ~SHAPE_HURT_NO_PUSH;
        if (record->flags & PLAYER_INVINCIBLE) {
            attack = INVINCIBLE_LEVEL;
            guard = INVINCIBLE_LEVEL;
            if (work->mode == PLAYER_MODE_SONIC_BOUNCE) {
                shape->flags |= SHAPE_HURT_NO_PUSH;
            }
        } else if (record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15)) {
            attack = 3;
            guard = 3;
        } else if (work->mode == PLAYER_MODE_SONIC_LIGHT_ATTACK) {
            attack = 2;
            guard = 3;
        } else if (work->mode == PLAYER_MODE_SONIC_LIGHT_DASH) {
            attack = 1;
            guard = 3;
        } else if (work->mode == PLAYER_MODE_SONIC_BOUNCE) {
            attack = 2;
            guard = 1;
            shape->flags |= SHAPE_HURT_NO_PUSH;
        } else if (work->flags & PLAYER_WORK_ATTACK) {
            attack = 1;
            guard = 1;
            if (work->mode == PLAYER_MODE_SONIC_COMBO_1 ||
                work->mode == PLAYER_MODE_SONIC_COMBO_2 ||
                work->mode == PLAYER_MODE_SONIC_COMBO_3 ||
                work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_1 ||
                work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_2 ||
                work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_3 ||
                sonic->by_character.sonic.hold_steps >= 2) {
                /* where this test holds, the arm below that curls the sizes
                 * clears SHAPE_HURT_NO_PUSH before anything reads it */
                shape->flags |= SHAPE_HURT_NO_PUSH;
            }
        } else {
            attack = 0;
            guard = 0;
        }
        /* @hack type: the masks of the levels taken unsigned, where
         * ~SHAPE_ATTACK and ~SHAPE_GUARD would do */
        shape->levels =
            (shape->levels & ~(Uint32)SHAPE_ATTACK) | (attack & SHAPE_ATTACK);
        shape->levels = (shape->levels & ~(Uint32)SHAPE_GUARD) |
                        ((guard << SHAPE_GUARD_SHIFT) & SHAPE_GUARD);
        if (work->mode == PLAYER_MODE_SONIC_COMBO_3) {
            shape[BALL_SHAPE].flags &= ~SHAPE_OFF;
        } else if (work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
            shape[BALL_SHAPE].flags &= ~SHAPE_OFF;
        } else {
            shape[BALL_SHAPE].flags |= SHAPE_OFF;
        }
    }
    if (work->mode == PLAYER_MODE_SONIC_COMBO_1 ||
        work->mode == PLAYER_MODE_SONIC_COMBO_2 ||
        work->mode == PLAYER_MODE_SONIC_COMBO_3 ||
        work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_1 ||
        work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_2 ||
        work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_3 ||
        sonic->by_character.sonic.hold_steps >= 2) {
        record->consts.height =
            character_constants[PLAYER_SONIC].height * CURLED_SIZE;
        record->consts.radius =
            character_constants[PLAYER_SONIC].radius * CURLED_SIZE;
        record->consts.snap =
            character_constants[PLAYER_SONIC].snap * CURLED_SIZE;
        record->consts.center_height =
            character_constants[PLAYER_SONIC].center_height * CURLED_SIZE;
        work->collision->shapes->flags &= ~SHAPE_HURT_NO_PUSH;
    } else {
        record->consts.height = character_constants[PLAYER_SONIC].height;
        record->consts.radius = character_constants[PLAYER_SONIC].radius;
        record->consts.snap = character_constants[PLAYER_SONIC].snap;
        record->consts.center_height =
            character_constants[PLAYER_SONIC].center_height;
    }
    /* @hack spelling: three nested tests, where one test of all three would
     * do */
    if (!(game_state.time_stopped & TIME_STOPPED_BIT(record->slot))) {
        if (record->kind == PLAYER_KIND_METALSONIC) {
            if (sonic->by_character.sonic.jets > 0) {
                Float size;

                if (sonic->by_character.sonic.jets > METAL_JETS_MAX) {
                    sonic->by_character.sonic.jets = METAL_JETS_MAX;
                }
                size = sonic->by_character.sonic.jets * METAL_JET_SIZE;
                metal_jet_create(record->slot, JET_SCALED, size);
                sonic->by_character.sonic.jets--;
                if (sonic->by_character.sonic.jets <= 0) {
                    sonic->by_character.sonic.jets = 0;
                }
            }
        }
    }
    /* @hack spelling: the return and the empty else, where an else holding
     * the rest of the step would do */
    if (game_state.time_stopped & TIME_STOPPED_BIT(record->slot)) {
        sonic_center_shapes(work, motion, record);
        sonic_drop_held(work, motion, record);
        surface_probe_player(work, record);
        if (record->anim.current != ANIM_WIN &&
            record->anim.current != ANIM_LOSE) {
            collision_register(task);
            collision_set_radius(task, record->consts.height +
                                           njScalor(&record->spd));
        } else {
            work->collision->hit_shape = -1;
            work->collision->hit = NULL;
            work->collision->flags &= ~COLLISION_TOUCH_FLAGS;
        }
        player_finish_step(work, motion, record);
        return;
    } else {
    }
    /* @hack spelling: the empty arm, where a test of the bit itself would
     * do */
    if (!(work->flags & PLAYER_WORK_BALL)) {
    } else {
        sonic->by_character.sonic.ball_steps++;
    }
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_stand_anim(work, motion, record);
        break;
    case PLAYER_MODE_RUN: {
        /* @hack redundant: speed, a local the step stores and never reads */
        Float speed; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
        Float strength;
        int jet;

        speed = njScalor(&motion->spd);
        if ((record->anim.last == ANIM_TURN &&
             record->anim.current != ANIM_TURN) ||
            (record->anim.last == ANIM_UNK_157 &&
             record->anim.current != ANIM_UNK_157)) {
            record->spd.x += record->consts.trot_speed;
        }
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record) == PLAYER_CRASHED) {
            /* @hack spelling: the empty arm, where a test of the stage
             * around the tests of the kind would do */
            if (stage_current_id() == STAGE_GREEN_HILL) {
            } else if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CRASH_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CRASH_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            vibration_start(record->slot, CRASH_VIBRATION_POWER,
                            CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
            work->mode = PLAYER_MODE_CRASH;
            record->anim.next = ANIM_CRASH;
            crash_stars_burst(record->slot);
            record->spd.x = -(record->spd.x * PLAYER_CRASH_REBOUND);
            record->spd.z = -(record->spd.z * PLAYER_CRASH_REBOUND);
        } else {
            player_take_speed(work, motion, record);
            player_run_anim(work, motion, record);
            if (record->spd.x > record->consts.sprint_speed) {
                speed_lines_create(record->slot,
                                   (int)(rand() * (3.0F * RAND_SCALE)),
                                   &record->unk_218, &motion->spd);
            }
        }
        if (record->kind == PLAYER_KIND_METALSONIC) {
            strength = 0.0F;
            player_read_stick(work, record, NULL, &strength);
            if (njScalor(&record->spd) >= record->consts.running_speed &&
                strength > METAL_JET_STICK) {
                if (strength > METAL_JET_FAST_STICK) {
                    jet = JET_FAST;
                } else {
                    jet = JET_SLOW;
                }
                metal_jet_create(record->slot, jet, 1.0F);
            }
            sonic->by_character.sonic.jets += METAL_JETS_PER_STEP;
        }
        break;
    }
    case PLAYER_MODE_PATH_RUN:
        player_accel_on_path(work, motion, record);
        player_apply_path_speed(work, motion, record);
        player_move_on_path_tilted(work, motion, record);
        player_run_anim(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_COMBO_1:
    case PLAYER_MODE_SONIC_COMBO_2:
    case PLAYER_MODE_SONIC_COMBO_3:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_1:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_2:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_3:
        player_turn_to_landed_target(work, motion, record);
        player_accel_gravity(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->upgrades & (UPGRADE_BIT(UPGRADE_SONIC_FLAMERING) |
                                UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING))) {
            flame_spark_create(record->slot);
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_WAIT:
    case PLAYER_MODE_SONIC_COMBO_BACK:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_SPIN:
        player_turn_to_stick(work, motion, record);
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        ball_jitter_create(record->slot);
        break;
    case PLAYER_MODE_ROLL:
        player_turn_to_stick(work, motion, record);
        player_accel_roll(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->spd.x >= record->consts.running_speed) {
            dash_create(record->slot);
            sonic->by_character.sonic.dash_steps |= SONIC_DASH_STEP(0);
            ball_follow_create(record->slot);
        }
        break;
    case PLAYER_MODE_SONIC_LIGHT_DASH: {
        int result;

        sonic_light_dash(work, motion, record, sonic);
        result = player_collide(work, motion, record);
        if (result == PLAYER_CRASHED) {
            /* @hack spelling: the empty arm, where a test of the stage
             * around the tests of the kind would do */
            if (stage_current_id() == STAGE_GREEN_HILL) {
            } else if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CRASH_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CRASH_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            vibration_start(record->slot, CRASH_VIBRATION_POWER,
                            CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
            work->mode = PLAYER_MODE_CRASH;
            record->anim.next = ANIM_CRASH;
            work->flags &= ~PLAYER_WORK_ATTACK;
            sonic->by_character.sonic.light_dash_steps = 0;
            sound_stop_id(SOUND_LIGHT_DASH);
            crash_stars_burst(record->slot);
        } else if (result != 0) {
            vibration_start(record->slot, 6, 15, VIBRATION_UNTIMED);
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~PLAYER_WORK_ATTACK;
            sonic->by_character.sonic.light_dash_steps = 0;
            sound_stop_id(SOUND_LIGHT_DASH);
            if (njScalor(&motion->spd) > light_dash_exit_speed) {
                njUnitVector(&motion->spd);
                motion->spd.x *= light_dash_exit_speed;
                motion->spd.y *= light_dash_exit_speed;
                motion->spd.z *= light_dash_exit_speed;
            } else {
                player_stop(motion, NULL);
            }
            player_take_speed(work, motion, record);
            if (njScalor(&record->spd) > light_dash_exit_speed) {
                njUnitVector(&record->spd);
                record->spd.x *= light_dash_exit_speed;
                record->spd.y *= light_dash_exit_speed;
                record->spd.z *= light_dash_exit_speed;
            } else {
                player_stop(NULL, record);
            }
        } else {
            player_take_speed(work, motion, record);
        }
        light_dash_afterimage(work, record, sonic);
        break;
    }
    case PLAYER_MODE_SONIC_LIGHT_ATTACK:
        player_turn_upright(work, motion, record);
        if (!sonic_light_attack(work, motion, record, sonic)) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        dash_create(record->slot);
        sonic->by_character.sonic.dash_steps |= SONIC_DASH_STEP(0);
        break;
    case PLAYER_MODE_PATH_ROLL:
        player_accel_roll(work, motion, record);
        player_apply_path_speed(work, motion, record);
        player_move_on_path(work, motion, record);
        ball_follow_create(record->slot);
        break;
    case PLAYER_MODE_SONIC_RAIL: {
        NJS_VECTOR spd;

        sonic_rail(work, motion, record, sonic);
        player_apply_path_speed(work, motion, record);
        player_move_on_path(work, motion, record);
        if (_builtin_fabsf(record->spd.x) >= RAIL_POWDER_SPEED) {
            spd.x = motion->spd.x * RAIL_POWDER_SHARE;
            spd.y = motion->spd.y * RAIL_POWDER_SHARE;
            spd.z = motion->spd.z * RAIL_POWDER_SHARE;
            /* @hack redundant: the 0 each count of specks adds its random
             * product to */
            powder_create(record->slot,
                          (int)(0.0F + (rand() * (3.0F * RAND_SCALE))),
                          &record->unk_200, &spd);
            powder_create(record->slot,
                          (int)(0.0F + (rand() * (3.0F * RAND_SCALE))),
                          &record->unk_20c, &spd);
            wave_create(record->slot);
        }
        break;
    }
    case PLAYER_MODE_SONIC_RAIL_BENEATH:
        sonic_rail_beneath(work, motion, record, sonic);
        player_apply_path_speed(work, motion, record);
        player_move_toward_path(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_SOAP_JUMP:
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_RAMP_JUMP:
        player_turn_upright(work, motion, record);
        player_accel_gravity(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (stage_current_id() == STAGE_CITY_ESCAPE) {
            sound_play_at_for(SOUND_TRICK_CITY, work, SOUND_PRIORITY,
                              SOUND_LOUDEST, SOUND_DURATION, &work->pos);
        } else if (stage_current_id() == STAGE_RADICAL_HIGHWAY) {
            sound_play_at_for(SOUND_TRICK_HIGHWAY, work, SOUND_PRIORITY,
                              SOUND_LOUDEST, SOUND_DURATION, &work->pos);
        }
        break;
    case PLAYER_MODE_JUMP:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        ball_brief_create(record->slot);
        break;
    case PLAYER_MODE_SONIC_BOUNCE:
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        ball_shrink_create(record->slot);
        break;
    case PLAYER_MODE_SONIC_BOUNCE_UP:
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_BOUNCE:
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_LAUNCH:
        if (record->spd.y >= record->consts.launch_steer_speed) {
            player_accel_launched(work, motion, record);
        } else {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FLING:
        player_accel_gravity(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FALL:
        player_turn_upright(work, motion, record);
        if (record->anim.next == ANIM_FREE_FALL) {
            player_accel_gravity(work, motion, record);
        } else if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_TEETER:
        break;
    case PLAYER_MODE_BRAKE:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_HOMING: {
        NJS_VECTOR wall;

        player_turn_upright(work, motion, record);
        homing_step(work, motion, record, sonic);
        wall = record->wall_normal;
        player_collide(work, motion, record);
        if (njScalor(&record->wall_normal) > WALL_NORMAL_MIN &&
            njScalor(&wall) > WALL_NORMAL_MIN &&
            njInnerProduct(&wall, &record->wall_normal) <
                njCos(HOMING_WALL_TURN)) {
            record->spd = motion->spd;
            turn_from_world(work, &record->spd);
            record->spd.x = 0.0F;
            record->spd.z = 0.0F;
            if (work->flags & PLAYER_WORK_GROUNDED) {
                record->spd.y = 0.0F;
            }
            motion->spd = record->spd;
            turn_to_world(work, &motion->spd);
        }
        player_take_speed(work, motion, record);
        dash_create(record->slot);
        sonic->by_character.sonic.dash_steps |= SONIC_DASH_STEP(0);
        ball_follow_create(record->slot);
        break;
    }
    case PLAYER_MODE_UNK_0E:
        if (record->anim.next == ANIM_PUSH_START) {
            player_accel_brake(work, motion, record);
        } else {
            player_accel_push(work, motion, record);
            if (player_action_flags(work, record) & ACTION_FLAG_400) {
                fun_8c05bce0();
            }
            /* @hack spelling: the empty arm, where a test of the animation
             * and the stage around the tests of the kind would do */
            if (record->anim.last != ANIM_PUSH_START ||
                stage_current_id() == STAGE_GREEN_HILL) {
            } else if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_PUSH_START_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_PUSH_START_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HURT:
        if (work->flags & PLAYER_WORK_GROUNDED) {
            player_turn_upright(work, motion, record);
        }
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_CRASH:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SLIDE:
        player_turn_along_slope(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HANG:
        break;
    case PLAYER_MODE_PLAY_ANIM:
        break;
    case PLAYER_MODE_SONIC_MAGIC_HAND:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PICK_UP:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PULL_HEAVY:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_14:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_STAND_HOLDING:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_hold_stand_anim(work, motion, record);
        break;
    case PLAYER_MODE_GRAB_PUSHABLE:
        player_approach_touched(work, motion, record);
        player_accel_push(work, motion, record);
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_17:
        player_approach_touched(work, motion, record);
        player_accel_push(work, motion, record);
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_THROW:
        player_turn_to_target(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_19:
        if (record->anim.last == ANIM_HOLD_TURN &&
            record->anim.current != ANIM_HOLD_TURN) {
            record->spd.x += record->consts.trot_speed;
        }
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_run_anim(work, motion, record);
        break;
    case PLAYER_MODE_UNK_1B:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FALL_HOLDING:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_JUMP_HOLDING:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_BOUNCE_HOLDING:
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_LAUNCH_HOLDING:
        if (record->spd.y >= record->consts.launch_steer_speed) {
            player_accel_launched(work, motion, record);
        } else {
            player_accel_air(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_20:
        player_accel_push(work, motion, record);
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (player_action_flags(work, record) & ACTION_FLAG_400) {
            fun_8c05bce0();
        }
        break;
    case PLAYER_MODE_AIR_THROW:
        player_turn_upright(work, motion, record);
        player_turn_to_target(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_22:
        player_shake_touched(work, motion, record);
        break;
    case PLAYER_MODE_SHAKE_HOLDING:
        TASK_MOTION_OF(record->held)->acc.x =
            TASK_MOTION_OF(record->held)->acc.y =
                TASK_MOTION_OF(record->held)->acc.z =
                    record->anim.frame * PLAYER_HELD_ACC;
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PET_CHAO:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_THROW_AT_TOUCH:
        player_turn_to_touch(work, motion, record);
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_PULL_BURIED:
        player_accel_launched(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_LEDGE_HANG:
        break;
    case PLAYER_MODE_LEDGE_CLIMB:
        break;
    case PLAYER_MODE_UNK_2A:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_ASK_HINT:
        break;
    case PLAYER_MODE_PLAY_FLUTE:
        break;
    case PLAYER_MODE_PLAY_FLUTE_AGAIN:
    case PLAYER_MODE_PLAYING_FLUTE_AGAIN:
        break;
    case PLAYER_MODE_CANT_PLAY_FLUTE:
        break;
    case PLAYER_MODE_HOLD_HANDLE:
        break;
    case PLAYER_MODE_CAUGHT:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SWIM:
        player_turn_upright(work, motion, record);
        if (record->stick_lock > 0) {
            player_accel_gravity(work, motion, record);
        } else {
            player_accel_path(work, motion, record);
        }
        player_apply_path_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_BOARD:
        player_accel_board(work, motion, record, BOARD_TOP_SPEED);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record) == PLAYER_CRASHED) {
            /* @hack spelling: the empty arm, where a test of the stage
             * around the tests of the kind would do */
            if (stage_current_id() == STAGE_GREEN_HILL) {
            } else if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CRASH_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CRASH_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            vibration_start(record->slot, CRASH_VIBRATION_POWER,
                            CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
            work->mode = PLAYER_MODE_SONIC_BOARD_CRASH;
            if (vector_angle(&motion->spd, &record->wall_normal,
                             &record->ball->up) > HALF_TURN) {
                record->anim.next = ANIM_BOARD_CRASH_NEG;
            } else {
                record->anim.next = ANIM_BOARD_CRASH_POS;
            }
            crash_stars_burst(record->slot);
        } else {
            int pitch;
            Sint8 volume;

            player_take_speed(work, motion, record);
            sonic_board_sparks_and_lean(work, motion, record);
            if (stage_current_id() == STAGE_CITY_ESCAPE) {
                if (_builtin_fabsf(record->spd.x) > BOARD_RUN_SOUND_SPEED) {
                    volume = _builtin_fabsf(record->spd.x) *
                             BOARD_RUN_VOLUME_PER_SPEED;
                    if (volume > BOARD_RUN_VOLUME_RISE_MAX) {
                        volume = BOARD_RUN_VOLUME_RISE_MAX;
                    }
                    sound_play_at_for(SOUND_BOARD_RUN_CITY, work,
                                      SOUND_PRIORITY,
                                      volume + BOARD_RUN_VOLUME_BASE,
                                      SOUND_DURATION, &work->pos);
                    /* @hack redundant: the 0 the pitch adds its product
                     * to */
                    pitch = 0.0F + (_builtin_fabsf(record->spd.x) *
                                    BOARD_RUN_PITCH_PER_SPEED);
                    if (pitch > BOARD_RUN_PITCH_MAX) {
                        pitch = BOARD_RUN_PITCH_MAX;
                    }
                    sound_set_pitch(work, SOUND_BOARD_RUN_CITY,
                                    pitch - BOARD_RUN_PITCH_BASE);
                }
            } else if (stage_current_id() == STAGE_METAL_HARBOR) {
                if (_builtin_fabsf(record->spd.x) > BOARD_RUN_SOUND_SPEED) {
                    volume = _builtin_fabsf(record->spd.x) *
                             BOARD_RUN_VOLUME_PER_SPEED;
                    if (volume > BOARD_RUN_VOLUME_RISE_MAX) {
                        volume = BOARD_RUN_VOLUME_RISE_MAX;
                    }
                    sound_play_at_for(SOUND_BOARD_RUN_HARBOR, work,
                                      SOUND_PRIORITY,
                                      volume + BOARD_RUN_VOLUME_BASE,
                                      SOUND_DURATION, &work->pos);
                    /* @hack redundant: the 0 the pitch adds its product
                     * to */
                    pitch = 0.0F + (_builtin_fabsf(record->spd.x) *
                                    BOARD_RUN_PITCH_PER_SPEED);
                    if (pitch > BOARD_RUN_PITCH_MAX) {
                        pitch = BOARD_RUN_PITCH_MAX;
                    }
                    sound_set_pitch(work, SOUND_BOARD_RUN_HARBOR,
                                    pitch - BOARD_RUN_PITCH_BASE);
                }
            }
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_FAST:
        player_accel_board(work, motion, record, BOARD_FAST_TOP_SPEED);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record) == PLAYER_CRASHED) {
            /* @hack spelling: the empty arm, where a test of the stage
             * around the tests of the kind would do */
            if (stage_current_id() == STAGE_GREEN_HILL) {
            } else if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CRASH_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CRASH_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            vibration_start(record->slot, CRASH_VIBRATION_POWER,
                            CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
            work->mode = PLAYER_MODE_SONIC_BOARD_CRASH;
            if (vector_angle(&motion->spd, &record->wall_normal,
                             &record->ball->up) > HALF_TURN) {
                record->anim.next = ANIM_BOARD_CRASH_NEG;
            } else {
                record->anim.next = ANIM_BOARD_CRASH_POS;
            }
            crash_stars_burst(record->slot);
        } else {
            player_take_speed(work, motion, record);
            sonic_board_sparks_and_lean(work, motion, record);
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_BRAKE: {
        Float speed;
        NJS_VECTOR spd;
        NJS_POINT3 pos;
        NJS_VECTOR offset;
        int i;

        speed = njScalor(&record->spd);
        player_accel_board_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (!(record->ground_flags & SPARKLESS_GROUND)) {
            if (njScalor(&record->spd) > BOARD_SPARKS_SPEED ||
                (njScalor(&record->spd) > BRAKE_SPRAY_SPEED_MIN &&
                 !(game_state.steps & 3))) {
                spd.x = (motion->spd.x * SPARK_SPEED_SHARE) -
                        (world_gravity.x * SPARK_FALL_SHARE);
                spd.y = (motion->spd.y * SPARK_SPEED_SHARE) -
                        (world_gravity.y * SPARK_FALL_SHARE);
                spd.z = (motion->spd.z * SPARK_SPEED_SHARE) -
                        (world_gravity.z * SPARK_FALL_SHARE);
                matrix_push_unit();
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, -work->ang.y);
                for (i = (int)(speed * BRAKE_SPECKS_PER_SPEED); i > 0; i--) {
                    offset.x = 0.0F;
                    offset.y = 0.0F;
                    offset.z = rand() * (BRAKE_SPRAY_SPREAD * RAND_SCALE);
                    njCalcVector(NULL, &offset, &pos);
                    njAddVector(&pos, &work->pos);
                    powder_create(record->slot, 1, &pos, &spd);
                }
                njPopMatrixEx();
                if (stage_current_id() == STAGE_CITY_ESCAPE) {
                    sound_play_at_for(
                        SOUND_BOARD_CITY, work, SOUND_PRIORITY,
                        (int)(BRAKE_VOLUME_BASE + (njScalor(&record->spd) *
                                                   BRAKE_VOLUME_PER_SPEED)),
                        BRAKE_SOUND_STEPS, &work->pos);
                }
            }
        }
        break;
    }
    case PLAYER_MODE_SONIC_BOARD_FALL:
        player_turn_upright(work, motion, record);
        player_accel_board_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_BOARD_JUMP:
        player_turn_upright(work, motion, record);
        player_accel_board_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_SONIC_BOARD_TRICK:
        player_turn_upright(work, motion, record);
        player_accel_board_trick(work, motion, record);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record)) {
            record->anim.next = ANIM_BOARD_FALL;
        }
        player_take_speed(work, motion, record);
        if (stage_current_id() == STAGE_CITY_ESCAPE) {
            sound_play_at_for(SOUND_TRICK_CITY, work, SOUND_PRIORITY,
                              SOUND_LOUDEST, SOUND_DURATION, &work->pos);
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_CRASH:
        player_accel_board_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_35:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_EVENT:
        break;
    case PLAYER_MODE_DROWN:
        if ((record->anim.flags & PLAYER_ANIM_DONE) &&
            record->ground.above_y > work->collision->shapes->center.y &&
            (record->ground.above_flags & WORLD_FLAT_THIN)) {
            record->spd.y =
                record->ground.above_y - work->collision->shapes->center.y;
            if (record->spd.y > PLAYER_RISE) {
                record->spd.y = PLAYER_RISE;
            }
        } else {
            record->spd.y = 0.0F;
        }
        player_turn_upright(work, motion, record);
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_38:
        player_turn_upright(work, motion, record);
        if (record->anim.current == ANIM_SINK) {
            player_turn_to_stick_at_speed(work, motion, record);
        } else {
            player_accel_air_weightless(work, motion, record);
        }
        if (record->spd.x > SINK_SPEED_MAX) {
            record->spd.x = SINK_SPEED_MAX;
        } else if (record->spd.x < -SINK_SPEED_MAX) {
            record->spd.x = -SINK_SPEED_MAX;
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        record->anim_speed = record->spd.x;
        break;
    case PLAYER_MODE_MISS:
        /* @hack spelling: the break that ends each arm, where one break
         * after the test would do */
        if (record->anim.current == ANIM_IN_CURRENT) {
            player_accel_path(work, motion, record);
            player_apply_path_accel(work, motion, record);
            player_collide(work, motion, record);
            player_take_speed(work, motion, record);
            break;
        } else {
            if (work->flags & PLAYER_WORK_GROUNDED) {
                record->anim.next = ANIM_MISS;
            }
            player_turn_upright(work, motion, record);
            player_accel_brake(work, motion, record);
            player_apply_accel(work, motion, record);
            player_collide(work, motion, record);
            player_take_speed(work, motion, record);
            break;
        }
    case PLAYER_MODE_FREE_MOVE:
        sonic_free_move(work, motion, record, sonic);
        break;
    }
    player_anim_step(&record->anim);
    sonic_center_shapes(work, motion, record);
    if (sonic->by_character.sonic.jiggle != NULL) {
        Float random;

        random = rand() * RAND_SCALE;
        if (record->kind == PLAYER_SONIC) {
            Float moved;
            NJS_VECTOR way;

            way = record->unk_218;
            njSubVector(&way, &sonic->by_character.sonic.jiggle_pos);
            moved = njScalor(&way) * SONIC_JIGGLE_SHARE;
            sonic->by_character.sonic.jiggle_pos = record->unk_218;
            if (_builtin_fabsf(moved) >
                _builtin_fabsf(sonic->by_character.sonic.jiggle->param)) {
                sonic->by_character.sonic.jiggle->param = moved;
            }
            if (_builtin_fabsf(moved) >= SONIC_JIGGLE_KICK) {
                sonic->by_character.sonic.jiggle->phase +=
                    (random * SONIC_KICK_SPREAD) + SONIC_KICK_PHASE;
            } else if (_builtin_fabsf(moved) > PLAYER_SWING_KICK) {
                sonic->by_character.sonic.jiggle->phase = QUARTER_TURN;
            }
            jiggle_step(sonic->by_character.sonic.jiggle);
        }
        if (record->kind == PLAYER_SHADOW) {
            Float moved;
            NJS_VECTOR way;

            way = record->unk_218;
            njSubVector(&way, &sonic->by_character.sonic.jiggle_pos);
            moved = njScalor(&way) * SHADOW_JIGGLE_SHARE;
            sonic->by_character.sonic.jiggle_pos = record->unk_218;
            if (_builtin_fabsf(moved) >
                _builtin_fabsf(sonic->by_character.sonic.jiggle->param)) {
                sonic->by_character.sonic.jiggle->param = moved;
            }
            if (_builtin_fabsf(moved) >= SHADOW_JIGGLE_KICK) {
                sonic->by_character.sonic.jiggle->phase +=
                    (random * SHADOW_KICK_SPREAD) + SHADOW_KICK_PHASE;
            } else if (_builtin_fabsf(moved) > PLAYER_SWING_KICK) {
                sonic->by_character.sonic.jiggle->phase = QUARTER_TURN;
            }
            jiggle_step(sonic->by_character.sonic.jiggle);
        }
    }
    if (player_free_move_forced(record->slot)) {
        work->mode = PLAYER_MODE_FREE_MOVE;
        work->flags &= ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_REQUEST |
                         PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
    }
    sonic_drop_held(work, motion, record);
    /* @hack spelling: the shift written out, where <<= would do */
    sonic->by_character.sonic.dash_steps = sonic->by_character.sonic.dash_steps
                                           << 1;
    surface_probe_player(work, record);
    if (record->anim.current != ANIM_WIN && record->anim.current != ANIM_LOSE &&
        work->mode != PLAYER_MODE_EVENT) {
        collision_register(task);
        collision_set_radius(task,
                             record->consts.height + njScalor(&record->spd));
    } else {
        work->collision->hit_shape = -1;
        work->collision->hit = NULL;
        work->collision->flags &= ~COLLISION_TOUCH_FLAGS;
    }
    player_finish_step(work, motion, record);
}

/* NOLINTNEXTLINE(readability-function-size) */
static void sonic_pick_mode(struct task_work *work,
                            struct player_motion *motion, struct player *record,
                            struct player *sonic)
{
    /* @bug memory: reads a null record, which the test lets through, TST
     * R3,R3 at 0x8c067f66. player_take_hit reads it, and so does the test of
     * its kind where the hit counts, MOV.B @(3,R2),R0 at 0x8c067fb8 */
    if (record != NULL && (record->flags & PLAYER_INVINCIBLE)) {
        work->flags |= PLAYER_WORK_UNK_8000;
        work->timer = 0;
    } else if (work->timer == 0) {
        if (player_take_hit(work, record)) {
            /* @hack spelling: the empty arm, where a test of the stage
             * around the tests of the kind would do */
            if (stage_current_id() == STAGE_GREEN_HILL) {
            } else if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_HURT_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_HURT_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            sonic->by_character.sonic.hold_steps = 0;
            work->flags |= PLAYER_WORK_UNK_8000;
            if (work->flags & PLAYER_WORK_HOLDING) {
                player_release_held(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_CHARGED |
                             PLAYER_WORK_ATTACK | PLAYER_WORK_HOLDING |
                             PLAYER_WORK_REQUEST | PLAYER_WORK_ON_PATH);
            /* @hack spelling: the test and the empty else, where the clear
             * alone would do */
            if (record->flags & (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER)) {
                record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
            } else {
            }
        } else {
            work->flags &= ~PLAYER_WORK_UNK_8000;
        }
    } else {
        if (game_state.solo_player == 0) {
            work->timer--;
        }
        work->flags &= ~WORK_HURT;
    }
    sonic_offer_light_attack(work, record, sonic);
    sonic_offer_versus_actions(work, record);
    sonic_offer_light_dash(work, record);
    player_offer_flute(work, record);
    player_offer_hint(work, record);
    sonic_offer_hold_handle(work, record);
    sonic_offer_magic_hand(work, record);
    player_offer_target_actions(work, motion, record);
    player_offer_held_actions(work, record);
    player_offer_release(work, record);
    player_offer_call(work, record);
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        if (sonic_test_request(work, motion, record, sonic) ||
            sonic_test_action(work, motion, record, sonic) ||
            player_test_fall(work, motion, record) ||
            sonic_test_jump(work, motion, record, sonic)) {
            sonic->by_character.sonic.hold_steps = 0;
            break;
        }
        if (sonic_combo(work, motion, record, sonic)) {
            break;
        }
        /* @hack spelling: the else after the break, where an if of its own
         * would do */
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            break;
        } else if (record->ball->up.y < PLAYER_SLIDE_STEEP &&
                   njScalor(&record->spd) > record->consts.slide_speed) {
            work->mode = PLAYER_MODE_SLIDE;
            if (player_turn_along_slope(work, motion, record) < 0.0F) {
                record->anim.next = ANIM_SLIDE_BACK;
            } else {
                record->anim.next = ANIM_SLIDE;
            }
        }
        break;
    case PLAYER_MODE_RUN:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        {
            int result;

            /* @hack redundant: the 0 that result starts at, which the next
             * statement replaces before any read */
            result = 0;
            result = sonic_test_action(work, motion, record, sonic);
            if (result || player_test_fall(work, motion, record) ||
                sonic_test_jump(work, motion, record, sonic) ||
                sonic_combo(work, motion, record, sonic) ||
                player_test_stand(work, motion, record)) {
                if (result != SONIC_ACTION_VERSUS) {
                    work->flags &= ~PLAYER_WORK_ON_PATH;
                }
                if (work->mode != PLAYER_MODE_STAND) {
                    sonic->by_character.sonic.hold_steps = 0;
                }
                break;
            }
        }
        if (player_last_side_touch(work, record) != NULL) {
            /* @hack spelling: a switch with an empty default, where a test
             * of the grade alone would do */
            switch (player_stick_into_offer(work, record)) {
            case PLAYER_WALL_HEAD_ON:
                player_start_push(work, motion, record);
                sonic->by_character.sonic.hold_steps = 0;
                break;
            default:
                break;
            }
            break;
        }
        if (player_stick_back(work, record) &&
            record->spd.x >= record->consts.trot_speed) {
            work->mode = PLAYER_MODE_BRAKE;
            record->anim.next = ANIM_BRAKE;
            record->anim.after = ANIM_STAND;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic) ||
            player_test_fall(work, motion, record) ||
            sonic_test_jump(work, motion, record, sonic)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (record->spd.x < record->consts.trot_speed) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_test_stand(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_1:
    case PLAYER_MODE_SONIC_COMBO_2:
    case PLAYER_MODE_SONIC_COMBO_3:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_1:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_2:
    case PLAYER_MODE_SONIC_COMBO_RUNNING_3:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            if (record->anim.next != ANIM_TEETER) {
                record->anim.next = ANIM_FALL;
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            break;
        }
        if (sonic_combo(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_WAIT:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_combo(work, motion, record, sonic)) {
            break;
        }
        if (record->anim.next != ANIM_COMBO_WAIT &&
            record->anim.next != ANIM_LAND) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_BACK:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_COMBO_BACK) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_SONIC_SPIN:
        if (sonic_test_request(work, motion, record, sonic)) {
            sound_stop_id(SOUND_SPIN);
            sound_stop_id(SOUND_SPIN_SHOES);
            break;
        }
        if (player_test_fall(work, motion, record)) {
            record->anim.next = ANIM_SPIN;
            record->anim_speed = SPIN_PACE;
            work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
            sound_stop_id(SOUND_SPIN);
            sound_stop_id(SOUND_SPIN_SHOES);
            break;
        }
        if (sonic_spin(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_ROLL:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            record->anim.next = ANIM_SPIN;
            record->anim_speed = SPIN_PACE;
            work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_test_slow(work, motion, record)) {
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            break;
        }
        if (player_pad_records[record->slot].press & pad_action_buttons) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_STAND_UP;
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        }
        break;
    case PLAYER_MODE_SONIC_LIGHT_DASH:
        if (sonic_test_request(work, motion, record, sonic)) {
            sonic->by_character.sonic.light_dash_steps = 0;
            sound_stop_id(SOUND_LIGHT_DASH);
            if (record->spd.x > LIGHT_DASH_END_SPEED) {
                record->spd.x = LIGHT_DASH_END_SPEED;
            }
            work->flags &= ~PLAYER_WORK_ATTACK;
            break;
        }
        if (sonic->by_character.sonic.light_dash_steps-- <= 0 ||
            sonic->by_character.sonic.steps++ > LIGHT_DASH_TIMEOUT) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            if (player_stick_ahead(work, record) >= PLAYER_WALL_ACROSS &&
                sonic->by_character.sonic.light_dash_rings >
                    LIGHT_DASH_SHORT_RINGS) {
                record->spd.x = record->consts.full_speed;
                if (njScalor(&record->spd) > 0.0F) {
                    njUnitVector(&record->spd);
                    record->spd.x *= record->consts.full_speed;
                    record->spd.y *= record->consts.full_speed;
                    record->spd.z *= record->consts.full_speed;
                }
            } else if (njScalor(&record->spd) > LIGHT_DASH_END_SPEED) {
                njUnitVector(&record->spd);
                record->spd.x *= LIGHT_DASH_END_SPEED;
                record->spd.y *= LIGHT_DASH_END_SPEED;
                record->spd.z *= LIGHT_DASH_END_SPEED;
            }
            sonic->by_character.sonic.light_dash_steps = 0;
            work->flags &= ~PLAYER_WORK_ATTACK;
            sound_stop_id(SOUND_LIGHT_DASH);
            break;
        } else {
            struct collision_work *hit;
            struct task *task;

            hit = work->collision->hit;
            if (hit != NULL) {
                task = hit->owner;
                if (task != NULL) {
                    if (task->work->id == WORK_ID_STOP_LSD) {
                        work->mode = PLAYER_MODE_FALL;
                        record->anim.next = ANIM_FALL;
                        record->spd.x = 0.0F;
                        sonic->by_character.sonic.light_dash_steps = 0;
                        work->flags &= ~PLAYER_WORK_ATTACK;
                        sound_stop_id(SOUND_LIGHT_DASH);
                        break;
                    } else if (task->work->id == WORK_ID_RING) {
                        sonic->by_character.sonic.steps = 0;
                        if (sonic->by_character.sonic.light_dash_rings < 1) {
                            sonic->by_character.sonic.light_dash_ring_pos =
                                task->work->pos;
                            sonic->by_character.sonic.light_dash_rings++;
                        } else if (njDistanceP2P(&sonic->by_character.sonic
                                                      .light_dash_ring_pos,
                                                 &task->work->pos) >
                                   LIGHT_DASH_RINGS_APART) {
                            sonic->by_character.sonic.light_dash_rings++;
                            sonic->by_character.sonic.light_dash_ring_pos =
                                task->work->pos;
                        }
                    }
                }
            }
        }
        {
            NJS_POINT3 pos;

            player_get_past(record->slot, 1, &pos, NULL);
            if (njDistanceP2P(&pos, &work->pos) <= LIGHT_DASH_STUCK) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
                if (record->spd.x > LIGHT_DASH_END_SPEED) {
                    record->spd.x = LIGHT_DASH_END_SPEED;
                }
                work->flags &= ~PLAYER_WORK_ATTACK;
                sonic->by_character.sonic.light_dash_steps = 0;
                sound_stop_id(SOUND_LIGHT_DASH);
                break;
            }
        }
        break;
    case PLAYER_MODE_SONIC_LIGHT_ATTACK:
        /* @bug logic: never ends the attack by its steps.
         * sonic_light_attack_seek, called first, sets steps to 0 in this mode,
         * MOV.W at 0x8c06fb98, so the test of LIGHT_ATTACK_TIMEOUT, CMP/GT at
         * 0x8c068d5c, compares 0 */
        if (sonic_light_attack_seek(work, record, sonic) == NULL ||
            pad_action_pressed[record->slot] ||
            sonic->by_character.sonic.steps++ > LIGHT_ATTACK_TIMEOUT) {
            Float speed;

            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            speed = record->consts.jump_speed;
            motion->spd.x = world_gravity.x * speed;
            motion->spd.y = world_gravity.y * speed;
            motion->spd.z = world_gravity.z * speed;
            record->spd.x = 0.0F;
            record->spd.y = speed;
            record->spd.z = 0.0F;
        }
        break;
    case PLAYER_MODE_PATH_ROLL:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            record->anim.next = ANIM_SPIN;
            record->anim_speed = SPIN_PACE;
            work->ang.x = work->ang.z = 0;
            work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (record->spd.x < PLAYER_PATH_ROLL_MIN) {
            record->anim.next = ANIM_SPIN;
            record->anim_speed = SPIN_PACE;
            work->ang.x = work->ang.z = 0;
            work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_SONIC_RAIL:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        /* @hack spelling: the empty arm, where a test of the bit's absence
         * would do */
        if (work->flags & PLAYER_WORK_ON_PATH) {
        } else {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (sonic_test_soap_switch(work, record, sonic)) {
            break;
        }
        {
            int grade;

            if (player_jump_pressed(work, record)) {
                grade = player_stick_ahead(work, record);
                if (grade != PLAYER_WALL_NONE && grade <= PLAYER_WALL_ASKEW) {
                    Angle turn;

                    work->mode = PLAYER_MODE_FALL;
                    work->flags &= ~PLAYER_WORK_ON_PATH;
                    if (record->character == PLAYER_SONIC) {
                        sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
                    } else {
                        sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
                    }
                    player_read_stick(work, record, &turn, NULL);
                    turn = angle_difference(work->ang.y, turn);
                    if (turn > 0) {
                        work->ang.y += leap_turn - (leap_turn_per_speed *
                                                    njScalor(&record->spd));
                    } else if (turn < 0) {
                        work->ang.y -= leap_turn - (leap_turn_per_speed *
                                                    njScalor(&record->spd));
                    }
                    if (record->spd.x < record->consts.running_speed) {
                        record->spd.x = record->consts.running_speed;
                    }
                    record->spd.y += leap_rise - (leap_rise_per_speed *
                                                  njScalor(&record->spd));
                    if (record->stick_lock < LEAP_STICK_LOCK) {
                        record->stick_lock = LEAP_STICK_LOCK;
                    }
                    record->anim.next = ANIM_FALL;
                    break;
                }
            }
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (sonic->by_character.sonic.ball_steps > RAIL_TOTTER_STEPS) {
            if (record->anim.next == ANIM_RAIL_TOTTER_MINUS) {
                work->ang.y -= QUARTER_TURN;
            } else if (record->anim.next == ANIM_RAIL_TOTTER_PLUS) {
                work->ang.y += QUARTER_TURN;
            }
            motion->aim_y = work->ang.y;
            if (record->spd.x < record->consts.running_speed) {
                record->spd.x = record->consts.running_speed;
            }
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (!pad_action_held[record->slot]) {
            OFFER(record, PLAYER_MODE_SONIC_RAIL);
        }
        break;
    case PLAYER_MODE_SONIC_RAIL_BENEATH:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        /* @hack spelling: the empty arm, where a test of the bit's absence
         * would do */
        if (work->flags & PLAYER_WORK_ON_PATH) {
        } else {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            sonic_drop(work);
            break;
        }
        if (player_jump_pressed(work, record)) {
            work->mode = PLAYER_MODE_FALL;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            if (record->character == PLAYER_SONIC) {
                sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
            } else {
                sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
            }
            if (record->spd.x < record->consts.running_speed) {
                record->spd.x = record->consts.running_speed;
            }
            record->spd.y = RAIL_BENEATH_JUMP_SPEED;
            record->anim.next = ANIM_FALL;
            sonic_drop(work);
        }
        break;
    case PLAYER_MODE_SONIC_SOAP_JUMP:
        if (sonic_test_request(work, motion, record, sonic)) {
            camera_motion_end(record->slot);
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
                work->mode = PLAYER_MODE_STAND;
                record->mode_steps = 0;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            camera_motion_end(record->slot);
            if (sonic->by_character.sonic.trick_grade == SONIC_TRICK_GRADE_A) {
                technique_score_start(TRICK_SCORE_A);
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_B) {
                technique_score_start(TRICK_SCORE_B);
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_C) {
                technique_score_start(TRICK_SCORE_C);
            }
        }
        break;
    case PLAYER_MODE_SONIC_RAMP_JUMP:
        if (sonic_test_request(work, motion, record, sonic)) {
            camera_motion_end(record->slot);
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            camera_motion_end(record->slot);
            if (sonic->by_character.sonic.trick_grade == SONIC_TRICK_GRADE_A) {
                technique_score_start(TRICK_SCORE_A);
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_B) {
                technique_score_start(TRICK_SCORE_B);
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_C) {
                technique_score_start(TRICK_SCORE_C);
            }
        }
        break;
    case PLAYER_MODE_JUMP:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
                if (pad_action_held[record->slot] &&
                    !(work->flags & PLAYER_WORK_CHARGED)) {
                    sonic->by_character.sonic.hold_steps = 1;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                if (pad_action_held[record->slot] &&
                    !(work->flags & PLAYER_WORK_CHARGED)) {
                    sonic->by_character.sonic.hold_steps = 1;
                }
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            break;
        }
        if (player_jump_pressed(work, record) &&
            (_builtin_fabsf(world_gravity.y) != 1.0F ||
             _builtin_fabsf(record->ground.below_y - work->pos.y) >
                 HOMING_HEIGHT ||
             stage_current_id() == STAGE_BOSS_GOLEM)) {
            homing_start(work, motion, record, sonic);
            if (sonic->by_character.sonic.homing > 0) {
                work->mode = PLAYER_MODE_SONIC_HOMING;
                record->anim.next = ANIM_HOMING_DASH;
            } else {
                work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
                work->mode = PLAYER_MODE_SONIC_HOMING;
            }
            if (stage_current_id() == STAGE_GREEN_HILL) {
                break;
            }
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_HOMING_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_HOMING_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            break;
        }
        if (sonic_test_bounce(work, record, sonic)) {
            break;
        }
        if (player_count_jump_hold(work, record)) {
            record->spd.y += record->consts.jump_lift * PLAYER_JUMP_LIFT;
        }
        if (record->anim.next == ANIM_JUMP ||
            (record->anim.next == ANIM_JUMP_RISE &&
             record->anim.frame >
                 (motion_table[record->anim.actions[ANIM_JUMP_RISE].motion]
                      .motion->nbFrame *
                  JUMP_RISE_SHARE))) {
            if (record->spd.y <= 0.0F) {
                record->anim.next = ANIM_JUMP_FALL;
            }
        }
        if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        }
        break;
    case PLAYER_MODE_SONIC_BOUNCE:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            work->mode = PLAYER_MODE_SONIC_BOUNCE_UP;
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            if (sonic->by_character.sonic.steps == 0) {
                record->spd.y = BOUNCE_UP_SPEED;
                sonic->by_character.sonic.steps = 1;
            } else {
                record->spd.y = BOUNCE_UP_AGAIN_SPEED;
            }
            vibration_start(record->slot, 7, 15, VIBRATION_UNTIMED);
            break;
        }
        if (player_jump_pressed(work, record) &&
            (_builtin_fabsf(world_gravity.y) != 1.0F ||
             _builtin_fabsf(record->ground.below_y - work->pos.y) >
                 HOMING_HEIGHT)) {
            homing_start(work, motion, record, sonic);
            if (sonic->by_character.sonic.homing > 0) {
                work->mode = PLAYER_MODE_SONIC_HOMING;
                record->anim.next = ANIM_HOMING_DASH;
                work->flags &= ~PLAYER_WORK_BALL;
            } else {
                work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
                work->mode = PLAYER_MODE_SONIC_HOMING;
            }
            if (stage_current_id() == STAGE_GREEN_HILL) {
                break;
            }
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_HOMING_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_HOMING_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        break;
    case PLAYER_MODE_SONIC_BOUNCE_UP:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            break;
        }
        if (player_jump_pressed(work, record) &&
            (_builtin_fabsf(world_gravity.y) != 1.0F ||
             _builtin_fabsf(record->ground.below_y - work->pos.y) >
                 HOMING_HEIGHT)) {
            homing_start(work, motion, record, sonic);
            if (sonic->by_character.sonic.homing > 0) {
                work->mode = PLAYER_MODE_SONIC_HOMING;
                record->anim.next = ANIM_HOMING_DASH;
                work->flags &= ~PLAYER_WORK_BALL;
            } else {
                work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
                work->mode = PLAYER_MODE_SONIC_HOMING;
            }
            if (stage_current_id() == STAGE_GREEN_HILL) {
                break;
            }
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_HOMING_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_HOMING_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            break;
        }
        if (sonic_test_bounce_again(work, record)) {
            break;
        }
        if (record->anim.next == ANIM_BALL && record->spd.y <= 0.0F) {
            record->anim.next = ANIM_JUMP_FALL;
            work->flags &= ~PLAYER_WORK_BALL;
        }
        if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        }
        break;
    case PLAYER_MODE_BOUNCE:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_LAUNCH:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_FLING:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
        }
        break;
    case PLAYER_MODE_FALL:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                }
                if (pad_action_held[record->slot] &&
                    !(work->flags & PLAYER_WORK_CHARGED)) {
                    sonic->by_character.sonic.hold_steps = 1;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                if (pad_action_held[record->slot] &&
                    !(work->flags & PLAYER_WORK_CHARGED)) {
                    sonic->by_character.sonic.hold_steps = 1;
                }
            }
        }
        if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        }
        break;
    case PLAYER_MODE_TEETER:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (record->mode_steps++ > PLAYER_TEETER_STEPS ||
            pad_stick_pushed[record->slot] || pad_jump_pressed[record->slot] ||
            pad_action_pressed[record->slot]) {
            if (work->flags & PLAYER_WORK_GROUNDED) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                player_stand(work, motion, record);
            } else {
                work->mode = PLAYER_MODE_FALL;
                record->anim.next = ANIM_FALL;
            }
        }
        break;
    case PLAYER_MODE_BRAKE:
        if (sonic_test_request(work, motion, record, sonic) ||
            sonic_test_action(work, motion, record, sonic) ||
            player_test_fall(work, motion, record) ||
            sonic_test_jump(work, motion, record, sonic)) {
            sonic->by_character.sonic.hold_steps = 0;
            break;
        }
        if (sonic_combo(work, motion, record, sonic)) {
            break;
        }
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
            break;
        }
        if (record->anim.next != ANIM_BRAKE) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_SONIC_HOMING:
        if (sonic_test_request(work, motion, record, sonic)) {
            sonic->by_character.sonic.homing_hits = 0;
            break;
        }
        {
            struct task *own;
            struct task *task;
            struct collision_work *collision;
            NJS_VECTOR way;

            own = player_tasks[record->slot];
            /* @hack redundant: the null that task starts at, which the test
             * sets before any read */
            task = NULL;
            /* @hack spelling: the first call's assignment inside the test,
             * where a statement of its own would do */
            if ((task = collision_next_player(own)) != NULL ||
                (task = collision_next_item_or_enemy(own)) != NULL ||
                ((task = collision_next_object(own)) != NULL &&
                 task->work->id == WORK_ID_HOMING_TARGET)) {
                collision = task->work->collision;
                sonic->by_character.sonic.homing = -1;
                work->mode = PLAYER_MODE_JUMP;
                if (sonic->by_character.sonic.homing_hits == 0) {
                    record->anim.next = ANIM_HOMING_HIT_1;
                } else if (sonic->by_character.sonic.homing_hits == 1) {
                    record->anim.next = ANIM_HOMING_HIT_2;
                } else {
                    record->anim.next = ANIM_COMBO_JUMP;
                }
                work->flags &= ~PLAYER_WORK_BALL;
                if (++sonic->by_character.sonic.homing_hits >= HOMING_HITS) {
                    sonic->by_character.sonic.homing_hits = 0;
                }
                if (collision->kind == COLLISION_PLAYER) {
                    way = motion->spd;
                    way.y = 0.0F;
                    njUnitVector(&way);
                    if (njScalor(&way) > 0.0F) {
                        player_face_along(record->slot, way.x, 0.0F, way.z);
                        way.x *= HOMING_REBOUND_BACK;
                        way.y = HOMING_REBOUND_UP;
                        way.z *= HOMING_REBOUND_BACK;
                        player_set_speed(record->slot, way.x, way.y, way.z);
                    }
                }
                break;
            }
        }
        {
            NJS_POINT3 pos;

            player_get_past(record->slot, 1, &pos, NULL);
            if (njDistanceP2P(&pos, &work->pos) <= HOMING_STUCK &&
                sonic->by_character.sonic.homing == 0) {
                sonic->by_character.sonic.homing = 1;
            }
        }
        if (sonic->by_character.sonic.homing >= HOMING_COAST_STEPS ||
            sonic->by_character.sonic.steps++ >= HOMING_TIMEOUT) {
            sonic->by_character.sonic.homing = 0;
            work->mode = PLAYER_MODE_FALL;
            if (record->anim.next == ANIM_HOMING_DASH) {
                record->anim.next = ANIM_FALL;
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            sonic->by_character.sonic.homing_hits = 0;
            break;
        }
        if (pad_action_pressed[record->slot]) {
            sonic->by_character.sonic.homing = 0;
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            sonic->by_character.sonic.homing_hits = 0;
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            sonic->by_character.sonic.homing_hits = 0;
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_STAND;
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    record->anim.next = ANIM_LAND_STILL;
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
        }
        break;
    case PLAYER_MODE_UNK_0E:
        if (sonic_test_request(work, motion, record, sonic) ||
            sonic_test_action(work, motion, record, sonic) ||
            sonic_combo(work, motion, record, sonic) ||
            sonic_test_jump(work, motion, record, sonic) ||
            player_test_fall(work, motion, record) ||
            player_test_stand(work, motion, record)) {
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        if ((work->collision->flags & COLLISION_PUSHED) &&
            !(work->flags & WORK_HURT) &&
            (!(work->collision->hit->shapes[work->collision->hit_shape].flags &
               SHAPE_UNK_2000) ||
             (work->flags & PLAYER_WORK_HOLDING)) &&
            (work->collision->flags & COLLISION_SIDE)) {
            switch (player_stick_into_offer(work, record)) {
            case PLAYER_WALL_HEAD_ON:
                break;
            case PLAYER_WALL_ASKEW:
                if (!(work->flags & PLAYER_WORK_HOLDING)) {
                    work->mode = PLAYER_MODE_RUN;
                    record->anim.next = ANIM_WALK;
                    sound_stop_id(PLAYER_SOUND_PUSH);
                }
                break;
            default:
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_20;
                    record->anim.next = ANIM_DRAG;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                    record->anim.next = ANIM_WALK;
                }
                sound_stop_id(PLAYER_SOUND_PUSH);
                break;
            }
            break;
        }
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_RUN;
        record->anim.next = ANIM_WALK;
        sound_stop_id(PLAYER_SOUND_PUSH);
        break;
    case PLAYER_MODE_HURT:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (record->anim.next != ANIM_HURT &&
            record->anim.next != ANIM_HURT_LAND) {
            if (player_test_stand(work, motion, record)) {
                player_stand(work, motion, record);
                break;
            } else {
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                break;
            }
        }
        /* @hack spelling: the unsigned timer tested as <= 0, where == 0 would
         * do */
        else if (work->timer <= 0) {
            if (work->flags & PLAYER_WORK_GROUNDED) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                break;
            } else {
                work->mode = PLAYER_MODE_FALL;
                record->anim.next = ANIM_FALL;
                break;
            }
        }
        if (work->timer <= PLAYER_HURT_STEER_STEPS &&
            player_read_stick(work, record, NULL, NULL)) {
            if (sonic_test_jump(work, motion, record, sonic)) {
                break;
            }
            if (work->flags & PLAYER_WORK_GROUNDED) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
                break;
            }
            break;
        }
        if (work->timer == PLAYER_HIT_STEPS) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            record->anim.next = ANIM_HURT_LAND;
        }
        break;
    case PLAYER_MODE_HANG:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_PLAY_ANIM:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (sonic_test_request_holding(work, motion, record, sonic)) {
                break;
            }
        } else if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_CRASH:
        if (record->anim.next == ANIM_CRASH) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_CRASH_END) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_SLIDE:
        if (sonic_test_request(work, motion, record, sonic) ||
            sonic_test_action(work, motion, record, sonic) ||
            player_test_fall(work, motion, record) ||
            sonic_test_jump(work, motion, record, sonic) ||
            sonic_combo(work, motion, record, sonic)) {
            sonic->by_character.sonic.hold_steps = 0;
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            break;
        }
        if (njScalor(&motion->spd) < record->consts.slide_speed ||
            record->ball->up.y > PLAYER_SLIDE_LEVEL) {
            work->mode = PLAYER_MODE_STAND;
            record->mode_steps = 0;
            record->anim.next = ANIM_LAND_STILL;
        }
        break;
    case PLAYER_MODE_CAUGHT:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (pad_stick_pushed[record->slot] ||
            (player_pad_records[record->slot].press &
             PLAYER_STRUGGLE_BUTTONS)) {
            sonic->by_character.sonic.steps -= PLAYER_STRUGGLE_PUSH;
        }
        if (sonic->by_character.sonic.steps > 0) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_2A:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (!(record->anim.flags & PLAYER_ANIM_DONE)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_ASK_HINT:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_PLAY_FLUTE:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        work->mode = PLAYER_MODE_UNK_35;
        break;
    case PLAYER_MODE_PLAY_FLUTE_AGAIN:
    case PLAYER_MODE_PLAYING_FLUTE_AGAIN:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (record->anim.current != ANIM_FLUTE &&
            record->anim.current != ANIM_FLUTE_AGAIN) {
            player_stand(work, motion, record);
            break;
        }
        work->mode = PLAYER_MODE_PLAYING_FLUTE_AGAIN;
        break;
    case PLAYER_MODE_CANT_PLAY_FLUTE:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        work->mode = PLAYER_MODE_UNK_35;
        break;
    case PLAYER_MODE_HOLD_HANDLE:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_UNK_35:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (!(record->anim.flags & PLAYER_ANIM_DONE)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_38:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (record->anim.current != ANIM_SINK &&
            record->anim.next != ANIM_SINK &&
            sonic_test_jump(work, motion, record, sonic)) {
            record->jump_steps = record->consts.jump_hold_steps;
            record->spd.y *= SINK_JUMP_SHARE;
            if (sonic->by_character.sonic.steps > SINK_JUMP_STEPS) {
                record->spd.y *= SINK_JUMP_LATE_SHARE;
            } else {
                record->spd.y -= record->spd.y *
                                 sonic->by_character.sonic.steps *
                                 SINK_JUMP_FADE;
            }
        }
        sonic->by_character.sonic.steps++;
        break;
    case PLAYER_MODE_SONIC_MAGIC_HAND:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic->by_character.sonic.steps++ <= MAGIC_HAND_STEPS) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_SWIM:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (game_state.time_stopped ||
            !(record->ground.last_above_flags & WORLD_FLAT)) {
            if (work->flags & PLAYER_WORK_GROUNDED) {
                if (player_test_stand(work, motion, record)) {
                    work->ang.x = motion->aim_x;
                    work->ang.z = motion->aim_z;
                    record->anim.next = ANIM_LAND_FAST;
                } else {
                    work->ang.x = motion->aim_x;
                    work->ang.z = motion->aim_z;
                    work->mode = PLAYER_MODE_RUN;
                    player_run_anim(work, motion, record);
                }
            } else {
                work->mode = PLAYER_MODE_FALL;
                record->anim.next = ANIM_FALL;
            }
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        /* @hack redundant: the break that ends the arm, which the break
         * after it does */
        if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        break;
    case PLAYER_MODE_PICK_UP:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (sonic_test_request_holding(work, motion, record, sonic)) {
                break;
            }
        } else if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_test_pick_up(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_PULL_HEAVY:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (player_test_pull_heavy(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_UNK_17:
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_GRAB_PUSHABLE:
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        break;
    case PLAYER_MODE_STAND_HOLDING:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (player_test_hold_end(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (player_test_shake_held(work, record)) {
            work->mode = PLAYER_MODE_SHAKE_HOLDING;
            record->anim.next = ANIM_HOLD_SHAKE;
            player_stop(motion, record);
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_WALK;
        }
        break;
    case PLAYER_MODE_UNK_19:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (player_test_hold_end(work, motion, record) ||
            sonic_test_action(work, motion, record, sonic) ||
            player_test_fall(work, motion, record) ||
            sonic_test_jump(work, motion, record, sonic) ||
            player_test_stand(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (player_stick_back(work, record) &&
            record->spd.x >= record->consts.trot_speed) {
            work->mode = PLAYER_MODE_UNK_1B;
            record->anim.next = ANIM_HOLD_BRAKE;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            work->flags &= ~PLAYER_WORK_ON_PATH;
            /* @hack redundant: the break that ends the arm, which the break
             * after it does */
            break;
        }
        break;
    case PLAYER_MODE_UNK_14:
    case PLAYER_MODE_THROW:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (sonic_test_request_holding(work, motion, record, sonic)) {
                break;
            }
        } else if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_throw(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT &&
            record->anim.next != ANIM_THROW &&
            record->anim.next != ANIM_SET_DOWN) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_UNK_1B:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            work->mode = PLAYER_MODE_STAND_HOLDING;
            record->anim.next = ANIM_HOLD_STAND;
        }
        break;
    case PLAYER_MODE_FALL_HOLDING:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_UNK_1B;
                record->anim.next = ANIM_HOLD_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_HOLD_STAND;
                } else {
                    record->anim.next = ANIM_HOLD_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_UNK_19;
                player_run_anim(work, motion, record);
            }
        }
        break;
    case PLAYER_MODE_JUMP_HOLDING:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_UNK_1B;
                record->anim.next = ANIM_HOLD_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_HOLD_STAND;
                } else {
                    record->anim.next = ANIM_HOLD_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_UNK_19;
                player_run_anim(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            break;
        }
        if (player_count_jump_hold(work, record)) {
            record->spd.y +=
                record->consts.jump_lift * PLAYER_JUMP_HOLDING_LIFT;
        }
        if (player_test_hold_end(work, motion, record)) {
            break;
        }
        if (sonic_test_action(work, motion, record, sonic)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL_HOLDING;
            record->anim.next = ANIM_HOLD_FALL_START;
        }
        break;
    case PLAYER_MODE_BOUNCE_HOLDING:
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL_HOLDING;
            record->anim.next = ANIM_HOLD_FALL_START;
        }
        break;
    case PLAYER_MODE_LAUNCH_HOLDING:
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL_HOLDING;
            record->anim.next = ANIM_HOLD_FALL_START;
        }
        break;
    case PLAYER_MODE_UNK_20:
        if (sonic_test_request(work, motion, record, sonic) ||
            sonic_test_action(work, motion, record, sonic) ||
            sonic_test_jump(work, motion, record, sonic) ||
            player_test_fall(work, motion, record)) {
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        if ((work->collision->flags & COLLISION_PUSHED) &&
            !(work->flags & WORK_HURT) &&
            (!(work->collision->hit->shapes[work->collision->hit_shape].flags &
               SHAPE_UNK_2000) ||
             (work->flags & PLAYER_WORK_HOLDING)) &&
            (work->collision->flags & COLLISION_SIDE)) {
            switch (player_stick_into_offer(work, record)) {
            case PLAYER_WALL_HEAD_ON:
                player_start_push(work, motion, record);
                break;
            case PLAYER_WALL_NONE:
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_GRAB_PUSHABLE;
                    record->anim.next = ANIM_GRAB_STAND;
                    sound_stop_id(PLAYER_SOUND_PUSH);
                }
                break;
            default:
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_20;
                    record->anim.next = ANIM_DRAG;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                    record->anim.next = ANIM_WALK;
                }
                sound_stop_id(PLAYER_SOUND_PUSH);
                break;
            }
            break;
        } else {
            /* @hack redundant: the else after the break, and the break that
             * ends it, which the break after it does */
            player_release_held(work, motion, record);
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            sound_stop_id(PLAYER_SOUND_PUSH);
            break;
        }
        break;
    case PLAYER_MODE_AIR_THROW:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (sonic_test_request_holding(work, motion, record, sonic)) {
                break;
            }
        } else if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_UNK_1B;
                record->anim.next = ANIM_HOLD_BRAKE;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (work->flags & PLAYER_WORK_HOLDING) {
                    if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                        record->anim.next = ANIM_HOLD_STAND;
                    } else {
                        record->anim.next = ANIM_HOLD_LAND_FAST;
                        vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                        LANDING_VIBRATION_FREQ,
                                        VIBRATION_UNTIMED);
                    }
                } else if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
                    player_stand_anim(work, motion, record);
                } else if (njScalor(&motion->spd) < PLAYER_LAND_SLOW) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_19;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                }
                player_run_anim(work, motion, record);
            }
            break;
        }
        if (sonic_throw(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_AIR_THROW_AT &&
            record->anim.next != ANIM_AIR_THROW) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
        }
        break;
    case PLAYER_MODE_UNK_22:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        if (player_touches_shakable(work, motion, record)) {
            if (player_test_shake_end(work, record)) {
                work->mode = PLAYER_MODE_UNK_17;
                record->anim.next = ANIM_GRAB_STAND;
            }
            break;
        }
        player_stand(work, motion, record);
        player_release_held(work, motion, record);
        break;
    case PLAYER_MODE_SHAKE_HOLDING:
        if (sonic_test_request_holding(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        if (record->held == NULL ||
            record->held->work->id != FALL_KIND_NORMAL ||
            !(work->flags & PLAYER_WORK_HOLDING)) {
            player_stand(work, motion, record);
            player_release_held(work, motion, record);
            break;
        }
        if (player_test_shake_end(work, record)) {
            work->mode = PLAYER_MODE_STAND_HOLDING;
            record->anim.next = ANIM_HOLD_STAND;
            /* @hack redundant: the break that ends the arm, which the break
             * after it does */
            break;
        }
        break;
    case PLAYER_MODE_THROW_AT_TOUCH:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (sonic_test_request_holding(work, motion, record, sonic)) {
                break;
            }
        } else if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (sonic_throw_at_mark(player_tasks[record->slot], work, motion,
                                record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT_TOUCH) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_PET_CHAO:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_test_jump(work, motion, record, sonic)) {
            break;
        }
        /* @hack redundant: the break inside the test, which the break after
         * it does */
        if (!player_test_petting(work, motion, record)) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_PULL_BURIED:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (sonic_test_request_holding(work, motion, record, sonic)) {
                break;
            }
        } else if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (sonic_test_pull(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_HANG:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_CLIMB:
        if (sonic_test_request(work, motion, record, sonic)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            player_onto_ledge(work, motion, record);
            work->mode = PLAYER_MODE_STAND;
        }
        break;
    case PLAYER_MODE_SONIC_BOARD:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            work->mode = PLAYER_MODE_SONIC_BOARD_FALL;
            record->anim.next = ANIM_BOARD_FALL;
            break;
        }
        if (pad_jump_pressed[record->slot]) {
            work->mode = PLAYER_MODE_SONIC_BOARD_JUMP;
            record->anim.next = ANIM_BOARD_JUMP;
            record->spd.y = record->consts.jump_speed;
            if (record->spd.x < record->consts.sprint_speed) {
                record->spd.x += record->consts.trot_speed;
            }
            if (record->ground_task != NULL) {
                struct task_force *force;

                force = record->ground_task->force.p;
                if (force != NULL) {
                    force = &force[record->slot];
                    player_set_acc(record->slot, force->speed.x, force->speed.y,
                                   force->speed.z);
                }
                if (record->ground_task->work->id == WORK_ID_RAMP) {
                    NJS_VECTOR in;
                    NJS_VECTOR out;
                    struct task_work *ramp;
                    Angle ang_x;
                    Angle ang_y;
                    Angle ang_z;
                    NJS_VECTOR axis;

                    work->mode = PLAYER_MODE_SONIC_BOARD_TRICK;
                    ramp = record->ground_task->work;
                    in.x = 0.0F;
                    in.y = RAMP_THROW_UP;
                    if (record->spd.x < record->consts.full_speed) {
                        in.z = RAMP_THROW_AHEAD;
                    } else if (record->spd.x <
                               record->consts.full_speed + RAMP_THROW_AHEAD) {
                        /* @bug logic: the throw ahead drops at both ends of
                         * this arm, as sonic_test_jump's does, FSUB at
                         * 0x8c06c900 */
                        in.z = record->spd.x - record->consts.full_speed;
                    } else {
                        in.z = 0.0F;
                    }
                    njPushMatrix(&_nj_unit_matrix_);
                    njRotateZ(NULL, ramp->ang.z);
                    njRotateY(NULL, ramp->ang.y);
                    njRotateX(NULL, ramp->ang.x);
                    njCalcVector(NULL, &in, &out);
                    in.y = 0.0F;
                    in.x = 0.0F;
                    in.z = 1.0F;
                    njCalcVector(NULL, &in, &axis);
                    ang_y = NJM_RAD_ANG(atan2f(axis.x, axis.z));
                    in.x = in.z = 0.0F;
                    in.y = 1.0F;
                    njCalcVector(NULL, &in, &axis);
                    ang_x = NJM_RAD_ANG(asinf(axis.z));
                    ang_z = -NJM_RAD_ANG(asinf(axis.x));
                    player_set_angles(record->slot, ang_x, ang_y, ang_z);
                    njPopMatrixEx();
                    player_set_acc(record->slot, out.x, out.y, out.z);
                    if (record->spd.x > BOARD_TRICK_A_SPEED) {
                        record->anim.next = ANIM_BOARD_TRICK_A;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_A;
                        if (record->kind == PLAYER_SONIC) {
                            voice_play_priority_2(VOICE_TRICK_A_SONIC);
                        } else if (record->kind == PLAYER_SHADOW) {
                            voice_play_priority_2(VOICE_TRICK_A_SHADOW);
                        } else if (record->kind == PLAYER_KIND_AMY) {
                            voice_play_priority_2(VOICE_TRICK_AMY);
                        }
                    } else if (record->spd.x > BOARD_TRICK_B_SPEED) {
                        record->anim.next = ANIM_BOARD_TRICK_B;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_B;
                        if (record->kind == PLAYER_SONIC) {
                            voice_play_priority_2(VOICE_TRICK_B_SONIC);
                        } else if (record->kind == PLAYER_SHADOW) {
                            voice_play_priority_2(VOICE_TRICK_B_SHADOW);
                        } else if (record->kind == PLAYER_KIND_AMY) {
                            voice_play_priority_2(VOICE_TRICK_AMY);
                        }
                    } else if (record->spd.x > BOARD_TRICK_C_SPEED) {
                        record->anim.next = ANIM_BOARD_TRICK_C;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_C;
                        if (record->kind == PLAYER_SONIC) {
                            voice_play_priority_2(VOICE_TRICK_C_SONIC);
                        } else if (record->kind == PLAYER_SHADOW) {
                            voice_play_priority_2(VOICE_TRICK_C_SHADOW);
                        } else if (record->kind == PLAYER_KIND_AMY) {
                            voice_play_priority_2(VOICE_TRICK_AMY);
                        }
                    } else {
                        work->mode = PLAYER_MODE_SONIC_BOARD_JUMP;
                        record->anim.next = ANIM_BOARD_JUMP;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_NONE;
                    }
                    if (placement_get_state(record->ground_task)) {
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_PASSED;
                    }
                }
            }
            work->flags &= ~PLAYER_WORK_ON_SHAPE;
            record->jump_steps = 0;
            break;
        }
        if (pad_action_held[record->slot]) {
            Angle turn;

            work->mode = PLAYER_MODE_SONIC_BOARD_BRAKE;
            player_read_stick(work, record, &turn, NULL);
            turn = angle_difference(work->ang.y, turn);
            if (turn > 0) {
                record->anim.next = ANIM_BOARD_BRAKE_POS;
            } else {
                record->anim.next = ANIM_BOARD_BRAKE_NEG;
            }
            break;
        } else {
            OFFER(record, PLAYER_MODE_SONIC_BOARD_BRAKE);
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_FAST:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            work->mode = PLAYER_MODE_SONIC_BOARD_FALL;
            record->anim.next = ANIM_BOARD_FALL;
            break;
        }
        if (pad_jump_released[record->slot]) {
            work->mode = PLAYER_MODE_SONIC_BOARD_JUMP;
            record->anim.next = ANIM_BOARD_JUMP;
            record->spd.y = record->consts.jump_speed;
            if (record->spd.x < record->consts.sprint_speed) {
                record->spd.x += record->consts.trot_speed;
            }
            if (record->ground_task != NULL) {
                struct task_force *force;

                force = record->ground_task->force.p;
                if (force != NULL) {
                    force = &force[record->slot];
                    player_set_acc(record->slot, force->speed.x, force->speed.y,
                                   force->speed.z);
                }
                if (record->ground_task->work->id == WORK_ID_SOAP_SWITCH) {
                    NJS_VECTOR in;
                    NJS_VECTOR out;
                    struct task_work *soap;
                    Angle ang_x;
                    Angle ang_y;
                    Angle ang_z;
                    NJS_VECTOR axis;

                    work->mode = PLAYER_MODE_SONIC_BOARD_TRICK;
                    soap = record->ground_task->work;
                    in.x = 0.0F;
                    in.y = SOAP_THROW_UP;
                    in.z = SOAP_THROW_AHEAD;
                    njPushMatrix(&_nj_unit_matrix_);
                    njRotateZ(NULL, soap->ang.z);
                    njRotateY(NULL, soap->ang.y);
                    njRotateX(NULL, soap->ang.x);
                    njRotateY(NULL, HALF_TURN);
                    njCalcVector(NULL, &in, &out);
                    in.z = 0.0F;
                    in.x = 0.0F;
                    in.y = 1.0F;
                    njCalcVector(NULL, &in, &axis);
                    ang_y = NJM_RAD_ANG(atan2f(out.x, out.z));
                    ang_x = NJM_RAD_ANG(asinf(axis.z));
                    ang_z = -NJM_RAD_ANG(asinf(axis.x));
                    player_set_angles(record->slot, ang_x, ang_y, ang_z);
                    njPopMatrixEx();
                    player_set_acc(record->slot, out.x, out.y, out.z);
                    if (record->spd.x > BOARD_TRICK_A_SPEED) {
                        record->anim.next = ANIM_BOARD_TRICK_A;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_A;
                        if (record->kind == PLAYER_SONIC) {
                            voice_play_priority_2(VOICE_TRICK_A_SONIC);
                        } else if (record->kind == PLAYER_SHADOW) {
                            voice_play_priority_2(VOICE_TRICK_A_SHADOW);
                        } else if (record->kind == PLAYER_KIND_AMY) {
                            voice_play_priority_2(VOICE_TRICK_AMY);
                        }
                    } else if (record->spd.x > BOARD_TRICK_B_SPEED) {
                        record->anim.next = ANIM_BOARD_TRICK_B;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_B;
                        if (record->kind == PLAYER_SONIC) {
                            voice_play_priority_2(VOICE_TRICK_B_SONIC);
                        } else if (record->kind == PLAYER_SHADOW) {
                            voice_play_priority_2(VOICE_TRICK_B_SHADOW);
                        } else if (record->kind == PLAYER_KIND_AMY) {
                            voice_play_priority_2(VOICE_TRICK_AMY);
                        }
                    } else if (record->spd.x > BOARD_TRICK_C_SPEED) {
                        record->anim.next = ANIM_BOARD_TRICK_C;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_C;
                        if (record->kind == PLAYER_SONIC) {
                            voice_play_priority_2(VOICE_TRICK_C_SONIC);
                        } else if (record->kind == PLAYER_SHADOW) {
                            voice_play_priority_2(VOICE_TRICK_C_SHADOW);
                        } else if (record->kind == PLAYER_KIND_AMY) {
                            voice_play_priority_2(VOICE_TRICK_AMY);
                        }
                    } else {
                        work->mode = PLAYER_MODE_SONIC_BOARD_JUMP;
                        record->anim.next = ANIM_BOARD_JUMP;
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_NONE;
                    }
                    if (placement_get_state(record->ground_task)) {
                        sonic->by_character.sonic.trick_grade =
                            SONIC_TRICK_GRADE_PASSED;
                    }
                }
            }
            work->flags &= ~PLAYER_WORK_ON_SHAPE;
            record->jump_steps = 0;
            break;
        }
        if (!pad_jump_held[record->slot] && !pad_action_held[record->slot]) {
            work->mode = PLAYER_MODE_SONIC_BOARD;
            break;
        }
        /* @hack redundant: the second test of spd.x against trot_speed */
        if (player_stick_back(work, record) &&
            /* NOLINTNEXTLINE(misc-redundant-expression) */
            record->spd.x >= record->consts.trot_speed &&
            record->spd.x >= record->consts.trot_speed) {
            Angle turn;

            work->mode = PLAYER_MODE_SONIC_BOARD_BRAKE;
            player_read_stick(work, record, &turn, NULL);
            turn = angle_difference(work->ang.y, turn);
            if (turn > 0) {
                record->anim.next = ANIM_BOARD_BRAKE_POS;
            } else {
                record->anim.next = ANIM_BOARD_BRAKE_NEG;
            }
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_BRAKE:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            work->mode = PLAYER_MODE_SONIC_BOARD_FALL;
            record->anim.next = ANIM_BOARD_FALL;
            break;
        }
        if (pad_jump_released[record->slot]) {
            struct task_force *force;

            work->mode = PLAYER_MODE_SONIC_BOARD_JUMP;
            record->spd.y = record->consts.jump_speed;
            if (record->spd.x < record->consts.sprint_speed) {
                record->spd.x += record->consts.trot_speed;
            }
            /* @hack spelling: force's load and test in one comma
             * expression, where an assignment of its own would do */
            if (record->ground_task != NULL &&
                (force = record->ground_task->force.p, force != NULL)) {
                force = &force[record->slot];
                player_set_acc(record->slot, force->speed.x, force->speed.y,
                               force->speed.z);
            }
            record->anim.next = ANIM_BOARD_JUMP;
            work->flags &= ~PLAYER_WORK_ON_SHAPE;
            record->jump_steps = 0;
            if (record->character == PLAYER_SONIC) {
                sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
            } else {
                sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
            }
            break;
        }
        if (!pad_action_held[record->slot]) {
            work->mode = PLAYER_MODE_SONIC_BOARD;
            record->anim.next = ANIM_BOARD_LEAN_NEG;
            record->anim.frame = 0.0F;
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_FALL:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            work->mode = PLAYER_MODE_SONIC_BOARD;
            record->anim.next = ANIM_BOARD_LEAN_NEG;
            record->anim.frame = 0.0F;
            if (record->spd.x <= BOARD_LAND_SLOW) {
                record->spd.x = BOARD_LAND_SPEED;
            }
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            if (stage_current_id() == STAGE_CITY_ESCAPE) {
                sound_play_at(SOUND_BOARD_LAND_CITY, NULL, 0, 0, &work->pos);
            }
            vibration_start(record->slot, LANDING_VIBRATION_POWER,
                            LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_JUMP:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            work->mode = PLAYER_MODE_SONIC_BOARD;
            record->anim.next = ANIM_BOARD_LEAN_NEG;
            record->anim.frame = 0.0F;
            if (record->spd.x <= BOARD_LAND_SLOW) {
                record->spd.x = BOARD_LAND_SPEED;
            }
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            if (stage_current_id() == STAGE_CITY_ESCAPE) {
                sound_play_at(SOUND_BOARD_LAND_CITY, NULL, 0,
                              BOARD_JUMP_LAND_VOLUME, &work->pos);
            }
            vibration_start(record->slot, LANDING_VIBRATION_POWER,
                            LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
            break;
        }
        if (record->spd.y <= 0.0F && record->anim.current == ANIM_BOARD_JUMP) {
            record->anim.next = ANIM_BOARD_JUMP_FALL;
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_TRICK:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            work->mode = PLAYER_MODE_SONIC_BOARD;
            record->anim.next = ANIM_BOARD_LEAN_NEG;
            record->anim.frame = 0.0F;
            if (record->spd.x <= BOARD_LAND_SLOW) {
                record->spd.x = BOARD_LAND_SPEED;
            }
            work->ang.x = motion->aim_x;
            work->ang.z = motion->aim_z;
            if (sonic->by_character.sonic.trick_grade == SONIC_TRICK_GRADE_A) {
                technique_score_start(TRICK_SCORE_A);
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_B) {
                technique_score_start(TRICK_SCORE_B);
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_C) {
                technique_score_start(TRICK_SCORE_C);
            }
            if (stage_current_id() == STAGE_CITY_ESCAPE) {
                sound_play_at(SOUND_BOARD_LAND_CITY, NULL, 0, 0, &work->pos);
            }
            vibration_start(record->slot, LANDING_VIBRATION_POWER,
                            LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
            break;
        }
        break;
    case PLAYER_MODE_SONIC_BOARD_CRASH:
        if (sonic_board_take_request(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_BOARD_CRASH_POS &&
            record->anim.next != ANIM_BOARD_CRASH_NEG) {
            work->mode = PLAYER_MODE_SONIC_BOARD;
            record->anim.next = ANIM_BOARD_LEAN_NEG;
            record->anim.frame = 0.0F;
        }
        break;
    case PLAYER_MODE_EVENT:
        if (work->submode == PLAYER_REQUEST_RELEASE) {
            player_stand(work, motion, record);
            work->flags &=
                ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
        break;
    case PLAYER_MODE_FREE_MOVE:
        if (player_pad_records[record->slot].press & PDD_DGT_TA) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            work->flags &=
                ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
        break;
    }
}

static void sonic_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct player *sonic = task->extra.p;
    /* @hack spelling: record, a second pointer to the record taken through
     * its first field, where sonic alone would do */
    struct player *record = (struct player *)&sonic->slot;

    if (sonic->by_character.sonic.jiggle != NULL) {
        jiggle_free(sonic->by_character.sonic.jiggle);
        sonic->by_character.sonic.jiggle = NULL;
    }
    player_unregister(record->slot);
    sound_stop_key(work);
    njReleaseTexture(sonic->by_character.sonic.texlist);
    syFree(sonic->by_character.sonic.texlist);
    sonic->by_character.sonic.texlist = NULL;
    life_icon_release(record->kind);
    model_file_free(sonic->by_character.sonic.model_file);
    sonic->by_character.sonic.model_file = NULL;
    motion_file_free(sonic->by_character.sonic.motion_file);
    sonic->by_character.sonic.motion_file = NULL;
}

/* center both shapes center_height up the player's y axis from its pos, or
 * down it beneath a rail */
static void sonic_center_shapes(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record)
{
    struct collision_shape *shape = work->collision->shapes;
    NJS_VECTOR center;

    center.x = center.z = 0.0F;
    center.y = record->consts.center_height;
    if (work->mode == PLAYER_MODE_SONIC_RAIL_BENEATH) {
        /* @hack spelling: the product by -1, where a negation would do */
        center.y *= -1.0F;
    }
    turn_to_world(work, &center);
    shape->center = center;
    shape->center.x += work->pos.x;
    shape->center.y += work->pos.y;
    shape->center.z += work->pos.z;
    shape[BALL_SHAPE].center = shape->center;
}

/* leave an afterimage of SONIC_LIGHT_DASH's model each even step, where it is
 * loaded. Shadow and Metal Sonic leave none */
static void light_dash_afterimage(struct task_work *work, struct player *record,
                                  struct player *sonic)
{
    NJS_CNK_OBJECT *object;

    /* @hack spelling: the tests in two ifs and the else that returns, where
     * one test of all three would do */
    if (!(game_state.steps & 1)) {
        if (record->character == PLAYER_SONIC &&
            model_table[SONIC_LIGHT_DASH].object != NULL) {
            object = model_table[SONIC_LIGHT_DASH].object;
        } else {
            return;
        }
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        player_afterimage(object, NULL, sonic->by_character.sonic.texlist, 0.0F,
                          NULL, record->slot);
        njPopMatrixEx();
    }
}

/* the motion of animation index in the record's table, which
 * sonic_pose_afterimage and sonic_draw take through the table twice */
#define ANIM_MOTION(record, index)                                             \
    (motion_table                                                              \
         [(record)->anim.actions[(record)->anim.actions[index].motion].motion] \
             .motion)

void sonic_pose_afterimage(struct task_work *work, struct player *record,
                           struct player *sonic)
{
    NJS_CNK_OBJECT *object;
    int current;

    if (!(game_state.steps & 1)) {
        current = record->anim.current;
        object = model_table[record->anim.actions[current].model].object;
        if (object == model_table[SONIC_BODY].object &&
            model_table[SONIC_BODY_FAR].object != NULL) {
            object = model_table[SONIC_BODY_FAR].object;
        } else if (object == model_table[SHADOW_BODY].object &&
                   model_table[SHADOW_BODY_FAR].object != NULL) {
            object = model_table[SHADOW_BODY_FAR].object;
        }
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        player_afterimage(object, ANIM_MOTION(record, current),
                          sonic->by_character.sonic.texlist, record->anim.frame,
                          NULL, record->slot);
        njPopMatrixEx();
    }
}

/* sonic_draw's motion callback for Sonic's model and Amy's, which the draw
 * calls at each node of the body it shows. it keeps in drawn_record the places
 * in the world of the nodes the draws track. with each place it keeps one or
 * two of the node's axes.
 * it keeps the matrices of the nodes the upgrades draw at. it turns _LEAN about
 * x by the record's lean in PLAYER_MODE_RUN. while the player holds a task, it
 * turns _ARM_0 about x by the held task's motion's hold_turn, and _ARM_1 the
 * other way */
static void sonic_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *sonic = drawn_record;
    struct task_work *work;
    NJS_VECTOR origin;
    NJS_VECTOR axis;

    origin.z = 0.0F;
    origin.y = 0.0F;
    origin.x = 0.0F;
    /* @hack spelling: the ball's child in an arm of its own with the first
     * arm's body, where a fourth test in the first arm would do */
    if (SONIC_NODE(object, SONIC_AT_230, LESSER_AT_230)) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2cc);
    } else if (object == model_table[SONIC_BALL].object->child) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2cc);
    } else if (SONIC_NODE(object, SONIC_LEAN, LESSER_LEAN)) {
        work = player_work[sonic->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, sonic->lean);
        }
    } else if (SONIC_NODE(object, SONIC_ARM_0, LESSER_ARM_0)) {
        work = player_work[sonic->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, TASK_MOTION_OF(sonic->held)->hold_turn);
        }
    } else if (SONIC_NODE(object, SONIC_ARM_1, LESSER_ARM_1)) {
        work = player_work[sonic->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, -TASK_MOTION_OF(sonic->held)->hold_turn);
        }
    } else if (SONIC_NODE(object, SONIC_HEAD, LESSER_HEAD)) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_218);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2b4);
    } else if (SONIC_NODE(object, SONIC_MOUTH, LESSER_MOUTH)) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_224);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2c0);
    } else if (SONIC_NODE(object, SONIC_HAND_0, LESSER_HAND_0)) {
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_1e8);
        axis.z = 1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_26c);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_284);
    } else if (SONIC_NODE(object, SONIC_HAND_1, LESSER_HAND_1)) {
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_1f4);
        axis.z = -1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_278);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_290);
    } else if (SONIC_NODE(object, SONIC_FOOT_0, LESSER_FOOT_0)) {
        axis.x = -1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_200);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_29c);
    } else if (SONIC_NODE(object, SONIC_FOOT_1, LESSER_FOOT_1)) {
        axis.x = -1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_20c);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2a8);
    }
    if (SONIC_NODE(object, SONIC_WRIST, LESSER_SONIC_WRIST)) {
        njGetMatrix(&wrist_matrix);
    } else if (SONIC_NODE(object, SONIC_FLAME_RING_NODE,
                          LESSER_SONIC_FLAME_RING_NODE)) {
        njGetMatrix(&flame_ring_matrix);
    }
    if (SONIC_NODE(object, SONIC_SHOE_NODE_0, LESSER_SHOE_NODE_0)) {
        njGetMatrix(&shoe_matrix_0);
    } else if (SONIC_NODE(object, SONIC_SHOE_NODE_1, LESSER_SHOE_NODE_1)) {
        njGetMatrix(&shoe_matrix_1);
    } else if (object == model_table[SONIC_PSO_NODE].object ||
               object == model_table[SONIC_PSO_NODE_MIDDLE].object ||
               object == model_table[SONIC_PSO_NODE_FAR].object) {
        njGetMatrix(&costume_matrix);
    }
}

/* sonic_motion_callback for Shadow's model and Metal Sonic's. it keeps the
 * run matrices too */
static void shadow_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *sonic = drawn_record;
    struct task_work *work;
    NJS_VECTOR origin;
    NJS_VECTOR axis;

    origin.z = 0.0F;
    origin.y = 0.0F;
    origin.x = 0.0F;
    /* @hack spelling: the ball's child in an arm of its own with the first
     * arm's body, where a fourth test in the first arm would do */
    if (SHADOW_NODE(object, SHADOW_AT_230, LESSER_AT_230)) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2cc);
    } else if (model_table[SHADOW_BALL].object != NULL &&
               object == model_table[SHADOW_BALL].object->child) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2cc);
    } else if (SHADOW_NODE(object, SHADOW_LEAN, LESSER_LEAN)) {
        work = player_work[sonic->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, sonic->lean);
        }
    } else if (SHADOW_NODE(object, SHADOW_ARM_0, LESSER_ARM_0)) {
        work = player_work[sonic->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, TASK_MOTION_OF(sonic->held)->hold_turn);
        }
    } else if (SHADOW_NODE(object, SHADOW_ARM_1, LESSER_ARM_1)) {
        work = player_work[sonic->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, -TASK_MOTION_OF(sonic->held)->hold_turn);
        }
    } else if (SHADOW_NODE(object, SHADOW_HEAD, LESSER_HEAD)) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_218);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2b4);
    } else if (SHADOW_NODE(object, SHADOW_MOUTH, LESSER_MOUTH)) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_224);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2c0);
    } else if (SHADOW_NODE(object, SHADOW_HAND_0, LESSER_HAND_0)) {
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_1e8);
        axis.z = 1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_26c);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_284);
    } else if (SHADOW_NODE(object, SHADOW_HAND_1, LESSER_HAND_1)) {
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_1f4);
        axis.z = -1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_278);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_290);
    } else if (SHADOW_NODE(object, SHADOW_FOOT_0, LESSER_FOOT_0)) {
        axis.x = -1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_200);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_29c);
        njGetMatrix(&run_matrix_0);
    } else if (SHADOW_NODE(object, SHADOW_FOOT_1, LESSER_FOOT_1)) {
        axis.x = -1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &origin, &origin);
        njCalcPoint(&view_to_world, &origin, &sonic->unk_20c);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &sonic->unk_2a8);
        njGetMatrix(&run_matrix_1);
    } else if (SHADOW_NODE(object, SHADOW_RUN_NODE_2,
                           LESSER_SHADOW_RUN_NODE_2)) {
        njGetMatrix(&run_matrix_2);
    } else if (SHADOW_NODE(object, SHADOW_RUN_NODE_3,
                           LESSER_SHADOW_RUN_NODE_3)) {
        njGetMatrix(&run_matrix_3);
    }
    if (SHADOW_NODE(object, SHADOW_WRIST, LESSER_SHADOW_WRIST)) {
        njGetMatrix(&wrist_matrix);
    } else if (SHADOW_NODE(object, SHADOW_FLAME_RING_NODE,
                           LESSER_SHADOW_FLAME_RING_NODE)) {
        njGetMatrix(&flame_ring_matrix);
    }
    if (SHADOW_NODE(object, SHADOW_SHOE_NODE_0, LESSER_SHOE_NODE_0)) {
        njGetMatrix(&shoe_matrix_0);
    } else if (SHADOW_NODE(object, SHADOW_SHOE_NODE_1, LESSER_SHOE_NODE_1)) {
        njGetMatrix(&shoe_matrix_1);
    }
}

/* in a split screen, the other player's camera leaves out a player beyond its
 * view. it tests a sphere of VIEW_RADIUS about the player's pos against the
 * view */
#define VIEW_RADIUS 50.0F
/* the middle body takes the near body's place beyond DRAW_MIDDLE of the camera,
 * or in a two-player game. the far body takes it beyond DRAW_FAR */
#define DRAW_MIDDLE 100.0F
#define DRAW_FAR 150.0F
/* under PLAYER_WORK_BALL, ANIM_BALL takes the playing animation's place in
 * each step whose ball_steps has a bit of BALL_FLICKER. Metal Sonic shows no
 * ball */
#define BALL_FLICKER 0x11
/* a hanging player's body hangs HANG_DROP of its height below its shape's
 * center */
#define HANG_DROP 0.5F
/* a spin on the ground draws the ball SPIN_POSE_DROP down. it turns the ball
 * EIGHTH_TURN about z, through a point SPIN_POSE_PIVOT above it. the ball is
 * then scaled by SPIN_POSE_SCALE_X, SPIN_POSE_SCALE_Y and SPIN_POSE_SCALE_Z */
#define SPIN_POSE_DROP 1.0F
#define SPIN_POSE_PIVOT 5.0F
#define SPIN_POSE_SCALE_X 0.7F
#define SPIN_POSE_SCALE_Y 1.1F
#define SPIN_POSE_SCALE_Z 0.8F
/* the shadows at the hands and the feet, each shadow_model scaled across z or
 * x. Sonic's hand's shadow lies SONIC_HAND_SHADOW_DROP below the hand. the
 * ball's shadow lies BALL_SHADOW_RISE above the player's pos, scaled by
 * BALL_SHADOW_SIZE across x and z */
#define HAND_SHADOW_Z 0.7F
#define FOOT_SHADOW_X 2.6F
#define SONIC_HAND_SHADOW_DROP 1.0F
#define BALL_SHADOW_RISE 0.3F
#define BALL_SHADOW_SIZE 5.0F

/* the task's draw. the body blinks as a hit's timer counts down. while
 * game_state.solo_player isn't 0, only that player's task draws. it then
 * draws unblinking, turned only about y, with its near body */
static void sonic_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *sonic = task->extra.p;
    /* @hack spelling: record, a second pointer to the record taken through
     * its first field, where sonic alone would do */
    struct player *record = (struct player *)&sonic->slot;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
    NJS_CNK_OBJECT *draw;
    NJS_CNK_MODEL *model;
    struct collision_shape *shape;
    int current;
    Float dist;
    NJS_VECTOR offset;
    NJS_POINT3 pos;

    if (game_state.single_view == 0 && game_state.split_screen == 1 &&
        camera_current_index != record->slot) {
        if (sphere_out_of_view(&work->pos, VIEW_RADIUS)) {
            return;
        }
    }
    if (game_state.solo_player != 0 &&
        game_state.solo_player != record->slot + 1) {
        return;
    }
    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    if (light_selected[record->slot] != LIGHT_UNK_1 &&
        light_selected[record->slot] != LIGHT_VOLUME) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    drawn_record = record;
    njGetMatrix(&view_to_world);
    njInvertMatrix(&view_to_world);
    if (record->character == PLAYER_SONIC) {
        njCnkSetMotionCallback(sonic_motion_callback);
    } else {
        njCnkSetMotionCallback(shadow_motion_callback);
    }
    current = record->anim.current;
    /* @hack redundant: the test of the unsigned timer against 0, which
     * always holds */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state.solo_player != 0) {
        njSetTexture(sonic->by_character.sonic.texlist);
        njPushMatrixEx();
        if (work->mode != PLAYER_MODE_HANG) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            offset.x = offset.z = 0.0F;
            offset.y = -(record->consts.height * HANG_DROP);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &offset, &pos);
            njPopMatrixEx();
            pos.x += shape->center.x;
            pos.y += shape->center.y;
            pos.z += shape->center.z;
            njTranslateEx(&pos);
        }
        if (game_state.solo_player == record->slot + 1) {
            njRotateY(NULL, HALF_TURN - work->ang.y);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, HALF_TURN - work->ang.y);
            if (current == ANIM_SPIN && (work->flags & PLAYER_WORK_GROUNDED)) {
                offset.x = 0.0F;
                offset.y = -SPIN_POSE_DROP;
                offset.z = 0.0F;
                njTranslateEx(&offset);
                njTranslate(NULL, 0.0F, SPIN_POSE_PIVOT, 0.0F);
                njRotateZ(NULL, EIGHTH_TURN);
                njTranslate(NULL, 0.0F, -SPIN_POSE_PIVOT, 0.0F);
                offset.x = SPIN_POSE_SCALE_X;
                offset.y = SPIN_POSE_SCALE_Y;
                offset.z = SPIN_POSE_SCALE_Z;
                njScaleEx(&offset);
            }
        }
        njScaleEx(&work->scl);
#ifdef NONMATCHING
        /* GCC warns that model may be read unset. the second test of the
         * jiggle reads it only where the first set it */
        model = NULL;
#endif
        if (sonic->by_character.sonic.jiggle != NULL) {
            if (record->character == PLAYER_SONIC) {
                model = model_table[SONIC_JIGGLE].object->child->model;
                model_table[SONIC_JIGGLE].object->child->model =
                    sonic->by_character.sonic.jiggle->copy->model;
            } else {
                model = model_table[SHADOW_JIGGLE].object->child->model;
                model_table[SHADOW_JIGGLE].object->child->model =
                    sonic->by_character.sonic.jiggle->copy->model;
            }
        }
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions->model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else {
            object = model_table[record->anim.actions[current].model].object;
            if (game_state.demo == 0) {
                dist = njDistanceP2P(&work->pos,
                                     &camera_views[camera_current_index]->pos);
            } else {
                dist = 0.0F;
            }
            if (game_state.solo_player != record->slot + 1) {
                if (object == model_table[SONIC_BODY].object &&
                    model_table[SONIC_BODY_FAR].object != NULL &&
                    model_table[SONIC_BODY_MIDDLE].object != NULL) {
                    if (dist > DRAW_FAR) {
                        object = model_table[SONIC_BODY_FAR].object;
                    } else if (dist > DRAW_MIDDLE) {
                        object = model_table[SONIC_BODY_MIDDLE].object;
                    } else if (game_state.two_players != 0) {
                        object = model_table[SONIC_BODY_MIDDLE].object;
                    }
                } else if (object == model_table[SHADOW_BODY].object &&
                           model_table[SHADOW_BODY_FAR].object != NULL &&
                           model_table[SHADOW_BODY_MIDDLE].object != NULL) {
                    if (dist > DRAW_FAR) {
                        object = model_table[SHADOW_BODY_FAR].object;
                    } else if (dist > DRAW_MIDDLE) {
                        object = model_table[SHADOW_BODY_MIDDLE].object;
                    } else if (game_state.two_players != 0) {
                        object = model_table[SHADOW_BODY_MIDDLE].object;
                    }
                }
            }
            if (record->anim.mode != PLAYER_ANIM_BLEND) {
                draw = object;
                if ((work->flags & PLAYER_WORK_BALL) &&
                    record->kind != PLAYER_KIND_METALSONIC &&
                    (sonic->by_character.sonic.ball_steps & BALL_FLICKER)) {
                    current = ANIM_BALL;
                    draw = model_table[record->anim.actions[ANIM_BALL].model]
                               .object;
                }
                /* @bug logic: draws ANIM_BOARD_CRASH_POS with the motion
                 * ANIM_BOARD_CRASH_NEG's entry names, and the other way
                 * round, as it does the pair 15 entries on. each table gives
                 * the two of a pair each other's motions, and the draw looks
                 * the table up twice, MOV.W @R3,R1 at 0x8c06e978 and MOV.W
                 * @R5,R5 at 0x8c06e980. sonic_pick_mode looks a motion up
                 * once, MOV.W @R3,R3 at 0x8c069a90 */
                njCnkSimpleDrawMotion(draw, ANIM_MOTION(record, current),
                                      record->anim.frame);
            } else {
                njCnkSimpleDrawMotion(object, record->anim.blend,
                                      record->anim.frame);
            }
        }
        if (record->kind == PLAYER_SHADOW &&
            model_table[SHADOW_RUN_PART_1].object != NULL &&
            (current == ANIM_RUN || current == ANIM_SPRINT ||
             current == ANIM_FULL_SPEED)) {
            /* @hack redundant: the calls of njSin and njCos, whose results
             * nothing reads */
            njSin((game_state.steps & 0x1f) << 11);
            njCos((game_state.steps & 0x1f) << 11);
            draw_control_clear(DRAW_CONTROL_SHADOW);
            njPushMatrixEx();
            if (game_state.steps & 1) {
                part = model_table[SHADOW_RUN_PART_1].object;
            } else {
                part = model_table[SHADOW_RUN_PART_1 + 1].object;
            }
            njSetMatrix(NULL, &run_matrix_1);
            njCnkEasyDrawObject(part);
            if (game_state.steps & 1) {
                part = model_table[SHADOW_RUN_PART_0].object;
            } else {
                part = model_table[SHADOW_RUN_PART_0 + 1].object;
            }
            njSetMatrix(NULL, &run_matrix_0);
            njCnkEasyDrawObject(part);
            if (game_state.steps & 1) {
                part = model_table[SHADOW_RUN_PART_3].object;
            } else {
                part = model_table[SHADOW_RUN_PART_3 + 1].object;
            }
            njSetMatrix(NULL, &run_matrix_3);
            njCnkEasyDrawObject(part);
            if (game_state.steps & 1) {
                part = model_table[SHADOW_RUN_PART_2].object;
            } else {
                part = model_table[SHADOW_RUN_PART_2 + 1].object;
            }
            njSetMatrix(NULL, &run_matrix_2);
            njCnkEasyDrawObject(part);
            njPopMatrixEx();
            if (light_selected[record->slot] != LIGHT_UNK_1 &&
                light_selected[record->slot] != LIGHT_VOLUME) {
                draw_control_set(DRAW_CONTROL_SHADOW);
            }
        }
        if (current != ANIM_BALL) {
            njPushMatrixEx();
            if (record->upgrades & (UPGRADE_BIT(UPGRADE_SONIC_FLAMERING) |
                                    UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING))) {
                if (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_FLAMERING)) {
                    part = model_table[SONIC_FLAME_RING].object;
                } else {
                    part = model_table[SHADOW_FLAME_RING].object;
                }
                if (part != NULL) {
                    njSetMatrix(NULL, &flame_ring_matrix);
                    njCnkSimpleDrawModel(part->model);
                    draw_control_save();
                    draw_control_clear(NJD_CONTROL_3D_CONSTANT_MATERIAL);
                    /* @hack redundant: the product by 1 */
                    constant_material_set(
                        -(njCos(sonic->by_character.sonic.flame_ring_glow) *
                          1.0F),
                        0.0F, 0.0F, 0.0F);
                    njCnkSimpleDrawModel(part->child->model);
                    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
                    draw_control_restore();
                    njColorBlendingMode(NJD_SOURCE_COLOR,
                                        NJD_COLOR_BLENDING_SRCALPHA);
                    njColorBlendingMode(NJD_DESTINATION_COLOR,
                                        NJD_COLOR_BLENDING_INVSRCALPHA);
                }
            }
            if (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT)) {
                part = model_table[SONIC_RUBBER_UNIT].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &wrist_matrix);
                    njCnkSimpleDrawModel(part->model);
                    draw_control_save();
                    draw_control_clear(NJD_CONTROL_3D_CONSTANT_MATERIAL);
                    /* @hack redundant: the product by 1 */
                    constant_material_set(
                        -(njCos(sonic->by_character.sonic.bounce_glow) * 1.0F),
                        0.0F, 0.0F, 0.0F);
                    part = part->child;
                    njTranslateEx((NJS_VECTOR *)part->pos);
                    njRotateEx(part->ang, 0);
                    njCnkSimpleDrawModel(part->model);
                    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
                    draw_control_restore();
                }
            }
            if (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_MAGICWRIST)) {
                part = model_table[SONIC_MAGIC_WRIST].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &wrist_matrix);
                    njCnkSimpleDrawObject(part);
                }
            }
            if (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_SHOES)) {
                part = model_table[SONIC_SHOE_1].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &shoe_matrix_1);
                    njCnkSimpleDrawObject(part);
                    njSetMatrix(NULL, &shoe_matrix_0);
                    njCnkSimpleDrawObject(model_table[SONIC_SHOE_0].object);
                }
            }
            if (record->upgrades & UPGRADE_BIT(UPGRADE_SHADOW_SHOES)) {
                part = model_table[SHADOW_SHOE_1].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &shoe_matrix_1);
                    njCnkSimpleDrawObject(part);
                    njSetMatrix(NULL, &shoe_matrix_0);
                    njCnkSimpleDrawObject(model_table[SHADOW_SHOE_0].object);
                }
            }
            if (record->costume == COSTUME_PSO &&
                record->character == PLAYER_SONIC) {
                part = model_table[SONIC_PSO].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &costume_matrix);
                    njCnkSimpleDrawObject(part);
                }
            }
            njPopMatrixEx();
        }
        if (sonic->by_character.sonic.jiggle != NULL) {
            if (record->character == PLAYER_SONIC) {
                model_table[SONIC_JIGGLE].object->child->model = model;
            } else {
                model_table[SHADOW_JIGGLE].object->child->model = model;
            }
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    if (record->character == PLAYER_SONIC) {
        sonic_shadows(work, record, current);
    } else {
        shadow_shadows(work, record, current);
    }
    if (record->upgrades & (UPGRADE_BIT(UPGRADE_SONIC_FLAMERING) |
                            UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING))) {
        switch (work->mode) {
        case PLAYER_MODE_SONIC_COMBO_1:
        case PLAYER_MODE_SONIC_COMBO_2:
        case PLAYER_MODE_SONIC_COMBO_3:
        case PLAYER_MODE_SONIC_COMBO_RUNNING_1:
        case PLAYER_MODE_SONIC_COMBO_RUNNING_2:
        case PLAYER_MODE_SONIC_COMBO_RUNNING_3:
            flame_ring_draw(record->slot, &shoe_matrix_0);
            break;
        }
    }
    sonic_hold_place(work, record);
}

/* draw the modifier shadows of Sonic's model or Amy's. the near body casts its
 * own at the places sonic_motion_callback keeps. the ball casts one flat shadow
 * under the player */
static void sonic_shadows(struct task_work *work, struct player *record,
                          int current)
{
    int model = record->anim.actions[current].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == SONIC_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[SONIC_HEAD_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_1e8);
        njTranslate(NULL, 0.0F, -SONIC_HAND_SHADOW_DROP, 0.0F);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, 1.0F, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_1f4);
        njTranslate(NULL, 0.0F, -SONIC_HAND_SHADOW_DROP, 0.0F);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, 1.0F, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
    } else if (model == SONIC_BALL) {
        njTranslate(NULL, work->pos.x, work->pos.y + BALL_SHADOW_RISE,
                    work->pos.z);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, BALL_SHADOW_SIZE, 1.0F, BALL_SHADOW_SIZE);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* sonic_shadows for Shadow's model and Metal Sonic's */
static void shadow_shadows(struct task_work *work, struct player *record,
                           int current)
{
    int model = record->anim.actions[current].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == SHADOW_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[SHADOW_HEAD_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_1e8);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, 1.0F, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_1f4);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, 1.0F, 1.0F, HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
    } else if (model == SHADOW_BALL) {
        njTranslate(NULL, work->pos.x, work->pos.y + BALL_SHADOW_RISE,
                    work->pos.z);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, BALL_SHADOW_SIZE, 1.0F, BALL_SHADOW_SIZE);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* in PLAYER_MODE_FREE_MOVE the stick moves the player FREE_MOVE_STEP a step,
 * its way turned by the slot's camera. while X is held it moves the player up
 * or down instead. it does nothing within PAD_DEAD_ZONE of center on both
 * axes. B places the player anew, by stage_place_player */
static void sonic_free_move(struct task_work *work,
                            struct player_motion *motion, struct player *record,
                            struct player *sonic)
{
    Float x;
    Float y;
    Float step;
    Angle ang;

    if (player_pad_records[record->slot].press & PDD_DGT_TB) {
        stage_place_player(record->slot);
    }
    /* @hack type: the stick's axes made an int on their way to a Float,
     * where the Float alone would do */
    if (player_pad_records[record->slot].on & PDD_DGT_TX) {
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        if (x > PAD_DEAD_ZONE || x < -PAD_DEAD_ZONE || y > PAD_DEAD_ZONE ||
            y < -PAD_DEAD_ZONE) {
            if (y != 0.0F) {
                step = y / njSqrt(y * y);
            } else {
                step = 0.0F;
            }
            work->pos.y -= step * FREE_MOVE_STEP;
        }
    } else {
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        /* @hack spelling: the return and the else, where the move under the
         * negated test would do */
        if (x <= PAD_DEAD_ZONE && x >= -PAD_DEAD_ZONE && y <= PAD_DEAD_ZONE &&
            y >= -PAD_DEAD_ZONE) {
            return;
        } else {
            ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) -
                  camera_views[record->slot]->ang.y;
            work->pos.x += njCos(ang) * FREE_MOVE_STEP;
            work->pos.z += njSin(ang) * FREE_MOVE_STEP;
        }
    }
}

/* start the homing attack. on STAGE_CHAO, or with no near target nor rival,
 * it coasts from HOMING_SPEED, or HOMING_SPEED_SUPER with that upgrade.
 * otherwise it takes its first step at once. where that stops the steering,
 * the attack goes on at HOMING_SPEED, whatever the upgrade */
static void homing_start(struct task_work *work, struct player_motion *motion,
                         struct player *record, struct player *sonic)
{
    struct near_target *target;

    if (!(record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15))) {
        ball_large_create(record->slot);
    }
    sonic->by_character.sonic.homing = -1;
    sonic->by_character.sonic.steps = 0;
    sound_play(SOUND_HOMING, NULL, 0, 0);
    if (record->slot == 0) {
        target = near_targets;
    } else {
        target = near_targets_player1;
    }
    if (stage_current_id() == STAGE_CHAO ||
        (target->work == NULL && player_find_rival(record->slot) == -1)) {
        if (record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15)) {
            record->spd.x = HOMING_SPEED_SUPER;
        } else {
            record->spd.x = HOMING_SPEED;
        }
        sonic->by_character.sonic.homing = 1;
    } else {
        homing_step(work, motion, record, sonic);
        if (sonic->by_character.sonic.homing != 0) {
            record->spd.x = HOMING_SPEED;
        }
    }
}

/* a step of the homing attack. it coasts while homing is above 0, on
 * STAGE_CHAO, and with no near target nor rival. otherwise it steers at the
 * nearest target in reach, or stops steering where there is none. a near
 * target needs a collision record with a shape. the rival's work must have
 * one, unchecked */
static void homing_step(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic)
{
    struct near_target *entry;
    struct task_work *target;
    struct collision_work *collision;
    struct collision_shape *shape;
    NJS_VECTOR way;
    Float nearest;
    Float speed;
    Angle ang;
    int stage;
    Bool golem;
    int other;

    stage = stage_current_id();
    golem = stage == STAGE_BOSS_GOLEM;
    if (record->slot == 0) {
        entry = near_targets;
    } else {
        entry = near_targets_player1;
    }
    if (sonic->by_character.sonic.homing > 0 ||
        (entry->work == NULL && player_find_rival(record->slot) == -1) ||
        stage_current_id() == STAGE_CHAO) {
        sonic->by_character.sonic.homing++;
        record->spd.x *= HOMING_COAST;
    move:
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((other = player_find_rival(record->slot)) != -1) {
        target = player_work[other];
        way.x = work->pos.x - target->pos.x;
        way.y = work->pos.y - target->pos.y;
        way.z = work->pos.z - target->pos.z;
        nearest = njScalor2(&way);
        ang = (Angle)(atan2f(-way.z, -way.x) * ANGLE_PER_RADIAN);
        if (world_gravity.y > 0.0F) {
            ang = HALF_TURN - ang;
        }
        if (angle_distance(work->ang.y, ang) > HOMING_CONE ||
            !(nearest < HOMING_RANGE)) {
            target = NULL;
            nearest = HOMING_RANGE;
        }
    } else {
        target = NULL;
        nearest = HOMING_RANGE;
    }
    for (; entry->work != NULL; entry++) {
        if (entry->work->id == FALL_KIND_NORMAL) {
            continue;
        }
        collision = entry->work->collision;
        if (collision == NULL) {
            continue;
        }
        shape = collision->shapes;
        if (shape == NULL) {
            continue;
        }
        way = shape->center;
        if (!(shape->flags & SHAPE_IN_WORLD)) {
            njAddVector(&way, &entry->work->pos);
        }
        njSubVector(&way, &work->pos);
        ang = (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN);
        if (world_gravity.y > 0.0F) {
            ang = HALF_TURN - ang;
        }
        if (!(record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15)) &&
            angle_distance(work->ang.y, ang) > HOMING_CONE && !golem) {
            continue;
        }
        if (nearest > entry->distance) {
            nearest = entry->distance;
            target = entry->work;
        }
    }
    if (target == NULL) {
        sonic->by_character.sonic.homing = 1;
        /* @hack spelling: the goto into the first arm's tail, where a copy
         * of its two calls would do */
        goto move;
    }
    collision = target->collision;
    shape = collision->shapes;
    way = shape->center;
    if (!(shape->flags & SHAPE_IN_WORLD)) {
        njAddVector(&way, &target->pos);
    }
    njSubVector(&way, &work->pos);
    ang = (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN) & 0xffff;
    if (world_gravity.y > 0.0F) {
        ang = HALF_TURN - ang;
    }
    if (sonic->by_character.sonic.steps > HOMING_LOCK_STEPS || golem) {
        work->ang.y = ang;
    } else if (record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15)) {
        work->ang.y = angle_approach(work->ang.y, ang, HOMING_TURN_SUPER);
    } else {
        work->ang.y = angle_approach(work->ang.y, ang, HOMING_TURN);
    }
    if (way.y * world_gravity.y < 0.0F &&
        (!golem || way.y >= HOMING_GOLEM_RISE) &&
        sonic->by_character.sonic.homing < 0) {
        way.y = 0.0F;
        sonic->by_character.sonic.homing = 1;
    }
    njUnitVector(&way);
    ang = work->ang.y;
    if (world_gravity.y > 0.0F) {
        ang = HALF_TURN - ang;
    }
    way.x = njCos(ang) * njSqrt(1.0F - (way.y * way.y));
    way.z = njSin(ang) * njSqrt(1.0F - (way.y * way.y));
    if (record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15)) {
        speed = HOMING_SPEED_SUPER;
    } else {
        speed = HOMING_SPEED;
    }
    if (sonic->by_character.sonic.steps > HOMING_TIRE_STEPS) {
        /* @hack spelling: speed read again in its own product, where *=
         * would do */
        speed = speed *
                (HOMING_TIRED + (rand() * (HOMING_TIRED_SPREAD * RAND_SCALE)));
    }
    way.x *= speed;
    way.y *= speed;
    way.z *= speed;
    motion->spd = way;
    turn_from_world(work, &way);
    record->spd = way;
    if (sonic->by_character.sonic.homing < 0) {
        sonic->by_character.sonic.homing = 0;
    }
}

/* Sonic's and Shadow's upgrades of one kind, which the task tests together */
#define UPGRADE_SHOES                                                          \
    (UPGRADE_BIT(UPGRADE_SONIC_SHOES) | UPGRADE_BIT(UPGRADE_SHADOW_SHOES))
#define UPGRADE_SPARKLE                                                        \
    (UPGRADE_BIT(UPGRADE_SONIC_SPARKLE) | UPGRADE_BIT(UPGRADE_SHADOW_SPARKLE))

/* a speed-up task's extra */
struct speed_up {
    Uint8 slot; /* the slot of the player it speeds up */
    Uint8 unk_01;
    Uint16 steps; /* the steps the task has run */
};

/* a wind cutter task's extra. slots[WIND_CUTTER_TARGET] is the slot of the
 * player it hurts, and slots[WIND_CUTTER_OWNER] that of the player it is
 * from */
struct wind_cutter {
    /* @hack type: the two slots as an array, where two Sint8 fields would
     * do */
    Sint8 slots[2];
    Uint16 steps; /* the steps the task has run */
};
#define WIND_CUTTER_TARGET 0
#define WIND_CUTTER_OWNER 1

/* a time stop task's extra */
struct time_stop {
    Sint8 owner; /* the slot of the player who stopped time */
    Uint8 stopped; /* the slot of the player whose time stops */
    Uint16 steps; /* the steps the stop has counted, from 1 */
};

/* the extra of the task that holds a light attack's charge */
struct light_charge {
    struct task_work *work;
    struct player *record;
    Float top_speed; /* the record's own, which the charge puts back */
};

static void light_dash_start(struct task_work *work, struct player *record,
                             struct player *sonic);
static void light_attack_start(struct task_work *work, struct player *record,
                               struct player *sonic);
static void magic_hand_start(struct task_work *work, struct player *record,
                             struct player *sonic);
static void hold_handle_start(struct task_work *work, struct player *record,
                              struct player *sonic);
static void versus_speed_up_start(struct task_work *work, struct player *record,
                                  struct player *sonic);
static void versus_wind_cutter_start(struct task_work *work,
                                     struct player_motion *motion,
                                     struct player *record,
                                     struct player *sonic);
static void versus_time_stop_start(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record, struct player *sonic);
static void light_charge_start(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record);

/* the nearest of the player's near rings within range of its body's center, or
 * null. the ring must lie within LIGHT_DASH_CONE of the player's heading. a
 * ring lies at its pos plus its first shape's center. one without a collision
 * record or a shape is passed over. the player must have a collision record
 * with a shape, unchecked */
static struct near_target *light_dash_ring(struct task_work *work,
                                           struct player *record, Float range)
{
    struct near_target *near;
    struct near_target *found;
    struct task_work *ring;
    struct collision_work *collision;
    struct collision_shape *shape;
    NJS_VECTOR front;
    NJS_VECTOR to;
    Float nearest;
    Angle angle;

    if (record->slot == 0) {
        near = near_rings;
    } else {
        near = near_rings_player1;
    }
    if (near->work == NULL) {
        return NULL;
    }
    front.x = 1.0F;
    front.y = 0.0F;
    front.z = 0.0F;
    turn_to_world(work, &front);
    njUnitVector(&front);
    found = NULL;
    for (; near->work != NULL; near++) {
        ring = near->work;
        collision = ring->collision;
        if (collision == NULL) {
            continue;
        }
        /* @hack spelling: the shapes read through ring again, where
         * collision->shapes would do */
        shape = ring->collision->shapes;
        if (shape == NULL) {
            continue;
        }
        to = ring->pos;
        njAddVector(&to, &shape->center);
        njSubVector(&to, &work->collision->shapes->center);
        if (njScalor(&to) > range) {
            continue;
        }
        njUnitVector(&to);
        angle = vector_angle(&front, &to, NULL);
        if (angle > LIGHT_DASH_CONE) {
            continue;
        }
        if (found == NULL || nearest > near->distance) {
            found = near;
            nearest = near->distance;
        }
    }
    return found;
}

void sonic_light_dash(struct task_work *work, struct player_motion *motion,
                      struct player *record, struct player *sonic)
{
    struct near_target *near;
    struct task_work *ring;
    struct collision_shape *shape;
    NJS_VECTOR to;
    Float speed;
    Angle heading;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((near = light_dash_ring(work, record, LIGHT_DASH_RANGE)) == NULL) {
        if (sonic->by_character.sonic.light_dash_rings <=
                LIGHT_DASH_SHORT_RINGS &&
            light_dash_ring(work, record, LIGHT_DASH_FAR_RANGE) == NULL) {
            if (njScalor(&record->spd) > light_dash_exit_speed) {
                njUnitVector(&record->spd);
                record->spd.x *= light_dash_exit_speed;
                record->spd.y *= light_dash_exit_speed;
                record->spd.z *= light_dash_exit_speed;
            }
            sonic->by_character.sonic.light_dash_steps = 0;
        }
        player_accel_roll(work, motion, record);
        player_apply_accel(work, motion, record);
    } else {
        ring = near->work;
        shape = ring->collision->shapes;
        speed = near->distance;
        if (sonic->by_character.sonic.light_dash_steps <
            LIGHT_DASH_RING_STEPS) {
            sonic->by_character.sonic.light_dash_steps = LIGHT_DASH_RING_STEPS;
        }
        to = ring->pos;
        njAddVector(&to, &shape->center);
        njSubVector(&to, &work->collision->shapes->center);
        heading = (Angle)(atan2f(to.z, to.x) * ANGLE_PER_RADIAN);
        /* under gravity that points up, headings run the other way */
        if (world_gravity.y > 0.0F) {
            heading = HALF_TURN - heading;
        }
        work->ang.y = angle_approach(work->ang.y, heading, LIGHT_DASH_TURN);
        motion->aim_y = work->ang.y;
        speed = njSqrt(speed);
        if (speed > LIGHT_DASH_STEP_MAX) {
            speed = LIGHT_DASH_STEP_MAX;
        } else if (speed < LIGHT_DASH_STEP_MIN) {
            speed = LIGHT_DASH_STEP_MIN;
        }
        if (njScalor(&to) <= LIGHT_DASH_NEAR) {
            to = motion->spd;
        }
        njUnitVector(&to);
        motion->spd.x = to.x * speed;
        motion->spd.y = to.y * speed;
        motion->spd.z = to.z * speed;
        turn_from_world(work, &to);
        record->spd = to;
    }
}

struct task_work *sonic_light_attack_seek(struct task_work *work,
                                          struct player *record,
                                          struct player *sonic)
{
    struct near_target *near;
    struct task_work *found;
    struct collision_work *collision;
    struct collision_shape *shape;
    NJS_VECTOR to;
    Float nearest;
    int rival;

    if (record->slot == 0) {
        near = near_targets;
    } else {
        near = near_targets_player1;
    }
    if (work->mode == PLAYER_MODE_SONIC_LIGHT_ATTACK) {
        sonic->by_character.sonic.steps = 0;
    }
    if (near->work == NULL || stage_current_id() == STAGE_CHAO) {
        return NULL;
    }
    rival = player_find_rival(record->slot);
    if (rival != -1) {
        found = player_work[rival];
        to.x = work->pos.x - found->pos.x;
        to.y = work->pos.y - found->pos.y;
        to.z = work->pos.z - found->pos.z;
        nearest = njScalor2(&to);
    } else {
        found = NULL;
        nearest = LIGHT_ATTACK_RANGE;
    }
    for (; near->work != NULL; near++) {
        collision = near->work->collision;
        if (collision == NULL) {
            continue;
        }
        /* @hack spelling: shape, a local set only to be tested, where a
         * test of collision->shapes would do */
        shape = collision->shapes;
        if (shape == NULL) {
            continue;
        }
        if (collision->kind != COLLISION_ITEM &&
            collision->kind != COLLISION_ENEMY) {
            continue;
        }
        if (nearest > near->distance) {
            nearest = near->distance;
            found = near->work;
        }
    }
    return found;
}

Bool sonic_light_attack(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic)
{
    struct task_work *target;
    struct collision_shape *shape;
    NJS_VECTOR to;
    NJS_VECTOR way;
    Float strength;
    Float pull;
    Float inertia;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((target = sonic_light_attack_seek(work, record, sonic)) == NULL) {
        record->spd.x *= LIGHT_ATTACK_SLOW;
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        return FALSE;
    }
    /* @hack spelling: an else after the arm that returns, where an if of its
     * own would do */
    else if (sonic->by_character.sonic.ball_steps <
             LIGHT_ATTACK_STRAIGHT_STEPS) {
        to = record->spd;
        turn_to_world(work, &to);
        motion->spd = to;
    } else {
        shape = target->collision->shapes;
        to = shape->center;
        if (!(shape->flags & SHAPE_IN_WORLD)) {
            njAddVector(&to, &target->pos);
        }
        njSubVector(&to, &work->pos);
        if (njScalor(&to) == 0.0F) {
            to.x = -world_gravity.x;
            to.y = -world_gravity.y;
            to.z = -world_gravity.z;
        } else {
            njUnitVector(&to);
        }
        way = motion->spd;
        if (njScalor(&way) == 0.0F) {
            way = to;
        } else {
            njUnitVector(&way);
        }
        pull = LIGHT_ATTACK_PULL +
               ((Uint8)sonic->by_character.sonic.ball_steps * (1.0F / 255.0F));
        inertia = LIGHT_ATTACK_WEIGHT - pull;
        if (player_read_stick(work, record, NULL, &strength)) {
            inertia -= strength * LIGHT_ATTACK_STICK_PULL;
            pull += strength * LIGHT_ATTACK_STICK_PULL;
        }
        to.x = (way.x * inertia) + (to.x * pull);
        to.y = (way.y * inertia) + (to.y * pull);
        to.z = (way.z * inertia) + (to.z * pull);
        if (njScalor(&to) == 0.0F) {
            to = way;
        } else {
            njUnitVector(&to);
        }
        to.x *= LIGHT_ATTACK_SPEED;
        to.y *= LIGHT_ATTACK_SPEED;
        to.z *= LIGHT_ATTACK_SPEED;
        motion->spd = to;
        turn_from_world(work, &to);
        record->spd = to;
    }
    return TRUE;
}

Bool sonic_test_jump(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *sonic)
{
    struct task_force *force;
    /* @hack type: one local for the ramp's force and then its work, where
     * a struct task_force local for the force would do */
    struct task_work *ramp;
    NJS_VECTOR way;
    NJS_VECTOR throw;
    NJS_VECTOR axis;
    Angle ang_x;
    Angle ang_y;
    Angle ang_z;
    int flags;

    if (player_jump_pressed(work, record)) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            flags = player_action_flags(work, record);
            /* @hack spelling: an else after the arm that returns, where the
             * call after the if would do */
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_JUMP_HOLDING;
                record->spd.y = record->consts.jump_speed;
                record->anim.next = ANIM_HOLD_JUMP;
                work->flags &= ~PLAYER_WORK_ON_SHAPE;
                record->jump_steps = 0;
                if (record->character == PLAYER_SONIC) {
                    sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
                } else {
                    sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
                }
                if (stage_current_id() != STAGE_GREEN_HILL) {
                    if (record->kind == PLAYER_SONIC) {
                        sound_play_at(CRY_JUMP_SONIC, NULL, 0, SOUND_LOUDEST,
                                      &work->pos);
                    } else if (record->kind == PLAYER_SHADOW) {
                        sound_play_at(CRY_JUMP_SHADOW, NULL, 0, SOUND_LOUDEST,
                                      &work->pos);
                    } else if (record->kind == PLAYER_KIND_AMY) {
                        voice_play_priority_2(VOICE_TRICK_AMY);
                    }
                }
                return TRUE;
            } else {
                player_release_held(work, motion, record);
            }
        } else if (!(work->flags & PLAYER_WORK_BALL) &&
                   record->spd.x > record->consts.sprint_speed &&
                   record->ground_task != NULL &&
                   record->ground_task->work->id == WORK_ID_RAMP) {
            ramp = record->ground_task->force.p;
            /* the throw's player_set_acc below replaces this acc before
             * anything reads it */
            if (ramp != NULL) {
                ramp = (struct task_work *)((struct task_force *)ramp +
                                            record->slot);
                player_set_acc(record->slot,
                               ((struct task_force *)ramp)->speed.x,
                               ((struct task_force *)ramp)->speed.y,
                               ((struct task_force *)ramp)->speed.z);
            }
            work->mode = PLAYER_MODE_SONIC_RAMP_JUMP;
            ramp = record->ground_task->work;
            way.x = 0.0F;
            way.y = RAMP_THROW_UP;
            if (record->spd.x < record->consts.full_speed) {
                way.z = RAMP_THROW_AHEAD;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_A_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_A_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else if (record->spd.x <
                       record->consts.full_speed + RAMP_THROW_AHEAD) {
                /* @bug logic: the throw ahead drops at both ends of this arm.
                 * it grows from 0 to RAMP_THROW_AHEAD as spd.x passes
                 * full_speed, FSUB at 0x8c07016a, where the arms on either
                 * side fall from RAMP_THROW_AHEAD to 0 */
                way.z = record->spd.x - record->consts.full_speed;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_B_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_B_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else {
                way.z = 0.0F;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_C_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_C_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            }
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, ramp->ang.z);
            njRotateY(NULL, ramp->ang.y);
            njRotateX(NULL, ramp->ang.x);
            njCalcVector(NULL, &way, &throw);
            way.x = way.y = 0.0F;
            way.z = 1.0F;
            njCalcVector(NULL, &way, &axis);
            ang_y = (Angle)(atan2f(axis.x, axis.z) * ANGLE_PER_RADIAN);
            way.x = way.z = 0.0F;
            way.y = 1.0F;
            njCalcVector(NULL, &way, &axis);
            ang_x = (Angle)(asinf(axis.z) * ANGLE_PER_RADIAN);
            ang_z = -(Angle)(asinf(axis.x) * ANGLE_PER_RADIAN);
            player_set_angles(record->slot, ang_x, ang_y, ang_z);
            njPopMatrixEx();
            player_set_acc(record->slot, throw.x, throw.y, throw.z);
            record->anim.next = ANIM_RAMP_JUMP;
            record->anim.after = ANIM_FALL;
            if (placement_get_state(record->ground_task)) {
                sonic->by_character.sonic.trick_grade =
                    SONIC_TRICK_GRADE_PASSED;
            } else {
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_C;
            }
            work->flags &= ~PLAYER_WORK_ON_SHAPE;
            work->flags &=
                ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            if (record->character == PLAYER_SONIC) {
                sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
            } else {
                sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
            }
            record->jump_steps = 0;
            return TRUE;
        }
        work->mode = PLAYER_MODE_JUMP;
        record->spd.y = record->consts.jump_speed;
        /* a jump leaves the force of the Egg Golem's platforms and sensors
         * behind */
        if (record->ground_task != NULL &&
            (record->ground_task->work->id < WORK_ID_GOLEM_PLATFORM_A ||
             record->ground_task->work->id > WORK_ID_GOLEM_SENSOR)) {
            force = record->ground_task->force.p;
            if (force != NULL) {
                player_set_acc(record->slot, force->speed.x, force->speed.y,
                               force->speed.z);
            }
        }
        record->anim.next = ANIM_JUMP;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ATTACK;
        sonic->by_character.sonic.homing_hits = 0;
        work->flags &= ~PLAYER_WORK_ON_SHAPE;
        record->jump_steps = 0;
        if (record->character == PLAYER_SONIC) {
            sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
        } else {
            sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
        }
        return TRUE;
    }
    return FALSE;
}

Bool sonic_test_request(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic)
{
    struct path_sample sample;
    NJS_VECTOR axis;
    NJS_VECTOR ahead;

    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_LAUNCH_SPEED:
        if (work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
            work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
            work->mode = PLAYER_MODE_RUN;
            player_run_anim(work, motion, record);
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        } else if (work->mode == PLAYER_MODE_SONIC_RAIL) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_FREE_FALL:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FREE_FALL;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_FLING:
        work->mode = PLAYER_MODE_FLING;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_UNK_23;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_RUN_PATH:
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_BOOST_PATH:
        work->mode = PLAYER_MODE_PATH_ROLL;
        record->anim.next = ANIM_ROLL;
        record->path_distance = 0.0F;
        work->flags |=
            PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        break;
    case PLAYER_REQUEST_ANIM:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        if (record->request_arg == ANIM_UNK_86 &&
            motion_table[ANIM_UNK_86].motion == NULL) {
            record->request_arg = ANIM_STAND;
        }
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_HANG:
        work->mode = PLAYER_MODE_HANG;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_SPIN:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = ANIM_SPIN;
        work->ang.x = work->ang.z = 0;
        player_stop(motion, record);
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        record->anim_speed = SPIN_PACE;
        sound_play(SOUND_SPIN, NULL, 0, 0);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RELEASE:
        if (record->request_arg) {
            work->mode = PLAYER_MODE_STAND;
            record->anim.next = record->request_arg;
            record->mode_steps = 0;
        } else {
            player_stand(work, motion, record);
        }
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_LAUNCH_BALL:
        work->mode = PLAYER_MODE_ROLL;
        record->anim.next = ANIM_ROLL;
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        sound_play(SOUND_ROLL, NULL, 0, 0);
        break;
    case PLAYER_REQUEST_JOIN_PATH:
        if (work->mode == PLAYER_MODE_RUN || work->mode == PLAYER_MODE_ROLL ||
            record->spd.x > record->consts.running_speed) {
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_CAPTURE:
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        sonic->by_character.sonic.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_CAPTURE_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_CAPTURE_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_CAPTURE_AMY);
        }
        break;
    case PLAYER_REQUEST_UNK_23:
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        sonic->by_character.sonic.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_UNK_23_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_UNK_23_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_CAPTURE_AMY);
        }
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CRASH_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CRASH_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        sonic->by_character.sonic.light_dash_steps = 0;
        crash_stars_burst(record->slot);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_BOARD:
        work->mode = PLAYER_MODE_SONIC_BOARD_FALL;
        record->anim.next = ANIM_BOARD_FALL;
        /* @hack redundant: a test of PLAYER_WORK_BALL before its clear */
        if (work->flags & PLAYER_WORK_BALL) {
            work->flags &= ~PLAYER_WORK_BALL;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        record->drive = 0.0F;
        return TRUE;
    case PLAYER_REQUEST_RAIL_PATH:
        sample.distance = record->path_distance;
        if (!path_get_sample(record->path, &sample)) {
            return FALSE;
        }
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        sonic->by_character.sonic.steps = 0;
        sonic->by_character.sonic.ball_steps = 0;
        record->drive = 0.0F;
        /* the speed the player lands with joins its speed along the rail, by
         * how steeply the rail runs the way the player faces */
        axis.x = 1.0F;
        axis.y = 0.0F;
        axis.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, sample.slope_z);
        njRotateX(NULL, sample.slope_x);
        njRotateY(NULL, -work->ang.y);
        njCalcVector(NULL, &axis, &axis);
        if (record->spd.x >= 0.0F) {
            record->spd.x +=
                _builtin_fabsf(record->spd.y) * _builtin_fabsf(axis.y);
        } else {
            record->spd.x -=
                _builtin_fabsf(record->spd.y) * _builtin_fabsf(axis.y);
        }
        work->mode = PLAYER_MODE_SONIC_RAIL;
        if (rand() * RAND_SCALE > 0.5F) {
            record->anim.next = ANIM_RAIL_A_MINUS;
        } else {
            record->anim.next = ANIM_RAIL_B_MINUS;
        }
        if (stage_current_id() == STAGE_GREEN_FOREST) {
            sound_play_at(SOUND_RAIL_LAND_FOREST, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (stage_current_id() == STAGE_WHITE_JUNGLE) {
            sound_play_at(SOUND_RAIL_LAND_JUNGLE, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else {
            sound_play_at(SOUND_RAIL_LAND, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        vibration_start(record->slot, LANDING_VIBRATION_POWER,
                        LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
        njPopMatrixEx();
        record->spd.y = 0.0F;
        return TRUE;
    case PLAYER_REQUEST_RAIL_PATH_BENEATH:
        /* @hack spelling: the case's body inside the test of the motion,
         * with a return of its own, where a return before it would do */
        if (motion_table[ANIM_RAIL_BENEATH].motion != NULL) {
            sample.distance = record->path_distance;
            if (!path_get_sample(record->path, &sample)) {
                return FALSE;
            }
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            work->flags |= PLAYER_WORK_ON_PATH;
            sonic->by_character.sonic.steps = 0;
            sonic->by_character.sonic.ball_steps = 0;
            record->drive = 0.0F;
            /* the player hangs upside down from the rail: its up is the
             * rail's down, and it keeps facing the way it faced */
            ahead.x = 1.0F;
            ahead.y = 0.0F;
            ahead.z = 0.0F;
            turn_to_world(work, &ahead);
            axis.x = 0.0F;
            axis.y = 1.0F;
            axis.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateZ(NULL, sample.slope_z);
            njRotateX(NULL, sample.slope_x);
            njCalcVector(NULL, &axis, &axis);
            njPopMatrixEx();
            work->ang.x = (Angle)(asinf(-axis.z) * ANGLE_PER_RADIAN);
            work->ang.z = -(Angle)(atan2f(-axis.x, -axis.y) * ANGLE_PER_RADIAN);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateX(NULL, -work->ang.x);
            njRotateZ(NULL, -work->ang.z);
            njCalcVector(NULL, &ahead, &axis);
            njPopMatrixEx();
            motion->aim_y = work->ang.y =
                (Angle)(atan2f(axis.z, axis.x) * ANGLE_PER_RADIAN);
            work->mode = PLAYER_MODE_SONIC_RAIL_BENEATH;
            record->anim.next = ANIM_RAIL_BENEATH;
            record->spd.y = 0.0F;
            return TRUE;
        }
        return TRUE;

    case PLAYER_REQUEST_CURRENT_PATH:
        work->mode = PLAYER_MODE_SWIM;
        record->anim.next = ANIM_IN_CURRENT;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_SCARE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_BREATHE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = ANIM_BREATHE;
        record->underwater_steps = 0;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        /* @bug memory: writes the handed task's work with no test for null,
         * at 0x8c070c20, and sonic_test_request_holding does too, at
         * 0x8c071190. sonic_throw and sonic_throw_at_mark test a held task's
         * work for null, at 0x8c0733f6 and 0x8c07371e */
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_DROWN:
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_SINK:
        work->mode = PLAYER_MODE_UNK_38;
        record->anim.next = ANIM_WALK;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        sonic->by_character.sonic.steps = 0;
        break;
    case PLAYER_REQUEST_RESET:
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_MISS:
        work->mode = PLAYER_MODE_MISS;
        /* @hack redundant: an empty arm, where a test of the other arm's
         * condition would do */
        if (record->anim.current == ANIM_IN_CURRENT) {
        } else {
            record->anim.next = ANIM_MISS;
        }
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_MISS_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_MISS_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_MISS_AMY);
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

Bool sonic_test_request_holding(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *sonic)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE_HOLDING;
        record->anim.next = ANIM_HOLD_JUMP;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH_HOLDING;
        record->anim.next = ANIM_HOLD_JUMP;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL_HOLDING;
        record->anim.next = ANIM_HOLD_FALL;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RUN_PATH:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_BOOST_PATH:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PATH_ROLL;
        record->anim.next = ANIM_ROLL;
        record->path_distance = 0.0F;
        work->flags |=
            PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        break;
    case PLAYER_REQUEST_ANIM:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PLAY_ANIM;
        if (record->request_arg == ANIM_UNK_86 &&
            motion_table[ANIM_UNK_86].motion == NULL) {
            record->request_arg = ANIM_STAND;
        }
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_HANG:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_HANG;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_SPIN:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = ANIM_SPIN;
        work->ang.x = work->ang.z = 0;
        player_stop(motion, record);
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        record->anim_speed = SPIN_PACE;
        sound_play(SOUND_SPIN, NULL, 0, 0);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RELEASE:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_LAUNCH_BALL:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_ROLL;
        record->anim.next = ANIM_ROLL;
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        sound_play(SOUND_ROLL, NULL, 0, 0);
        break;
    case PLAYER_REQUEST_JOIN_PATH:
        if (work->mode == PLAYER_MODE_UNK_19 ||
            record->spd.x > record->consts.running_speed) {
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_CAPTURE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        sonic->by_character.sonic.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_CAPTURE_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_CAPTURE_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_CAPTURE_AMY);
        }
        break;
    case PLAYER_REQUEST_UNK_23:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        sonic->by_character.sonic.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_UNK_23_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_UNK_23_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_CAPTURE_AMY);
        }
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CRASH_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CRASH_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        vibration_start(record->slot, CRASH_VIBRATION_POWER,
                        CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
        sonic->by_character.sonic.light_dash_steps = 0;
        crash_stars_burst(record->slot);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_CURRENT_PATH:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_SWIM;
        record->anim.next = ANIM_IN_CURRENT;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_SCARE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_DROWN:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_SINK:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_UNK_38;
        record->anim.next = ANIM_WALK;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        sonic->by_character.sonic.steps = 0;
        break;
    case PLAYER_REQUEST_RESET:
        player_release_held(work, motion, record);
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_MISS:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_MISS;
        /* @hack redundant: an empty arm, where a test of the other arm's
         * condition would do */
        if (record->anim.current == ANIM_IN_CURRENT) {
        } else {
            record->anim.next = ANIM_MISS;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_MISS_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_MISS_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_MISS_AMY);
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

int sonic_test_action(struct task_work *work, struct player_motion *motion,
                      struct player *record, struct player *sonic)
{
    Sint8 *actions = record->actions;
    int count;
    int action;
    int index;

    count = record->action_count;
    if (!pad_action_pressed[record->slot]) {
        /* sonic_offer_light_attack offers the light attack while the action
         * button is held: it starts once the button is up */
        if (!(player_pad_records[record->slot].on & pad_action_buttons) &&
            count > 0 && record->actions[0] == PLAYER_MODE_SONIC_LIGHT_ATTACK) {
            light_attack_start(work, record, sonic);
            return SONIC_ACTION_TAKEN;
        }
        return SONIC_ACTION_NONE;
    }
    if (count == 0) {
        return SONIC_ACTION_NONE;
    }
    action = record->action;
    /* @hack spelling: the list walked by a pointer, where actions[index]
     * would do */
    for (index = 0; index < count; index++) {
        if (action == *actions) {
            break;
        }
        actions++;
    }
    if (count == index) {
        action = record->actions[0];
    }
    record->action_taken = action;
    switch (action) {
    case PLAYER_MODE_SONIC_LIGHT_DASH:
        light_dash_start(work, record, sonic);
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_GRAB_PUSHABLE:
        player_hold(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_UNK_17:
        player_grab_shakable(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_PICK_UP:
        player_pick_up(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_FALL:
        player_fall(work, record);
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_THROW:
        player_throw_held(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_AIR_THROW:
        player_air_throw_held(work, record);
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_THROW_AT_TOUCH:
        player_throw_at_touch(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_PET_CHAO:
        player_pet_chao(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_UNK_14:
        player_set_down_held(work, motion, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_LET_GO:
        player_release_and_stand(work, motion, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_UNK_2A:
        player_call(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_PULL_HEAVY:
        player_pull_heavy(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_ASK_HINT:
        player_ask_hint(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_CANT_PLAY_FLUTE:
        player_cant_play_flute(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_HOLD_HANDLE:
        hold_handle_start(work, record, sonic);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_PLAY_FLUTE:
        player_play_flute(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_PLAY_FLUTE_AGAIN:
        player_play_flute_again(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_PULL_BURIED:
        player_pull_buried(work, record);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case PLAYER_MODE_SONIC_MAGIC_HAND:
        magic_hand_start(work, record, sonic);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_TAKEN;
    case SONIC_ACTION_SPEED_UP:
        versus_speed_up_start(work, record, sonic);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_VERSUS;
    case SONIC_ACTION_WIND_CUTTER:
        versus_wind_cutter_start(work, motion, record, sonic);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_VERSUS;
    case SONIC_ACTION_TIME_STOP:
        versus_time_stop_start(work, motion, record, sonic);
        sonic->by_character.sonic.hold_steps = 0;
        return SONIC_ACTION_VERSUS;
    }
    return SONIC_ACTION_NONE;
}

/* curl up in PLAYER_MODE_SONIC_SPIN. the charge starts at the player's speed,
 * trot_speed at least */
static void spin_start(struct task_work *work, struct player *record,
                       struct player *sonic)
{
    work->mode = PLAYER_MODE_SONIC_SPIN;
    record->anim.next = ANIM_SPIN;
    work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
    sonic->by_character.sonic.ball_steps = 0;
    if (record->spd.x > record->consts.trot_speed) {
        record->anim_speed = record->spd.x;
    } else {
        record->anim_speed = record->consts.trot_speed;
    }
    sound_play_for(SOUND_SPIN, work, SOUND_PRIORITY, 0, SOUND_DURATION);
    sonic->by_character.sonic.steps = 0;
}

static void combo_kick(struct player *record)
{
    sound_play(SOUND_COMBO, NULL, 0, 0);
    vibration_start(record->slot, 4, 20, VIBRATION_UNTIMED);
}

/* start the combo's first move. its next mode is PLAYER_MODE_SONIC_SPIN until
 * the action button decides */
static void combo_start(struct task_work *work, struct player *record,
                        struct player *sonic)
{
    work->mode = PLAYER_MODE_SONIC_COMBO_1;
    record->anim.next = ANIM_COMBO_FIRST;
    work->flags |= PLAYER_WORK_ATTACK;
    sonic->by_character.sonic.steps = 0;
    sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_SPIN;
    record->spd.x = COMBO_SPEED;
}

static void combo_running_start(struct task_work *work, struct player *record,
                                struct player *sonic)
{
    work->mode = PLAYER_MODE_SONIC_COMBO_RUNNING_1;
    record->anim.next = ANIM_COMBO_RUNNING;
    work->flags |= PLAYER_WORK_ATTACK;
    sonic->by_character.sonic.steps = 0;
    sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_SPIN;
    record->spd.x = COMBO_SPEED;
    combo_kick(record);
}

/* end a combo's move. it runs on where player_stick_ahead grades the stick
 * PLAYER_WALL_ASKEW or nearer, or else waits in PLAYER_MODE_SONIC_COMBO_WAIT.
 * spd.x is held to running_speed */
static void combo_end(struct task_work *work, struct player *record,
                      struct player *sonic)
{
    struct player_motion *motion;

    if (record->spd.x > record->consts.running_speed) {
        record->spd.x = record->consts.running_speed;
    }
    if (player_stick_ahead(work, record) >= PLAYER_WALL_ASKEW) {
        /* @hack spelling: the motion in a local set before the mode, where
         * the call's argument would do */
        motion = player_motion[record->slot];
        work->mode = PLAYER_MODE_RUN;
        player_run_anim(work, motion, record);
    } else {
        work->mode = PLAYER_MODE_SONIC_COMBO_WAIT;
        record->anim.next = ANIM_COMBO_WAIT;
        record->anim.after = ANIM_STAND;
    }
    work->flags &= ~PLAYER_WORK_ATTACK;
    sonic->by_character.sonic.steps = 0;
}

/* combo_end, but a wait leaves the animation as it is */
static void combo_end_last(struct task_work *work, struct player *record,
                           struct player *sonic)
{
    struct player_motion *motion;

    if (record->spd.x > record->consts.running_speed) {
        record->spd.x = record->consts.running_speed;
    }
    if (player_stick_ahead(work, record) >= PLAYER_WALL_ASKEW) {
        /* @hack spelling: the motion in a local set before the mode, where
         * the call's argument would do */
        motion = player_motion[record->slot];
        work->mode = PLAYER_MODE_RUN;
        player_run_anim(work, motion, record);
    } else {
        work->mode = PLAYER_MODE_SONIC_COMBO_WAIT;
    }
    work->flags &= ~PLAYER_WORK_ATTACK;
    sonic->by_character.sonic.steps = 0;
}

/* jump out of a move where the stick is pushed, or else step back from it in
 * PLAYER_MODE_SONIC_COMBO_BACK */
static void combo_jump(struct task_work *work, struct player_motion *motion,
                       struct player *record, struct player *sonic)
{
    if (player_read_stick(work, record, NULL, NULL) &&
        sonic_test_jump(work, motion, record, sonic)) {
        record->anim.next = ANIM_COMBO_JUMP;
    } else {
        record->spd.x = COMBO_BACK_SPEED;
        record->spd.y = 0.0F;
        record->spd.z = 0.0F;
        work->mode = PLAYER_MODE_SONIC_COMBO_BACK;
        record->anim.next = ANIM_COMBO_BACK;
        record->anim.after = ANIM_STAND;
        work->flags &= ~PLAYER_WORK_ATTACK;
        sonic->by_character.sonic.steps = 0;
    }
}

/* whether the ground probe found a ceiling no more than COMBO_ROOM above the
 * player's pos, other than water */
#define COMBO_UNDER_CEILING(work, record)                                      \
    (!((record)->ground.above_y > (work)->pos.y + COMBO_ROOM) &&               \
     !((record)->ground.above_flags & WORLD_FLAT))

Bool sonic_combo(struct task_work *work, struct player_motion *motion,
                 struct player *record, struct player *sonic)
{
    /* @hack type: collision volatile, where a plain local would do */
    struct collision_work *volatile collision;
    struct collision_work *hit;
    /* @hack type: stopped an int set to 1 or 0 and tested against 1, where a
     * Bool tested alone would do */
    int stopped;
    /* @hack redundant: a local set to the mode, which nothing reads */
    int mode; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    /* @hack spelling: two tests that each return, where one test with ||
     * would do */
    if (work->flags & PLAYER_WORK_HOLDING) {
        sonic->by_character.sonic.hold_steps = 0;
        return FALSE;
    }
    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        sonic->by_character.sonic.hold_steps = 0;
        return FALSE;
    }
    /* a move's spd.x stops where the player's collision last hit a
     * WORK_ID_STOP_LSD task */
    /* @hack spelling: the assignments inside the test, where statements of
     * their own would do */
    if ((hit = (collision = work->collision)->hit) != NULL &&
        hit->owner->work->id == WORK_ID_STOP_LSD) {
        stopped = 1;
    } else {
        stopped = 0;
    }
    switch (mode = work->mode) {
    case PLAYER_MODE_SONIC_COMBO_1:
        sonic->by_character.sonic.steps++;
        if (stopped == 1) {
            record->spd.x = 0.0F;
        }
        if (pad_jump_pressed[record->slot]) {
            combo_jump(work, motion, record, sonic);
            return TRUE;
        }
        if (pad_action_pressed[record->slot]) {
            sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_COMBO_2;
        } else if (!pad_action_held[record->slot] &&
                   sonic->by_character.sonic.combo_next !=
                       PLAYER_MODE_SONIC_COMBO_2) {
            sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_COMBO_WAIT;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            if (COMBO_UNDER_CEILING(work, record)) {
                record->anim.next = ANIM_COMBO_FIRST;
                sonic->by_character.sonic.steps = 0;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                record->spd.x = COMBO_SPEED;
                combo_kick(record);
                return FALSE;
            }
            work->mode = sonic->by_character.sonic.combo_next;
            if (work->mode == PLAYER_MODE_SONIC_SPIN) {
                spin_start(work, record, sonic);
            } else if (work->mode == PLAYER_MODE_SONIC_COMBO_WAIT) {
                combo_end(work, record, sonic);
            } else {
                record->spd.x = COMBO_2_SPEED;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                sonic->by_character.sonic.steps = 0;
                combo_kick(record);
            }
            return TRUE;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_RUNNING_1:
        sonic->by_character.sonic.steps++;
        if (stopped == 1) {
            record->spd.x = 0.0F;
        }
        if (pad_jump_pressed[record->slot]) {
            combo_jump(work, motion, record, sonic);
            return TRUE;
        }
        if (pad_action_pressed[record->slot]) {
            sonic->by_character.sonic.combo_next =
                PLAYER_MODE_SONIC_COMBO_RUNNING_2;
        } else if (!pad_action_held[record->slot] &&
                   sonic->by_character.sonic.combo_next !=
                       PLAYER_MODE_SONIC_COMBO_RUNNING_2) {
            sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_COMBO_WAIT;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            if (sonic->by_character.sonic.steps <= COMBO_RUNNING_REPEAT_STEPS ||
                COMBO_UNDER_CEILING(work, record)) {
                record->anim.next = ANIM_COMBO_RUNNING;
                sonic->by_character.sonic.steps = COMBO_RUNNING_REPEAT_FROM;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                record->spd.x = COMBO_SPEED;
                combo_kick(record);
                return FALSE;
            }
            work->mode = sonic->by_character.sonic.combo_next;
            if (work->mode == PLAYER_MODE_SONIC_SPIN) {
                spin_start(work, record, sonic);
            } else if (work->mode == PLAYER_MODE_SONIC_COMBO_WAIT) {
                combo_end(work, record, sonic);
            } else {
                record->spd.x = COMBO_RUNNING_2_SPEED;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                sonic->by_character.sonic.steps = 0;
                combo_kick(record);
            }
            return TRUE;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_RUNNING_2:
        sonic->by_character.sonic.steps++;
        if (stopped == 1) {
            record->spd.x = 0.0F;
        }
        if (pad_jump_pressed[record->slot]) {
            combo_jump(work, motion, record, sonic);
            return TRUE;
        }
        if (pad_action_pressed[record->slot]) {
            sonic->by_character.sonic.combo_next =
                PLAYER_MODE_SONIC_COMBO_RUNNING_3;
        } else if (!pad_action_held[record->slot] &&
                   sonic->by_character.sonic.combo_next !=
                       PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
            sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_COMBO_WAIT;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            if (COMBO_UNDER_CEILING(work, record)) {
                record->anim.next = ANIM_COMBO_FIRST;
                sonic->by_character.sonic.steps = 0;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                record->spd.x = COMBO_SPEED;
                combo_kick(record);
                return FALSE;
            }
            work->mode = sonic->by_character.sonic.combo_next;
            if (work->mode == PLAYER_MODE_SONIC_COMBO_RUNNING_3) {
                record->spd.x = COMBO_RUNNING_3_SPEED;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                sonic->by_character.sonic.steps = 0;
                sound_play(SOUND_COMBO_LAST, NULL, 0, 0);
                if (stage_current_id() != STAGE_GREEN_HILL) {
                    if (record->kind == PLAYER_SONIC) {
                        sound_play_at(CRY_COMBO_LAST_SONIC, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    } else if (record->kind == PLAYER_SHADOW) {
                        sound_play_at(CRY_COMBO_LAST_SHADOW, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                }
                vibration_start(record->slot, COMBO_VIBRATION_POWER,
                                COMBO_VIBRATION_FREQ, VIBRATION_UNTIMED);
            } else {
                combo_end(work, record, sonic);
            }
            return TRUE;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_2:
        sonic->by_character.sonic.steps++;
        if (stopped == 1) {
            record->spd.x = 0.0F;
        }
        if (pad_jump_pressed[record->slot]) {
            combo_jump(work, motion, record, sonic);
            return TRUE;
        }
        if (pad_action_pressed[record->slot]) {
            sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_COMBO_3;
        } else if (!pad_action_held[record->slot] &&
                   sonic->by_character.sonic.combo_next !=
                       PLAYER_MODE_SONIC_COMBO_3) {
            sonic->by_character.sonic.combo_next = PLAYER_MODE_SONIC_COMBO_WAIT;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            if (COMBO_UNDER_CEILING(work, record)) {
                record->anim.next = ANIM_COMBO_FIRST;
                sonic->by_character.sonic.steps = 0;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                record->spd.x = COMBO_SPEED;
                combo_kick(record);
                return FALSE;
            }
            work->mode = sonic->by_character.sonic.combo_next;
            if (work->mode == PLAYER_MODE_SONIC_COMBO_3) {
                record->spd.x = COMBO_3_SPEED;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                sonic->by_character.sonic.steps = 0;
                sound_play(SOUND_COMBO_LAST, NULL, 0, 0);
                if (stage_current_id() != STAGE_GREEN_HILL) {
                    if (record->kind == PLAYER_SONIC) {
                        sound_play_at(CRY_COMBO_LAST_SONIC, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    } else if (record->kind == PLAYER_SHADOW) {
                        sound_play_at(CRY_COMBO_LAST_SHADOW, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                }
                vibration_start(record->slot, COMBO_VIBRATION_POWER,
                                COMBO_VIBRATION_FREQ, VIBRATION_UNTIMED);
            } else {
                combo_end(work, record, sonic);
            }
            return TRUE;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_3:
        sonic->by_character.sonic.steps++;
        if (pad_jump_pressed[record->slot]) {
            combo_jump(work, motion, record, sonic);
            return TRUE;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            if (COMBO_UNDER_CEILING(work, record)) {
                record->anim.next = ANIM_COMBO_FIRST;
                sonic->by_character.sonic.steps = 0;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                record->spd.x = COMBO_SPEED;
                combo_kick(record);
                return FALSE;
            }
            combo_end_last(work, record, sonic);
            return TRUE;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_RUNNING_3:
        sonic->by_character.sonic.steps++;
        if (stopped == 1) {
            record->spd.x = 0.0F;
        }
        if (pad_jump_pressed[record->slot]) {
            combo_jump(work, motion, record, sonic);
            return TRUE;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            if (record->anim.next != ANIM_COMBO_RUNNING_LAST) {
                return FALSE;
            }
            if (COMBO_UNDER_CEILING(work, record)) {
                record->anim.next = ANIM_COMBO_FIRST;
                sonic->by_character.sonic.steps = 0;
                sonic->by_character.sonic.combo_next =
                    PLAYER_MODE_SONIC_COMBO_WAIT;
                record->spd.x = COMBO_SPEED;
                combo_kick(record);
                return FALSE;
            }
            combo_end_last(work, record, sonic);
            return TRUE;
        }
        break;
    case PLAYER_MODE_SONIC_COMBO_WAIT:
    default:
        if (pad_action_pressed[record->slot]) {
            sonic->by_character.sonic.hold_steps = 1;
            break;
        }
        if (pad_action_held[record->slot] &&
            sonic->by_character.sonic.hold_steps >= 1) {
            sonic->by_character.sonic.hold_steps++;
            /* @hack spelling: an else after the arm that returns, where the
             * statement after the if would do */
            if (sonic->by_character.sonic.hold_steps > COMBO_HOLD_STEPS) {
                spin_start(work, record, sonic);
                sonic->by_character.sonic.hold_steps = 0;
                return TRUE;
            } else {
                work->flags |= PLAYER_WORK_ATTACK;
            }
            break;
        }
        if ((!pad_action_held[record->slot] &&
             sonic->by_character.sonic.hold_steps > COMBO_HOLD_MIN) ||
            (!(record->ground.above_y > work->pos.y + COMBO_ROOM) &&
             (work->flags & PLAYER_WORK_GROUNDED) &&
             record->ball->up.y > COMBO_LEVEL &&
             !(record->ground.above_flags & WORLD_FLAT))) {
            if (record->spd.x <= record->consts.running_speed) {
                combo_start(work, record, sonic);
            } else {
                combo_running_start(work, record, sonic);
            }
            if (stage_current_id() != STAGE_GREEN_HILL) {
                if (record->kind == PLAYER_SONIC) {
                    sound_play_at(CRY_JUMP_SONIC, NULL, 0, SOUND_LOUDEST,
                                  &work->pos);
                } else if (record->kind == PLAYER_SHADOW) {
                    sound_play_at(CRY_JUMP_SHADOW, NULL, 0, SOUND_LOUDEST,
                                  &work->pos);
                }
            }
            vibration_start(record->slot, COMBO_VIBRATION_POWER,
                            COMBO_VIBRATION_FREQ, VIBRATION_UNTIMED);
            sonic->by_character.sonic.hold_steps = 0;
            return TRUE;
        }
        sonic->by_character.sonic.hold_steps = 0;
        if (work->mode == PLAYER_MODE_SONIC_COMBO_WAIT) {
            work->flags &= ~PLAYER_WORK_ATTACK;
        }
        break;
    }
    return FALSE;
}

Bool sonic_offer_light_dash(struct task_work *work, struct player *record)
{
    if (!(record->upgrades & UPGRADE_SHOES)) {
        return FALSE;
    }
    if (work->flags & PLAYER_WORK_HOLDING) {
        return FALSE;
    }
    if (work->mode >= PLAYER_MODE_SONIC_LIGHT_DASH) {
        return FALSE;
    }
    if (light_dash_ring(work, record, LIGHT_DASH_RANGE) != NULL) {
        OFFER(record, PLAYER_MODE_SONIC_LIGHT_DASH);
        return TRUE;
    }
    return FALSE;
}

static void light_dash_start(struct task_work *work, struct player *record,
                             struct player *sonic)
{
    work->mode = PLAYER_MODE_SONIC_LIGHT_DASH;
    record->anim.next = ANIM_LIGHT_DASH;
    sonic->by_character.sonic.light_dash_steps = LIGHT_DASH_STEPS;
    sonic->by_character.sonic.steps = 0;
    sonic->by_character.sonic.light_dash_rings = 0;
    record->spd.x = LIGHT_DASH_SPEED;
    work->flags &= ~PLAYER_WORK_BALL;
    work->flags |= PLAYER_WORK_ATTACK;
    sound_play(SOUND_LIGHT_DASH, NULL, 0, 0);
}

static void light_attack_start(struct task_work *work, struct player *record,
                               struct player *sonic)
{
    work->flags &= ~PLAYER_WORK_CHARGED;
    work->mode = PLAYER_MODE_SONIC_LIGHT_ATTACK;
    record->anim.next = ANIM_ROLL;
    record->spd.y = LIGHT_ATTACK_RISE;
    work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
    sound_play(SOUND_LIGHT_DASH, NULL, 0, 0);
    if (stage_current_id() != STAGE_GREEN_HILL) {
        if (record->kind == PLAYER_SONIC) {
            sound_play_at(CRY_LIGHT_ATTACK_SONIC, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_SHADOW) {
            sound_play_at(CRY_LIGHT_ATTACK_SHADOW, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        }
    }
    sonic->by_character.sonic.ball_steps = 0;
    work->collision->shapes->levels =
        (work->collision->shapes->levels & ~SHAPE_ATTACK) |
        SHAPE_ATTACK_LEVEL(LIGHT_ATTACK_LEVEL);
    work->collision->shapes->levels =
        (work->collision->shapes->levels & ~SHAPE_GUARD) |
        SHAPE_GUARD_LEVEL(LIGHT_ATTACK_LEVEL);
}

Bool sonic_offer_light_attack(struct task_work *work, struct player *record,
                              struct player *sonic)
{
    if (work->flags & PLAYER_WORK_CHARGED) {
        if (!pad_action_held[record->slot]) {
            work->flags &= ~PLAYER_WORK_CHARGED;
        }
        if (sonic_light_attack_seek(work, record, sonic) != NULL) {
            OFFER(record, PLAYER_MODE_SONIC_LIGHT_ATTACK);
            return TRUE;
        }
    }
    return FALSE;
}

/* the task of the nearest enemy among the player's near targets within
 * MAGIC_HAND_RANGE, or null. the enemy's first shape must be on, and its task
 * must have a motion. there is none on STAGE_CHAO */
static struct task *magic_hand_target(struct task_work *work,
                                      struct player *record)
{
    struct near_target *near;
    struct collision_work *collision;
    struct collision_shape *shape;
    struct task *found;
    Float nearest;
    /* @hack redundant: a vector nothing uses, which sizes the frame */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */

    if (record->slot == 0) {
        near = near_targets;
    } else {
        near = near_targets_player1;
    }
    if (near->work == NULL || stage_current_id() == STAGE_CHAO) {
        return NULL;
    }
    nearest = MAGIC_HAND_RANGE;
    found = NULL;
    for (; near->work != NULL; near++) {
        collision = near->work->collision;
        if (collision == NULL) {
            continue;
        }
        shape = collision->shapes;
        if (shape == NULL) {
            continue;
        }
        if (shape->flags & SHAPE_OFF) {
            continue;
        }
        if (collision->kind == COLLISION_ENEMY &&
            collision->owner->motion.p != NULL && near->distance < nearest) {
            nearest = near->distance;
            found = collision->owner;
        }
    }
    return found;
}

Bool sonic_offer_magic_hand(struct task_work *work, struct player *record)
{
    struct task *target;

    if (work->mode != PLAYER_MODE_STAND && work->mode != PLAYER_MODE_RUN &&
        work->mode != PLAYER_MODE_FALL) {
        return FALSE;
    }
    if (!(record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_MAGICWRIST))) {
        return FALSE;
    }
    target = magic_hand_target(work, record);
    if (target == NULL) {
        return FALSE;
    }
    if (record->action_count == 0) {
        OFFER(record, ACTION_SKIPPED);
    }
    OFFER(record, PLAYER_MODE_SONIC_MAGIC_HAND);
    return TRUE;
}

/* take magic_hand_target's enemy, where there is one, in
 * PLAYER_MODE_SONIC_MAGIC_HAND. the enemy takes WORK_HURT, and
 * ENEMY_IN_MAGIC_HAND with the player's slot in shot_by */
static void magic_hand_start(struct task_work *work, struct player *record,
                             struct player *sonic)
{
    struct task *target;
    struct task_work *target_work;
    struct enemy_work *enemy;

    target = magic_hand_target(work, record);
    if (target != NULL) {
        work->mode = PLAYER_MODE_SONIC_MAGIC_HAND;
        record->anim.next = ANIM_HOLD_STAND;
        target_work = target->work;
        enemy = target->motion.p;
        target_work->flags |= WORK_HURT;
        enemy->flags |= ENEMY_IN_MAGIC_HAND;
        enemy->shot_by = record->slot;
        sonic->by_character.sonic.steps = 0;
    }
}

Bool sonic_offer_hold_handle(struct task_work *work, struct player *record)
{
    if (work->flags & PLAYER_WORK_GROUNDED) {
        if ((work->mode == PLAYER_MODE_STAND ||
             work->mode == PLAYER_MODE_RUN) &&
            record->action == PLAYER_MODE_HOLD_HANDLE) {
            OFFER(record, PLAYER_MODE_HOLD_HANDLE);
            return TRUE;
        }
    }
    return FALSE;
}

static void hold_handle_start(struct task_work *work, struct player *record,
                              struct player *sonic)
{
    work->mode = PLAYER_MODE_HOLD_HANDLE;
    record->anim.next = ANIM_HOLD_HANDLE;
    sonic->by_character.sonic.steps = 0;
}

Bool sonic_offer_versus_actions(struct task_work *work, struct player *record)
{
    if (game_state.two_players == 0 &&
        stage_current_id() != STAGE_VERSUS_SONIC_2) {
        return FALSE;
    }
    if (game_state.ring_level_timer[record->slot] > 0) {
        return FALSE;
    }
    if (game_state.cleared != 0) {
        return FALSE;
    }
    if (work->mode != PLAYER_MODE_STAND && work->mode != PLAYER_MODE_RUN) {
        return FALSE;
    }
    if (game_state.ring_levels[record->slot][RING_LEVEL_TIME_STOP] == 1) {
        OFFER(record, SONIC_ACTION_TIME_STOP);
        return TRUE;
    }
    if (game_state.ring_levels[record->slot][RING_LEVEL_WIND_CUTTER] == 1) {
        OFFER(record, SONIC_ACTION_WIND_CUTTER);
        return TRUE;
    }
    if (game_state.ring_levels[record->slot][RING_LEVEL_SPEED_UP] == 1) {
        OFFER(record, SONIC_ACTION_SPEED_UP);
        return TRUE;
    }
    return FALSE;
}

static void versus_speed_up_step(struct task *task)
{
    /* @hack spelling: the extra cast at each use, where a local for it
     * would do */
    struct player *record =
        player_records[((struct speed_up *)task->extra.p)->slot];

    if (record == NULL ||
        ((struct speed_up *)task->extra.p)->steps++ >= SPEED_UP_STEPS) {
        task_kill(task);
    }
}

static void versus_speed_up_destroy(struct task *task)
{
    struct player *record =
        player_records[((struct speed_up *)task->extra.p)->slot];

    if (record != NULL) {
        record->consts.ground_accel *= 1.0F / SPEED_UP_ACCEL;
        record->consts.top_speed *= 1.0F / SPEED_UP_FACTOR;
        record->consts.still_grip *= 1.0F / SPEED_UP_FACTOR;
    }
}

/* the versus speed-up, whose task holds the record's raised parameters while
 * it runs. where task_create fails, the start spends the ring level all the
 * same, with no speed-up */
static void versus_speed_up_start(struct task_work *work, struct player *record,
                                  struct player *sonic)
{
    struct task *task;

    task = task_create(TASK_EXTRA, SPEED_UP_LEVEL, versus_speed_up_step,
                       "Sonic2PSpeedUpMan");
    if (task != NULL) {
        ((struct speed_up *)task->extra.p)->slot = record->slot;
        record->consts.ground_accel *= SPEED_UP_ACCEL;
        record->consts.top_speed *= SPEED_UP_FACTOR;
        record->consts.still_grip *= SPEED_UP_FACTOR;
        if (work->mode == PLAYER_MODE_RUN) {
            record->spd.x *= SPEED_UP_RUN;
        }
        task->destructor = versus_speed_up_destroy;
    }
    player_ectoplasm_start(record->slot, SPEED_UP_EFFECT_STEPS);
    game_state.ring_levels[record->slot][RING_LEVEL_SPEED_UP] = 0;
    if (record->kind == PLAYER_SONIC) {
        voice_play_priority_2(VOICE_SPEED_UP_SONIC);
    } else if (record->kind == PLAYER_KIND_AMY) {
        voice_play_priority_2(VOICE_SPEED_UP_AMY);
    } else if (record->kind == PLAYER_SHADOW) {
        voice_play_priority_2(VOICE_SPEED_UP_SHADOW);
    }
    vibration_start_fading(record->slot, 2, 59, 32);
}

/* the wind cutter's step. its hurt reaches the target only in a two-player
 * game, or below the target's sprint_speed. the owner's slot must hold a
 * record, unchecked */
static void versus_wind_cutter_step(struct task *task)
{
    /* @hack spelling: the extra cast at each use, where a local for it
     * would do */
    int target =
        ((struct wind_cutter *)task->extra.p)->slots[WIND_CUTTER_TARGET];
    int owner = ((struct wind_cutter *)task->extra.p)->slots[WIND_CUTTER_OWNER];

    if (player_work[target] == NULL ||
        ((struct wind_cutter *)task->extra.p)->steps++ >= WIND_CUTTER_STEPS) {
        task_kill(task);
        return;
    }
    if (((struct wind_cutter *)task->extra.p)->steps == WIND_CUTTER_SEND) {
        /* @hack spelling: two switches on the owner's character, where one
         * would do */
        switch (player_records[owner]->character) {
        case PLAYER_SONIC:
            wind_cutters_start(owner, target, WIND_CUTTER_BLADES);
            break;
        case PLAYER_SHADOW:
            chaos_spears_start(owner, target, CHAOS_SPEARS);
            break;
        }
        switch (player_records[owner]->character) {
        case PLAYER_SONIC:
            sound_play_for(SOUND_WIND_CUTTER, task, SOUND_PRIORITY, 0,
                           WIND_CUTTER_STEPS);
            break;
        case PLAYER_SHADOW:
            sound_play_for(SOUND_CHAOS_SPEARS, task, SOUND_PRIORITY, 0,
                           WIND_CUTTER_STEPS);
            break;
        }
    } else if (((struct wind_cutter *)task->extra.p)->steps ==
               WIND_CUTTER_HURT) {
        /* @hack spelling: two arms that both hurt, where one test with ||
         * would do */
        if (game_state.two_players) {
            player_work[target]->flags |= WORK_HURT;
        } else if (player_records[target]->spd.x <
                   player_records[target]->consts.sprint_speed) {
            player_work[target]->flags |= WORK_HURT;
        }
    }
}

static void versus_wind_cutter_destroy(struct task *task)
{
}

/* the versus wind cutter, at the player in the other of slots 0 and 1. where
 * task_create fails, the start spends the ring level all the same, with no
 * wind cutter */
static void versus_wind_cutter_start(struct task_work *work,
                                     struct player_motion *motion,
                                     struct player *record,
                                     struct player *sonic)
{
    struct task *task;
    int other;

    if (record->slot == 0) {
        other = 1;
    } else {
        other = 0;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, WIND_CUTTER_LEVEL,
                            versus_wind_cutter_step, "Sonic2PWindCutterMan")) !=
        NULL) {
        ((struct wind_cutter *)task->extra.p)->slots[WIND_CUTTER_TARGET] =
            other;
        ((struct wind_cutter *)task->extra.p)->slots[WIND_CUTTER_OWNER] =
            record->slot;
        task->destructor = versus_wind_cutter_destroy;
    }
    game_state.ring_levels[record->slot][RING_LEVEL_WIND_CUTTER] = 0;
    if (record->character == PLAYER_SONIC) {
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_WIND_CUTTER_SONIC);
        } else if (record->kind == PLAYER_KIND_AMY) {
            if (game_state.voice_language == VOICE_JAPANESE) {
                voice_play_priority_2(VOICE_WIND_CUTTER_AMY_JAPANESE);
            } else {
                voice_play_priority_2(VOICE_WIND_CUTTER_AMY);
            }
        }
        player_pose(record->slot, ANIM_SONIC_POSE);
    } else if (record->character == PLAYER_SHADOW) {
        if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_WIND_CUTTER_SHADOW);
        }
        player_pose(record->slot, ANIM_SHADOW_POSE);
    }
}

static void versus_time_stop_step(struct task *task)
{
    /* @hack spelling: the extra cast at each use, where a local for it
     * would do */
    int stopped = ((struct time_stop *)task->extra.p)->stopped;

    if (pad_press_repeat[stopped] & PLAYER_STRUGGLE_BUTTONS) {
        ((struct time_stop *)task->extra.p)->steps += TIME_STOP_PRESS;
    }
    if (player_work[stopped] == NULL ||
        ((struct time_stop *)task->extra.p)->steps++ >= TIME_STOP_STEPS ||
        game_state.cleared == 1) {
        task_kill(task);
    }
}

/* the seconds a time stop has left, in one digit at twice its size.
 * versus_time_stop_draw sets its value and its x */
#define STOP_SECONDS_MAX 9
/* clang-format off */
static struct hud_number time_stop_seconds = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS, 0, 0,
    STOP_SECONDS_MAX, 0, {64.0F, 224.0F, 0.0F}, 2.0F, 0xff8080ff,
};
/* clang-format on */
/* the screen, or the stopped player's half of it, which versus_time_stop_draw
 * inverts. the draw sets each corner's x */
static NJS_POLYGON_VTX time_stop_screen[4] = {
#include "player/sonic/time_stop_screen.inc"
};

/* the time stop's draw: the seconds left, over the stopped player's half of the
 * screen. it inverts that half at half brightness, or the whole screen outside
 * a two-player game */
static void versus_time_stop_draw(struct task *task)
{
    /* @hack spelling: the extra cast at each use, where a local for it
     * would do */
    int stopped = ((struct time_stop *)task->extra.p)->stopped;

    if (game_state.solo_player != 0) {
        return;
    }
    time_stop_seconds.value =
        TIME_STOP_SECONDS -
        (Uint32)(((struct time_stop *)task->extra.p)->steps / STEPS_PER_SECOND);
    if (stopped == 0) {
        time_stop_seconds.pos.x = STOP_SECONDS_X_0;
    } else {
        time_stop_seconds.pos.x = STOP_SECONDS_X_1;
    }
    hud_number_draw(&time_stop_seconds);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_INVDESTCOLOR);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ZERO);
    if (game_state.two_players != 0) {
        if (stopped == 0) {
            time_stop_screen[0].x = 0.0F;
            time_stop_screen[1].x = 0.0F;
            time_stop_screen[2].x = SPLIT_LEFT_END;
            time_stop_screen[3].x = SPLIT_LEFT_END;
        } else {
            time_stop_screen[0].x = SPLIT_RIGHT_START;
            time_stop_screen[1].x = SPLIT_RIGHT_START;
            time_stop_screen[2].x = SCREEN_WIDTH;
            time_stop_screen[3].x = SCREEN_WIDTH;
        }
    } else {
        time_stop_screen[0].x = 0.0F;
        time_stop_screen[1].x = 0.0F;
        time_stop_screen[2].x = SCREEN_WIDTH;
        time_stop_screen[3].x = SCREEN_WIDTH;
    }
    njDrawPolygon(time_stop_screen, 4, TRUE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* let the stopped player's time run again. the stopped slot must hold a
 * record, unchecked */
static void versus_time_stop_destroy(struct task *task)
{
    /* @hack spelling: the task through a local, where the call's argument
     * would do */
    void *key = task;
    /* @hack spelling: the extra through a local, where a cast in stopped's
     * would do */
    struct time_stop *stop = task->extra.p;
    int stopped = stop->stopped;

    game_state.time_stopped &= ~TIME_STOPPED_BIT(stopped);
    /* @bug logic: leaves a Sonic's stop's sound playing out TIME_STOP_STEPS
     * where it stops a Shadow and ends early, by the Shadow's presses or a
     * clear. the test reads the stopped player's character, MOV.B at
     * 0x8c072b68, where versus_time_stop_start keys the sound to the task by
     * the stopping player's, at 0x8c072c38 */
    if (player_records[stopped]->character == PLAYER_SONIC) {
        sound_stop_key(key);
    }
}

/* the versus time stop. game_state.time_stopped takes the bit of the other of
 * slots 0 and 1. the stop's task clears it as it ends. where task_create fails,
 * the bit stays set */
static void versus_time_stop_start(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record, struct player *sonic)
{
    struct task *task;
    int other;

    if (record->slot == 0) {
        other = 1;
    } else {
        other = 0;
    }
    game_state.time_stopped |= TIME_STOPPED_BIT(other);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, TIME_STOP_LEVEL, versus_time_stop_step,
                            "Sonic2PTimeStopMan")) != NULL) {
        ((struct time_stop *)task->extra.p)->owner = record->slot;
        ((struct time_stop *)task->extra.p)->stopped = other;
        ((struct time_stop *)task->extra.p)->steps = 1;
        task->draw = versus_time_stop_draw;
        task->destructor = versus_time_stop_destroy;
    }
    chaos_control_create(record->slot);
    switch (record->character) {
    case PLAYER_SONIC:
        sound_play_for(SOUND_TIME_STOP_SONIC, task, SOUND_PRIORITY, 0,
                       TIME_STOP_STEPS);
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_TIME_STOP_SONIC);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_TIME_STOP_AMY);
        }
        player_pose(record->slot, ANIM_SONIC_POSE);
        break;
    case PLAYER_SHADOW:
        sound_play(SOUND_TIME_STOP_SHADOW, NULL, 0, 0);
        if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_TIME_STOP_SHADOW);
        }
        player_pose(record->slot, ANIM_SHADOW_POSE);
        break;
    }
    game_state.ring_levels[record->slot][RING_LEVEL_TIME_STOP] = 0;
}

Bool sonic_spin(struct task_work *work, struct player_motion *motion,
                struct player *record, struct player *sonic)
{
    if (sonic->by_character.sonic.ball_steps >= SPIN_CHARGE_STEPS &&
        (record->upgrades & UPGRADE_SPARKLE)) {
        work->flags |= PLAYER_WORK_CHARGED;
        light_charge_start(work, motion, record);
        sonic->by_character.sonic.ball_steps = 0;
        if (record->spd.x <= 0.0F) {
            work->mode = PLAYER_MODE_STAND;
            record->mode_steps = 0;
        } else {
            work->mode = PLAYER_MODE_RUN;
        }
        record->anim.next = ANIM_STAND_UP;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        sound_stop_id(SOUND_SPIN);
        sound_stop_id(SOUND_SPIN_SHOES);
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_CHARGED_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_CHARGED_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        return TRUE;
    } else if (sonic->by_character.sonic.ball_steps >= SPIN_STEPS) {
        sonic->by_character.sonic.ball_steps = 0;
        if (record->spd.x <= 0.0F) {
            work->mode = PLAYER_MODE_STAND;
            record->mode_steps = 0;
        } else {
            work->mode = PLAYER_MODE_RUN;
        }
        record->anim.next = ANIM_STAND_UP;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        sound_stop_id(SOUND_SPIN);
        sound_stop_id(SOUND_SPIN_SHOES);
        return TRUE;
    } else if (pad_action_held[record->slot] == 0) {
        work->mode = PLAYER_MODE_ROLL;
        record->anim.next = ANIM_ROLL;
        record->spd.x = record->anim_speed;
        sound_play(SOUND_ROLL, NULL, 0, 0);
        sound_stop_id(SOUND_SPIN);
        sound_stop_id(SOUND_SPIN_SHOES);
        sonic->by_character.sonic.ball_steps = 0;
        record->anim_speed = 0.0F;
        return TRUE;
    } else {
        if (record->anim_speed < SPIN_CHARGE_MAX) {
            record->anim_speed += SPIN_CHARGE_GAIN;
        }
        if (record->upgrades & UPGRADE_SHOES) {
            sound_play_for(SOUND_SPIN_SHOES, work, SOUND_PRIORITY, 0,
                           SOUND_DURATION);
        } else {
            sound_play_for(SOUND_SPIN, work, SOUND_PRIORITY, 0, SOUND_DURATION);
        }
        /* @hack spelling: two ifs, where one test with && would do */
        if (record->upgrades & UPGRADE_SPARKLE) {
            if (game_state.steps & 1) {
                sparkle_light_create(record->slot);
            }
        }
        return FALSE;
    }
}

Bool sonic_test_bounce(struct task_work *work, struct player *record,
                       struct player *sonic)
{
    if ((record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT)) &&
        pad_action_pressed[record->slot]) {
        record->anim.next = ANIM_BALL;
        work->mode = PLAYER_MODE_SONIC_BOUNCE;
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        record->spd.y = BOUNCE_SPEED;
        sonic->by_character.sonic.steps = 0;
        sound_play(SOUND_BOUNCE, NULL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

Bool sonic_test_bounce_again(struct task_work *work, struct player *record)
{
    if ((record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT)) &&
        pad_action_pressed[record->slot]) {
        record->anim.next = ANIM_BALL;
        work->mode = PLAYER_MODE_SONIC_BOUNCE;
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        record->spd.y = BOUNCE_AGAIN_SPEED;
        sound_play(SOUND_BOUNCE, NULL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

Bool sonic_test_pick_up(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.frame < PICK_UP_FRAME) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((flags = player_action_flags(work, record)) != 0) {
        /* @hack spelling: a switch of one case, where an if would do */
        switch (player_face_offer(work, motion, record)) {
        case PLAYER_WALL_HEAD_ON:
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_STAND_HOLDING;
                if (stage_current_id() != STAGE_GREEN_HILL) {
                    if (record->kind == PLAYER_SONIC) {
                        sound_play_at(CRY_PICK_UP_SONIC, NULL, 0, SOUND_LOUDEST,
                                      &work->pos);
                    } else if (record->kind == PLAYER_SHADOW) {
                        sound_play_at(CRY_PICK_UP_SHADOW, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                }
                sound_play(PLAYER_SOUND_PICK_UP, NULL, 0, 0);
                record->held = record->hold_offer;
                record->held->work->flags |= WORK_HELD_BY_PLAYER;
                work->flags |= PLAYER_WORK_HOLDING;
                work->flags &= ~PLAYER_WORK_UNK_8;
                return TRUE;
            }
            break;
        }
    }
    player_stand(work, motion, record);
    return FALSE;
}

Bool sonic_test_pull(struct task_work *work, struct player_motion *motion,
                     struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.current == ANIM_PULL &&
        record->anim.frame < record->anim.actions[record->anim.current].rate &&
        (player_read_stick(work, record, NULL, NULL) ||
         rand() * RAND_SCALE < PULL_LIFT_CHANCE)) {
        record->anim.next = ANIM_PULL_LIFT;
        record->anim.after = ANIM_HOLD_STAND;
        return FALSE;
    }
    if (record->anim.current == ANIM_PULL_LIFT) {
        if (!(work->flags & PLAYER_WORK_HOLDING)) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((flags = player_action_flags(work, record)) != 0) {
                /* @hack spelling: a switch of one case, where an if would
                 * do */
                switch (player_face_offer(work, motion, record)) {
                case PLAYER_WALL_HEAD_ON:
                    if (flags & ACTION_FLAG_1000) {
                        if (stage_current_id() != STAGE_GREEN_HILL) {
                            if (record->kind == PLAYER_SONIC) {
                                sound_play_at(CRY_PICK_UP_SONIC, NULL, 0,
                                              SOUND_LOUDEST, &work->pos);
                            } else if (record->kind == PLAYER_SHADOW) {
                                sound_play_at(CRY_PICK_UP_SHADOW, NULL, 0,
                                              SOUND_LOUDEST, &work->pos);
                            }
                        }
                        sound_play(PLAYER_SOUND_PICK_UP, NULL, 0, 0);
                        record->held = record->hold_offer;
                        record->held->work->flags |=
                            WORK_HELD_BY_PLAYER | FALL_UNK_200;
                        record->held->work->id = FALL_KIND_NORMAL;
                        work->flags |= PLAYER_WORK_HOLDING;
                        work->flags &= ~PLAYER_WORK_UNK_8;
                        return FALSE;
                    }
                    break;
                }
            }
        } else if (player_action_flags(work, record)) {
            return FALSE;
        }
        player_stand(work, motion, record);
    } else if (record->anim.current == ANIM_HOLD_STAND) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((flags = player_action_flags(work, record)) != 0) {
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_STAND_HOLDING;
                return TRUE;
            }
        }
    }
    return FALSE;
}

Bool sonic_throw(struct task_work *work, struct player_motion *motion,
                 struct player *record)
{
    struct near_target *near;
    struct task_work *held;
    struct task_work *nearest;
    Float distance;
    struct collision_shape *shape;
    NJS_VECTOR way;

    if (record->slot == 0) {
        near = near_targets;
    } else {
        near = near_targets_player1;
    }
    if (record->held == NULL) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            work->flags &= ~PLAYER_WORK_HOLDING;
            return TRUE;
        }
        return FALSE;
    }
    if (((record->anim.next == ANIM_THROW_AT &&
          record->anim.frame >= THROW_FRAME) ||
         (record->anim.next == ANIM_AIR_THROW_AT &&
          record->anim.frame >= AIR_THROW_FRAME)) &&
        near->work != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            /* the nearest of the first near target and the items and enemies
             * whose id isn't FALL_KIND_NORMAL. the throw rises toward it along
             * the player's heading */
            nearest = near->work;
            distance = near->distance;
            while (near->work != NULL) {
                if ((near->work->collision->kind == COLLISION_ITEM ||
                     near->work->collision->kind == COLLISION_ENEMY) &&
                    near->work->id != FALL_KIND_NORMAL &&
                    near->distance < distance) {
                    distance = near->distance;
                    nearest = near->work;
                }
                near++;
            }
            shape = nearest->collision->shapes;
            way.x = (nearest->pos.x + shape->center.x) - work->pos.x;
            way.y = (nearest->pos.y + shape->center.y) - work->pos.y;
            way.z = (nearest->pos.z + shape->center.z) - work->pos.z;
            way.x = njSqrt((way.x * way.x) + (way.z * way.z));
            way.z = 0.0F;
            njUnitVector(&way);
            way.x *= THROW_SPEED;
            way.y *= THROW_SPEED;
            turn_to_world(work, &way);
            TASK_MOTION_OF(record->held)->spd = way;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_THROW_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_THROW_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if ((record->anim.next == ANIM_THROW &&
                record->anim.frame >= THROW_FRAME) ||
               (record->anim.next == ANIM_AIR_THROW &&
                record->anim.frame >= AIR_THROW_FRAME)) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            way.x = THROW_AHEAD_X;
            way.y = THROW_AHEAD_Y;
            way.z = 0.0F;
            turn_to_world(work, &way);
            TASK_MOTION_OF(record->held)->spd = way;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_THROW_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_THROW_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if (record->anim.next == ANIM_SET_DOWN &&
               record->anim.frame <= SET_DOWN_FRAME) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            player_set_down(work, held, record);
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        return TRUE;
    }
    return FALSE;
}

Bool sonic_throw_at_mark(struct task *task, struct task_work *work,
                         struct player_motion *motion, struct player *record)
{
    struct task *mark;
    struct task_work *held;
    struct task_work *mark_work;
    struct collision_shape *shape;
    NJS_VECTOR way;

    if (record->held == NULL) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            work->flags &= ~PLAYER_WORK_HOLDING;
            return TRUE;
        }
        return FALSE;
    }
    if (record->anim.next == ANIM_THROW_AT_TOUCH &&
        record->anim.frame >= THROW_AT_TOUCH_FRAME &&
        (mark = collision_first_with_id(task, SHAPE_ID_FACED)) != NULL) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            mark_work = mark->work;
            shape = mark_work->collision->shapes;
            way.x = (mark_work->pos.x + shape->center.x) - held->pos.x;
            way.y = (mark_work->pos.y + shape->center.y) - held->pos.y;
            way.z = (mark_work->pos.z + shape->center.z) - held->pos.z;
            way.x = njSqrt((way.x * way.x) + (way.z * way.z));
            way.z = 0.0F;
            njUnitVector(&way);
            way.x *= THROW_SPEED;
            way.y *= THROW_SPEED;
            turn_to_world(work, &way);
            TASK_MOTION_OF(record->held)->spd = way;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_THROW_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_THROW_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if (record->anim.next == ANIM_THROW_AT_TOUCH &&
               record->anim.frame >= THROW_AT_TOUCH_FRAME) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            way.x = THROW_AHEAD_X;
            way.y = THROW_AHEAD_Y;
            way.z = 0.0F;
            turn_to_world(work, &way);
            TASK_MOTION_OF(record->held)->spd = way;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (stage_current_id() != STAGE_GREEN_HILL) {
            if (record->kind == PLAYER_SONIC) {
                sound_play_at(CRY_THROW_SONIC, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_SHADOW) {
                sound_play_at(CRY_THROW_SHADOW, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

void sonic_drop_held(struct task_work *work, struct player_motion *motion,
                     struct player *record)
{
    struct task_work *held;

    if (record->held == NULL) {
        return;
    }
    held = record->held->work;
    if (player_action_flags(work, record) & ACTION_FLAG_200) {
        return;
    }
    /* @hack spelling: an else after the arm that returns, where an if of its
     * own would do */
    else if (work->collision->hit != NULL &&
             held == work->collision->hit->owner->work) {
        return;
    }
    player_release_held(work, motion, record);
    if (record->held != NULL) {
        return;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        player_stand(work, motion, record);
    } else {
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
    }
}

void sonic_hold_place(struct task_work *work, struct player *record)
{
    struct task_work *held;
    NJS_POINT3 pos;
    NJS_VECTOR way;

    if (record->held == NULL) {
        return;
    }
    if (!(player_action_flags(work, record) & ACTION_FLAG_200)) {
        return;
    }
    held = record->held->work;
    if (record->anim.next == ANIM_THROW_AT || record->anim.next == ANIM_THROW ||
        record->anim.next == ANIM_AIR_THROW_AT ||
        record->anim.next == ANIM_AIR_THROW ||
        record->anim.next == ANIM_THROW_AT_TOUCH) {
        pos.x = record->unk_1e8.x + (record->unk_26c.x * HOLD_REACH);
        pos.y = record->unk_1e8.y + (record->unk_26c.y * HOLD_REACH);
        pos.z = record->unk_1e8.z + (record->unk_26c.z * HOLD_REACH);
        pos.x += record->unk_284.x * HOLD_OUT;
        pos.y += record->unk_284.y * HOLD_OUT;
        pos.z += record->unk_284.z * HOLD_OUT;
    } else {
        pos.x = (record->unk_1e8.x + record->unk_1f4.x) * 0.5F;
        pos.y = (record->unk_1e8.y + record->unk_1f4.y) * 0.5F;
        pos.z = (record->unk_1e8.z + record->unk_1f4.z) * 0.5F;
        way.x = record->unk_284.x + record->unk_290.x;
        way.y = record->unk_284.y + record->unk_290.y;
        way.z = record->unk_284.z + record->unk_290.z;
        if (njScalor(&way) == 0.0F) {
            way.x = way.y = way.z = 0.0F;
        } else {
            njUnitVector(&way);
        }
        pos.x += way.x * HOLD_OUT;
        pos.y += way.y * HOLD_OUT;
        pos.z += way.z * HOLD_OUT;
    }
    held->pos = pos;
}

static void light_charge_step(struct task *task)
{
    struct light_charge
        *charge; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    struct task_work *work;
    struct player *record;

    /* @hack redundant: the extra in a local nothing reads, set in an
     * operand that is evaluated conditionally */
    1 && (charge = task->extra.p); /* NOLINT(clang-diagnostic-unused-value) */
    work = ((struct light_charge *)task->extra.p)->work;
    record = ((struct light_charge *)task->extra.p)->record;
    if (!(record->flags & PLAYER_MISSED) &&
        (work->flags & PLAYER_WORK_CHARGED)) {
        if (pad_action_released[record->slot]) {
            task_kill(task);
            return;
        }
        if (!(game_state.steps & 3)) {
            spin_charge_light_create(record->slot);
        }
    } else {
        task_kill(task);
    }
}

static void light_charge_destroy(struct task *task)
{
    struct player *record = ((struct light_charge *)task->extra.p)->record;

    if (player_work[record->slot] != NULL) {
        record->consts.top_speed =
            ((struct light_charge *)task->extra.p)->top_speed;
    }
}

/* hold the record's top_speed at CHARGED_TOP_SPEED while the light attack's
 * charge lasts. where task_create fails, nothing changes */
static void light_charge_start(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record)
{
    struct task *task;

    task = task_create(TASK_EXTRA, LIGHT_CHARGE_LEVEL, light_charge_step,
                       "ManageSonicLightSpinCharge");
    if (task != NULL) {
        ((struct light_charge *)task->extra.p)->work = work;
        ((struct light_charge *)task->extra.p)->record = record;
        ((struct light_charge *)task->extra.p)->top_speed =
            record->consts.top_speed;
        record->consts.top_speed = CHARGED_TOP_SPEED;
        task->destructor = light_charge_destroy;
    }
}

/* the curve's push across a rail is the fourth root of the product of
 * spd.x's size and rail_curve_scale. the stick's push is its strength times
 * rail_stick_push, times the sine of its angle from the player's heading */
static Float rail_curve_scale = 0.16F;
static Float rail_stick_push = 5.0F;

void sonic_rail(struct task_work *work, struct player_motion *motion,
                struct player *record, struct player *sonic)
{
    NJS_VECTOR acc;
    Angle angle;
    Bool steered;
    unsigned int index;
    Sint8 volume;
    int pitch;
    Float strength;
    Angle heading;
    /* @hack type: downhill an int set to 1 or 0 and tested against 1, where a
     * Bool tested alone would do */
    int downhill;
    int sound;
    Float limit;
    Float weight;
    Float frames;
    Float step;
    Float goal;
    int anim;

    {
        struct path_sample sample;

        /* @hack spelling: the player's pos copied into sample.ahead, where a
         * local of its own would do */
        sample.ahead = work->pos;
        path_find_nearest(record->path, &sample.ahead, &work->pos,
                          &record->path_distance);
        sample.distance = record->path_distance;
        if (path_get_sample(record->path, &sample)) {
            work->ang.x = sample.slope_x;
            work->ang.z = sample.slope_z;
            matrix_push_unit();
            njRotateX(NULL, -work->ang.x);
            njRotateZ(NULL, -work->ang.z);
            njCalcVector(NULL, &sample.ahead, &acc);
            njPopMatrixEx();
            heading = (Angle)(atan2f(acc.z, acc.x) * ANGLE_PER_RADIAN);
            if (angle_distance(heading, work->ang.y) >= QUARTER_TURN) {
                heading += HALF_TURN;
            }
            work->ang.y = motion->aim_y = heading;
        }
    }
    acc = motion->spd;
    njUnitVector(&acc);
    if (njInnerProduct(&acc, &world_gravity) >= 0.0F) {
        downhill = 1;
    } else {
        downhill = 0;
    }
    if (pad_action_held[record->slot]) {
        if (record->anim.current == ANIM_RAIL_A_MINUS) {
            record->anim.next = ANIM_RAIL_A_CROUCH_MINUS;
            record->anim.start_frame = record->anim.frame;
        }
        if (record->anim.current == ANIM_RAIL_A_PLUS) {
            record->anim.next = ANIM_RAIL_A_CROUCH_PLUS;
            record->anim.start_frame = record->anim.frame;
        }
        if (record->anim.current == ANIM_RAIL_B_MINUS) {
            record->anim.next = ANIM_RAIL_B_CROUCH_MINUS;
            record->anim.start_frame = record->anim.frame;
        }
        if (record->anim.current == ANIM_RAIL_B_PLUS) {
            record->anim.next = ANIM_RAIL_B_CROUCH_PLUS;
            record->anim.start_frame = record->anim.frame;
        }
        limit = RAIL_CROUCH_RESIST_SPEED;
        if (downhill == 1) {
            weight = record->consts.gravity * RAIL_CROUCH_GRAVITY_DOWNHILL;
        } else {
            weight = record->consts.gravity * RAIL_CROUCH_GRAVITY_UPHILL;
        }
        if (_builtin_fabsf(record->spd.x) > RAIL_SOUND_SPEED) {
            if (stage_current_id() == STAGE_GREEN_FOREST) {
                sound = SOUND_RAIL_FOREST;
            } else if (stage_current_id() == STAGE_WHITE_JUNGLE) {
                sound = SOUND_RAIL_JUNGLE;
            } else {
                sound = SOUND_RAIL;
            }
            volume = (Sint8)(_builtin_fabsf(record->spd.x) *
                             RAIL_CROUCH_VOLUME_PER_SPEED);
            if (volume > RAIL_VOLUME_RISE_MAX) {
                volume = RAIL_VOLUME_RISE_MAX;
            }
            sound_play_at_for(sound, work, SOUND_PRIORITY,
                              volume + RAIL_CROUCH_VOLUME_BASE, SOUND_DURATION,
                              &work->pos);
            /* @hack redundant: the 0 added to the pitch */
            pitch =
                (int)((_builtin_fabsf(record->spd.x) * RAIL_PITCH_PER_SPEED) +
                      0.0F);
            if (pitch > RAIL_PITCH_MAX) {
                pitch = RAIL_PITCH_MAX;
            }
            sound_set_pitch(work, sound, pitch);
            if (stage_current_id() == STAGE_GREEN_FOREST) {
                sound = SOUND_RAIL_FOREST;
            } else if (stage_current_id() == STAGE_WHITE_JUNGLE) {
                sound = SOUND_RAIL_JUNGLE;
            } else {
                sound = SOUND_RAIL_HIGH;
            }
            if (pitch > RAIL_PITCH_HIGH) {
                sound_play_for(sound, work, SOUND_PRIORITY, 0,
                               SOUND_RAIL_HIGH_STEPS);
                sound_set_volume(work, sound, volume + RAIL_CROUCH_HIGH_VOLUME);
            }
        }
    } else {
        if (record->anim.current == ANIM_RAIL_A_CROUCH_MINUS) {
            record->anim.next = ANIM_RAIL_A_MINUS;
            record->anim.start_frame = record->anim.frame;
        }
        if (record->anim.current == ANIM_RAIL_A_CROUCH_PLUS) {
            record->anim.next = ANIM_RAIL_A_PLUS;
            record->anim.start_frame = record->anim.frame;
        }
        if (record->anim.current == ANIM_RAIL_B_CROUCH_MINUS) {
            record->anim.next = ANIM_RAIL_B_MINUS;
            record->anim.start_frame = record->anim.frame;
        }
        if (record->anim.current == ANIM_RAIL_B_CROUCH_PLUS) {
            record->anim.next = ANIM_RAIL_B_PLUS;
            record->anim.start_frame = record->anim.frame;
        }
        limit = RAIL_RESIST_SPEED;
        if (downhill == 1) {
            weight = record->consts.gravity * RAIL_GRAVITY_DOWNHILL;
        } else {
            weight = record->consts.gravity * RAIL_GRAVITY_UPHILL;
        }
        if (_builtin_fabsf(record->spd.x) > RAIL_SOUND_SPEED) {
            if (stage_current_id() == STAGE_GREEN_FOREST) {
                sound = SOUND_RAIL_FOREST;
            } else if (stage_current_id() == STAGE_WHITE_JUNGLE) {
                sound = SOUND_RAIL_JUNGLE;
            } else {
                sound = SOUND_RAIL;
            }
            volume =
                (Sint8)(_builtin_fabsf(record->spd.x) * RAIL_VOLUME_PER_SPEED);
            if (volume > RAIL_VOLUME_RISE_MAX) {
                volume = RAIL_VOLUME_RISE_MAX;
            }
            sound_play_at_for(sound, work, SOUND_PRIORITY,
                              volume + RAIL_VOLUME_BASE, SOUND_DURATION,
                              &work->pos);
            /* @hack redundant: the 0 added to the pitch */
            pitch =
                (int)((_builtin_fabsf(record->spd.x) * RAIL_PITCH_PER_SPEED) +
                      0.0F);
            if (pitch > RAIL_PITCH_MAX) {
                pitch = RAIL_PITCH_MAX;
            }
            sound_set_pitch(work, sound, pitch);
            if (stage_current_id() == STAGE_GREEN_FOREST) {
                sound = SOUND_RAIL_FOREST;
            } else if (stage_current_id() == STAGE_WHITE_JUNGLE) {
                sound = SOUND_RAIL_JUNGLE;
            } else {
                sound = SOUND_RAIL_HIGH;
            }
            if (pitch > RAIL_PITCH_HIGH) {
                sound_play_for(sound, work, SOUND_PRIORITY, 0,
                               SOUND_RAIL_HIGH_STEPS);
                sound_set_volume(work, sound, volume);
            }
        }
    }
    acc = motion->acc;
    acc.x += weight * world_gravity.x;
    acc.y += weight * world_gravity.y;
    acc.z += weight * world_gravity.z;
    turn_from_world(work, &acc);
    steered = player_read_stick(work, record, &angle, &strength);
    /* from trot_speed on, the rail's curve at the point behind the player
     * pushes it across the rail. the push runs against the change of the
     * rail's way there */
    if (sonic->by_character.sonic.steps > RAIL_CURVE_STEPS) {
        path_segment_at(record->path, record->path_distance, &index);
        if (index > 0 && index < (unsigned int)(record->path->count - 1) &&
            record->consts.trot_speed <= _builtin_fabsf(record->spd.x)) {
            NJS_POINT3 point[3];
            NJS_VECTOR way[2];
            Float root;

            path_point_pos(record->path, index - 1, &point[0]);
            path_point_pos(record->path, index, &point[1]);
            path_point_pos(record->path, index + 1, &point[2]);
            way[0].x = point[1].x - point[0].x;
            way[0].y = point[1].y - point[0].y;
            way[0].z = point[1].z - point[0].z;
            if (njScalor(&way[0]) < RAIL_CURVE_REACH) {
                way[1].x = point[2].x - point[1].x;
                way[1].y = point[2].y - point[1].y;
                way[1].z = point[2].z - point[1].z;
                njSubVector(&way[0], &way[1]);
                if (njScalor(&way[0]) != 0.0F) {
                    njUnitVector(&way[0]);
                    turn_from_world(work, &way[0]);
                    root = njSqrt(_builtin_fabsf(record->spd.x) *
                                  rail_curve_scale);
                    acc.z += way[0].z * njSqrt(root);
                }
            }
        }
    }
    /* @hack spelling: two arms that add the same drag, where one test of
     * either would do */
    if (record->spd.x > limit) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (record->spd.x < -limit) {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    if (!steered) {
        strength = 0.0F;
    } else {
        strength *= njSin(angle_difference(work->ang.y, angle));
    }
    acc.z += strength * rail_stick_push;
    /* downhill, the push along the rail shrinks as the push across it grows.
     * it is scaled by 1 less the size of the push across, and by 0 at
     * least */
    if (downhill == 1) {
        step = 1.0F - _builtin_fabsf(acc.z);
        if (step < 0.0F) {
            step = 0.0F;
        }
        acc.x *= step;
    }
    /* the frame of the lean's animation follows how far the player leans:
     * acc.z from trot_speed on, and the stick below it. a lean the other
     * way waits for the animation's first frame to change to the other
     * side's */
    if (record->anim.mode == PLAYER_ANIM_DRIVEN) {
        anim = record->anim.current;
        frames =
            motion_table[record->anim.actions[anim].motion].motion->nbFrame;
        if (_builtin_fabsf(record->spd.x) > record->consts.trot_speed) {
            strength = acc.z;
            if (strength >= 1.0F) {
                strength = 1.0F;
            } else if (strength <= -1.0F) {
                strength = -1.0F;
            }
        }
        if (strength < 0.0F && anim == ANIM_RAIL_A_PLUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_A_MINUS;
            }
        } else if (strength < 0.0F && anim == ANIM_RAIL_B_PLUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_B_MINUS;
            }
        } else if (strength < 0.0F && anim == ANIM_RAIL_A_CROUCH_PLUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_A_CROUCH_MINUS;
            }
        } else if (strength < 0.0F && anim == ANIM_RAIL_B_CROUCH_PLUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_B_CROUCH_MINUS;
            }
        } else if (strength > 0.0F && anim == ANIM_RAIL_A_MINUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_A_PLUS;
            }
        } else if (strength > 0.0F && anim == ANIM_RAIL_B_MINUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_B_PLUS;
            }
        } else if (strength > 0.0F && anim == ANIM_RAIL_A_CROUCH_MINUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_A_CROUCH_PLUS;
            }
        } else if (strength > 0.0F && anim == ANIM_RAIL_B_CROUCH_MINUS) {
            strength = 0.0F;
            if (record->anim.frame == 0.0F) {
                record->anim.next = ANIM_RAIL_B_CROUCH_PLUS;
            }
        }
        if (strength < 0.0F) {
            strength = _builtin_fabsf(strength);
        }
        goal = (frames - 1.0F) * strength;
        step = _builtin_fabsf(record->anim.frame - goal) * RAIL_LEAN_SHARE;
        if (step < RAIL_LEAN_STEP_MIN) {
            step = RAIL_LEAN_STEP_MIN;
        }
        /* @hack spelling: the goal reckoned again in the call, where goal
         * would do */
        record->anim.frame = float_approach(record->anim.frame,
                                            (frames - 1.0F) * strength, step);
    }
    record->accel = acc;
    sonic->by_character.sonic.steps++;
    if (sonic->by_character.sonic.steps < 0) {
        sonic->by_character.sonic.steps = SONIC_STEPS_MAX;
    }
    /* a totter brakes the player by ground_drag above trot_speed. ball_steps
     * counts the steps of a totter, and of the lean too far that starts it */
    anim = record->anim.current;
    if (anim == ANIM_RAIL_TOTTER_MINUS || anim == ANIM_RAIL_TOTTER_PLUS) {
        if (_builtin_fabsf(acc.z) < RAIL_STEADY) {
            sonic->by_character.sonic.ball_steps = 0;
            if (anim == ANIM_RAIL_TOTTER_MINUS) {
                if (pad_action_held[record->slot]) {
                    anim = ANIM_RAIL_A_CROUCH_MINUS;
                } else {
                    anim = ANIM_RAIL_A_MINUS;
                }
            } else if (anim == ANIM_RAIL_TOTTER_PLUS) {
                if (pad_action_held[record->slot]) {
                    anim = ANIM_RAIL_A_CROUCH_PLUS;
                } else {
                    anim = ANIM_RAIL_A_PLUS;
                }
            }
            record->anim.next = anim;
            record->anim.start_frame =
                (Float)motion_table[record->anim.actions[anim].motion]
                    .motion->nbFrame *
                RAIL_TOTTER_MIDDLE;
        } else {
            sonic->by_character.sonic.ball_steps++;
            if (_builtin_fabsf(record->spd.x) > record->consts.trot_speed) {
                if (record->spd.x > 0.0F) {
                    record->accel.x = record->consts.ground_drag;
                } else if (record->spd.x < 0.0F) {
                    record->accel.x = -record->consts.ground_drag;
                }
            }
        }
    } else if ((anim == ANIM_RAIL_A_CROUCH_MINUS ||
                anim == ANIM_RAIL_A_CROUCH_PLUS ||
                anim == ANIM_RAIL_B_CROUCH_MINUS ||
                anim == ANIM_RAIL_B_CROUCH_PLUS) &&
               record->anim.frame >=
                   (Float)motion_table[record->anim.actions[anim].motion]
                           .motion->nbFrame *
                       RAIL_CROUCH_TOTTER) {
        if (anim == ANIM_RAIL_A_CROUCH_MINUS) {
            record->anim.next = ANIM_RAIL_TOTTER_MINUS;
        } else if (anim == ANIM_RAIL_A_CROUCH_PLUS) {
            record->anim.next = ANIM_RAIL_TOTTER_PLUS;
        } else if (anim == ANIM_RAIL_B_CROUCH_MINUS) {
            record->anim.next = ANIM_RAIL_TOTTER_MINUS;
        } else if (anim == ANIM_RAIL_B_CROUCH_PLUS) {
            record->anim.next = ANIM_RAIL_TOTTER_PLUS;
        }
        sonic->by_character.sonic.ball_steps++;
    } else if ((anim == ANIM_RAIL_A_MINUS || anim == ANIM_RAIL_A_PLUS ||
                anim == ANIM_RAIL_B_MINUS || anim == ANIM_RAIL_B_PLUS) &&
               _builtin_fabsf(record->spd.x) < RAIL_TOTTER_SPEED &&
               record->anim.frame >=
                   (Float)motion_table[record->anim.actions[anim].motion]
                           .motion->nbFrame *
                       RAIL_TOTTER) {
        if (anim == ANIM_RAIL_A_MINUS) {
            record->anim.next = ANIM_RAIL_TOTTER_MINUS;
        } else if (anim == ANIM_RAIL_A_PLUS) {
            record->anim.next = ANIM_RAIL_TOTTER_PLUS;
        } else if (anim == ANIM_RAIL_B_MINUS) {
            record->anim.next = ANIM_RAIL_TOTTER_MINUS;
        } else if (anim == ANIM_RAIL_B_PLUS) {
            record->anim.next = ANIM_RAIL_TOTTER_PLUS;
        }
        sonic->by_character.sonic.ball_steps++;
    } else {
        sonic->by_character.sonic.ball_steps = 0;
    }
}

Bool sonic_test_soap_switch(struct task_work *work, struct player *record,
                            struct player *sonic)
{
    struct collision_work *hit;
    struct camera_motion camera;
    struct task_work *soap;
    Float boost;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((hit = work->collision->hit) != NULL) {
        soap = hit->owner->work;
        if (soap != NULL && soap->id == WORK_ID_SOAP_SWITCH &&
            _builtin_fabsf(record->spd.x) > record->consts.trot_speed &&
            player_jump_pressed(work, record)) {
            work->mode = PLAYER_MODE_SONIC_SOAP_JUMP;
            record->spd.y = record->consts.jump_speed;
            work->ang.z = 0;
            work->ang.x = 0;
            camera.origin = &work->pos;
            camera.ang = HALF_TURN - work->ang.y;
            camera.motion = NULL;
            camera.flags = CAMERA_MOTION_END_BLENDS | CAMERA_MOTION_FOLLOWS;
            camera.done = NULL;
            camera.speed = SOAP_CAMERA_SPEED;
            if (record->spd.x >= SOAP_A_SPEED) {
                record->anim.next = ANIM_SOAP_A;
                camera.motion = soap_grade_a_camera;
                record->spd.x = SOAP_A_SPEED_X;
                record->spd.y = SOAP_A_SPEED_Y;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_A;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_A_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_A_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else if (record->spd.x >= SOAP_B_SPEED) {
                record->anim.next = ANIM_SOAP_B;
                camera.motion = &soap_grade_b_camera;
                record->spd.x = SOAP_B_SPEED_X;
                record->spd.y = SOAP_B_SPEED_Y;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_B;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_B_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_B_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else if (record->spd.x >= SOAP_C_SPEED) {
                record->anim.next = ANIM_SOAP_C;
                record->spd.y = SOAP_C_SPEED_Y;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_C;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_C_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_C_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else if (record->spd.x <= -SOAP_A_SPEED) {
                record->anim.next = ANIM_SOAP_A;
                camera.motion = soap_grade_a_camera;
                record->spd.x = -SOAP_A_SPEED_X;
                record->spd.y = SOAP_A_SPEED_Y;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_A;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_A_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_A_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else if (record->spd.x <= -SOAP_B_SPEED) {
                record->anim.next = ANIM_SOAP_B;
                camera.motion = &soap_grade_b_camera;
                record->spd.x = -SOAP_B_SPEED_X;
                record->spd.y = SOAP_B_SPEED_Y;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_B;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_B_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_B_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else if (record->spd.x <= -SOAP_C_SPEED) {
                record->anim.next = ANIM_SOAP_C;
                record->spd.y = SOAP_C_SPEED_Y;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_C;
                if (record->kind == PLAYER_SONIC) {
                    voice_play_priority_2(VOICE_TRICK_C_SONIC);
                } else if (record->kind == PLAYER_SHADOW) {
                    voice_play_priority_2(VOICE_TRICK_C_SHADOW);
                } else if (record->kind == PLAYER_KIND_AMY) {
                    voice_play_priority_2(VOICE_TRICK_AMY);
                }
            } else {
                record->anim.next = ANIM_FALL;
                sonic->by_character.sonic.trick_grade = SONIC_TRICK_GRADE_NONE;
            }
            if (placement_get_state(hit->owner)) {
                sonic->by_character.sonic.trick_grade =
                    SONIC_TRICK_GRADE_PASSED;
            }
            if (sonic->by_character.sonic.trick_grade == SONIC_TRICK_GRADE_A) {
                boost = njRoundOff(
                    _builtin_fabsf(njFraction(soap->scl.z) * SOAP_PERCENT));
            } else if (sonic->by_character.sonic.trick_grade ==
                       SONIC_TRICK_GRADE_B) {
                boost = njRoundOff(
                    _builtin_fabsf(njFraction(soap->scl.y) * SOAP_PERCENT));
            } else {
                boost = 0.0F;
            }
            if (boost >= SOAP_BOOST_LOW) {
                boost = (boost - SOAP_WHOLE) * SOAP_BOOST_FACTOR;
            } else if (boost < SOAP_BOOST_MIN) {
                boost = 0.0F;
            } else {
                boost *= SOAP_BOOST_FACTOR;
            }
            /* @bug logic: the change adds to spd.x whichever way the trick
             * goes, FADD at 0x8c074cb8, where the grades' arms set spd.x
             * either way: a trick along -x is slowed by what speeds one
             * along +x */
            record->spd.x += boost;
            if (soap->tick == 0 && camera.motion != NULL) {
                camera_motion_start(record->slot, &camera);
            }
            record->anim.after = ANIM_FALL;
            work->flags &=
                ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            record->jump_steps = 0;
            if (record->character == PLAYER_SONIC) {
                sound_play(SOUND_JUMP_SONIC, NULL, 0, 0);
            } else {
                sound_play(SOUND_JUMP_SHADOW, NULL, 0, 0);
            }
            record->stick_lock = soap->timer;
            return TRUE;
        }
    }
    return FALSE;
}

/* the ride beneath a rail sounds above RIDE_SOUND_SPEED along x. its volume
 * is RIDE_VOLUME_BASE plus RIDE_VOLUME_PER_SPEED for each unit of that speed.
 * the part past the base is RIDE_VOLUME_RISE_MAX at most. its pitch is
 * RIDE_PITCH_PER_SPEED for each unit, up to RIDE_PITCH_MAX */
#define RIDE_SOUND_SPEED 0.2F
#define RIDE_VOLUME_BASE 80
#define RIDE_VOLUME_PER_SPEED 5.0F
#define RIDE_VOLUME_RISE_MAX 40
#define RIDE_PITCH_PER_SPEED 512.0F
#define RIDE_PITCH_MAX 0x800
/* the ride's pull along world_gravity, in units of the constants' gravity.
 * its drag, resist.x times spd.x, acts past RIDE_RESIST_SPEED */
#define RIDE_GRAVITY 1.4F
#define RIDE_RESIST_SPEED 15.64F

/* the ride's sound on a stage that gives it none */
#define NO_SOUND (-1)

/* how far sonic_drop moves a player down its own y axis */
#define DROP_HEIGHT 10.0F

/* the first burst starts SPARK_BEHIND back from unk_20c along the motion's spd.
 * it starts SPARK_DROP down world_gravity and SPARK_OUT along unk_29c too. it
 * then moves a random share of the way to unk_200, up to SPARK_REACH of it. the
 * second burst starts SPARK_APART back along unk_29c from the first */
#define SPARK_BEHIND 4.0F
#define SPARK_DROP 0.5F
#define SPARK_OUT 2.0F
#define SPARK_REACH 1.3F
#define SPARK_APART 4.0F
/* a burst holds SPARKS_PER_SPEED specks for each unit of the record's
 * speed. the first holds drive times SPARKS_PER_DRIVE more, and the second
 * as many fewer */
#define SPARKS_PER_SPEED 0.5F
#define SPARKS_PER_DRIVE 5.0F
/* the lean's sound plays at BOARD_VOLUME while drive times
 * BOARD_SOUND_PER_DRIVE lies more than BOARD_SOUND_MIN from 0. its pan is
 * drive times BOARD_PAN_PER_DRIVE, within BOARD_PAN_MAX either way */
#define BOARD_SOUND_PER_DRIVE 5.0F
#define BOARD_SOUND_MIN 2.0F
#define BOARD_VOLUME 80
#define BOARD_PAN_PER_DRIVE 50.0F
#define BOARD_PAN_MAX 127

/* a lean aims at the frame LEAN_FRAME plus LEAN_FRAME_PER_DRIVE for each unit
 * of drive's size. its frame steps toward the aim by drive's size, or by
 * LEAN_STEP_MIN where that is more. within LEAN_SNAP of the aim, it takes the
 * aim. where drive changes sign, the lean aims at frame 0. it runs back there
 * by LEAN_RETURN_STEP a step, and then changes sides. without the stick, or
 * while drive is 0, the frame runs back by LEAN_IDLE_STEP */
#define LEAN_FRAME 4.0F
#define LEAN_FRAME_PER_DRIVE 12.0F
#define LEAN_STEP_MIN 0.5F
#define LEAN_SNAP 1.0F
#define LEAN_RETURN_STEP 2.0F
#define LEAN_IDLE_STEP 1.0F

void sonic_rail_beneath(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic)
{
    /* @hack spelling: the locals declared as acc, sample, up, strength and
     * heading_asked ahead of sound, not in the order the code first uses
     * them */
    NJS_VECTOR acc;
    struct path_sample sample;
    NJS_VECTOR up;
    Float strength;
    Angle heading_asked;
    int sound;
    Sint8 volume;
    int pitch;
    Angle heading;
    Float fall;
    Float resist_speed;

    if (_builtin_fabsf(record->spd.x) > RIDE_SOUND_SPEED) {
        sound = NO_SOUND;
        /* @hack spelling: two tests of the stage, each setting sound, where
         * one test of both would do */
        if (stage_current_id() == STAGE_CRAZY_GADGET) {
            sound = SOUND_RIDE;
        } else if (stage_current_id() == STAGE_RELAY_SONIC) {
            sound = SOUND_RIDE;
        }
        if (sound != NO_SOUND) {
            /* @bug arithmetic: the volume, a Sint8, wraps below 0 from a spd.x
             * of 25.6, so the cap at 0x8c074dbc lets it through */
            volume =
                (Sint8)(_builtin_fabsf(record->spd.x) * RIDE_VOLUME_PER_SPEED);
            if (volume > RIDE_VOLUME_RISE_MAX) {
                volume = RIDE_VOLUME_RISE_MAX;
            }
            sound_play_at_for(sound, work, SOUND_PRIORITY,
                              volume + RIDE_VOLUME_BASE, SOUND_DURATION,
                              &work->pos);
            /* @hack redundant: the 0 added to the pitch */
            pitch =
                (int)((_builtin_fabsf(record->spd.x) * RIDE_PITCH_PER_SPEED) +
                      0.0F);
            if (pitch > RIDE_PITCH_MAX) {
                pitch = RIDE_PITCH_MAX;
            }
            sound_set_pitch(work, sound, pitch);
        }
    }
    /* @hack spelling: the pos copied into the sample's ahead, which
     * path_get_sample then fills, where a point of its own would do */
    sample.ahead = work->pos;
    path_find_nearest(record->path, &sample.ahead, &work->pos,
                      &record->path_distance);
    sample.distance = record->path_distance;
    if (path_get_sample(record->path, &sample)) {
        up.x = 0.0F;
        up.y = 1.0F;
        up.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, sample.slope_z);
        njRotateX(NULL, sample.slope_x);
        njCalcVector(NULL, &up, &up);
        njPopMatrixEx();
        work->ang.x = (Angle)(asinf(-up.z) * ANGLE_PER_RADIAN);
        work->ang.z = -(Angle)(atan2f(-up.x, -up.y) * ANGLE_PER_RADIAN);
        heading =
            (Angle)(atan2f(sample.ahead.z, sample.ahead.x) * ANGLE_PER_RADIAN);
        if (angle_distance(heading, work->ang.y) >= QUARTER_TURN) {
            heading += HALF_TURN;
        }
        motion->aim_y = heading;
        work->ang.y = heading;
    }
    fall = record->consts.gravity * RIDE_GRAVITY;
    /* @hack spelling: the limit in a local of its own, where the constant
     * in each test would do */
    resist_speed = RIDE_RESIST_SPEED;
    acc = motion->acc;
    acc.x += fall * world_gravity.x;
    acc.y += fall * world_gravity.y;
    acc.z += fall * world_gravity.z;
    turn_from_world(work, &acc);
    /* @hack redundant: the stick's test, whose heading and strength nothing
     * reads */
    player_read_stick(work, record, &heading_asked, &strength);
    /* @hack spelling: two arms that add the same, where one test of both
     * would do */
    if (record->spd.x > resist_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    } else if (record->spd.x < -resist_speed) {
        acc.x += record->consts.resist.x * record->spd.x;
    }
    record->accel = acc;
}

void sonic_drop(struct task_work *work)
{
    NJS_VECTOR drop;

    drop.x = 0.0F;
    drop.y = -DROP_HEIGHT;
    drop.z = 0.0F;
    turn_to_world(work, &drop);
    njAddVector(&work->pos, &drop);
}

void sonic_board_sparks_and_lean(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float reach;
    int pan;
    Float aim;

    if (!(record->ground_flags & SPARKLESS_GROUND) &&
        njScalor(&record->spd) > BOARD_SPARKS_SPEED) {
        spd.x = (motion->spd.x * SPARK_SPEED_SHARE) -
                (world_gravity.x * SPARK_FALL_SHARE);
        spd.y = (motion->spd.y * SPARK_SPEED_SHARE) -
                (world_gravity.y * SPARK_FALL_SHARE);
        spd.z = (motion->spd.z * SPARK_SPEED_SHARE) -
                (world_gravity.z * SPARK_FALL_SHARE);
        pos = motion->spd;
        njUnitVector(&pos);
        /* @hack spelling: the step back negated and added, where its
         * subtraction from unk_20c would do */
        pos.x = (-pos.x * SPARK_BEHIND) + record->unk_20c.x +
                (world_gravity.x * SPARK_DROP) +
                (record->unk_29c.x * SPARK_OUT);
        pos.y = (-pos.y * SPARK_BEHIND) + record->unk_20c.y +
                (world_gravity.y * SPARK_DROP) +
                (record->unk_29c.y * SPARK_OUT);
        pos.z = (-pos.z * SPARK_BEHIND) + record->unk_20c.z +
                (world_gravity.z * SPARK_DROP) +
                (record->unk_29c.z * SPARK_OUT);
        reach = rand() * (SPARK_REACH * RAND_SCALE);
        pos.x += reach * (record->unk_200.x - record->unk_20c.x);
        pos.y += reach * (record->unk_200.y - record->unk_20c.y);
        pos.z += reach * (record->unk_200.z - record->unk_20c.z);
        powder_create(record->slot,
                      (int)((njScalor(&record->spd) * SPARKS_PER_SPEED) +
                            (record->drive * SPARKS_PER_DRIVE)),
                      &pos, &spd);
        pos.x -= record->unk_29c.x * SPARK_APART;
        pos.y -= record->unk_29c.y * SPARK_APART;
        pos.z -= record->unk_29c.z * SPARK_APART;
        powder_create(record->slot,
                      (int)((njScalor(&record->spd) * SPARKS_PER_SPEED) -
                            (record->drive * SPARKS_PER_DRIVE)),
                      &pos, &spd);
        if (stage_current_id() == STAGE_CITY_ESCAPE &&
            _builtin_fabsf(record->drive * BOARD_SOUND_PER_DRIVE) >
                BOARD_SOUND_MIN) {
            sound_play_at_for(SOUND_BOARD_CITY, work, SOUND_PRIORITY,
                              BOARD_VOLUME, SOUND_DURATION, &work->pos);
            pan = (int)(record->drive * BOARD_PAN_PER_DRIVE);
            /* @hack spelling: the tests against BOARD_PAN_MAX + 1 either
             * way, where tests against BOARD_PAN_MAX would do */
            if (pan >= BOARD_PAN_MAX + 1) {
                pan = BOARD_PAN_MAX;
            }
            if (pan <= -(BOARD_PAN_MAX + 1)) {
                pan = -BOARD_PAN_MAX;
            }
            sound_set_pan(work, SOUND_BOARD_CITY, pan);
        }
    }
    if (record->anim.next != ANIM_BOARD_LEAN_NEG &&
        record->anim.next != ANIM_BOARD_LEAN_POS &&
        record->anim.next != ANIM_BOARD_FAST_LEAN_NEG &&
        record->anim.next != ANIM_BOARD_FAST_LEAN_POS) {
        return;
    }
    if (record->anim.mode != PLAYER_ANIM_DRIVEN) {
        return;
    }
    if (work->mode == PLAYER_MODE_SONIC_BOARD_FAST) {
        if (record->anim.next == ANIM_BOARD_LEAN_NEG) {
            record->anim.next = ANIM_BOARD_FAST_LEAN_NEG;
        } else if (record->anim.next == ANIM_BOARD_LEAN_POS) {
            record->anim.next = ANIM_BOARD_FAST_LEAN_POS;
        }
    } else {
        if (record->anim.next == ANIM_BOARD_FAST_LEAN_NEG) {
            record->anim.next = ANIM_BOARD_LEAN_NEG;
        } else if (record->anim.next == ANIM_BOARD_FAST_LEAN_POS) {
            record->anim.next = ANIM_BOARD_LEAN_POS;
        }
    }
    if (player_read_stick(work, record, NULL, NULL)) {
        if (record->drive < 0.0F) {
            /* @hack spelling: the aim set in each arm, where one aim after
             * the test of the frame would do */
            if (record->anim.next == ANIM_BOARD_LEAN_POS ||
                record->anim.next == ANIM_BOARD_FAST_LEAN_POS) {
                if (record->anim.frame == 0.0F) {
                    if (record->anim.next == ANIM_BOARD_LEAN_POS) {
                        record->anim.next = ANIM_BOARD_LEAN_NEG;
                    } else {
                        record->anim.next = ANIM_BOARD_FAST_LEAN_NEG;
                    }
                    aim = LEAN_FRAME - (record->drive * LEAN_FRAME_PER_DRIVE);
                } else {
                    aim = 0.0F;
                }
            } else {
                aim = LEAN_FRAME - (record->drive * LEAN_FRAME_PER_DRIVE);
            }
        } else if (record->drive > 0.0F) {
            if (record->anim.next == ANIM_BOARD_LEAN_NEG ||
                record->anim.next == ANIM_BOARD_FAST_LEAN_NEG) {
                if (record->anim.frame == 0.0F) {
                    if (record->anim.next == ANIM_BOARD_LEAN_NEG) {
                        record->anim.next = ANIM_BOARD_LEAN_POS;
                    } else {
                        record->anim.next = ANIM_BOARD_FAST_LEAN_POS;
                    }
                    aim = LEAN_FRAME + (record->drive * LEAN_FRAME_PER_DRIVE);
                } else {
                    aim = 0.0F;
                }
            } else {
                aim = LEAN_FRAME + (record->drive * LEAN_FRAME_PER_DRIVE);
            }
        } else {
            record->anim.frame -= LEAN_IDLE_STEP;
            if (record->anim.frame <= 0.0F) {
                record->anim.frame = 0.0F;
            }
            aim = record->anim.frame;
        }
        if (record->anim.frame > aim) {
            if (aim == 0.0F) {
                record->anim.frame -= LEAN_RETURN_STEP;
                if (record->anim.frame < 0.0F) {
                    record->anim.frame = 0.0F;
                }
            } else if (record->anim.frame - aim > LEAN_SNAP) {
                if (_builtin_fabsf(record->drive) > LEAN_STEP_MIN) {
                    record->anim.frame -= _builtin_fabsf(record->drive);
                } else {
                    record->anim.frame -= LEAN_STEP_MIN;
                }
            } else {
                record->anim.frame = aim;
            }
        } else if (record->anim.frame < aim) {
            if (aim - record->anim.frame > LEAN_SNAP) {
                if (_builtin_fabsf(record->drive) > LEAN_STEP_MIN) {
                    record->anim.frame += _builtin_fabsf(record->drive);
                } else {
                    record->anim.frame += LEAN_STEP_MIN;
                }
            } else {
                record->anim.frame = aim;
            }
        }
    } else {
        record->anim.frame -= LEAN_IDLE_STEP;
        if (record->anim.frame <= 0.0F) {
            record->anim.frame = 0.0F;
        }
    }
}

Bool sonic_board_take_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_JOIN_PATH:
        if (record->spd.x > record->consts.running_speed) {
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_ANIM:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_SCARE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_RESET:
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_stand(work, motion, record);
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        break;
    case PLAYER_REQUEST_MISS:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_MISS;
        /* @hack spelling: an empty arm for ANIM_IN_CURRENT, where a test of
         * the other animations would do */
        if (record->anim.current == ANIM_IN_CURRENT) {
        } else {
            record->anim.next = ANIM_MISS;
        }
        work->flags &= ~PLAYER_WORK_ON_PATH;
        if (record->kind == PLAYER_SONIC) {
            voice_play_priority_2(VOICE_MISS_SONIC);
        } else if (record->kind == PLAYER_SHADOW) {
            voice_play_priority_2(VOICE_MISS_SHADOW);
        } else if (record->kind == PLAYER_KIND_AMY) {
            voice_play_priority_2(VOICE_MISS_AMY);
        }
        break;
    case PLAYER_REQUEST_RELEASE:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
