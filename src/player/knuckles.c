/* the treasure hunters: Knuckles and Rouge, and Tikal and Chaos 0 in their
 * places in a two-player game. their task, its draws and moves, their
 * two-player attacks, and the hunt that stage/hunt.h declares */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include <string.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/color.h"
#include "draw/helpers.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "effect/black_wave.h"
#include "effect/bubbles.h"
#include "effect/puff.h"
#include "effect/stars.h"
#include "effect/thunder.h"
#include "event/mini_event.h"
#include "hud/action_window.h"
#include "hud/hud.h"
#include "main.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/action.h"
#include "player/knuckles.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/snapshot.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the size in bytes of a hunter's record, which its task's extra points to.
 * its own fields are struct player's by_character.hunter */
#define HUNTER_RECORD_SIZE 0x3c8

/* the hunters' slots of model_table, which their model files fill. each
 * table's animations pose KNUCKLES_BODY, ROUGE_BODY or ROUGE_PART. the rest
 * are nodes of a body and the models its draw adds. a _SHADOW slot is a
 * modifier volume */
#define KNUCKLES_BODY 142
/* the draw keeps its place in unk_230, and its x axis in unk_2cc */
#define KNUCKLES_AT_230 143
#define KNUCKLES_LEAN 144
/* the head, and the two eyes, which the look task turns */
#define KNUCKLES_HEAD 145
#define KNUCKLES_EYE_0 146
#define KNUCKLES_EYE_1 147
#define KNUCKLES_HEAD_SHADOW 148
#define KNUCKLES_MOUTH 149
#define KNUCKLES_FOOT_0 150
#define KNUCKLES_FOOT_1 151
#define KNUCKLES_HAND_0 152
#define KNUCKLES_HAND_1 153
#define KNUCKLES_ARM_0 154
#define KNUCKLES_ARM_1 155
/* the swing jiggle copies this slot's child, and takes KNUCKLES_SWING_SHAPE
 * as its shape_a. the draw swaps the child's model for the copy's */
#define KNUCKLES_SWING_PARENT 156
#define KNUCKLES_FACE 160 /* the node the sunglasses draw at */
#define KNUCKLES_SUNGLASSES 161 /* its child is the glasses */
/* the claws of UPGRADE_KNUCKLES_CLAWS and the gloves of
 * UPGRADE_KNUCKLES_GROVES, drawn at the hands */
#define KNUCKLES_CLAW_0 162
#define KNUCKLES_CLAW_1 163
#define KNUCKLES_GLOVE_0 164
#define KNUCKLES_GLOVE_1 165
#define KNUCKLES_SWING_SHAPE 166
#define KNUCKLES_NECK 167 /* the node the necklace draws at */
#define KNUCKLES_NECKLACE 168 /* the necklace of UPGRADE_KNUCKLES_AIR */
/* the afterimages' bodies, where they are loaded. the animation playing poses
 * KNUCKLES_AFTERIMAGE, and hunter_spin_afterimage turns KNUCKLES_SPIN by
 * its frame */
#define KNUCKLES_AFTERIMAGE 169
#define KNUCKLES_SPIN 170

/* the slots Rouge's files fill */
#define ROUGE_BODY 171
#define ROUGE_AT_230 172
#define ROUGE_LEAN 173
/* the squash jiggle copies this slot's object, and takes it as its shape_a.
 * the draw swaps the object's model for the copy's */
#define ROUGE_SQUASH 174
#define ROUGE_HEAD 175
#define ROUGE_EYE_0 176
#define ROUGE_EYE_1 177
#define ROUGE_HEAD_SHADOW 178
#define ROUGE_MOUTH 179
#define ROUGE_FOOT_0 180
#define ROUGE_FOOT_1 181
#define ROUGE_HAND_0 182
#define ROUGE_HAND_1 183
#define ROUGE_ARM_0 184
#define ROUGE_ARM_1 185
/* the swing jiggle copies this slot's child, and takes the child as its
 * shape_a. the draw swaps the child's model for the copy's */
#define ROUGE_SWING_PARENT 186
/* the part, which the draw poses by part_motion at ROUGE_PART_NODE's place */
#define ROUGE_PART 190
#define ROUGE_PART_NODE 191
/* two models drawn at two nodes, each with the nails of UPGRADE_ROUGE_NAILS
 * after it */
#define ROUGE_UNDER_NAILS_0 192
#define ROUGE_NAILS_NODE_0 193
#define ROUGE_NAILS_0 195
#define ROUGE_UNDER_NAILS_1 196
#define ROUGE_NAILS_NODE_1 197
#define ROUGE_NAILS_1 199
/* the boots of UPGRADE_ROUGE_BOOTS, each drawn at its node */
#define ROUGE_BOOT_NODE_0 200
#define ROUGE_BOOT_0 201
#define ROUGE_BOOT_NODE_1 202
#define ROUGE_BOOT_1 203
#define ROUGE_FACE 204 /* the node the scope draws at */
/* the scope of UPGRADE_ROUGE_SCOPE, whose child turns down */
#define ROUGE_SCOPE 205
#define ROUGE_AFTERIMAGE 206
#define ROUGE_SPIN 207

/* the param and phase_step the makers give each jiggle, and the squash's
 * by_kind.angle. a swing follows the head's place, unk_218. it takes a phase of
 * QUARTER_TURN at a step in which that place moves farther than
 * PLAYER_SWING_KICK. Knuckles's swing flutters in a glide instead */
#define JIGGLE_PARAM 0.4F
#define JIGGLE_PHASE_STEP 0x400
#define SQUASH_ANGLE 0x3000

/* the slots of motion_table that Rouge's part plays. the slot after each
 * holds the motion of the part's shape. the part plays PART_MOTION_FAST in a
 * glide, a fall, a landing, ANIM_UNK_179 and ANIM_UNK_180. it plays
 * PART_MOTION otherwise, or PART_MOTION_PSO in COSTUME_PSO. its frame steps
 * PART_STEP_FAST a step in PART_MOTION_FAST, and PART_STEP in the others and
 * in a blend */
#define PART_MOTION_FAST 205
#define PART_MOTION 207
#define PART_MOTION_PSO 209
#define PART_STEP_FAST 0.3F
#define PART_STEP 0.05F
/* two of Rouge's animations, which follow her poses in her table */
#define ANIM_UNK_179 179
#define ANIM_UNK_180 180

/* outside PLAYER_INVINCIBLE, hunter_next_mode leaves a work timer of
 * TIMER_HELD as it is. there it clears no WORK_HURT and tries no hit */
#define TIMER_HELD 0x7ff0

/* a dig sinks DIG_SINK a step along the ground's normal, from its
 * animation's DIG_SINK_FRAME on. the find turns up once the dig sinks past
 * DIG_DEPTH. the hunter then rises DIG_RISE a step out of the ground, or
 * WALL_DIG_RISE out of a wall. a dig into a wall goes on to
 * ANIM_DIG_WALL_END once the hunter has risen above WALL_DIG_END_DEPTH */
#define DIG_SINK 0.4F
#define DIG_SINK_FRAME 6.0F
#define DIG_DEPTH (-20.0F)
#define DIG_RISE 2.0F
#define WALL_DIG_RISE 0.4F
#define WALL_DIG_END_DEPTH (-12.0F)
/* a dig's debris flies every DIG_DEBRIS_STEPS steps once DIG_START steps
 * have passed. the debris is a ring of pieces DEBRIS_RING_STEP apart, and
 * one piece along the hunter's up. rand() times DEBRIS_RING_START is a
 * random angle below DEBRIS_RING_STEP, at which the ring starts. a dig out of
 * the ground throws the ring's pieces up to DEBRIS_OUT_RISE times higher. it
 * throws the piece along the up at DEBRIS_OUT_UP */
#define DIG_DEBRIS_STEPS 8
#define DIG_START 3
#define DEBRIS_RING_STEP 0x800
#define DEBRIS_RING_START 0.0625F
#define DEBRIS_OUT_RISE 3.5F
#define DEBRIS_OUT_UP 2.0F
/* a dig out of the ground ends in a jump at DIG_JUMP_SPEED */
#define DIG_JUMP_SPEED 2.0F

/* a glide outside the kill areas, as hunter_outside_kill_areas finds them,
 * turns back in PLAYER_MODE_HUNTER_TURN_BACK. it turns TURN_BACK_ANGLE a step
 * while it stays outside. it glides again TURN_BACK_STEPS steps after it comes
 * back */
#define TURN_BACK_STEPS 30
#define TURN_BACK_ANGLE 0x400
/* a player outside the kill areas moves back toward one of its last
 * WALK_BACK_STEPS - 1 places. that place is the most recent more than
 * WALK_BACK_MIN away, or else the earliest. the player moves by its offset from
 * that place once. while it stays outside, it moves by it again, up to
 * WALK_BACK_STEPS more times */
#define WALK_BACK_STEPS 128
#define WALK_BACK_MIN 0.1F

/* a climber on a shape or on top of one stands. so does one climbing down,
 * once the y of its ball's hit.normal reaches HUNTER_STEEP_Y. there one
 * climbing up, or still, climbs over the top instead. each step of the climb
 * over moves the hunter CLIMB_OVER_ACROSS times its height along its own -y. it
 * moves CLIMB_OVER_UP times its height up too. a stand moves the hunter
 * CLIMB_OFF times its height out along the normal's x and z */
#define CLIMB_OFF 0.3F
#define CLIMB_OVER_ACROSS 0.01F
#define CLIMB_OVER_UP 0.03125F
/* the climb plays its sound in ANIM_CLIMB_X, ANIM_CLIMB_MINUS_Z and
 * ANIM_CLIMB_Z. it plays it at the frames from 0 up to CLIMB_SOUND_0_END, and
 * from CLIMB_SOUND_1_START up to CLIMB_SOUND_1_END */
#define CLIMB_SOUND_0_END 1.6F
#define CLIMB_SOUND_1_START 15.0F
#define CLIMB_SOUND_1_END 16.6F

/* PLAYER_MODE_PATH_HANG's speed along its path, either way */
#define PATH_SPEED 0.375F
/* the anim_speed ANIM_SPIN plays at */
#define SPIN_PACE 2.0F

/* the speeds the attacks lunge at: LUNGE_FAST in ANIM_RUN_ATTACK_1 and
 * ANIM_RUN_ATTACK_2, LUNGE_SLOW in ANIM_ATTACK_2, and LUNGE in ANIM_ATTACK_1.
 * PLAYER_MODE_HUNTER_ATTACK_3 closes in on the record's faced task by its
 * distance a step, CLOSE_IN_MAX at most */
#define LUNGE_FAST 3.125F
#define LUNGE_SLOW 0.41666666F
#define LUNGE 0.625F
#define CLOSE_IN_MAX 1.4705882F

/* a swim is at the surface while the record's unk_224 lies less than
 * SURFACE_REACH below it. there ANIM_SWIM plays the stroke's sound at each
 * step whose game_state.steps is a multiple of SWIM_SOUND_STEPS */
#define SURFACE_REACH 2.0F
#define SWIM_SOUND_STEPS 16

/* the sounds of the hunters' moves. a sound named for Knuckles or Rouge is
 * that character's, and Tikal's or Chaos 0's too */
#define SOUND_JUMP_KNUCKLES 0x2000
#define SOUND_JUMP_ROUGE 0x2001
#define SOUND_BRAKE 0x2002
#define SOUND_GLIDE_KNUCKLES 0x2007
#define SOUND_GLIDE_ROUGE 0x2008
#define SOUND_CLIMB 0x2009
#define SOUND_DIG 0x200a
#define SOUND_ATTACK_KNUCKLES 0x200b
#define SOUND_LAST_ATTACK_KNUCKLES 0x200c
#define SOUND_ATTACK_ROUGE 0x200d
#define SOUND_LAST_ATTACK_ROUGE 0x200e
#define SOUND_DIG_JUMP 0x2010
#define SOUND_GLIDING 0x2015 /* each step of a glide below GLIDE_SCRAPE_END */
#define SOUND_DIVE 0x2016
/* the climb's sound and the grab's on STAGE_BOSS_FDOG */
#define SOUND_CLIMB_FDOG 18
#define SOUND_GRAB_FDOG 17
/* the sounds of water */
#define SOUND_LEAVE_WATER 0x7002
#define SOUND_WATER_JUMP 0x7003 /* a jump or a dive in deep water */
#define SOUND_SWIM 0x7004 /* a stroke at the surface */
#define SOUND_WATER_LAND 0x7005 /* a water dive's landing on the ground */
#define SOUND_UNDER_WATER 0x7007 /* each step of a move under the surface */
/* the cries of a Knuckles look and of a Rouge look, which Tikal and Chaos 0
 * don't cry */
#define CRY_JUMP_KNUCKLES 0x3000
#define CRY_JUMP_ROUGE 0x3011
#define CRY_PICK_UP_KNUCKLES 0x3004
#define CRY_PICK_UP_ROUGE 0x3015
#define CRY_THROW_KNUCKLES 0x3005
#define CRY_THROW_ROUGE 0x3016
#define CRY_PUSH_START_KNUCKLES 0x3006
#define CRY_PUSH_START_ROUGE 0x3017
#define CRY_LAST_ATTACK_KNUCKLES 0x3007
#define CRY_LAST_ATTACK_ROUGE 0x3018
#define CRY_SPIRAL_KNUCKLES 0x3008
#define CRY_SPIRAL_ROUGE 0x3019
/* a float's rise out of ANIM_FLOAT_UNDER */
#define CRY_SURFACE_KNUCKLES 0x3009
#define CRY_SURFACE_ROUGE 0x301a
#define CRY_CRASH_KNUCKLES 0x300b
#define CRY_CRASH_ROUGE 0x301c

Float dig_find_chances[DIG_FINDS] = {
    0.9F, 0.2F, 0.12F, 0.08F, 0.05F, 0.03F, 0.02F, 0.02F, 0.02F, 0.01F,
};
Float dig_find_chances_two_players[DIG_FINDS] = {
    0.9F, 0.1F, 0.2F, 0.16F, 0.09F, 0.02F, 0.01F, 0.01F, 0.01F, 0.01F,
};
Float dig_find_chances_rings[DIG_FINDS] = {
    0.9F, 0.18F, 0.0F, 0.0F, 0.0F, 0.03F, 0.02F, 0.02F, 0.02F, 0.01F,
};

/* the hunters' animations, each record's anim.actions. Tikal plays
 * Knuckles's table, and Chaos 0 plays Rouge's. the draws index a table twice
 * for a motion: entry i's motion m, then entry m's motion. entry m of each
 * table has motion m wherever an entry names m. so the second index changes
 * nothing */
#define HUNTER_ANIMS 217
static struct player_action knuckles_anims[HUNTER_ANIMS] = {
#include "player/knuckles/knuckles_anims.inc"
};
static struct player_action rouge_anims[HUNTER_ANIMS] = {
#include "player/knuckles/rouge_anims.inc"
};

/* the hunters' collision shapes. the first is the body's, whose radius each
 * maker sets to the character's, and which hunter_place_body places. the
 * others are spheres at world positions. hunter_set_shapes turns on HIT_SHAPE
 * in an attack, the glide's two in a glide, and SPIRAL_SHAPE in the spiral. the
 * spheres stand firm against every group except pickups, and push items and
 * enemies */
#define HIT_SHAPE 1 /* at a hand, or for Rouge at a foot */
#define GLIDE_FRONT_SHAPE 2
#define GLIDE_BACK_SHAPE 3
#define SPIRAL_SHAPE 4
#define SHAPES 5
static struct collision_shape hunter_shapes[SHAPES] = {
#include "player/knuckles/hunter_shapes.inc"
};

/* the nodes each maker gives player_look_start: Knuckles's for a Knuckles or
 * a Tikal, and Rouge's. Chaos 0 has no look task */
#define LOOK_NODES 3
static NJS_CNK_OBJECT *knuckles_look_nodes[LOOK_NODES];
static NJS_CNK_OBJECT *rouge_look_nodes[LOOK_NODES];

static void tikal_create(int slot);
static void chaos0_create(int slot);
static void hunter_step(struct task *task);
static void hunter_next_mode(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *hunter);
static void hunter_destroy(struct task *task);
static void hunter_place_body(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *hunter);

void knuckles_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *hunter;
    struct player *record;

    if (game_state.two_players != 0 && game_state.alt_character[slot] != 0) {
        tikal_create(slot);
        return;
    }
    hunt_start();
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            hunter_step, "KnucklesTheEchidna")) == NULL) {
        return;
    }
    if ((hunter = syCalloc(1, HUNTER_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = hunter;
    work = task->work;
    /* @hack spelling: record read back from task's extra, where hunter
     * would do */
    record = task->extra.p;
    record->slot = slot;
    record->character = PLAYER_KNUCKLES;
    work->submode = 0;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = knuckles_anims;
    record->anim.next = ANIM_STAND;
    collision_init(task, hunter_shapes, SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tags in hunter_probe_glide and
     * hunter_probe_climb: the create empties the slot and destroys the task
     * where collision_init leaves the work no collision record, as
     * sonic_create's side does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    if (game_state.two_players == 0 || game_state.costume[slot] == 0) {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("KNUCKTEX");
        hunter->by_character.hunter.models = model_file_read("KNUCKMDL.PRS");
    } else if (game_state.costume[slot] == COSTUME_PSO) {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("BKNUCKTEX");
        hunter->by_character.hunter.models = model_file_read("BKNUCKMDL.PRS");
        record->costume = COSTUME_PSO;
    } else if (game_state.costume[slot] == COSTUME_H) {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("HKNUCKTEX");
        hunter->by_character.hunter.models = model_file_read("HKNUCKMDL.PRS");
        record->costume = COSTUME_H;
    } else {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("XKNUCKTEX");
        hunter->by_character.hunter.models = model_file_read("XKNUCKMDL.PRS");
        record->costume = COSTUME_X;
    }
    hunter->by_character.hunter.effects_texlist =
        texture_load_pvm_alloc("K_EFFTEX");
    life_icon_load(record->character);
    hunter->by_character.hunter.motions = motion_file_read("KNUCKMTN.PRS");
    player_anim_step(&record->anim);
    knuckles_look_nodes[0] = model_table[KNUCKLES_HEAD].object;
    knuckles_look_nodes[1] = model_table[KNUCKLES_EYE_0].object;
    knuckles_look_nodes[2] = model_table[KNUCKLES_EYE_1].object;
    player_look_start(slot, knuckles_look_nodes);
    hunter->by_character.hunter.swing =
        jiggle_create(model_table[KNUCKLES_SWING_PARENT].object->child);
    hunter->by_character.hunter.swing->kind = JIGGLE_SWING_UP;
    hunter->by_character.hunter.swing->param = JIGGLE_PARAM;
    hunter->by_character.hunter.swing->by_kind.steps = 0;
    hunter->by_character.hunter.swing->phase_step = JIGGLE_PHASE_STEP;
    hunter->by_character.hunter.swing->shape_a =
        model_table[KNUCKLES_SWING_SHAPE].object;
    hunter->by_character.hunter.swing_last = work->pos;
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = hunter_destroy;
    task->draw = knuckles_draw;
    hunter->by_character.hunter.dig_end.x =
        hunter->by_character.hunter.dig_end.y =
            hunter->by_character.hunter.dig_end.z = 0.0F;
    /* @hack spelling: one store under each of two tests, where one test of
     * either would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS);
    } else if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS);
    } else if (stage_current_id() == STAGE_VERSUS_KNUCKLES && slot == 1) {
        record->upgrades = 0;
    }
}

void rouge_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *hunter;
    struct player *record;

    if (game_state.two_players != 0 && game_state.alt_character[slot] != 0) {
        chaos0_create(slot);
        return;
    }
    hunt_start();
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            hunter_step, "KnucklesTheEchidna")) == NULL) {
        return;
    }
    if ((hunter = syCalloc(1, HUNTER_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = hunter;
    work = task->work;
    /* @hack spelling: record read back from task's extra, where hunter
     * would do */
    record = task->extra.p;
    record->slot = slot;
    record->character = PLAYER_ROUGE;
    work->submode = 0;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = rouge_anims;
    record->anim.next = ANIM_STAND;
    collision_init(task, hunter_shapes, SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tags in hunter_probe_glide and
     * hunter_probe_climb, as knuckles_create's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    if (game_state.two_players == 0 || game_state.costume[slot] == 0) {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("ROUGETEX");
        hunter->by_character.hunter.models = model_file_read("ROUGEMDL.PRS");
    } else if (game_state.costume[slot] == COSTUME_PSO) {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("BROUGETEX");
        hunter->by_character.hunter.models = model_file_read("BROUGEMDL.PRS");
        record->costume = COSTUME_PSO;
    } else if (game_state.costume[slot] == COSTUME_H) {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("HROUGETEX");
        hunter->by_character.hunter.models = model_file_read("HROUGEMDL.PRS");
        record->costume = COSTUME_H;
    } else {
        hunter->by_character.hunter.texlist =
            texture_load_pvm_alloc("XROUGETEX");
        hunter->by_character.hunter.models = model_file_read("XROUGEMDL.PRS");
        record->costume = COSTUME_X;
    }
    hunter->by_character.hunter.effects_texlist =
        texture_load_pvm_alloc("R_EFFTEX");
    life_icon_load(record->character);
    hunter->by_character.hunter.motions = motion_file_read("ROUGEMTN.PRS");
    player_anim_step(&record->anim);
    rouge_look_nodes[0] = model_table[ROUGE_HEAD].object;
    rouge_look_nodes[1] = model_table[ROUGE_EYE_0].object;
    rouge_look_nodes[2] = model_table[ROUGE_EYE_1].object;
    player_look_start(slot, rouge_look_nodes);
    hunter->by_character.hunter.squash =
        jiggle_create(model_table[ROUGE_SQUASH].object);
    hunter->by_character.hunter.squash->kind = JIGGLE_SQUASH;
    hunter->by_character.hunter.squash->param = JIGGLE_PARAM;
    hunter->by_character.hunter.squash->by_kind.angle = SQUASH_ANGLE;
    hunter->by_character.hunter.squash->phase_step = JIGGLE_PHASE_STEP;
    hunter->by_character.hunter.squash->shape_a =
        model_table[ROUGE_SQUASH].object;
    hunter->by_character.hunter.squash_last = work->pos;
    if (record->costume != COSTUME_H) {
        hunter->by_character.hunter.swing =
            jiggle_create(model_table[ROUGE_SWING_PARENT].object->child);
        hunter->by_character.hunter.swing->kind = JIGGLE_SWING_DOWN;
        hunter->by_character.hunter.swing->param = JIGGLE_PARAM;
        hunter->by_character.hunter.swing->by_kind.steps = 0;
        hunter->by_character.hunter.swing->phase_step = JIGGLE_PHASE_STEP;
        hunter->by_character.hunter.swing->shape_a =
            model_table[ROUGE_SWING_PARENT].object->child;
        hunter->by_character.hunter.swing_last = work->pos;
    }
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = hunter_destroy;
    task->draw = rouge_draw;
    hunter->by_character.hunter.part_mode = PLAYER_ANIM_LOOP;
    if (record->costume == COSTUME_PSO) {
        hunter->by_character.hunter.part_motion = PART_MOTION_PSO;
    } else {
        hunter->by_character.hunter.part_motion = PART_MOTION;
    }
    hunter->by_character.hunter.part_frame = 0.0F;
    hunter->by_character.hunter.dig_end.x =
        hunter->by_character.hunter.dig_end.y =
            hunter->by_character.hunter.dig_end.z = 0.0F;
    /* @hack spelling: one store under each of two tests, where one test of
     * either would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_ROUGE_NAILS);
    } else if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_ROUGE_NAILS);
    } else if (stage_current_id() == STAGE_VERSUS_KNUCKLES && slot == 1) {
        record->upgrades = 0;
    }
}

/* make Tikal the player in slot, as knuckles_create makes Knuckles. she
 * takes Tikal's files and her look, PLAYER_KIND_TICAL */
static void tikal_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *hunter;
    struct player *record;

    hunt_start();
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            hunter_step, "KnucklesTheEchidna")) == NULL) {
        return;
    }
    if ((hunter = syCalloc(1, HUNTER_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = hunter;
    work = task->work;
    /* @hack type: record taken as the address of hunter's first field,
     * where hunter would do */
    record = (struct player *)&hunter->slot;
    record->slot = slot;
    record->character = PLAYER_KNUCKLES;
    work->submode = 0;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = knuckles_anims;
    record->anim.next = ANIM_STAND;
    record->kind = PLAYER_KIND_TICAL;
    collision_init(task, hunter_shapes, SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tags in hunter_probe_glide and
     * hunter_probe_climb, as knuckles_create's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    hunter->by_character.hunter.texlist = texture_load_pvm_alloc("TICALTEX");
    hunter->by_character.hunter.models = model_file_read("TICALMDL.PRS");
    hunter->by_character.hunter.effects_texlist =
        texture_load_pvm_alloc("K_EFFTEX");
    life_icon_load(record->kind);
    hunter->by_character.hunter.motions = motion_file_read("TICALMTN.PRS");
    player_anim_step(&record->anim);
    knuckles_look_nodes[0] = model_table[KNUCKLES_HEAD].object;
    knuckles_look_nodes[1] = model_table[KNUCKLES_EYE_0].object;
    knuckles_look_nodes[2] = model_table[KNUCKLES_EYE_1].object;
    player_look_start(slot, knuckles_look_nodes);
    hunter->by_character.hunter.swing =
        jiggle_create(model_table[KNUCKLES_SWING_PARENT].object->child);
    hunter->by_character.hunter.swing->kind = JIGGLE_SWING_UP;
    hunter->by_character.hunter.swing->param = JIGGLE_PARAM;
    hunter->by_character.hunter.swing->by_kind.steps = 0;
    hunter->by_character.hunter.swing->phase_step = JIGGLE_PHASE_STEP;
    hunter->by_character.hunter.swing->shape_a =
        model_table[KNUCKLES_SWING_SHAPE].object;
    hunter->by_character.hunter.swing_last = work->pos;
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = hunter_destroy;
    task->draw = knuckles_draw;
    hunter->by_character.hunter.dig_end.x =
        hunter->by_character.hunter.dig_end.y =
            hunter->by_character.hunter.dig_end.z = 0.0F;
    /* @hack spelling: one store under each of two tests, where one test of
     * either would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS);
    } else if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS);
    } else if (stage_current_id() == STAGE_VERSUS_KNUCKLES && slot == 1) {
        record->upgrades = 0;
    }
}

/* make Chaos 0 the player in slot, as rouge_create makes Rouge. he takes
 * Chaos 0's files and his look, PLAYER_KIND_CHAOS0. he has no look task and
 * no jiggles. his part stays in PLAYER_ANIM_END, which the step doesn't
 * advance */
static void chaos0_create(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *hunter;
    struct player *record;

    hunt_start();
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            hunter_step, "KnucklesTheEchidna")) == NULL) {
        return;
    }
    if ((hunter = syCalloc(1, HUNTER_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = hunter;
    work = task->work;
    /* @hack type: record taken as the address of hunter's first field,
     * where hunter would do */
    record = (struct player *)&hunter->slot;
    record->slot = slot;
    record->character = PLAYER_ROUGE;
    work->submode = 0;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = rouge_anims;
    record->anim.next = ANIM_STAND;
    record->kind = PLAYER_KIND_CHAOS0;
    collision_init(task, hunter_shapes, SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tags in hunter_probe_glide and
     * hunter_probe_climb, as knuckles_create's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    hunter->by_character.hunter.texlist = texture_load_pvm_alloc("CHAOS0TEX");
    hunter->by_character.hunter.models = model_file_read("CHAOS0MDL.PRS");
    hunter->by_character.hunter.effects_texlist =
        texture_load_pvm_alloc("R_EFFTEX");
    life_icon_load(record->kind);
    hunter->by_character.hunter.motions = motion_file_read("CHAOS0MTN.PRS");
    player_anim_step(&record->anim);
    action_window_start(record->slot);
    bubbles_create(record->slot);
    task->destructor = hunter_destroy;
    task->draw = rouge_draw;
    hunter->by_character.hunter.part_mode = PLAYER_ANIM_END;
    hunter->by_character.hunter.part_motion = 0;
    hunter->by_character.hunter.part_frame = 0.0F;
    hunter->by_character.hunter.dig_end.x =
        hunter->by_character.hunter.dig_end.y =
            hunter->by_character.hunter.dig_end.z = 0.0F;
    /* @hack spelling: one store under each of two tests, where one test of
     * either would do */
    if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_ROUGE_NAILS);
    } else if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_ROUGE_NAILS);
    } else if (stage_current_id() == STAGE_VERSUS_KNUCKLES && slot == 1) {
        record->upgrades = 0;
    }
}

/* the task's step. in PLAYER_MODE_MISS and PLAYER_MODE_DROWN it answers only
 * PLAYER_REQUEST_RESET, by standing the player. in GAME_MODE_STAGE_RUN, a
 * player outside the kill areas walks back, except in a glide or its turn
 * back. the player's collision goes unregistered in PLAYER_MODE_EVENT, and
 * while ANIM_WIN or ANIM_LOSE is asked for */
static void hunter_step(struct task *task)
{
    struct task_work *work;
    struct player_motion *motion;
    /* @hack redundant: hunter, a second local for the record, where record
     * alone would do */
    struct player *hunter;
    struct player *record;

    work = task->work;
    motion = task->motion.p;
    hunter = task->extra.p;
    record = hunter;
    player_ground_step(task, work, motion, record);
    if (work->mode == PLAYER_MODE_MISS || work->mode == PLAYER_MODE_DROWN) {
        if (work->submode == PLAYER_REQUEST_RESET) {
            work->flags &= ~PLAYER_RESET_WORK_FLAGS;
            record->flags &= ~PLAYER_RESET_FLAGS;
            player_stand(work, motion, record);
        }
    } else {
        hunter_next_mode(work, motion, record, hunter);
    }
    if (work->flags & PLAYER_WORK_BALL) {
        hunter->by_character.hunter.ball_steps++;
    }
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if ((work->flags & PLAYER_WORK_ON_GROUND) &&
            record->ground_task != NULL && record->ground_task->work != NULL &&
            record->ground_task->work->id == WORK_ID_TURTLE) {
            record->anim.next = ANIM_TURTLE_STAND;
        } else {
            player_stand_anim(work, motion, record);
        }
        break;
    case PLAYER_MODE_RUN:
        if (record->anim.last == ANIM_TURN &&
            record->anim.current != ANIM_TURN) {
            record->spd.x += record->consts.trot_speed;
        }
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record) == PLAYER_CRASHED) {
            work->mode = PLAYER_MODE_CRASH;
            record->anim.next = ANIM_CRASH;
            if (record->kind == PLAYER_KNUCKLES) {
                sound_play_at(CRY_CRASH_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_ROUGE) {
                sound_play_at(CRY_CRASH_ROUGE, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
            vibration_start(record->slot, CRASH_VIBRATION_POWER,
                            CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
            crash_stars_burst(record->slot);
            record->spd.x = -record->spd.x;
            record->spd.z = -record->spd.z;
        } else {
            player_take_speed(work, motion, record);
            player_run_anim(work, motion, record);
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        player_accel_on_path(work, motion, record);
        player_apply_path_speed(work, motion, record);
        player_move_on_path(work, motion, record);
        player_run_anim(work, motion, record);
        break;
    case PLAYER_MODE_PATH_ROLL:
        player_accel_roll(work, motion, record);
        player_apply_path_speed(work, motion, record);
        player_move_on_path(work, motion, record);
        break;
    case PLAYER_MODE_JUMP:
        player_turn_upright(work, motion, record);
        if (!player_steer_to_target(work, motion, record)) {
            player_accel_air(work, motion, record);
        }
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
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
        }
        break;
    case PLAYER_MODE_HUNTER_GLIDE:
        player_turn_upright(work, motion, record);
        if (record->spd.y > 0.0F) {
            player_accel_air(work, motion, record);
            player_apply_accel(work, motion, record);
        } else {
            player_accel_glide(work, motion, record);
            player_apply_glide_accel(work, motion, record);
        }
        switch (hunter_probe_glide(work, motion, record, hunter)) {
        case HUNTER_PROBE_BLOCKED:
            player_take_speed(work, motion, record);
            vibration_start(record->slot, 3, 15, VIBRATION_UNTIMED);
            hunter->by_character.hunter.glide_scrape += GLIDE_SCRAPE;
            break;
        case HUNTER_PROBE_WALL:
            hunter_grab_wall(work, motion, record);
            vibration_start(record->slot, 3, 15, VIBRATION_UNTIMED);
            break;
        case HUNTER_PROBE_FLAT:
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            work->mode = PLAYER_MODE_BRAKE;
            record->anim.next = ANIM_GLIDE_LAND;
            break;
        default:
            player_collide(work, motion, record);
            player_take_speed(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_HUNTER_TURN_BACK:
        player_turn_upright(work, motion, record);
        if (hunter->by_character.hunter.turn_steps == TURN_BACK_STEPS) {
            work->ang.y += TURN_BACK_ANGLE;
        }
        if (motion->spd.y > 0.0F) {
            player_apply_accel(work, motion, record);
        } else {
            player_apply_glide_accel(work, motion, record);
        }
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_UNK_0E:
        if (record->anim.next == ANIM_PUSH_START) {
            player_accel_brake(work, motion, record);
        } else {
            player_accel_push(work, motion, record);
            if (player_action_flags(work, record) & ACTION_FLAG_400) {
                fun_8c05bce0();
            }
            if (record->anim.last == ANIM_PUSH_START) {
                if (record->kind == PLAYER_KNUCKLES) {
                    sound_play_at(CRY_PUSH_START_KNUCKLES, NULL, 0,
                                  SOUND_LOUDEST, &work->pos);
                } else if (record->kind == PLAYER_ROUGE) {
                    sound_play_at(CRY_PUSH_START_ROUGE, NULL, 0, SOUND_LOUDEST,
                                  &work->pos);
                }
            }
        }
        player_apply_push(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HURT:
        player_turn_upright(work, motion, record);
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HANG:
        break;
    case PLAYER_MODE_PLAY_ANIM:
        break;
    case PLAYER_MODE_CRASH:
        if (record->anim.next == ANIM_KNOCKED_DOWN_END) {
            record->anim.after = ANIM_STAND;
        }
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
    case PLAYER_MODE_HUNTER_DIVE:
        sound_start_for(SOUND_DIVE, work, 0, 0, 30);
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        hunter_spin_afterimage(work, record, hunter);
        break;
    case PLAYER_MODE_HUNTER_DIG:
    case PLAYER_MODE_HUNTER_DIG_OUT:
    case PLAYER_MODE_HUNTER_WALL_DIG:
    case PLAYER_MODE_HUNTER_WALL_DIG_OUT:
        break;
    case PLAYER_MODE_HUNTER_DIG_FAIL:
    case PLAYER_MODE_HUNTER_WALL_DIG_FAIL:
        break;
    case PLAYER_MODE_HUNTER_CLIMB:
        if (record->anim.current != ANIM_GRAB_WALL &&
            record->anim.current != ANIM_CLIMB_OVER) {
            player_accel_climb(work, motion, record);
            player_apply_climb_accel(work, motion, record);
            player_collide_climb(work, motion, record);
        }
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HUNTER_CLIMB_OVER:
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        break;
    case PLAYER_MODE_HUNTER_ATTACK_1:
    case PLAYER_MODE_HUNTER_ATTACK_2:
    case PLAYER_MODE_HUNTER_RUN_ATTACK_1:
    case PLAYER_MODE_HUNTER_RUN_ATTACK_2: {
        NJS_POINT3 *pos;

        player_turn_to_landed_target(work, motion, record);
        player_accel_gravity(work, motion, record);
        /* @hack spelling: one store under each of two tests, where one test
         * of either would do */
        if (record->anim.next == ANIM_RUN_ATTACK_2) {
            record->spd.x = LUNGE_FAST;
        } else if (record->anim.next == ANIM_RUN_ATTACK_1) {
            record->spd.x = LUNGE_FAST;
        } else if (record->anim.next == ANIM_ATTACK_2) {
            record->spd.x = LUNGE_SLOW;
        } else {
            record->spd.x = LUNGE;
        }
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record)) {
            player_get_past(record->slot, 1, &work->pos, NULL);
            player_stop(motion, record);
        }
        player_take_speed(work, motion, record);
        /* the tests below set pos in each of the case's four modes. it stays
         * unset only where a call above changes the work's mode */
#ifdef NONMATCHING
        /* for GCC, which can't see that */
        pos = &record->unk_1e8;
#endif
        /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
        if (record->upgrades & (UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES) |
                                UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) {
            if (record->character == PLAYER_KNUCKLES) {
                /* @hack spelling: one store under each of three tests, where
                 * one test of the three would do */
                if (work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1) {
                    pos = &record->unk_1e8;
                } else if (work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2) {
                    pos = &record->unk_1e8;
                } else if (work->mode == PLAYER_MODE_HUNTER_ATTACK_1) {
                    pos = &record->unk_1e8;
                } else if (work->mode == PLAYER_MODE_HUNTER_ATTACK_2) {
                    pos = &record->unk_1f4;
                }
            } else {
                /* @hack spelling: one store under each of four tests, where
                 * one test for each place would do */
                if (work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_1) {
                    pos = &record->unk_20c;
                } else if (work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_2) {
                    pos = &record->unk_200;
                } else if (work->mode == PLAYER_MODE_HUNTER_ATTACK_1) {
                    pos = &record->unk_20c;
                } else if (work->mode == PLAYER_MODE_HUNTER_ATTACK_2) {
                    pos = &record->unk_200;
                }
            }
            hunter_puff_start(record->slot, pos);
        }
        /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    } break;
    case PLAYER_MODE_HUNTER_ATTACK_3:
        player_turn_to_target_ahead(work, motion, record);
        if (record->faced == NULL) {
            player_accel_air(work, motion, record);
        } else {
            Float dx;
            Float dz;
            Float distance;
            struct task_work *target;

            target = record->faced->work;
            dx = work->pos.x - target->pos.x;
            dz = work->pos.z - target->pos.z;
            distance = njSqrt((dx * dx) + (dz * dz));
            if (distance > CLOSE_IN_MAX) {
                distance = CLOSE_IN_MAX;
            }
            record->spd.x = distance;
            player_accel_gravity(work, motion, record);
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->upgrades & (UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES) |
                                UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) {
            if (record->character == PLAYER_KNUCKLES) {
                hunter_puff_start(record->slot, &record->unk_1e8);
                hunter_puff_start(record->slot, &record->unk_1f4);
            } else {
                hunter_puff_start(record->slot, &record->unk_200);
                hunter_puff_start(record->slot, &record->unk_20c);
            }
        }
        break;
    case PLAYER_MODE_HUNTER_RUN_ATTACK_3:
        player_turn_to_landed_target(work, motion, record);
        player_accel_gravity(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->upgrades & (UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES) |
                                UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) {
            if (record->character == PLAYER_KNUCKLES) {
                hunter_puff_start(record->slot, &record->unk_1e8);
                hunter_puff_start(record->slot, &record->unk_1f4);
            } else {
                hunter_puff_start(record->slot, &record->unk_200);
                hunter_puff_start(record->slot, &record->unk_20c);
            }
        }
        break;
    case PLAYER_MODE_HUNTER_SPIRAL:
        player_accel_gravity(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        hunter_afterimage_slow(work, record, hunter);
        break;
    case PLAYER_MODE_PATH_HANG:
    case PLAYER_MODE_PATH_HANG_GO:
        player_shift_on_path(work, motion, record);
        hunter_climb_step(work, motion, record);
        break;
    case PLAYER_MODE_HUNTER_FLOAT:
        hunter_float(work, motion, record);
        player_turn_upright(work, motion, record);
        player_turn_to_stick(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->ground.above_y < record->unk_224.y) {
            if (record->anim.current == ANIM_FLOAT_UNDER &&
                record->anim.next != ANIM_FLOAT) {
                if (record->kind == PLAYER_KNUCKLES) {
                    sound_play(CRY_SURFACE_KNUCKLES, NULL, 0, 0);
                } else if (record->kind == PLAYER_ROUGE) {
                    sound_play(CRY_SURFACE_ROUGE, NULL, 0, 0);
                }
                vibration_start(record->slot, 6, 15, VIBRATION_UNTIMED);
            }
            record->anim.next = ANIM_FLOAT;
        } else if (record->anim.next != ANIM_FLOAT_UNDER_START &&
                   record->anim.next != ANIM_FLOAT_UNDER) {
            record->anim.next = ANIM_FLOAT_UNDER_START;
        }
        break;
    case PLAYER_MODE_HUNTER_SWIM:
        hunter_float(work, motion, record);
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->ground.above_y < record->unk_224.y + SURFACE_REACH) {
            if (record->anim.current == ANIM_SWIM) {
                if (!(game_state.steps & (SWIM_SOUND_STEPS - 1))) {
                    sound_play_at_for(SOUND_SWIM, work, 1, 40, 8, &work->pos);
                }
            }
        } else {
            sound_play_at_for(SOUND_UNDER_WATER, work, 1, SOUND_LOUDEST, 4,
                              &work->pos);
        }
        break;
    case PLAYER_MODE_HUNTER_WATER_DIVE:
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        if (record->ground.above_y > record->unk_224.y) {
            sound_play_at_for(SOUND_UNDER_WATER, work, 1, SOUND_LOUDEST, 4,
                              &work->pos);
        }
        break;
    case PLAYER_MODE_PICK_UP:
        player_turn_to_stick(work, motion, record);
        player_accel_ground(work, motion, record);
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
    case PLAYER_MODE_ROUGE_RIDE_FERRY:
        break;
    case PLAYER_MODE_ROUGE_RIDE_SWIMMER:
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
            player_accel_air(work, motion, record);
        }
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
    case PLAYER_MODE_MISS:
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
    if (work->mode != PLAYER_MODE_HUNTER_GLIDE &&
        work->mode != PLAYER_MODE_HUNTER_TURN_BACK &&
        game_state.mode == GAME_MODE_STAGE_RUN &&
        hunter_outside_kill_areas(work, record) == 1) {
        Uint8 back;
        Float dx;
        Float dz;
        NJS_POINT3 past;

        for (back = 1; back < WALK_BACK_STEPS; back++) {
            /* @hack type: back passed as a Sint8, where back would do */
            player_get_past(record->slot, (Sint8)back, &past, NULL);
            dx = past.x - work->pos.x;
            dz = past.z - work->pos.z;
            if ((dx * dx) + (dz * dz) > WALK_BACK_MIN * WALK_BACK_MIN) {
                break;
            }
        }
        back = WALK_BACK_STEPS;
        work->pos.x += dx;
        work->pos.z += dz;
        while (back > 0 && hunter_outside_kill_areas(work, record) == 1) {
            back--;
            work->pos.x += dx;
            work->pos.z += dz;
        }
    }
    player_anim_step(&record->anim);
    hunter_place_body(work, motion, record, hunter);
    if (hunter->by_character.hunter.squash != NULL &&
        record->kind == PLAYER_ROUGE) {
        Float moved;
        NJS_VECTOR step;

        step = record->unk_230;
        njSubVector(&step, &hunter->by_character.hunter.squash_last);
        moved = njScalor(&step);
        hunter->by_character.hunter.squash_last = record->unk_230;
        if (njAbs(moved) > njAbs(hunter->by_character.hunter.squash->param)) {
            hunter->by_character.hunter.squash->param = moved;
        }
        jiggle_step(hunter->by_character.hunter.squash);
    }
    if (hunter->by_character.hunter.swing != NULL) {
        if (record->character == PLAYER_KNUCKLES) {
            Float moved;
            NJS_VECTOR step;

            step = record->unk_218;
            njSubVector(&step, &hunter->by_character.hunter.swing_last);
            moved = njScalor(&step);
            hunter->by_character.hunter.swing_last = record->unk_218;
            if (work->mode == PLAYER_MODE_HUNTER_GLIDE) {
                hunter->by_character.hunter.swing->kind = JIGGLE_FLUTTER_UP;
            } else {
                hunter->by_character.hunter.swing->kind = JIGGLE_SWING_UP;
                if (njAbs(moved) >
                    njAbs(hunter->by_character.hunter.swing->param)) {
                    hunter->by_character.hunter.swing->param = moved;
                }
                if (njAbs(moved) > PLAYER_SWING_KICK) {
                    hunter->by_character.hunter.swing->phase = QUARTER_TURN;
                }
            }
            jiggle_step(hunter->by_character.hunter.swing);
        } else if (record->kind == PLAYER_ROUGE) {
            Float moved;
            NJS_VECTOR step;

            step = record->unk_218;
            njSubVector(&step, &hunter->by_character.hunter.swing_last);
            moved = njScalor(&step);
            hunter->by_character.hunter.swing_last = record->unk_218;
            if (njAbs(moved) >
                njAbs(hunter->by_character.hunter.swing->param)) {
                hunter->by_character.hunter.swing->param = moved;
            }
            if (njAbs(moved) > PLAYER_SWING_KICK) {
                hunter->by_character.hunter.swing->phase = QUARTER_TURN;
            }
            jiggle_step(hunter->by_character.hunter.swing);
        }
    }
    if (record->kind == PLAYER_ROUGE) {
        Sint32 part_motion;

        switch (record->anim.current) {
        case ANIM_GLIDE:
        case ANIM_GLIDE_LAND:
        case ANIM_UNK_179:
        case ANIM_UNK_180:
        case ANIM_LAND:
        case ANIM_LAND_FAST:
        case ANIM_FALL:
        case ANIM_JUMP_FALL:
        case ANIM_FALL_JUMP:
            part_motion = PART_MOTION_FAST;
            break;
        default:
            if (record->costume == COSTUME_PSO) {
                part_motion = PART_MOTION_PSO;
            } else {
                part_motion = PART_MOTION;
            }
            break;
        }
        switch (hunter->by_character.hunter.part_mode) {
        case PLAYER_ANIM_LOOP:
            if (hunter->by_character.hunter.part_motion != part_motion) {
                hunter->by_character.hunter.part_mode = PLAYER_ANIM_BLEND;
                hunter->by_character.hunter.part_motion_link.motion[0] =
                    motion_table[hunter->by_character.hunter.part_motion]
                        .motion;
                hunter->by_character.hunter.part_motion_link.frame[0] =
                    hunter->by_character.hunter.part_frame;
                hunter->by_character.hunter.part_motion_link.motion[1] =
                    motion_table[part_motion].motion;
                hunter->by_character.hunter.part_motion_link.frame[1] = 0.0F;
                hunter->by_character.hunter.part_shape_link.motion[0] =
                    motion_table[hunter->by_character.hunter.part_motion + 1]
                        .motion;
                hunter->by_character.hunter.part_shape_link.frame[0] =
                    hunter->by_character.hunter.part_frame;
                hunter->by_character.hunter.part_shape_link.motion[1] =
                    motion_table[part_motion + 1].motion;
                hunter->by_character.hunter.part_shape_link.frame[1] = 0.0F;
                hunter->by_character.hunter.part_frame = 0.0F;
                hunter->by_character.hunter.part_motion = part_motion;
            } else {
                if (part_motion == PART_MOTION_FAST) {
                    hunter->by_character.hunter.part_frame += PART_STEP_FAST;
                } else {
                    hunter->by_character.hunter.part_frame += PART_STEP;
                }
                while (hunter->by_character.hunter.part_frame >=
                       (Float)motion_table[part_motion].motion->nbFrame) {
                    hunter->by_character.hunter.part_frame -=
                        (Float)motion_table[part_motion].motion->nbFrame;
                }
            }
            break;
        case PLAYER_ANIM_BLEND:
            hunter->by_character.hunter.part_frame += PART_STEP;
            if (hunter->by_character.hunter.part_frame >= 1.0F) {
                hunter->by_character.hunter.part_mode = PLAYER_ANIM_LOOP;
                hunter->by_character.hunter.part_frame = 0.0F;
            }
            break;
        }
    }
    /* player_free_move_forced returns 0, so no step takes
     * PLAYER_MODE_FREE_MOVE here */
    if (player_free_move_forced(record->slot) &&
        work->mode != PLAYER_MODE_FREE_MOVE) {
        work->mode = PLAYER_MODE_FREE_MOVE;
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_REQUEST | PLAYER_WORK_ON_PATH);
    }
    hunter_drop_held(work, motion, record);
    surface_probe_player(work, record);
    hunter_set_shapes(work, record);
    if (record->anim.next != ANIM_WIN && record->anim.next != ANIM_LOSE &&
        work->mode != PLAYER_MODE_EVENT) {
        collision_register(task);
    }
    player_finish_step(work, motion, record);
}

/* choose the player's mode: take a hit, make the offers, then run the
 * mode's tests until one holds. PLAYER_INVINCIBLE sets the work's timer to 0,
 * and no hit is tried. otherwise a timer of 0 lets player_take_hit try one.
 * a hit that counts lets go of the held task and takes the barriers. a timer
 * of TIMER_HELD stays as it is. under any other timer the work loses
 * WORK_HURT, and the timer counts down while game_state.solo_player is 0 */
/* @hack redundant: hunter, the record again, where record alone would do */
/* NOLINTNEXTLINE(readability-function-size) */
static void hunter_next_mode(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *hunter)
{
    /* @bug memory: reads through a null record, which passes the test of
     * PLAYER_INVINCIBLE (BT at 0x8c07adf2). at a timer of 0, player_take_hit
     * reads it (JSR at 0x8c07ae1e). at any timer, rouge_offer_actions reads it
     * (JSR at 0x8c07aef8) */
    if (record != NULL && (record->flags & PLAYER_INVINCIBLE)) {
        work->flags |= PLAYER_WORK_UNK_8000;
        work->timer = 0;
    } else if (work->timer == 0) {
        if (player_take_hit(work, record)) {
            work->flags |= PLAYER_WORK_UNK_8000;
            if (work->flags & PLAYER_WORK_HOLDING) {
                player_release_held(work, motion, record);
            }
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_HOLDING |
                             PLAYER_WORK_REQUEST | PLAYER_WORK_ON_PATH);
            /* @hack redundant: the empty else */
            if (record->flags & (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER)) {
                record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
            } else {
            }
        } else {
            work->flags &= ~PLAYER_WORK_UNK_8000;
        }
    } else if (work->timer == TIMER_HELD) {
        /* @hack redundant: the empty arm, where a test of its own in the
         * else would do */
    } else {
        if (!game_state.solo_player) {
            work->timer--;
        }
        work->flags &= ~WORK_HURT;
    }
    hunter_offer_ring_attack(work, record);
    rouge_offer_actions(work, record);
    player_offer_flute(work, record);
    player_offer_hint(work, record);
    player_offer_target_actions(work, motion, record);
    player_offer_held_actions(work, record);
    hunter_offer_attack(work, record, hunter);
    hunter_offer_dig(work, motion, record, hunter);
    hunter_offer_dive(work, motion, record);
    player_offer_release(work, record);
    player_offer_call(work, record);
    hunter_offer_sunglasses(work, record);
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
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
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (player_test_stand(work, motion, record)) {
            break;
        }
        if (player_last_side_touch(work, record) != NULL) {
            /* @hack spelling: a switch, where a test of the grade for
             * PLAYER_WALL_HEAD_ON would do */
            switch (player_face_offer(work, motion, record)) {
            case PLAYER_WALL_HEAD_ON:
                player_start_push(work, motion, record);
            }
            break;
        }
        if (player_stick_back(work, record) &&
            record->spd.x >= record->consts.trot_speed) {
            work->mode = PLAYER_MODE_BRAKE;
            record->anim.next = ANIM_BRAKE;
            record->anim.after = ANIM_STAND;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            break;
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_take_action(work, motion, record, hunter) ||
            hunter_test_jump(work, motion, record)) {
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
    case PLAYER_MODE_PATH_ROLL:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            record->anim.next = ANIM_SPIN;
            record->anim_speed = SPIN_PACE;
            work->ang.x = work->ang.z = 0;
            work->flags |= PLAYER_WORK_BALL | PLAYER_WORK_ATTACK;
            work->flags &= ~PLAYER_WORK_ON_PATH;
            hunter->by_character.hunter.steps = 0;
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
    case PLAYER_MODE_JUMP:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_bump(work, motion, record)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
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
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            break;
        }
        if (record->anim.next != ANIM_GLIDE_JUMP &&
            !hunter_in_deep_water(work, motion, record) &&
            player_jump_pressed(work, record)) {
            work->mode = PLAYER_MODE_HUNTER_GLIDE;
            record->anim.next = ANIM_GLIDE;
            hunter_start_glide(record, hunter);
            work->flags |= PLAYER_WORK_ATTACK;
            hunter->by_character.hunter.glide_scrape = 0;
            break;
        }
        if (player_count_jump_hold(work, record)) {
            record->spd.y += record->consts.jump_lift * PLAYER_JUMP_LIFT;
            break;
        }
        if ((record->anim.next == ANIM_JUMP ||
             record->anim.next == ANIM_JUMP_RISE) &&
            record->spd.y <= 0.0F) {
            record->anim.next = ANIM_JUMP_FALL;
        } else if (record->spd.y <= 0.0F &&
                   record->anim.next == ANIM_DIG_JUMP) {
            record->anim.next = ANIM_DIG_JUMP_FALL;
        }
        if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            break;
        }
        if (motion->spd.y > 0.0F) {
            break;
        }
        if (hunter_probe_climb(work, motion, record) == HUNTER_PROBE_WALL) {
            work->flags &= ~PLAYER_WORK_BALL;
            hunter_grab_wall(work, motion, record);
        }
        break;
    case PLAYER_MODE_BOUNCE:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_LAUNCH:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            if (record->anim.next == ANIM_DIG_JUMP) {
                record->anim.next = ANIM_DIG_JUMP_FALL;
            } else {
                record->anim.next = ANIM_FALL_START;
            }
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_FALL:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
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
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            break;
        }
        if (pad_jump_pressed[record->slot]) {
            hunter_jump(work, motion, record);
            break;
        }
        if (pad_jump_held[record->slot] &&
            hunter->by_character.hunter.steps++ > GLIDE_HOLD) {
            work->mode = PLAYER_MODE_HUNTER_GLIDE;
            record->anim.next = ANIM_GLIDE;
            hunter_start_glide(record, hunter);
            work->flags |= PLAYER_WORK_ATTACK;
            hunter->by_character.hunter.glide_scrape = 0;
        }
        break;
    case PLAYER_MODE_BRAKE:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_BRAKE) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_TEETER:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
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
                hunter->by_character.hunter.steps = 0;
            }
        }
        break;
    case PLAYER_MODE_HUNTER_GLIDE:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_outside_kill_areas(work, record)) {
            motion->aim_y = work->ang.y + HALF_TURN;
            hunter->by_character.hunter.turn_steps = TURN_BACK_STEPS;
            work->mode = PLAYER_MODE_HUNTER_TURN_BACK;
            break;
        }
        if (hunter_test_bump(work, motion, record)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        {
            struct task *object;

            collision_restart_scans();
            while ((object = collision_next_object(
                        player_tasks[record->slot])) != NULL) {
                if (object->work->collision->shapes->flags &
                    SHAPE_HURT_NO_PUSH) {
                    work->flags &= ~PLAYER_WORK_ATTACK;
                    work->mode = PLAYER_MODE_FALL;
                    record->anim.next = ANIM_FALL;
                    hunter->by_character.hunter.steps = 0;
                    /* @bug logic: runs the glide's tests below on the fall.
                     * the break leaves the scan, not the glide's case: the
                     * BRA at 0x8c07bcd8 goes on to 0x8c07bd34, where the
                     * case's other falls branch to its end at 0x8c07e854.
                     * so a player on the ground lands. one that lets go of
                     * jump jumps as from a fall. any other plays
                     * SOUND_GLIDING below GLIDE_SCRAPE_END */
                    break;
                }
            }
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_GLIDE_LAND;
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                record->anim.next = ANIM_GLIDE_LAND;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            break;
        }
        if (!pad_jump_held[record->slot]) {
            hunter_jump(work, motion, record);
            break;
        }
        /* @hack redundant: the else after the break */
        if (hunter->by_character.hunter.glide_scrape >= GLIDE_SCRAPE_END) {
            work->flags &= ~PLAYER_WORK_ATTACK;
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            hunter->by_character.hunter.steps = 0;
            break;
        } else {
            if (--hunter->by_character.hunter.glide_scrape < 0) {
                hunter->by_character.hunter.glide_scrape = 0;
            }
            sound_play_for(SOUND_GLIDING, work, 1, 0, 2);
            break;
        }
    case PLAYER_MODE_HUNTER_TURN_BACK:
        if (!hunter_outside_kill_areas(work, record)) {
            if (--hunter->by_character.hunter.turn_steps <= 0) {
                work->mode = PLAYER_MODE_HUNTER_GLIDE;
                hunter->by_character.hunter.glide_scrape = 0;
            }
        } else if (hunter->by_character.hunter.turn_steps != TURN_BACK_STEPS) {
            hunter->by_character.hunter.turn_steps = TURN_BACK_STEPS;
        }
        break;
    case PLAYER_MODE_UNK_0E:
        if (hunter_take_request(work, motion, record, hunter)) {
            sound_stop(PLAYER_SOUND_PUSH, work);
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            sound_stop(PLAYER_SOUND_PUSH, work);
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            sound_stop(PLAYER_SOUND_PUSH, work);
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            sound_stop(PLAYER_SOUND_PUSH, work);
            break;
        }
        if (player_test_stand(work, motion, record)) {
            sound_stop(PLAYER_SOUND_PUSH, work);
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
                    sound_stop(PLAYER_SOUND_PUSH, work);
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
                sound_stop(PLAYER_SOUND_PUSH, work);
                break;
            }
            break;
        }
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_RUN;
        record->anim.next = ANIM_WALK;
        sound_stop(PLAYER_SOUND_PUSH, work);
        break;
    case PLAYER_MODE_HURT:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (work->timer == PLAYER_HIT_STEPS) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                record->anim.next = ANIM_HURT_LAND;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            break;
        }
        if (!pad_jump_pressed[record->slot]) {
            break;
        }
        hunter_jump(work, motion, record);
        break;
    case PLAYER_MODE_HANG:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_PLAY_ANIM:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (hunter_take_held_request(work, motion, record, hunter)) {
                break;
            }
        } else if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        break;
    case PLAYER_MODE_CRASH:
        if (record->anim.next == ANIM_CRASH ||
            record->anim.next == ANIM_KNOCKED_DOWN) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (record->anim.next != ANIM_CRASH_END &&
            record->anim.next != ANIM_KNOCKED_DOWN_END) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_HUNTER_DIVE:
        if (hunter_take_request(work, motion, record, hunter) ||
            hunter_take_action(work, motion, record, hunter)) {
            sound_stop(SOUND_DIVE, work);
            break;
        }
        if (stage_current_id() != STAGE_CHAO &&
            hunter_in_deep_water(work, motion, record)) {
            work->mode = PLAYER_MODE_HUNTER_FLOAT;
            record->anim.next = ANIM_FLOAT;
            work->flags &= ~PLAYER_WORK_ATTACK;
            sound_stop(SOUND_DIVE, work);
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            player_stand(work, motion, record);
            work->flags &= ~PLAYER_WORK_ATTACK;
            sound_stop(SOUND_DIVE, work);
            if (record->kind != PLAYER_ROUGE) {
                record->anim.next = ANIM_DIVE_LAND;
            }
            break;
        }
        if (record->anim.next == ANIM_GLIDE_JUMP) {
            break;
        }
        /* @hack spelling: the break inside the test, where a break after
         * it would do */
        if (player_jump_pressed(work, record)) {
            work->mode = PLAYER_MODE_HUNTER_GLIDE;
            record->anim.next = ANIM_GLIDE;
            hunter_start_glide(record, hunter);
            work->flags |= PLAYER_WORK_ATTACK;
            hunter->by_character.hunter.glide_scrape = 0;
            sound_stop(SOUND_DIVE, work);
            break;
        }
        break;
    case PLAYER_MODE_HUNTER_DIG:
    case PLAYER_MODE_HUNTER_WALL_DIG:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        hunter->by_character.hunter.dig_steps++;
        if (hunter->by_character.hunter.dig_steps > DIG_START &&
            !(hunter->by_character.hunter.dig_steps & (DIG_DEBRIS_STEPS - 1))) {
            if (player_hooks.unk_04 != NULL) {
                Angle ang;
                NJS_VECTOR speed;

                for (ang = (Angle)((Float)rand() * DEBRIS_RING_START);
                     ang < FULL_TURN; ang += DEBRIS_RING_STEP) {
                    speed.x = (Float)rand() * njSin(ang) * RAND_SCALE;
                    speed.y = (Float)rand() * RAND_SCALE;
                    speed.z = (Float)rand() * njCos(ang) * RAND_SCALE;
                    player_hooks.unk_04(&work->pos, &speed, 1.0F);
                }
                speed.x = speed.z = 0.0F;
                speed.y = 1.0F;
                turn_to_world(work, &speed);
                player_hooks.unk_04(&work->pos, &speed, 1.0F);
            }
        }
        if ((record->anim.next == ANIM_DIG ||
             record->anim.next == ANIM_DIG_WALL) &&
            record->anim.frame < DIG_SINK_FRAME) {
            break;
        }
        hunter->by_character.hunter.dig_depth -= DIG_SINK;
        if (hunter->by_character.hunter.dig_depth < DIG_DEPTH) {
            hunter_dig_find(work, motion, record, hunter);
            if (work->mode == PLAYER_MODE_HUNTER_DIG) {
                work->mode = PLAYER_MODE_HUNTER_DIG_OUT;
            } else {
                work->mode = PLAYER_MODE_HUNTER_WALL_DIG_OUT;
            }
        }
        if (!(hunter->by_character.hunter.dig_steps & (DIG_DEBRIS_STEPS - 1))) {
            /* @hack redundant: the empty arm, where a test for any other
             * stage would do */
            if (stage_current_id() == STAGE_SECURITY_HALL) {
            } else {
                sound_play(SOUND_DIG, NULL, 0, 0);
            }
            vibration_start(record->slot, 2, 15, VIBRATION_UNTIMED);
        }
        break;
    case PLAYER_MODE_HUNTER_DIG_OUT:
    case PLAYER_MODE_HUNTER_WALL_DIG_OUT:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (work->mode == PLAYER_MODE_HUNTER_DIG_OUT) {
            hunter->by_character.hunter.dig_depth += DIG_RISE;
        } else {
            if (record->anim.next == ANIM_DIG_WALL_LOOP &&
                hunter->by_character.hunter.dig_depth > WALL_DIG_END_DEPTH) {
                record->anim.next = ANIM_DIG_WALL_END;
            }
            hunter->by_character.hunter.dig_depth += WALL_DIG_RISE;
        }
        if (hunter->by_character.hunter.dig_depth >= 0.0F) {
            hunter->by_character.hunter.dig_end = work->pos;
            if (work->mode == PLAYER_MODE_HUNTER_DIG_OUT) {
                record->spd.y = DIG_JUMP_SPEED;
                record->jump_steps = record->consts.jump_hold_steps;
                record->anim.next = ANIM_DIG_JUMP;
                work->mode = PLAYER_MODE_JUMP;
                sound_play(SOUND_DIG_JUMP, NULL, 0, 0);
                break;
            } else {
                work->mode = PLAYER_MODE_HUNTER_CLIMB;
                break;
            }
        }
        if (work->mode == PLAYER_MODE_HUNTER_DIG_OUT) {
            Angle ang;
            NJS_VECTOR speed;

            if (!(++hunter->by_character.hunter.dig_steps &
                  (DIG_DEBRIS_STEPS - 1)) &&
                player_hooks.unk_04 != NULL) {
                for (ang = (Angle)((Float)rand() * DEBRIS_RING_START);
                     ang < FULL_TURN; ang += DEBRIS_RING_STEP) {
                    speed.x = (Float)rand() * njSin(ang) * RAND_SCALE;
                    speed.y = (Float)rand() * (DEBRIS_OUT_RISE * RAND_SCALE);
                    speed.z = (Float)rand() * njCos(ang) * RAND_SCALE;
                    player_hooks.unk_04(&work->pos, &speed, 1.0F);
                }
                speed.x = speed.z = 0.0F;
                speed.y = DEBRIS_OUT_UP;
                turn_to_world(work, &speed);
                player_hooks.unk_04(&work->pos, &speed, 1.0F);
            }
            break;
        } else {
            Angle ang;
            NJS_VECTOR speed;

            if (!(++hunter->by_character.hunter.dig_steps &
                  (DIG_DEBRIS_STEPS - 1)) &&
                player_hooks.unk_04 != NULL) {
                for (ang = (Angle)((Float)rand() * DEBRIS_RING_START);
                     ang < FULL_TURN; ang += DEBRIS_RING_STEP) {
                    speed.x = (Float)rand() * njSin(ang) * RAND_SCALE;
                    speed.y = (Float)rand() * RAND_SCALE;
                    speed.z = (Float)rand() * njCos(ang) * RAND_SCALE;
                    player_hooks.unk_04(&work->pos, &speed, 1.0F);
                }
                speed.x = speed.z = 0.0F;
                speed.y = 1.0F;
                turn_to_world(work, &speed);
                player_hooks.unk_04(&work->pos, &speed, 1.0F);
            }
        }
        break;
    case PLAYER_MODE_HUNTER_DIG_FAIL:
    case PLAYER_MODE_HUNTER_WALL_DIG_FAIL:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (record->anim.next == ANIM_DIG_FAIL ||
            record->anim.next == ANIM_DIG_WALL_FAIL ||
            record->anim.next == ANIM_KNUCKLES_EARTHQUAKE ||
            record->anim.next == ANIM_ROUGE_EARTHQUAKE) {
            break;
        }
        if (work->mode == PLAYER_MODE_HUNTER_DIG_FAIL) {
            if (hunter_test_jump(work, motion, record)) {
                break;
            }
            if (player_offer_target_actions(work, motion, record)) {
                break;
            }
            if (player_read_stick(work, record, NULL, NULL) ||
                record->spd.x != 0.0F) {
                work->mode = PLAYER_MODE_RUN;
                record->anim.next = ANIM_WALK;
                break;
            }
            work->mode = PLAYER_MODE_STAND;
            break;
        } else {
            work->mode = PLAYER_MODE_HUNTER_CLIMB;
        }
        break;
    case PLAYER_MODE_HUNTER_CLIMB:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (work->flags & (PLAYER_WORK_ON_SHAPE | WORK_ON_TOP)) {
            record->spd.x = record->spd.y = record->spd.z = motion->spd.x =
                motion->spd.y = motion->spd.z = 0.0F;
            work->pos.x +=
                record->consts.height * record->ball->hit.normal.x * CLIMB_OFF;
            work->pos.z +=
                record->consts.height * record->ball->hit.normal.z * CLIMB_OFF;
            player_stand(work, motion, record);
            break;
        }
        if (motion->spd.y >= 0.0F) {
            if (record->ball->hit.normal.y >= HUNTER_STEEP_Y) {
                NJS_VECTOR dir;

                dir.x = dir.z = 0.0F;
                dir.y = -1.0F;
                turn_to_world(work, &dir);
                motion->spd.x =
                    dir.x * record->consts.height * CLIMB_OVER_ACROSS;
                motion->spd.z =
                    dir.z * record->consts.height * CLIMB_OVER_ACROSS;
                motion->spd.y = record->consts.height * CLIMB_OVER_UP;
                work->mode = PLAYER_MODE_HUNTER_CLIMB_OVER;
                record->anim.next = ANIM_CLIMB_OVER;
                break;
            }
        } else if (record->ball->hit.normal.y >= HUNTER_STEEP_Y) {
            record->spd.x = record->spd.y = record->spd.z = motion->spd.x =
                motion->spd.y = motion->spd.z = 0.0F;
            work->pos.x +=
                record->consts.height * record->ball->hit.normal.x * CLIMB_OFF;
            work->pos.z +=
                record->consts.height * record->ball->hit.normal.z * CLIMB_OFF;
            player_stand(work, motion, record);
            break;
        }
        if (hunter_test_wall_jump(work, motion, record)) {
            break;
        }
        if (!hunter_probe_steep(work, motion, record)) {
            if (--hunter->by_character.hunter.climb_grace <= 0) {
                work->mode = PLAYER_MODE_FALL;
                record->anim.next = ANIM_FALL;
                hunter->by_character.hunter.steps = 0;
                break;
            }
        } else {
            hunter->by_character.hunter.climb_grace = CLIMB_GRACE;
        }
        switch (record->anim.next) {
        case ANIM_CLIMB_X:
        case ANIM_CLIMB_MINUS_Z:
        case ANIM_CLIMB_Z:
            if ((record->anim.frame >= 0.0F &&
                 record->anim.frame < CLIMB_SOUND_0_END) ||
                (record->anim.frame >= CLIMB_SOUND_1_START &&
                 record->anim.frame < CLIMB_SOUND_1_END)) {
                if (stage_current_id() == STAGE_BOSS_FDOG) {
                    sound_play_for(SOUND_CLIMB_FDOG, work, 1, 0, 10);
                } else {
                    sound_play_for(SOUND_CLIMB, work, 1, 0, 10);
                }
            }
            break;
        }
        hunter_climb_anim(work, record);
        break;
    case PLAYER_MODE_HUNTER_ATTACK_1:
    case PLAYER_MODE_HUNTER_ATTACK_2:
    case PLAYER_MODE_HUNTER_RUN_ATTACK_1:
    case PLAYER_MODE_HUNTER_RUN_ATTACK_2:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_bump(work, motion, record)) {
            break;
        }
        if (hunter_attack_step(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            record->anim.next = ANIM_FALL;
            work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_HUNTER_ATTACK_3:
    case PLAYER_MODE_HUNTER_RUN_ATTACK_3:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_bump(work, motion, record)) {
            break;
        }
        /* @hack redundant: the test of hunter_attack_step, with a break each
         * way and an empty else, where the call alone would do */
        if (!hunter_attack_step(work, motion, record, hunter)) {
            break;
        } else {
        }
        break;
    case PLAYER_MODE_HUNTER_SPIRAL:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_bump(work, motion, record)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            work->flags &= ~PLAYER_WORK_ATTACK;
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_HUNTER_CLIMB_OVER:
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            work->mode = PLAYER_MODE_FALL;
            work->ang.x = work->ang.z = 0;
            motion->aim_x = motion->aim_z = 0;
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_PATH_HANG:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        if (work->flags & PLAYER_WORK_ON_PATH) {
            Angle heading;

            if (!player_read_stick(work, record, &heading, NULL)) {
                break;
            }
            /* @hack spelling: a break in each arm, and one after them that
             * nothing reaches, where that one alone would do */
            if (angle_distance((HALF_TURN + QUARTER_TURN) -
                                   camera_views[record->slot]->ang.y,
                               heading) < QUARTER_TURN) {
                record->spd.x = PATH_SPEED;
                break;
            } else {
                record->spd.x = -PATH_SPEED;
                break;
            }
            break;
        } else {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            player_stop(motion, record);
            work->flags &= ~PLAYER_WORK_ON_PATH;
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_PATH_HANG_GO:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            work->flags &= ~PLAYER_WORK_ON_PATH;
            break;
        }
        /* @hack redundant: the empty arm, where a test of its own in the
         * else would do */
        if (work->flags & PLAYER_WORK_ON_PATH) {
        } else {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            player_stop(motion, record);
            work->flags &= ~PLAYER_WORK_ON_PATH;
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_SLIDE:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_swim(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
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
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (pad_stick_pushed[record->slot] ||
            (player_pad_records[record->slot].press &
             PLAYER_STRUGGLE_BUTTONS)) {
            hunter->by_character.hunter.steps -= PLAYER_STRUGGLE_PUSH;
        }
        if (hunter->by_character.hunter.steps > 0) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_2A:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (!(record->anim.flags & PLAYER_ANIM_DONE)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_ASK_HINT:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_PLAY_FLUTE:
        if (hunter_take_request(work, motion, record, hunter)) {
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
        if (hunter_take_request(work, motion, record, hunter)) {
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
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        work->mode = PLAYER_MODE_UNK_35;
        break;
    case PLAYER_MODE_UNK_35:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (!(record->anim.flags & PLAYER_ANIM_DONE)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_HUNTER_FLOAT:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (!hunter_in_deep_water(work, motion, record)) {
            player_stand(work, motion, record);
            sound_play_at(SOUND_LEAVE_WATER, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_HUNTER_SWIM;
            record->anim.next = ANIM_SWIM;
            break;
        }
        if (pad_action_held[record->slot]) {
            work->mode = PLAYER_MODE_HUNTER_WATER_DIVE;
            record->anim.next = ANIM_WATER_DIVE;
            sound_start_at_for(SOUND_WATER_JUMP, work, 1, SOUND_LOUDEST, 60,
                               &work->pos);
            break;
        }
        break;
    case PLAYER_MODE_HUNTER_SWIM:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (!hunter_in_deep_water(work, motion, record)) {
            work->mode = PLAYER_MODE_RUN;
            player_run_anim(work, motion, record);
            sound_play_at(SOUND_LEAVE_WATER, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (!player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_HUNTER_FLOAT;
            record->anim.next = ANIM_FLOAT_UNDER_START;
            break;
        }
        if (pad_action_held[record->slot]) {
            work->mode = PLAYER_MODE_HUNTER_WATER_DIVE;
            record->anim.next = ANIM_WATER_DIVE;
            sound_start_at_for(SOUND_WATER_JUMP, work, 1, SOUND_LOUDEST, 60,
                               &work->pos);
            break;
        }
        break;
    case PLAYER_MODE_HUNTER_WATER_DIVE:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_can_swim_off_ground(work, motion, record)) {
            work->mode = PLAYER_MODE_HUNTER_FLOAT;
            record->anim.next = ANIM_FLOAT_UNDER_START;
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
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
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                player_run_anim(work, motion, record);
            }
            /* @hack spelling: a return, where a break would do */
            sound_start_at_for(SOUND_WATER_LAND, work, 1, SOUND_LOUDEST, 45,
                               &work->pos);
            return;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (pad_action_held[record->slot]) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_HUNTER_SWIM;
            record->anim.next = ANIM_SWIM;
            break;
        }
        work->mode = PLAYER_MODE_HUNTER_FLOAT;
        record->anim.next = ANIM_FLOAT_UNDER_START;
        break;
    case PLAYER_MODE_ROUGE_RIDE_FERRY:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        break;
    case PLAYER_MODE_ROUGE_RIDE_SWIMMER:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (pad_jump_pressed[record->slot]) {
            work->mode = PLAYER_MODE_HUNTER_SWIM;
            record->anim.next = ANIM_SWIM;
        }
        break;
    case PLAYER_MODE_PICK_UP:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (hunter_take_held_request(work, motion, record, hunter)) {
                break;
            }
        } else if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_test_pick_up(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_PULL_HEAVY:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (player_test_pull_heavy(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_UNK_17:
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_GRAB_PUSHABLE:
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (player_test_push(work, motion, record)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_STAND_HOLDING:
        if (hunter_take_held_request(work, motion, record, hunter)) {
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
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_WALK;
        }
        break;
    case PLAYER_MODE_UNK_19:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_hold_end(work, motion, record) ||
            hunter_take_action(work, motion, record, hunter) ||
            player_test_fall(work, motion, record) ||
            hunter_test_jump(work, motion, record) ||
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
            /* @hack redundant: the break, where the one after the test would
             * do */
            break;
        }
        break;
    case PLAYER_MODE_UNK_14:
    case PLAYER_MODE_THROW:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (hunter_take_held_request(work, motion, record, hunter)) {
                break;
            }
        } else if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_throw(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT &&
            record->anim.next != ANIM_THROW &&
            record->anim.next != ANIM_SET_DOWN) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_UNK_1B:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (record->spd.x <= 0.0F && player_stick_back(work, record)) {
            work->mode = PLAYER_MODE_UNK_19;
            record->anim.next = ANIM_HOLD_TURN;
            player_turn_to_stick(work, motion, record);
            work->ang.y = motion->aim_y;
            break;
        }
        if (record->anim.next != ANIM_HOLD_BRAKE) {
            work->mode = PLAYER_MODE_STAND_HOLDING;
            record->anim.next = ANIM_HOLD_STAND;
        }
        break;
    case PLAYER_MODE_FALL_HOLDING:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (!(work->flags & PLAYER_WORK_GROUNDED)) {
            break;
        }
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
        sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
        break;
    case PLAYER_MODE_JUMP_HOLDING:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
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
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            break;
        }
        if (player_count_jump_hold(work, record)) {
            record->spd.y +=
                record->consts.jump_lift * PLAYER_JUMP_HOLDING_LIFT;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL_HOLDING;
            record->anim.next = ANIM_HOLD_FALL_START;
        }
        break;
    case PLAYER_MODE_BOUNCE_HOLDING:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
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
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_take_action(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
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
                    work->mode = PLAYER_MODE_UNK_17;
                    record->anim.next = ANIM_GRAB_STAND;
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
                break;
            }
            break;
        } else {
            /* @hack spelling: the else and its break, where the three
             * statements after the test would do */
            player_release_held(work, motion, record);
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            break;
        }
        break;
    case PLAYER_MODE_AIR_THROW:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (hunter_take_held_request(work, motion, record, hunter)) {
                break;
            }
        } else if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
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
                    }
                } else if (njScalor(&motion->spd) < PLAYER_LAND_STILL) {
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
                if (work->flags & PLAYER_WORK_HOLDING) {
                    work->mode = PLAYER_MODE_UNK_19;
                } else {
                    work->mode = PLAYER_MODE_RUN;
                }
                player_run_anim(work, motion, record);
            }
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            break;
        }
        if (hunter_test_throw(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_AIR_THROW_AT &&
            record->anim.next != ANIM_AIR_THROW) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            hunter->by_character.hunter.steps = 0;
        }
        break;
    case PLAYER_MODE_UNK_22:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        if (player_touches_shakable(work, motion, record)) {
            if (!player_read_stick(work, record, NULL, NULL)) {
                work->mode = PLAYER_MODE_UNK_17;
                record->anim.next = ANIM_GRAB_STAND;
            }
            break;
        }
        player_stand(work, motion, record);
        player_release_held(work, motion, record);
        break;
    case PLAYER_MODE_SHAKE_HOLDING:
        if (hunter_take_held_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
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
            /* @hack redundant: the break, where the one after the test would
             * do */
            break;
        }
        break;
    case PLAYER_MODE_THROW_AT_TOUCH:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (hunter_take_held_request(work, motion, record, hunter)) {
                break;
            }
        } else if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_throw_at(player_tasks[record->slot], work, motion,
                                 record)) {
            break;
        }
        if (record->anim.next != ANIM_THROW_AT_TOUCH) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_PET_CHAO:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        /* @hack redundant: the break inside the test, where the one after
         * it would do */
        if (!player_test_petting(work, motion, record)) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_PULL_BURIED:
        if (work->flags & PLAYER_WORK_HOLDING) {
            if (hunter_take_held_request(work, motion, record, hunter)) {
                break;
            }
        } else if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (hunter_test_pull(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_HANG:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_LEDGE_CLIMB:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (player_test_ledge_hang(work, motion, record)) {
            hunter->by_character.hunter.steps = 0;
            break;
        }
        if (!(record->anim.flags & PLAYER_ANIM_ENDING)) {
            break;
        }
        player_onto_ledge(work, motion, record);
        work->mode = PLAYER_MODE_STAND;
        break;
    case PLAYER_MODE_SWIM:
        if (hunter_take_request(work, motion, record, hunter)) {
            break;
        }
        if (hunter_test_jump(work, motion, record)) {
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
        } else if (player_test_ledge(work, motion, record)) {
            player_hang_ledge(work, motion, record);
            work->flags &= ~PLAYER_WORK_ON_PATH;
        }
        break;
    case PLAYER_MODE_EVENT:
        if (work->submode == PLAYER_REQUEST_RELEASE) {
            player_stand(work, motion, record);
            work->flags &=
                ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        }
        break;
    case PLAYER_MODE_FREE_MOVE:
        if (player_pad_records[record->slot].press & PDD_DGT_TA) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
        }
    }
}

/* the task's destructor: free what the maker made, and take the player out
 * of the players' tables */
static void hunter_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct player *hunter = task->extra.p;
    /* @hack type: record taken as the address of hunter's first field,
     * where hunter would do */
    struct player *record = (struct player *)&hunter->slot;

    if (hunter->by_character.hunter.swing != NULL) {
        jiggle_free(hunter->by_character.hunter.swing);
        hunter->by_character.hunter.swing = NULL;
    }
    if (hunter->by_character.hunter.squash != NULL) {
        jiggle_free(hunter->by_character.hunter.squash);
        hunter->by_character.hunter.squash = NULL;
    }
    player_unregister(record->slot);
    sound_stop_key(work);
    njReleaseTexture(hunter->by_character.hunter.texlist);
    syFree(hunter->by_character.hunter.texlist);
    hunter->by_character.hunter.texlist = NULL;
    njReleaseTexture(hunter->by_character.hunter.effects_texlist);
    syFree(hunter->by_character.hunter.effects_texlist);
    hunter->by_character.hunter.effects_texlist = NULL;
    life_icon_release(record->kind);
    model_file_free(hunter->by_character.hunter.models);
    hunter->by_character.hunter.models = NULL;
    motion_file_free(hunter->by_character.hunter.motions);
    hunter->by_character.hunter.motions = NULL;
}

/* center the body's shape center_height above the work's pos, along the
 * player's up. in a dig, it then sinks by dig_depth along the ground's
 * normal */
/* @hack redundant: hunter, the record again, where record alone would do */
static void hunter_place_body(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *hunter)
{
    struct collision_shape *body = work->collision->shapes;
    NJS_VECTOR up;

    up.x = up.z = 0.0F;
    up.y = record->consts.center_height;
    turn_to_world(work, &up);
    body->center = up;
    if (work->mode == PLAYER_MODE_HUNTER_DIG ||
        work->mode == PLAYER_MODE_HUNTER_DIG_OUT ||
        work->mode == PLAYER_MODE_HUNTER_WALL_DIG ||
        work->mode == PLAYER_MODE_HUNTER_WALL_DIG_OUT) {
        body->center.x +=
            hunter->by_character.hunter.dig_depth * record->ground_normal.x;
        body->center.y +=
            hunter->by_character.hunter.dig_depth * record->ground_normal.y;
        body->center.z +=
            hunter->by_character.hunter.dig_depth * record->ground_normal.z;
    }
    body->center.x += work->pos.x;
    body->center.y += work->pos.y;
    body->center.z += work->pos.z;
}

struct hunt *hunt_current = NULL;
/* these six statics hold the places the hunt's pieces C, B and A are pinned to.
 * each piece's place is a kind and an id. hunt_step keeps a pinned place alone
 * in its list where the list holds it. each is -1, which no place has, and
 * nothing writes them */
static Sint8 hunt_pin_c_kind = -1;
static Sint8 hunt_pin_b_kind = -1;
static Sint8 hunt_pin_a_kind = -1;
static Sint8 hunt_pin_c_id = -1;
static Sint8 hunt_pin_b_id = -1;
static Sint8 hunt_pin_a_id = -1;

/* the record of the hunter being drawn. each draw sets it, and the node
 * callbacks write the record's places through it */
static struct player *drawn_record;
static Sint32 unused_word;
/* the inverse of the view as each draw begins. it takes a place in the view
 * back to the world */
static NJS_MATRIX view_to_world;
static NJS_MATRIX unused_matrix;
/* the matrices the node callbacks keep for the upgrades' models.
 * upgrade_0_matrix and upgrade_1_matrix are KNUCKLES_HAND_0's and
 * KNUCKLES_HAND_1's, or ROUGE_BOOT_NODE_0's and ROUGE_BOOT_NODE_1's.
 * face_matrix is KNUCKLES_FACE's or ROUGE_FACE's, and neck_matrix
 * KNUCKLES_NECK's. the other three are Rouge's, at the nodes they name */
static NJS_MATRIX upgrade_1_matrix;
static NJS_MATRIX upgrade_0_matrix;
static NJS_MATRIX face_matrix;
static NJS_MATRIX neck_matrix;
static NJS_MATRIX part_matrix;
static NJS_MATRIX nails_1_matrix;
static NJS_MATRIX nails_0_matrix;

/* the motion of animation index in the record's table, which the draws take
 * through the table twice */
#define ANIM_MOTION(record, index)                                             \
    (motion_table                                                              \
         [(record)->anim.actions[(record)->anim.actions[index].motion].motion] \
             .motion)

/* the turn about y that each frame of the dive gives its afterimage:
 * Knuckles's, and Rouge's */
#define KNUCKLES_SPIN_TURN 4096.0F
#define ROUGE_SPIN_TURN 8192.0F

/* in a split screen, the other player's camera leaves out a hunter beyond
 * its view. it tests a sphere of VIEW_RADIUS against the view. it leaves out
 * a hunter behind the camera too, or more than VIEW_DEPTH ahead of it */
#define VIEW_RADIUS 50.0F
#define VIEW_DEPTH 1000.0F
/* the shadows at the hands and the feet, each shadow_model scaled across x
 * and z. Rouge's hand's shadow lies ROUGE_HAND_SHADOW_DROP below the hand */
#define KNUCKLES_HAND_SHADOW_X 1.4F
#define KNUCKLES_HAND_SHADOW_Z 1.1F
#define KNUCKLES_FOOT_SHADOW_X 2.6F
#define KNUCKLES_FOOT_SHADOW_Z 1.2F
#define ROUGE_HAND_SHADOW_DROP 2.0F
#define ROUGE_HAND_SHADOW_X 1.2F
#define ROUGE_HAND_SHADOW_Z 0.7F
#define ROUGE_FOOT_SHADOW_X 2.0F

void hunter_spin_afterimage(struct task_work *work, struct player *record,
                            struct player *hunter)
{
    NJS_CNK_OBJECT *object;
    Angle spin;

    if (!(game_state.steps & 1)) {
        if (record->character == PLAYER_KNUCKLES &&
            model_table[KNUCKLES_SPIN].object != NULL) {
            object = model_table[KNUCKLES_SPIN].object;
            spin = record->anim.frame * KNUCKLES_SPIN_TURN;
        } else if (record->character == PLAYER_ROUGE &&
                   model_table[ROUGE_SPIN].object != NULL) {
            object = model_table[ROUGE_SPIN].object;
            spin = record->anim.frame * ROUGE_SPIN_TURN;
        } else {
            return;
        }
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njRotateY(NULL, spin);
        player_afterimage(object, NULL, hunter->by_character.hunter.texlist,
                          0.0F, NULL, record->slot);
        njPopMatrixEx();
    }
}

/* leave a fading copy of the animation playing, at each step whose
 * game_state.steps is even. the body's afterimage model takes the body's
 * place where it is loaded */
static void hunter_afterimage(struct task_work *work, struct player *record,
                              struct player *hunter)
{
    NJS_CNK_OBJECT *object;
    int current;

    if (!(game_state.steps & 1)) {
        current = record->anim.current;
        object = model_table[record->anim.actions[current].model].object;
        if (object == model_table[KNUCKLES_BODY].object &&
            model_table[KNUCKLES_AFTERIMAGE].object != NULL) {
            object = model_table[KNUCKLES_AFTERIMAGE].object;
        } else if (object == model_table[ROUGE_BODY].object &&
                   model_table[ROUGE_AFTERIMAGE].object != NULL) {
            object = model_table[ROUGE_AFTERIMAGE].object;
        }
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        player_afterimage(object, ANIM_MOTION(record, current),
                          hunter->by_character.hunter.texlist,
                          record->anim.frame, NULL, record->slot);
        njPopMatrixEx();
    }
}

void hunter_afterimage_slow(struct task_work *work, struct player *record,
                            struct player *hunter)
{
    if (!(game_state.steps & 3)) {
        hunter_afterimage(work, record, hunter);
    }
}

/* Knuckles's motion callback, which the body's draw calls at each node. it
 * keeps the places in the world of the nodes the draws track, in
 * drawn_record. with each place it keeps one or two of the node's axes. it
 * keeps the matrices of the nodes the upgrades draw at. it turns the _LEAN
 * node about x by the record's lean in PLAYER_MODE_RUN. while the hunter
 * holds a task, it turns _ARM_0 about x by the held task's motion's
 * hold_turn. it turns _ARM_1 the other way */
static void knuckles_node_callback(NJS_CNK_OBJECT *object)
{
    struct player *record = drawn_record;
    struct task_work *work;
    NJS_VECTOR pos;
    NJS_VECTOR axis;

    pos.z = 0.0F;
    pos.y = 0.0F;
    pos.x = 0.0F;
    if (object == model_table[KNUCKLES_AT_230].object) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2cc);
    } else if (object == model_table[KNUCKLES_LEAN].object) {
        work = player_work[record->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, record->lean);
        }
    } else if (object == model_table[KNUCKLES_ARM_0].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[KNUCKLES_ARM_1].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, -TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[KNUCKLES_HEAD].object) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_218);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2b4);
    } else if (object == model_table[KNUCKLES_MOUTH].object) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_224);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2c0);
    } else if (object == model_table[KNUCKLES_HAND_0].object) {
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_1e8);
        axis.z = 1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_26c);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_284);
        njGetMatrix(&upgrade_0_matrix);
    } else if (object == model_table[KNUCKLES_HAND_1].object) {
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_1f4);
        axis.z = -1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_278);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_290);
        njGetMatrix(&upgrade_1_matrix);
    } else if (object == model_table[KNUCKLES_FOOT_0].object) {
        axis.y = -1.0F;
        axis.z = 0.0F;
        axis.x = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_200);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_29c);
    } else if (object == model_table[KNUCKLES_FOOT_1].object) {
        axis.y = -1.0F;
        axis.z = 0.0F;
        axis.x = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_20c);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2a8);
    } else if (object == model_table[KNUCKLES_FACE].object) {
        njGetMatrix(&face_matrix);
    } else if (object == model_table[KNUCKLES_NECK].object) {
        njGetMatrix(&neck_matrix);
    }
}

static void knuckles_draw_shadows(struct task_work *work, struct player *record,
                                  int current);
static void hunter_place_held(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);

void knuckles_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player_motion *motion = task->motion.p;
    struct player *hunter = task->extra.p;
    /* @hack type: record taken as the address of hunter's first field,
     * where hunter would do */
    struct player *record = (struct player *)&hunter->slot;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
    NJS_CNK_MODEL *model;
    struct collision_shape *shape;
    int current;
    Angle ang;
    Float rate;
    NJS_POINT3 pos;
    NJS_POINT3 center;

    if (game_state.single_view == 0 && game_state.split_screen == 1 &&
        camera_current_index != record->slot) {
        static const NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

        pos = origin;
        if (sphere_out_of_view(&work->pos, VIEW_RADIUS)) {
            return;
        }
        njCalcPoint(NULL, &work->pos, &pos);
        if (pos.z < 0.0F || pos.z > VIEW_DEPTH) {
            return;
        }
    } else if (game_state.solo_player != 0 &&
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
    njCnkSetMotionCallback(knuckles_node_callback);
    current = record->anim.current;
    /* @hack redundant: the test that the timer, a Uint16, is at least 0 */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state.solo_player != 0) {
        njSetTexture(hunter->by_character.hunter.texlist);
        njPushMatrixEx();
        if (work->mode == PLAYER_MODE_PLAY_ANIM) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            pos.x = pos.z = 0.0F;
            pos.y = -(record->consts.height * 0.5F);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &pos, &center);
            njPopMatrixEx();
            center.x += shape->center.x;
            center.y += shape->center.y;
            center.z += shape->center.z;
            njTranslateEx(&center);
        }
        if (game_state.solo_player == record->slot + 1) {
            njRotateY(NULL, HALF_TURN - work->ang.y);
        } else {
            switch (record->anim.current) {
            case ANIM_GLIDE:
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                hunter->by_character.hunter.glide_roll = angle_approach(
                    hunter->by_character.hunter.glide_roll,
                    -angle_difference(work->ang.y, motion->aim_y),
                    GLIDE_ROLL_STEP);
                njRotateY(NULL, HALF_TURN - work->ang.y);
                njRotateX(NULL, hunter->by_character.hunter.glide_roll);
                break;
            case ANIM_GRAB_WALL:
                rate = 1.0F - (record->anim.frame /
                               ANIM_MOTION(record, current)->nbFrame);
                ang = hunter->by_character.hunter.glide_roll * rate;
                njRotateY(NULL, HALF_TURN - work->ang.y);
                njRotateX(NULL, ang);
                njRotateZ(NULL, -(Angle)(acosf(record->ball->hit.normal.y) *
                                         ANGLE_PER_RADIAN));
                break;
            default:
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, HALF_TURN - work->ang.y);
                break;
            }
        }
        njScaleEx(&work->scl);
#ifdef NONMATCHING
        /* GCC can't see that model is set wherever it is read. the swing's
         * two tests guard the set and the read. the draw between them leaves
         * the swing as it is */
        model = NULL;
#endif
        if (hunter->by_character.hunter.swing != NULL) {
            model = model_table[KNUCKLES_SWING_PARENT].object->child->model;
            model_table[KNUCKLES_SWING_PARENT].object->child->model =
                hunter->by_character.hunter.swing->copy->model;
        }
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions->model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[current].model].object,
                ANIM_MOTION(record, current), record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[current].model].object,
                record->anim.blend, record->anim.frame);
        }
        if (hunter->by_character.hunter.swing != NULL) {
            model_table[KNUCKLES_SWING_PARENT].object->child->model = model;
        }
        if (current != ANIM_NO_UPGRADES) {
            njPushMatrixEx();
            if (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_SUNGLASS)) {
                object = model_table[KNUCKLES_SUNGLASSES].object->child;
                njSetMatrix(NULL, &face_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateZ(NULL,
                          hunter->by_character.hunter.sunglasses - EIGHTH_TURN);
                njCnkSimpleDrawModel(object->model);
            }
            if (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_AIR)) {
                object = model_table[KNUCKLES_NECKLACE].object;
                njSetMatrix(NULL, &neck_matrix);
                njCnkSimpleDrawObject(object);
            }
            if (record->upgrades & (UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS) |
                                    UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES))) {
                njSetMatrix(NULL, &upgrade_0_matrix);
                if (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS)) {
                    part = model_table[KNUCKLES_CLAW_0].object;
                    if (part != NULL) {
                        njCnkSimpleDrawObject(part);
                    }
                }
                if (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES)) {
                    part = model_table[KNUCKLES_GLOVE_0].object;
                    if (part != NULL) {
                        njCnkSimpleDrawObject(part);
                    }
                }
                njSetMatrix(NULL, &upgrade_1_matrix);
                if (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS)) {
                    part = model_table[KNUCKLES_CLAW_1].object;
                    if (part != NULL) {
                        njCnkSimpleDrawObject(part);
                    }
                }
                if (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES)) {
                    part = model_table[KNUCKLES_GLOVE_1].object;
                    if (part != NULL) {
                        njCnkSimpleDrawObject(part);
                    }
                }
            }
            njPopMatrixEx();
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    knuckles_draw_shadows(work, record, current);
    hunter_place_held(work, motion, record);
}

/* draw the modifier volumes that shade the body: the head's, and one at
 * each hand and foot, while the animation poses KNUCKLES_BODY */
static void knuckles_draw_shadows(struct task_work *work, struct player *record,
                                  int current)
{
    int model = record->anim.actions[current].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == KNUCKLES_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[KNUCKLES_HEAD_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_1e8);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, KNUCKLES_HAND_SHADOW_X, 1.0F, KNUCKLES_HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_1f4);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, KNUCKLES_HAND_SHADOW_X, 1.0F, KNUCKLES_HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, KNUCKLES_FOOT_SHADOW_X, 1.0F, KNUCKLES_FOOT_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, KNUCKLES_FOOT_SHADOW_X, 1.0F, KNUCKLES_FOOT_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* Rouge's motion callback, as knuckles_node_callback is Knuckles's */
static void rouge_node_callback(NJS_CNK_OBJECT *object)
{
    struct player *record = drawn_record;
    struct task_work *work;
    NJS_VECTOR pos;
    NJS_VECTOR axis;

    pos.z = 0.0F;
    pos.y = 0.0F;
    pos.x = 0.0F;
    if (object == model_table[ROUGE_AT_230].object) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_230);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2cc);
    } else if (object == model_table[ROUGE_LEAN].object) {
        work = player_work[record->slot];
        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, record->lean);
        }
    } else if (object == model_table[ROUGE_ARM_0].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[ROUGE_ARM_1].object) {
        work = player_work[record->slot];
        if (work->flags & PLAYER_WORK_HOLDING) {
            njRotateX(NULL, -TASK_MOTION_OF(record->held)->hold_turn);
        }
    } else if (object == model_table[ROUGE_HEAD].object) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_218);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2b4);
    } else if (object == model_table[ROUGE_MOUTH].object) {
        axis.x = 1.0F;
        axis.z = 0.0F;
        axis.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_224);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2c0);
    } else if (object == model_table[ROUGE_HAND_0].object) {
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_1e8);
        axis.z = 1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_26c);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_284);
    } else if (object == model_table[ROUGE_HAND_1].object) {
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_1f4);
        axis.z = -1.0F;
        axis.y = 0.0F;
        axis.x = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_278);
        axis.y = -1.0F;
        axis.x = 0.0F;
        axis.z = 0.0F;
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_290);
    } else if (object == model_table[ROUGE_FOOT_0].object) {
        axis.y = -1.0F;
        axis.z = 0.0F;
        axis.x = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_200);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_29c);
    } else if (object == model_table[ROUGE_FOOT_1].object) {
        axis.y = -1.0F;
        axis.z = 0.0F;
        axis.x = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_20c);
        njCalcVector(NULL, &axis, &axis);
        njCalcVector(&view_to_world, &axis, &record->unk_2a8);
    }
    if (object == model_table[ROUGE_PART_NODE].object) {
        njGetMatrix(&part_matrix);
    }
    if (object == model_table[ROUGE_NAILS_NODE_0].object) {
        njGetMatrix(&nails_0_matrix);
    }
    if (object == model_table[ROUGE_NAILS_NODE_1].object) {
        njGetMatrix(&nails_1_matrix);
    }
    if (object == model_table[ROUGE_BOOT_NODE_0].object) {
        njGetMatrix(&upgrade_0_matrix);
    }
    if (object == model_table[ROUGE_BOOT_NODE_1].object) {
        njGetMatrix(&upgrade_1_matrix);
    }
    if (object == model_table[ROUGE_FACE].object) {
        njGetMatrix(&face_matrix);
    }
}

static void rouge_draw_shadows(struct task_work *work, struct player *record,
                               int current);

void rouge_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player_motion *motion = task->motion.p;
    struct player *hunter = task->extra.p;
    /* @hack type: record taken as the address of hunter's first field,
     * where hunter would do */
    struct player *record = (struct player *)&hunter->slot;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
    NJS_CNK_MODEL *model;
    NJS_CNK_MODEL *jiggled;
    struct collision_shape *shape;
    int current;
    Angle ang;
    Float rate;
    NJS_POINT3 pos;
    NJS_POINT3 center;
    /* @hack spelling: njRotateY through a local, where a direct call would
     * do */
    void (*rotate_y)(NJS_MATRIX *matrix, Angle ang);

    if (game_state.single_view == 0 && game_state.split_screen == 1 &&
        camera_current_index != record->slot) {
        static const NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

        pos = origin;
        if (sphere_out_of_view(&work->pos, VIEW_RADIUS)) {
            return;
        }
        njCalcPoint(NULL, &work->pos, &pos);
        if (pos.z < 0.0F || pos.z > VIEW_DEPTH) {
            return;
        }
    } else if (game_state.solo_player != 0 &&
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
    njCnkSetMotionCallback(rouge_node_callback);
    current = record->anim.current;
    /* @hack redundant: the test that the timer, a Uint16, is at least 0 */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state.solo_player != 0) {
        njSetTexture(hunter->by_character.hunter.texlist);
        njPushMatrixEx();
        rotate_y = njRotateY;
        if (work->mode == PLAYER_MODE_PLAY_ANIM) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            pos.x = pos.z = 0.0F;
            pos.y = -(record->consts.height * 0.5F);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            rotate_y(NULL, -work->ang.y);
            njCalcVector(NULL, &pos, &center);
            njPopMatrixEx();
            center.x += shape->center.x;
            center.y += shape->center.y;
            center.z += shape->center.z;
            njTranslateEx(&center);
        }
        if (game_state.solo_player == record->slot + 1) {
            rotate_y(NULL, HALF_TURN - work->ang.y);
        } else {
            switch (record->anim.current) {
            case ANIM_GLIDE:
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                hunter->by_character.hunter.glide_roll = angle_approach(
                    hunter->by_character.hunter.glide_roll,
                    -angle_difference(work->ang.y, motion->aim_y),
                    GLIDE_ROLL_STEP);
                rotate_y(NULL, HALF_TURN - work->ang.y);
                njRotateX(NULL, hunter->by_character.hunter.glide_roll);
                break;
            case ANIM_GRAB_WALL:
                rate = 1.0F - (record->anim.frame /
                               ANIM_MOTION(record, current)->nbFrame);
                ang = hunter->by_character.hunter.glide_roll * rate;
                rotate_y(NULL, HALF_TURN - work->ang.y);
                njRotateX(NULL, ang);
                njRotateZ(NULL, -(Angle)(acosf(record->ball->hit.normal.y) *
                                         ANGLE_PER_RADIAN));
                break;
            default:
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                rotate_y(NULL, HALF_TURN - work->ang.y);
                break;
            }
        }
        njScaleEx(&work->scl);
#ifdef NONMATCHING
        /* GCC can't see that jiggled and model are set wherever they are
         * read. each jiggle's two tests guard its set and its read. the draw
         * between them leaves both jiggles as they are */
        jiggled = NULL;
        model = NULL;
#endif
        if (hunter->by_character.hunter.squash != NULL) {
            jiggled = model_table[ROUGE_SQUASH].object->model;
            model_table[ROUGE_SQUASH].object->model =
                hunter->by_character.hunter.squash->copy->model;
        }
        if (hunter->by_character.hunter.swing != NULL) {
            model = model_table[ROUGE_SWING_PARENT].object->child->model;
            model_table[ROUGE_SWING_PARENT].object->child->model =
                hunter->by_character.hunter.swing->copy->model;
        }
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions->model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[current].model].object,
                ANIM_MOTION(record, current), record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[current].model].object,
                record->anim.blend, record->anim.frame);
        }
        if (hunter->by_character.hunter.squash != NULL) {
            model_table[ROUGE_SQUASH].object->model = jiggled;
        }
        if (hunter->by_character.hunter.swing != NULL) {
            model_table[ROUGE_SWING_PARENT].object->child->model = model;
        }
        /* @hack spelling: the part tested through object, and read from the
         * table again for the draws, where object would do for both */
        object = model_table[ROUGE_PART].object;
        if (object != NULL) {
            njPushMatrix(&part_matrix);
            if (hunter->by_character.hunter.part_mode != PLAYER_ANIM_BLEND) {
                njCnkSimpleDrawShapeMotion(
                    model_table[ROUGE_PART].object,
                    motion_table[hunter->by_character.hunter.part_motion]
                        .motion,
                    motion_table[hunter->by_character.hunter.part_motion + 1]
                        .motion,
                    hunter->by_character.hunter.part_frame);
            } else {
                njCnkSimpleDrawShapeMotionLink(
                    model_table[ROUGE_PART].object,
                    &hunter->by_character.hunter.part_motion_link,
                    &hunter->by_character.hunter.part_shape_link,
                    hunter->by_character.hunter.part_frame);
            }
            njPopMatrixEx();
        }
        if (current != ANIM_NO_UPGRADES) {
            njPushMatrixEx();
            if (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_SCOPE)) {
                object = model_table[ROUGE_SCOPE].object;
                if (object != NULL) {
                    njSetMatrix(NULL, &face_matrix);
                    njCnkSimpleDrawModel(object->model);
                    object = object->child;
                    njTranslateEx((NJS_VECTOR *)object->pos);
                    njRotateZ(NULL, hunter->by_character.hunter.sunglasses -
                                        EIGHTH_TURN);
                    njCnkSimpleDrawModel(object->model);
                }
            }
            if (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_BOOTS)) {
                part = model_table[ROUGE_BOOT_0].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &upgrade_0_matrix);
                    njCnkSimpleDrawObject(part);
                }
                part = model_table[ROUGE_BOOT_1].object;
                if (part != NULL) {
                    njSetMatrix(NULL, &upgrade_1_matrix);
                    njCnkSimpleDrawObject(part);
                }
            }
            part = model_table[ROUGE_UNDER_NAILS_1].object;
            if (part != NULL) {
                njSetMatrix(NULL, &nails_1_matrix);
                njCnkSimpleDrawObject(part);
                if (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_NAILS)) {
                    njCnkSimpleDrawObject(model_table[ROUGE_NAILS_1].object);
                }
            }
            part = model_table[ROUGE_UNDER_NAILS_0].object;
            if (part != NULL) {
                njSetMatrix(NULL, &nails_0_matrix);
                njCnkSimpleDrawObject(part);
                if (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_NAILS)) {
                    njCnkSimpleDrawObject(model_table[ROUGE_NAILS_0].object);
                    /* @hack redundant: a call that never runs */
                    if (0) {
                        njCnkSimpleDrawObject(NULL);
                    }
                }
            }
            njPopMatrixEx();
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    if (record->kind == PLAYER_KIND_CHAOS0) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    rouge_draw_shadows(work, record, current);
    hunter_place_held(work, motion, record);
}

/* draw Rouge's modifier volumes, as knuckles_draw_shadows draws Knuckles's,
 * while the animation poses ROUGE_BODY */
static void rouge_draw_shadows(struct task_work *work, struct player *record,
                               int current)
{
    int model = record->anim.actions[current].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == ROUGE_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[ROUGE_HEAD_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_1e8);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njTranslate(NULL, 0.0F, -ROUGE_HAND_SHADOW_DROP, 0.0F);
        njScale(NULL, ROUGE_HAND_SHADOW_X, 1.0F, ROUGE_HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_1f4);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njTranslate(NULL, 0.0F, -ROUGE_HAND_SHADOW_DROP, 0.0F);
        njScale(NULL, ROUGE_HAND_SHADOW_X, 1.0F, ROUGE_HAND_SHADOW_Z);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, ROUGE_FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScale(NULL, ROUGE_FOOT_SHADOW_X, 1.0F, 1.0F);
        njCnkModDrawModel(shadow_model);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* a hand of the path hang's animation takes a hold at HANG_HOLD_0 and
 * HANG_HOLD_1. a round of it is HANG_FRAMES frames, and a step moves it
 * HANG_STEP */
#define HANG_HOLD_0 16.0F
#define HANG_HOLD_1 30.0F
#define HANG_FRAMES 32.0F
#define HANG_STEP 2.0F
/* a hang with the stick held calls fun_8c05bce0 at HANG_HOLD_0 as the frame
 * steps up, or at HANG_HOLD_1 as it steps down. fun_8c05bce0 returns at once.
 * the hang calls it on HANG_CALL_STEPS of each HANG_CALL_PERIOD steps */
#define HANG_CALL_PERIOD 0x80
#define HANG_CALL_STEPS 31

void hunter_climb_step(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    NJS_VECTOR up;

    up.x = 0.0F;
    up.y = record->consts.height;
    up.z = 0.0F;
    turn_to_world(work, &up);
    njSubVector(&work->pos, &up);
    if (record->spd.x != 0.0F && record->drive == 0.0F) {
        if (record->anim.frame == HANG_HOLD_0 ||
            record->anim.frame == HANG_HOLD_1) {
            work->mode = PLAYER_MODE_PATH_HANG;
            record->spd.x = 0.0F;
        }
    } else if (record->spd.x > 0.0F) {
        if (record->anim.frame >= 0.0F && record->anim.frame < HANG_HOLD_0) {
            record->anim.frame += HANG_STEP;
            if (record->anim.frame == HANG_HOLD_0 &&
                !player_read_stick(work, record, NULL, NULL)) {
                work->mode = PLAYER_MODE_PATH_HANG;
                record->spd.x = 0.0F;
            } else if (record->anim.frame == HANG_HOLD_0) {
                if ((game_state.steps & (HANG_CALL_PERIOD - 1)) <
                    HANG_CALL_STEPS) {
                    fun_8c05bce0();
                }
            }
        } else {
            record->anim.frame += HANG_STEP;
            if (record->anim.frame >= HANG_FRAMES) {
                record->anim.frame = 0.0F;
            }
            if (record->anim.frame == HANG_HOLD_1 &&
                !player_read_stick(work, record, NULL, NULL)) {
                work->mode = PLAYER_MODE_PATH_HANG;
                record->spd.x = 0.0F;
            }
        }
    } else if (record->spd.x < 0.0F) {
        if (record->anim.frame > HANG_HOLD_0 &&
            record->anim.frame <= HANG_FRAMES) {
            record->anim.frame -= HANG_STEP;
            if (record->anim.frame == HANG_HOLD_0 &&
                !player_read_stick(work, record, NULL, NULL)) {
                work->mode = PLAYER_MODE_PATH_HANG;
                record->spd.x = 0.0F;
            }
        } else {
            record->anim.frame -= HANG_STEP;
            if (record->anim.frame < 0.0F) {
                record->anim.frame = HANG_HOLD_1;
            }
            if (record->anim.frame == HANG_HOLD_1 &&
                !player_read_stick(work, record, NULL, NULL)) {
                work->mode = PLAYER_MODE_PATH_HANG;
                record->spd.x = 0.0F;
            } else if (record->anim.frame == HANG_HOLD_1) {
                if ((game_state.steps & (HANG_CALL_PERIOD - 1)) <
                    HANG_CALL_STEPS) {
                    fun_8c05bce0();
                }
            }
        }
    } else {
        record->spd.x = 0.0F;
    }
}

void hunter_start_glide(struct player *record, struct player *hunter)
{
    if (record->spd.x <= record->consts.running_speed) {
        record->spd.x = record->consts.running_speed;
    }
    record->spd.y = 0.0F;
    hunter->by_character.hunter.glide_roll = 0;
    if (record->character == PLAYER_KNUCKLES) {
        sound_play(SOUND_GLIDE_KNUCKLES, NULL, 0, 0);
    } else {
        sound_play(SOUND_GLIDE_ROUGE, NULL, 0, 0);
    }
}

void hunter_jump(struct task_work *work, struct player_motion *motion,
                 struct player *record)
{
    struct task_force *force;
    int mode;

    if (record->ground_task != NULL) {
        force = record->ground_task->force.p;
        if (force != NULL) {
            player_set_acc(record->slot, force->speed.x, force->speed.y,
                           force->speed.z);
        }
    }

    mode = work->mode;
    if (mode == PLAYER_MODE_FALL || mode == PLAYER_MODE_HURT) {
        record->anim.next = ANIM_FALL_JUMP;
    } else if (mode == PLAYER_MODE_HUNTER_GLIDE) {
        record->anim.next = ANIM_GLIDE_JUMP;
    } else {
        record->spd.y = record->consts.jump_speed;
        record->anim.next = ANIM_JUMP;
        if (hunter_in_deep_water(work, motion, record) == FALSE) {
            if (record->kind == PLAYER_KNUCKLES) {
                sound_play_at(CRY_JUMP_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            } else if (record->kind == PLAYER_ROUGE) {
                sound_play_at(CRY_JUMP_ROUGE, NULL, 0, SOUND_LOUDEST,
                              &work->pos);
            }
        }
    }

    if (work->mode != PLAYER_MODE_HUNTER_GLIDE) {
        if (hunter_in_deep_water(work, motion, record)) {
            sound_start_at_for(SOUND_WATER_JUMP, work, 1, SOUND_LOUDEST, 60,
                               &work->pos);
        } else if (record->character == PLAYER_KNUCKLES) {
            sound_play(SOUND_JUMP_KNUCKLES, NULL, 0, 0);
        } else {
            sound_play(SOUND_JUMP_ROUGE, NULL, 0, 0);
        }
    }
    work->mode = PLAYER_MODE_JUMP;
    work->flags &=
        ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
    work->flags |= PLAYER_WORK_ATTACK;
    work->flags &= ~PLAYER_WORK_ON_SHAPE;
    record->jump_steps = 0;
}

Bool hunter_test_jump(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    int flags;

    if (player_jump_pressed(work, record)) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            flags = player_action_flags(work, record);
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_JUMP_HOLDING;
                record->spd.y = record->consts.jump_speed;
                record->anim.next = ANIM_HOLD_JUMP;
                work->flags &= ~PLAYER_WORK_ON_SHAPE;
                record->jump_steps = 0;
                if (hunter_in_deep_water(work, motion, record)) {
                    sound_start_at_for(SOUND_WATER_JUMP, work, 1, SOUND_LOUDEST,
                                       60, &work->pos);
                } else {
                    if (record->character == PLAYER_KNUCKLES) {
                        sound_play(SOUND_JUMP_KNUCKLES, NULL, 0, 0);
                    } else {
                        sound_play(SOUND_JUMP_ROUGE, NULL, 0, 0);
                    }
                    if (record->kind == PLAYER_KNUCKLES) {
                        sound_play_at(CRY_JUMP_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                                      &work->pos);
                    } else if (record->kind == PLAYER_ROUGE) {
                        sound_play_at(CRY_JUMP_ROUGE, NULL, 0, SOUND_LOUDEST,
                                      &work->pos);
                    }
                }
                return TRUE;
            } else {
                player_release_held(work, motion, record);
            }
        }
        hunter_jump(work, motion, record);
        return TRUE;
    }
    return FALSE;
}

/* how far out from the wall the leap off it moves the player. the leap's
 * spd.x and spd.y each take jump_speed times WALL_JUMP_SHARE. that is
 * WALL_JUMP_SPEED at a slant of 45 degrees. WALL_JUMP_SLANT is the slant's
 * cosine to two places */
#define WALL_JUMP_OUT 5.0F
#define WALL_JUMP_SLANT 0.71F
#define WALL_JUMP_SPEED 0.75F
#define WALL_JUMP_SHARE (WALL_JUMP_SLANT * WALL_JUMP_SPEED)

Bool hunter_test_wall_jump(struct task_work *work, struct player_motion *motion,
                           struct player *record)
{
    NJS_VECTOR up;

    if (player_jump_pressed(work, record)) {
        record->spd.x = record->consts.jump_speed * WALL_JUMP_SHARE;
        /* @bug logic: loses the leap's spd.y. the climb that calls the leap,
         * at 0x8c07cc90, is no mode in which hunter_jump keeps spd.y. so
         * hunter_jump sets spd.y to jump_speed at 0x8c08077c, after the
         * store below at 0x8c080a02 */
        record->spd.y = record->consts.jump_speed * WALL_JUMP_SHARE;
        record->spd.z = 0.0F;
        up.z = 0.0F;
        up.x = 0.0F;
        up.y = 1.0F;
        turn_to_world(work, &up);
        work->pos.x += up.x * WALL_JUMP_OUT;
        work->pos.y += up.y * WALL_JUMP_OUT;
        work->pos.z += up.z * WALL_JUMP_OUT;
        work->ang.x = 0;
        work->ang.y = atan2f(up.z, up.x) * ANGLE_PER_RADIAN;
        work->ang.z = 0;
        up.z = 0.0F;
        up.x = 0.0F;
        up.y = 1.0F;
        turn_to_world(work, &up);
        work->pos.x -= up.x * WALL_JUMP_OUT;
        work->pos.y -= up.y * WALL_JUMP_OUT;
        work->pos.z -= up.z * WALL_JUMP_OUT;
        hunter_jump(work, motion, record);
        record->jump_steps = record->consts.jump_hold_steps;
        return TRUE;
    }
    return FALSE;
}

static void hunter_start_dive(struct task_work *work, struct player *record,
                              struct player *hunter);
static void hunter_start_attack(struct task_work *work, struct player *record,
                                struct player *hunter);
static void hunter_start_run_attack(struct task_work *work,
                                    struct player *record,
                                    struct player *hunter);
static void hunter_start_spiral(struct task_work *work, struct player *record,
                                struct player *hunter);
static void hunter_start_sunglasses(struct task_work *work,
                                    struct player *record);
static void hunter_start_earthquake(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record,
                                    struct player *hunter);
static void hunter_start_earthflame(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record,
                                    struct player *hunter);

/* the voices of a Knuckles look and of a Rouge look. VOICE_KNOCKED_ is a
 * knock back's on STAGE_VERSUS_KNUCKLES. VOICE_CAPTURE_ is
 * PLAYER_REQUEST_CAPTURE's, which an earthflame's victim says too.
 * VOICE_UNK_23_ is PLAYER_REQUEST_UNK_23's */
#define VOICE_KNOCKED_KNUCKLES 0x844
#define VOICE_KNOCKED_ROUGE 0x89b
#define VOICE_CAPTURE_KNUCKLES 0x510
#define VOICE_CAPTURE_ROUGE 0x511
#define VOICE_UNK_23_KNUCKLES 0x512
#define VOICE_UNK_23_ROUGE 0x513

Bool hunter_take_request(struct task_work *work, struct player_motion *motion,
                         struct player *record, struct player *hunter)
{
    int i;

    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH;
        record->anim.next = ANIM_LAUNCH;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_FREE_FALL:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FREE_FALL;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        hunter->by_character.hunter.steps = 0;
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_UNK_23;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        hunter->by_character.hunter.steps = 0;
        break;
    case PLAYER_REQUEST_RUN_PATH:
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        record->path_distance = 0.0F;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_LAUNCH_BALL:
        work->mode = PLAYER_MODE_JUMP;
        record->anim.next = ANIM_JUMP_RISE;
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        record->jump_steps = record->consts.jump_hold_steps;
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
        fun_8c05bce0();
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RELEASE:
        player_stand(work, motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_JOIN_PATH:
        if (work->mode == PLAYER_MODE_RUN ||
            record->spd.x > record->consts.running_speed) {
            record->path_distance = 0.0F;
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_KNOCK_BACK:
        work->mode = PLAYER_MODE_CRASH;
        if (record->request_arg == ANIM_KNOCKED_DOWN) {
            record->anim.next = ANIM_KNOCKED_DOWN;
        } else {
            record->anim.next = ANIM_CRASH;
        }
        if (stage_current_id() == STAGE_VERSUS_KNUCKLES) {
            for (i = 0; i < VOICES; i++) {
                if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                    voice_stop(i);
                }
            }
            /* @hack spelling: a switch, where an if and an else if would
             * do */
            switch (record->kind) {
            case PLAYER_KNUCKLES:
                voice_play(VOICE_KNOCKED_KNUCKLES, RIVAL_VOICE_PRIORITY);
                break;
            case PLAYER_ROUGE:
                voice_play(VOICE_KNOCKED_ROUGE, RIVAL_VOICE_PRIORITY);
                break;
            }
        } else if (record->kind == PLAYER_KNUCKLES) {
            sound_play_at(CRY_CRASH_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_ROUGE) {
            sound_play_at(CRY_CRASH_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        vibration_start(record->slot, CRASH_VIBRATION_POWER,
                        CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
        crash_stars_burst(record->slot);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        /* @bug memory: writes the handed task's work with no test for null,
         * at 0x8c080e12, and hunter_take_held_request does too, at
         * 0x8c081364. hunter_test_throw and hunter_test_throw_at test a held
         * task's work for null, at 0x8c081cc2 and 0x8c08201e */
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_CAPTURE:
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        hunter->by_character.hunter.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (record->kind == PLAYER_KNUCKLES) {
            voice_play_priority_2(VOICE_CAPTURE_KNUCKLES);
        } else if (record->kind == PLAYER_ROUGE) {
            voice_play_priority_2(VOICE_CAPTURE_ROUGE);
        }
        break;
    case PLAYER_REQUEST_UNK_23:
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        hunter->by_character.hunter.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        if (record->kind == PLAYER_KNUCKLES) {
            voice_play_priority_2(VOICE_UNK_23_KNUCKLES);
        } else if (record->kind == PLAYER_ROUGE) {
            voice_play_priority_2(VOICE_UNK_23_ROUGE);
        }
        break;
    case PLAYER_REQUEST_CURRENT_PATH:
        work->mode = PLAYER_MODE_SWIM;
        record->anim.next = ANIM_UNK_23;
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
    case PLAYER_REQUEST_DROWN:
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
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
        record->anim.next = ANIM_MISS;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

Bool hunter_take_held_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *hunter)
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
        record->path_distance = 0.0F;
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_LAUNCH_BALL:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_JUMP;
        record->anim.next = ANIM_JUMP_RISE;
        work->flags |= PLAYER_WORK_ATTACK | PLAYER_WORK_BALL;
        record->jump_steps = record->consts.jump_hold_steps;
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
        fun_8c05bce0();
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_RELEASE:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_JOIN_PATH:
        if (work->mode == PLAYER_MODE_UNK_19 ||
            record->spd.x > record->consts.running_speed) {
            record->path_distance = 0.0F;
            work->flags |= PLAYER_WORK_ON_PATH;
        }
        return FALSE;
    case PLAYER_REQUEST_CAPTURE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        hunter->by_character.hunter.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        if (record->kind == PLAYER_KNUCKLES) {
            voice_play_priority_2(VOICE_CAPTURE_KNUCKLES);
        } else if (record->kind == PLAYER_ROUGE) {
            voice_play_priority_2(VOICE_CAPTURE_ROUGE);
        }
        break;
    case PLAYER_REQUEST_UNK_23:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CAUGHT;
        record->anim.next = ANIM_CAUGHT;
        hunter->by_character.hunter.steps = PLAYER_STRUGGLE;
        player_stop(motion, record);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        if (record->kind == PLAYER_KNUCKLES) {
            voice_play_priority_2(VOICE_UNK_23_KNUCKLES);
        } else if (record->kind == PLAYER_ROUGE) {
            voice_play_priority_2(VOICE_UNK_23_ROUGE);
        }
        break;
    case PLAYER_REQUEST_KNOCK_BACK:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_CRASH;
        record->anim.next = ANIM_CRASH;
        if (record->kind == PLAYER_KNUCKLES) {
            sound_play_at(CRY_CRASH_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_ROUGE) {
            sound_play_at(CRY_CRASH_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        vibration_start(record->slot, CRASH_VIBRATION_POWER,
                        CRASH_VIBRATION_FREQ, VIBRATION_UNTIMED);
        crash_stars_burst(record->slot);
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_HAND:
        work->mode = PLAYER_MODE_STAND_HOLDING;
        record->anim.next = ANIM_HOLD_STAND;
        record->held = record->handed;
        record->handed = NULL;
        record->held->work->flags |= WORK_HELD_BY_PLAYER;
        work->flags |= PLAYER_WORK_HOLDING;
        work->flags &= ~PLAYER_WORK_UNK_8;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_SCARE:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        break;
    case PLAYER_REQUEST_BREATHE:
        record->underwater_steps = 0;
        break;
    case PLAYER_REQUEST_DROWN:
        player_release_held(work, motion, record);
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &= ~PLAYER_WORK_ON_PATH;
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
        record->anim.next = ANIM_MISS;
        work->flags &= ~PLAYER_WORK_ON_PATH;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/* PLAYER_MODE_ROUGE_RIDE_FERRY's setter, which SHC writes into
 * hunter_take_action, its one caller */
/* @hack spelling: the two statements as a function of their own, where the
 * statements in the caller's case would do */
#pragma inline(rouge_start_ride_ferry)
static void rouge_start_ride_ferry(struct task_work *work,
                                   struct player_motion *motion,
                                   struct player *record)
{
    player_stop(motion, record);
    work->mode = PLAYER_MODE_ROUGE_RIDE_FERRY;
}

Bool hunter_take_action(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *hunter)
{
    Sint8 *actions = record->actions;
    int count;
    int action;
    int i;
    Sint8 *offered;

    count = record->action_count;
    if (!pad_action_pressed[record->slot]) {
        if (work->mode == PLAYER_MODE_HUNTER_DIVE) {
            /* @hack spelling: the scan for PLAYER_MODE_HUNTER_DIG as a loop of
             * gotos, where a for loop would do */
            i = 0;
            offered = record->actions;
            if (i < count) {
            scan:
                if (*offered == PLAYER_MODE_HUNTER_DIG) {
                    hunter_dig(work, motion, record, hunter);
                    sound_stop(SOUND_DIVE, work);
                    return TRUE;
                }
                i++;
                /* @hack spelling: offered = offered + 1, where offered++
                 * would do */
                offered = offered + 1;
                if (i < count) {
                    goto scan;
                }
            }
            /* @hack redundant: a loop that never runs */
            while (0) {
                sound_stop(SOUND_DIVE, work);
            }
        }
        return FALSE;
    }
    if (count == 0) {
        return FALSE;
    }
    action = record->action;
    for (i = 0; i < count; i++) {
        if (action == *actions) {
            break;
        }
        actions++;
    }
    if (count == i) {
        action = record->actions[0];
    }
    record->action_taken = action;
    switch (action) {
    case PLAYER_MODE_GRAB_PUSHABLE:
        player_hold(work, record);
        return TRUE;
    case PLAYER_MODE_UNK_17:
        player_grab_shakable(work, record);
        return TRUE;
    case PLAYER_MODE_PICK_UP:
        player_pick_up(work, record);
        return TRUE;
    case PLAYER_MODE_FALL:
        player_fall(work, record);
        return TRUE;
    case PLAYER_MODE_THROW:
        player_throw_held(work, record);
        return TRUE;
    case PLAYER_MODE_AIR_THROW:
        player_air_throw_held(work, record);
        return TRUE;
    case PLAYER_MODE_UNK_14:
        player_set_down_held(work, motion, record);
        return TRUE;
    case PLAYER_MODE_HUNTER_DIG:
        hunter_dig(work, motion, record, hunter);
        return TRUE;
    case PLAYER_MODE_HUNTER_DIG_FAIL:
        hunter_dig_fail(work, record);
        return TRUE;
    case PLAYER_MODE_HUNTER_WALL_DIG:
        hunter_dig_wall(work, motion, record, hunter);
        return TRUE;
    case PLAYER_MODE_HUNTER_WALL_DIG_FAIL:
        hunter_dig_wall_fail(work, record);
        return TRUE;
    case PLAYER_MODE_HUNTER_DIVE:
        hunter_start_dive(work, record, hunter);
        return TRUE;
    case PLAYER_MODE_LET_GO:
        player_release_and_stand(work, motion, record);
        return TRUE;
    case PLAYER_MODE_UNK_2A:
        player_call(work, record);
        return TRUE;
    case PLAYER_MODE_PULL_HEAVY:
        player_pull_heavy(work, record);
        return TRUE;
    case PLAYER_MODE_ASK_HINT:
        player_ask_hint(work, record);
        return TRUE;
    case PLAYER_MODE_CANT_PLAY_FLUTE:
        player_cant_play_flute(work, record);
        return TRUE;
    case PLAYER_MODE_PLAY_FLUTE:
        player_play_flute(work, record);
        return TRUE;
    case PLAYER_MODE_PLAY_FLUTE_AGAIN:
        player_play_flute_again(work, record);
        return TRUE;
    case PLAYER_MODE_PET_CHAO:
        player_pet_chao(work, record);
        return TRUE;
    case PLAYER_MODE_THROW_AT_TOUCH:
        player_throw_at_touch(work, record);
        return TRUE;
    case PLAYER_MODE_PULL_BURIED:
        player_pull_buried(work, record);
        return TRUE;
    case PLAYER_MODE_HUNTER_ATTACK_1:
        hunter_start_attack(work, record, hunter);
        return TRUE;
    case PLAYER_MODE_HUNTER_RUN_ATTACK_1:
        hunter_start_run_attack(work, record, hunter);
        return TRUE;
    case PLAYER_MODE_HUNTER_SPIRAL:
        hunter_start_spiral(work, record, hunter);
        return TRUE;
    case ACTION_SUNGLASSES:
        hunter_start_sunglasses(work, record);
        return TRUE;
    case ACTION_EARTHQUAKE:
        hunter_start_earthquake(work, motion, record, hunter);
        return TRUE;
    case ACTION_EARTHFLAME:
        hunter_start_earthflame(work, motion, record, hunter);
        return TRUE;
    case ACTION_POWER_FLASH:
        hunter_power_flash(work, motion, record, hunter);
        return TRUE;
    case PLAYER_MODE_ROUGE_RIDE_FERRY:
        rouge_start_ride_ferry(work, motion, record);
        return TRUE;
    case PLAYER_MODE_ROUGE_RIDE_SWIMMER:
        rouge_start_ride_swimmer(work, motion, record);
        return TRUE;
    default:
        return FALSE;
    }
}

/* the spd.y a dive starts at */
#define DIVE_SPEED (-5.0F)

static void hunter_start_dive(struct task_work *work, struct player *record,
                              struct player *hunter)
{
    record->anim.next = ANIM_DIVE;
    work->mode = PLAYER_MODE_HUNTER_DIVE;
    work->flags |= PLAYER_WORK_ATTACK;
    record->spd.y = DIVE_SPEED;
    hunter->by_character.hunter.steps = 0;
    sound_play_for(SOUND_DIVE, work, 1, 0, 2);
}

Bool hunter_offer_dive(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    int mode = work->mode;

    if (work->flags & PLAYER_WORK_GROUNDED) {
        return FALSE;
    }
    if (hunter_in_deep_water(work, motion, record)) {
        return FALSE;
    }
    if (mode != PLAYER_MODE_JUMP && mode != PLAYER_MODE_HUNTER_GLIDE) {
        return FALSE;
    }
    if (record->anim.current == ANIM_UNK_23) {
        return FALSE;
    }
    OFFER(record, PLAYER_MODE_HUNTER_DIVE);
    return TRUE;
}

/* the frame of PLAYER_MODE_PICK_UP's animation from which the hands take the
 * task: Knuckles's, and Rouge's */
#define KNUCKLES_PICK_UP_FRAME 2.0F
#define ROUGE_PICK_UP_FRAME 6.0F

Bool hunter_test_pick_up(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    int flags;
    int character;

    player_approach_touched(work, motion, record);
    character = record->character;
    if ((character == PLAYER_KNUCKLES &&
         record->anim.frame >= KNUCKLES_PICK_UP_FRAME) ||
        (character == PLAYER_ROUGE &&
         record->anim.frame >= ROUGE_PICK_UP_FRAME)) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((flags = player_action_flags(work, record)) != 0 &&
            player_face_offer(work, motion, record) == PLAYER_WALL_HEAD_ON) {
            if (flags & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_STAND_HOLDING;
                if (record->kind == PLAYER_KNUCKLES) {
                    sound_play_at(CRY_PICK_UP_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                                  &work->pos);
                } else if (record->kind == PLAYER_ROUGE) {
                    sound_play_at(CRY_PICK_UP_ROUGE, NULL, 0, SOUND_LOUDEST,
                                  &work->pos);
                }
                sound_play(PLAYER_SOUND_PICK_UP, NULL, 0, 0);
                record->held = record->hold_offer;
                record->held->work->flags |= WORK_HELD_BY_PLAYER;
                work->flags |= PLAYER_WORK_HOLDING;
                work->flags &= ~PLAYER_WORK_UNK_8;
                return TRUE;
            }
        } else {
            /* @hack redundant: an empty arm */
        }
        player_stand(work, motion, record);
    }
    return FALSE;
}

/* the chance that a pass of ANIM_PULL's loop goes on to ANIM_PULL_LIFT
 * without the stick */
#define PULL_LIFT_CHANCE 0.1F

Bool hunter_test_pull(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    int flags;

    player_approach_touched(work, motion, record);
    if (record->anim.current == ANIM_PULL &&
        record->anim.frame < record->anim.actions[record->anim.current].rate &&
        (player_read_stick(work, record, NULL, NULL) ||
         (Float)rand() * RAND_SCALE < PULL_LIFT_CHANCE)) {
        record->anim.next = ANIM_PULL_LIFT;
        record->anim.after = ANIM_HOLD_STAND;
        return FALSE;
    }
    if (record->anim.current == ANIM_PULL_LIFT) {
        if (!(work->flags & PLAYER_WORK_HOLDING)) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((flags = player_action_flags(work, record)) != 0 &&
                player_face_offer(work, motion, record) ==
                    PLAYER_WALL_HEAD_ON) {
                if (flags & ACTION_FLAG_1000) {
                    if (record->kind == PLAYER_KNUCKLES) {
                        sound_play_at(CRY_PICK_UP_KNUCKLES, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    } else if (record->kind == PLAYER_ROUGE) {
                        sound_play_at(CRY_PICK_UP_ROUGE, NULL, 0, SOUND_LOUDEST,
                                      &work->pos);
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
            } else {
                /* @hack redundant: an empty arm */
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

/* a throw on the ground lets go of the task from its frame THROW_FRAME. one
 * in the air lets go from AIR_THROW_FRAME, and ANIM_THROW_AT_TOUCH from
 * THROW_AT_TOUCH_FRAME */
#define THROW_FRAME 3.0F
#define AIR_THROW_FRAME 2.0F
#define THROW_AT_TOUCH_FRAME 7.0F
/* the frames of the set-down's animation up to which the task is let go:
 * Knuckles's, and Rouge's */
#define KNUCKLES_SET_DOWN_FRAME 2.0F
#define ROUGE_SET_DOWN_FRAME 6.0F
/* the speed a throw at a target gives the task. a throw ahead gives it
 * (THROW_AHEAD_X, THROW_AHEAD_Y, 0) in the hunter's own axes */
#define THROW_SPEED 3.0F
#define THROW_AHEAD_X 2.0F
#define THROW_AHEAD_Y 0.8F

Bool hunter_test_throw(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    struct near_target *target;
    struct task_work *held;
    struct task_work *nearest;
    Float distance;
    struct collision_shape *shape;
    NJS_VECTOR speed;

    if (record->slot == 0) {
        target = near_targets;
    } else {
        target = near_targets_player1;
    }
    if (!record->held) {
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
        target->work) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            /* @bug logic: aims at the first near target whatever its kind
             * and id (0x8c081cce). the loop replaces it only with a nearer
             * item or enemy whose id isn't FALL_KIND_NORMAL, as its tests at
             * 0x8c081ce2 and 0x8c081cf0 say */
            nearest = target->work;
            distance = target->distance;
            while (target->work) {
                if ((target->work->collision->kind == COLLISION_ITEM ||
                     target->work->collision->kind == COLLISION_ENEMY) &&
                    target->work->id != FALL_KIND_NORMAL &&
                    target->distance < distance) {
                    distance = target->distance;
                    nearest = target->work;
                }
                target++;
            }
            shape = nearest->collision->shapes;
            speed.x = nearest->pos.x + shape->center.x - work->pos.x;
            speed.y = nearest->pos.y + shape->center.y - work->pos.y;
            speed.z = nearest->pos.z + shape->center.z - work->pos.z;
            speed.x = njSqrt((speed.x * speed.x) + (speed.z * speed.z));
            speed.z = 0.0F;
            njUnitVector(&speed);
            speed.x *= THROW_SPEED;
            speed.y *= THROW_SPEED;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (record->kind == PLAYER_KNUCKLES) {
            sound_play_at(CRY_THROW_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_ROUGE) {
            sound_play_at(CRY_THROW_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if ((record->anim.next == ANIM_THROW &&
                record->anim.frame >= THROW_FRAME) ||
               (record->anim.next == ANIM_AIR_THROW &&
                record->anim.frame >= AIR_THROW_FRAME)) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((held = record->held->work) != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            speed.x = THROW_AHEAD_X;
            speed.y = THROW_AHEAD_Y;
            speed.z = 0.0F;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (record->kind == PLAYER_KNUCKLES) {
            sound_play_at(CRY_THROW_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_ROUGE) {
            sound_play_at(CRY_THROW_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if (record->anim.next == ANIM_SET_DOWN &&
               ((record->character == PLAYER_KNUCKLES &&
                 record->anim.frame <= KNUCKLES_SET_DOWN_FRAME) ||
                (record->character == PLAYER_ROUGE &&
                 record->anim.frame <= ROUGE_SET_DOWN_FRAME))) {
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

Bool hunter_test_throw_at(struct task *task, struct task_work *work,
                          struct player_motion *motion, struct player *record)
{
    struct task *target;
    struct task_work *held;
    struct task_work *aim;
    struct collision_shape *shape;
    NJS_VECTOR speed;

    if (!record->held) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            work->flags &= ~PLAYER_WORK_HOLDING;
            return TRUE;
        }
        return FALSE;
    }
    if (record->anim.next == ANIM_THROW_AT_TOUCH &&
        record->anim.frame >= THROW_AT_TOUCH_FRAME &&
        (target = collision_first_with_id(task, SHAPE_ID_FACED)) != NULL) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            aim = target->work;
            shape = aim->collision->shapes;
            speed.x = aim->pos.x + shape->center.x - held->pos.x;
            speed.y = aim->pos.y + shape->center.y - held->pos.y;
            speed.z = aim->pos.z + shape->center.z - held->pos.z;
            speed.x = njSqrt((speed.x * speed.x) + (speed.z * speed.z));
            speed.z = 0.0F;
            njUnitVector(&speed);
            speed.x *= THROW_SPEED;
            speed.y *= THROW_SPEED;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (record->kind == PLAYER_KNUCKLES) {
            sound_play_at(CRY_THROW_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_ROUGE) {
            sound_play_at(CRY_THROW_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    } else if (record->anim.next == ANIM_THROW_AT_TOUCH &&
               record->anim.frame >= THROW_AT_TOUCH_FRAME) {
        held = record->held->work;
        if (held != NULL) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
            speed.x = THROW_AHEAD_X;
            speed.y = THROW_AHEAD_Y;
            speed.z = 0.0F;
            turn_to_world(work, &speed);
            TASK_MOTION_OF(record->held)->spd = speed;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        record->held = NULL;
        if (record->kind == PLAYER_KNUCKLES) {
            sound_play_at(CRY_THROW_KNUCKLES, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        } else if (record->kind == PLAYER_ROUGE) {
            sound_play_at(CRY_THROW_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
        }
        sound_play(PLAYER_SOUND_THROW, NULL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

void hunter_drop_held(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    struct task_work *held;

    if (!record->held) {
        return;
    }
    held = record->held->work;
    if (player_action_flags(work, record) & ACTION_FLAG_200) {
        return;
    } else if (work->collision->hit &&
               held == work->collision->hit->owner->work) {
        return;
    }
    player_release_held(work, motion, record);
    if (record->held) {
        return;
    }
    if (work->flags & PLAYER_WORK_GROUNDED) {
        player_stand(work, motion, record);
    } else {
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
    }
}

/* a thrown task lies HOLD_REACH out along the hand's z. any held task lies
 * below the hands along their down: KNUCKLES_HOLD_OUT for Knuckles, and
 * ROUGE_HOLD_OUT for Rouge */
#define HOLD_REACH 2.5F
#define KNUCKLES_HOLD_OUT 1.2F
#define ROUGE_HOLD_OUT 2.0F

/* put the held task between the hands, or at a hand in a throw. it does so
 * where player_action_flags gives the task ACTION_FLAG_200 */
static void hunter_place_held(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    struct task_work *held;
    Float drop;
    NJS_POINT3 pos;
    NJS_VECTOR down;

    if (!record->held) {
        return;
    }
    if (!(player_action_flags(work, record) & ACTION_FLAG_200)) {
        return;
    }
    held = record->held->work;
    if (record->character == PLAYER_KNUCKLES) {
        drop = KNUCKLES_HOLD_OUT;
    } else {
        drop = ROUGE_HOLD_OUT;
    }
    if (record->anim.next == ANIM_THROW_AT || record->anim.next == ANIM_THROW ||
        record->anim.next == ANIM_AIR_THROW_AT ||
        record->anim.next == ANIM_AIR_THROW) {
        pos.x = record->unk_1e8.x + (record->unk_26c.x * HOLD_REACH);
        pos.y = record->unk_1e8.y + (record->unk_26c.y * HOLD_REACH);
        pos.z = record->unk_1e8.z + (record->unk_26c.z * HOLD_REACH);
        pos.x += record->unk_284.x * drop;
        pos.y += record->unk_284.y * drop;
        pos.z += record->unk_284.z * drop;
    } else {
        pos.x = (record->unk_1e8.x + record->unk_1f4.x) * 0.5F;
        pos.y = (record->unk_1e8.y + record->unk_1f4.y) * 0.5F;
        pos.z = (record->unk_1e8.z + record->unk_1f4.z) * 0.5F;
        down.x = record->unk_284.x + record->unk_290.x;
        down.y = record->unk_284.y + record->unk_290.y;
        down.z = record->unk_284.z + record->unk_290.z;
        if (njScalor(&down) == 0.0F) {
            down.x = down.y = down.z = 0.0F;
        } else {
            njUnitVector(&down);
        }
        pos.x += down.x * drop;
        pos.y += down.y * drop;
        pos.z += down.z * drop;
    }
    held->pos = pos;
}

/* the steps the spiral stays offered after the stick turns a full circle */
#define SPIRAL_OFFER_STEPS 60
/* the stick's history keeps the high byte of each heading */
#define HEADING_BYTE_SHIFT 8

Bool hunter_offer_attack(struct task_work *work, struct player *record,
                         struct player *hunter)
{
    Angle heading;
    int i;
    Angle turn;
    Sint8 step;
    Angle first;
    Angle last;
    Angle now;
    Angle change;
    int value;
    Sint8 *history;

    if (player_read_stick(work, record, &heading, NULL)) {
        value = heading >> HEADING_BYTE_SHIFT;
    } else {
        value = 0;
    }
    hunter->by_character.hunter
        .stick_history[hunter->by_character.hunter.stick_step &
                       (HUNTER_STICK_STEPS - 1)] = value;
    if (hunter->by_character.hunter.spin_steps == 0) {
        turn = 0;
        /* @hack spelling: the history through a local, where the record's
         * own field would do */
        history = hunter->by_character.hunter.stick_history;
        step = hunter->by_character.hunter.stick_step;
        for (i = 0; i < HUNTER_STICK_STEPS; i++, step--) {
            step &= HUNTER_STICK_STEPS - 1;
            now = history[step] << HEADING_BYTE_SHIFT;
            if (i == 0) {
                first = last = now;
            }
            change = angle_difference(now, last);
            if (turn == 0) {
                turn = change;
            } else if (turn > 0) {
                if (change < 0) {
                    break;
                }
                if (angle_difference(now, first) > 0 &&
                    angle_difference(first, last) > 0) {
                    hunter->by_character.hunter.spin_steps = SPIRAL_OFFER_STEPS;
                    break;
                }
            } else if (turn < 0) {
                if (change > 0) {
                    break;
                }
                if (angle_difference(now, first) < 0 &&
                    angle_difference(first, last) < 0) {
                    hunter->by_character.hunter.spin_steps = SPIRAL_OFFER_STEPS;
                    break;
                }
            }
            if (angle_distance(change, 0) > QUARTER_TURN) {
                break;
            }
            last = now;
        }
    } else {
        hunter->by_character.hunter.spin_steps--;
    }
    hunter->by_character.hunter.stick_step++;
    if (work->mode != PLAYER_MODE_STAND && work->mode != PLAYER_MODE_RUN &&
        work->mode != PLAYER_MODE_BRAKE &&
        work->mode != PLAYER_MODE_HUNTER_DIG_FAIL &&
        work->mode != PLAYER_MODE_SLIDE) {
        return FALSE;
    }
    if (hunter->by_character.hunter.spin_steps > 0) {
        OFFER(record, PLAYER_MODE_HUNTER_SPIRAL);
    } else if (record->spd.x <= record->consts.running_speed) {
        OFFER(record, PLAYER_MODE_HUNTER_ATTACK_1);
    } else {
        OFFER(record, PLAYER_MODE_HUNTER_RUN_ATTACK_1);
    }
    return TRUE;
}

static void hunter_end_attack(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *hunter);

/* the spd.y that PLAYER_MODE_HUNTER_ATTACK_3 and the spiral start at */
#define ATTACK_3_RISE 2.4F
#define SPIRAL_RISE 3.2F
/* PLAYER_MODE_HUNTER_RUN_ATTACK_3 ends after RUN_ATTACK_3_STEPS steps, or
 * after RUN_ATTACK_3_MIN_STEPS once the action button is let go */
#define RUN_ATTACK_3_STEPS 40
#define RUN_ATTACK_3_MIN_STEPS 10
/* the vibration_start power and freq of an attack */
#define ATTACK_VIBRATION_POWER 6
#define ATTACK_VIBRATION_FREQ 30

Bool hunter_attack_step(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *hunter)
{
    if (work->mode != PLAYER_MODE_HUNTER_ATTACK_3 &&
        work->mode != PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
        if (!(work->flags & PLAYER_WORK_GROUNDED)) {
            return FALSE;
        }
        if (work->flags & PLAYER_WORK_HOLDING) {
            return FALSE;
        }
        if (work->mode == PLAYER_MODE_HUNTER_DIVE) {
            return FALSE;
        }
    }
    hunter->by_character.hunter.steps++;
    switch (work->mode) {
    case PLAYER_MODE_HUNTER_ATTACK_1:
        if (pad_action_pressed[record->slot]) {
            hunter->by_character.hunter.next_mode = PLAYER_MODE_HUNTER_ATTACK_2;
        } else if (!pad_action_held[record->slot] &&
                   hunter->by_character.hunter.next_mode !=
                       PLAYER_MODE_HUNTER_ATTACK_2) {
            hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            work->mode = hunter->by_character.hunter.next_mode;
            if (work->mode == PLAYER_MODE_RUN) {
                hunter_end_attack(work, motion, record, hunter);
            } else {
                hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
                hunter->by_character.hunter.steps = 0;
                if (record->character == PLAYER_KNUCKLES) {
                    sound_play(SOUND_ATTACK_KNUCKLES, NULL, 0, 0);
                } else {
                    sound_play(SOUND_ATTACK_ROUGE, NULL, 0, 0);
                }
                vibration_start(record->slot, ATTACK_VIBRATION_POWER,
                                ATTACK_VIBRATION_FREQ, VIBRATION_UNTIMED);
            }
            return TRUE;
        }
        return FALSE;
    case PLAYER_MODE_HUNTER_RUN_ATTACK_1:
        if (pad_action_pressed[record->slot]) {
            hunter->by_character.hunter.next_mode =
                PLAYER_MODE_HUNTER_RUN_ATTACK_2;
        } else if (!pad_action_held[record->slot] &&
                   hunter->by_character.hunter.next_mode !=
                       PLAYER_MODE_HUNTER_RUN_ATTACK_2) {
            hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            work->mode = hunter->by_character.hunter.next_mode;
            if (work->mode == PLAYER_MODE_RUN) {
                hunter_end_attack(work, motion, record, hunter);
            } else {
                hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
                hunter->by_character.hunter.steps = 0;
                if (record->character == PLAYER_KNUCKLES) {
                    sound_play(SOUND_ATTACK_KNUCKLES, NULL, 0, 0);
                } else {
                    sound_play(SOUND_ATTACK_ROUGE, NULL, 0, 0);
                }
                vibration_start(record->slot, ATTACK_VIBRATION_POWER,
                                ATTACK_VIBRATION_FREQ, VIBRATION_UNTIMED);
            }
            return TRUE;
        }
        return FALSE;
    case PLAYER_MODE_HUNTER_RUN_ATTACK_2:
        if (pad_action_pressed[record->slot]) {
            hunter->by_character.hunter.next_mode =
                PLAYER_MODE_HUNTER_RUN_ATTACK_3;
        } else if (!pad_action_held[record->slot] &&
                   hunter->by_character.hunter.next_mode !=
                       PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
            hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            work->mode = hunter->by_character.hunter.next_mode;
            if (work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
                hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
                hunter->by_character.hunter.steps = 0;
                if (record->character == PLAYER_KNUCKLES) {
                    sound_play(SOUND_LAST_ATTACK_KNUCKLES, NULL, 0, 0);
                    if (record->kind == PLAYER_KNUCKLES) {
                        sound_play_at(CRY_LAST_ATTACK_KNUCKLES, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                } else {
                    sound_play(SOUND_LAST_ATTACK_ROUGE, NULL, 0, 0);
                    if (record->kind == PLAYER_ROUGE) {
                        sound_play_at(CRY_LAST_ATTACK_ROUGE, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                }
                vibration_start(record->slot, 4, 30, 1);
            } else {
                hunter_end_attack(work, motion, record, hunter);
            }
            return TRUE;
        }
        return FALSE;
    case PLAYER_MODE_HUNTER_ATTACK_2:
        if (pad_action_pressed[record->slot]) {
            hunter->by_character.hunter.next_mode = PLAYER_MODE_HUNTER_ATTACK_3;
        } else if (!pad_action_held[record->slot] &&
                   hunter->by_character.hunter.next_mode !=
                       PLAYER_MODE_HUNTER_ATTACK_3) {
            hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
        }
        if (record->anim.flags & PLAYER_ANIM_ENDING) {
            work->mode = hunter->by_character.hunter.next_mode;
            if (work->mode == PLAYER_MODE_HUNTER_ATTACK_3) {
                hunter->by_character.hunter.next_mode = PLAYER_MODE_RUN;
                hunter->by_character.hunter.steps = 0;
                record->spd.y = ATTACK_3_RISE;
                if (record->character == PLAYER_KNUCKLES) {
                    sound_play(SOUND_LAST_ATTACK_KNUCKLES, NULL, 0, 0);
                    if (record->kind == PLAYER_KNUCKLES) {
                        sound_play_at(CRY_LAST_ATTACK_KNUCKLES, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                } else {
                    sound_play(SOUND_LAST_ATTACK_ROUGE, NULL, 0, 0);
                    if (record->kind == PLAYER_ROUGE) {
                        sound_play_at(CRY_LAST_ATTACK_ROUGE, NULL, 0,
                                      SOUND_LOUDEST, &work->pos);
                    }
                }
                vibration_start(record->slot, 3, 15, 1);
            } else {
                hunter_end_attack(work, motion, record, hunter);
            }
            return TRUE;
        }
        return FALSE;
    case PLAYER_MODE_HUNTER_ATTACK_3:
        if (record->spd.y > 0.0F) {
            return FALSE;
        }
        hunter_end_attack(work, motion, record, hunter);
        return TRUE;
    case PLAYER_MODE_HUNTER_RUN_ATTACK_3:
        if (hunter->by_character.hunter.steps > RUN_ATTACK_3_STEPS ||
            (hunter->by_character.hunter.steps > RUN_ATTACK_3_MIN_STEPS &&
             !pad_action_held[record->slot])) {
            hunter_end_attack(work, motion, record, hunter);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/* start the chain of attacks on the ground, its first in
 * PLAYER_MODE_HUNTER_ATTACK_1 */
static void hunter_start_attack(struct task_work *work, struct player *record,
                                struct player *hunter)
{
    work->mode = PLAYER_MODE_HUNTER_ATTACK_1;
    record->anim.next = ANIM_ATTACK_1;
    hunter->by_character.hunter.steps = 0;
    work->flags |= PLAYER_WORK_ATTACK;
    if (record->character == PLAYER_KNUCKLES) {
        sound_play(SOUND_ATTACK_KNUCKLES, NULL, 0, 0);
    } else {
        sound_play(SOUND_ATTACK_ROUGE, NULL, 0, 0);
    }
    vibration_start(record->slot, ATTACK_VIBRATION_POWER, ATTACK_VIBRATION_FREQ,
                    VIBRATION_UNTIMED);
}

/* start the chain of attacks on the run, its first in
 * PLAYER_MODE_HUNTER_RUN_ATTACK_1 */
static void hunter_start_run_attack(struct task_work *work,
                                    struct player *record,
                                    struct player *hunter)
{
    work->mode = PLAYER_MODE_HUNTER_RUN_ATTACK_1;
    record->anim.next = ANIM_RUN_ATTACK_1;
    hunter->by_character.hunter.steps = 0;
    work->flags |= PLAYER_WORK_ATTACK;
    if (record->character == PLAYER_KNUCKLES) {
        sound_play(SOUND_ATTACK_KNUCKLES, NULL, 0, 0);
    } else {
        sound_play(SOUND_ATTACK_ROUGE, NULL, 0, 0);
    }
    vibration_start(record->slot, ATTACK_VIBRATION_POWER, ATTACK_VIBRATION_FREQ,
                    VIBRATION_UNTIMED);
}

/* start the spiral, which rises straight up in PLAYER_MODE_HUNTER_SPIRAL */
static void hunter_start_spiral(struct task_work *work, struct player *record,
                                struct player *hunter)
{
    work->mode = PLAYER_MODE_HUNTER_SPIRAL;
    record->anim.next = ANIM_SPIRAL;
    hunter->by_character.hunter.steps = 0;
    record->spd.x = record->spd.z = 0.0F;
    record->spd.y = SPIRAL_RISE;
    work->flags |= PLAYER_WORK_ATTACK;
    if (record->kind == PLAYER_KNUCKLES) {
        sound_play_at(CRY_SPIRAL_KNUCKLES, NULL, 0, SOUND_LOUDEST, &work->pos);
    } else if (record->kind == PLAYER_ROUGE) {
        sound_play_at(CRY_SPIRAL_ROUGE, NULL, 0, SOUND_LOUDEST, &work->pos);
    }
    vibration_start(record->slot, 5, 20, 1);
}

/* end an attack: run on the ground, or fall in the air */
static void hunter_end_attack(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *hunter)
{
    if (work->flags & PLAYER_WORK_GROUNDED) {
        work->mode = PLAYER_MODE_RUN;
        player_run_anim(work, motion, record);
    } else {
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
    }
    hunter->by_character.hunter.steps = 0;
    work->flags &= ~PLAYER_WORK_ATTACK;
}

/* the arc about head on within which hunter_turn_off_surface turns a
 * hunter back off a surface. outside it, the hunter turns along the
 * surface */
#define HEAD_ON_ARC EIGHTH_TURN

/* turn the hunter off the wall or floor whose normal is normal. within
 * HEAD_ON_ARC of head on, it turns back, its heading reversed and mirrored
 * about normal. outside it, it turns along the surface. the motion's aim_y
 * follows, and glide_roll becomes 0. the work's pos then moves across x and z
 * by normal. it moves again by its move since the step before, as
 * player_get_past gives it */
static void hunter_turn_off_surface(struct task_work *work,
                                    struct player *record,
                                    struct player *hunter, NJS_VECTOR *normal)
{
    struct player_motion *motion;
    NJS_POINT3 before;
    Float moved_x;
    Float moved_z;
    Angle ang;

    motion = player_motion[record->slot];
    ang = (Angle)(atan2f(normal->z, normal->x) * ANGLE_PER_RADIAN);
    if (angle_distance(work->ang.y + HALF_TURN, ang) > HEAD_ON_ARC) {
        if (angle_difference(work->ang.y + HALF_TURN, ang) > 0) {
            work->ang.y = ang + QUARTER_TURN;
        } else {
            work->ang.y = ang - QUARTER_TURN;
        }
    } else {
        work->ang.y = ang + angle_difference(work->ang.y + HALF_TURN, ang);
    }
    motion->aim_y = work->ang.y;
    hunter->by_character.hunter.glide_roll = 0;
    player_get_past(record->slot, 1, &before, NULL);
    moved_x = work->pos.x - before.x;
    moved_z = work->pos.z - before.z;
    work->pos.x += moved_x + normal->x;
    work->pos.z += moved_z + normal->z;
}

/* a probe's contact whose normal's y lies FLOOR_NORMAL_Y or more from 0 is a
 * floor's or a ceiling's */
#define FLOOR_NORMAL_Y 0.71F
/* the world flags of an object the hunter can't grab. a glide turns off such an
 * object, and a climb can't hold on to it */
#define UNGRABBABLE (WORLD_MOVING | GROUND_DEFLECTS | WORLD_NO_CLIMB)

int hunter_probe_glide(struct task_work *work, struct player_motion *motion,
                       struct player *record, struct player *hunter)
{
    struct polygon_ball ball;
    NJS_VECTOR back;
    Bool met;
    struct world_object *object;
    struct polygon_contact *contact;
    int i;
    Sint16 j;

    if (work->collision && (work->collision->flags & COLLISION_PUSHED)) {
        if (work->collision->kind == COLLISION_PLAYER ||
            work->collision->kind == COLLISION_ATTACK ||
            work->collision->kind == COLLISION_ITEM ||
            work->collision->kind == COLLISION_ENEMY) {
            return HUNTER_PROBE_CLEAR;
        }
        work->flags &= ~PLAYER_WORK_ATTACK;
        work->flags &= ~PLAYER_WORK_BALL;
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
        hunter->by_character.hunter.steps = 0;
        return HUNTER_PROBE_BLOCKED;
    }
    ball.radius = record->consts.height * 0.5F;
    ball.hit.ang_x = work->ang.x;
    ball.hit.ang_z = work->ang.z;
    /* @bug memory: reads through null for a work without a collision
     * record. the test of COLLISION_PUSHED at 0x8c082d7c lets it through, and
     * the read at 0x8c082e40 takes it */
    ball.pos.x =
        work->collision->shapes->center.x + (ball.radius * njCos(work->ang.y));
    ball.pos.y = work->collision->shapes->center.y;
    ball.pos.z =
        work->collision->shapes->center.z + (ball.radius * njSin(work->ang.y));
    ball.speed = motion->spd;
    polygon_collect_near(&ball);
    for (i = world_count, object = world_objects; i > 0; i--, object++) {
        if ((object->flags & WORLD_FLAT) &&
            polygon_sweep(&ball, object->object, object->flags, FALSE) ==
                TRUE) {
            return HUNTER_PROBE_FLAT;
        }
    }
    met = FALSE;
    for (i = world_count, object = world_objects; i > 0; i--, object++) {
        if (object->flags & WORLD_FLAT) {
            continue;
        }
        if (polygon_sweep(&ball, object->object, object->flags, FALSE) ==
            TRUE) {
            met = TRUE;
            if (object->flags & UNGRABBABLE) {
                hunter_turn_off_surface(work, record, hunter, &ball.hit.normal);
                return HUNTER_PROBE_BLOCKED;
            }
            for (j = ball.count, contact = polygon_contacts; j > 0;
                 j--, contact++) {
                if (contact->hit.normal.y <= -FLOOR_NORMAL_Y ||
                    contact->hit.normal.y >= FLOOR_NORMAL_Y) {
                    if (contact->hit.normal.y >= 0.0F) {
                        hunter_turn_off_surface(work, record, hunter,
                                                &contact->hit.normal);
                        return HUNTER_PROBE_BLOCKED;
                    } else {
                        back = contact->hit.pos;
                        njSubVector(&back, &work->collision->shapes->center);
                        njAddVector(&work->pos, &back);
                        return HUNTER_PROBE_BLOCKED;
                    }
                }
            }
        }
    }
    if (met == TRUE) {
        if (player_collide(work, motion, record)) {
            return HUNTER_PROBE_WALL;
        }
        return HUNTER_PROBE_BLOCKED;
    }
    return HUNTER_PROBE_CLEAR;
}

int hunter_probe_climb(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    struct polygon_ball ball;
    Bool met;
    struct world_object *object;
    struct polygon_contact *contact;
    int i;
    Sint16 j;

    if (work->collision && (work->collision->flags & COLLISION_PUSHED)) {
        return HUNTER_PROBE_BLOCKED;
    }
    ball.radius = record->consts.height * 0.5F;
    ball.hit.ang_x = work->ang.x;
    ball.hit.ang_z = work->ang.z;
    /* @bug memory: reads through null for a work without a collision
     * record. the test of COLLISION_PUSHED at 0x8c083030 lets it through, and
     * the read at 0x8c083060 takes it */
    ball.pos.x =
        work->collision->shapes->center.x + (ball.radius * njCos(work->ang.y));
    ball.pos.y = work->collision->shapes->center.y;
    ball.pos.z =
        work->collision->shapes->center.z + (ball.radius * njSin(work->ang.y));
    ball.speed = motion->spd;
    polygon_collect_near(&ball);
    for (i = world_count, object = world_objects; i > 0; i--, object++) {
        if ((object->flags & WORLD_FLAT) &&
            polygon_sweep(&ball, object->object, object->flags, FALSE) ==
                TRUE) {
            return HUNTER_PROBE_FLAT;
        }
    }
    met = FALSE;
    for (i = world_count, object = world_objects; i > 0; i--, object++) {
        if (object->flags & WORLD_FLAT) {
            continue;
        }
        if (polygon_sweep(&ball, object->object, object->flags, FALSE) ==
            TRUE) {
            met = TRUE;
            if (object->flags & UNGRABBABLE) {
                return HUNTER_PROBE_BLOCKED;
            }
            for (j = ball.count, contact = polygon_contacts; j > 0;
                 j--, contact++) {
                if (contact->hit.normal.y <= -FLOOR_NORMAL_Y ||
                    contact->hit.normal.y >= FLOOR_NORMAL_Y) {
                    return HUNTER_PROBE_BLOCKED;
                }
            }
        }
    }
    if (met == TRUE) {
        if (player_collide(work, motion, record)) {
            return HUNTER_PROBE_WALL;
        }
        return HUNTER_PROBE_BLOCKED;
    }
    return HUNTER_PROBE_CLEAR;
}

/* hunter_probe_steep's nine points lie STEEP_GRID apart along x and z
 * about the first shape's center */
#define STEEP_GRID 1.5F

Bool hunter_probe_steep(struct task_work *work, struct player_motion *motion,
                        struct player *record)
{
    struct polygon_ball ball;
    struct world_object *object;
    Float dx;
    Float dz;
    int i;

    ball.radius = record->consts.height;
    ball.hit.ang_x = work->ang.x;
    ball.hit.ang_z = work->ang.z;
    ball.pos = work->collision->shapes->center;
    ball.speed.x = ball.speed.y = ball.speed.z = 0.0F;
    polygon_collect_near(&ball);
    /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
    for (dz = -STEEP_GRID; dz <= STEEP_GRID; dz += STEEP_GRID) {
        /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
        for (dx = -STEEP_GRID; dx <= STEEP_GRID; dx += STEEP_GRID) {
            ball.pos = work->collision->shapes->center;
            ball.pos.x += dx;
            ball.pos.z += dz;
            for (i = world_count, object = world_objects; i > 0;
                 i--, object++) {
                if (polygon_sweep(&ball, object->object, object->flags,
                                  FALSE) == TRUE) {
                    if (ball.hit.normal.y < HUNTER_STEEP_Y || ball.count > 1) {
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

void hunter_climb_anim(struct task_work *work, struct player *record)
{
    if (record->anim.current == ANIM_GRAB_WALL ||
        record->anim.current == ANIM_CLIMB_OVER ||
        record->anim.current == ANIM_DIG_WALL_END) {
        return;
    }
    if (!(work->flags & PLAYER_WORK_ON_GROUND) ||
        (record->spd.x == 0.0F && record->spd.z == 0.0F)) {
        record->anim.next = ANIM_CLIMB_STILL;
    } else if (_builtin_fabsf(record->spd.x) > _builtin_fabsf(record->spd.z)) {
        record->anim.next = ANIM_CLIMB_X;
    } else if (record->spd.z > 0.0F) {
        record->anim_speed = record->spd.z;
        record->anim.next = ANIM_CLIMB_Z;
    } else {
        record->anim_speed = -record->spd.z;
        record->anim.next = ANIM_CLIMB_MINUS_Z;
    }
}

/* the glide's front shape lies GLIDE_FRONT_REACH ahead of the body's center.
 * its back one lies GLIDE_BACK_REACH behind it, and GLIDE_BACK_DROP lower */
#define GLIDE_FRONT_REACH 2.0F
#define GLIDE_BACK_REACH 4.75F
#define GLIDE_BACK_DROP 0.75F
/* the radius of the spiral's shape, and on STAGE_DEATH_CHAMBER */
#define SPIRAL_REACH 30.0F
#define SPIRAL_NARROW_REACH 18.0F
/* the collision record's radius is the reach of its farthest shape times
 * SHAPES_MARGIN */
#define SHAPES_MARGIN 1.01F

/* set a shape's attack level, or its guard level, 0 to HIGHEST_LEVEL */
#define SET_ATTACK(shape, level)                                               \
    ((shape)->levels = ((shape)->levels & ~SHAPE_ATTACK) |                     \
                       (SHAPE_ATTACK_LEVEL(level) & SHAPE_ATTACK))
#define SET_GUARD(shape, level)                                                \
    ((shape)->levels = ((shape)->levels & ~SHAPE_GUARD) |                      \
                       (SHAPE_GUARD_LEVEL(level) & SHAPE_GUARD))
#define HIGHEST_LEVEL 3

void hunter_set_shapes(struct task_work *work, struct player *record)
{
    struct collision_work *collision;
    struct collision_shape *shapes;
    struct collision_shape *shape;
    struct collision_shape *end;
    NJS_VECTOR reach;
    int i;
    int attack;
    int guard;
    Float size_x;
    Float size_y;
    Float size_z;
    Float far;
    Float radius;

    if (work->collision == NULL) {
        return;
    }
    shapes = work->collision->shapes;
    for (i = HIT_SHAPE; i < SHAPES; i++) {
        shapes[i].flags |= SHAPE_OFF;
    }
    shapes->flags &= ~SHAPE_HURT_NO_PUSH;
    if (record->flags & PLAYER_INVINCIBLE) {
        attack = HIGHEST_LEVEL;
        guard = HIGHEST_LEVEL;
        if (work->mode == PLAYER_MODE_HUNTER_DIVE ||
            work->mode == PLAYER_MODE_HUNTER_SPIRAL) {
            shapes->flags |= SHAPE_HURT_NO_PUSH;
        }
    } else if (work->mode == PLAYER_MODE_HUNTER_SPIRAL) {
        attack = 1;
        guard = HIGHEST_LEVEL;
    } else if (work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
        attack = 1;
        guard = 1;
        /* @hack redundant: the second test of PLAYER_MODE_HUNTER_SPIRAL, which
         * an arm above takes first */
    } else if (work->mode == PLAYER_MODE_HUNTER_DIVE ||
               work->mode == PLAYER_MODE_HUNTER_SPIRAL) {
        attack = 2;
        guard = 1;
        shapes->flags |= SHAPE_HURT_NO_PUSH;
        /* @hack spelling: one pair of stores under each of two tests, where
         * one test of either would do */
    } else if (work->flags & PLAYER_WORK_BALL) {
        attack = 1;
        guard = 1;
    } else if (work->flags & PLAYER_WORK_ATTACK) {
        attack = 1;
        guard = 1;
    } else {
        attack = 0;
        guard = 0;
    }
    SET_ATTACK(shapes, attack);
    SET_GUARD(shapes, guard);
    switch (record->anim.next) {
    case ANIM_GLIDE:
        SET_ATTACK(shapes, 0);
        SET_GUARD(shapes, HIGHEST_LEVEL);
        shapes[GLIDE_FRONT_SHAPE].flags &= ~SHAPE_OFF;
        shapes[GLIDE_FRONT_SHAPE].center.x =
            shapes->center.x + (njCos(work->ang.y) * GLIDE_FRONT_REACH);
        shapes[GLIDE_FRONT_SHAPE].center.y = shapes->center.y;
        shapes[GLIDE_FRONT_SHAPE].center.z =
            shapes->center.z + (njSin(work->ang.y) * GLIDE_FRONT_REACH);
        shapes[GLIDE_BACK_SHAPE].flags &= ~SHAPE_OFF;
        shapes[GLIDE_BACK_SHAPE].center.x =
            shapes->center.x - (njCos(work->ang.y) * GLIDE_BACK_REACH);
        shapes[GLIDE_BACK_SHAPE].center.y = shapes->center.y - GLIDE_BACK_DROP;
        shapes[GLIDE_BACK_SHAPE].center.z =
            shapes->center.z - (njSin(work->ang.y) * GLIDE_BACK_REACH);
        break;
    case ANIM_ATTACK_1:
    case ANIM_ATTACK_3:
    case ANIM_RUN_ATTACK_1:
    case ANIM_RUN_ATTACK_3:
        /* @hack spelling: the shape reached through the work's collision
         * again, here and in the two cases below, where shapes would do */
        work->collision->shapes[HIT_SHAPE].flags &= ~SHAPE_OFF;
        if (record->kind != PLAYER_ROUGE) {
            shapes[HIT_SHAPE].center = record->unk_1e8;
        } else {
            reach = record->unk_2a8;
            reach.x *= record->consts.height * 0.5F;
            reach.y *= record->consts.height * 0.5F;
            reach.z *= record->consts.height * 0.5F;
            njAddVector(&reach, &record->unk_20c);
            shapes[HIT_SHAPE].center = reach;
        }
        break;
    case ANIM_ATTACK_2:
    case ANIM_RUN_ATTACK_2:
        work->collision->shapes[HIT_SHAPE].flags &= ~SHAPE_OFF;
        if (record->kind != PLAYER_ROUGE) {
            shapes[HIT_SHAPE].center = record->unk_1f4;
        } else {
            reach = record->unk_29c;
            reach.x *= record->consts.height * 0.5F;
            reach.y *= record->consts.height * 0.5F;
            reach.z *= record->consts.height * 0.5F;
            njAddVector(&reach, &record->unk_200);
            shapes[HIT_SHAPE].center = reach;
        }
        break;
    case ANIM_SPIRAL:
        work->collision->shapes[SPIRAL_SHAPE].flags &= ~SHAPE_OFF;
        shapes[SPIRAL_SHAPE].center = record->unk_1e8;
        if (stage_current_id() == STAGE_DEATH_CHAMBER) {
            shapes[SPIRAL_SHAPE].radius = SPIRAL_NARROW_REACH;
        } else {
            shapes[SPIRAL_SHAPE].radius = SPIRAL_REACH;
        }
        break;
    }
    collision = work->collision;
    end = &collision->shapes[collision->count];
    radius = 0.0F;
    for (shape = collision->shapes; shape < end; shape++) {
        size_x = shape->radius;
        size_y = shape->size_y;
        size_z = shape->size_z;
        if (shape->flags & SHAPE_IN_WORLD) {
            far = njDistanceP2P(&work->pos, &shape->center);
        } else {
            far = njScalor(&shape->center);
        }
        switch (shape->form) {
        case SHAPE_SPHERE:
            far += size_x;
            break;
        case SHAPE_CAPSULE:
            far += size_x + size_y;
            break;
        case SHAPE_CYLINDER:
        case SHAPE_CYLINDER2:
            far += njSqrt((size_x * size_x) + (size_y * size_y));
            break;
        default:
            far += njSqrt((size_x * size_x) + (size_y * size_y) +
                          (size_z * size_z));
            break;
        }
        if (far > radius) {
            radius = far;
        }
    }
    collision->radius = radius * SHAPES_MARGIN;
}

Bool hunter_offer_sunglasses(struct task_work *work, struct player *record)
{
    if (work->mode != PLAYER_MODE_STAND) {
        return FALSE;
    }
    if (game_state.sg_players & (1 << record->slot)) {
        return FALSE;
    }
    if ((record->character == PLAYER_KNUCKLES &&
         (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_SUNGLASS))) ||
        (record->character == PLAYER_ROUGE &&
         (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_SCOPE)))) {
        OFFER(record, ACTION_SUNGLASSES);
        return TRUE;
    }
    return FALSE;
}

/* the extra of a KnucklesSunglassesMan task */
struct sunglasses {
    Uint8 slot; /* the wearer's */
    Uint8 unk_01;
    Sint16 steps; /* the task's steps so far */
};

/* a KnucklesSunglassesMan task's level of task_create. the sunglasses start
 * down after SUNGLASSES_WAIT steps, and turn SUNGLASSES_TURN a step */
#define SUNGLASSES_LEVEL 2
#define SUNGLASSES_WAIT 30
#define SUNGLASSES_TURN 0x400

/* the step of a KnucklesSunglassesMan task. the glasses turn down, and set
 * the wearer's bit of game_state.sg_players once they are. the task ends once
 * the wearer's slot is empty, or at a press of jump */
static void hunter_sunglasses_step(struct task *task)
{
    struct sunglasses *sunglasses = task->extra.p;
    int slot = sunglasses->slot;
    struct player *hunter = player_tasks[slot]->extra.p;

    if (sunglasses->steps++ > SUNGLASSES_WAIT) {
        hunter->by_character.hunter.sunglasses =
            angle_approach(hunter->by_character.hunter.sunglasses, EIGHTH_TURN,
                           SUNGLASSES_TURN);
        if (hunter->by_character.hunter.sunglasses == EIGHTH_TURN) {
            game_state.sg_players |= 1 << slot;
        }
    }
    if (player_work[slot] == NULL || pad_jump_pressed[slot] != 0) {
        task_kill(task);
    }
}

/* the quad the sunglasses shade the screen with. the draw sets its color
 * and its corners' x */
static NJS_POLYGON_VTX shade_quad[4] = {
#include "player/knuckles/shade_quad.inc"
};

/* the shade's color, as ARGB: for Rouge's scope, and for Knuckles's
 * sunglasses */
#define ROUGE_SHADE 0x80006000
#define KNUCKLES_SHADE 0x80000060

/* the draw of a KnucklesSunglassesMan task. while the glasses are down, it
 * shades the screen, or in a two-player game the wearer's half. it shades
 * nothing while game_state.solo_player isn't 0 */
static void hunter_sunglasses_draw(struct task *task)
{
    int slot = ((struct sunglasses *)task->extra.p)->slot;
    Uint32 color;

    if (!(game_state.sg_players & (1 << slot))) {
        return;
    }
    if (game_state.solo_player != 0) {
        return;
    }
    if (player_records[slot]->character == PLAYER_ROUGE) {
        color = ROUGE_SHADE;
    } else {
        color = KNUCKLES_SHADE;
    }
    shade_quad[0].col = shade_quad[1].col = shade_quad[2].col =
        shade_quad[3].col = color;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (game_state.two_players != 0) {
        if (slot == 0) {
            shade_quad[0].x = 0.0F;
            shade_quad[1].x = 0.0F;
            shade_quad[2].x = SPLIT_LEFT_END;
            shade_quad[3].x = SPLIT_LEFT_END;
        } else {
            shade_quad[0].x = SPLIT_RIGHT_START;
            shade_quad[1].x = SPLIT_RIGHT_START;
            shade_quad[2].x = SCREEN_WIDTH;
            shade_quad[3].x = SCREEN_WIDTH;
        }
    } else {
        shade_quad[0].x = 0.0F;
        shade_quad[1].x = 0.0F;
        shade_quad[2].x = SCREEN_WIDTH;
        shade_quad[3].x = SCREEN_WIDTH;
    }
    njDrawPolygon(shade_quad, 4, TRUE);
}

#define SOUND_SUNGLASSES_OFF 0x200f

/* the destructor of a KnucklesSunglassesMan task: the glasses come off */
static void hunter_sunglasses_end(struct task *task)
{
    int slot = ((struct sunglasses *)task->extra.p)->slot;
    struct player *hunter = player_tasks[slot]->extra.p;

    game_state.sg_players &= ~(1 << slot);
    hunter->by_character.hunter.sunglasses = 0;
    sound_play(SOUND_SUNGLASSES_OFF, NULL, 0, 0);
}

/* put the sunglasses on, in PLAYER_MODE_UNK_35, with a
 * KnucklesSunglassesMan task */
static void hunter_start_sunglasses(struct task_work *work,
                                    struct player *record)
{
    struct task *task;

    record->action = 0;
    work->mode = PLAYER_MODE_UNK_35;
    if (record->character == PLAYER_KNUCKLES) {
        record->anim.next = ANIM_KNUCKLES_SUNGLASSES;
    } else if (record->character == PLAYER_ROUGE) {
        record->anim.next = ANIM_ROUGE_SUNGLASSES;
    }
    record->anim.after = ANIM_STAND;
    work->flags &=
        ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, SUNGLASSES_LEVEL,
                            hunter_sunglasses_step, "KnucklesSunglassesMan")) !=
        NULL) {
        ((struct sunglasses *)task->extra.p)->slot = record->slot;
        task->draw = hunter_sunglasses_draw;
        task->destructor = hunter_sunglasses_end;
    }
}

Bool hunter_offer_ring_attack(struct task_work *work, struct player *record)
{
    if (game_state.two_players == 0 &&
        stage_current_id() != STAGE_VERSUS_KNUCKLES) {
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
    if (game_state.ring_levels[record->slot][RING_LEVEL_POWER_FLASH] == 1) {
        OFFER(record, ACTION_POWER_FLASH);
        return TRUE;
    }
    if (game_state.ring_levels[record->slot][RING_LEVEL_EARTHFLAME] == 1) {
        OFFER(record, ACTION_EARTHFLAME);
        return TRUE;
    }
    if (game_state.ring_levels[record->slot][RING_LEVEL_EARTHQUAKE] == 1) {
        OFFER(record, ACTION_EARTHQUAKE);
        return TRUE;
    }
    return FALSE;
}

/* the extra of a Knuckles2PEarthquakeMan task */
struct earthquake {
    Uint8 slot; /* the shaken player's */
    Uint8 unk_01;
    Uint16 steps; /* the task's steps so far */
};

/* a Knuckles2PEarthquakeMan task's level of task_create. the earthquake
 * shakes after EARTHQUAKE_STEPS steps. its camera shakes at EARTHQUAKE_SHAKE
 * for EARTHQUAKE_SHAKE_STEPS + 1 steps, as camera_shake counts them, then
 * dies down. a player it knocks down takes no stick for
 * EARTHQUAKE_STICK_LOCK steps */
#define EARTHQUAKE_LEVEL 1
#define EARTHQUAKE_STEPS 60
#define EARTHQUAKE_SHAKE_STEPS 60
#define EARTHQUAKE_SHAKE 3.0F
#define EARTHQUAKE_STICK_LOCK 120
#define SOUND_EARTHQUAKE 0x2011

/* the step of a Knuckles2PEarthquakeMan task. it ends once the shaken
 * player's slot is empty, or after EARTHQUAKE_STEPS steps. the shake comes
 * in its destructor */
static void hunter_earthquake_step(struct task *task)
{
    int slot = ((struct earthquake *)task->extra.p)->slot;

    /* @hack spelling: the extra read again, where a local would do */
    if (player_work[slot] == NULL ||
        ((struct earthquake *)task->extra.p)->steps++ >= EARTHQUAKE_STEPS) {
        task_kill(task);
    }
}

/* the destructor of a Knuckles2PEarthquakeMan task: the shake. it shakes the
 * shaken player's camera and pad. it knocks that player down where it stands
 * on the ground. the attacker's pad shakes too. nothing shakes once the
 * round is cleared, or while game_state.solo_player isn't 0 */
static void hunter_earthquake_end(struct task *task)
{
    int slot = ((struct earthquake *)task->extra.p)->slot;

#ifdef NONMATCHING
    /* this side repairs the tag below: nothing shakes once the shaken
     * player's slot is empty */
    if (player_work[slot] == NULL) {
        return;
    }
#endif
    if (game_state.cleared != 1 && game_state.solo_player == 0) {
        camera_shake(slot, EARTHQUAKE_SHAKE_STEPS, EARTHQUAKE_SHAKE);
        /* @bug memory: reads through a null player_work[slot]. the step
         * kills the task once its test at 0x8c083b9e finds the slot empty.
         * the flags are then read at 0x8c083c18, and sound_play_for takes the
         * null at 0x8c083c52 */
        if (player_work[slot]->flags & PLAYER_WORK_GROUNDED) {
            /* @bug arithmetic: divides by zero for the knock's direction, (0,
             * 0, 0), which player_knock_back passes on to player_face_along
             * (0x8c083c2a). its angles divide by the direction's z, and by its
             * x and y */
            player_knock_back(slot, 0.0F, 0.0F, 0.0F);
            player_records[slot]->request_arg = ANIM_KNOCKED_DOWN;
            player_records[slot]->stick_lock = EARTHQUAKE_STICK_LOCK;
        }
        sound_play_for(SOUND_EARTHQUAKE, player_work[slot], 1, 0, 60);
        vibration_start_fading(slot, 7, 36, 48);
        vibration_start(~slot & 1, 5, 15, VIBRATION_UNTIMED);
    }
}

/* the voices of a Knuckles look and of a Rouge look as each two-player
 * attack starts */
#define VOICE_EARTHQUAKE_KNUCKLES 0x4de
#define VOICE_EARTHQUAKE_ROUGE 0x4df
#define VOICE_EARTHFLAME_KNUCKLES 0x4e0
#define VOICE_EARTHFLAME_ROUGE 0x4e1
#define VOICE_POWER_FLASH_KNUCKLES 0x4e2
#define VOICE_POWER_FLASH_ROUGE 0x4e3

/* start the earthquake on the other player, and spend the player's first
 * ring level. on the ground, the hunter lets go of what it holds and poses
 * in PLAYER_MODE_HUNTER_DIG_FAIL. a Knuckles or a Rouge look then says its
 * voice. where task_create fails, only the ring level is spent */
/* NOLINTBEGIN(clang-diagnostic-cast-function-type-mismatch) */
static void hunter_start_earthquake(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record,
                                    struct player *hunter)
{
    struct task *task;
    int other;
    int voice;
    int mode;
    int anim;
    int after;
    int i;

    game_state.ring_levels[record->slot][RING_LEVEL_EARTHQUAKE] = 0;
    if (record->slot == 0) {
        other = 1;
    } else {
        other = 0;
    }
    /* @hack redundant: an empty arm, and the assignment inside the test,
     * where a statement of its own and a test for non-null would do */
    if ((task =
             task_create(TASK_EXTRA, EARTHQUAKE_LEVEL, hunter_earthquake_step,
                         "Knuckles2PEarthquakeMan")) == NULL) {
    } else {
        /* @hack spelling: a copy of other for the table's index, where
         * other would do */
        int index = other;
        void *extra = player_tasks[index]->extra.p;

        ((struct earthquake *)task->extra.p)->slot = other;
        task->destructor = hunter_earthquake_end;
        if (work->flags & PLAYER_WORK_GROUNDED) {
            voice = -1;
            if (record->character == PLAYER_KNUCKLES) {
                anim = ANIM_KNUCKLES_EARTHQUAKE;
                after = 0;
                mode = PLAYER_MODE_HUNTER_DIG_FAIL;
                if (record->kind == PLAYER_KNUCKLES) {
                    voice = VOICE_EARTHQUAKE_KNUCKLES;
                }
            } else {
                anim = ANIM_ROUGE_EARTHQUAKE;
                after = 0;
                mode = PLAYER_MODE_HUNTER_DIG_FAIL;
                if (record->kind == PLAYER_ROUGE) {
                    voice = VOICE_EARTHQUAKE_ROUGE;
                }
            }
            if (voice != -1) {
                if (work->id == WORK_ID_RIVAL) {
                    for (i = 0; i < VOICES; i++) {
                        if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                            voice_stop(i);
                        }
                    }
                    voice_play(voice, RIVAL_VOICE_PRIORITY);
                } else {
                    /* @hack type: the call with a second argument, extra,
                     * which voice_play_priority_2 doesn't take */
#ifdef NONMATCHING
                    /* the hack's plain form: voice_play_priority_2 takes
                     * the voice alone */
                    voice_play_priority_2(voice);
#else
                    ((void (*)(int, void *))voice_play_priority_2)(voice,
                                                                   extra);
#endif
                }
            }
            player_release_held(work, motion, record);
            player_stop(motion, record);
            work->mode = mode;
            record->anim.next = anim;
            record->anim.after = after;
            /* @hack spelling: a plain assignment, where &= would do */
            work->flags =
                work->flags &
                ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        }
    }
}

/* NOLINTEND(clang-diagnostic-cast-function-type-mismatch) */

/* the extra of a Knuckles2PEarthflameMan task: the attacker's slot, then
 * the slot of the player it burns */
struct earthflame {
    /* @hack type: an array of the two slots, where two fields would do */
    Uint8 slots[2];
    Uint16 steps; /* the task's steps so far */
};

/* a Knuckles2PEarthflameMan task's level of task_create */
#define EARTHFLAME_LEVEL 1
/* an earthflame lasts EARTHFLAME_STEPS steps. a Rouge's black wave starts at
 * its step EARTHFLAME_WAVE, and a Knuckles's thunder at EARTHFLAME_THUNDER.
 * the burn comes at EARTHFLAME_BURN, and the cry at EARTHFLAME_CRY */
#define EARTHFLAME_STEPS 625
#define EARTHFLAME_CRY 80
#define EARTHFLAME_BURN 25
#define EARTHFLAME_THUNDER 20
#define EARTHFLAME_WAVE 3
/* the steps a burnt player stays confused, in a two-player game */
#define EARTHFLAME_CONFUSION 600
/* a Knuckles's earthflame strikes the other player with THUNDER_BOLTS bolts. a
 * Rouge's sends BLACK_WAVES waves of dark at it */
#define THUNDER_BOLTS 3
#define BLACK_WAVES 11
#define SOUND_THUNDER 0x2012
#define SOUND_BLACK_WAVE 0x2013

/* the step of a Knuckles2PEarthflameMan task. it ends once the burnt
 * player's slot is empty, once the round is cleared, or after
 * EARTHFLAME_STEPS steps. the burn spares a player in a dig, on its way in or
 * out. those are the modes from PLAYER_MODE_HUNTER_DIG to PLAYER_MODE_UNK_3F
 * and from PLAYER_MODE_HUNTER_WALL_DIG to PLAYER_MODE_UNK_43. in a two-player
 * game the burn hurts and confuses any other player. in a one-player game it
 * only hurts. it then spares a player at running_speed or faster, and one in
 * PLAYER_MODE_HUNTER_GLIDE. it spares one in ANIM_UNK_23 with the stick too. at
 * the cry, a confused Knuckles or Rouge look says its voice */
static void hunter_earthflame_step(struct task *task)
{
    /* @hack spelling: task's extra cast afresh at each use, where a local
     * for the extra would do */
    int other = ((struct earthflame *)task->extra.p)->slots[1];
    int slot = ((struct earthflame *)task->extra.p)->slots[0];
    struct task_work *work = player_work[other];
    struct player *record = player_records[other];
    /* @hack type: the mode as a Sint8, where an int would do */
    Sint8 mode;
    int kind;
    int voice;
    int i;

    if (work == NULL ||
        ((struct earthflame *)task->extra.p)->steps++ >= EARTHFLAME_STEPS ||
        game_state.cleared == 1) {
        task_kill(task);
        return;
    }
    if (((struct earthflame *)task->extra.p)->steps == EARTHFLAME_CRY) {
        kind = record->kind;
        if (record->confused_steps > 0) {
            voice = -1;
            if (kind == PLAYER_KNUCKLES) {
                voice = VOICE_CAPTURE_KNUCKLES;
            } else if (kind == PLAYER_ROUGE) {
                voice = VOICE_CAPTURE_ROUGE;
            }
            if (voice != -1) {
                if (work->id == WORK_ID_RIVAL) {
                    for (i = 0; i < VOICES; i++) {
                        if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                            voice_stop(i);
                        }
                    }
                    voice_play(voice, RIVAL_VOICE_PRIORITY);
                } else {
                    voice_play_priority_2(voice);
                }
            }
        }
    } else if (((struct earthflame *)task->extra.p)->steps == EARTHFLAME_BURN) {
        mode = work->mode;
        if (game_state.two_players != 0) {
            if ((mode < PLAYER_MODE_HUNTER_DIG || mode > PLAYER_MODE_UNK_3F) &&
                (mode < PLAYER_MODE_HUNTER_WALL_DIG ||
                 mode > PLAYER_MODE_UNK_43)) {
                work->flags |= WORK_HURT;
                record->confused_steps = EARTHFLAME_CONFUSION;
                confu_stars_start(other);
            }
        } else if (njScalor(&record->spd) < record->consts.running_speed) {
            if ((mode < PLAYER_MODE_HUNTER_DIG || mode > PLAYER_MODE_UNK_3F) &&
                (mode < PLAYER_MODE_HUNTER_WALL_DIG ||
                 mode > PLAYER_MODE_UNK_43)) {
                if (record->anim.current != ANIM_UNK_23 ||
                    player_read_stick(work, record, NULL, NULL) == FALSE) {
                    if (mode != PLAYER_MODE_HUNTER_GLIDE) {
                        work->flags |= WORK_HURT;
                    }
                }
            }
        }
    } else if (((struct earthflame *)task->extra.p)->steps ==
                   EARTHFLAME_THUNDER &&
               player_records[slot]->character == PLAYER_KNUCKLES) {
        thunder_strike(other, THUNDER_BOLTS);
        sound_play_for(SOUND_THUNDER, player_work[slot], 1, 0, 60);
    } else if (((struct earthflame *)task->extra.p)->steps == EARTHFLAME_WAVE &&
               player_records[slot]->character == PLAYER_ROUGE) {
        black_wave_start(slot, other, BLACK_WAVES);
        sound_play_for(SOUND_BLACK_WAVE, player_work[slot], 1, 0, 60);
    }
}

/* the draw of a Knuckles2PEarthflameMan task, which draws nothing */
static void hunter_earthflame_draw(struct task *task)
{
}

/* the destructor of a Knuckles2PEarthflameMan task: the burnt player's
 * confusion ends */
static void hunter_earthflame_end(struct task *task)
{
    struct earthflame *earthflame;
    Uint8 *slots;
    int other;

    /* @hack redundant: the store under a test that always holds */
    1 && (earthflame = task->extra.p);
    slots = earthflame->slots;
    other = slots[1];

    if (player_records[other]) {
        player_records[other]->confused_steps = 0;
    }
}

/* start the earthflame on the other player, and spend the player's second
 * ring level. the hunter poses, and a Knuckles or a Rouge look says its
 * voice. where task_create fails, only the ring level is spent */
static void hunter_start_earthflame(struct task_work *work,
                                    struct player_motion *motion,
                                    struct player *record,
                                    struct player *hunter)
{
    struct task *task;
    int other;
    int voice;
    int i;

    if (record->slot == 0) {
        other = 1;
    } else {
        other = 0;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task =
             task_create(TASK_EXTRA, EARTHFLAME_LEVEL, hunter_earthflame_step,
                         "Knuckles2PEarthflameMan")) != NULL) {
        ((struct earthflame *)task->extra.p)->slots[0] = record->slot;
        ((struct earthflame *)task->extra.p)->slots[1] = other;
        task->draw = hunter_earthflame_draw;
        task->destructor = hunter_earthflame_end;
        voice = -1;
        if (record->character == PLAYER_KNUCKLES) {
            if (record->kind == PLAYER_KNUCKLES) {
                voice = VOICE_EARTHFLAME_KNUCKLES;
            }
            player_pose(record->slot, ANIM_KNUCKLES_SPECIAL);
        } else if (record->character == PLAYER_ROUGE) {
            if (record->kind == PLAYER_ROUGE) {
                voice = VOICE_EARTHFLAME_ROUGE;
            }
            player_pose(record->slot, ANIM_ROUGE_SPECIAL);
        }
        if (voice != -1) {
            if (work->id == WORK_ID_RIVAL) {
                for (i = 0; i < VOICES; i++) {
                    if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                        voice_stop(i);
                    }
                }
                voice_play(voice, RIVAL_VOICE_PRIORITY);
            } else {
                voice_play_priority_2(voice);
            }
        }
    }
    game_state.ring_levels[record->slot][RING_LEVEL_EARTHFLAME] = 0;
}

/* the extra of a Knuckles2PPowerFlashMan task: the slot of the flash's
 * target, then its maker's character */
struct power_flash {
    /* @hack type: an array of the two, where two fields would do */
    Uint8 target_and_character[2];
    Uint16 steps; /* the task's steps so far */
};

/* the seconds of the flash left, in one digit. its draw shows them over the
 * other player's half of the screen */
#define FLASH_SECONDS_MAX 9
/* clang-format off */
static struct hud_number flash_seconds = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS, 0, 0,
    FLASH_SECONDS_MAX, 0, {64.0F, 224.0F, 0.0F}, 2.0F, 0xff8080ff,
};
/* clang-format on */

/* the flash's two quads, Knuckles's and then Rouge's, which cover the
 * screen. the draw sets their corners' x */
static NJS_POLYGON_VTX flash_quads[8] = {
#include "player/knuckles/flash_quads.inc"
};

/* the draw counts the seconds left down from POWER_FLASH_SECONDS, at
 * STEPS_PER_SECOND steps to a second. their number lies at FLASH_SECONDS_X_0
 * on player 0's half of the screen, and at FLASH_SECONDS_X_1 on player 1's */
#define POWER_FLASH_SECONDS 10.0F
#define FLASH_SECONDS_X_0 128.0F
#define FLASH_SECONDS_X_1 448.0F

/* the draw of a Knuckles2PPowerFlashMan task, which the step sets at
 * POWER_FLASH_START. it draws the seconds left, and the flash over the
 * target's half of the screen. a one-player game's flash covers the whole
 * screen */
static void hunter_power_flash_draw(struct task *task)
{
    /* @hack spelling: task's extra cast afresh at each use, where a local
     * for the extra would do */
    int other = ((struct power_flash *)task->extra.p)->target_and_character[0];
    int character =
        ((struct power_flash *)task->extra.p)->target_and_character[1];
    NJS_POLYGON_VTX *quad;

    flash_seconds.value =
        POWER_FLASH_SECONDS -
        (Uint32)(((struct power_flash *)task->extra.p)->steps /
                 STEPS_PER_SECOND);
    flash_seconds.pos.x = other == 0 ? FLASH_SECONDS_X_0 : FLASH_SECONDS_X_1;
    hud_number_draw(&flash_seconds);
    if (character == PLAYER_ROUGE) {
        quad = &flash_quads[4];
    } else {
        quad = flash_quads;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    if (game_state.two_players) {
        if (other == 0) {
            quad[0].x = 0.0F;
            quad[1].x = 0.0F;
            quad[2].x = SPLIT_LEFT_END;
            quad[3].x = SPLIT_LEFT_END;
        } else {
            quad[0].x = SPLIT_RIGHT_START;
            quad[1].x = SPLIT_RIGHT_START;
            quad[2].x = SCREEN_WIDTH;
            quad[3].x = SCREEN_WIDTH;
        }
    } else {
        quad[0].x = 0.0F;
        quad[1].x = 0.0F;
        quad[2].x = SCREEN_WIDTH;
        quad[3].x = SCREEN_WIDTH;
    }
    njDrawPolygon(quad, 4, TRUE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* the flash shows from its step POWER_FLASH_START, and ends at
 * POWER_FLASH_STEPS. each press of a PLAYER_STRUGGLE_BUTTONS button by its
 * target takes POWER_FLASH_MASH steps off it, as in a struggle */
#define POWER_FLASH_START 20
#define POWER_FLASH_MASH 5
#define POWER_FLASH_STEPS 620
#define SOUND_POWER_FLASH 0x2014

void hunter_power_flash_step(struct task *task)
{
    /* @hack spelling: task's extra cast afresh at each use, where a local
     * for the extra would do */
    int other = ((struct power_flash *)task->extra.p)->target_and_character[0];

    if (((struct power_flash *)task->extra.p)->steps == POWER_FLASH_START) {
        pad_disable_player(other);
        task->draw = hunter_power_flash_draw;
        sound_play_for(SOUND_POWER_FLASH, task, 1, 0,
                       POWER_FLASH_STEPS - POWER_FLASH_START);
    } else if (((struct power_flash *)task->extra.p)->steps >
               POWER_FLASH_START) {
        if (pad_press_repeat[other] & PLAYER_STRUGGLE_BUTTONS) {
            ((struct power_flash *)task->extra.p)->steps += POWER_FLASH_MASH;
        }
    }
    if (player_work[other] == NULL ||
        ((struct power_flash *)task->extra.p)->steps++ >= POWER_FLASH_STEPS ||
        game_state.cleared == 1) {
        task_kill(task);
    }
}

void hunter_power_flash_end(struct task *task)
{
    int other = ((struct power_flash *)task->extra.p)->target_and_character[0];

    if (game_state.cleared != 1) {
        pad_enable_player(other);
    }
    sound_stop_key(task);
}

/* the item kinds of the icons player_hooks.add_item_icon shows for a find.
 * the stages' item boxes number them: ITEM_ of shared/item_box.h, a header
 * the main executable can't include */
#define ICON_10_RINGS 0
#define ICON_20_RINGS 1
#define ICON_5_RINGS 2
#define ICON_BARRIER 3
#define ICON_BOMB 4
#define ICON_MAGNETIC_BARRIER 6
#define ICON_SPEED_UP 8
#define ICON_1UP 10
/* a dig within DIG_POS_REACH of the hunter's dig_end turns up nothing */
#define DIG_POS_REACH 10.0F
/* the speed a skull flies up at from a dig */
#define SKULL_SPEED 3.0F
/* the voices after a dig, by look: of nothing or a skull, of 5 or 10 rings,
 * and of 20 rings, a bomb or a life */
#define VOICE_DUG_NOTHING_KNUCKLES 0x4fa
#define VOICE_DUG_NOTHING_ROUGE 0x4fb
#define VOICE_DUG_RINGS_KNUCKLES 0x4fc
#define VOICE_DUG_RINGS_ROUGE 0x4fd
#define VOICE_DUG_MORE_KNUCKLES 0x4fe
#define VOICE_DUG_MORE_ROUGE 0x4ff

/* these two stages list no EMERALD_KIND_A_PATHMOVE or EMERALD_KIND_A_1P_TECH
 * place in a one-player game until player 0 can dig. the macro reads stage
 * once for each test */
#define STAGE_DIG_GATED(stage)                                                 \
    ((stage) == STAGE_PUMPKIN_HILL || (stage) == STAGE_EGG_QUARTERS)

/* the task_create level of the power flash's task */
#define POWER_FLASH_LEVEL 2

void hunter_power_flash(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *hunter)
{
    struct task *task;
    int target;
    int voice;
    int i;

    if (record->slot == 0) {
        target = 1;
    } else {
        target = 0;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task =
             task_create(TASK_EXTRA, POWER_FLASH_LEVEL, hunter_power_flash_step,
                         "Knuckles2PPowerFlashMan")) != NULL) {
        /* @hack spelling: each write through task's extra cast afresh, where
         * a local for the extra would do */
        ((struct power_flash *)task->extra.p)->target_and_character[0] = target;
        ((struct power_flash *)task->extra.p)->target_and_character[1] =
            record->character;
        task->destructor = hunter_power_flash_end;
        voice = -1;
        if (record->character == PLAYER_KNUCKLES) {
            if (record->kind == PLAYER_KNUCKLES) {
                voice = VOICE_POWER_FLASH_KNUCKLES;
            }
            player_pose(record->slot, ANIM_KNUCKLES_SPECIAL);
        } else if (record->character == PLAYER_ROUGE) {
            if (record->kind == PLAYER_ROUGE) {
                voice = VOICE_POWER_FLASH_ROUGE;
            }
            player_pose(record->slot, ANIM_ROUGE_SPECIAL);
        }
        if (voice != -1) {
            if (work->id == WORK_ID_RIVAL) {
                for (i = 0; i < VOICES; i++) {
                    if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                        voice_stop(i);
                    }
                }
                voice_play(voice, RIVAL_VOICE_PRIORITY);
            } else {
                voice_play_priority_2(voice);
            }
        }
    }
    game_state.ring_levels[record->slot][RING_LEVEL_POWER_FLASH] = 0;
}

/* the voices of a dig and of a refused dig, by look */
#define VOICE_DIG_KNUCKLES 0x4f6
#define VOICE_DIG_ROUGE 0x4f7
#define VOICE_DIG_FAIL_KNUCKLES 0x4f8
#define VOICE_DIG_FAIL_ROUGE 0x4f9

void hunter_dig(struct task_work *work, struct player_motion *motion,
                struct player *record, struct player *hunter)
{
    player_stop(motion, record);
    hunter->by_character.hunter.dig_depth = 0.0F;
    hunter->by_character.hunter.dig_steps = 0;
    if (work->mode != PLAYER_MODE_HUNTER_DIVE) {
        record->anim.next = ANIM_DIG;
    }
    work->mode = PLAYER_MODE_HUNTER_DIG;
    if (record->kind == PLAYER_KNUCKLES) {
        voice_play_priority_2(VOICE_DIG_KNUCKLES);
    } else if (record->kind == PLAYER_ROUGE) {
        voice_play_priority_2(VOICE_DIG_ROUGE);
    }
}

void hunter_dig_wall(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *hunter)
{
    player_stop(motion, record);
    hunter->by_character.hunter.dig_depth = 0.0F;
    hunter->by_character.hunter.dig_steps = 0;
    record->anim.next = ANIM_DIG_WALL;
    work->mode = PLAYER_MODE_HUNTER_WALL_DIG;
    if (record->kind == PLAYER_KNUCKLES) {
        voice_play_priority_2(VOICE_DIG_KNUCKLES);
    } else if (record->kind == PLAYER_ROUGE) {
        voice_play_priority_2(VOICE_DIG_ROUGE);
    }
}

void hunter_dig_fail(struct task_work *work, struct player *record)
{
    record->spd.x = 0.0F;
    record->spd.y = 0.0F;
    record->spd.z = 0.0F;
    work->mode = PLAYER_MODE_HUNTER_DIG_FAIL;
    record->anim.next = ANIM_DIG_FAIL;
    record->anim.after = ANIM_STAND;
    if (record->kind == PLAYER_KNUCKLES) {
        voice_play_priority_2(VOICE_DIG_FAIL_KNUCKLES);
    } else if (record->kind == PLAYER_ROUGE) {
        voice_play_priority_2(VOICE_DIG_FAIL_ROUGE);
    }
}

void hunter_dig_wall_fail(struct task_work *work, struct player *record)
{
    record->spd.x = 0.0F;
    record->spd.y = 0.0F;
    record->spd.z = 0.0F;
    work->mode = PLAYER_MODE_HUNTER_WALL_DIG_FAIL;
    record->anim.next = ANIM_DIG_WALL_FAIL;
    record->anim.after = ANIM_CLIMB_STILL;
    if (record->kind == PLAYER_KNUCKLES) {
        voice_play_priority_2(VOICE_DIG_FAIL_KNUCKLES);
    } else if (record->kind == PLAYER_ROUGE) {
        voice_play_priority_2(VOICE_DIG_FAIL_ROUGE);
    }
}

/* what hunter_dig_ground returns */
#define DIG_NONE (-1) /* off the ground, or in the grab of a wall */
#define DIG_REFUSED 0
#define DIG_TAKEN 1

/* on STAGE_SECURITY_HALL, dusty ground with a ground_task takes a dig only
 * near a spot. the work's pos must lie within DIG_SPOT_REACH of a point
 * DIG_SPOT_HEIGHT above that task's pos */
#define DIG_SPOT_HEIGHT 20.0F
#define DIG_SPOT_REACH 3.0F

/* what the hunter stands on makes of a dig, a DIG_ value, as
 * hunter_offer_dig's contract says */
static int hunter_dig_ground(struct task_work *work, struct player *record)
{
    NJS_POINT3 pos;

    if (!(work->flags & PLAYER_WORK_GROUNDED) ||
        record->anim.next == ANIM_GRAB_WALL) {
        return DIG_NONE;
    }
    if ((work->flags & PLAYER_WORK_ON_SHAPE) &&
        (work->collision->hit->shapes->flags & SHAPE_DIGGABLE)) {
        return DIG_TAKEN;
    }
    if ((work->flags & PLAYER_WORK_GROUNDED) != PLAYER_WORK_ON_GROUND) {
        return DIG_REFUSED;
    }
    if (!(record->ground_flags & WORLD_DUSTY)) {
        return DIG_REFUSED;
    }
    if (record->ground_flags & WORLD_MOVING) {
        return DIG_REFUSED;
    }
    if (stage_current_id() != STAGE_SECURITY_HALL) {
        return DIG_TAKEN;
    }
    if (record->ground_task == NULL) {
        return DIG_TAKEN;
    }
    /* @bug memory: reads through null for a ground_task without a work
     * (MOV.L @(24,R2),R0 at 0x8c084758). rouge_offer_actions tests the same
     * work for null (TST R2,R2 at 0x8c0848a6) */
    pos = record->ground_task->work->pos;
    pos.y += DIG_SPOT_HEIGHT;
    if (njDistanceP2P(&work->pos, &pos) < DIG_SPOT_REACH) {
        return DIG_TAKEN;
    }
    return DIG_REFUSED;
}

/* a hunter that doesn't dive is offered a dig at a spd.x of DIG_SPEED_MAX or
 * less */
#define DIG_SPEED_MAX 2.0F

Bool hunter_offer_dig(struct task_work *work, struct player_motion *motion,
                      struct player *record, struct player *hunter)
{
    int ground;

    if (!(record->upgrades & (UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS) |
                              UPGRADE_BIT(UPGRADE_ROUGE_NAILS)))) {
        return FALSE;
    }
    if (work->mode != PLAYER_MODE_STAND && work->mode != PLAYER_MODE_RUN &&
        work->mode != PLAYER_MODE_HUNTER_CLIMB &&
        work->mode != PLAYER_MODE_HUNTER_DIVE) {
        return FALSE;
    }
    if (record->spd.x > DIG_SPEED_MAX &&
        work->mode != PLAYER_MODE_HUNTER_DIVE) {
        return FALSE;
    }
    ground = hunter_dig_ground(work, record);
    if (ground == DIG_NONE) {
        return FALSE;
    }
    if (ground == DIG_REFUSED) {
        if (work->mode == PLAYER_MODE_HUNTER_CLIMB) {
            OFFER(record, PLAYER_MODE_HUNTER_WALL_DIG_FAIL);
        } else {
            OFFER(record, PLAYER_MODE_HUNTER_DIG_FAIL);
        }
    } else {
        if (work->mode == PLAYER_MODE_HUNTER_CLIMB) {
            OFFER(record, PLAYER_MODE_HUNTER_WALL_DIG);
        } else {
            OFFER(record, PLAYER_MODE_HUNTER_DIG);
        }
    }
    return TRUE;
}

/* the id of a swimming turtle's shape, which Rouge touches to be offered
 * PLAYER_MODE_ROUGE_RIDE_SWIMMER, as modules/stg18/turtle.c's SWIM_SHAPE_ID */
#define TURTLE_SHAPE_ID 5

Bool rouge_offer_actions(struct task_work *work, struct player *record)
{
    if (record->character != PLAYER_ROUGE) {
        return FALSE;
    }
    if ((work->flags & PLAYER_WORK_ON_GROUND) && record->ground_task &&
        record->ground_task->work &&
        record->ground_task->work->id == WORK_ID_TURTLE &&
        (work->mode == PLAYER_MODE_STAND || work->mode == PLAYER_MODE_RUN)) {
        OFFER(record, PLAYER_MODE_ROUGE_RIDE_FERRY);
        return TRUE;
    }
    if (work->mode >= PLAYER_MODE_HUNTER_FLOAT &&
        work->mode <= PLAYER_MODE_HUNTER_WATER_DIVE &&
        collision_first_with_id(player_tasks[record->slot], TURTLE_SHAPE_ID)) {
        OFFER(record, PLAYER_MODE_ROUGE_RIDE_SWIMMER);
        return TRUE;
    }
    /* @hack implicit: no return value for a Rouge offered neither action,
     * where return FALSE would do while no caller reads that result */
#ifdef NONMATCHING
    /* the tag's plain form, which no caller tells from the image's result */
    return FALSE;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void rouge_start_ride_swimmer(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    player_stop(motion, record);
    work->mode = PLAYER_MODE_ROUGE_RIDE_SWIMMER;
    record->anim.next = ANIM_ROUGE_RIDE_SWIMMER;
}

/* hunter_float's weights, which the image keeps as data. a hunter whose top
 * lies under the surface rises by float_rise, less float_brake times its
 * spd.y. in PLAYER_MODE_HUNTER_SWIM it takes float_slow_rise and
 * float_slow_brake in their place. one whose foot lies above the surface
 * sinks, by its gravity less float_lift. between the two it rises by
 * float_surface_rise for each unit of depth, less float_surface_brake times
 * spd.y. it then sinks by float_fall_share of that sinking for each unit its
 * top lies above the surface */
static Float float_slow_rise = 0.06F;
static Float float_slow_brake = 0.04F;
static Float float_rise = 0.16F;
static Float float_brake = 0.09F;
static Float float_surface_rise = 0.023F;
static Float float_surface_brake = 0.3F;
static Float float_lift = 0.08F;
static Float float_fall_share = 0.1F;
/* a floating hunter reaches from FLOAT_FOOT above its pos up to FLOAT_HEAD
 * above its height. FLOAT_JITTER is the most the random part of its acc
 * adds */
#define FLOAT_FOOT 1.0F
#define FLOAT_HEAD 10.0F
#define FLOAT_JITTER 0.0001F

void hunter_float(struct task_work *work, struct player_motion *motion,
                  struct player *record)
{
    /* @hack spelling: y, a copy of work->pos.y, where its two reads would
     * do */
    Float y;
    Float top;
    Float bottom;
    Float surface;
    Float acc;

    y = work->pos.y;
    bottom = y + FLOAT_FOOT;
    top = y + record->consts.height + FLOAT_HEAD;
    surface = record->ground.above_y;
    if (surface > top) {
        if (work->mode == PLAYER_MODE_HUNTER_SWIM) {
            acc = float_slow_rise - (record->spd.y * float_slow_brake);
        } else {
            acc = float_rise - (record->spd.y * float_brake);
        }
    } else if (bottom > surface) {
        acc = -float_lift + record->consts.gravity;
    } else {
        acc = (((surface - bottom) * float_surface_rise) -
               (record->spd.y * float_surface_brake)) +
              ((-float_lift + record->consts.gravity) * (top - surface) *
               float_fall_share);
    }
    acc += rand() * (FLOAT_JITTER * RAND_SCALE);
    player_set_acc(record->slot, 0.0F, acc, 0.0F);
}

/* a hunter on the ground swims where the surface lies more than SWIM_DEPTH
 * above its first shape's center */
#define SWIM_DEPTH 4.0F

Bool hunter_can_swim_off_ground(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record)
{
    if ((!pad_action_held[record->slot] || pad_jump_pressed[record->slot]) &&
        (record->ground.above_flags & WORLD_FLAT_THIN) &&
        record->ground.above_y > work->collision->shapes->center.y &&
        !(work->flags & PLAYER_WORK_GROUNDED) &&
        _builtin_fabsf(record->ground.above_y - record->ground.below_y) >
            WATER_DEEP) {
        return TRUE;
    }
    return FALSE;
}

static Bool hunter_can_swim(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    if (!pad_action_held[record->slot] &&
        !(pad_jump_held[record->slot] && work->mode == PLAYER_MODE_JUMP &&
          record->spd.y > 0.0F) &&
        (record->ground.above_flags & WORLD_FLAT_THIN) &&
        (record->ground.above_y >
             work->collision->shapes->center.y + SWIM_DEPTH ||
         !(work->flags & PLAYER_WORK_GROUNDED)) &&
        _builtin_fabsf(record->ground.above_y - record->ground.below_y) >
            WATER_DEEP) {
        return TRUE;
    }
    return FALSE;
}

Bool hunter_in_deep_water(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    if ((record->ground.above_flags & WORLD_FLAT_THIN) &&
        _builtin_fabsf(record->ground.above_y - record->ground.below_y) >
            WATER_DEEP) {
        return TRUE;
    }
    return FALSE;
}

Bool hunter_test_swim(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    if (stage_current_id() != STAGE_CHAO &&
        hunter_can_swim(work, motion, record)) {
        work->mode = PLAYER_MODE_HUNTER_FLOAT;
        record->anim.next = ANIM_FLOAT;
        return TRUE;
    }
    return FALSE;
}

void hunter_dig_find(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *hunter)
{
    struct collision_hit *hit;
    /* @hack spelling: end, a local for the end of the hits, where the loop's
     * test would compute it */
    struct collision_hit *end;
    /* @hack type: stage, a Sint16 local for stage_current_id()'s int, where
     * a switch on the call would do */
    Sint16 stage;
    Float roll;
    Float sum;
    /* @hack spelling: weight, a cursor over the weights beside find, where
     * weights[find] would do */
    Float *weight;
    Float *weights;
    /* @hack type: find as unsigned, where an int would do */
    unsigned int find;
    int voice;
    int look;

    if (work->collision->flags & COLLISION_TOUCHED) {
        for (hit = work->collision->hits,
            end = work->collision->hits + COLLISION_HITS;
             hit < end; hit++) {
            if (hit->hit_shape == HIT_UNUSED) {
                continue;
            }
            if (!(hit->task->work->collision->shapes[hit->hit_shape].flags &
                  SHAPE_DIGGABLE)) {
                return;
            }
        }
    }
    stage = stage_current_id();
    switch (stage) {
    case STAGE_SECURITY_HALL:
    case STAGE_BOSS_BIGBOGY:
    case STAGE_CHAO:
        return;
    }
    if (njDistanceP2P(&work->pos, &hunter->by_character.hunter.dig_end) <
        DIG_POS_REACH) {
        find = HUNTER_FIND_NOTHING;
    } else {
        roll = rand() * RAND_SCALE;
        if (game_state.mission == MISSION_RINGS) {
            weights = dig_find_chances_rings;
        } else if (game_state.two_players) {
            weights = dig_find_chances_two_players;
        } else {
            weights = dig_find_chances;
        }
        for (find = DIG_FINDS - 1, weight = weights + (DIG_FINDS - 1),
            sum = *weight;
             find > 0 && roll > sum;) {
            weight--;
            find--;
            sum += *weight;
        }
    }
    voice = -1;
    look = record->kind;
    switch (find) {
    case HUNTER_FIND_NOTHING:
        if (look == PLAYER_KNUCKLES) {
            voice = VOICE_DUG_NOTHING_KNUCKLES;
        } else if (look == PLAYER_ROUGE) {
            voice = VOICE_DUG_NOTHING_ROUGE;
        }
        break;
    case HUNTER_FIND_SKULL:
        if (player_hooks.throw_skull) {
            player_hooks.throw_skull(&work->pos, SKULL_SPEED);
        }
        if (look == PLAYER_KNUCKLES) {
            voice = VOICE_DUG_NOTHING_KNUCKLES;
        } else if (look == PLAYER_ROUGE) {
            voice = VOICE_DUG_NOTHING_ROUGE;
        }
        break;
    case HUNTER_FIND_5_RINGS:
        player_hooks.add_item_icon(record->slot, ICON_5_RINGS);
        if (!game_state.two_players) {
            if (look == PLAYER_KNUCKLES) {
                voice = VOICE_DUG_RINGS_KNUCKLES;
            } else if (look == PLAYER_ROUGE) {
                voice = VOICE_DUG_RINGS_ROUGE;
            }
        }
        ring_add(record->slot, 5);
        game_state.ring_total += 5;
        snapshot_current->ring_total += 5;
        break;
    case HUNTER_FIND_10_RINGS:
        player_hooks.add_item_icon(record->slot, ICON_10_RINGS);
        if (!game_state.two_players) {
            if (look == PLAYER_KNUCKLES) {
                voice = VOICE_DUG_RINGS_KNUCKLES;
            } else if (look == PLAYER_ROUGE) {
                voice = VOICE_DUG_RINGS_ROUGE;
            }
        } else {
            /* @hack redundant: an empty else */
        }
        ring_add(record->slot, 10);
        game_state.ring_total += 10;
        snapshot_current->ring_total += 10;
        break;
    case HUNTER_FIND_20_RINGS:
        player_hooks.add_item_icon(record->slot, ICON_20_RINGS);
        if (!game_state.two_players) {
            if (look == PLAYER_KNUCKLES) {
                voice = VOICE_DUG_MORE_KNUCKLES;
            } else if (look == PLAYER_ROUGE) {
                voice = VOICE_DUG_MORE_ROUGE;
            }
        } else {
            /* @hack redundant: an empty else */
        }
        ring_add(record->slot, 20);
        game_state.ring_total += 20;
        snapshot_current->ring_total += 20;
        break;
    case HUNTER_FIND_BARRIER:
        player_hooks.add_item_icon(record->slot, ICON_BARRIER);
        player_give_barrier(record->slot);
        break;
    case HUNTER_FIND_BOMB:
        player_hooks.add_item_icon(record->slot, ICON_BOMB);
        if (player_hooks.explode) {
            player_hooks.explode(&work->pos);
        }
        if (look == PLAYER_KNUCKLES) {
            voice = VOICE_DUG_MORE_KNUCKLES;
        } else if (look == PLAYER_ROUGE) {
            voice = VOICE_DUG_MORE_ROUGE;
        }
        break;
    case HUNTER_FIND_SPEED_UP:
        player_hooks.add_item_icon(record->slot, ICON_SPEED_UP);
        player_speed_up(record->slot);
        break;
    case HUNTER_FIND_MAGNETIC_BARRIER:
        player_hooks.add_item_icon(record->slot, ICON_MAGNETIC_BARRIER);
        player_give_magnetic_barrier(record->slot);
        break;
    case HUNTER_FIND_LIFE:
        player_hooks.add_item_icon(record->slot, ICON_1UP);
        lives_add(record->slot, 1);
        if (look == PLAYER_KNUCKLES) {
            voice = VOICE_DUG_MORE_KNUCKLES;
        } else if (look == PLAYER_ROUGE) {
            voice = VOICE_DUG_MORE_ROUGE;
        }
        break;
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
}

Bool hunter_outside_kill_areas(struct task_work *work, struct player *record)
{
    struct height_probe probe;
    struct kill_area *area;
    int bit;

    area = kill_areas;
    if (area == NULL) {
        return FALSE;
    }
    bit = 1 << record->character;
    probe.pos = work->pos;
    while (area->object) {
        if ((area->flags & bit) &&
            polygon_find_model_heights(&probe, area->object)) {
            return FALSE;
        }
        area++;
    }
    return TRUE;
}

/* a grab takes a wall_normal whose y lies within WALL_LEVEL of 0 for level */
#define WALL_LEVEL 0.001F

void hunter_grab_wall(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    work->flags |= PLAYER_WORK_ON_GROUND;
    player_stop(motion, record);
    if (_builtin_fabsf(record->wall_normal.y) < WALL_LEVEL) {
        record->wall_normal.y = 0.0F;
        njUnitVector(&record->wall_normal);
    }
    work->ang.x = (Angle)(asinf(record->wall_normal.z) * ANGLE_PER_RADIAN);
    work->ang.z =
        -(Angle)(atan2f(record->wall_normal.x, record->wall_normal.y) *
                 ANGLE_PER_RADIAN);
    work->pos.x = record->ball->hit.point.x;
    work->pos.y = record->ball->hit.point.y;
    work->pos.z = record->ball->hit.point.z;
    work->flags &= ~PLAYER_WORK_ATTACK;
    work->mode = PLAYER_MODE_HUNTER_CLIMB;
    record->anim.next = ANIM_GRAB_WALL;
    if (stage_current_id() == STAGE_BOSS_FDOG) {
        sound_play(SOUND_GRAB_FDOG, NULL, 0, 0);
    }
}

/* a bump flies the hunter back at BUMP_BACK and up at BUMP_UP, in its own
 * axes. its flash takes BUMP_FLASH_SCALE, BUMP_FLASH_SPARKS and
 * BUMP_FLASH_BIAS as player_hooks.flash's scale, sparks and depth_bias */
#define BUMP_BACK 3.0F
#define BUMP_UP 2.8F
#define BUMP_FLASH_SCALE 11.0F
#define BUMP_FLASH_SPARKS 4
#define BUMP_FLASH_BIAS 8.0F

Bool hunter_test_bump(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    struct task *task;
    struct task_work *other;
    int other_slot;
    NJS_VECTOR dir;

    task = player_tasks[record->slot];
    if (stage_current_id() == STAGE_VERSUS_KNUCKLES) {
        /* @hack spelling: other_slot, a local for the other slot, where the
         * index's own expression would do */
        other_slot = record->slot ? 0 : 1;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((other = player_work[other_slot]) != NULL &&
            other->mode != PLAYER_MODE_HURT &&
            other->mode != PLAYER_MODE_MISS && !(other->flags & WORK_HURT)) {
            collision_restart_scans();
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((task = collision_next_player(task)) != NULL) {
                /* @hack redundant: other, set to the touched task's work,
                 * which nothing reads */
                other = task->work;
                work->mode = PLAYER_MODE_JUMP;
                record->anim.next = ANIM_UNK_23;
                work->flags &= ~PLAYER_WORK_ATTACK;
                dir = motion->spd;
                dir.y = 0.0F;
                if (njScalor(&dir) > 0.0F) {
                    njUnitVector(&dir);
                    player_face_along(record->slot, dir.x, 0.0F, dir.z);
                } else {
                    /* @hack redundant: a store of dir's x and z that the
                     * speed's store overwrites */
                    dir.x = dir.z = 0.0F;
                }
                /* @hack spelling: dir set to the speed, where the speed's
                 * three constants in the call would do */
                dir.x = -BUMP_BACK;
                dir.y = BUMP_UP;
                dir.z = 0.0F;
                player_set_local_speed(record->slot, dir.x, dir.y, dir.z);
                if (player_hooks.flash) {
                    player_hooks.flash(record->unk_1e8.x, record->unk_1e8.y,
                                       record->unk_1e8.z, BUMP_FLASH_SCALE,
                                       BUMP_FLASH_SPARKS, BUMP_FLASH_BIAS);
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* the task_create levels of the hunt's task and of a piece's icon */
#define HUNT_LEVEL 1
#define PIECE_ICON_LEVEL 1

/* a place hunt_step weighs for a piece, with its measure from the pieces
 * placed before. its rank counts the other places whose measure its own
 * passes. a tie goes to the later place */
struct hunt_choice {
    Sint8 kind;
    Sint8 id;
    Sint16 rank;
    Float measure;
    NJS_POINT3 pos;
};

/* the radar beeps for the current piece within BEEP_RANGE of the nearer
 * player. it beeps once each BEEP_FAR_STEPS steps beyond BEEP_FAR, each
 * BEEP_MIDDLE_STEPS beyond BEEP_NEAR, and each BEEP_NEAR_STEPS within it. a
 * player within BEEP_NEAR says a voice once */
#define BEEP_RANGE 500.0F
#define BEEP_FAR 300.0F
#define BEEP_NEAR 100.0F
#define BEEP_FAR_STEPS 64
#define BEEP_MIDDLE_STEPS 32
#define BEEP_NEAR_STEPS 16
#define SOUND_BEEP 0x1009
/* the voices of a hunter near a piece, by look. hunt->voices keeps
 * HEARD_NEAR(slot) once the player in slot has heard one */
#define HEARD_NEAR(slot) (1 << (slot))
#define VOICE_NEAR_KNUCKLES 0x506
#define VOICE_NEAR_ROUGE 0x507

#ifdef NONMATCHING
/* this side's radar follows every piece not taken, as the first game's does,
 * where SEGA's follows the current piece alone. each such piece shows the
 * gauge its own distance gives, and none shows RADAR_DIM's. the beep goes by
 * the nearest of them. a hunter says the near voice once for each piece:
 * hunt->voices keeps HEARD_PIECE(slot, piece) once the player in slot has
 * heard piece's. a taking clears them all, as it clears SEGA's. in a
 * two-player game, each screen shows its own player's gauges, each at its
 * piece's own place, and the beep goes by the nearer player.
 *
 * the hints keep to the current piece, and so does the bonus they earn */
#define HEARD_PIECE(slot, piece) (HEARD_NEAR(slot) << ((piece) * GAME_PLAYERS))

/* beep for the nearest piece not taken, and say each near voice due. the
 * beep sounds as SEGA's does: in STAGE_PLAY, outside a mini event, while the
 * stage isn't cleared. the voice of a one-player game waits for the same.
 * a two-player game's needs none of it, as SEGA's needs none */
static void hunt_sound_radar(struct hunt *hunt)
{
    Bool playing = stage_get_mode() == STAGE_PLAY &&
                   game_state.mini_event_running == 0 &&
                   game_state.cleared == 0;
    int players = game_state.two_players != 0 ? GAME_PLAYERS : 1;
    Float nearest = BEEP_RANGE + 1.0F;
    Float dist;
    int every;
    int slot;
    int piece;

    for (piece = 0; piece < HUNT_PIECES; piece++) {
        if (hunt->pieces[piece].kind == HUNT_PIECE_TAKEN) {
            continue;
        }
        for (slot = 0; slot < players; slot++) {
            dist = njDistanceP2P(&player_work[slot]->pos,
                                 &hunt->pieces[piece].pos);
            if (dist < nearest) {
                nearest = dist;
            }
            if (dist < BEEP_NEAR &&
                !(hunt->voices & HEARD_PIECE(slot, piece)) &&
                (playing || players == GAME_PLAYERS)) {
                switch (player_records[slot]->kind) {
                case PLAYER_KNUCKLES:
                    voice_play_priority_2(VOICE_NEAR_KNUCKLES);
                    break;
                case PLAYER_ROUGE:
                    voice_play_priority_2(VOICE_NEAR_ROUGE);
                    break;
                }
                hunt->voices |= HEARD_PIECE(slot, piece);
            }
        }
    }
    if (!playing || nearest > BEEP_RANGE) {
        return;
    }
    if (nearest > BEEP_FAR) {
        every = BEEP_FAR_STEPS;
    } else if (nearest > BEEP_NEAR) {
        every = BEEP_MIDDLE_STEPS;
    } else {
        every = BEEP_NEAR_STEPS;
    }
    if (!(game_state.steps & (every - 1))) {
        sound_play(SOUND_BEEP, NULL, 0, 0);
    }
}
#endif

/* the room hunt_step has for the places it weighs for A, more than A's list
 * holds. B's room is stage/hunt.h's HUNT_B_CHOICES, less than B's list and
 * the groups' hold together */
#define A_CHOICES 64
/* B and A each take a place of a rank in the upper half, at random */
#define PICK_SCALE (0.5F * RAND_SCALE)
/* outside a demo, the step draws rand() game_state.total_steps &
 * RAND_SKIP_MASK times before it picks */
#define RAND_SKIP_MASK 0x3ff

/* the step of the task named SearchEmeraldsGameManager, the hunt */
static void hunt_step(struct task *task)
{
    struct hunt *hunt = task->extra.p;
    /* @hack spelling: state, a local for &game_state, where game_state's own
     * fields would do */
    struct game_state *state = &game_state;
    struct hunt_choice a_choices[A_CHOICES];
    NJS_POINT3 b_pos;
    NJS_POINT3 c_pos;
    NJS_VECTOR from_b;
    NJS_VECTOR from_c;
    NJS_VECTOR cross;
    struct hunt_choice b_choices[HUNT_B_CHOICES];
    NJS_POINT3 origin;
    Sint8 index;
    Sint8 pick;
    /* @hack type: one, the outer index of the rankings, as a Sint8, where
     * an int would do */
    Sint8 one;
    Sint8 last;
    int count;
    int other;
    struct hunt_choice *b_cursor;
    struct hunt_choice *a_cursor;
    /* @hack spelling: base, a copy of the choices' array in each selection,
     * where the cursor set from the array would do */
    struct hunt_choice *base;
    struct hunt_piece *piece;
    /* @hack spelling: the slot's id read through id_piece, a second cursor
     * over the slots, with the loop's entry test written out, here and in
     * HUNT_WON, where one cursor in a for would do */
    struct hunt_piece *id_piece;
    int i;
    /* @hack spelling: chosen, an index of its own, where i would do */
    int chosen;
    int j;
    int choices;
    Float dist;
    Float dist_1;
    /* @hack spelling: steps, a local for state->steps in a one-player game,
     * where its reads would do */
    Sint32 steps;
    int winner;

    if (hunt->finish == 1 && hunt->mode != HUNT_START &&
        hunt->mode != HUNT_WAIT) {
        hunt->mode = HUNT_FINISHED;
    } else if (hunt->restart == 1 && hunt->mode != HUNT_START &&
               hunt->mode != HUNT_WAIT && state->mission != MISSION_HD) {
        hunt->mode = HUNT_RESTARTED;
    }
    hunt->finish = 0;
    hunt->restart = 0;
    switch (hunt->mode) {
    case HUNT_START:
        hunt->mode = HUNT_WAIT;
        hunt->c_count = 0;
        hunt->b_count = 0;
        hunt->a_count = 0;
        hunt->enemy_count = 0;
        hunt->c_places = syCalloc(HUNT_PLACES, sizeof(struct hunt_piece));
        hunt->b_places = syCalloc(HUNT_PLACES, sizeof(struct hunt_piece));
        hunt->a_places = syCalloc(HUNT_PLACES, sizeof(struct hunt_piece));
        hunt->enemy_places =
            syCalloc(HUNT_ENEMY_PLACES, sizeof(struct hunt_piece));
        hunt->time = 0;
        hunt->pieces[HUNT_SLOT_C].kind = HUNT_PIECE_EMPTY;
        hunt->pieces[HUNT_SLOT_C].id = 0;
        hunt->pieces[HUNT_SLOT_B].kind = HUNT_PIECE_EMPTY;
        hunt->pieces[HUNT_SLOT_B].id = 0;
        hunt->pieces[HUNT_SLOT_A].kind = HUNT_PIECE_EMPTY;
        hunt->pieces[HUNT_SLOT_A].id = 0;
        hunt->error_kind = HUNT_PIECE_EMPTY;
        hunt->twin.kind = HUNT_PIECE_EMPTY;
        break;
    case HUNT_WAIT:
        if (set_passed == TRUE) {
            hunt->mode = HUNT_SET_DONE;
        }
        break;
    case HUNT_SET_DONE:
        hunt->mode = HUNT_PLACE;
        /* fall through */
    case HUNT_PLACE:
        if (state->mission == MISSION_HD) {
            hunt->mode = HUNT_PLAY;
            hunt->placed = HUNT_PIECES;
            break;
        }
        /* the scans of the four lists run from C's to the enemies'. each keeps
         * the last pair of places listed twice that it finds. one place goes
         * into error_kind, error_id and error_pos, and the other into twin. so
         * the error is the last such pair of all four lists. a list that holds
         * its piece's pinned place keeps that place alone */
        for (i = 0; i < hunt->c_count; i++) {
            for (j = 0; j < hunt->c_count; j++) {
                if (i != j &&
                    hunt->c_places[i].kind == hunt->c_places[j].kind &&
                    hunt->c_places[i].id == hunt->c_places[j].id) {
                    hunt->error_kind = hunt->c_places[i].kind;
                    hunt->error_id = hunt->c_places[i].id;
                    hunt->error_pos = hunt->c_places[i].pos;
                    hunt->twin.kind = hunt->c_places[j].kind;
                    hunt->twin.id = hunt->c_places[j].id;
                    hunt->twin.pos = hunt->c_places[j].pos;
                }
                if (hunt_pin_c_kind == hunt->c_places[i].kind &&
                    hunt_pin_c_id == hunt->c_places[i].id) {
                    hunt->c_count = 1;
                    hunt->c_places[0].kind = hunt->c_places[i].kind;
                    hunt->c_places[0].id = hunt->c_places[i].id;
                    hunt->c_places[0].pos = hunt->c_places[i].pos;
                    break;
                }
            }
        }
        /* @hack spelling: the searches of B's and A's lists as a do-while
         * under a test of its own, where a for would do */
        i = 0;
        if (hunt->b_count > 0) {
            do {
                for (j = 0; j < hunt->b_count; j++) {
                    if (i != j &&
                        hunt->b_places[i].kind == hunt->b_places[j].kind &&
                        hunt->b_places[i].id == hunt->b_places[j].id) {
                        hunt->error_kind = hunt->b_places[i].kind;
                        hunt->error_id = hunt->b_places[i].id;
                        hunt->error_pos = hunt->b_places[i].pos;
                        hunt->twin.kind = hunt->b_places[j].kind;
                        hunt->twin.id = hunt->b_places[j].id;
                        hunt->twin.pos = hunt->b_places[j].pos;
                    }
                    if (hunt_pin_b_kind == hunt->b_places[i].kind &&
                        hunt_pin_b_id == hunt->b_places[i].id) {
                        hunt->b_count = 1;
                        hunt->b_places[0].kind = hunt->b_places[i].kind;
                        hunt->b_places[0].id = hunt->b_places[i].id;
                        hunt->b_places[0].pos = hunt->b_places[i].pos;
                        break;
                    }
                }
                i++;
            } while (i < hunt->b_count);
        }
        i = 0;
        if (hunt->a_count > 0) {
            do {
                for (j = 0; j < hunt->a_count; j++) {
                    if (i != j &&
                        hunt->a_places[i].kind == hunt->a_places[j].kind &&
                        hunt->a_places[i].id == hunt->a_places[j].id) {
                        hunt->error_kind = hunt->a_places[i].kind;
                        hunt->error_id = hunt->a_places[i].id;
                        hunt->error_pos = hunt->a_places[i].pos;
                        hunt->twin.kind = hunt->a_places[j].kind;
                        hunt->twin.id = hunt->a_places[j].id;
                        hunt->twin.pos = hunt->a_places[j].pos;
                    }
                    if (hunt_pin_a_kind == hunt->a_places[i].kind &&
                        hunt_pin_a_id == hunt->a_places[i].id) {
                        hunt->a_count = 1;
                        hunt->a_places[0].kind = hunt->a_places[i].kind;
                        hunt->a_places[0].id = hunt->a_places[i].id;
                        hunt->a_places[0].pos = hunt->a_places[i].pos;
                        break;
                    }
                }
                i++;
            } while (i < hunt->a_count);
        }
        for (i = 0; i < hunt->enemy_count; i++) {
            for (j = 0; j < hunt->enemy_count; j++) {
                if (i != j &&
                    hunt->enemy_places[i].kind == hunt->enemy_places[j].kind &&
                    hunt->enemy_places[i].id == hunt->enemy_places[j].id) {
                    hunt->error_kind = hunt->enemy_places[i].kind;
                    hunt->error_id = hunt->enemy_places[i].id;
                    hunt->error_pos = hunt->enemy_places[i].pos;
                    hunt->twin.kind = hunt->enemy_places[j].kind;
                    hunt->twin.id = hunt->enemy_places[j].id;
                    hunt->twin.pos = hunt->enemy_places[j].pos;
                }
                if (hunt_pin_c_kind == hunt->enemy_places[i].kind &&
                    hunt_pin_c_id == hunt->enemy_places[i].id) {
                    hunt->enemy_count = 1;
                    hunt->enemy_places[0].kind = hunt->enemy_places[i].kind;
                    hunt->enemy_places[0].id = hunt->enemy_places[i].id;
                    hunt->enemy_places[0].pos = hunt->enemy_places[i].pos;
                    break;
                }
            }
        }
        /* a pin of C to a group's kind empties C's own list, and a pin to
         * one of C's kinds empties the groups' */
        if (hunt_pin_c_kind == EMERALD_KIND_INENEMY) {
            hunt->c_count = 0;
        } else if (hunt_pin_c_kind == EMERALD_KIND_C_NORMAL ||
                   hunt_pin_c_kind == EMERALD_KIND_C_HIDDEN) {
            hunt->enemy_count = 0;
        }
        if (state->demo == 0) {
            for (choices = state->total_steps & RAND_SKIP_MASK; choices > 0;
                 choices--) {
                rand();
            }
        }
        /* C lies at one of its places or the groups', at random. a group's
         * place it takes leaves the groups' list */
        if (hunt->pieces[HUNT_SLOT_C].kind != HUNT_PIECE_TAKEN) {
            pick = (Float)(hunt->c_count + hunt->enemy_count) * rand() *
                   RAND_SCALE;
            if (pick >= hunt->c_count) {
                pick -= hunt->c_count;
                if (pick >= hunt->enemy_count) {
                    pick = 0;
                }
                hunt->pieces[HUNT_SLOT_C] = hunt->enemy_places[pick];
                if (pick < hunt->enemy_count - 1) {
                    /* @hack spelling: last, a local for the index, where its
                     * one use would do */
                    last = hunt->enemy_count - 1;
                    hunt->enemy_places[pick] = hunt->enemy_places[last];
                }
                hunt->enemy_count--;
            } else {
                hunt->pieces[HUNT_SLOT_C] = hunt->c_places[pick];
            }
            hunt->placed++;
        }
        /* B lies among the farther half of its places and the groups'
         * from C */
        if (hunt->pieces[HUNT_SLOT_B].kind != HUNT_PIECE_TAKEN) {
            origin = hunt->pieces[HUNT_SLOT_C].pos;
            choices = 0;
            for (index = 0; index < hunt->b_count; index++) {
                b_choices[choices].kind = hunt->b_places[index].kind;
                b_choices[choices].id = hunt->b_places[index].id;
                b_choices[choices].pos = hunt->b_places[index].pos;
                b_choices[choices].measure =
                    njDistanceP2P(&hunt->b_places[index].pos, &origin);
                b_choices[choices].rank = 0;
                choices++;
            }
            for (index = 0; index < hunt->enemy_count; index++) {
                b_choices[choices].kind = hunt->enemy_places[index].kind;
                b_choices[choices].id = hunt->enemy_places[index].id;
                b_choices[choices].pos = hunt->enemy_places[index].pos;
                b_choices[choices].measure =
                    njDistanceP2P(&hunt->enemy_places[index].pos, &origin);
                b_choices[choices].rank = 0;
                choices++;
            }
            /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
            for (one = 0; one < choices; one++) {
                for (other = one + 1; other < choices; other++) {
                    if (b_choices[one].measure > b_choices[other].measure) {
                        b_choices[one].rank++;
                    } else {
                        b_choices[other].rank++;
                    }
                }
            }
            pick = (Float)choices - ((Float)choices * rand() * PICK_SCALE);
            if (pick >= choices) {
                pick = choices - 1;
            }
            i = 0;
            base = b_choices;
            b_cursor = base;
            while (i < choices) {
                if (b_cursor->rank == pick) {
                    hunt->pieces[HUNT_SLOT_B].kind = b_choices[i].kind;
                    hunt->pieces[HUNT_SLOT_B].id = b_choices[i].id;
                    hunt->pieces[HUNT_SLOT_B].pos = b_choices[i].pos;
                }
                i++;
                b_cursor++;
            }
            hunt->placed++;
        }
        /* A lies among the half of its places farther from the line through
         * C and B. a place's measure is the length of the cross product of
         * its offsets from B and C */
        if (hunt->pieces[HUNT_SLOT_A].kind != HUNT_PIECE_TAKEN) {
            c_pos = hunt->pieces[HUNT_SLOT_C].pos;
            b_pos = hunt->pieces[HUNT_SLOT_B].pos;
            choices = 0;
            for (index = 0; index < hunt->a_count; index++) {
                from_b = hunt->a_places[index].pos;
                from_c = hunt->a_places[index].pos;
                njSubVector(&from_b, &b_pos);
                njSubVector(&from_c, &c_pos);
                a_choices[choices].kind = hunt->a_places[index].kind;
                a_choices[choices].id = hunt->a_places[index].id;
                a_choices[choices].pos = hunt->a_places[index].pos;
                a_choices[choices].measure =
                    njOuterProduct(&from_b, &from_c, &cross);
                a_choices[choices].rank = 0;
                choices++;
            }
            /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
            for (one = 0; one < choices; one++) {
                for (other = one + 1; other < choices; other++) {
                    if (a_choices[one].measure > a_choices[other].measure) {
                        a_choices[one].rank++;
                    } else {
                        a_choices[other].rank++;
                    }
                }
            }
            pick = (Float)choices - ((Float)choices * rand() * PICK_SCALE);
            if (pick >= choices) {
                pick = choices - 1;
            }
            i = 0;
            base = a_choices;
            a_cursor = base;
            while (i < choices) {
                if (a_cursor->rank == pick) {
                    hunt->pieces[HUNT_SLOT_A].kind = a_choices[i].kind;
                    hunt->pieces[HUNT_SLOT_A].id = a_choices[i].id;
                    hunt->pieces[HUNT_SLOT_A].pos = a_choices[i].pos;
                }
                i++;
                a_cursor++;
            }
            hunt->placed++;
        }
        hunt->mode = HUNT_PLAY;
        break;
    case HUNT_PLAY: {
        /* @hack spelling: beep_range, a const local for BEEP_RANGE, where
         * the constant would do */
        const Float beep_range = BEEP_RANGE;

        /* @hack redundant: reads in a block that never runs */
        if (0) {
            /* NOLINTBEGIN(misc-redundant-expression) */
            if (!hunt | !hunt | !hunt | !hunt | (count >= 2) | (count >= 2) |
                (count >= 2) | (dist_1 > BEEP_FAR)) {
            }
            /* NOLINTEND(misc-redundant-expression) */
            if (dist > BEEP_NEAR) {
            }
        }
        /* a two-player game is won once player 0 holds two pieces. it is won
         * too once one piece at most is placed and player 0 holds none */
        if (state->two_players != 0) {
            count = 0;
            piece = hunt->pieces;
            id_piece = hunt->pieces;
            if (piece < hunt->pieces + HUNT_PIECES) {
                do {
                    if (piece->kind == HUNT_PIECE_TAKEN) {
                        if (id_piece->id == 0) {
                            count++;
                        }
                    }
                    piece++;
                    /* @hack spelling: id_piece = id_piece + 1, here and in
                     * HUNT_WON, where id_piece++ would do */
                    id_piece = id_piece + 1;
                } while (piece < hunt->pieces + HUNT_PIECES);
            }
            if (count >= 2 || (hunt->placed <= 1 && count == 0)) {
                hunt->mode = HUNT_WON;
                break;
            }
#ifdef NONMATCHING
            /* this side beeps and speaks for every piece not taken */
            hunt_sound_radar(hunt);
#else
            /* the current piece beeps by the nearer player's distance. each
             * player says the voice once, near it */
            for (chosen = 0; chosen < HUNT_PIECES; chosen++) {
                /* @hack spelling: a test that continues before the break,
                 * here and in the one-player arm, where a test that breaks
                 * would do */
                if (hunt->pieces[chosen].kind == HUNT_PIECE_TAKEN) {
                    continue;
                }
                break;
            }
            if (chosen < HUNT_PIECES) {
                dist = njDistanceP2P(&player_work[0]->pos,
                                     &hunt->pieces[chosen].pos);
                if (!(hunt->voices & HEARD_NEAR(0)) && dist < BEEP_NEAR) {
                    switch (player_records[0]->kind) {
                    case PLAYER_KNUCKLES:
                        voice_play_priority_2(VOICE_NEAR_KNUCKLES);
                        break;
                    case PLAYER_ROUGE:
                        voice_play_priority_2(VOICE_NEAR_ROUGE);
                        break;
                    }
                    hunt->voices |= HEARD_NEAR(0);
                }
                dist_1 = njDistanceP2P(&player_work[1]->pos,
                                       &hunt->pieces[chosen].pos);
                if (!(hunt->voices & HEARD_NEAR(1)) && dist_1 < BEEP_NEAR) {
                    switch (player_records[1]->kind) {
                    case PLAYER_KNUCKLES:
                        voice_play_priority_2(VOICE_NEAR_KNUCKLES);
                        break;
                    case PLAYER_ROUGE:
                        voice_play_priority_2(VOICE_NEAR_ROUGE);
                        break;
                    }
                    hunt->voices |= HEARD_NEAR(1);
                }
                if (dist > dist_1) {
                    dist = dist_1;
                }
                if (stage_get_mode() == STAGE_PLAY) {
                    /* @hack type: game_state's mini_event_running and
                     * cleared read as game_state_mini_event_running and
                     * game_state_cleared, here and below, where state's would
                     * do */
                    if (game_state_mini_event_running == 0) {
                        if (game_state_cleared == 0) {
                            if (!(dist > beep_range)) {
                                if (dist > BEEP_FAR) {
                                    if (!(state->steps &
                                          (BEEP_FAR_STEPS - 1))) {
                                        sound_play(SOUND_BEEP, NULL, 0, 0);
                                    }
                                } else if (dist > BEEP_NEAR) {
                                    if (!(state->steps &
                                          (BEEP_MIDDLE_STEPS - 1))) {
                                        sound_play(SOUND_BEEP, NULL, 0, 0);
                                    }
                                } else {
                                    if (!(state->steps &
                                          (BEEP_NEAR_STEPS - 1))) {
                                        sound_play(SOUND_BEEP, NULL, 0, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
#endif
        } else {
#ifdef NONMATCHING
            /* this side beeps and speaks for every piece not taken */
            hunt_sound_radar(hunt);
#else
            for (i = 0; i < HUNT_PIECES; i++) {
                if (hunt->pieces[i].kind == HUNT_PIECE_TAKEN) {
                    continue;
                }
                break;
            }
            if (i < HUNT_PIECES) {
                dist =
                    njDistanceP2P(&player_work[0]->pos, &hunt->pieces[i].pos);
                if (stage_get_mode() == STAGE_PLAY) {
                    if (game_state_mini_event_running == 0) {
                        if (game_state_cleared == 0) {
                            if (!(dist > beep_range)) {
                                steps = state->steps;
                                if (dist > BEEP_FAR) {
                                    if (!(steps & (BEEP_FAR_STEPS - 1))) {
                                        sound_play(SOUND_BEEP, NULL, 0, 0);
                                    }
                                } else if (dist > BEEP_NEAR) {
                                    if (!(steps & (BEEP_MIDDLE_STEPS - 1))) {
                                        sound_play(SOUND_BEEP, NULL, 0, 0);
                                    }
                                } else {
                                    if (!(steps & (BEEP_NEAR_STEPS - 1))) {
                                        sound_play(SOUND_BEEP, NULL, 0, 0);
                                    }
                                    if (hunt->voices == 0) {
                                        switch (player_records[0]->kind) {
                                        case PLAYER_KNUCKLES:
                                            voice_play_priority_2(
                                                VOICE_NEAR_KNUCKLES);
                                            break;
                                        case PLAYER_ROUGE:
                                            voice_play_priority_2(
                                                VOICE_NEAR_ROUGE);
                                            break;
                                        }
                                        hunt->voices = HEARD_NEAR(0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
#endif
        }
        if (hunt->placed == 0) {
            hunt->mode = HUNT_WON;
        }
        break;
    }
    case HUNT_WON:
        /* player 1 wins a two-player game where it holds two pieces or
         * more, and player 0 otherwise */
        winner = 0;
        if (state->two_players != 0) {
            /* @hack redundant: a store to i that nothing reads */
            i = 0;
            count = 0;
            id_piece = piece = hunt->pieces;
            if (piece < hunt->pieces + HUNT_PIECES) {
                do {
                    if (piece->kind == HUNT_PIECE_TAKEN) {
                        if (id_piece->id == 1) {
                            /* @hack spelling: count = count + 1, where
                             * count++ would do */
                            count = count + 1;
                        }
                    }
                    /* @hack redundant: reads in a block that never runs */
                    if (0) {
                        if (!id_piece | !count | (count == 0) |
                            (id_piece == 0)) {
                        }
                    }
                    piece++;
                    id_piece = id_piece + 1;
                } while (piece < hunt->pieces + HUNT_PIECES);
            }
            if (count >= 2) {
                winner = 1;
            }
        }
        stage_clear(winner);
        hunt->mode = HUNT_FINISHED;
        break;
    case HUNT_FINISHED:
        if ((stage_get_mode() == STAGE_FADE_IN ||
             stage_get_mode() == STAGE_PLAY) &&
            game_state_cleared != 1) {
            /* @hack redundant: a test of the mission whose arms both set
             * HUNT_WAIT, here and in HUNT_RESTARTED */
            if (state->mission == MISSION_HD) {
                hunt->mode = HUNT_WAIT;
            } else {
                hunt->mode = HUNT_WAIT;
            }
            hunt->c_count = 0;
            hunt->b_count = 0;
            hunt->a_count = 0;
            hunt->enemy_count = 0;
            hunt->placed = 0;
            hunt->hints = 0;
            hunt->voices = 0;
            hunt->pieces[HUNT_SLOT_C].kind = HUNT_PIECE_EMPTY;
            hunt->pieces[HUNT_SLOT_C].id = 0;
            hunt->pieces[HUNT_SLOT_B].kind = HUNT_PIECE_EMPTY;
            hunt->pieces[HUNT_SLOT_B].id = 0;
            hunt->pieces[HUNT_SLOT_A].kind = HUNT_PIECE_EMPTY;
            hunt->pieces[HUNT_SLOT_A].id = 0;
            hunt->error_kind = HUNT_PIECE_EMPTY;
            hunt->twin.kind = HUNT_PIECE_EMPTY;
            hunt->time = 0;
        }
        break;
    case HUNT_RESTARTED:
        if ((stage_get_mode() == STAGE_FADE_IN ||
             stage_get_mode() == STAGE_PLAY) &&
            game_state_cleared != 1) {
            if (state->mission == MISSION_HD) {
                hunt->mode = HUNT_WAIT;
            } else {
                hunt->mode = HUNT_WAIT;
            }
            hunt->c_count = 0;
            hunt->b_count = 0;
            hunt->a_count = 0;
            hunt->enemy_count = 0;
            hunt->placed = 0;
            hunt->hints = 0;
            hunt->voices = 0;
            if (hunt->pieces[HUNT_SLOT_C].kind != HUNT_PIECE_TAKEN) {
                hunt->pieces[HUNT_SLOT_C].kind = HUNT_PIECE_EMPTY;
                hunt->pieces[HUNT_SLOT_C].id = 0;
            }
            if (hunt->pieces[HUNT_SLOT_B].kind != HUNT_PIECE_TAKEN) {
                hunt->pieces[HUNT_SLOT_B].kind = HUNT_PIECE_EMPTY;
                hunt->pieces[HUNT_SLOT_B].id = 0;
            }
            if (hunt->pieces[HUNT_SLOT_A].kind != HUNT_PIECE_TAKEN) {
                hunt->pieces[HUNT_SLOT_A].kind = HUNT_PIECE_EMPTY;
                hunt->pieces[HUNT_SLOT_A].id = 0;
            }
            hunt->error_kind = HUNT_PIECE_EMPTY;
            hunt->twin.kind = HUNT_PIECE_EMPTY;
            hunt->time = 0;
        }
        break;
    }
    if (hunt->time < HUNT_TIME_MAX) {
        hunt->time++;
    }
}

/* the radar's gauge textures. each name starts a run of HUNT_PIECES, one
 * texture for each slot in order. a piece after the current one shows
 * RADAR_DIM's. the current piece's goes by the distance to it: RADAR_FAR's
 * beyond BEEP_RANGE, and RADAR_MIDDLE's beyond BEEP_FAR. it shows
 * RADAR_NEAR's beyond BEEP_NEAR, and RADAR_CLOSE's within it. a gauge of the
 * runs from RADAR_CLOSE to RADAR_MIDDLE's draws a ring about it */
#define RADAR_FAR 4
#define RADAR_DIM 7
#define RADAR_CLOSE 10
#define RADAR_NEAR 13
#define RADAR_MIDDLE 16
/* the texture of the radar's ring and of an icon's flash for slot or place
 * 0. each next one's is the next texture */
#define TEX_FLASH 19

/* where the gauges draw on the screen, in pixels. a gauge reaches from
 * GAUGE_TOP down to GAUGE_BOTTOM, and is GAUGE_SIDE wide. on one screen,
 * place 0's left edge lies at GAUGE_LEFT, and each next place's GAUGE_GAP
 * further right. on two screens, place 0's lies at GAUGE_LEFT_0 on player
 * 0's and GAUGE_LEFT_1 on player 1's. each next place's lies GAUGE_GAP_SPLIT
 * further right */
#define GAUGE_TOP 408.0F
#define GAUGE_BOTTOM 440.0F
#define GAUGE_SIDE 32.0F
#define GAUGE_LEFT 264.0F
#define GAUGE_GAP 40.0F
#define GAUGE_LEFT_0 112.0F
#define GAUGE_LEFT_1 432.0F
#define GAUGE_GAP_SPLIT 32.0F
#define GAUGE_RIGHT (GAUGE_LEFT + GAUGE_SIDE)
#define GAUGE_RIGHT_0 (GAUGE_LEFT_0 + GAUGE_SIDE)
#define GAUGE_RIGHT_1 (GAUGE_LEFT_1 + GAUGE_SIDE)
/* a ring swells (steps & RING_SWELL_BITS) >> RING_SWELL_SHIFT pixels on each
 * side, up to RING_SWELLS - 1. it grows a pixel each 2 steps, and starts
 * again each 32. the ring is RING_COLOR, white, at an alpha that falls by
 * RING_ALPHA_STEP with each pixel. ALPHA_SHIFT places an alpha in a color */
#define RING_SWELL_BITS 0x1e
#define RING_SWELL_SHIFT 1
#define RING_SWELLS 16
#define RING_COLOR 0x00ffffff
#define RING_ALPHA_STEP 16

/* the radar's two quads: a piece's gauge, and the ring about it.
 * hunt_draw_gauge sets their corners' x and y before each draw */
static NJS_TEXTURE_VTX radar_quads[8] = {
#include "player/knuckles/radar_quads.inc"
};

/* draw the radar's gauge of texture tex at place, on camera_current_index's
 * screen. place is the slot in a one-player game. in a two-player game it is
 * the count of pieces the screen's player holds. a gauge of the runs from
 * RADAR_CLOSE to RADAR_MIDDLE's first draws a ring about it. the ring's texture
 * is place + TEX_FLASH. swell, a count of steps, swells the ring as
 * RING_SWELL_BITS says. the call leaves the current texture at tex */
static void hunt_draw_gauge(int tex, int place, int swell)
{
    /* @hack spelling: swell, the count of steps, made the ring's swell,
     * where a local of its own would do */
    if (tex >= RADAR_FAR && tex <= RADAR_DIM + HUNT_PIECES - 1) {
        swell = 0;
    } else {
        swell = (swell & RING_SWELL_BITS) >> RING_SWELL_SHIFT;
    }
    if (game_state.two_players == 0) {
        if (tex >= RADAR_CLOSE && tex <= RADAR_MIDDLE + HUNT_PIECES - 1) {
            radar_quads[4].x = radar_quads[5].x =
                ((place * GAUGE_GAP) - swell) + GAUGE_LEFT;
            radar_quads[6].x = radar_quads[7].x =
                (swell + (place * GAUGE_GAP)) + GAUGE_RIGHT;
            radar_quads[4].y = radar_quads[6].y = GAUGE_TOP - swell;
            radar_quads[5].y = radar_quads[7].y = swell + GAUGE_BOTTOM;
            radar_quads[4].col = radar_quads[5].col = radar_quads[6].col =
                radar_quads[7].col =
                    ((((RING_SWELLS - 1) - swell) * RING_ALPHA_STEP)
                     << ALPHA_SHIFT) |
                    RING_COLOR;
            njSetTextureNum(place + TEX_FLASH);
            njDrawTexture(&radar_quads[4], 4, njGetTextureNumG(), 1);
        }
        radar_quads[0].x = radar_quads[1].x = (place * GAUGE_GAP) + GAUGE_LEFT;
        radar_quads[2].x = radar_quads[3].x = (place * GAUGE_GAP) + GAUGE_RIGHT;
        radar_quads[0].y = radar_quads[2].y = GAUGE_TOP;
        radar_quads[1].y = radar_quads[3].y = GAUGE_BOTTOM;
    } else {
        if (tex >= RADAR_CLOSE && tex <= RADAR_MIDDLE + HUNT_PIECES - 1) {
            if (camera_current_index == 0) {
                radar_quads[4].x = radar_quads[5].x =
                    ((place * GAUGE_GAP_SPLIT) - swell) + GAUGE_LEFT_0;
                radar_quads[6].x = radar_quads[7].x =
                    (swell + (place * GAUGE_GAP_SPLIT)) + GAUGE_RIGHT_0;
            } else {
                radar_quads[4].x = radar_quads[5].x =
                    ((place * GAUGE_GAP_SPLIT) - swell) + GAUGE_LEFT_1;
                radar_quads[6].x = radar_quads[7].x =
                    (swell + (place * GAUGE_GAP_SPLIT)) + GAUGE_RIGHT_1;
            }
            radar_quads[4].y = radar_quads[6].y = GAUGE_TOP - swell;
            radar_quads[5].y = radar_quads[7].y = swell + GAUGE_BOTTOM;
            radar_quads[4].col = radar_quads[5].col = radar_quads[6].col =
                radar_quads[7].col =
                    ((((RING_SWELLS - 1) - swell) * RING_ALPHA_STEP)
                     << ALPHA_SHIFT) |
                    RING_COLOR;
            njSetTextureNum(place + TEX_FLASH);
            njDrawTexture(&radar_quads[4], 4, njGetTextureNumG(), 1);
        }
        if (camera_current_index == 0) {
            radar_quads[0].x = radar_quads[1].x =
                (place * GAUGE_GAP_SPLIT) + GAUGE_LEFT_0;
            radar_quads[2].x = radar_quads[3].x =
                (place * GAUGE_GAP_SPLIT) + GAUGE_RIGHT_0;
        } else {
            radar_quads[0].x = radar_quads[1].x =
                (place * GAUGE_GAP_SPLIT) + GAUGE_LEFT_1;
            radar_quads[2].x = radar_quads[3].x =
                (place * GAUGE_GAP_SPLIT) + GAUGE_RIGHT_1;
        }
        radar_quads[0].y = radar_quads[2].y = GAUGE_TOP;
        radar_quads[1].y = radar_quads[3].y = GAUGE_BOTTOM;
    }
    njSetTextureNum(tex);
    njDrawTexture(radar_quads, 4, njGetTextureNumG(), 1);
}

/* the hunt's draw: the radar at the screen's foot. it draws nothing while
 * game_state.unk_0b is 0, or a player plays solo. nor does it while the
 * stage is cleared or a mini event runs */
static void hunt_draw(struct task *task)
{
    struct hunt *hunt = hunt_current;
    /* @hack spelling: state, a local for &game_state, where game_state's own
     * fields would do */
    struct game_state *state = &game_state;
    /* TRUE for the current piece, the first not taken */
    Bool first = TRUE;
    int steps;
    int count;
    int far;
    int middle;
    int near;
    int close;
    int i;
    int j;
    Float dist;
    /* @hack spelling: works, a local for player_work, where player_work
     * would do */
    struct task_work **works;
    /* @hack spelling: none, a local for the -1 of no distance, where the
     * constant would do */
    Float none;

    if (state->unk_0b == 0) {
        return;
    }
    if (state->cleared == 1) {
        return;
    }
    if (state->solo_player != 0) {
        return;
    }
    if (state->mini_event_running == 1) {
        return;
    }
    steps = state->steps;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(hunt->texlist);
    works = player_work;
    none = -1.0F;
    for (i = 0, count = 0; i < HUNT_PIECES; i++) {
        /* @hack spelling: far and middle set in the arm of a test that
         * continues in its else, where a test that continues before them
         * would do */
        if (hunt->pieces[i].kind != HUNT_PIECE_TAKEN) {
            far = i + RADAR_FAR;
            middle = i + RADAR_MIDDLE;
        } else {
            continue;
        }
        near = i + RADAR_NEAR;
        close = i + RADAR_CLOSE;
        /* @hack redundant: reads in a block that never runs */
        if (0) {
            if (first) {
            }
            if (hunt->pieces[i].kind) {
            }
            if (hunt->pieces[i].kind) {
            }
        }
        /* in a two-player game, the screen's player sees the current piece's
         * gauge alone. it lies at the place the player's count of pieces
         * gives. the player sees none once it holds two pieces, or with no
         * piece left */
        if (state->two_players != 0) {
#ifdef NONMATCHING
            /* this side shows the screen's player a gauge for each piece
             * not taken, as it shows a one-player game's below: at the
             * piece's own place, by that player's distance. the player
             * still sees none once it holds two pieces */
            for (j = 0, count = 0; j < HUNT_PIECES; j++) {
                if (hunt->pieces[j].kind == HUNT_PIECE_TAKEN &&
                    hunt->pieces[j].id == camera_current_index) {
                    count++;
                }
            }
            if (count >= 2) {
                break;
            }
            works = &player_work[camera_current_index];
#else
            dist = none;
            for (j = 0; j < HUNT_PIECES; j++) {
                if (hunt->pieces[j].kind == HUNT_PIECE_TAKEN) {
                    if (hunt->pieces[j].id == camera_current_index) {
                        count++;
                    }
                } else if (dist < 0.0F) {
                    dist =
                        njDistanceP2P(&player_work[camera_current_index]->pos,
                                      &hunt->pieces[j].pos);
                }
            }
            if (count >= 2) {
                break;
            }
            if (dist < 0.0F) {
                break;
            }
            if (dist > BEEP_RANGE) {
                hunt_draw_gauge(far, count, steps);
            } else if (dist > BEEP_FAR) {
                hunt_draw_gauge(middle, count, steps);
            } else if (dist > BEEP_NEAR) {
                hunt_draw_gauge(near, count, steps);
            } else {
                hunt_draw_gauge(close, count, steps);
            }
            break;
#endif
        }
        /* a one-player game shows a gauge for each piece not taken, by
         * player 0's distance for the current piece and dim for the
         * others */
        dist = njDistanceP2P(&works[0]->pos, &hunt->pieces[i].pos);
        if (!first) {
            hunt_draw_gauge(i + RADAR_DIM, i, steps);
        } else if (dist > BEEP_RANGE) {
            hunt_draw_gauge(far, i, steps);
        } else if (dist > BEEP_FAR) {
            hunt_draw_gauge(middle, i, steps);
        } else if (dist > BEEP_NEAR) {
            hunt_draw_gauge(near, i, steps);
        } else {
            hunt_draw_gauge(close, i, steps);
        }
#ifdef NONMATCHING
        /* this side leaves first TRUE, so that each piece not taken shows
         * the gauge its own distance gives (the port's radar, above
         * hunt_step) */
#else
        /* @hack spelling: first = count, where first = 0 would do: only the
         * two-player arm counts, and it leaves the loop */
        first = count;
#endif
    }
    /* @hack redundant: five reads of hunt and two of two_players, in a block
     * that never runs */
    if (0) {
        if (hunt) {
        }
        if (hunt) {
        }
        if (hunt) {
        }
        if (hunt) {
        }
        if (hunt) {
        }
        if (state->two_players) {
        }
        if (state->two_players) {
        }
    }
    /* @hack redundant: a read in a block that never runs */
    if (0) {
        if (camera_current_index) {
        }
    }
    /* @hack redundant: a test of the hints with nothing in its arm */
    if (hunt->hints) {
    }
}

/* texts of the hunt's errors. OKISUGI is too many places in a list, and
 * DABURI a place listed twice */
/* @hack redundant: hunt_print_errors, which nothing calls. SHC leaves out
 * its code, and keeps its texts among the object's constants */
static void
hunt_print_errors(void) /* NOLINT(clang-diagnostic-unused-function) */
{
    njPrintC(0, "EMERALD C ERROR(OKISUGI)");
    njPrintC(0, "EMERALD B ERROR(OKISUGI)");
    njPrintC(0, "EMERALD A ERROR(OKISUGI)");
    njPrintC(0, "EMERALD ERROR(OKISUGI)");
    njPrintC(0, "EMERALD ERROR(ID)");
    njPrintC(0, "TYPE:");
    njPrintC(0, "ID:");
    njPrintC(0, "EMERALD ERROR(DABURI)");
}

/* the hunt's destructor. hunt_current becomes null, which lets the next
 * hunt_start make a hunt. it frees the lists and the textures, and
 * task_destroy frees the record */
static void hunt_destroy(struct task *task)
{
    struct hunt *hunt = task->extra.p;

    hunt_current = NULL;
    if (hunt == NULL) {
        return;
    }
    if (hunt->c_places) {
        syFree(hunt->c_places);
        hunt->c_places = NULL;
    }
    if (hunt->b_places) {
        syFree(hunt->b_places);
        hunt->b_places = NULL;
    }
    if (hunt->a_places) {
        syFree(hunt->a_places);
        hunt->a_places = NULL;
    }
    if (hunt->enemy_places) {
        syFree(hunt->enemy_places);
        hunt->enemy_places = NULL;
    }
    if (hunt->texlist) {
        njReleaseTexture(hunt->texlist);
        syFree(hunt->texlist);
        hunt->texlist = NULL;
    }
}

void hunt_start(void)
{
    struct task *task;
    struct hunt *hunt;

    if (stage_current_id() > STAGE_STG_END) {
        return;
    }
    if (stage_current_id() == STAGE_VERSUS_KNUCKLES) {
        return;
    }
    if (stage_current_id() == STAGE_RELAY_ROUGE) {
        return;
    }
    if (stage_current_id() == STAGE_RELAY_KNUCKLES) {
        return;
    }
    if (game_state.mission == MISSION_RINGS ||
        game_state.mission == MISSION_CHAO) {
        return;
    }
    if (hunt_current) {
        return;
    }
    /* @hack spelling: the assignments inside the tests, here and below,
     * where a statement of its own for each would do */
    if ((task = task_create(0, HUNT_LEVEL, hunt_step,
                            "SearchEmeraldsGameManager")) == NULL) {
        return;
    }
    if ((hunt = syCalloc(1, sizeof(struct hunt))) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = hunt;
    hunt_current = hunt;
    switch (stage_current_id()) {
    case STAGE_SECURITY_HALL:
        hunt->texlist = texture_load_pvm_alloc("KGAMETEXSH");
        break;
    case STAGE_EGG_QUARTERS:
    case STAGE_DEATH_CHAMBER:
        hunt->texlist = texture_load_pvm_alloc("KGAMETEXDZ");
        break;
    default:
        hunt->texlist = texture_load_pvm_alloc("KGAMETEX");
        break;
    }
    task->destructor = hunt_destroy;
    task->draw = hunt_draw;
}

/* the fields of a piece's icon in its task's work. ICON_PLACE is a PLACE_
 * place. the size shrinks by ICON_SHRINK a step, from ICON_SIZE_START to 1.
 * the alpha climbs by ICON_FADE_IN a step, from ICON_ALPHA_START to 1. an
 * alpha below 0 draws as 0. once the size is 1, the flash's alpha falls by
 * ICON_FLASH_FADE a step, from 1 to 0 */
#define ICON_SLOT(work) ((work)->tick)
#define ICON_PLACE(work) ((work)->submode)
#define ICON_SIZE(work) ((work)->scl.x)
#define ICON_ALPHA(work) ((work)->scl.y)
#define ICON_FLASH(work) ((work)->scl.z)
#define ICON_SIZE_START 8.0F
#define ICON_SHRINK 0.5F
#define ICON_ALPHA_START (-0.9F)
#define ICON_FADE_IN 0.1F
#define ICON_FLASH_FADE 0.02F
/* the icons' places: the slot's own in a one-player game, and in a
 * two-player game the first and the second of player 0's pieces and of
 * player 1's */
#define PLACE_ONE_PLAYER 0
#define PLACE_PLAYER_0 3
#define PLACE_PLAYER_1 5
#define ICON_PLACES 7
/* the y of the icons on the screen, and their priorities and their
 * flashes' */
#define ICON_Y 48.0F
#define ICON_PRIORITY (-2.0F)
#define FLASH_PRIORITY (-1.99F)

/* the cells of the icons: the flash over an icon, which piece_icon_flash
 * points at its slot's texture, and the icon of each piece by its slot */
static NJS_TEXANIM flash_cells[1] = {
#include "player/knuckles/flash_cells.inc"
};
static NJS_TEXANIM icon_cells[HUNT_PIECES] = {
#include "player/knuckles/icon_cells.inc"
};

/* the x of an icon's center on the screen, by its place, and on
 * STAGE_SECURITY_HALL */
static Float icon_x[ICON_PLACES] = {
    288.0F, 320.0F, 352.0F, 128.0F, 160.0F, 448.0F, 480.0F,
};
static Float icon_x_sh[ICON_PLACES] = {
    282.0F, 320.0F, 358.0F, 128.0F, 160.0F, 448.0F, 480.0F,
};

/* the flash over an icon: it grows from nothing to twice the icon's size as
 * it fades */
static void piece_icon_flash(struct task *task)
{
    struct hunt *hunt = hunt_current;
    struct task_work *work = task->work;
    NJS_SPRITE sprite;

    sprite.tanim = flash_cells;
    sprite.tlist = hunt->texlist;
    sprite.p = work->pos;
    sprite.sx = sprite.sy = (1.0F - ICON_FLASH(work)) * 2.0F;
    constant_material_set(ICON_FLASH(work), 1.0F, 1.0F, 1.0F);
    flash_cells[0].texid = ICON_SLOT(work) + TEX_FLASH;
    njDrawSprite2D(&sprite, 0, FLASH_PRIORITY,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
}

/* the icon's draw. it draws nothing while game_state.unk_0b is 0, or a
 * player plays solo. nor does it while the stage is cleared or a mini event
 * runs. the icon's slot must be a piece's, unchecked: past HUNT_SLOT_A the
 * sprite's cell is left unset. the call leaves the constant material at 0 */
static void piece_icon_draw(struct task *task)
{
    struct hunt *hunt = hunt_current;
    struct task_work *work = task->work;
    NJS_SPRITE sprite;
    int slot;

    if (game_state.unk_0b == 0) {
        return;
    }
    if (game_state.cleared == 1) {
        return;
    }
    if (game_state.mini_event_running == 1) {
        return;
    }
    if (game_state.solo_player != 0) {
        return;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* @hack spelling: a test for each slot, where &icon_cells[slot] would
     * do for the three */
    slot = ICON_SLOT(work);
    if (slot == HUNT_SLOT_C) {
        sprite.tanim = &icon_cells[HUNT_SLOT_C];
    } else if (slot == HUNT_SLOT_B) {
        sprite.tanim = &icon_cells[HUNT_SLOT_B];
    } else if (slot == HUNT_SLOT_A) {
        sprite.tanim = &icon_cells[HUNT_SLOT_A];
    }
    sprite.tlist = hunt->texlist;
    sprite.p = work->pos;
    sprite.sx = sprite.sy = ICON_SIZE(work);
    constant_material_set(ICON_ALPHA(work), 1.0F, 1.0F, 1.0F);
    njDrawSprite2D(&sprite, 0, ICON_PRIORITY,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
    if (ICON_SIZE(work) == 1.0F && ICON_FLASH(work) > 0.0F) {
        piece_icon_flash(task);
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
}

/* the step of the task named ExecDisplayGottenEmeraldIcon. the icon ends in
 * STAGE_GO_TO_FADE, and once its piece's slot no longer holds a taken piece.
 * hunt_current must not be null, unchecked */
static void piece_icon_step(struct task *task)
{
    struct task_work *work = task->work;

    if (ICON_SIZE(work) > 1.0F) {
        ICON_SIZE(work) -= ICON_SHRINK;
    } else {
        ICON_SIZE(work) = 1.0F;
        if (ICON_FLASH(work) > 0.0F) {
            ICON_FLASH(work) -= ICON_FLASH_FADE;
        } else {
            ICON_FLASH(work) = 0.0F;
        }
    }
    if (ICON_ALPHA(work) < 1.0F) {
        ICON_ALPHA(work) += ICON_FADE_IN;
    } else {
        ICON_ALPHA(work) = 1.0F;
    }
    if (stage_get_mode() == STAGE_GO_TO_FADE) {
        task_kill(task);
        return;
    }
    if (hunt_current->pieces[ICON_SLOT(work)].kind != HUNT_PIECE_TAKEN) {
        task_kill(task);
    }
}

/* start the icon of the piece in slot, which player has taken, at the
 * screen's top. in a two-player game its place is that of the player's
 * first or second piece. a third piece leaves the place at 0, as
 * task_create's work starts it. nothing starts where task_create fails */
static void piece_icon_start(int player, int slot)
{
    struct hunt *hunt;
    struct task *task;
    struct task_work *work;
    int count;
    int i;

    hunt = hunt_current;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, PIECE_ICON_LEVEL, piece_icon_step,
                            "ExecDisplayGottenEmeraldIcon")) == NULL) {
        return;
    }
    work = task->work;
    ICON_SLOT(work) = slot;
    if (game_state.two_players == 0) {
        /* @hack spelling: a test for each slot, where PLACE_ONE_PLAYER +
         * slot would do for the three */
        if (slot == HUNT_SLOT_C) {
            ICON_PLACE(work) = PLACE_ONE_PLAYER + HUNT_SLOT_C;
        } else if (slot == HUNT_SLOT_B) {
            ICON_PLACE(work) = PLACE_ONE_PLAYER + HUNT_SLOT_B;
        } else if (slot == HUNT_SLOT_A) {
            ICON_PLACE(work) = PLACE_ONE_PLAYER + HUNT_SLOT_A;
        }
    } else {
        count = -1;
        for (i = 0; i < HUNT_PIECES; i++) {
            if (hunt->pieces[i].kind == HUNT_PIECE_TAKEN &&
                hunt->pieces[i].id == player) {
                count++;
            }
        }
        if (player == 0) {
            if (count == 0) {
                ICON_PLACE(work) = PLACE_PLAYER_0;
            } else if (count == 1) {
                ICON_PLACE(work) = PLACE_PLAYER_0 + 1;
            }
        } else {
            if (count == 0) {
                ICON_PLACE(work) = PLACE_PLAYER_1;
            } else if (count == 1) {
                ICON_PLACE(work) = PLACE_PLAYER_1 + 1;
            }
        }
    }
    if (stage_current_id() == STAGE_SECURITY_HALL) {
        work->pos.x = icon_x_sh[ICON_PLACE(work)];
    } else {
        work->pos.x = icon_x[ICON_PLACE(work)];
    }
    work->pos.y = ICON_Y;
    ICON_SIZE(work) = ICON_SIZE_START;
    ICON_ALPHA(work) = ICON_ALPHA_START;
    ICON_FLASH(work) = 1.0F;
    task->draw = piece_icon_draw;
}

void hunt_add_place(int kind, Sint8 id, const NJS_POINT3 *pos)
{
    struct hunt *hunt = hunt_current;

    if (hunt == NULL) {
        return;
    }
    if (hunt->mode == HUNT_WAIT || hunt->mode == HUNT_SET_DONE ||
        hunt->mode == HUNT_PLACE) {
        switch (kind) {
        case EMERALD_KIND_C_NORMAL:
        case EMERALD_KIND_C_HIDDEN:
            if (hunt->c_places) {
                hunt->c_places[hunt->c_count].kind = kind;
                hunt->c_places[hunt->c_count].id = id;
                hunt->c_places[hunt->c_count++].pos = *pos;
            }
            break;
        case EMERALD_KIND_B_NORMAL:
        case EMERALD_KIND_B_HIDDEN:
        case EMERALD_KIND_B_UNDERGND:
            if (hunt->b_places) {
                hunt->b_places[hunt->b_count].kind = kind;
                hunt->b_places[hunt->b_count].id = id;
                hunt->b_places[hunt->b_count++].pos = *pos;
            }
            break;
        case EMERALD_KIND_INENEMY:
            if (hunt->enemy_places) {
                hunt->enemy_places[hunt->enemy_count].kind = kind;
                hunt->enemy_places[hunt->enemy_count].id = id;
                hunt->enemy_places[hunt->enemy_count++].pos = *pos;
            }
            break;
        case EMERALD_KIND_A_UNDERGND:
            if (hunt->a_places) {
                hunt->a_places[hunt->a_count].kind = kind;
                hunt->a_places[hunt->a_count].id = id;
                hunt->a_places[hunt->a_count++].pos = *pos;
            }
            break;
        case EMERALD_KIND_A_2P_UNDGND:
            /* @hack spelling: the next test as the else of a test that
             * breaks, here and for EMERALD_KIND_A_PATHMOVE and
             * EMERALD_KIND_FINAL, where a test of its own would do */
            if (game_state.two_players == 0) {
                break;
            } else if (hunt->a_places) {
                hunt->a_places[hunt->a_count].kind = kind;
                hunt->a_places[hunt->a_count].id = id;
                hunt->a_places[hunt->a_count++].pos = *pos;
            }
            break;
        case EMERALD_KIND_A_1P_TECH:
            if (game_state.two_players != 0) {
                break;
            } else {
                /* @hack redundant: an empty else */
            }
            /* fall through */
        case EMERALD_KIND_A_PATHMOVE:
            /* @hack type: game_state's two_players read as
             * game_state_two_players, where game_state.two_players would do */
            if (STAGE_DIG_GATED(stage_current_id()) &&
                game_state_two_players == 0 &&
                !(player_records[0]->upgrades &
                  (UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS) |
                   UPGRADE_BIT(UPGRADE_ROUGE_NAILS)))) {
                break;
            } else if (hunt->a_places) {
                hunt->a_places[hunt->a_count].kind = kind;
                hunt->a_places[hunt->a_count].id = id;
                hunt->a_places[hunt->a_count++].pos = *pos;
            }
            break;
        case EMERALD_KIND_FINAL:
            if (game_state.mission != MISSION_HD) {
                break;
            } else if (id > HUNT_SLOT_A || id < HUNT_SLOT_C) {
                hunt->error_kind = kind;
                hunt->error_id = id;
                hunt->error_pos = *pos;
                break;
            }
            if (hunt->pieces[id].kind != HUNT_PIECE_EMPTY &&
                hunt->pieces[id].kind != HUNT_PIECE_TAKEN) {
                /* of the six stores below, the last three replace the first
                 * three before anything reads them */
                hunt->error_kind = hunt->pieces[id].kind;
                hunt->error_id = hunt->pieces[id].id;
                hunt->error_pos = hunt->pieces[id].pos;
                hunt->error_kind = kind;
                hunt->error_id = id;
                hunt->error_pos = *pos;
                break;
            }
            if (hunt->pieces[id].kind != HUNT_PIECE_TAKEN) {
                hunt->pieces[id].kind = kind;
                hunt->pieces[id].id = id;
                hunt->pieces[id].pos = *pos;
                hunt->placed++;
            }
            break;
        }
    }
}

#define STEPS_PER_MINUTE (60 * STEPS_PER_SECOND)
/* the most hints a piece's search boxes give */
#define HINTS_MAX 3

/* start the bonus that the hunt's time and hints earn, by
 * technique_score_start. none starts where they earn none */
static void hunt_award_bonus(struct hunt *hunt)
{
    int bonus = 0;

    if (hunt->hints == 0) {
        if (hunt->time <= (1 * STEPS_PER_MINUTE)) {
            bonus = 2000;
        } else if (hunt->time <= (5 * STEPS_PER_MINUTE)) {
            bonus = 1000;
        } else {
            bonus = 200;
        }
    } else if (hunt->hints == 1) {
        if (hunt->time <= (1 * STEPS_PER_MINUTE)) {
            bonus = 1500;
        } else if (hunt->time <= (5 * STEPS_PER_MINUTE)) {
            bonus = 800;
        } else {
            bonus = 100;
        }
    } else if (hunt->hints == 2) {
        if (hunt->time <= (3 * STEPS_PER_MINUTE)) {
            bonus = 600;
        } else if (hunt->time <= (5 * STEPS_PER_MINUTE)) {
            bonus = 500;
        } else {
            bonus = 0;
        }
    } else if (hunt->hints == HINTS_MAX) {
        if (hunt->time <= (3 * STEPS_PER_MINUTE)) {
            bonus = 400;
        } else if (hunt->time <= (5 * STEPS_PER_MINUTE)) {
            bonus = 300;
        } else {
            bonus = 0;
        }
    }
    if (bonus) {
        technique_score_start(bonus);
    }
}

/* the pieces a sign shows, as player_hooks.show_piece_sign numbers them.
 * they are EMERALD_PIECE_ of shared/emerald.h, a header the main executable
 * can't include */
#define SIGN_PIECE_B 0
#define SIGN_PIECE_A 1
#define SIGN_PIECE_C 2
/* the sound of a piece taken */
#define SOUND_PIECE_TAKEN 0x1008
/* the voices of a piece taken, by look. in a one-player game they are the
 * first piece's and the second's. in a two-player game, VOICE_AHEAD_ is the
 * taker's first piece while the other player holds none. VOICE_LEVEL_ is its
 * first while the other holds one */
#define VOICE_FIRST_PIECE_KNUCKLES 0x4f2
#define VOICE_FIRST_PIECE_ROUGE 0x4f3
#define VOICE_SECOND_PIECE_KNUCKLES 0x4f4
#define VOICE_SECOND_PIECE_ROUGE 0x4f5
#define VOICE_AHEAD_KNUCKLES 0x4ee
#define VOICE_AHEAD_ROUGE 0x4ef
#define VOICE_LEVEL_KNUCKLES 0x4f0
#define VOICE_LEVEL_ROUGE 0x4f1

void hunt_take_piece(int kind, Sint8 id, int player, NJS_POINT3 *pos)
{
    struct hunt *hunt = hunt_current;
    NJS_POINT3 start_pos;
    struct angles start_ang;
    struct task *(*create_piece)(NJS_POINT3 *pos, Uint8 kind, Uint8 id);
    int look;
    int voice;
    int mine;
    int others;
    int i;
    int j;

    if (hunt == NULL) {
        return;
    }
    for (i = 0; i < HUNT_PIECES; i++) {
        if (hunt->pieces[i].kind == kind && hunt->pieces[i].id == id) {
            if (player == 0 || player == 1) {
                hunt->placed--;
                hunt->pieces[i].kind = HUNT_PIECE_TAKEN;
                hunt->pieces[i].id = player;
                if (player_hooks.show_piece_sign) {
                    if (stage_current_id() == STAGE_DEATH_CHAMBER ||
                        stage_current_id() == STAGE_EGG_QUARTERS ||
                        stage_current_id() == STAGE_SECURITY_HALL) {
                        switch (i) {
                        case HUNT_SLOT_C:
                            player_hooks.show_piece_sign(player, SIGN_PIECE_C);
                            break;
                        case HUNT_SLOT_B:
                            player_hooks.show_piece_sign(player, SIGN_PIECE_B);
                            break;
                        /* @hack spelling: the last case without a break,
                         * where one would do */
                        case HUNT_SLOT_A:
                            player_hooks.show_piece_sign(player, SIGN_PIECE_A);
                        }
                    } else {
                        player_hooks.show_piece_sign(player, SIGN_PIECE_B);
                    }
                }
                piece_icon_start(player, i);
                sound_play(SOUND_PIECE_TAKEN, NULL, 0, 0);
                look = player_records[player]->kind;
                voice = -1;
                mine = 0;
                others = 0;
                for (j = 0; j < HUNT_PIECES; j++) {
                    if (hunt->pieces[j].kind == HUNT_PIECE_TAKEN) {
                        if (hunt->pieces[j].id == player) {
                            mine++;
                        } else {
                            others++;
                        }
                    }
                }
                if (game_state.two_players == 0) {
                    switch (mine) {
                    case 1:
                        if (look == PLAYER_KNUCKLES) {
                            voice = VOICE_FIRST_PIECE_KNUCKLES;
                        } else if (look == PLAYER_ROUGE) {
                            voice = VOICE_FIRST_PIECE_ROUGE;
                        }
                        break;
                    /* @hack spelling: the last case without a break, where
                     * one would do */
                    case 2:
                        if (look == PLAYER_KNUCKLES) {
                            voice = VOICE_SECOND_PIECE_KNUCKLES;
                        } else if (look == PLAYER_ROUGE) {
                            voice = VOICE_SECOND_PIECE_ROUGE;
                        }
                    }
                } else if (mine == 1 && others == 0) {
                    if (look == PLAYER_KNUCKLES) {
                        voice = VOICE_AHEAD_KNUCKLES;
                    } else if (look == PLAYER_ROUGE) {
                        voice = VOICE_AHEAD_ROUGE;
                    }
                } else if (mine == 1 && others == 1) {
                    if (look == PLAYER_KNUCKLES) {
                        voice = VOICE_LEVEL_KNUCKLES;
                    } else if (look == PLAYER_ROUGE) {
                        voice = VOICE_LEVEL_ROUGE;
                    }
                }
                if (voice != -1) {
                    voice_play_priority_2(voice);
                }
                score_add(HUNT_PIECE_SCORE);
                hunt_award_bonus(hunt);
                hunt->time = 0;
                if (i == HUNT_SLOT_C ||
                    (i == HUNT_SLOT_B &&
                     hunt->pieces[HUNT_SLOT_C].kind == HUNT_PIECE_TAKEN) ||
                    (i == HUNT_SLOT_A && hunt->placed == 0)) {
                    hunt->hints = 0;
                }
                hunt->voices = 0;
                if (game_state.mission != MISSION_TIMED) {
                    /* @hack spelling: the call in an empty arm's test, where
                     * a test of the call alone would do */
                    if (!stage_find_start(player, &start_pos, &start_ang)) {
                    } else {
                        restart_point_keep(player, &start_pos, &start_ang, 0);
                    }
                }
            } else {
                /* @hack spelling: create_piece, a local for the hook, where
                 * its two reads would do */
                create_piece = player_hooks.create_piece;
                if (create_piece) {
                    if (pos == NULL) {
                        pos = &hunt->pieces[i].pos;
                    }
                    create_piece(pos, kind, id);
                }
            }
            return;
        }
    }
    /* @hack redundant: a read in a block that never runs */
    if (0) {
        if (kind) {
        }
    }
}

int hunt_find_piece(int kind, Sint8 id, const NJS_POINT3 *pos)
{
    struct hunt *hunt;
    int i;

    hunt = hunt_current;
    if (hunt == NULL) {
        return HUNT_NONE;
    }
    for (i = 0; i < HUNT_PIECES; i++) {
        if (hunt->pieces[i].kind == kind && hunt->pieces[i].id == id) {
            if (pos) {
                hunt->pieces[i].pos = *pos;
            }
            return i + 1;
        }
    }
    return 0;
}

/* the stage's table of hints, as hunt_set_hints takes it */
static struct hunt_hint *hunt_hints = NULL;
/* a word that fun_8c087448 and fun_8c08744e write, and nothing reads */
static Sint32 dat_8c0ee7b0 = 0;
static Sint32 *hunt_hint_file = NULL;
/* the suffixes of the hint files' names, by text language */
static char *hint_suffixes[TEXT_LANGUAGES] = {"j", "e", "f", "s", "g"};

void hunt_set_hints(struct hunt_hint *hints)
{
    hunt_hints = hints;
}

void hunt_clear_hints(void)
{
    hunt_hints = NULL;
}

/* SHC leaves out a static that nothing calls, so these three are external. no
 * other source calls them, and no header declares them. SHC writes
 * hunt_set_hint_file's body into hunt_load_hint_file */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void fun_8c087448(Sint32 value)
{
    dat_8c0ee7b0 = value;
}

void fun_8c08744e(void)
{
    dat_8c0ee7b0 = 0;
}

void hunt_set_hint_file(Sint32 *file)
{
    hunt_hint_file = file;
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the room for a hint file's path: its name, the language's suffix, .prs
 * and the NUL */
#define HINT_PATH_SIZE 16

Sint32 *hunt_load_hint_file(const char *name)
{
    char path[HINT_PATH_SIZE];
    Sint32 *file;

    strcpy(path, name);
    strcat(path, hint_suffixes[game_state.text_language]);
    strcat(path, ".prs");
    file = loader_read_prs_alloc(path);
    if (file == NULL) {
        return NULL;
    }
    hunt_set_hint_file(file);
    return file;
}

char *hunt_next_hint(void)
{
    struct hunt_hint *hint;
    char *message;
    int kind;
    int id;
    int count;
    int i;

    if (hunt_current == NULL) {
        return NULL;
    }
    if (hunt_hints == NULL) {
        return NULL;
    }
    if (hunt_hint_file == NULL) {
        return NULL;
    }
    hint = hunt_hints;
    for (i = 0; i < HUNT_PIECES; i++) {
        kind = hunt_current->pieces[i].kind;
        id = hunt_current->pieces[i].id;
        /* @hack spelling: a test that continues before the break, where a
         * test that breaks would do */
        if (kind == HUNT_PIECE_TAKEN) {
            continue;
        }
        break;
    }
#ifdef NONMATCHING
    /* this side keeps the tag's result, as hunt.h states it: (char *)3 */
    message = (char *)3;
#endif
    /* @bug uninitialized: a table that lists no hint for the piece leaves
     * message unset, and the call returns the 3 that R6 holds for the count's
     * test (MOV #3,R6 at 0x8c0874f4, MOV R6,R0 at 0x8c08758e) */
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    while (hint->kind != -1) {
        if (hint->kind == kind && hint->id == id) {
            count = hunt_current->hints;
            if (count >= HINTS_MAX) {
                count = HINTS_MAX;
            } else {
                count++;
            }
            hunt_current->hints = count;
            if (count > 0) {
                message = (char *)hunt_hint_file +
                          hunt_hint_file[hint->messages[count - 1]];
            } else {
                message = NULL;
            }
            break;
        }
        hint++;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    return message;
}
