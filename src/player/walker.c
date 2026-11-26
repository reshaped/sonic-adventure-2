/* the walkers, the mechs Eggman and Tails drive. in a two-player game the dark
 * Chao walker can stand in for Eggman's. the Chao walker can stand in for
 * Tails's. the prefixes ewalk_, twalk_, bwalk_ and cwalk_ name the four, as
 * their files do. EWALKMDL.PRS holds the models of Eggman's walker.
 *
 * one task, named EggWalker, steps and draws a walker. the action button fires
 * its cannon. held after a shot, the button aims the gun's lock-on, which locks
 * one target at a time. a walker strikes what stands close ahead. in a
 * two-player game, and on STAGE_VERSUS_MECH_2, each of its ring levels gives it
 * a special move */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/color.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "effect/powder.h"
#include "effect/stars.h"
#include "enemy/enemy.h"
#include "event/mini_event.h"
#include "hud/action_window.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/action.h"
#include "player/player.h"
#include "player/walker.h"
#include "player/walker_lock_assets.h"
#include "player/walker_weapons.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the walkers' slots of model_table. the EWALK_ slots hold the models of
 * Eggman's walker or of the dark Chao walker. the TWALK_ slots hold those of
 * Tails's walker or of the Chao walker. each _BODY is the model its animations
 * pose */
#define EWALK_BODY 248
/* nodes of the body. a motion callback keeps some of their places in the
 * record, turned into the world. it keeps _UPPER's in unk_230, and _COCKPIT's
 * in unk_218. it keeps the feet's in unk_200 and unk_20c, and the aim's start,
 * _AIM's, in unk_254. it keeps an _AT_ node's in the field its suffix names. it
 * keeps the matrices of the nodes a draw adds parts at too */
#define EWALK_UPPER 249
#define EWALK_LEANS 250
#define EWALK_COCKPIT 251
#define EWALK_FOOT_0 253
#define EWALK_FOOT_1 254
#define EWALK_TURRET 256
/* the cannon draws at the matrix of EWALK_CANNON_MOUNT_A or
 * EWALK_CANNON_MOUNT_B, whichever the motion reaches last */
#define EWALK_CANNON_MOUNT_A 261
#define EWALK_CANNON_MOUNT_B 264
#define EWALK_AT_1E8 269
#define EWALK_AIM 271
#define EWALK_NOZZLE0 273
#define EWALK_NOZZLE1 275
/* the shadows, and the parts a draw adds */
#define EWALK_COCKPIT_SHADOW 252
#define EWALK_FOOT_SHADOW 255
/* the gun's muzzle is whichever of _GUN_MUZZLE and _LAMP_MUZZLE the drawn gun's
 * chain of nodes holds. a draw keeps the muzzle's place in unk_23c. the lamp,
 * the sibling of _LAMP_MUZZLE, draws with an alpha that follows the walker's
 * pulse */
#define EWALK_GUN_MUZZLE 257
#define EWALK_GUN 258
#define EWALK_LAMP_MUZZLE 259
#define EWALK_POWERGUN 260
#define EWALK_BAZOOKA_MUZZLE 262
#define EWALK_BAZOOKA 263
#define EWALK_CANNON_MUZZLE 265
#define EWALK_CANNON 266
#define EWALK_STRIKE_PART 268
#define EWALK_JET0 274
#define EWALK_JET1 276
#define EWALK_SHIELD 278
/* the bazooka's two flashes. each is two models. the draw takes the first on
 * the game's odd steps, and the second on its even ones */
#define EWALK_FLASH_A 279
#define EWALK_FLASH_B 281
#define EWALK_FLAME 283 /* its child is a hover's flame at each nozzle */
/* the parts the Eggman walker's draw adds on STAGE_LOST_COLONY alone, without
 * fog */
#define EWALK_COLONY_A 284
#define EWALK_COLONY_B 285
#define EWALK_COLONY_C 286
#define EWALK_LASER 287 /* the laser cannon */
#define EWALK_GLOW 291 /* the glow of the second special move */
#define TWALK_BODY 293
/* nodes of the body, kept as the EWALK_ nodes are. the motion callback keeps
 * the cannon's muzzle in unk_248 */
#define TWALK_UPPER 294
#define TWALK_LEANS 295
#define TWALK_COCKPIT 296
#define TWALK_FOOT_0 298
#define TWALK_FOOT_1 299
#define TWALK_TURRET 302
#define TWALK_CANNON_MOUNT 310
#define TWALK_CANNON_MUZZLE 311
#define TWALK_AT_1E8 314
#define TWALK_AIM 316
#define TWALK_NOZZLE0 318
#define TWALK_NOZZLE1 320
/* the shadows, and the parts a draw adds */
#define TWALK_COCKPIT_SHADOW 297
/* the shadows at the feet, at unk_200 and at unk_20c */
#define TWALK_FOOT_SHADOW_A 300
#define TWALK_FOOT_SHADOW_B 301
#define TWALK_GUN_MUZZLE 303
#define TWALK_GUN 304
#define TWALK_LAMP_MUZZLE 305
#define TWALK_BLASTER 306
#define TWALK_HYPERCANNON 309
#define TWALK_CANNON 312
#define TWALK_STRIKE_PART 313
/* its child and the child's sibling are a hover's flames */
#define TWALK_FLAME 317
#define TWALK_BOOSTER0 319
#define TWALK_BOOSTER1 321
#define TWALK_GLOW 325

/* entries of the walkers' tables of animations that player/player.h doesn't
 * name */
#define ANIM_MISS_LANDED 60
#define ANIM_EGGMAN_JUMP_FAST 121
#define ANIM_EGGMAN_JUMP_FAST_RISE 122
#define ANIM_EGGMAN_HOVER 127
#define ANIM_EGGMAN_STRIKE 128
/* the first special move fires in ANIM_SPECIAL_1. ANIM_SPECIAL plays as the
 * first waits to fire, and as the second fires */
#define ANIM_SPECIAL_1 131
#define ANIM_SPECIAL 132
#define ANIM_LASER 133
#define ANIM_EGGMAN_POSE 135
#define ANIM_TAILS_JUMP_FAST 173
#define ANIM_TAILS_JUMP_FAST_RISE 174
#define ANIM_TAILS_HOVER 179
#define ANIM_TAILS_STRIKE 180
#define ANIM_TAILS_POSE 186
/* the entries whose motions pose a strike's part. EWALK_STRIKE_PART is the part
 * of Eggman's walker, posed at the strike's frame. TWALK_STRIKE_PART is
 * Tails's, posed at TWALK_STRIKE_PART_RATE times that frame */
#define ANIM_EGGMAN_STRIKE_PART 129
#define ANIM_TAILS_STRIKE_PART 181
#define TWALK_STRIKE_PART_RATE 2.0F
/* the motion of EWALK_LASER in motion_table, which the draws pose at the
 * walker's laser_frame */
#define MOTION_EGGMAN_LASER 130

/* a walker's collision shapes: its body, a middle sphere, a top sphere, its
 * strike and a guard. the top lies at the body's center_height, and the guard
 * stays off. the table holds a sixth, SHAPE_LEFT_OUT, which collision_init
 * leaves out */
#define WALKER_SHAPES 5
#define SHAPE_BODY 0
#define SHAPE_MIDDLE 1
#define SHAPE_TOP 2
#define SHAPE_STRIKE 3
#define SHAPE_LEFT_OUT 5
/* a target whose first shape has SHAPE_ID_NO_LOCKON takes no lock */
#define SHAPE_ID_NO_LOCKON 11
/* the radius walker_set_shapes gives the collision record */
#define WALKER_RADIUS 50.0F

/* the most locks a walker holds */
#define WALKER_LOCKS 32
/* the steps between two shots of the cannon. a shot gives the action button
 * WALKER_LOCK_STEPS held steps in which it aims the lock-on */
#define WALKER_SHOT_STEPS 20
#define WALKER_LOCK_STEPS 360
#define WALKER_LOCK_WAIT 10 /* the aiming steps between two locks */
#define WALKER_RELEASE_STEPS 8 /* the steps between two released shots */
/* the jet_power at which a jet of Tails's walker blows at its full strength. a
 * jet's strength is jet_power over WALKER_JET_MAX. a step brings a jet_power
 * above WALKER_JET_MAX + 1 back to WALKER_JET_MAX. so a jet_power of
 * WALKER_JET_MAX + 1 blows a little above full strength */
#define WALKER_JET_MAX 95
/* PLAYER_MODE_MECH_LASER ends on the step that finds mode_count past
 * WALKER_LASER_STEPS */
#define WALKER_LASER_STEPS 160
/* a hover_floor lower than any floor, from which no hover lifts. a hover that
 * begins from a fall or a hit takes it. so does a lift on STAGE_COSMIC_WALL
 * that rises more than WALKER_HOVER_RISE above its floor. the walker then
 * lifts no more until a hover from a jump sets a floor again */
#define WALKER_NO_HOVER_FLOOR (-1000000.0F)
/* how far above its hover_floor a hover on STAGE_COSMIC_WALL lifts the walker
 */
#define WALKER_HOVER_RISE 120.0F

/* the sounds a walker plays. a jump and a strike play a cry besides, at the
 * walker's pos. Eggman's and Tails's own walkers play the cries, and the Chao
 * walkers don't. SOUND_POWER_SHOT is the first special move's volley too */
#define SOUND_COSMIC_HOVER 9
#define SOUND_JUMP_TAILS 0x2000
#define SOUND_JUMP_EGGMAN 0x2001
#define SOUND_BRAKE 0x2002
#define SOUND_STEP 0x2007
#define SOUND_HOVER 0x2009
#define SOUND_STRIKE_EGGMAN 0x200a
#define SOUND_LASER 0x200d
#define SOUND_AIM 0x200e
#define SOUND_LOCK 0x200f
#define SOUND_SHOT 0x2010
#define SOUND_POWER_SHOT 0x2011
#define SOUND_CANNON 0x2012
#define SOUND_BAZOOKA 0x2013
#define SOUND_STRIKE_TAILS 0x2016
#define SOUND_MISSILES 0x2017
#define SOUND_LASER_START 0x2018
#define SOUND_CRY_JUMP_TAILS 0x3000
#define SOUND_CRY_JUMP_FAST_TAILS 0x3001
#define SOUND_CRY_STRIKE_TAILS 0x3004
#define SOUND_CRY_JUMP_EGGMAN 0x300a
#define SOUND_CRY_JUMP_FAST_EGGMAN 0x300b
#define SOUND_CRY_STRIKE_EGGMAN 0x300e
/* the voices a walker plays, which the Chao walkers don't. on
 * STAGE_VERSUS_MECH_2 the second special move plays its VOICE_VERSUS_ voice
 * at VERSUS_VOICE_PRIORITY */
#define VOICE_SPECIAL_1_EGGMAN 0x662
#define VOICE_SPECIAL_1_TAILS 0x663
#define VOICE_SPECIAL_2_EGGMAN 0x664
#define VOICE_SPECIAL_2_TAILS 0x665
#define VOICE_LASER_EGGMAN 0x666
#define VOICE_LASER_TAILS 0x667
#define VOICE_MISS_EGGMAN 0x68c
#define VOICE_MISS_TAILS 0x68d
#define VOICE_VERSUS_SPECIAL_2_EGGMAN 0x824
#define VOICE_VERSUS_SPECIAL_2_TAILS 0x8e3
#define VERSUS_VOICE_PRIORITY 3

/* a lock, a lock-on task's extra. a walker's locks form a list through prev
 * and next, from its by_character.mech.locks. letting the action button go
 * releases the list to a score task, named WalkerTimeSubtractingTask. the
 * score task fires the locks' shots one at a time, and scores their hits. a
 * lock's task goes on as its shot, and the shot's as a blast */
struct lock {
    struct task *prev;
    struct task *next;
    Sint8 slot; /* the walker's slot */
    /* 1 where the release found the target live, as lock_target_live says */
    Sint8 counted;
    Sint8 released; /* 1 once a release handed the lock to a score task */
    Sint8 fired; /* 1 once the score task let the lock's shot fly */
    Sint16 hits; /* the hits its shot and its blast made */
    /* the marker's steps of growth, up to LOCK_GROWN. the marker's scale is
     * walker_lock_scales[grow] */
    Sint16 grow;
    NJS_POINT3 pos; /* the target's center */
    struct task *target;
};
#define LOCK_GROWN 7

/* a score task's extra */
struct lock_score {
    struct task *locks; /* the first of the released locks */
    Sint8 slot;
    Uint8 unk_05;
    /* the score, in units of SCORE_STEP_POINTS, as SCORE_STEP_UP raises it. -1
     * is no score */
    Sint16 score;
    Sint16 unk_08;
    Sint16 steps; /* the steps since the last lock ended */
    Float unk_0c;
};

/* the extra of a special move's task, and of the laser's */
struct special {
    Sint8 slot;
    Sint8 other; /* the slot of the other player, whom the move aims at */
    Sint16 steps; /* the steps before the move fires */
};
#define SPECIAL_STEPS 60
#define LASER_STEPS 80
/* the task_create levels of a walker's special move or laser, SPECIAL_LEVEL,
 * and of its score tasks, SCORE_LEVEL */
#define SPECIAL_LEVEL 3
#define SCORE_LEVEL 5
/* PLAY_PRIORITY is the priority of the sounds a walker plays for a time or
 * until a stop. the rest are volumes and durations */
#define PLAY_PRIORITY 1
#define AIM_DURATION 3
#define COSMIC_HOVER_VOLUME 20
#define COSMIC_HOVER_DURATION 4
#define POWER_SHOT_DURATION 120

/* the walkers' animations, the record's anim.actions. Eggman's table is the
 * dark Chao walker's too, and Tails's the Chao walker's. a draw indexes a table
 * twice for a motion: the entry's motion m, then entry m's motion. every motion
 * m of either table is also entry m's, so the second index changes nothing */
static struct player_action ewalk_anims[139] = {
#include "player/walker/ewalk_anims.inc"
};
static struct player_action twalk_anims[190] = {
#include "player/walker/twalk_anims.inc"
};

/* the walkers' shapes, whose centers are world positions. walker_place_shapes
 * moves the body's and the top's with the walker each step. walker_set_shapes
 * moves the middle's and the strike's. Those spheres and the top push and
 * resist every group but pickups. The collision record uses the first five of
 * the six stored shapes. */
static struct collision_shape walker_shapes[6] = {
#include "player/walker/walker_shapes.inc"
};

/* each draw's flicker table, by which a hover's flame flickers. at the game's
 * step n, the draw scales the flame across by entry n + FLICKER_LEAD. it scales
 * it along by entry n. both indexes wrap round the table's FLICKER_STEPS
 * entries. the dark Chao walker's table is the Eggman walker's, and the Chao
 * walker's the Tails walker's */
#define FLICKER_STEPS 8
#define FLICKER_LEAD 3
static Float ewalk_flicker[FLICKER_STEPS] = {1.0F,  1.38F, 0.95F, 1.25F,
                                             1.05F, 0.85F, 1.15F, 0.7F};
/* each draw's missile points and missile way. the missiles of the second
 * special move leave from the points, and set off along the way. both lie in
 * the axes of the cockpit's node. a draw turns them into the world while the
 * walker's glow is above 0. the two Chao walkers' are the Eggman walker's
 */
static NJS_POINT3 ewalk_missile_points[PLAYER_MISSILE_POINTS] = {
#include "player/walker/ewalk_missile_points.inc"
};
static NJS_VECTOR ewalk_missile_dir = {
#include "player/walker/ewalk_missile_dir.inc"
};
static Float cwalk_flicker[FLICKER_STEPS] = {1.0F,  1.25F, 0.95F, 1.35F,
                                             1.05F, 0.9F,  0.85F, 1.1F};
static NJS_POINT3 cwalk_missile_points[PLAYER_MISSILE_POINTS] = {
#include "player/walker/cwalk_missile_points.inc"
};
static NJS_VECTOR cwalk_missile_dir = {
#include "player/walker/cwalk_missile_dir.inc"
};
static Float bwalk_flicker[FLICKER_STEPS] = {1.0F,  1.38F, 0.95F, 1.25F,
                                             1.05F, 0.85F, 1.15F, 0.7F};
static NJS_POINT3 bwalk_missile_points[PLAYER_MISSILE_POINTS] = {
#include "player/walker/bwalk_missile_points.inc"
};
static NJS_VECTOR bwalk_missile_dir = {
#include "player/walker/bwalk_missile_dir.inc"
};
static Float twalk_flicker[FLICKER_STEPS] = {1.0F,  1.25F, 0.95F, 1.35F,
                                             1.05F, 0.9F,  0.85F, 1.1F};
static NJS_POINT3 twalk_missile_points[PLAYER_MISSILE_POINTS] = {
#include "player/walker/twalk_missile_points.inc"
};
static NJS_VECTOR twalk_missile_dir = {
#include "player/walker/twalk_missile_dir.inc"
};

/* the record a draw is drawing, which its motion callback reads */
static struct player *walker_drawn;
static Sint32 unk_walker_bss; /* NOLINT(clang-diagnostic-unused-variable) */
/* the view's inverse as a draw starts. it takes a node's place and axes from
 * the view into the world */
static NJS_MATRIX view_to_world;
static NJS_MATRIX unk_matrix; /* NOLINT(clang-diagnostic-unused-variable) */
/* the matrices of the nodes a draw adds parts at, as its motion callback keeps
 * them. they are the turret's, the cannon mount's, the upper body's, the two
 * nozzles' and the cockpit's */
static NJS_MATRIX turret_matrix;
static NJS_MATRIX cannon_matrix;
static NJS_MATRIX upper_matrix;
static NJS_MATRIX nozzle0_matrix;
static NJS_MATRIX nozzle1_matrix;
static NJS_MATRIX cockpit_matrix;

static void walker_create_cwalk(int slot);
static void walker_create_bwalk(int slot);
static void walker_step(struct task *task);
static void walker_choose_mode(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, struct player *walker);
static void walker_destroy(struct task *task);
static void walker_place_shapes(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *walker);
static void ewalk_motion_callback(NJS_CNK_OBJECT *object);
static void ewalk_draw(struct task *task);
static void ewalk_draw_shadows(struct task_work *work, struct player *record,
                               int anim);
static void cwalk_motion_callback(NJS_CNK_OBJECT *object);
static void cwalk_draw(struct task *task);
static void cwalk_draw_shadows(struct task_work *work, struct player *record,
                               int anim);
static void bwalk_draw(struct task *task);
static void twalk_motion_callback(NJS_CNK_OBJECT *object);
static void twalk_draw(struct task *task);
static void twalk_draw_shadows(struct task_work *work, struct player *record,
                               int anim);
static void walker_free_move(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
static void walker_jump(struct task_work *work, struct player *record);
static void walker_jump_fast(struct task_work *work, struct player *record);
static Bool walker_test_jump(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record);
static Bool walker_take_request(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *walker);
static Bool walker_take_action(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, struct player *walker);
static Bool walker_offer_special(struct task_work *work, struct player *record);
static void special_step(struct task *task);
static void special_1_fire(struct task *task);
static void walker_special_1(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *walker);
static void special_2_fire(struct task *task);
static void walker_special_2(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *walker);
static void laser_wait_step(struct task *task);
static void laser_fire(struct task *task);
static void walker_laser(struct task_work *work, struct player_motion *motion,
                         struct player *record, struct player *walker);
static Bool walker_offer_release(struct task_work *work, struct player *record,
                                 struct player *walker);
static Bool walker_target_near(struct task_work *work, struct player *record);
static Bool walker_offer_strike(struct task_work *work, struct player *record);
static void walker_strike_eggman(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record, struct player *walker);
static void walker_strike_tails(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *walker);
static void walker_set_shapes(struct task *task, struct task_work *work,
                              struct player *record);
static void walker_run_anim(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);
static void lock_score_destroy(struct task *task);
static void lock_score_collect(struct lock_score *score);
static void lock_score_unlink(struct lock_score *score);
static void lock_score_fire(struct lock_score *score);
static void lock_score_step(struct task *task);
static void lock_score_start(int slot, int count, struct task *locks);
static void lock_list_add(struct task *task, struct player *walker);
static void lock_list_remove(struct task *task, struct player *walker);
static void lock_blast_draw_part(Float size, Float fade, int part,
                                 int character);
static void lock_blast_draw(struct task *task);
static void lock_shot_draw(struct task *task);
static void lock_blast_step(struct task *task);
static void lock_shot_step(struct task *task);
static void lock_destroy(struct task *task);
static void lock_released_destroy(struct task *task);
static void lock_draw(struct task *task);
static void lock_step(struct task *task);
static Bool lock_target_live(struct task *task);
static Float lockon_target_distance(struct collision_work *collision,
                                    struct player *walker);
static Float lockon_box_distance(struct collision_work *collision,
                                 struct player *walker);
static Float lockon_player_distance(struct collision_work *collision,
                                    struct player *walker);
static struct task *lockon_find_nearest(struct player *walker,
                                        struct task **list, int count,
                                        Float *distance);
static struct task *lockon_find_player(struct player *walker, struct task *task,
                                       Float *distance);
static void walker_start_lockon(struct player *record, struct player *walker);

/* a walker's functions hold its record twice. through walker they read
 * by_character.mech, and through record, RECORD_OF(walker), the rest. a call
 * that takes both takes the one record twice */
/* @hack spelling: the record held twice, the second time through the address
 * of its first field, where walker alone would do */
#define RECORD_OF(walker) ((struct player *)&(walker)->slot)

void walker_create_eggman(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *walker;
    struct player *record;

    if (game_state.two_players != 0 && game_state.alt_character[slot] != 0) {
        walker_create_bwalk(slot);
        return;
    }
    /* @hack spelling: task and walker each set inside its test, where
     * statements of their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            walker_step, "EggWalker")) == NULL) {
        return;
    }
    if ((walker = syCalloc(1, MECH_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = walker;
    work = task->work;
    record = RECORD_OF(walker);
    record->slot = slot;
    record->character = PLAYER_EGGMAN_MECH;
    work->mode = work->submode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = ewalk_anims;
    record->anim.next = ANIM_STAND;
    collision_init(task, walker_shapes, WALKER_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in walker_place_shapes: the create empties
     * the slot and destroys the task where collision_init leaves the work no
     * collision record, as sonic_create's side does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    if (game_state.two_players == 0 || game_state.costume[slot] == 0) {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("EWALKTEX");
        walker->by_character.mech.models = model_file_read("EWALKMDL.PRS");
    } else if (game_state.costume[slot] == COSTUME_PSO) {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("EWALK1TEX");
        walker->by_character.mech.models = model_file_read("EWALK1MDL.PRS");
        record->costume = COSTUME_PSO;
    } else if (game_state.costume[slot] == COSTUME_H) {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("HEWALKTEX");
        walker->by_character.mech.models = model_file_read("HEWALKMDL.PRS");
        record->costume = COSTUME_H;
    } else {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("XEWALKTEX");
        walker->by_character.mech.models = model_file_read("XEWALKMDL.PRS");
        record->costume = COSTUME_X;
    }
    life_icon_load(record->character);
    walker->by_character.mech.effects_texlist =
        texture_load_pvm_alloc("WALKTEX");
    walker->by_character.mech.motions = motion_file_read("EWALKMTN.PRS");
    player_anim_step(&record->anim);
    action_window_start(record->slot);
    task->destructor = walker_destroy;
    task->draw = ewalk_draw;
    /* @hack spelling: two tests that store the same upgrades, where one
     * test joined by || would do */
    if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE);
    } else if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_2 && slot == 1) {
        record->upgrades = UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE) |
                           UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_1 && slot == 1) {
        record->upgrades = 0;
    }
    walker_fill_gauge(record);
}

void walker_create_tails(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *walker;
    struct player *record;

    if (game_state.two_players != 0 && game_state.alt_character[slot] != 0) {
        walker_create_cwalk(slot);
        return;
    }
    /* @hack spelling: task and walker each set inside its test, where
     * statements of their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            walker_step, "EggWalker")) == NULL) {
        return;
    }
    if ((walker = syCalloc(1, MECH_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = walker;
    work = task->work;
    record = RECORD_OF(walker);
    record->slot = slot;
    record->character = PLAYER_TAILS_MECH;
    work->mode = work->submode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = twalk_anims;
    record->anim.next = ANIM_STAND;
    collision_init(task, walker_shapes, WALKER_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in walker_place_shapes, as
     * walker_create_eggman's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    if (game_state.two_players == 0 || game_state.costume[slot] == 0) {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("TWALKTEX");
        walker->by_character.mech.models = model_file_read("TWALKMDL.PRS");
    } else if (game_state.costume[slot] == COSTUME_PSO) {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("TWALK1TEX");
        walker->by_character.mech.models = model_file_read("TWALK1MDL.PRS");
        record->costume = COSTUME_PSO;
    } else if (game_state.costume[slot] == COSTUME_H) {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("HTWALKTEX");
        walker->by_character.mech.models = model_file_read("HTWALKMDL.PRS");
        record->costume = COSTUME_H;
    } else {
        walker->by_character.mech.texlist = texture_load_pvm_alloc("XTWALKTEX");
        walker->by_character.mech.models = model_file_read("XTWALKMDL.PRS");
        record->costume = COSTUME_X;
    }
    life_icon_load(record->character);
    walker->by_character.mech.effects_texlist =
        texture_load_pvm_alloc("WALKTEX");
    walker->by_character.mech.motions = motion_file_read("TWALKMTN.PRS");
    player_anim_step(&record->anim);
    action_window_start(record->slot);
    task->destructor = walker_destroy;
    task->draw = twalk_draw;
    /* @hack spelling: two tests that store the same upgrades, where one
     * test joined by || would do */
    if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER);
    } else if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_2 && slot == 1) {
        record->upgrades = UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER) |
                           UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_1 && slot == 1) {
        record->upgrades = 0;
    }
    walker_fill_gauge(record);
}

/* the Chao walker, in the place of Tails's */
static void walker_create_cwalk(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *walker;
    struct player *record;

    /* @hack spelling: task and walker each set inside its test, where
     * statements of their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            walker_step, "EggWalker")) == NULL) {
        return;
    }
    if ((walker = syCalloc(1, MECH_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = walker;
    work = task->work;
    record = RECORD_OF(walker);
    record->slot = slot;
    record->character = PLAYER_TAILS_MECH;
    /* @hack spelling: submode and mode cleared in two statements, where one
     * chained store would do */
    work->submode = PLAYER_MODE_STAND;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = twalk_anims;
    record->anim.next = ANIM_STAND;
    record->kind = PLAYER_KIND_CWALK;
    collision_init(task, walker_shapes, WALKER_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in walker_place_shapes, as
     * walker_create_eggman's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    walker->by_character.mech.texlist = texture_load_pvm_alloc("CWALKTEX");
    life_icon_load(record->kind);
    walker->by_character.mech.effects_texlist =
        texture_load_pvm_alloc("WALKTEX");
    walker->by_character.mech.models = model_file_read("CWALKMDL.PRS");
    walker->by_character.mech.motions = motion_file_read("CWALKMTN.PRS");
    player_anim_step(&record->anim);
    action_window_start(record->slot);
    task->destructor = walker_destroy;
    task->draw = cwalk_draw;
    /* @hack spelling: two tests that store the same upgrades, where one
     * test joined by || would do */
    if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER);
    } else if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_2 && slot == 1) {
        record->upgrades = UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER) |
                           UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_1 && slot == 1) {
        record->upgrades = 0;
    }
    walker_fill_gauge(record);
}

/* the dark Chao walker, in the place of Eggman's */
static void walker_create_bwalk(int slot)
{
    struct task *task;
    struct task_work *work;
    struct player *walker;
    struct player *record;

    /* @hack spelling: task and walker each set inside its test, where
     * statements of their own would do */
    if ((task = task_create(TASK_MOTION | TASK_WORK | TASK_FORCE, PLAYER_LEVEL,
                            walker_step, "EggWalker")) == NULL) {
        return;
    }
    if ((walker = syCalloc(1, MECH_RECORD_SIZE)) == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = walker;
    work = task->work;
    record = RECORD_OF(walker);
    record->slot = slot;
    record->character = PLAYER_EGGMAN_MECH;
    /* @hack spelling: submode and mode cleared in two statements, where one
     * chained store would do */
    work->submode = PLAYER_MODE_STAND;
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_register(slot, task);
    record->anim.actions = ewalk_anims;
    record->anim.next = ANIM_STAND;
    record->kind = PLAYER_KIND_BWALK;
    collision_init(task, walker_shapes, WALKER_SHAPES, COLLISION_PLAYER);
#ifdef NONMATCHING
    /* this side repairs the tag in walker_place_shapes, as
     * walker_create_eggman's does */
    if (work->collision == NULL) {
        player_unregister(slot);
        task_destroy(task);
        return;
    }
#endif
    work->collision->shapes->radius = record->consts.radius;
    walker->by_character.mech.texlist = texture_load_pvm_alloc("BWALKTEX");
    life_icon_load(record->kind);
    walker->by_character.mech.effects_texlist =
        texture_load_pvm_alloc("WALKTEX");
    walker->by_character.mech.models = model_file_read("BWALKMDL.PRS");
    walker->by_character.mech.motions = motion_file_read("EWALKMTN.PRS");
    player_anim_step(&record->anim);
    action_window_start(record->slot);
    task->destructor = walker_destroy;
    task->draw = bwalk_draw;
    /* @hack spelling: two tests that store the same upgrades, where one
     * test joined by || would do */
    if (game_state.two_players != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE);
    } else if (game_state.demo != 0) {
        record->upgrades = UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_2 && slot == 1) {
        record->upgrades = UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE) |
                           UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD);
    } else if (stage_current_id() == STAGE_VERSUS_MECH_1 && slot == 1) {
        record->upgrades = 0;
    }
    walker_fill_gauge(record);
}

void walker_fill_gauge(struct player *record)
{
    if (stage_current_id() == STAGE_VERSUS_MECH_2 && record->slot == 1) {
        record->gauge = WALKER_GAUGE_FULL + WALKER_GAUGE_SHIELD;
    } else if (stage_current_id() == STAGE_VERSUS_MECH_1 && record->slot == 1) {
        record->gauge = WALKER_GAUGE_FULL;
    } else if (record->character == PLAYER_EGGMAN_MECH) {
        record->gauge = WALKER_GAUGE_FULL;
        if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD)) {
            record->gauge += WALKER_GAUGE_SHIELD;
        }
    } else if (record->character == PLAYER_TAILS_MECH) {
        record->gauge = WALKER_GAUGE_FULL;
    }
}

/* the work's flags that a mode taken for a request or an action clears */
#define WALKER_REQUEST_CLEARS                                                  \
    (PLAYER_WORK_ON_PATH | PLAYER_WORK_ATTACK | PLAYER_WORK_BALL)

/* the cannon's shot leaves at CANNON_SPEED faster than the walker runs. a shot
 * of the bazooka or the hyper cannon sets the cannon's recoil to CANNON_RECOIL.
 * a lock's shot sets the gun's to GUN_RECOIL. each step turns a recoil on by
 * its step. RECOIL_MASK wraps it to 0 past a whole turn */
#define CANNON_SPEED 3.0F
#define CANNON_RECOIL 0x3000
#define CANNON_RECOIL_STEP 0x1000
#define GUN_RECOIL QUARTER_TURN
#define GUN_RECOIL_STEP 0x2000
#define RECOIL_MASK 0xf000
/* the turret turns toward turret_aim by TURRET_TURN a step while released shots
 * wait. it turns only on steps that leave release_wait at RELEASE_AIM_STEPS or
 * below. it turns back toward 0 once no shot waits. an aiming walker's body
 * turns toward its stick by BODY_TURN_AIM a step. otherwise it turns back
 * toward 0 by BODY_TURN_BACK */
#define TURRET_TURN 0xc00
#define RELEASE_AIM_STEPS 4
#define BODY_TURN_AIM 0x1000
#define BODY_TURN_BACK 0x200
#define PULSE_STEP 0x200 /* the turn the lamp's pulse grows by a step */
/* the lamp's alpha for constant_material_set as its pulse turns. it is 0 at a
 * pulse of 0, and -1 half a turn on */
#define PULSE_ALPHA(pulse) ((njCos(pulse) * 0.5F) - 0.5F)
/* the jets of Tails's walker. each starts ahead of unk_218, along unk_2b4, and
 * blows back at JET_BLOW of the walker's speed.
 *
 * while jet_power is above 0, each step blows a JET_SCALED jet and takes 1 from
 * jet_power. the jet's strength is jet_power over WALKER_JET_MAX. it starts
 * JET_REACH ahead, plus as many units as its strength. each step of a run adds
 * JET_RUN_POWER to jet_power, and each step of a hover JET_HOVER_POWER.
 *
 * a run at running_speed or more blows a jet RUN_JET_REACH ahead, JET_LARGE
 * above full_speed and JET_SMALL otherwise. on the step a run's speed passes
 * full_speed, it blows out a blast and BOOST_POWDER specks of powder. they
 * leave back along unk_2b4 at BOOST_BLAST_SPEED */
#define JET_REACH 6.5F
#define JET_BLOW 0.5F
#define RUN_JET_REACH 7.5F
#define BOOST_BLAST_SPEED 4.0F
#define BOOST_POWDER 32
#define JET_RUN_POWER 6
#define JET_HOVER_POWER 8
/* a run on the ground at STEP_SPEED or more plays SOUND_STEP twice each loop of
 * its animation. it plays as the frame passes the character's STEP_FRAME_. it
 * plays again as the frame nears that plus half the loop */
#define STEP_SPEED 0.1F
#define STEP_FRAME_EGGMAN 24.0F
#define STEP_FRAME_TAILS 12.0F
/* a hover holds spd.y at HOVER_FALL_SPEED or above. on STAGE_COSMIC_WALL a
 * hover may lift the walker. it lifts while the walker lies no more than
 * WALKER_HOVER_RISE above its hover_floor. a lift that rises slower than
 * COSMIC_LIFT_SPEED takes COSMIC_LIFT times the constants' gravity as its acc.y
 */
#define HOVER_FALL_SPEED (-0.35F)
#define COSMIC_LIFT_SPEED 1.5F
#define COSMIC_LIFT 1.15F
/* PLAYER_MODE_UNK_38, which PLAYER_REQUEST_SINK starts, holds spd.x within
 * SINK_SPEED of 0 */
#define SINK_SPEED 0.5F
/* how far the glow and the laser cannon's laser_frame move toward their ends in
 * a step */
#define GLOW_IN_STEP 0.025F
#define GLOW_OUT_STEP 0.05F
#define LASER_FRAME_STEP 0.1F
/* the vibration_start power and freq of a cannon's shot */
#define CANNON_VIBRATION_POWER 4
#define CANNON_VIBRATION_FREQ 15

static void walker_step(struct task *task)
{
    struct task_work *work;
    struct player_motion *motion;
    struct player *walker;
    struct player *record;

    work = task->work;
    motion = task->motion.p;
    walker = task->extra.p;
    record = RECORD_OF(walker);
    /* @bug logic: tests for a latest touch by SHAPE_LEFT_OUT, which the
     * record never holds, so WORK_ON_TOP is never cleared here, at
     * 0x8c08e642 */
    if (record->character == PLAYER_TAILS_MECH && work != NULL &&
        work->collision != NULL &&
        work->collision->own_shape == SHAPE_LEFT_OUT &&
        (work->flags & WORK_ON_TOP)) {
        work->flags &= ~WORK_ON_TOP;
    }
    /* @bug memory: reads the work with no test, in player_ground_step and in
     * the test of its mode below, where the test at 0x8c08e62c expects a
     * null work, at 0x8c08e660 and 0x8c08e686 */
    player_ground_step(task, work, motion, record);
    walker->by_character.mech.aiming = 0;
    walker->by_character.mech.release_shot = 0;
    walker->by_character.mech.has_locks = 0;
    if (work->mode == PLAYER_MODE_MISS || work->mode == PLAYER_MODE_DROWN) {
        if (work->submode == PLAYER_REQUEST_RESET) {
            work->flags &= ~PLAYER_RESET_WORK_FLAGS;
            record->flags &= ~PLAYER_RESET_FLAGS;
            walker_fill_gauge(record);
            player_stand(work, motion, record);
        }
    } else {
        walker_choose_mode(work, motion, record, walker);
    }
    if (work->mode != PLAYER_MODE_UNK_43 &&
        (work->mode < PLAYER_MODE_HURT || work->mode > PLAYER_MODE_UNK_3B ||
         work->mode == PLAYER_MODE_PLAY_ANIM ||
         work->mode == PLAYER_MODE_SLIDE || work->mode == PLAYER_MODE_HANG)) {
        if (walker->by_character.mech.shot_wait <= 0) {
            if (pad_action_pressed[record->slot] != 0 &&
                work->mode != PLAYER_MODE_EGGMAN_MECH_STRIKE &&
                work->mode != PLAYER_MODE_TAILS_MECH_STRIKE) {
                NJS_VECTOR spd;
                Float speed;

                /* the shot leaves the cannon's muzzle, unk_248, along
                 * cannon_dir */
                speed = njAbs(record->spd.x) + CANNON_SPEED;
                spd.x = record->cannon_dir.x * speed;
                spd.y = record->cannon_dir.y * speed;
                spd.z = record->cannon_dir.z * speed;
                if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA)) {
                    walker_bazooka_create(task, &spd, &record->unk_248);
                    walker_back_blast_create(record->slot, &spd,
                                             &record->unk_248);
                    vibration_start(record->slot, CANNON_VIBRATION_POWER,
                                    CANNON_VIBRATION_FREQ, VIBRATION_UNTIMED);
                    walker->by_character.mech.cannon_recoil = CANNON_RECOIL;
                    sound_play(SOUND_BAZOOKA, NULL, 0, 0);
                } else if (record->upgrades &
                           UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON)) {
                    walker_bazooka_create(task, &spd, &record->unk_248);
                    walker_muzzle_blast_create(record->slot, &spd,
                                               &record->unk_248);
                    vibration_start(record->slot, CANNON_VIBRATION_POWER,
                                    CANNON_VIBRATION_FREQ, VIBRATION_UNTIMED);
                    walker->by_character.mech.cannon_recoil = CANNON_RECOIL;
                    sound_play(SOUND_BAZOOKA, NULL, 0, 0);
                } else {
                    walker_bullet_create(task, &spd, &record->unk_248);
                    vibration_start(record->slot, 4, 45, VIBRATION_UNTIMED);
                    sound_play(SOUND_CANNON, NULL, 0, 0);
                }
                walker->by_character.mech.lock_window = WALKER_LOCK_STEPS;
                walker->by_character.mech.shot_wait = WALKER_SHOT_STEPS;
                if ((work->id == WORK_ID_RIVAL ||
                     game_state.two_players != 0) &&
                    record->stick_lock <= 0) {
                    /* @hack redundant: the empty arm of the test of the
                     * ground */
                    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
                    } else if (record->spd.x > record->consts.full_speed) {
                        record->spd.x = record->consts.full_speed;
                    } else if (record->spd.x > record->consts.sprint_speed) {
                        record->spd.x = record->consts.sprint_speed;
                    } else if (record->spd.x > record->consts.running_speed) {
                        record->spd.x = record->consts.running_speed;
                    } else if (record->spd.x > record->consts.trot_speed) {
                        record->spd.x = record->consts.trot_speed;
                    }
                }
            }
        } else {
            walker->by_character.mech.shot_wait--;
            if ((record->upgrades &
                 (UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA) |
                  UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON))) &&
                walker->by_character.mech.cannon_recoil) {
                walker->by_character.mech.cannon_recoil =
                    (walker->by_character.mech.cannon_recoil +
                     CANNON_RECOIL_STEP) &
                    RECOIL_MASK;
            }
        }
        if (pad_action_held[record->slot] &&
            walker->by_character.mech.lock_window > 0) {
            if (--walker->by_character.mech.lock_window < 0) {
                walker->by_character.mech.lock_window = 0;
            }
            walker->by_character.mech.aiming = 1;
            if (walker->by_character.mech.lock_wait <= 0) {
                /* @bug logic: the stage is never both versus stages, so
                 * the test always holds, at 0x8c08ea7e */
                if (work->id == WORK_ID_PLAYER ||
                    stage_current_id() != STAGE_VERSUS_MECH_1 ||
                    stage_current_id() != STAGE_VERSUS_MECH_2) {
                    walker_start_lockon(record, walker);
                }
            } else {
                walker->by_character.mech.lock_wait--;
            }
            /* @bug logic: the stage is never both versus stages, so the test
             * always holds, at 0x8c08eab6 */
            if (work->id == WORK_ID_PLAYER ||
                stage_current_id() != STAGE_VERSUS_MECH_1 ||
                stage_current_id() != STAGE_VERSUS_MECH_2) {
                NJS_POINT3 to;

                if (walker->by_character.mech.laser_on_lock == 0) {
                    to = record->unk_254;
                    njAddVector(&to, &record->aim);
                } else {
                    to = walker->by_character.mech.lock_pos;
                    if (walker->by_character.mech.lock_wait <= 0) {
                        walker->by_character.mech.laser_on_lock = 0;
                    }
                }
                walker_lockon_line_create(task, &record->unk_254, &to);
                sound_play_for(SOUND_AIM, work, PLAY_PRIORITY, 0, AIM_DURATION);
            }
        } else if (pad_action_held[record->slot] &&
                   walker->by_character.mech.lock_window <= 0) {
            walker->by_character.mech.aiming = 0;
        }
        if (walker->by_character.mech.shot_count > 0 ||
            walker->by_character.mech.lock_count > 0) {
            walker->by_character.mech.has_locks = 1;
        }
        if (!pad_action_held[record->slot] &&
            walker->by_character.mech.lock_count > 0) {
            walker->by_character.mech.shot_count +=
                walker->by_character.mech.lock_count;
            lock_score_start(record->slot, walker->by_character.mech.lock_count,
                             walker->by_character.mech.locks);
            walker->by_character.mech.lock_count = 0;
            walker->by_character.mech.locks = NULL;
        }
        if (walker->by_character.mech.shot_count > 0) {
            if (walker->by_character.mech.release_wait <= 0) {
                walker->by_character.mech.release_shot = 1;
                walker->by_character.mech.release_wait = WALKER_RELEASE_STEPS;
            } else {
                walker->by_character.mech.release_wait--;
            }
            if (walker->by_character.mech.release_wait <= RELEASE_AIM_STEPS &&
                walker->by_character.mech.turret_aim) {
                NJS_VECTOR facing;
                NJS_VECTOR toward;
                Angle ang;
                NJS_VECTOR axis;

                facing = record->gun_dir;
                toward = *walker->by_character.mech.turret_aim;
                njSubVector(&toward, &record->unk_23c);
                facing.y = 0.0F;
                toward.y = 0.0F;
                if (njScalor(&facing) != 0.0F && njScalor(&toward) != 0.0F) {
                    axis.z = 0.0F;
                    axis.x = 0.0F;
                    axis.y = 1.0F;
                    njUnitVector(&facing);
                    njUnitVector(&toward);
                    ang = vector_angle(&facing, &toward, &axis) +
                          walker->by_character.mech.turret_ang;
                    walker->by_character.mech.turret_ang = angle_approach(
                        walker->by_character.mech.turret_ang, ang, TURRET_TURN);
                }
            }
            walker->by_character.mech.turret_aim = NULL;
        } else {
            walker->by_character.mech.release_wait = WALKER_RELEASE_STEPS;
            walker->by_character.mech.turret_ang = angle_approach(
                walker->by_character.mech.turret_ang, 0, TURRET_TURN);
        }
        if (walker->by_character.mech.gun_recoil) {
            walker->by_character.mech.gun_recoil =
                (walker->by_character.mech.gun_recoil + GUN_RECOIL_STEP) &
                RECOIL_MASK;
        }
        if (work->mode != PLAYER_MODE_EGGMAN_MECH_STRIKE &&
            work->mode != PLAYER_MODE_TAILS_MECH_STRIKE &&
            work->mode != PLAYER_MODE_MECH_LASER) {
            if (walker->by_character.mech.aiming == 1) {
                Angle heading;
                Angle turn;

                if (player_read_stick(work, record, &heading, NULL)) {
                    turn = angle_difference(heading, work->ang.y);
                    walker->by_character.mech.body_turn =
                        angle_approach(walker->by_character.mech.body_turn,
                                       turn, BODY_TURN_AIM);
                }
            } else {
                walker->by_character.mech.body_turn = angle_approach(
                    walker->by_character.mech.body_turn, 0, BODY_TURN_BACK);
            }
        }
        walker->by_character.mech.pulse += PULSE_STEP;
        if (record->kind == PLAYER_TAILS_MECH &&
            walker->by_character.mech.jet_power > 0) {
            NJS_VECTOR pos;
            NJS_VECTOR spd;
            Float reach;
            Float strength;

            if (walker->by_character.mech.jet_power > WALKER_JET_MAX + 1) {
                walker->by_character.mech.jet_power = WALKER_JET_MAX;
            }
            strength =
                walker->by_character.mech.jet_power * (1.0F / WALKER_JET_MAX);
            pos = record->unk_218;
            reach = strength + JET_REACH;
            pos.x += record->unk_2b4.x * reach;
            pos.y += record->unk_2b4.y * reach;
            pos.z += record->unk_2b4.z * reach;
            reach = -(njScalor(&record->spd) * JET_BLOW);
            spd.x = record->unk_2b4.x * reach;
            spd.y = record->unk_2b4.y * reach;
            spd.z = record->unk_2b4.z * reach;
            walker_jet_create(record->slot, JET_SCALED, &pos, &spd, strength);
            walker->by_character.mech.jet_power--;
            if (walker->by_character.mech.jet_power <= 0) {
                walker->by_character.mech.jet_power = 0;
            }
        }
        if ((game_state.two_players != 0 ||
             stage_current_id() == STAGE_VERSUS_MECH_2) &&
            work->mode != PLAYER_MODE_EVENT) {
            if (game_state.ring_levels[record->slot][RING_LEVEL_SPECIAL_2] !=
                0) {
                if (walker->by_character.mech.glow < 1.0F) {
                    walker->by_character.mech.glow = float_approach(
                        walker->by_character.mech.glow, 1.0F, GLOW_IN_STEP);
                }
            } else if (game_state.solo_player == 0) {
                /* @hack spelling: the glow's test in an if of its own, where
                 * one joined by && would do */
                if (walker->by_character.mech.glow > 0.0F) {
                    walker->by_character.mech.glow = float_approach(
                        walker->by_character.mech.glow, 0.0F, GLOW_OUT_STEP);
                }
            }
            if (record->character == PLAYER_EGGMAN_MECH) {
                NJS_MOTION *laser;

                laser = motion_table[MOTION_EGGMAN_LASER].motion;
                if (game_state.ring_levels[record->slot][RING_LEVEL_LASER] !=
                    0) {
                    if (walker->by_character.mech.laser_frame <
                        (Float)(laser->nbFrame - 1)) {
                        walker->by_character.mech.laser_frame = float_approach(
                            walker->by_character.mech.laser_frame,
                            (Float)(laser->nbFrame - 1), LASER_FRAME_STEP);
                    }
                } else if (game_state.solo_player == 0 &&
                           work->mode != PLAYER_MODE_MECH_LASER) {
                    if (walker->by_character.mech.laser_frame > 0.0F) {
                        walker->by_character.mech.laser_frame = float_approach(
                            walker->by_character.mech.laser_frame, 0.0F,
                            LASER_FRAME_STEP);
                    }
                }
            }
        }
    } else {
        walker->by_character.mech.shot_wait = 0;
        if ((record->upgrades & (UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA) |
                                 UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON))) &&
            walker->by_character.mech.cannon_recoil) {
            walker->by_character.mech.cannon_recoil =
                (walker->by_character.mech.cannon_recoil + CANNON_RECOIL_STEP) &
                RECOIL_MASK;
        }
        walker->by_character.mech.lock_window = 0;
        walker->by_character.mech.release_wait = 0;
        walker->by_character.mech.turret_ang = angle_approach(
            walker->by_character.mech.turret_ang, 0, TURRET_TURN);
        if (walker->by_character.mech.gun_recoil) {
            walker->by_character.mech.gun_recoil =
                (walker->by_character.mech.gun_recoil + GUN_RECOIL_STEP) &
                RECOIL_MASK;
        }
        walker->by_character.mech.body_turn = angle_approach(
            walker->by_character.mech.body_turn, 0, BODY_TURN_BACK);
        walker->by_character.mech.pulse += PULSE_STEP;
        if (game_state.solo_player == 0) {
            if (walker->by_character.mech.glow > 0.0F) {
                walker->by_character.mech.glow = float_approach(
                    walker->by_character.mech.glow, 0.0F, GLOW_OUT_STEP);
            }
            if (walker->by_character.mech.laser_frame > 0.0F) {
                walker->by_character.mech.laser_frame =
                    float_approach(walker->by_character.mech.laser_frame, 0.0F,
                                   LASER_FRAME_STEP);
            }
        }
    }
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        player_stand_anim(work, motion, record);
        break;
    case PLAYER_MODE_RUN: {
        Float speed;

        speed = njScalor(&motion->spd);
        if (record->anim.last == ANIM_TURN &&
            record->anim.current != ANIM_TURN) {
            record->spd.x += record->consts.trot_speed;
        }
        player_accel_ground(work, motion, record);
        player_apply_accel(work, motion, record);
        if (player_collide(work, motion, record) == PLAYER_CRASHED) {
            work->mode = PLAYER_MODE_CRASH;
            record->anim.next = ANIM_CRASH;
            crash_stars_burst(record->slot);
            record->spd.x = -record->spd.x;
            record->spd.z = -record->spd.z;
        } else {
            player_take_speed(work, motion, record);
        }
        walker_run_anim(work, motion, record);
        if (record->kind == PLAYER_TAILS_MECH) {
            if (njScalor(&record->spd) >= record->consts.running_speed) {
                NJS_POINT3 pos;
                NJS_VECTOR spd;
                Float reach;
                int kind;

                pos = record->unk_218;
                pos.x += record->unk_2b4.x * RUN_JET_REACH;
                pos.y += record->unk_2b4.y * RUN_JET_REACH;
                pos.z += record->unk_2b4.z * RUN_JET_REACH;
                reach = -(njScalor(&record->spd) * JET_BLOW);
                spd.x = record->unk_2b4.x * reach;
                spd.y = record->unk_2b4.y * reach;
                spd.z = record->unk_2b4.z * reach;
                if (njScalor(&record->spd) > record->consts.full_speed) {
                    kind = JET_LARGE;
                } else {
                    kind = JET_SMALL;
                }
                walker_jet_create(record->slot, kind, &pos, &spd, reach);
                if (speed < record->consts.full_speed &&
                    njScalor(&motion->spd) > record->consts.full_speed) {
                    reach = -BOOST_BLAST_SPEED;
                    spd.x = record->unk_2b4.x * reach;
                    spd.y = record->unk_2b4.y * reach;
                    spd.z = record->unk_2b4.z * reach;
                    powder_create(record->slot, BOOST_POWDER, &pos, &spd);
                    walker_muzzle_blast_create(record->slot, &spd, &pos);
                }
            }
            walker->by_character.mech.jet_power += JET_RUN_POWER;
        }
        if ((work->flags & PLAYER_WORK_ON_GROUND) &&
            record->spd.x >= STEP_SPEED) {
            Float step;
            Float frames;
            Float half;
            Float start;

            step = *record->anim.pace *
                   record->anim.actions[record->anim.current].rate;
            frames =
                motion_table[record->anim.actions[record->anim.current].motion]
                    .motion->nbFrame;
            half = frames * 0.5F;
            start = 0.0F;
            /* @hack spelling: the footfalls' frames each moved in two
             * statements, where a local for the offset would do */
            if (record->character == PLAYER_EGGMAN_MECH) {
                start += STEP_FRAME_EGGMAN;
                half += STEP_FRAME_EGGMAN;
            } else {
                start += STEP_FRAME_TAILS;
                half += STEP_FRAME_TAILS;
            }
            if ((record->anim.frame >= start &&
                 record->anim.frame < start + step) ||
                (record->anim.frame >= half - step &&
                 record->anim.frame < half)) {
                if (walker->by_character.mech.step_played == 0) {
                    sound_play(SOUND_STEP, NULL, 0, 0);
                    walker->by_character.mech.step_played = 1;
                }
            } else {
                walker->by_character.mech.step_played = 0;
            }
        }
    } break;
    case PLAYER_MODE_PATH_RUN:
        player_accel_on_path(work, motion, record);
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
    case PLAYER_MODE_MECH_HOVER:
        if (stage_current_id() == STAGE_COSMIC_WALL) {
            if (walker->by_character.mech.hover_floor >=
                work->pos.y + -WALKER_HOVER_RISE) {
                if (record->spd.y < COSMIC_LIFT_SPEED) {
                    motion->acc.y = record->consts.gravity * COSMIC_LIFT;
                }
                sound_play_at_for(SOUND_COSMIC_HOVER, work, PLAY_PRIORITY,
                                  COSMIC_HOVER_VOLUME, COSMIC_HOVER_DURATION,
                                  &work->pos);
            } else {
                walker->by_character.mech.hover_floor = WALKER_NO_HOVER_FLOOR;
            }
        }
        player_turn_upright(work, motion, record);
        player_accel_air(work, motion, record);
        if (record->spd.y < HOVER_FALL_SPEED) {
            record->spd.y = HOVER_FALL_SPEED;
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        walker_ground_dust_create(record->slot, &work->pos, &record->ground);
        if (record->kind == PLAYER_TAILS_MECH) {
            walker->by_character.mech.jet_power += JET_HOVER_POWER;
        }
        sound_play_held(SOUND_HOVER, work, PLAY_PRIORITY, 0);
        break;
    case PLAYER_MODE_EGGMAN_MECH_STRIKE:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_TAILS_MECH_STRIKE:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_MECH_SPECIAL_1:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_MECH_SPECIAL_2:
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_MECH_LASER:
        player_turn_to_stick_slowly(work, motion, record);
        player_accel_launched(work, motion, record);
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
    case PLAYER_MODE_HURT:
        player_turn_upright(work, motion, record);
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_HANG:
        break;
    case PLAYER_MODE_PLAY_ANIM:
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
    case PLAYER_MODE_UNK_43:
        player_accel_roll(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
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
    case PLAYER_MODE_CAUGHT:
        player_accel_brake(work, motion, record);
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
        /* a sinking walker turns by its stick. in the mode's other
         * animations, player_accel_air_weightless moves it */
        if (record->anim.current == ANIM_SINK_EGGMAN_MECH ||
            record->anim.current == ANIM_SINK_TAILS_MECH) {
            player_turn_to_stick_at_speed(work, motion, record);
        } else {
            player_accel_air_weightless(work, motion, record);
        }
        if (record->spd.x > SINK_SPEED) {
            record->spd.x = SINK_SPEED;
        } else if (record->spd.x < -SINK_SPEED) {
            record->spd.x = -SINK_SPEED;
        }
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        record->anim_speed = record->spd.x;
        break;
    case PLAYER_MODE_MISS:
        if (work->flags & PLAYER_WORK_GROUNDED) {
            record->anim.next = ANIM_MISS_LANDED;
        }
        player_turn_upright(work, motion, record);
        player_accel_brake(work, motion, record);
        player_apply_accel(work, motion, record);
        player_collide(work, motion, record);
        player_take_speed(work, motion, record);
        break;
    case PLAYER_MODE_FREE_MOVE:
        walker_free_move(work, motion, record);
        break;
    }
    player_anim_step(&record->anim);
    walker_place_shapes(work, motion, record, walker);
    if (player_free_move_forced(record->slot) &&
        work->mode != PLAYER_MODE_FREE_MOVE) {
        work->mode = PLAYER_MODE_FREE_MOVE;
        work->flags &=
            ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_REQUEST | PLAYER_WORK_BALL);
    }
    surface_probe_player(work, record);
    walker_set_shapes(task, work, record);
    if (record->anim.next != ANIM_WIN && record->anim.next != ANIM_LOSE &&
        work->mode != PLAYER_MODE_EVENT) {
        collision_register(task);
    }
    player_finish_step(work, motion, record);
#ifdef NONMATCHING
    /* this side: what the step left its wall slide ends with the step, so
     * that no other step's slide takes it. on the three steps watched, the
     * image's word doesn't outlast the step either. the tasks stepped and
     * drawn before the mech's next step write it many times over, and at
     * that step's walker_choose_mode it holds the work's address, which
     * lies within 2^-101 of 0 as a float */
    player_slide_y = 0.0F;
#endif
}

/* a landing at WALKER_HARD_LANDING or faster plays ANIM_LAND_FAST and shakes
 * the pad. its speed is the length of the motion's spd */
#define WALKER_HARD_LANDING 3.0F
/* a held jump adds HOVER_LIFT times the constants' gravity to a hover's acc.y
 */
#define HOVER_LIFT 0.85F
/* PLAYER_MODE_HURT ends on the ground once the hit's timer counts down to
 * HURT_LAND_STEPS. on STAGE_VERSUS_MECH_2 it ends at HURT_LAND_STEPS_VERSUS. a
 * rival's ends there once its timer leaves PLAYER_HIT_STEPS. once the timer is
 * down to HURT_LAND_STEPS, a jump held in the air can start a hover */
#define HURT_LAND_STEPS 90
#define HURT_LAND_STEPS_VERSUS 65
/* PLAYER_MODE_MECH_LASER holds laser_frame LASER_PAST_END frames past the end
 * of the laser's motion. a draw poses the motion's last frame for any frame
 * past it. so the cannon stays open while laser_frame eases back to it */
#define LASER_PAST_END 2

static void walker_choose_mode(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, struct player *walker)
{
    if (record != NULL && (record->flags & PLAYER_INVINCIBLE)) {
        work->flags |= PLAYER_WORK_UNK_8000;
        work->timer = 0;
    } else if (work->timer == 0) {
        if (player_take_hit(work, record)) {
            work->flags |= PLAYER_WORK_UNK_8000;
            work->flags &= ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_REQUEST |
                             PLAYER_WORK_HOLDING | PLAYER_WORK_BALL);
            if (record->flags & (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER)) {
                record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
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
    walker_offer_special(work, record);
    player_offer_flute(work, record);
    player_offer_hint(work, record);
    walker_offer_release(work, record, walker);
    walker_offer_strike(work, record);
    player_offer_call(work, record);
    switch (work->mode) {
    case PLAYER_MODE_STAND:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (walker_take_action(work, motion, record, walker)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (walker_test_jump(work, motion, record)) {
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
            break;
        }
        break;
    case PLAYER_MODE_RUN:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (walker_take_action(work, motion, record, walker)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (walker_test_jump(work, motion, record)) {
            break;
        }
        if (player_test_stand(work, motion, record)) {
            break;
        }
        if (player_stick_back(work, record) &&
            record->spd.x >= record->consts.trot_speed) {
            work->mode = PLAYER_MODE_BRAKE;
            record->anim.next = ANIM_BRAKE;
            record->anim.after = ANIM_WALK;
            sound_play(SOUND_BRAKE, NULL, 0, 0);
            break;
        }
        break;
    case PLAYER_MODE_PATH_RUN:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (player_test_fall(work, motion, record) ||
            walker_test_jump(work, motion, record)) {
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
            break;
        }
        break;
    case PLAYER_MODE_JUMP:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_WALK;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < WALKER_HARD_LANDING) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
                record->anim.after = ANIM_STAND;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                record->anim.next = ANIM_WALK;
            }
            work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            walker_foot_dust_create(record->slot,
                                    &walker->by_character.mech.nozzle0_pos,
                                    &walker->by_character.mech.nozzle0_dir);
            walker_foot_dust_create(record->slot,
                                    &walker->by_character.mech.nozzle1_pos,
                                    &walker->by_character.mech.nozzle1_dir);
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
        } else if (!(record->flags & PLAYER_MISSED) &&
                   player_jump_pressed(work, record)) {
            if (record->character == PLAYER_EGGMAN_MECH &&
                (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE))) {
                work->mode = PLAYER_MODE_MECH_HOVER;
                record->anim.next = ANIM_EGGMAN_HOVER;
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
            if (record->character == PLAYER_TAILS_MECH &&
                (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER))) {
                work->mode = PLAYER_MODE_MECH_HOVER;
                record->anim.next = ANIM_TAILS_HOVER;
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
            walker->by_character.mech.hover_floor = work->pos.y;
        } else {
            if (player_count_jump_hold(work, record)) {
                record->spd.y += record->consts.jump_lift * PLAYER_JUMP_LIFT;
            }
            if ((record->anim.next == ANIM_JUMP ||
                 record->anim.next == ANIM_JUMP_RISE ||
                 record->anim.next == ANIM_EGGMAN_JUMP_FAST ||
                 record->anim.next == ANIM_EGGMAN_JUMP_FAST_RISE ||
                 record->anim.next == ANIM_TAILS_JUMP_FAST ||
                 record->anim.next == ANIM_TAILS_JUMP_FAST_RISE) &&
                record->spd.y <= 0.0F) {
                record->anim.next = ANIM_JUMP_FALL;
            }
        }
        break;
    case PLAYER_MODE_BOUNCE:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_LAUNCH:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (njInnerProduct(&motion->spd, &world_gravity) >= 0.0F) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL_START;
        }
        break;
    case PLAYER_MODE_FALL:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_WALK;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < WALKER_HARD_LANDING) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
                record->anim.after = ANIM_STAND;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                record->anim.next = ANIM_WALK;
            }
            walker_foot_dust_create(record->slot,
                                    &walker->by_character.mech.nozzle0_pos,
                                    &walker->by_character.mech.nozzle0_dir);
            walker_foot_dust_create(record->slot,
                                    &walker->by_character.mech.nozzle1_pos,
                                    &walker->by_character.mech.nozzle1_dir);
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
        } else if (!(record->flags & PLAYER_MISSED) &&
                   player_jump_pressed(work, record)) {
            if (record->character == PLAYER_EGGMAN_MECH &&
                (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE))) {
                work->mode = PLAYER_MODE_MECH_HOVER;
                record->anim.next = ANIM_EGGMAN_HOVER;
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
            if (record->character == PLAYER_TAILS_MECH &&
                (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER))) {
                work->mode = PLAYER_MODE_MECH_HOVER;
                record->anim.next = ANIM_TAILS_HOVER;
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
            walker->by_character.mech.hover_floor = WALKER_NO_HOVER_FLOOR;
        }
        break;
    case PLAYER_MODE_MECH_HOVER:
        if (walker_take_request(work, motion, record, walker)) {
            sound_stop(SOUND_HOVER, work);
            break;
        }
        if (work->flags & PLAYER_WORK_GROUNDED) {
            if (player_stick_back(work, record) && record->spd.x > 0.0F) {
                work->mode = PLAYER_MODE_BRAKE;
                record->anim.next = ANIM_BRAKE;
                record->anim.after = ANIM_WALK;
            } else if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                if (njScalor(&motion->spd) < WALKER_HARD_LANDING) {
                    record->anim.next = ANIM_LAND;
                } else {
                    record->anim.next = ANIM_LAND_FAST;
                    vibration_start(record->slot, LANDING_VIBRATION_POWER,
                                    LANDING_VIBRATION_FREQ, VIBRATION_UNTIMED);
                }
                record->anim.after = ANIM_STAND;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                record->anim.next = ANIM_WALK;
            }
            walker_foot_dust_create(record->slot,
                                    &walker->by_character.mech.nozzle0_pos,
                                    &walker->by_character.mech.nozzle0_dir);
            walker_foot_dust_create(record->slot,
                                    &walker->by_character.mech.nozzle1_pos,
                                    &walker->by_character.mech.nozzle1_dir);
            sound_play(PLAYER_SOUND_LAND, NULL, 0, 0);
            sound_stop(SOUND_HOVER, work);
        } else if (!pad_jump_held[record->slot] ||
                   (record->flags & PLAYER_MISSED)) {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
            sound_stop(SOUND_HOVER, work);
        } else {
            motion->acc.y += record->consts.gravity * HOVER_LIFT;
        }
        break;
    case PLAYER_MODE_BRAKE:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (player_test_fall(work, motion, record)) {
            break;
        }
        if (walker_test_jump(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_BRAKE) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_TEETER:
        if (walker_take_request(work, motion, record, walker)) {
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
            break;
        }
        break;
    case PLAYER_MODE_HURT:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (((work->id == WORK_ID_RIVAL && work->timer != PLAYER_HIT_STEPS) ||
             (work->timer <= HURT_LAND_STEPS &&
              stage_current_id() != STAGE_VERSUS_MECH_2) ||
             (work->timer <= HURT_LAND_STEPS_VERSUS &&
              stage_current_id() == STAGE_VERSUS_MECH_2)) &&
            (work->flags & PLAYER_WORK_GROUNDED)) {
            if (player_test_stand(work, motion, record)) {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                record->anim.next = ANIM_HURT_LAND;
            } else {
                work->ang.x = motion->aim_x;
                work->ang.z = motion->aim_z;
                work->mode = PLAYER_MODE_RUN;
                record->anim.next = ANIM_WALK;
            }
            break;
        }
        if (work->timer <= HURT_LAND_STEPS &&
            !(work->flags & PLAYER_WORK_GROUNDED) &&
            !(work->flags & PLAYER_WORK_NO_CONTROL) &&
            !(record->flags & PLAYER_MISSED) && pad_jump_held[record->slot]) {
            if (record->character == PLAYER_EGGMAN_MECH &&
                (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE))) {
                work->mode = PLAYER_MODE_MECH_HOVER;
                record->anim.next = ANIM_EGGMAN_HOVER;
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
            if (record->character == PLAYER_TAILS_MECH &&
                (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER))) {
                work->mode = PLAYER_MODE_MECH_HOVER;
                record->anim.next = ANIM_TAILS_HOVER;
                work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
            }
            walker->by_character.mech.hover_floor = WALKER_NO_HOVER_FLOOR;
            break;
        }
        break;
    case PLAYER_MODE_HANG:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (walker_test_jump(work, motion, record)) {
            break;
        }
        break;
    case PLAYER_MODE_PLAY_ANIM:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        break;
    case PLAYER_MODE_CRASH:
        if (record->anim.next == ANIM_CRASH) {
            break;
        } else if (player_test_fall(work, motion, record)) {
            break;
        }
        if (record->anim.next != ANIM_CRASH_END) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_UNK_43:
        if (record->anim.next == ANIM_CRASH) {
            break;
        }
        if (record->anim.next != ANIM_CRASH_END) {
            player_stand(work, motion, record);
        }
        break;
    case PLAYER_MODE_SLIDE:
        if (walker_take_request(work, motion, record, walker) ||
            walker_take_action(work, motion, record, walker) ||
            player_test_fall(work, motion, record) ||
            walker_test_jump(work, motion, record)) {
            break;
        }
        if (player_read_stick(work, record, NULL, NULL)) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
            break;
        } else if (njScalor(&motion->spd) < record->consts.slide_speed ||
                   record->ball->up.y > PLAYER_SLIDE_LEVEL) {
            work->mode = PLAYER_MODE_STAND;
            record->mode_steps = 0;
            record->anim.next = ANIM_LAND_STILL;
            break;
        }
        break;
    case PLAYER_MODE_CAUGHT:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (pad_stick_pushed[record->slot] ||
            (player_pad_records[record->slot].press &
             PLAYER_STRUGGLE_BUTTONS)) {
            walker->by_character.mech.mode_count -= PLAYER_STRUGGLE_PUSH;
        }
        if (walker->by_character.mech.mode_count > 0) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_UNK_2A:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_ASK_HINT:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        player_stand(work, motion, record);
        break;
    case PLAYER_MODE_PLAY_FLUTE:
        if (walker_take_request(work, motion, record, walker)) {
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
        if (walker_take_request(work, motion, record, walker)) {
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
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        work->mode = PLAYER_MODE_UNK_35;
        break;
    case PLAYER_MODE_UNK_35:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.flags & PLAYER_ANIM_DONE) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_EGGMAN_MECH_STRIKE:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.next == ANIM_EGGMAN_STRIKE) {
            break;
        }
        player_stand(work, motion, record);
        work->flags &= ~PLAYER_WORK_ATTACK;
        break;
    case PLAYER_MODE_TAILS_MECH_STRIKE:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.next == ANIM_TAILS_STRIKE) {
            break;
        }
        player_stand(work, motion, record);
        work->flags &= ~PLAYER_WORK_ATTACK;
        break;
    case PLAYER_MODE_MECH_SPECIAL_1:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.last == ANIM_SPECIAL_1) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_MECH_SPECIAL_2:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (record->anim.last == ANIM_SPECIAL) {
            player_stand(work, motion, record);
            break;
        }
        break;
    case PLAYER_MODE_MECH_LASER:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        if (walker->by_character.mech.mode_count++ > WALKER_LASER_STEPS) {
            player_stand(work, motion, record);
            break;
        }
        if (record->character == PLAYER_EGGMAN_MECH) {
            walker->by_character.mech.laser_frame =
                motion_table[MOTION_EGGMAN_LASER].motion->nbFrame +
                LASER_PAST_END;
        }
        break;
    case PLAYER_MODE_UNK_38:
        if (walker_take_request(work, motion, record, walker)) {
            break;
        }
        break;
    case PLAYER_MODE_EVENT:
        if (work->submode == PLAYER_REQUEST_RELEASE) {
            player_stand(work, motion, record);
            work->flags &= ~WALKER_REQUEST_CLEARS;
            break;
        }
        break;
    case PLAYER_MODE_FREE_MOVE:
        if (player_pad_records[record->slot].press & PDD_DGT_TA) {
            work->mode = PLAYER_MODE_RUN;
            record->anim.next = ANIM_WALK;
        }
        break;
    }
}

static void walker_destroy(struct task *task)
{
    struct task_work *work = task->work;
    struct player *walker = task->extra.p;
    struct player *record = RECORD_OF(walker);

    player_unregister(record->slot);
    sound_stop_key(work);
    njReleaseTexture(walker->by_character.mech.texlist);
    syFree(walker->by_character.mech.texlist);
    walker->by_character.mech.texlist = NULL;
    njReleaseTexture(walker->by_character.mech.effects_texlist);
    syFree(walker->by_character.mech.effects_texlist);
    walker->by_character.mech.effects_texlist = NULL;
    life_icon_release(record->kind);
    model_file_free(walker->by_character.mech.models);
    walker->by_character.mech.models = NULL;
    motion_file_free(walker->by_character.mech.motions);
    walker->by_character.mech.motions = NULL;
}

/* put the body's shape and the top one at the record's center_height above
 * the work's pos, along the walker's up. motion and walker are unused */
static void walker_place_shapes(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *walker)
{
    /* @bug memory: reads the work's collision record with no test, where
     * walker_step at 0x8c08e634 and walker_set_shapes at 0x8c095570 expect
     * a null record, at 0x8c09100c */
    struct collision_shape *shape = work->collision->shapes;
    NJS_VECTOR center;

    center.x = center.z = 0.0F;
    center.y = record->consts.center_height;
    turn_to_world(work, &center);
    shape->center = center;
    njAddVector(&shape->center, &work->pos);
    /* @hack spelling: shape stepped ahead to the top shape, where
     * shapes[SHAPE_TOP] would do */
    shape += SHAPE_TOP;
    center.x = center.z = 0.0F;
    center.y = record->consts.center_height;
    turn_to_world(work, &center);
    shape->center = center;
    njAddVector(&shape->center, &work->pos);
}

/* a motion callback sets the record's aim AIM_LENGTH long from its start */
#define AIM_LENGTH 800.0F
/* the laser of PLAYER_MODE_MECH_LASER starts LASER_OUT along the -x axis of a
 * node. for the Eggman walker and the dark Chao walker, the node is the
 * cockpit. for the Chao walker it is the turret. the Tails walker's laser
 * starts TWALK_LASER_OUT along its cockpit's */
#define LASER_OUT 10.0F
#define TWALK_LASER_OUT 20.0F

/* the motion callback of the Eggman walker's draw and the dark Chao walker's.
 * it keeps the places and axes of the nodes the walker reads, turned into the
 * world. it keeps the matrices of the nodes a draw adds parts at. it turns
 * EWALK_LEANS about x by the record's lean in PLAYER_MODE_RUN. it turns
 * EWALK_COCKPIT about y by body_turn */
static void ewalk_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *walker = walker_drawn;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = pos.y = pos.z = 0.0F;
    if (object == model_table[EWALK_UPPER].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_230);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2cc);
        njGetMatrix(&upper_matrix);
    } else if (object == model_table[EWALK_LEANS].object) {
        /* @hack spelling: work, a local for the walker's player_work, where
         * the table read in place would do */
        struct task_work *work = player_work[walker->slot];

        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, walker->lean);
        }
    } else if (object == model_table[EWALK_COCKPIT].object) {
        /* @hack spelling: walker_drawn read again, where walker would do */
        njRotateY(NULL, walker_drawn->by_character.mech.body_turn);
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_218);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2b4);
        njGetMatrix(&cockpit_matrix);
        pos.x = -LASER_OUT;
        pos.y = pos.z = 0.0F;
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->laser_origin);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->laser_dir);
    } else if (object == model_table[EWALK_FOOT_0].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_200);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_29c);
    } else if (object == model_table[EWALK_FOOT_1].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_20c);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2a8);
    } else if (object == model_table[EWALK_AT_1E8].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_1e8);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_26c);
    } else if (object == model_table[EWALK_TURRET].object) {
        njGetMatrix(&turret_matrix);
    } else if (object == model_table[EWALK_CANNON_MOUNT_B].object) {
        njGetMatrix(&cannon_matrix);
    } else if (object == model_table[EWALK_CANNON_MOUNT_A].object) {
        njGetMatrix(&cannon_matrix);
    } else if (object == model_table[EWALK_NOZZLE0].object) {
        njGetMatrix(&nozzle0_matrix);
        dir.x = -1.0F;
        dir.z = dir.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        /* @hack spelling: walker_drawn read again for the nozzles, where
         * walker would do */
        njCalcPoint(&view_to_world, &pos,
                    &walker_drawn->by_character.mech.nozzle0_pos);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.nozzle0_dir);
    } else if (object == model_table[EWALK_NOZZLE1].object) {
        njGetMatrix(&nozzle1_matrix);
        dir.x = -1.0F;
        dir.z = dir.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos,
                    &walker_drawn->by_character.mech.nozzle1_pos);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.nozzle1_dir);
    } else if (object == model_table[EWALK_AIM].object) {
        dir.x = -AIM_LENGTH;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_254);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->aim);
    }
}

/* the Eggman walker's draw sets no DRAW_CONTROL_SHADOW under
 * LIGHT_LOST_COLONY on STAGE_LOST_COLONY, as under LIGHT_UNK_1 and
 * LIGHT_VOLUME, and draws without fog there */
#define LIGHT_LOST_COLONY 3
/* the x of the gun's first child at rest. the gun's recoil takes from it
 * GUN_RECOIL_DEPTH times 1 less the recoil's cosine. the cannon's recoil moves
 * the bazooka along x by 1 less its cosine. it moves the hyper cannon along y
 * by TWALK_HYPERCANNON_RECOIL times its cosine less 1 */
#define EWALK_GUN_X 3.0F
#define EWALK_POWERGUN_X 2.7F
#define TWALK_GUN_X 7.5F
#define GUN_RECOIL_DEPTH 0.5F
#define TWALK_HYPERCANNON_RECOIL 1.25F

/* the Eggman walker's draw. while game_state.solo_player names a player, only
 * that player's walker draws, and upright. otherwise a walker whose work's
 * timer has PLAYER_TIMER_HIDDEN draws only its shadows */
static void ewalk_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *walker = task->extra.p;
    struct player *record;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
    struct collision_shape *shape;
    NJS_MOTION *motion;
    int anim;
    int light;
    Float frame;
    Float across;
    Float along;
    Uint32 steps;
    /* @hack spelling: draw, a local for njCnkSimpleDrawObject, where calls
     * by name would do */
    void (*draw)(NJS_CNK_OBJECT *object);
    NJS_VECTOR dir;
    NJS_POINT3 pos;

    record = RECORD_OF(walker);
    /* @hack type: game_state's solo_player read as game_state_solo_player here
     * and below, where game_state.solo_player would do */
    if (game_state_solo_player != 0 &&
        game_state_solo_player != record->slot + 1) {
        return;
    }
    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    /* @hack spelling: light set inside the test, where a statement of its own
     * would do */
    if ((light = light_selected[record->slot]) != LIGHT_UNK_1 &&
        light != LIGHT_VOLUME &&
        (stage_current_id() != STAGE_LOST_COLONY ||
         light_selected[record->slot] != LIGHT_LOST_COLONY)) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    if (stage_current_id() == STAGE_LOST_COLONY &&
        light_selected[record->slot] == LIGHT_LOST_COLONY) {
        njFogDisable();
    }
    light_apply_player(record->slot);
    walker_drawn = walker;
    njGetMatrix(&view_to_world);
    njInvertMatrix(&view_to_world);
    njCnkSetMotionCallback(ewalk_motion_callback);
    anim = record->anim.current;
    /* @hack redundant: a test that never runs, which reads anim three
     * times */
    if (0) {
        if (anim + anim + anim) {
        }
    }
    /* @hack redundant: a test of the timer at 0 or more, which always
     * holds */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state_solo_player != 0) {
        njSetTexture(walker->by_character.mech.texlist);
        njPushMatrixEx();
        if (work->mode != PLAYER_MODE_HANG) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            /* @hack spelling: the zero stores in two statements, where one
             * chained store would do */
            dir.z = 0.0F;
            dir.x = 0.0F;
            dir.y = -(record->consts.height * 0.5F);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &dir, &pos);
            njPopMatrixEx();
            pos.x += shape->center.x;
            pos.y += shape->center.y;
            pos.z += shape->center.z;
            njTranslateEx(&pos);
        }
        if (game_state_solo_player == record->slot + 1) {
            njRotateY(NULL, HALF_TURN - work->ang.y);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, HALF_TURN - work->ang.y);
        }
        njScaleEx(&work->scl);
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[0].model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                motion_table[record->anim
                                 .actions[record->anim.actions[anim].motion]
                                 .motion]
                    .motion,
                record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                record->anim.blend, record->anim.frame);
        }
        pos.x = pos.y = pos.z = 0.0F;
        njPushMatrixEx();
        njSetMatrix(NULL, &turret_matrix);
        if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_POWERGUN)) {
            model_table[EWALK_POWERGUN].object->child->evalflags &=
                ~NJD_EVAL_UNIT_ANG;
            model_table[EWALK_POWERGUN].object->child->ang[1] =
                walker->by_character.mech.turret_ang;
            model_table[EWALK_POWERGUN].object->child->pos[0] =
                EWALK_POWERGUN_X -
                ((1.0F - njCos(walker->by_character.mech.gun_recoil)) *
                 GUN_RECOIL_DEPTH);
            object = model_table[EWALK_POWERGUN].object;
        } else {
            model_table[EWALK_GUN].object->child->evalflags &=
                ~NJD_EVAL_UNIT_ANG;
            model_table[EWALK_GUN].object->child->ang[1] =
                walker->by_character.mech.turret_ang;
            model_table[EWALK_GUN].object->child->pos[0] =
                EWALK_GUN_X -
                ((1.0F - njCos(walker->by_character.mech.gun_recoil)) *
                 GUN_RECOIL_DEPTH);
            object = model_table[EWALK_GUN].object;
        }
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        for (;;) {
            if (object == NULL) {
                break;
            }
            if (object == (part = model_table[EWALK_LAMP_MUZZLE].object)) {
                njPushMatrixEx();
            }
            if (part != NULL && object == part->sibling) {
                draw_control_save();
                draw_control_clear(NJD_CONTROL_3D_CONSTANT_MATERIAL);
                constant_material_set(
                    PULSE_ALPHA(walker->by_character.mech.pulse), 0.0F, 0.0F,
                    0.0F);
                njFogDisable();
            }
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            if (object->model != NULL) {
                njCnkSimpleDrawModel(object->model);
            }
            if (object == model_table[EWALK_GUN_MUZZLE].object ||
                object == part) {
                njCalcPoint(NULL, &pos, &pos);
                njCalcPoint(&view_to_world, &pos, &record->unk_23c);
                njCalcVector(NULL, &dir, &dir);
                njCalcVector(&view_to_world, &dir, &record->gun_dir);
                dir.y = -1.0F;
                dir.x = dir.z = 0.0F;
                njCalcVector(NULL, &dir, &dir);
                njCalcVector(&view_to_world, &dir,
                             &walker->by_character.mech.gun_down);
                if (object == part) {
                    object = object->sibling;
                    njPopMatrixEx();
                } else {
                    object = object->child;
                }
            } else {
                if (part != NULL && object == part->sibling) {
                    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
                    draw_control_restore();
                    if (stage_current_id() != STAGE_LOST_COLONY ||
                        light_selected[record->slot] != LIGHT_LOST_COLONY) {
                        njFogEnable();
                    }
                }
                object = object->child;
            }
            if (object == NULL) {
                break;
            }
        }
        njSetMatrix(NULL, &cannon_matrix);
        draw = njCnkSimpleDrawObject;
        if ((record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_BAZOOKA)) &&
            record->kind == PLAYER_EGGMAN_MECH) {
            njTranslate(NULL,
                        1.0F - njCos(walker->by_character.mech.cannon_recoil),
                        0.0F, 0.0F);
            draw(model_table[EWALK_BAZOOKA].object);
            object = model_table[EWALK_BAZOOKA_MUZZLE].object;
            if (walker->by_character.mech.cannon_recoil > 0 &&
                walker->by_character.mech.cannon_recoil < HALF_TURN) {
                njFogDisable();
                /* @hack type: game_state's steps read as game_state_steps,
                 * where game_state.steps would do */
                if (game_state_steps & 1) {
                    njCnkEasyDrawObject(model_table[EWALK_FLASH_B].object);
                    njCnkEasyDrawObject(model_table[EWALK_FLASH_A].object);
                } else {
                    njCnkEasyDrawObject(model_table[EWALK_FLASH_B + 1].object);
                    njCnkEasyDrawObject(model_table[EWALK_FLASH_A + 1].object);
                }
                if (stage_current_id() != STAGE_LOST_COLONY ||
                    light_selected[record->slot] != LIGHT_LOST_COLONY) {
                    njFogEnable();
                }
            }
        } else {
            draw(model_table[EWALK_CANNON].object);
            object = model_table[EWALK_CANNON_MUZZLE].object;
        }
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        pos.x = pos.y = pos.z = 0.0F;
        njTranslateEx((NJS_VECTOR *)object->pos);
        njRotateEx(object->ang, 0);
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_248);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &record->cannon_dir);
        njPopMatrixEx();
        if (anim == ANIM_EGGMAN_STRIKE &&
            record->anim.mode != PLAYER_ANIM_BLEND) {
            frame = record->anim.frame;
            njPushMatrixEx();
            njSetMatrix(NULL, &upper_matrix);
            njCnkSimpleDrawMotion(
                model_table[EWALK_STRIKE_PART].object,
                motion_table[record->anim.actions[ANIM_EGGMAN_STRIKE_PART]
                                 .motion]
                    .motion,
                frame);
            njPopMatrixEx();
        }
        if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE)) {
            njPushMatrixEx();
            /* @hack spelling: object set inside the tests, where statements
             * of their own would do */
            if ((object = model_table[EWALK_JET0].object) != NULL) {
                njSetMatrix(NULL, &nozzle0_matrix);
                draw(object);
            }
            if ((object = model_table[EWALK_JET1].object) != NULL) {
                njSetMatrix(NULL, &nozzle1_matrix);
                draw(object);
            }
            if (model_table[EWALK_FLAME].object != NULL &&
                anim == ANIM_EGGMAN_HOVER) {
                /* @hack type: game_state's steps read as game_state_steps,
                 * where game_state.steps would do */
                /* @hack spelling: steps set inside the index, where a
                 * statement of its own would do */
                across =
                    ewalk_flicker[((steps = game_state_steps) + FLICKER_LEAD) &
                                  (FLICKER_STEPS - 1)];
                along = ewalk_flicker[steps & (FLICKER_STEPS - 1)];
                draw_control_clear(DRAW_CONTROL_SHADOW);
                njFogDisable();
                object = model_table[EWALK_FLAME].object->child;
                njSetMatrix(NULL, &nozzle1_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                njSetMatrix(NULL, &nozzle0_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                /* @hack spelling: light set inside the test, where a
                 * statement of its own would do */
                if ((light = light_selected[record->slot]) != LIGHT_UNK_1 &&
                    light != LIGHT_VOLUME &&
                    (stage_current_id() != STAGE_LOST_COLONY ||
                     light_selected[record->slot] != LIGHT_LOST_COLONY)) {
                    draw_control_set(DRAW_CONTROL_SHADOW);
                }
                if (stage_current_id() != STAGE_LOST_COLONY ||
                    light_selected[record->slot] != LIGHT_LOST_COLONY) {
                    njFogEnable();
                }
            }
            njPopMatrixEx();
        }
        if (stage_current_id() == STAGE_LOST_COLONY) {
            /* @hack spelling: light set inside the test, where a statement
             * of its own would do */
            if ((light = light_selected[record->slot]) != LIGHT_UNK_1 &&
                light != LIGHT_VOLUME && light != LIGHT_LOST_COLONY) {
                draw_control_clear(DRAW_CONTROL_SHADOW);
            }
            njFogDisable();
            njPushMatrixEx();
            njSetMatrix(NULL, &cockpit_matrix);
            if ((object = model_table[EWALK_COLONY_A].object) != NULL) {
                njCnkEasyDrawObject(object);
            }
            if ((object = model_table[EWALK_COLONY_B].object) != NULL) {
                njCnkEasyDrawObject(object);
            }
            if ((object = model_table[EWALK_COLONY_C].object) != NULL) {
                njSetMatrix(NULL, &upper_matrix);
                njCnkEasyDrawObject(object);
            }
            light = light_selected[record->slot];
            if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME &&
                light != LIGHT_LOST_COLONY) {
                draw_control_set(DRAW_CONTROL_SHADOW);
            }
            njPopMatrixEx();
            if (light_selected[record->slot] != LIGHT_LOST_COLONY) {
                njFogEnable();
            }
        }
        if (walker->by_character.mech.laser_frame > 0.0F) {
            motion = motion_table[MOTION_EGGMAN_LASER].motion;
            /* @hack spelling: frame set inside the test, where a statement
             * of its own would do */
            if ((frame = walker->by_character.mech.laser_frame) >
                (Float)(motion->nbFrame - 1)) {
                frame = (Float)(motion->nbFrame - 1);
            }
            njPushMatrixEx();
            njSetMatrix(NULL, &upper_matrix);
            njCnkSimpleDrawMotion(model_table[EWALK_LASER].object,
                                  motion_table[MOTION_EGGMAN_LASER].motion,
                                  frame);
            njPopMatrixEx();
        }
        if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_EXTRASHIELD)) {
            object = model_table[EWALK_SHIELD].object;
            if (object != NULL) {
                njPushMatrix(&cockpit_matrix);
                draw(object);
                njPopMatrixEx();
            }
        }
        if (walker->by_character.mech.glow > 0.0F) {
            draw_control_save();
            draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            draw_attr_set(0, NJD_FST_UA);
            draw_attr_clear(0, NJD_FST_FL);
            constant_material_set(walker->by_character.mech.glow, 1.0F, 1.0F,
                                  1.0F);
            njPushMatrixEx();
            njSetMatrix(NULL, &cockpit_matrix);
            draw(model_table[EWALK_GLOW].object);
            njSetMatrix(NULL, &view_to_world);
            njMultiMatrix(NULL, &cockpit_matrix);
            njCalcPoints(NULL, ewalk_missile_points,
                         walker->by_character.mech.missile_points,
                         PLAYER_MISSILE_POINTS);
            njCalcVector(NULL, &ewalk_missile_dir,
                         &walker->by_character.mech.missile_dir);
            njPopMatrixEx();
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_control_restore();
            draw_attr_clear(0, NJD_FST_UA);
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    ewalk_draw_shadows(work, record, anim);
    if (game_state.unk_0b == 1) {
        walker_gauge_draw(record->slot);
    }
}

/* the shadows of the Eggman walker's body or the dark Chao walker's.
 * EWALK_COCKPIT_SHADOW falls at unk_218, and EWALK_FOOT_SHADOW at each foot,
 * unk_200 and unk_20c. none falls while anim poses another model than
 * EWALK_BODY */
static void ewalk_draw_shadows(struct task_work *work, struct player *record,
                               int anim)
{
    int model = record->anim.actions[anim].model;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == EWALK_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[EWALK_COCKPIT_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[EWALK_FOOT_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[EWALK_FOOT_SHADOW].object);
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* the motion callback of the Chao walker's draw, as ewalk_motion_callback is
 * the Eggman walker's. the draw adds no gun or cannon, so the callback keeps
 * the muzzles at nodes of the body. the gun's is TWALK_TURRET, and the cannon's
 * TWALK_CANNON_MOUNT */
static void cwalk_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *walker = walker_drawn;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = pos.y = pos.z = 0.0F;
    if (object == model_table[TWALK_UPPER].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_230);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2cc);
        njGetMatrix(&upper_matrix);
    } else if (object == model_table[TWALK_LEANS].object) {
        /* @hack spelling: work, a local for the walker's player_work, where
         * the table read in place would do */
        struct task_work *work = player_work[walker->slot];

        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, walker->lean);
        }
    } else if (object == model_table[TWALK_COCKPIT].object) {
        /* @hack spelling: walker_drawn read again, where walker would do */
        njRotateY(NULL, walker_drawn->by_character.mech.body_turn);
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_218);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2b4);
        njGetMatrix(&cockpit_matrix);
    } else if (object == model_table[TWALK_FOOT_0].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_200);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_29c);
    } else if (object == model_table[TWALK_FOOT_1].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_20c);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2a8);
    } else if (object == model_table[TWALK_AT_1E8].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_1e8);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_26c);
    } else if (object == model_table[TWALK_TURRET].object) {
        njGetMatrix(&turret_matrix);
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_23c);
        /* @bug uninitialized: turns dir into gun_dir, the way the gun
         * points, before the branch sets dir, which holds what the stack
         * held, at 0x8c0923d8. the other draws turn (-1, 0, 0) into it */
#ifdef NONMATCHING
        /* this side repairs the tag above: dir starts as (-1, 0, 0), as in
         * the other draws, so the gun points along the node's -x. Ninja's
         * draw_motion_tree calls this callback for each node, once it has
         * posed the node. the image's dir is three words below that walk's
         * frame, which this call reads before it writes them. they hold what
         * the walk's earlier calls left there: those that posed this node,
         * and those for earlier nodes. so the image's gun follows the walk's
         * leftovers, not one direction to keep */
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
#endif
        njCalcVector(NULL, &dir,
                     &dir); /* NOLINT(clang-diagnostic-uninitialized) */
        njCalcVector(&view_to_world, &dir, &walker->gun_dir);
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.gun_down);
        pos.x = -LASER_OUT;
        pos.y = pos.z = 0.0F;
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->laser_origin);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->laser_dir);
    } else if (object == model_table[TWALK_CANNON_MOUNT].object) {
        njGetMatrix(&cannon_matrix);
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_248);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->cannon_dir);
    } else if (object == model_table[TWALK_NOZZLE0].object) {
        njGetMatrix(&nozzle0_matrix);
        dir.x = -1.0F;
        dir.z = dir.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        /* @hack spelling: walker_drawn read again for the nozzles, where
         * walker would do */
        njCalcPoint(&view_to_world, &pos,
                    &walker_drawn->by_character.mech.nozzle0_pos);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.nozzle0_dir);
    } else if (object == model_table[TWALK_NOZZLE1].object) {
        njGetMatrix(&nozzle1_matrix);
        dir.x = -1.0F;
        dir.z = dir.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos,
                    &walker_drawn->by_character.mech.nozzle1_pos);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.nozzle1_dir);
    } else if (object == model_table[TWALK_AIM].object) {
        dir.x = -AIM_LENGTH;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_254);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->aim);
    }
}

/* the Chao walker's draw, as ewalk_draw is the Eggman walker's */
static void cwalk_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *walker = task->extra.p;
    struct player *record;
    NJS_CNK_OBJECT *object;
    struct collision_shape *shape;
    int anim;
    int light;
    Float frame;
    Float across;
    Float along;
    Uint32 steps;
    NJS_VECTOR dir;
    NJS_POINT3 pos;

    record = RECORD_OF(walker);
    if (game_state.solo_player != 0 &&
        game_state.solo_player != record->slot + 1) {
        return;
    }
    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    light = light_selected[record->slot];
    if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    walker_drawn = walker;
    njGetMatrix(&view_to_world);
    njInvertMatrix(&view_to_world);
    njCnkSetMotionCallback(cwalk_motion_callback);
    anim = record->anim.current;
    /* @hack redundant: a test of the timer at 0 or more, which always
     * holds */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state.solo_player != 0) {
        njSetTexture(walker->by_character.mech.texlist);
        njPushMatrixEx();
        if (work->mode != PLAYER_MODE_HANG) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            dir.x = dir.z = 0.0F;
            dir.y = -(record->consts.height * 0.5F);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &dir, &pos);
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
        }
        njScaleEx(&work->scl);
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[0].model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                motion_table[record->anim
                                 .actions[record->anim.actions[anim].motion]
                                 .motion]
                    .motion,
                record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                record->anim.blend, record->anim.frame);
        }
        if (anim == ANIM_TAILS_STRIKE &&
            record->anim.mode != PLAYER_ANIM_BLEND) {
            frame = record->anim.frame * TWALK_STRIKE_PART_RATE;
            njPushMatrixEx();
            njSetMatrix(NULL, &upper_matrix);
            njCnkSimpleDrawMotion(
                model_table[TWALK_STRIKE_PART].object,
                motion_table[record->anim.actions[ANIM_TAILS_STRIKE_PART]
                                 .motion]
                    .motion,
                frame);
            njPopMatrixEx();
        }
        /* @hack spelling: the upgrade's test in an if of its own, where one
         * joined by && would do */
        if (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER)) {
            if (model_table[TWALK_FLAME].object != NULL &&
                anim == ANIM_TAILS_HOVER) {
                /* @hack type: game_state's steps read as game_state_steps,
                 * where game_state.steps would do */
                /* @hack spelling: steps set inside the index, where a
                 * statement of its own would do */
                across =
                    cwalk_flicker[((steps = game_state_steps) + FLICKER_LEAD) &
                                  (FLICKER_STEPS - 1)];
                along = cwalk_flicker[steps & (FLICKER_STEPS - 1)];
                draw_control_clear(DRAW_CONTROL_SHADOW);
                njPushMatrixEx();
                /* the test above reads TWALK_FLAME, but the flame drawn is
                 * EWALK_FLAME's child */
                object = model_table[EWALK_FLAME].object->child;
                njSetMatrix(NULL, &nozzle1_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                njSetMatrix(NULL, &nozzle0_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                njPopMatrixEx();
                light = light_selected[record->slot];
                if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME) {
                    draw_control_set(DRAW_CONTROL_SHADOW);
                }
            }
        }
        if (walker->by_character.mech.glow > 0.0F) {
            draw_control_save();
            draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            draw_attr_set(0, NJD_FST_UA);
            draw_attr_clear(0, NJD_FST_FL);
            constant_material_set(walker->by_character.mech.glow, 1.0F, 1.0F,
                                  1.0F);
            njPushMatrixEx();
            njSetMatrix(NULL, &cockpit_matrix);
            njCnkSimpleDrawObject(model_table[TWALK_GLOW].object);
            njSetMatrix(NULL, &view_to_world);
            njMultiMatrix(NULL, &cockpit_matrix);
            njCalcPoints(NULL, cwalk_missile_points,
                         walker->by_character.mech.missile_points,
                         PLAYER_MISSILE_POINTS);
            njCalcVector(NULL, &cwalk_missile_dir,
                         &walker->by_character.mech.missile_dir);
            njPopMatrixEx();
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_control_restore();
            draw_attr_clear(0, NJD_FST_UA);
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    cwalk_draw_shadows(work, record, anim);
    if (game_state.unk_0b == 1) {
        walker_gauge_draw(record->slot);
    }
}

/* the shadows of the Chao walker's body, as ewalk_draw_shadows draws the
 * Eggman walker's */
static void cwalk_draw_shadows(struct task_work *work, struct player *record,
                               int anim)
{
    int model = record->anim.actions[anim].model;
    Angle unused; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == TWALK_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TWALK_COCKPIT_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TWALK_FOOT_SHADOW_A].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TWALK_FOOT_SHADOW_B].object);
        /* @hack redundant: a read of work's ang.y that nothing uses */
        unused = work->ang.y;
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* the dark Chao walker's draw, as ewalk_draw is the Eggman walker's. it
 * draws with ewalk_motion_callback */
static void bwalk_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *walker = task->extra.p;
    struct player *record;
    NJS_CNK_OBJECT *object;
    struct collision_shape *shape;
    NJS_MOTION *motion;
    int anim;
    int light;
    Float frame;
    Float across;
    Float along;
    Uint32 steps;
    NJS_VECTOR dir;
    NJS_POINT3 pos;

    record = RECORD_OF(walker);
    /* @hack type: game_state's solo_player read as game_state_solo_player here
     * and below, where game_state.solo_player would do */
    if (game_state_solo_player != 0 &&
        game_state_solo_player != record->slot + 1) {
        return;
    }
    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    light = light_selected[record->slot];
    if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    walker_drawn = walker;
    njGetMatrix(&view_to_world);
    njInvertMatrix(&view_to_world);
    njCnkSetMotionCallback(ewalk_motion_callback);
    anim = record->anim.current;
    /* @hack redundant: a test of the timer at 0 or more, which always
     * holds */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state_solo_player != 0) {
        njSetTexture(walker->by_character.mech.texlist);
        njPushMatrixEx();
        if (work->mode != PLAYER_MODE_HANG) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            dir.x = dir.z = 0.0F;
            dir.y = -(record->consts.height * 0.5F);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &dir, &pos);
            njPopMatrixEx();
            pos.x += shape->center.x;
            pos.y += shape->center.y;
            pos.z += shape->center.z;
            njTranslateEx(&pos);
        }
        if (game_state_solo_player == record->slot + 1) {
            njRotateY(NULL, HALF_TURN - work->ang.y);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, HALF_TURN - work->ang.y);
        }
        njScaleEx(&work->scl);
        if (work->mode == PLAYER_MODE_EVENT && record->event != NULL) {
            player_hide_event_models(record->slot);
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[0].model].object,
                record->event->motion, mini_event_frame);
            player_draw_event_parts(record->slot);
        } else if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                motion_table[record->anim
                                 .actions[record->anim.actions[anim].motion]
                                 .motion]
                    .motion,
                record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                record->anim.blend, record->anim.frame);
        }
        pos.x = pos.y = pos.z = 0.0F;
        njPushMatrixEx();
        njSetMatrix(NULL, &turret_matrix);
        model_table[EWALK_GUN].object->child->evalflags &= ~NJD_EVAL_UNIT_ANG;
        model_table[EWALK_GUN].object->child->ang[1] =
            walker->by_character.mech.turret_ang;
        model_table[EWALK_GUN].object->child->pos[0] =
            EWALK_GUN_X -
            ((1.0F - njCos(walker->by_character.mech.gun_recoil)) *
             GUN_RECOIL_DEPTH);
        object = model_table[EWALK_GUN].object;
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        for (;;) {
            if (object == NULL) {
                break;
            }
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            if (object->model != NULL) {
                njCnkSimpleDrawModel(object->model);
            }
            if (object == model_table[EWALK_GUN_MUZZLE].object) {
                njCalcPoint(NULL, &pos, &pos);
                njCalcPoint(&view_to_world, &pos, &record->unk_23c);
                njCalcVector(NULL, &dir, &dir);
                njCalcVector(&view_to_world, &dir, &record->gun_dir);
                dir.y = -1.0F;
                dir.x = dir.z = 0.0F;
                njCalcVector(NULL, &dir, &dir);
                njCalcVector(&view_to_world, &dir,
                             &walker->by_character.mech.gun_down);
                /* @hack spelling: the step to the child in both arms, where
                 * one step after the test would do */
                object = object->child;
            } else {
                object = object->child;
            }
            if (object == NULL) {
                break;
            }
        }
        njSetMatrix(NULL, &cannon_matrix);
        njCnkSimpleDrawObject(model_table[EWALK_CANNON].object);
        object = model_table[EWALK_CANNON_MUZZLE].object;
        /* @hack redundant: a test that never runs, which reads object three
         * times and record once */
        if (0) {
            /* NOLINTNEXTLINE(misc-redundant-expression) */
            if (object && object && object && record) {
            }
        }
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        pos.x = pos.y = pos.z = 0.0F;
        njTranslateEx((NJS_VECTOR *)object->pos);
        njRotateEx(object->ang, 0);
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_248);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &record->cannon_dir);
        njPopMatrixEx();
        if (anim == ANIM_EGGMAN_STRIKE &&
            record->anim.mode != PLAYER_ANIM_BLEND) {
            frame = record->anim.frame;
            njPushMatrixEx();
            njSetMatrix(NULL, &upper_matrix);
            njCnkSimpleDrawMotion(
                model_table[EWALK_STRIKE_PART].object,
                motion_table[record->anim.actions[ANIM_EGGMAN_STRIKE_PART]
                                 .motion]
                    .motion,
                frame);
            njPopMatrixEx();
        }
        if (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_JETENGINE)) {
            njPushMatrixEx();
            if (model_table[EWALK_FLAME].object != NULL &&
                anim == ANIM_EGGMAN_HOVER) {
                /* @hack type: game_state's steps read as game_state_steps,
                 * where game_state.steps would do */
                /* @hack spelling: steps set inside the index, where a
                 * statement of its own would do */
                across =
                    bwalk_flicker[((steps = game_state_steps) + FLICKER_LEAD) &
                                  (FLICKER_STEPS - 1)];
                along = bwalk_flicker[steps & (FLICKER_STEPS - 1)];
                draw_control_clear(DRAW_CONTROL_SHADOW);
                njFogDisable();
                object = model_table[EWALK_FLAME].object->child;
                njSetMatrix(NULL, &nozzle1_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                njSetMatrix(NULL, &nozzle0_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                light = light_selected[record->slot];
                if (light != LIGHT_UNK_1 && light != LIGHT_VOLUME) {
                    draw_control_set(DRAW_CONTROL_SHADOW);
                }
            }
            njPopMatrixEx();
        }
        if (walker->by_character.mech.laser_frame > 0.0F) {
            motion = motion_table[MOTION_EGGMAN_LASER].motion;
            /* @hack spelling: frame set inside the test, where a statement
             * of its own would do */
            if ((frame = walker->by_character.mech.laser_frame) >
                (Float)(motion->nbFrame - 1)) {
                frame = (Float)(motion->nbFrame - 1);
            }
            njPushMatrixEx();
            njSetMatrix(NULL, &upper_matrix);
            njCnkSimpleDrawMotion(model_table[EWALK_LASER].object,
                                  motion_table[MOTION_EGGMAN_LASER].motion,
                                  frame);
            njPopMatrixEx();
        }
        if (walker->by_character.mech.glow > 0.0F) {
            draw_control_save();
            draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            draw_attr_set(0, NJD_FST_UA);
            draw_attr_clear(0, NJD_FST_FL);
            constant_material_set(walker->by_character.mech.glow, 1.0F, 1.0F,
                                  1.0F);
            njPushMatrixEx();
            njSetMatrix(NULL, &cockpit_matrix);
            njCnkSimpleDrawObject(model_table[EWALK_GLOW].object);
            njSetMatrix(NULL, &view_to_world);
            njMultiMatrix(NULL, &cockpit_matrix);
            njCalcPoints(NULL, bwalk_missile_points,
                         walker->by_character.mech.missile_points,
                         PLAYER_MISSILE_POINTS);
            njCalcVector(NULL, &bwalk_missile_dir,
                         &walker->by_character.mech.missile_dir);
            njPopMatrixEx();
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_control_restore();
            draw_attr_clear(0, NJD_FST_UA);
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    ewalk_draw_shadows(work, record, anim);
    if (game_state.unk_0b == 1) {
        walker_gauge_draw(record->slot);
    }
}

/* the motion callback of the Tails walker's draw, as ewalk_motion_callback is
 * the Eggman walker's */
static void twalk_motion_callback(NJS_CNK_OBJECT *object)
{
    struct player *walker = walker_drawn;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = pos.y = pos.z = 0.0F;
    if (object == model_table[TWALK_UPPER].object) {
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        /* @hack spelling: unk_230 read through RECORD_OF, where walker's own
         * would do */
        njCalcPoint(&view_to_world, &pos, &RECORD_OF(walker)->unk_230);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2cc);
        njGetMatrix(&upper_matrix);
    } else if (object == model_table[TWALK_LEANS].object) {
        /* @hack spelling: work, a local for the walker's player_work, where
         * the table read in place would do */
        struct task_work *work = player_work[walker->slot];

        if (work->mode == PLAYER_MODE_RUN) {
            njRotateX(NULL, walker->lean);
        }
    } else if (object == model_table[TWALK_COCKPIT].object) {
        /* @hack spelling: walker_drawn read again, where walker would do */
        njRotateY(NULL, walker_drawn->by_character.mech.body_turn);
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_218);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2b4);
        njGetMatrix(&cockpit_matrix);
        pos.x = -TWALK_LASER_OUT;
        pos.y = pos.z = 0.0F;
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->laser_origin);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->laser_dir);
    } else if (object == model_table[TWALK_FOOT_0].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_200);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_29c);
    } else if (object == model_table[TWALK_FOOT_1].object) {
        dir.y = -1.0F;
        dir.x = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_20c);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_2a8);
    } else if (object == model_table[TWALK_CANNON_MUZZLE].object) {
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_248);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->cannon_dir);
    } else if (object == model_table[TWALK_AIM].object) {
        dir.x = -AIM_LENGTH;
        dir.y = dir.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_254);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->aim);
    } else if (object == model_table[TWALK_AT_1E8].object) {
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &walker->unk_1e8);
        dir.x = 1.0F;
        dir.y = dir.z = 0.0F;
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &walker->unk_26c);
    }
    if (object == model_table[TWALK_TURRET].object) {
        njGetMatrix(&turret_matrix);
    }
    if (object == model_table[TWALK_CANNON_MOUNT].object) {
        njGetMatrix(&cannon_matrix);
    } else if (object == model_table[TWALK_NOZZLE0].object) {
        njGetMatrix(&nozzle0_matrix);
        dir.x = -1.0F;
        dir.z = dir.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        /* @hack spelling: walker_drawn read again for the nozzles, where
         * walker would do */
        njCalcPoint(&view_to_world, &pos,
                    &walker_drawn->by_character.mech.nozzle0_pos);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.nozzle0_dir);
    } else if (object == model_table[TWALK_NOZZLE1].object) {
        njGetMatrix(&nozzle1_matrix);
        dir.x = -1.0F;
        dir.z = dir.y = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos,
                    &walker_drawn->by_character.mech.nozzle1_pos);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir,
                     &walker_drawn->by_character.mech.nozzle1_dir);
    }
}

/* the Tails walker's draw, as ewalk_draw is the Eggman walker's */
static void twalk_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct player *walker = task->extra.p;
    struct player *record;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *part;
    struct collision_shape *shape;
    int anim;
    Float frame;
    Float across;
    Float along;
    Uint32 steps;
    /* @hack spelling: draw, a local for njCnkSimpleDrawObject, where calls
     * by name would do */
    void (*draw)(NJS_CNK_OBJECT *object);
    NJS_VECTOR dir;
    NJS_POINT3 pos;

    record = RECORD_OF(walker);
    /* @hack type: game_state's solo_player read as game_state_solo_player here
     * and below, where game_state.solo_player would do */
    if (game_state_solo_player != 0 &&
        game_state_solo_player != record->slot + 1) {
        return;
    }
    draw_attr_save();
    draw_attr_clear(0, NJD_FST_IS);
    if (light_selected[record->slot] != LIGHT_UNK_1 &&
        light_selected[record->slot] != LIGHT_VOLUME) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    light_apply_player(record->slot);
    walker_drawn = walker;
    njGetMatrix(&view_to_world);
    njInvertMatrix(&view_to_world);
    njCnkSetMotionCallback(twalk_motion_callback);
    anim = record->anim.current;
    /* @hack redundant: a test of the timer at 0 or more, which always
     * holds */
    if ((work->timer >= 0 && !(work->timer & PLAYER_TIMER_HIDDEN)) ||
        game_state_solo_player != 0) {
        njSetTexture(walker->by_character.mech.texlist);
        njPushMatrixEx();
        if (work->mode != PLAYER_MODE_HANG) {
            njTranslateEx(&work->pos);
        } else {
            shape = work->collision->shapes;
            dir.x = dir.z = 0.0F;
            dir.y = -(record->consts.height * 0.5F);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &dir, &pos);
            njPopMatrixEx();
            pos.x += shape->center.x;
            pos.y += shape->center.y;
            pos.z += shape->center.z;
            njTranslateEx(&pos);
        }
        if (game_state_solo_player == record->slot + 1) {
            njRotateY(NULL, HALF_TURN - work->ang.y);
        } else {
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, HALF_TURN - work->ang.y);
        }
        njScaleEx(&work->scl);
        if (record->anim.mode != PLAYER_ANIM_BLEND) {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                motion_table[record->anim
                                 .actions[record->anim.actions[anim].motion]
                                 .motion]
                    .motion,
                record->anim.frame);
        } else {
            njCnkSimpleDrawMotion(
                model_table[record->anim.actions[anim].model].object,
                record->anim.blend, record->anim.frame);
        }
        pos.x = pos.y = pos.z = 0.0F;
        njPushMatrixEx();
        njSetMatrix(NULL, &turret_matrix);
        if (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER)) {
            model_table[TWALK_BLASTER].object->child->evalflags &=
                ~NJD_EVAL_UNIT_ANG;
            model_table[TWALK_BLASTER].object->child->ang[1] =
                walker->by_character.mech.turret_ang;
            model_table[TWALK_BLASTER].object->child->pos[0] =
                TWALK_GUN_X -
                ((1.0F - njCos(walker->by_character.mech.gun_recoil)) *
                 GUN_RECOIL_DEPTH);
            object = model_table[TWALK_BLASTER].object;
        } else {
            model_table[TWALK_GUN].object->child->evalflags &=
                ~NJD_EVAL_UNIT_ANG;
            model_table[TWALK_GUN].object->child->ang[1] =
                walker->by_character.mech.turret_ang;
            model_table[TWALK_GUN].object->child->pos[0] =
                TWALK_GUN_X -
                ((1.0F - njCos(walker->by_character.mech.gun_recoil)) *
                 GUN_RECOIL_DEPTH);
            object = model_table[TWALK_GUN].object;
        }
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        for (;;) {
            part = model_table[TWALK_LAMP_MUZZLE].object;
            if (object == part) {
                njPushMatrixEx();
            }
            if (part != NULL && object == part->sibling) {
                draw_control_save();
                draw_control_clear(NJD_CONTROL_3D_CONSTANT_MATERIAL);
                constant_material_set(
                    PULSE_ALPHA(walker->by_character.mech.pulse), 0.0F, 0.0F,
                    0.0F);
                njFogDisable();
            }
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            if (object->model != NULL) {
                njCnkSimpleDrawModel(object->model);
            }
            if (object == model_table[TWALK_GUN_MUZZLE].object ||
                object == part) {
                njCalcPoint(NULL, &pos, &pos);
                njCalcPoint(&view_to_world, &pos, &record->unk_23c);
                njCalcVector(NULL, &dir, &dir);
                njCalcVector(&view_to_world, &dir, &record->gun_dir);
                dir.y = -1.0F;
                /* @hack spelling: the zero stores in two statements, where
                 * one chained store would do */
                dir.z = 0.0F;
                dir.x = 0.0F;
                njCalcVector(NULL, &dir, &dir);
                njCalcVector(&view_to_world, &dir,
                             &walker->by_character.mech.gun_down);
                if (object == part) {
                    object = object->sibling;
                    njPopMatrixEx();
                } else {
                    object = object->child;
                }
            } else {
                if (part != NULL && object == part->sibling) {
                    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
                    draw_control_restore();
                    njFogEnable();
                }
                object = object->child;
            }
            if (object == NULL) {
                break;
            }
        }
        njSetMatrix(NULL, &cannon_matrix);
        if (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_HYPERCANNON)) {
            object = model_table[TWALK_HYPERCANNON].object;
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            njCnkSimpleDrawModel(object->model);
            object = object->child;
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            njTranslate(
                NULL, 0.0F,
                (njCos(walker->by_character.mech.cannon_recoil) - 1.0F) *
                    TWALK_HYPERCANNON_RECOIL,
                0.0F);
            njCnkSimpleDrawModel(object->model);
            part = object->child;
        } else {
            object = model_table[TWALK_CANNON].object;
            njTranslateEx((NJS_VECTOR *)object->pos);
            njRotateEx(object->ang, 0);
            njCnkSimpleDrawModel(object->model);
            part = object->child;
        }
        dir.x = -1.0F;
        dir.y = dir.z = 0.0F;
        pos.x = pos.y = pos.z = 0.0F;
        njTranslateEx((NJS_VECTOR *)part->pos);
        njRotateEx(part->ang, 0);
        njCalcPoint(NULL, &pos, &pos);
        njCalcPoint(&view_to_world, &pos, &record->unk_248);
        njCalcVector(NULL, &dir, &dir);
        njCalcVector(&view_to_world, &dir, &record->cannon_dir);
        njPopMatrixEx();
        if (anim == ANIM_TAILS_STRIKE &&
            record->anim.mode != PLAYER_ANIM_BLEND) {
            frame = record->anim.frame * TWALK_STRIKE_PART_RATE;
            njPushMatrixEx();
            njSetMatrix(NULL, &upper_matrix);
            njCnkSimpleDrawMotion(
                model_table[TWALK_STRIKE_PART].object,
                motion_table[record->anim.actions[ANIM_TAILS_STRIKE_PART]
                                 .motion]
                    .motion,
                frame);
            njPopMatrixEx();
        }
        draw = njCnkSimpleDrawObject;
        if (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_BOOSTER)) {
            njPushMatrixEx();
            njSetMatrix(NULL, &nozzle0_matrix);
            draw(model_table[TWALK_BOOSTER0].object);
            njSetMatrix(NULL, &nozzle1_matrix);
            draw(model_table[TWALK_BOOSTER1].object);
            if (model_table[TWALK_FLAME].object != NULL &&
                anim == ANIM_TAILS_HOVER) {
                /* @hack type: game_state's steps read as game_state_steps,
                 * where game_state.steps would do */
                /* @hack spelling: steps set inside the index, where a
                 * statement of its own would do */
                across =
                    twalk_flicker[((steps = game_state_steps) + FLICKER_LEAD) &
                                  (FLICKER_STEPS - 1)];
                along = twalk_flicker[steps & (FLICKER_STEPS - 1)];
                draw_control_clear(DRAW_CONTROL_SHADOW);
                object = model_table[TWALK_FLAME].object->child;
                njSetMatrix(NULL, &nozzle1_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                njSetMatrix(NULL, &nozzle0_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                object = object->sibling;
                njSetMatrix(NULL, &nozzle1_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                njSetMatrix(NULL, &nozzle0_matrix);
                njTranslateEx((NJS_VECTOR *)object->pos);
                njRotateEx(object->ang, 0);
                njScale(NULL, across, along, 1.0F);
                njCnkEasyDrawModel(object->model);
                if (light_selected[record->slot] != LIGHT_UNK_1 &&
                    light_selected[record->slot] != LIGHT_VOLUME) {
                    draw_control_set(DRAW_CONTROL_SHADOW);
                }
            }
            njPopMatrixEx();
        }
        if (walker->by_character.mech.glow > 0.0F) {
            draw_control_save();
            draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            draw_attr_set(0, NJD_FST_UA);
            draw_attr_clear(0, NJD_FST_FL);
            constant_material_set(walker->by_character.mech.glow, 1.0F, 1.0F,
                                  1.0F);
            njPushMatrixEx();
            njSetMatrix(NULL, &cockpit_matrix);
            draw(model_table[TWALK_GLOW].object);
            njSetMatrix(NULL, &view_to_world);
            njMultiMatrix(NULL, &cockpit_matrix);
            njCalcPoints(NULL, twalk_missile_points,
                         walker->by_character.mech.missile_points,
                         PLAYER_MISSILE_POINTS);
            njCalcVector(NULL, &twalk_missile_dir,
                         &walker->by_character.mech.missile_dir);
            njPopMatrixEx();
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_control_restore();
            draw_attr_clear(0, NJD_FST_UA);
        }
        njPopMatrixEx();
    }
    njCnkSetMotionCallback(NULL);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    draw_attr_restore();
    light_apply(light_after_draw);
    twalk_draw_shadows(work, record, anim);
    if (game_state.unk_0b == 1) {
        walker_gauge_draw(record->slot);
    }
}

/* the Tails walker's shadows, as cwalk_draw_shadows draws the Chao walker's */
static void twalk_draw_shadows(struct task_work *work, struct player *record,
                               int anim)
{
    int model = record->anim.actions[anim].model;
    Angle unused; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    if (model == TWALK_BODY) {
        njTranslateEx(&record->unk_218);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TWALK_COCKPIT_SHADOW].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_200);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TWALK_FOOT_SHADOW_A].object);
        matrix_load_parent();
        njTranslateEx(&record->unk_20c);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njCnkModDrawObject(model_table[TWALK_FOOT_SHADOW_B].object);
        /* @hack redundant: a read of work's ang.y that nothing uses */
        unused = work->ang.y;
    }
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* PLAYER_MODE_FREE_MOVE's step, which moves the work's pos through anything.
 * the stick moves the walker down or up while X is held. otherwise it moves the
 * walker across its camera's view, or across camera_views[0]'s while
 * camera_active_count is 1. a press of B places it again by stage_place_player
 */
static void walker_free_move(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    Float x;
    Float y;
    Float sign;
    Angle ang;

    if (player_pad_records[record->slot].press & PDD_DGT_TB) {
        stage_place_player(record->slot);
    }
    if (player_pad_records[record->slot].on & PDD_DGT_TX) {
        /* @hack type: the stick's axes converted through int, where their
         * own conversion to Float would do */
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        if (x > PAD_DEAD_ZONE || x < -PAD_DEAD_ZONE || y > PAD_DEAD_ZONE ||
            y < -PAD_DEAD_ZONE) {
            if (y != 0.0F) {
                sign = y / njSqrt(y * y);
            } else {
                sign = 0.0F;
            }
            work->pos.y -= sign * FREE_MOVE_STEP;
        }
    } else {
        /* @hack type: the stick's axes converted through int, where their
         * own conversion to Float would do */
        x = (Float)(int)player_pad_records[record->slot].x1;
        y = (Float)(int)player_pad_records[record->slot].y1;
        /* @hack redundant: an else after the return */
        if (!(x > PAD_DEAD_ZONE || x < -PAD_DEAD_ZONE || y > PAD_DEAD_ZONE ||
              y < -PAD_DEAD_ZONE)) {
            return;
        } else {
            if (camera_active_count == 1) {
                ang = camera_views[0]->ang.y;
            } else {
                ang = camera_views[record->slot]->ang.y;
            }
            ang = (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) - ang;
            work->pos.x += FREE_MOVE_STEP * njCos(ang);
            work->pos.z += FREE_MOVE_STEP * njSin(ang);
        }
    }
}

/* the jumps, which walker_test_jump starts. each sets spd.y to the constants'
 * jump_speed in PLAYER_MODE_JUMP. the motion's acc takes the speed of the force
 * of the record's ground_task, where there is one. walker_jump_fast is the jump
 * at running_speed or more, which plays an animation of its own */
static void walker_jump(struct task_work *work, struct player *record)
{
    struct task_force *ground;

    if (record->ground_task != NULL) {
        ground = record->ground_task->force.p;
        if (ground != NULL) {
            player_set_acc(record->slot, ground->speed.x, ground->speed.y,
                           ground->speed.z);
        }
    }
    record->spd.y = record->consts.jump_speed;
    work->mode = PLAYER_MODE_JUMP;
    record->anim.next = ANIM_JUMP;
    work->flags &= ~WALKER_REQUEST_CLEARS;
    work->flags &= ~PLAYER_WORK_ON_SHAPE;
    record->jump_steps = 0;
    if (record->character == PLAYER_TAILS_MECH) {
        sound_play(SOUND_JUMP_TAILS, NULL, 0, 0);
        if (record->kind == PLAYER_TAILS_MECH) {
            sound_play_at(SOUND_CRY_JUMP_TAILS, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        }
    } else if (record->character == PLAYER_EGGMAN_MECH) {
        sound_play(SOUND_JUMP_EGGMAN, NULL, 0, 0);
        if (record->kind == PLAYER_EGGMAN_MECH) {
            sound_play_at(SOUND_CRY_JUMP_EGGMAN, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        }
    }
}

static void walker_jump_fast(struct task_work *work, struct player *record)
{
    struct task_force *ground;

    if (record->ground_task != NULL) {
        ground = record->ground_task->force.p;
        if (ground != NULL) {
            player_set_acc(record->slot, ground->speed.x, ground->speed.y,
                           ground->speed.z);
        }
    }
    record->spd.y = record->consts.jump_speed;
    work->mode = PLAYER_MODE_JUMP;
    if (record->character == PLAYER_EGGMAN_MECH) {
        record->anim.next = ANIM_EGGMAN_JUMP_FAST;
    } else {
        record->anim.next = ANIM_TAILS_JUMP_FAST;
    }
    work->flags &= ~WALKER_REQUEST_CLEARS;
    work->flags &= ~PLAYER_WORK_ON_SHAPE;
    record->jump_steps = 0;
    if (record->character == PLAYER_TAILS_MECH) {
        sound_play(SOUND_JUMP_TAILS, NULL, 0, 0);
        if (record->kind == PLAYER_TAILS_MECH) {
            sound_play_at(SOUND_CRY_JUMP_FAST_TAILS, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        }
    } else {
        sound_play(SOUND_JUMP_EGGMAN, NULL, 0, 0);
        if (record->kind == PLAYER_EGGMAN_MECH) {
            sound_play_at(SOUND_CRY_JUMP_FAST_EGGMAN, NULL, 0, SOUND_LOUDEST,
                          &work->pos);
        }
    }
}

/* start a jump at a press of jump, and return TRUE. it returns FALSE without
 * one */
static Bool walker_test_jump(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record)
{
    if (player_jump_pressed(work, record)) {
        if (record->spd.x >= record->consts.running_speed) {
            walker_jump_fast(work, record);
        } else {
            walker_jump(work, record);
        }
        return TRUE;
    }
    return FALSE;
}

/* answer the request that PLAYER_WORK_REQUEST flags in the work's submode,
 * and return TRUE. it returns FALSE where no request waits, or for one the
 * walker has no answer to. PLAYER_REQUEST_JOIN_PATH returns FALSE too, and
 * sets PLAYER_WORK_ON_PATH where the walker is in PLAYER_MODE_RUN, or runs
 * above running_speed. PLAYER_WORK_REQUEST stays set */
static Bool walker_take_request(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *walker)
{
    if (!(work->flags & PLAYER_WORK_REQUEST)) {
        return FALSE;
    }
    switch (work->submode) {
    case PLAYER_REQUEST_BOUNCE:
        work->mode = PLAYER_MODE_BOUNCE;
        record->anim.next = ANIM_LAUNCH;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_LAUNCH:
        work->mode = PLAYER_MODE_LAUNCH;
        record->anim.next = ANIM_LAUNCH;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_FREE_FALL:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FREE_FALL;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_UPDRAFT:
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_UNK_23;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_RUN_PATH:
        work->mode = PLAYER_MODE_PATH_RUN;
        record->anim.next = ANIM_RUN;
        record->path_distance = 0.0F;
        work->flags &= ~(PLAYER_WORK_ATTACK | PLAYER_WORK_BALL);
        work->flags |= PLAYER_WORK_ON_PATH;
        break;
    case PLAYER_REQUEST_LAUNCH_BALL:
        work->mode = PLAYER_MODE_BOUNCE;
        record->anim.next = ANIM_LAUNCH;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_ANIM:
        work->mode = PLAYER_MODE_PLAY_ANIM;
        if (record->request_arg == ANIM_UNK_86 &&
            motion_table[ANIM_UNK_86].motion == NULL) {
            record->request_arg = ANIM_STAND;
        }
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_HANG:
        work->mode = PLAYER_MODE_HANG;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_RELEASE:
        player_stand(work, motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
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
        record->anim.next = ANIM_CRASH;
        fun_8c05bce0();
        crash_stars_burst(record->slot);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_SCARE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = record->request_arg;
        switch (record->request_arg) {
        case ANIM_UNK_86:
        case ANIM_WIN:
        case ANIM_LOSE:
            walker->by_character.mech.turret_ang = 0;
            break;
        }
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_BREATHE:
        work->mode = PLAYER_MODE_UNK_35;
        record->anim.next = ANIM_BREATHE;
        record->underwater_steps = 0;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_DROWN:
        work->mode = PLAYER_MODE_DROWN;
        record->anim.next = ANIM_DROWN;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_SINK:
        work->mode = PLAYER_MODE_UNK_38;
        record->anim.next = ANIM_WALK;
        player_stop(motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        break;
    case PLAYER_REQUEST_RESET:
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        record->flags &= ~PLAYER_RESET_FLAGS;
        walker_fill_gauge(record);
        player_stand(work, motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        walker->by_character.mech.turret_ang = 0;
        break;
    case PLAYER_REQUEST_MISS:
        work->mode = PLAYER_MODE_MISS;
        record->anim.next = ANIM_MISS;
        work->flags &= ~WALKER_REQUEST_CLEARS;
        if (record->kind == PLAYER_EGGMAN_MECH) {
            voice_play_priority_2(VOICE_MISS_EGGMAN);
        } else if (record->kind == PLAYER_TAILS_MECH) {
            voice_play_priority_2(VOICE_MISS_TAILS);
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/* at a press of the action button, take an action of the record's list. take
 * its action, the current one, where the list holds it, and else the list's
 * first. action_taken records the action taken. an action with a setter here
 * starts, and the call returns TRUE. one without, as ACTION_RELEASE is, starts
 * nothing, and the call returns FALSE. without a press, or with the list
 * empty, nothing changes and the call returns FALSE */
static Bool walker_take_action(struct task_work *work,
                               struct player_motion *motion,
                               struct player *record, struct player *walker)
{
    Sint8 *actions = record->actions;
    int count;
    int action;
    int i;

    count = record->action_count;
    if (!pad_action_pressed[record->slot]) {
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
    case PLAYER_MODE_FALL:
        player_fall(work, record);
        return TRUE;
    case PLAYER_MODE_UNK_2A:
        player_call(work, record);
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
    case PLAYER_MODE_EGGMAN_MECH_STRIKE:
        walker_strike_eggman(work, motion, record, walker);
        return TRUE;
    case PLAYER_MODE_TAILS_MECH_STRIKE:
        walker_strike_tails(work, motion, record, walker);
        return TRUE;
    case PLAYER_MODE_MECH_SPECIAL_1:
        walker_special_1(work, motion, record, walker);
        return TRUE;
    case PLAYER_MODE_MECH_SPECIAL_2:
        walker_special_2(work, motion, record, walker);
        return TRUE;
    case PLAYER_MODE_MECH_LASER:
        walker_laser(work, motion, record, walker);
        return TRUE;
    }
    return FALSE;
}

/* offer the special move of the highest ring level the walker holds, and
 * return TRUE. only a walker standing or running, in a two-player game or on
 * STAGE_VERSUS_MECH_2, takes an offer. none comes while the player's
 * ring_level_timer is above 0, or game_state.cleared isn't 0. without an offer
 * the call returns FALSE */
static Bool walker_offer_special(struct task_work *work, struct player *record)
{
    if (game_state.two_players == 0 &&
        stage_current_id() != STAGE_VERSUS_MECH_2) {
        return FALSE;
    }
    if (game_state.ring_level_timer[record->slot] > 0) {
        return FALSE;
    }
    if (game_state.cleared) {
        return FALSE;
    }
    if (work->mode != PLAYER_MODE_STAND && work->mode != PLAYER_MODE_RUN) {
        return FALSE;
    }

    if (game_state.ring_levels[record->slot][RING_LEVEL_LASER] == 1) {
        OFFER(record, PLAYER_MODE_MECH_LASER);
        return TRUE;
    } else if (game_state.ring_levels[record->slot][RING_LEVEL_SPECIAL_2] ==
               1) {
        OFFER(record, PLAYER_MODE_MECH_SPECIAL_2);
        return TRUE;
    } else if (game_state.ring_levels[record->slot][RING_LEVEL_SPECIAL_1] ==
               1) {
        OFFER(record, PLAYER_MODE_MECH_SPECIAL_1);
        return TRUE;
    }
    return FALSE;
}

/* the step of a special move's task, named EWalker2PSpecialMan. it waits out
 * its steps, and its destructor then fires the move. once game_state.cleared
 * is 1, it ends without its destructor, and so without the move */
static void special_step(struct task *task)
{
    if (game_state.cleared == 1) {
        task->destructor = NULL;
        task_kill(task);
        return;
    }
    if (((struct special *)task->extra.p)->steps-- < 0) {
        task_kill(task);
        return;
    }
}

/* the destructor of the first special move's task. the walker fires a volley at
 * the other player from its gun's muzzle */
static void special_1_fire(struct task *task)
{
    struct special *special = task->extra.p;
    int slot = special->slot;
    int other = special->other;
    struct player *walker = player_records[slot];

    sound_play_for(SOUND_POWER_SHOT, player_work[slot], PLAY_PRIORITY, 0,
                   POWER_SHOT_DURATION);
    walker_volley_create(player_tasks[slot], other, &walker->gun_dir,
                         &walker->unk_23c);
    walker->anim.next = ANIM_SPECIAL_1;
    walker->anim.after = ANIM_STAND;
    vibration_start(walker->slot, 6, 30, VIBRATION_UNTIMED);
}

/* spend the first ring level on the first special move. start its task, which
 * fires the move SPECIAL_STEPS on. the walker stops, and plays ANIM_SPECIAL
 * while it waits. where task_create fails, the level is spent and nothing else
 * happens */
static void walker_special_1(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *walker)
{
    struct task *task;
    int other;

    game_state.ring_levels[record->slot][RING_LEVEL_SPECIAL_1] = 0;
    if (record->slot == 0) {
        other = 1;
    } else {
        other = 0;
    }
    /* @hack spelling: task set inside the test, where a statement of its own
     * would do */
    if ((task = task_create(TASK_EXTRA, SPECIAL_LEVEL, special_step,
                            "EWalker2PSpecialMan")) != NULL) {
        /* @hack spelling: the extra cast at each use, where a local of its
         * type would do */
        ((struct special *)task->extra.p)->slot = record->slot;
        ((struct special *)task->extra.p)->other = other;
        ((struct special *)task->extra.p)->steps = SPECIAL_STEPS;
        task->destructor = special_1_fire;
        if (record->kind == PLAYER_EGGMAN_MECH) {
            voice_play_priority_2(VOICE_SPECIAL_1_EGGMAN);
        } else if (record->kind == PLAYER_TAILS_MECH) {
            voice_play_priority_2(VOICE_SPECIAL_1_TAILS);
        }
        player_stop(motion, record);
        record->anim.next = ANIM_SPECIAL;
        record->anim.after = ANIM_STAND;
        work->flags &= ~WALKER_REQUEST_CLEARS;
    }
}

/* the destructor of the second special move's task: the walker fires it at
 * the other player in PLAYER_MODE_MECH_SPECIAL_2 */
static void special_2_fire(struct task *task)
{
    struct special *special = task->extra.p;
    int slot = special->slot;
    int other = special->other;
    struct player *record = player_records[slot];

    sound_play(SOUND_MISSILES, NULL, 0, 0);
    walker_missiles_create(player_tasks[slot], other);
    player_work[slot]->mode = PLAYER_MODE_MECH_SPECIAL_2;
    record->anim.next = ANIM_SPECIAL;
    record->anim.after = ANIM_STAND;
    vibration_start_fading(record->slot, 3, 30, 32);
}

#define SPECIAL_2_GLOW 7.0F

/* spend the second ring level on the second special move, and play the move's
 * voice. on STAGE_VERSUS_MECH_2 the voice plays at VERSUS_VOICE_PRIORITY. there
 * the call first stops each voice stream that voice_can_start lets such a voice
 * take. the call then starts the move's task, which fires the move
 * SPECIAL_STEPS on. where the task starts, the walker stops and poses. its glow
 * then becomes SPECIAL_2_GLOW, above the 1 that the second ring level fades it
 * in to. where task_create fails, the voice still plays */
static void walker_special_2(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *walker)
{
    struct task *task;
    int other;
    int i;

    game_state.ring_levels[record->slot][RING_LEVEL_SPECIAL_2] = 0;
    if (record->slot == 0) {
        other = 1;
    } else {
        other = 0;
    }
    if (stage_current_id() == STAGE_VERSUS_MECH_2) {
        for (i = 0; i < VOICES; i++) {
            if (voice_can_start(VERSUS_VOICE_PRIORITY, i) == 1) {
                voice_stop(i);
            }
        }
        switch (record->kind) {
        case PLAYER_EGGMAN_MECH:
            voice_play(VOICE_VERSUS_SPECIAL_2_EGGMAN, VERSUS_VOICE_PRIORITY);
            break;
        case PLAYER_TAILS_MECH:
            voice_play(VOICE_VERSUS_SPECIAL_2_TAILS, VERSUS_VOICE_PRIORITY);
            break;
        }
    } else if (record->kind == PLAYER_EGGMAN_MECH) {
        voice_play_priority_2(VOICE_SPECIAL_2_EGGMAN);
    } else if (record->kind == PLAYER_TAILS_MECH) {
        voice_play_priority_2(VOICE_SPECIAL_2_TAILS);
    }
    /* @hack spelling: task set inside the test, where a statement of its own
     * would do */
    if ((task = task_create(TASK_EXTRA, SPECIAL_LEVEL, special_step,
                            "EWalker2PSpecialMan")) != NULL) {
        /* @hack spelling: the extra cast at each use, where a local of its
         * type would do */
        ((struct special *)task->extra.p)->slot = record->slot;
        ((struct special *)task->extra.p)->other = other;
        ((struct special *)task->extra.p)->steps = SPECIAL_STEPS;
        task->destructor = special_2_fire;
        player_stop(motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        walker->by_character.mech.glow = SPECIAL_2_GLOW;
        if (record->character == PLAYER_EGGMAN_MECH) {
            player_pose(record->slot, ANIM_EGGMAN_POSE);
        } else if (record->character == PLAYER_TAILS_MECH) {
            player_pose(record->slot, ANIM_TAILS_POSE);
        }
    }
}

#define LASER_OPEN_FRAME 200.0F
#define LASER_OPEN_STEP 0.2F

/* the step of the laser's task, named EWalker2PLaserMan, as special_step is the
 * special moves'. it ends without its destructor too where the walker has
 * missed. while it waits, a walker of PLAYER_EGGMAN_MECH opens its laser cannon
 * toward LASER_OPEN_FRAME, by LASER_OPEN_STEP a step */
static void laser_wait_step(struct task *task)
{
    /* @hack spelling: the extra cast at each use, where a local of its type
     * would do */
    int slot = ((struct special *)task->extra.p)->slot;
    struct player *walker = player_tasks[slot]->extra.p;

    if (game_state.cleared == 1 || (walker->flags & PLAYER_MISSED)) {
        task->destructor = NULL;
        task_kill(task);
        return;
    }
    if (((struct special *)task->extra.p)->steps-- < 0) {
        task_kill(task);
        return;
    }
    if (walker->character == PLAYER_EGGMAN_MECH) {
        walker->by_character.mech.laser_frame =
            float_approach(walker->by_character.mech.laser_frame,
                           LASER_OPEN_FRAME, LASER_OPEN_STEP);
    }
}

/* the destructor of the laser's task: the walker fires it in
 * PLAYER_MODE_MECH_LASER */
static void laser_fire(struct task *task)
{
    struct special *special = task->extra.p;
    int slot = special->slot;
    struct player *record = player_records[slot];

    sound_play(SOUND_LASER, NULL, 0, 0);
    walker_laser_create(slot);
    player_work[slot]->mode = PLAYER_MODE_MECH_LASER;
    record->anim.next = ANIM_LASER;
    record->anim.after = ANIM_STAND;
    player_work[slot]->flags &= ~PLAYER_WORK_CHARGED;
    player_stop(player_motion[slot], player_records[slot]);
    vibration_start_fading(record->slot, 5, 30, 16);
}

/* spend the third ring level on the laser: start its task, which fires the
 * laser LASER_STEPS on. the walker poses, and holds PLAYER_WORK_CHARGED until
 * the laser fires */
static void walker_laser(struct task_work *work, struct player_motion *motion,
                         struct player *record, struct player *walker)
{
    struct task *task;

    game_state.ring_levels[record->slot][RING_LEVEL_LASER] = 0;
    if (record->kind == PLAYER_EGGMAN_MECH) {
        voice_play_priority_2(VOICE_LASER_EGGMAN);
    } else if (record->kind == PLAYER_TAILS_MECH) {
        voice_play_priority_2(VOICE_LASER_TAILS);
    }
    sound_play(SOUND_LASER_START, NULL, 0, 0);
    /* @hack spelling: task set inside the test, where a statement of its own
     * would do */
    if ((task = task_create(TASK_EXTRA, SPECIAL_LEVEL, laser_wait_step,
                            "EWalker2PLaserMan")) != NULL) {
        /* @hack spelling: the extra cast at each use, where a local of its
         * type would do */
        ((struct special *)task->extra.p)->slot = record->slot;
        ((struct special *)task->extra.p)->steps = LASER_STEPS;
        task->destructor = laser_fire;
        player_stop(motion, record);
        work->flags &= ~WALKER_REQUEST_CLEARS;
        walker->by_character.mech.mode_count = 0;
        work->flags |= PLAYER_WORK_CHARGED;
        if (record->character == PLAYER_EGGMAN_MECH) {
            player_pose(record->slot, ANIM_EGGMAN_POSE);
        } else if (record->character == PLAYER_TAILS_MECH) {
            player_pose(record->slot, ANIM_TAILS_POSE);
        }
    }
}

/* while the walker holds locks, offer ACTION_RELEASE and return TRUE. return
 * FALSE otherwise */
static Bool walker_offer_release(struct task_work *work, struct player *record,
                                 struct player *walker)
{
    if (walker->by_character.mech.lock_count <= 0) {
        return FALSE;
    }
    OFFER(record, ACTION_RELEASE);
    return TRUE;
}

/* the strike's reach: a point ahead of the walker's unk_218. the point lies
 * STRIKE_PLAYER_REACH ahead for the other player, and STRIKE_TARGET_REACH ahead
 * for a target. the other player or the target must lie within STRIKE_NEAR of
 * it */
#define STRIKE_PLAYER_REACH 20.0F
#define STRIKE_TARGET_REACH 30.0F
#define STRIKE_NEAR 15.0F
/* the lists walker_target_near searches after the other player: items, then
 * enemies */
#define TARGET_LISTS 2

/* TRUE where a walker that stands, runs or slides has something within its
 * strike's reach. the other player's unk_218 counts, and the center of an
 * item's or an enemy's first shape. it passes over a target without a task or a
 * work, and one whose first shape has SHAPE_ID_NO_STRIKE */
static Bool walker_target_near(struct task_work *work, struct player *record)
{
    NJS_VECTOR ahead;
    NJS_POINT3 pos;
    struct task **list;
    struct task_work *target;
    struct collision_work *collision;
    int other;
    int mode;
    int count;
    int i;
    int j;

    mode = work->mode;
    if (mode != PLAYER_MODE_STAND && mode != PLAYER_MODE_RUN &&
        mode != PLAYER_MODE_SLIDE) {
        return FALSE;
    }
    /* @hack spelling: other set inside the test, where a statement of its
     * own would do */
    if ((other = player_find_rival(record->slot)) != -1) {
        pos = player_records[other]->unk_218;
        ahead.x = 1.0F;
        ahead.y = ahead.z = 0.0F;
        turn_to_world(work, &ahead);
        ahead.x *= STRIKE_PLAYER_REACH;
        ahead.y *= STRIKE_PLAYER_REACH;
        ahead.z *= STRIKE_PLAYER_REACH;
        njSubVector(&pos, &record->unk_218);
        njSubVector(&pos, &ahead);
#ifdef NONMATCHING
        /* this side keeps what the image's frame leaves the step's wall
         * slide, as the test of a target below does. no run of the image
         * was held to this one, which only a game of two players reaches */
        player_slide_y = pos.y;
#endif
        if (njScalor(&pos) < STRIKE_NEAR) {
            return TRUE;
        }
    }
    for (i = TARGET_LISTS; i > 0; i--) {
        /* @hack spelling: the count taken in each arm, where one call after
         * them would do */
        if (i == TARGET_LISTS) {
            list = collision_items;
            count = collision_get_last_count(COLLISION_COUNT_ITEMS);
        } else {
            list = collision_enemies;
            count = collision_get_last_count(COLLISION_COUNT_ENEMIES);
        }
        for (j = count; j > 0; j--) {
            collision = (*list++)->work->collision;
            /* @hack type: the shape's id read as a Uint8, where its Sint8
             * would do */
            /* @hack spelling: target set inside the test, where a statement
             * of its own would do */
            if (collision == NULL || collision->owner == NULL ||
                (target = collision->owner->work) == NULL ||
                (Uint8)collision->shapes->id == SHAPE_ID_NO_STRIKE) {
                continue;
            }
            collision_get_shape_center(target, collision->shapes, &pos);
            ahead.x = 1.0F;
            ahead.y = ahead.z = 0.0F;
            turn_to_world(work, &ahead);
            ahead.x *= STRIKE_TARGET_REACH;
            ahead.y *= STRIKE_TARGET_REACH;
            ahead.z *= STRIKE_TARGET_REACH;
            njSubVector(&pos, &record->unk_218);
            njSubVector(&pos, &ahead);
#ifdef NONMATCHING
            /* this side keeps what the image's frame leaves the step's wall
             * slide: under walker_choose_mode's call, pos.y lies in the word
             * that player_slide_y stands for. it is how far the last target
             * tested lies above the strike's reach */
            player_slide_y = pos.y;
#endif
            if (njScalor(&pos) < STRIKE_NEAR) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* where walker_target_near is TRUE, offer the strike of the walker's character
 * and return TRUE. return FALSE otherwise */
static Bool walker_offer_strike(struct task_work *work, struct player *record)
{
    if (walker_target_near(work, record) == TRUE) {
        if (record->character == PLAYER_EGGMAN_MECH) {
            OFFER(record, PLAYER_MODE_EGGMAN_MECH_STRIKE);
        } else {
            OFFER(record, PLAYER_MODE_TAILS_MECH_STRIKE);
        }
        return TRUE;
    }
    return FALSE;
}

/* a strike's vibration_start_fading power, freq and length */
#define STRIKE_VIBRATION_POWER 3
#define STRIKE_VIBRATION_FREQ 30
#define STRIKE_VIBRATION_LENGTH 32

/* the strikes, which walker_take_action starts. each stops the walker in its
 * mode, PLAYER_MODE_EGGMAN_MECH_STRIKE or PLAYER_MODE_TAILS_MECH_STRIKE, under
 * PLAYER_WORK_ATTACK. in either mode walker_set_shapes turns SHAPE_STRIKE on,
 * at the record's unk_1e8 */
static void walker_strike_eggman(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record, struct player *walker)
{
    work->mode = PLAYER_MODE_EGGMAN_MECH_STRIKE;
    record->anim.next = ANIM_EGGMAN_STRIKE;
    record->anim.after = ANIM_STAND;
    work->flags |= PLAYER_WORK_ATTACK;
    player_stop(motion, record);
    vibration_start_fading(record->slot, STRIKE_VIBRATION_POWER,
                           STRIKE_VIBRATION_FREQ, STRIKE_VIBRATION_LENGTH);
    sound_play(SOUND_STRIKE_EGGMAN, NULL, 0, 0);
    if (record->kind == PLAYER_EGGMAN_MECH) {
        sound_play_at(SOUND_CRY_STRIKE_EGGMAN, NULL, 0, SOUND_LOUDEST,
                      &work->pos);
    }
}

static void walker_strike_tails(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *walker)
{
    work->mode = PLAYER_MODE_TAILS_MECH_STRIKE;
    record->anim.next = ANIM_TAILS_STRIKE;
    record->anim.after = ANIM_STAND;
    work->flags |= PLAYER_WORK_ATTACK;
    player_stop(motion, record);
    vibration_start_fading(record->slot, STRIKE_VIBRATION_POWER,
                           STRIKE_VIBRATION_FREQ, STRIKE_VIBRATION_LENGTH);
    sound_play(SOUND_STRIKE_TAILS, NULL, 0, 0);
    if (record->kind == PLAYER_TAILS_MECH) {
        sound_play_at(SOUND_CRY_STRIKE_TAILS, NULL, 0, SOUND_LOUDEST,
                      &work->pos);
    }
}

/* set the walker's shapes for this step's collision checks. the body's and the
 * middle's attack and guard are INVINCIBLE_LEVEL under PLAYER_INVINCIBLE, and 0
 * otherwise. the middle lies above the work's pos, halfway up to unk_218.
 * SHAPE_TOP is on, SHAPE_STRIKE only in a strike's mode, and the guard off. a
 * rival in PLAYER_MODE_MECH_LASER gives its body SHAPE_ID_NO_LOCKON, so that no
 * lock-on takes it. the collision record's radius becomes WALKER_RADIUS. a
 * work without a collision record changes nothing */
static void walker_set_shapes(struct task *task, struct task_work *work,
                              struct player *record)
{
    struct collision_shape *shapes;
    int i;
    int attack;
    int guard;

    if (work->collision == NULL) {
        return;
    }
    shapes = work->collision->shapes;
    /* @hack spelling: the shapes read through the work's record each time
     * round, where shapes[i] would do */
    for (i = SHAPE_TOP; i < WALKER_SHAPES; i++) {
        work->collision->shapes[i].flags |= SHAPE_OFF;
    }
    /* @hack redundant: an arm for PLAYER_WORK_ATTACK that does what the
     * last arm does */
    if (record->flags & PLAYER_INVINCIBLE) {
        attack = INVINCIBLE_LEVEL;
        guard = INVINCIBLE_LEVEL;
    } else if (work->flags & PLAYER_WORK_ATTACK) {
        attack = 0;
        guard = 0;
    } else {
        attack = 0;
        guard = 0;
    }
    shapes[SHAPE_BODY].levels =
        (shapes[SHAPE_BODY].levels & ~SHAPE_ATTACK) | (attack & SHAPE_ATTACK);
    shapes[SHAPE_BODY].levels = (shapes[SHAPE_BODY].levels & ~SHAPE_GUARD) |
                                ((guard << SHAPE_GUARD_SHIFT) & SHAPE_GUARD);
    if (record->flags & PLAYER_INVINCIBLE) {
        attack = INVINCIBLE_LEVEL;
        guard = INVINCIBLE_LEVEL;
    } else {
        attack = 0;
        guard = 0;
    }
    shapes[SHAPE_MIDDLE].levels =
        (shapes[SHAPE_MIDDLE].levels & ~SHAPE_ATTACK) | (attack & SHAPE_ATTACK);
    shapes[SHAPE_MIDDLE].levels = (shapes[SHAPE_MIDDLE].levels & ~SHAPE_GUARD) |
                                  ((guard << SHAPE_GUARD_SHIFT) & SHAPE_GUARD);
    shapes[SHAPE_MIDDLE].center.x = work->pos.x;
    shapes[SHAPE_MIDDLE].center.y = (work->pos.y + record->unk_218.y) * 0.5F;
    shapes[SHAPE_MIDDLE].center.z = work->pos.z;
    shapes[SHAPE_TOP].flags &= ~SHAPE_OFF;
    if (work->mode == PLAYER_MODE_MECH_LASER && work->id == WORK_ID_RIVAL) {
        shapes[SHAPE_BODY].id = SHAPE_ID_NO_LOCKON;
    } else {
        shapes[SHAPE_BODY].id = 0;
    }
    if (work->mode == PLAYER_MODE_EGGMAN_MECH_STRIKE) {
        shapes[SHAPE_STRIKE].flags &= ~SHAPE_OFF;
        shapes[SHAPE_STRIKE].center = record->unk_1e8;
    }
    if (work->mode == PLAYER_MODE_TAILS_MECH_STRIKE) {
        shapes[SHAPE_STRIKE].flags &= ~SHAPE_OFF;
        shapes[SHAPE_STRIKE].center = record->unk_1e8;
    }
    collision_set_radius(task, WALKER_RADIUS);
}

/* unless the next animation is ANIM_TURN, pick the run's animation by spd.x,
 * and let its pace follow spd.x */
static void walker_run_anim(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    Float speed = record->spd.x;

    if (record->anim.next == ANIM_TURN) {
        return;
    }
    if (speed > record->consts.full_speed) {
        record->anim.next = ANIM_FULL_SPEED;
    } else if (speed > record->consts.running_speed) {
        record->anim.next = ANIM_RUN;
    } else {
        record->anim.next = ANIM_WALK;
    }
    record->anim_speed = record->spd.x;
}

/* the points a unit of a score counts */
#define SCORE_STEP_POINTS 100

/* the score task's destructor. a task killed with its score above 0 still adds
 * the score's points by technique_score_start */
static void lock_score_destroy(struct task *task)
{
    struct lock_score *score = task->extra.p;

    if (score->score > 0 && task->step == task_destroy) {
        technique_score_start(score->score * SCORE_STEP_POINTS);
        score->score = 0;
    }
}

/* move n, which holds a rung of the score's ladder, one rung up. the rungs are
 * -1, 0, 1, 2, 3, 4, 5, 6, 8, 10, 15 and 20. 20 stays 20 */
#define SCORE_STEP_UP(n)                                                       \
    {                                                                          \
        if ((n) <= -1) {                                                       \
            (n) = 0;                                                           \
        } else if ((n) <= 0) {                                                 \
            (n) = 1;                                                           \
        } else if ((n) <= 1) {                                                 \
            (n) = 2;                                                           \
        } else if ((n) <= 2) {                                                 \
            (n) = 3;                                                           \
        } else if ((n) <= 3) {                                                 \
            (n) = 4;                                                           \
        } else if ((n) <= 4) {                                                 \
            (n) = 5;                                                           \
        } else if ((n) <= 5) {                                                 \
            (n) = 6;                                                           \
        } else if ((n) <= 6) {                                                 \
            (n) = 8;                                                           \
        } else if ((n) <= 8) {                                                 \
            (n) = 10;                                                          \
        } else if ((n) <= 10) {                                                \
            (n) = 15;                                                          \
        } else if ((n) <= 15) {                                                \
            (n) = 20;                                                          \
        } else {                                                               \
            (n) = 20;                                                          \
        }                                                                      \
    }

/* take out of the score's list each lock whose task ends, or whose walker's
 * task ends. each hit of such a lock raises the score by SCORE_STEP_UP */
static void lock_score_collect(struct lock_score *score)
{
    struct task *node;
    struct lock *lock;
    struct lock *prev;
    struct lock *next;

    for (node = score->locks; node != NULL; node = lock->next) {
        lock = node->extra.p;
        if (node->step == task_destroy ||
            (node->parent != NULL && node->parent->step == task_destroy)) {
            if (lock->prev == NULL) {
                score->locks = lock->next;
                prev = NULL;
            } else {
                prev = lock->prev->extra.p;
            }
            if (lock->next == NULL) {
                next = NULL;
            } else {
                next = lock->next->extra.p;
            }
            if (prev != NULL) {
                prev->next = lock->next;
            }
            if (next != NULL) {
                next->prev = lock->prev;
            }
            while (lock->hits > 0) {
                SCORE_STEP_UP(score->unk_08);
                SCORE_STEP_UP(score->score);
                score->unk_0c = 0.5F;
                lock->hits--;
            }
            if (lock->counted == 1) {
                SCORE_STEP_UP(score->unk_08);
                if (score->locks == NULL) {
                    score->unk_0c = 1.0F;
                } else {
                    score->unk_0c = 0.5F;
                }
            }
        }
    }
}

/* unlink the score's locks from each other, and empty the list and the score */
static void lock_score_unlink(struct lock_score *score)
{
    struct task *node = score->locks;

    if (node != NULL) {
        struct lock *lock;

        do {
            lock = node->extra.p;
            lock->prev = NULL;
            node = lock->next;
            lock->next = NULL;
        } while (node != NULL);
    }

    score->score = 0;
    score->locks = NULL;
}

/* on a step with the walker's release_shot, fire the list's nearest lock that
 * hasn't fired. its distance is from the walker's pos. the call then clears
 * release_shot. a step without a lock to fire leaves release_shot set */
static void lock_score_fire(struct lock_score *score)
{
    struct lock *nearest = NULL;
    struct task *node = score->locks;
    NJS_POINT3 *pos = &player_work[score->slot]->pos;
    struct player *walker = player_tasks[score->slot]->extra.p;

    struct lock *lock;
    Float nearest_distance;
    Float distance;

    if (walker->by_character.mech.release_shot == 0) {
        return;
    }
    while (node != NULL) {
        lock = node->extra.p;
        if (lock->fired != 1) {
            distance = njDistanceP2P(pos, &lock->pos);
            if (nearest == NULL) {
                nearest = lock;
                nearest_distance = distance;
            } else if (nearest_distance > distance) {
                nearest = lock;
                nearest_distance = distance;
            }
        }
        node = lock->next;
    }
    if (nearest != NULL) {
        nearest->fired = 1;
        walker->by_character.mech.release_shot = 0;
    }
}

/* the least score each of a score's voices needs, and Tails's and Eggman's
 * voice for it */
#define SCORE_GREAT 15
#define SCORE_GOOD 5
#define SCORE_SOME 1
#define VOICE_SCORE_SOME_EGGMAN 0x672
#define VOICE_SCORE_SOME_TAILS 0x673
#define VOICE_SCORE_GOOD_EGGMAN 0x674
#define VOICE_SCORE_GOOD_TAILS 0x675
#define VOICE_SCORE_GREAT_EGGMAN 0x676
#define VOICE_SCORE_GREAT_TAILS 0x677
/* the score task waits SCORE_WAIT steps after its last lock ends */
#define SCORE_WAIT 60
/* once no lock has been left for UNK_0C_FADE_STEPS steps, unk_0c eases to 0 by
 * UNK_0C_FADE a step. while locks fly and unk_08 is above 0, it eases to 1 by
 * UNK_0C_RISE */
#define UNK_0C_FADE_STEPS 40
#define UNK_0C_FADE 0.05F
#define UNK_0C_RISE 0.1F
#define NO_VOICE (-1)

/* the step of the score task, named WalkerTimeSubtractingTask. while locks fly,
 * it collects those that end, and fires the next by lock_score_fire. it ends
 * without a score where the walker has missed, or is in PLAYER_MODE_HURT or
 * PLAYER_MODE_MISS. once no lock is left, it counts its steps. it ends on the
 * step that finds them past SCORE_WAIT. a score above 0 then adds its points by
 * technique_score_start, with the voice the score reached */
static void lock_score_step(struct task *task)
{
    struct lock_score *score = task->extra.p;
    struct task *first = score->locks;
    struct task_work *work;
    int voice;
    int mode;

    if (first == NULL) {
        if (score->steps >= UNK_0C_FADE_STEPS) {
            score->unk_0c = float_approach(score->unk_0c, 0.0F, UNK_0C_FADE);
        }
        if (score->steps++ > SCORE_WAIT) {
            if (score->score > 0) {
                voice = NO_VOICE;
                technique_score_start(score->score * SCORE_STEP_POINTS);
                if (player_records[score->slot]->kind == PLAYER_TAILS_MECH) {
                    if (score->score >= SCORE_GREAT) {
                        voice = VOICE_SCORE_GREAT_TAILS;
                    } else if (score->score >= SCORE_GOOD) {
                        voice = VOICE_SCORE_GOOD_TAILS;
                    } else if (score->score >= SCORE_SOME) {
                        voice = VOICE_SCORE_SOME_TAILS;
                    }
                } else if (player_records[score->slot]->kind ==
                           PLAYER_EGGMAN_MECH) {
                    if (score->score >= SCORE_GREAT) {
                        voice = VOICE_SCORE_GREAT_EGGMAN;
                    } else if (score->score >= SCORE_GOOD) {
                        voice = VOICE_SCORE_GOOD_EGGMAN;
                    } else if (score->score >= SCORE_SOME) {
                        voice = VOICE_SCORE_SOME_EGGMAN;
                    }
                }
                if (voice != NO_VOICE) {
                    voice_play_priority_2(voice);
                }
                score->score = 0;
            }
            task_kill(task);
        }
    } else {
        work = player_work[score->slot];
        mode = work->mode;
        if ((player_records[score->slot]->flags & PLAYER_MISSED) ||
            mode == PLAYER_MODE_HURT || mode == PLAYER_MODE_MISS) {
            lock_score_collect(score);
            lock_score_unlink(score);
            score->score = 0;
            task_kill(task);
            return;
        }
        if (score->unk_08 > 0) {
            score->unk_0c = float_approach(score->unk_0c, 1.0F, UNK_0C_RISE);
        }
        lock_score_collect(score);
        lock_score_fire(score);
    }
}

/* start a score task for the locks of the walker in slot, listed from locks.
 * each lock becomes released, and counted where lock_target_live holds for its
 * target. the score starts where SCORE_STEP_UP takes -1 once for each counted
 * lock. where task_create or syCalloc fails, no task is left and the locks stay
 * as they were. count goes unread */
static void lock_score_start(int slot, int count, struct task *locks)
{
    struct task *task;
    struct lock_score *score;
    struct task *node;
    struct lock *lock;
    int counted;

    task = task_create(TASK_WORK, SCORE_LEVEL, lock_score_step,
                       "WalkerTimeSubtractingTask");
    if (task == NULL) {
        return;
    }
    score = syCalloc(1, sizeof(struct lock_score));
    if (score == NULL) {
        task_destroy(task);
        return;
    }
    task->extra.p = score;
    score->slot = slot;
    score->locks = locks;
    lock_score_collect(score);
    counted = 0;
    for (node = locks; node != NULL; node = lock->next) {
        lock = node->extra.p;
        if (lock->target != NULL && lock_target_live(lock->target)) {
            counted++;
            lock->counted = 1;
        }
        lock->released = 1;
        node->destructor = lock_released_destroy;
    }
    switch (counted) {
    case 0:
        score->score = -1;
        break;
    case 1:
        score->score = 0;
        break;
    case 2:
        score->score = 1;
        break;
    case 3:
        score->score = 2;
        break;
    case 4:
        score->score = 3;
        break;
    case 5:
        score->score = 4;
        break;
    case 6:
        score->score = 5;
        break;
    case 7:
        score->score = 6;
        break;
    case 8:
        score->score = 8;
        break;
    case 9:
        score->score = 10;
        break;
    case 10:
        score->score = 15;
        break;
    case 11:
    default:
        score->score = 20;
        break;
    }
    task->destructor = lock_score_destroy;
}

/* add the lock of task at the end of the walker's list. where it is the
 * first, the walker's first_target_name and first_target_step take its
 * target's name and step */
static void lock_list_add(struct task *task, struct player *walker)
{
    struct lock *lock;
    struct task *node = walker->by_character.mech.locks;

    if (node == NULL) {
        walker->by_character.mech.locks = task;
        lock = task->extra.p;
        lock->prev = NULL;
        lock->next = NULL;
        walker->by_character.mech.first_target_name = lock->target->name;
        walker->by_character.mech.first_target_step = lock->target->step;
        return;
    }
    for (; node != NULL; node = lock->next) {
        lock = node->extra.p;
        if (lock->next == NULL) {
            lock->next = task;
            lock = task->extra.p;
            lock->prev = node;
            lock->next = NULL;
            return;
        }
    }
}

/* take the lock of task out of the walker's list. a lock may become the first
 * of it. where that lock holds a target, first_target_name and
 * first_target_step take the target's name and step */
static void lock_list_remove(struct task *task, struct player *walker)
{
    struct lock *lock;
    struct task *node = walker->by_character.mech.locks;
    struct task *next;
    struct task *prev;

    if (node == task) {
        lock = task->extra.p;
        if (lock->next == NULL) {
            walker->by_character.mech.locks = NULL;
            lock->prev = NULL;
            lock->next = NULL;
            return;
        }
        next = lock->next;
        lock->prev = NULL;
        lock->next = NULL;
        walker->by_character.mech.locks = next;
        lock = next->extra.p;
        lock->prev = NULL;
        if (lock->target != NULL) {
            walker->by_character.mech.first_target_name = lock->target->name;
            walker->by_character.mech.first_target_step = lock->target->step;
        }
        return;
    }
    for (; node != NULL; node = lock->next) {
        lock = node->extra.p;
        if (node == task) {
            next = lock->next;
            prev = lock->prev;
            lock->prev = NULL;
            lock->next = NULL;
            if (next != NULL) {
                lock = next->extra.p;
                lock->prev = prev;
            }
            if (prev != NULL) {
                lock = prev->extra.p;
                lock->next = next;
            }
            return;
        }
    }
}

void lockon_release(struct enemy_work *enemy)
{
    struct lock *lock;

    if (enemy->lock_task != NULL) {
        lock = enemy->lock_task->extra.p;
        if (lock != NULL && lock->target != NULL) {
            if (lock->released == 1 && lock->target->placement != NULL) {
                placement_stop(lock->target);
            }
            lock->target = NULL;
        }
    }
    enemy->lock_task = NULL;
}

void lockon_count_blast(struct task_work *work, struct enemy_work *enemy)
{
    struct collision_work *collision;
    struct task *task;
    struct task_work *attack;
    struct lock *lock;

    if (enemy->lock_task != NULL) {
        return;
    }
    collision = work->collision;
    if (collision == NULL || collision->own_shape == -1) {
        return;
    }
    if (collision->shapes[collision->own_shape].flags & SHAPE_UNK_800000) {
        return;
    }
    if (collision->hit == NULL) {
        return;
    }
    collision_restart_scans();
    /* @hack spelling: task and attack each set inside its test, where
     * statements of their own would do */
    if ((task = collision_next_attack(collision->owner)) != NULL) {
        if ((attack = task->work) != NULL) {
            if (attack->id == WORK_ID_LOCKON_BLAST) {
                lock = task->extra.p;
                lock->hits++;
            }
        }
    }
}

/* a blast's parts take BLAST_ALPHA_FULL at a fade below BLAST_FADE_FULL, and
 * BLAST_ALPHA_FAINT above BLAST_FADE_FAINT. between the two they fade evenly */
#define BLAST_FADE_FULL 0.5F
#define BLAST_FADE_FAINT 0.82F
#define BLAST_ALPHA_FULL 255
#define BLAST_ALPHA_FAINT 48
#define BLAST_ALPHA_SLOPE                                                      \
    ((BLAST_ALPHA_FULL - BLAST_ALPHA_FAINT) /                                  \
     (BLAST_FADE_FAINT - BLAST_FADE_FULL))
/* bit 24, the low bit of a vertex color's alpha byte. it marks a vertex that
 * takes the whole alpha */
#define BLAST_FULL_ALPHA 0x01000000U
/* a part's vertex chunk, as the draw reads it. a header word comes first, then
 * a word whose high half counts the vertices. then come VERTEX_WORDS words a
 * vertex. a vertex's color is its word VERTEX_COLOR, whose top byte is its
 * alpha */
#define CHUNK_HEADER_WORDS 1
#define VERTEX_COUNT(word) (((word) & 0xffff0000) >> 16)
#define VERTEX_WORDS 4
#define VERTEX_COLOR 3
#define COLOR_ALPHA 0xff000000
#define COLOR_RGB 0x00ffffff
/* each walker's parts in walker_blast_parts, Eggman's first */
#define BLAST_PARTS 4
/* a part draws with its radius PART_CLIP_MARGIN beyond its size */
#define PART_CLIP_MARGIN 10.0F

/* draw one part of a blast, scaled to size. part picks one of the BLAST_PARTS
 * of character's walker, counted round them. its vertex colors take the alpha
 * that fade gives, or half of it for a vertex without BLAST_FULL_ALPHA. the
 * draw writes the colors into the model */
static void lock_blast_draw_part(Float size, Float fade, int part,
                                 int character)
{
    NJS_CNK_MODEL *model;
    Uint32 *vertex;
    Uint32 alpha;
    Uint32 half;
    Uint32 color;
    int count;
    Float radius;
    Float scale;
    Float drawn_radius;

    part &= BLAST_PARTS - 1;
    if (character == PLAYER_TAILS_MECH) {
        part += BLAST_PARTS;
    }
    model = walker_blast_parts[part];
#ifdef NONMATCHING
    /* Each part scales by its original radius. Empty art has no drawable
     * extent. */
    if (model == NULL || model->r <= 0.0F) {
        return;
    }
#endif
    if (fade > BLAST_FADE_FAINT) {
        alpha = BLAST_ALPHA_FAINT;
    } else if (fade < BLAST_FADE_FULL) {
        alpha = BLAST_ALPHA_FULL;
    } else {
        alpha =
            ((BLAST_FADE_FAINT - fade) * BLAST_ALPHA_SLOPE) + BLAST_ALPHA_FAINT;
    }
    alpha <<= ALPHA_SHIFT;
    vertex = (Uint32 *)model->vlist + CHUNK_HEADER_WORDS;
    count = VERTEX_COUNT(*vertex++);
    half = (alpha >> 1) & (COLOR_ALPHA & ~BLAST_FULL_ALPHA);
    for (; count > 0; count--, vertex += VERTEX_WORDS) {
        color = vertex[VERTEX_COLOR];
        if (color & COLOR_ALPHA) {
            if (color & BLAST_FULL_ALPHA) {
#ifdef NONMATCHING
                /* this side repairs the tag below: it sets BLAST_FULL_ALPHA
                 * where the image sets blue's low bit */
                color = (color & COLOR_RGB) | alpha | BLAST_FULL_ALPHA;
#else
                /* @bug logic: sets blue's low bit in place of
                 * BLAST_FULL_ALPHA, at 0x8c096054. an even alpha then clears
                 * the vertex's mark, and later draws give it half */
                color = (color & COLOR_RGB) | alpha | 1;
#endif
            } else {
                color = (color & COLOR_RGB) | half;
            }
            vertex[VERTEX_COLOR] = color;
        }
    }
    radius = model->r;
    /* @hack spelling: drawn_radius, a local for the radius the model draws
     * with, where the sum stored at once would do */
    drawn_radius = size + PART_CLIP_MARGIN;
    model->r = drawn_radius;
    scale = size / radius;
    njPushMatrixEx();
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(model);
    njPopMatrixEx();
    model->r = radius;
}

/* the draw of a blast, which a lock's shot becomes. it draws six parts about
 * the work's pos, each at a turn and a size of its own. scl.x sizes them, and
 * scl.x over scl.y fades them. the work's tick, the shot's steps in flight,
 * names the first part. the others are the parts 1, 2, 4, 5 and 6 on from it */
static void lock_blast_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct lock *lock = task->extra.p;
    int character = player_records[lock->slot]->character;
    Float size = work->scl.x;
    Float fade = work->scl.x / work->scl.y;

    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (player_hooks.draw_blast_ring != NULL) {
        player_hooks.draw_blast_ring(size, fade);
    }
    njPushMatrixEx();
    njRotateX(NULL, 0x1000);
    njRotateZ(NULL, 0xe800);
    njRotateY(NULL, 0xe000);
    lock_blast_draw_part(size + 5.0F, fade, work->tick, character);
    matrix_load_parent();
    njRotateX(NULL, 0x2000);
    njRotateZ(NULL, 0x3000);
    njRotateY(NULL, 0xe000);
    lock_blast_draw_part(size, fade, work->tick + 1, character);
    matrix_load_parent();
    njRotateY(NULL, 0x8000);
    njRotateX(NULL, 0xe000);
    njRotateZ(NULL, 0x1000);
    njRotateY(NULL, 0xe000);
    lock_blast_draw_part(size - 6.0F, fade, work->tick + 2, character);
    matrix_load_parent();
    njRotateX(NULL, -0x4000);
    njRotateZ(NULL, 0xc000);
    njRotateY(NULL, 0xe000);
    lock_blast_draw_part(size - 15.0F, fade, work->tick + 4, character);
    matrix_load_parent();
    njRotateY(NULL, 0x1000);
    njRotateX(NULL, 0x2800);
    njRotateZ(NULL, 0xe000);
    njRotateY(NULL, 0xe000);
    lock_blast_draw_part(size - 15.0F, fade, work->tick + 5, character);
    matrix_load_parent();
    njRotateY(NULL, 0x4000);
    njRotateX(NULL, 0x4000);
    njRotateZ(NULL, 0x3000);
    njRotateY(NULL, 0xe000);
    lock_blast_draw_part(size + 5.0F, fade, work->tick + 6, character);
    njPopMatrix(2);
    njFogEnable();
}

/* the sprite cell of a shot in flight, by the walker's upgrade */
#define SHOT_CELL_PLAIN 0
#define SHOT_CELL_TAILS_UPGRADE 1
#define SHOT_CELL_EGGMAN_UPGRADE 2

/* the draw of a lock's shot in flight. it draws the shot's sprite at the work's
 * pos, then the lock's marker by lock_draw */
static void lock_shot_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct lock *lock = task->extra.p;
    struct player *walker = player_tasks[lock->slot]->extra.p;
    int cell;

    njFogDisable();
    if (walker->by_character.mech.effects_texlist != NULL) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(walker->by_character.mech.effects_texlist);
        walker_shot_sprite->p = work->pos;
        walker_shot_sprite->tlist = walker->by_character.mech.effects_texlist;
        if (player_records[lock->slot]->upgrades &
            (UPGRADE_BIT(UPGRADE_EWALKER_POWERGUN) |
             UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER))) {
            if (player_records[lock->slot]->character == PLAYER_EGGMAN_MECH) {
                cell = SHOT_CELL_EGGMAN_UPGRADE;
            } else {
                cell = SHOT_CELL_TAILS_UPGRADE;
            }
        } else {
            cell = SHOT_CELL_PLAIN;
        }
        njDrawSprite3D(walker_shot_sprite, cell,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    lock_draw(task);
    njFogEnable();
}

/* how far, squared, a lock's shot or blast may lie from both players. it ends
 * once it lies farther. outside a demo, it ends only once it lies that far
 * from the camera as well */
#define LOCK_RANGE 250000.0F

#define BLAST_GROWTH 3.0F

/* the step of a blast. its scl.x grows toward its scl.y by BLAST_GROWTH a step,
 * and its shape with it. the shape joins each step's collision checks. the
 * blast ends once full grown, or once placement_try_leave_by finds it too far.
 * it ends too where the walker's task holds no record, and in the game's first
 * two steps */
static void lock_blast_step(struct task *task)
{
    struct lock *lock = task->extra.p;
    struct player *walker = player_tasks[lock->slot]->extra.p;
    struct task_work *work = task->work;

    if (walker == NULL || game_state.steps <= 1) {
        task_kill(task);
        return;
    }
    work->scl.x = float_approach(work->scl.x, work->scl.y, BLAST_GROWTH);
    work->collision->shapes->radius = work->scl.x;
    work->collision->radius = work->scl.x;
    /* @hack spelling: two tests that each kill the task, where one joined by
     * || would do */
    if (placement_try_leave_by(task, LOCK_RANGE)) {
        task_kill(task);
    } else if (work->scl.x >= work->scl.y) {
        task_kill(task);
    } else {
        collision_register(task);
        work->timer++;
    }
}

/* a shot's speed. a versus game is a two-player game, or one where player 1
 * is a rival. a shot flies at SHOT_SPEED_VERSUS in one, and at SHOT_SPEED
 * otherwise */
#define SHOT_SPEED 5.0F
#define SHOT_SPEED_VERSUS 1.7F
/* a shot steers its way by a weight: its steps in flight, up to SHOT_STEER.
 * the new way is the way to the target times the weight, plus the old way
 * times SHOT_STEER less the weight. in a versus game the weight stops at
 * SHOT_STEER_VERSUS. from then on it is 0 where the target lies more than
 * SHOT_TURN_LIMIT off the old way. the shot then flies straight on. a shot
 * headed straight away from its target first swerves along the walker's
 * gun_down by SHOT_SWERVE */
#define SHOT_STEER 30.0F
#define SHOT_STEER_VERSUS 15.0F
#define SHOT_TURN_LIMIT ((Angle)(5 * ANGLE_PER_DEGREE))
#define SHOT_SWERVE 0.1F
/* how near its target's center a shot reaches it */
#define SHOT_REACH 4.0F
/* a shot that reaches its target becomes a blast BLAST_START across, which
 * grows to BLAST_SIZE */
#define BLAST_START 2.0F
#define BLAST_SIZE 50.0F
#define SHOT_SAFE_STEPS 4

/* the step of a lock's shot in flight. it steers its way, the work's scl,
 * toward the target's center, and moves along it. the work's timer counts its
 * steps in flight.
 *
 * the shot ends at once, and draws no more:
 * - in the game's first two steps;
 * - where the walker's task holds no record, or the lock holds no target;
 * - where the target's task, or its parent, ends.
 * it ends too where placement_try_leave_by finds it past LOCK_RANGE. it ends
 * where it touched the record of a task other than its walker's.
 *
 * within SHOT_REACH of the target's center, the shot hits, and the target takes
 * WORK_HURT. at a target of WORK_ID_LOCKON_GUARD, the shot ends as a
 * ricochet. at any other target, it becomes a blast where the walker has
 * UPGRADE_EWALKER_POWERGUN or UPGRADE_TWALKER_LAZERBALSTER. it ends where the
 * walker has neither. a target that isn't a player keeps the walker's slot in
 * its shot_by. it adds a hit to the lock where it has ENEMY_UNK_800.
 *
 * only a shot at a player joins the collision checks, once its timer passes
 * SHOT_SAFE_STEPS. so nothing touches a shot at an item or an enemy */
static void lock_shot_step(struct task *task)
{
    NJS_VECTOR way;
    NJS_POINT3 from;
    struct lock *lock = task->extra.p;
    struct player *walker = player_tasks[lock->slot]->extra.p;
    struct task_work *work = task->work;
    struct task_work *target;
    struct enemy_work *enemy;
    struct collision_work *collision;
    Float length;
    Float turn;
    Float speed;

    if (walker == NULL || lock->target == NULL || game_state.steps <= 1 ||
        lock->target->step == task_destroy ||
        (lock->target->parent != NULL &&
         lock->target->parent->step == task_destroy)) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    target = lock->target->work;
    collision_get_shape_center(target, target->collision->shapes, &lock->pos);
    way = lock->pos;
    njSubVector(&way, &work->pos);
    /* @hack spelling: length, a local for the way's length, where the call
     * tested in place would do */
    length = njScalor(&way);
    if (length == 0.0F) {
        way = work->scl;
    } else {
        njUnitVector(&way);
        if (njInnerProduct(&way, &work->scl) == -1.0F) {
            way.x += walker->by_character.mech.gun_down.x * SHOT_SWERVE;
            way.y += walker->by_character.mech.gun_down.y * SHOT_SWERVE;
            way.z += walker->by_character.mech.gun_down.z * SHOT_SWERVE;
        }
        turn = work->timer;
        if (game_state.two_players != 0 ||
            (player_work[1] != NULL && player_work[1]->id == WORK_ID_RIVAL)) {
            if (turn > SHOT_STEER_VERSUS) {
                turn = SHOT_STEER_VERSUS;
                if (njInnerProduct(&way, &work->scl) < njCos(SHOT_TURN_LIMIT)) {
                    turn = 0.0F;
                }
            }
        } else if (turn > SHOT_STEER) {
            turn = SHOT_STEER;
        }
        way.x = (way.x * turn) + (work->scl.x * (SHOT_STEER - turn));
        way.y = (way.y * turn) + (work->scl.y * (SHOT_STEER - turn));
        way.z = (way.z * turn) + (work->scl.z * (SHOT_STEER - turn));
        njUnitVector(&way);
        work->scl = way;
    }
    if (game_state.two_players != 0 ||
        (player_work[1] != NULL && player_work[1]->id == WORK_ID_RIVAL)) {
        speed = SHOT_SPEED_VERSUS;
    } else {
        speed = SHOT_SPEED;
    }
    way.x *= speed;
    way.y *= speed;
    way.z *= speed;
    from = work->pos;
    njAddVector(&work->pos, &way);
    walker_tracer_create(lock->slot, &from, &work->pos);
    if (placement_try_leave_by(task, LOCK_RANGE)) {
        task_kill(task);
        return;
    }
    collision = work->collision;
    if ((collision->flags & COLLISION_TOUCHED) &&
        collision->hit->owner != task->parent) {
        task_kill(task);
    } else if (njDistanceP2P(&lock->pos, &work->pos) < SHOT_REACH) {
        if (lock->target != NULL &&
            lock->target->work->id == WORK_ID_LOCKON_GUARD) {
            walker_ricochet_create(lock->slot, &work->pos, &work->scl);
            task_kill(task);
            lock->target->work->flags |= WORK_HURT;
            return;
        }
        if (player_records[lock->slot]->upgrades &
            (UPGRADE_BIT(UPGRADE_EWALKER_POWERGUN) |
             UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER))) {
            work->id = WORK_ID_LOCKON_BLAST;
            work->scl.x = BLAST_START;
            work->scl.y = BLAST_SIZE;
            work->tick = work->timer;
            if (game_state.two_players == 0) {
                work->collision->shapes->flags |= SHAPE_IGNORE_PLAYERS;
            } else {
                work->collision->shapes->flags &= ~SHAPE_IGNORE_PLAYERS;
            }
            task->step = lock_blast_step;
            task->draw = lock_blast_draw;
            collision_register(task);
        } else {
            task_kill(task);
        }
        if (lock->target != NULL) {
            lock->target->work->flags |= WORK_HURT;
            if (collision_get_kind(lock->target) != COLLISION_PLAYER) {
                enemy = lock->target->motion.p;
                enemy->shot_by = lock->slot;
                if (enemy->flags & ENEMY_UNK_800) {
                    lock->hits++;
                }
            }
        }
    } else {
        if (work->timer > SHOT_SAFE_STEPS &&
            collision_get_kind(lock->target) == COLLISION_PLAYER) {
            collision_register(task);
        }
        work->timer++;
    }
}

/* the destructor of a lock not yet released. the walker's lock_count drops by
 * one, and its turret_aim empties where it is the lock's pos. the target's
 * lock_task, or the other walker's locked_by, empties. the lock leaves the
 * walker's list */
static void lock_destroy(struct task *task)
{
    struct lock *lock = task->extra.p;
    struct player *walker;
    struct player *target_walker;
    struct enemy_work *enemy;

    if (player_tasks[lock->slot] == NULL) {
        return;
    }
    walker = player_tasks[lock->slot]->extra.p;
    walker->by_character.mech.lock_count--;
    if (walker->by_character.mech.turret_aim == &lock->pos) {
        walker->by_character.mech.turret_aim = NULL;
    }
    if (lock->target != NULL) {
        if (collision_get_kind(lock->target) == COLLISION_PLAYER) {
            target_walker = lock->target->extra.p;
            target_walker->by_character.mech.locked_by = NULL;
        } else {
            enemy = lock->target->motion.p;
            enemy->lock_task = NULL;
        }
    }
    lock_list_remove(task, walker);
}

/* the destructor of a released lock, as lock_destroy is the others'. here
 * shot_count drops by one in place of lock_count. the lock stays in its list,
 * which the score task holds */
static void lock_released_destroy(struct task *task)
{
    struct lock *lock = task->extra.p;
    struct player *walker;
    struct player *target_walker;
    struct enemy_work *enemy;

    if (player_tasks[lock->slot] == NULL) {
        return;
    }
    walker = player_tasks[lock->slot]->extra.p;
    walker->by_character.mech.shot_count--;
    if (walker->by_character.mech.turret_aim == &lock->pos) {
        walker->by_character.mech.turret_aim = NULL;
    }
    if (lock->target != NULL) {
        if (collision_get_kind(lock->target) == COLLISION_PLAYER) {
            target_walker = lock->target->extra.p;
            target_walker->by_character.mech.locked_by = NULL;
        } else {
            enemy = lock->target->motion.p;
            enemy->lock_task = NULL;
        }
    }
}

/* the game's first steps, in which no lock's marker draws */
#define MARKER_WAIT 5

/* the marker's count: a backdrop MARKER_BACKDROP_X right of the marker, then
 * digits from MARKER_DIGIT_X right of the backdrop. the digits lie
 * MARKER_DIGIT_WIDTH apart. each sprite's depth is MARKER_DEPTH_SCALE of the
 * marker's z, less MARKER_DEPTH_BIAS */
#define MARKER_BACKDROP_X 32.0F
#define MARKER_DIGIT_X 16.0F
#define MARKER_DIGIT_WIDTH 24.0F
#define MARKER_DEPTH_SCALE 0.0001F
#define MARKER_DEPTH_BIAS 3.0F
/* the marker's material, as constant_material_set takes it: MARKER_ALPHA, and
 * MARKER_TINT in red, green and blue. the last live lock's marker takes
 * -MARKER_TINT in green and blue, which tints it red */
#define MARKER_ALPHA 1.0F
#define MARKER_TINT 2.0F
/* the cells of walker_lock_cells. a draw sets the digit cell's texture to
 * each digit it draws */
#define LOCK_CELL_MARKER 0
#define LOCK_CELL_BACKDROP 1
#define LOCK_CELL_DIGIT 2

/* the draw of a lock: its marker on the screen at its pos, sized by its grow.
 * where its target is live, the marker counts the lock's place among the locks
 * with live targets. the count starts from 1 at the list's first, and the last
 * of them is tinted red. nothing draws while game_state.unk_0b is 0, or in the
 * game's first MARKER_WAIT steps. nor does anything draw for a lock whose task
 * ends, behind the view, or without the walker's effects_texlist */
static void lock_draw(struct task *task)
{
    struct task *target;
    NJS_POINT2 screen;
    struct lock *lock = task->extra.p;
    struct player *walker = player_tasks[lock->slot]->extra.p;
    struct lock *node;
    NJS_SPRITE *sprite;
    int count;

    if (game_state.unk_0b == 0) {
        return;
    }
    if (task->step == task_destroy) {
        return;
    }
    if (game_state.steps < MARKER_WAIT) {
        return;
    }
    if (walker->by_character.mech.effects_texlist == NULL) {
        return;
    }
    sprite = &walker_lock_marker;
    njCalcPoint(NULL, &lock->pos, &sprite->p);
    if (sprite->p.z < 0.0F) {
        return;
    }
    if (task->draw == lock_draw) {
        njFogDisable();
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(walker->by_character.mech.effects_texlist);
    njProjectScreen(NULL, &lock->pos, &screen);
    sprite->tlist = walker->by_character.mech.effects_texlist;
    sprite->p.x = screen.x;
    sprite->p.y = screen.y;
    sprite->sx = sprite->sy =
        walker_lock_scales[lock->grow & (LOCK_SCALES - 1)];
    njDrawSprite2D(sprite, LOCK_CELL_MARKER,
                   (sprite->p.z * MARKER_DEPTH_SCALE) - MARKER_DEPTH_BIAS,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* @hack spelling: target set inside the test, where a statement of its
     * own would do */
    if ((target = lock->target) != NULL && lock_target_live(target)) {
        sprite->p.x += MARKER_BACKDROP_X;
        njDrawSprite2D(sprite, LOCK_CELL_BACKDROP,
                       (sprite->p.z * MARKER_DEPTH_SCALE) - MARKER_DEPTH_BIAS,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
        sprite->tlist = &console_texlist;
        sprite->p.x += MARKER_DIGIT_X;
        count = 0;
        node = lock;
        while (node->next != NULL) {
            node = node->next->extra.p;
            if (node == NULL) {
                break;
            }
            target = node->target;
            if (target != NULL && lock_target_live(target)) {
                count++;
            }
        }
        if (count == 0) {
            constant_material_set(MARKER_ALPHA, MARKER_TINT, -MARKER_TINT,
                                  -MARKER_TINT);
        } else {
            constant_material_set(MARKER_ALPHA, MARKER_TINT, MARKER_TINT,
                                  MARKER_TINT);
        }
        count = 1;
        /* @hack redundant: a test that never runs, which reads screen's x
         * and y with 2 and 0.0001, and task twice */
        if (0) {
            /* NOLINTNEXTLINE(misc-redundant-expression) */
            if ((screen.x * 2.0F) + (screen.y * 0.0001F) && task && task) {
            }
        }
        while (lock->prev != NULL) {
            lock = lock->prev->extra.p;
            if (lock == NULL) {
                break;
            }
            target = lock->target;
            if (target != NULL && lock_target_live(target)) {
                count++;
            }
        }
        if (count > 9) {
            walker_lock_cells[LOCK_CELL_DIGIT].texid = count / 10;
            njDrawSprite2D(sprite, LOCK_CELL_DIGIT,
                           (sprite->p.z * MARKER_DEPTH_SCALE) -
                               MARKER_DEPTH_BIAS,
                           NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
            sprite->p.x += MARKER_DIGIT_WIDTH;
            count = count % 10;
        }
        walker_lock_cells[LOCK_CELL_DIGIT].texid = count;
        njDrawSprite2D(sprite, LOCK_CELL_DIGIT,
                       (sprite->p.z * MARKER_DEPTH_SCALE) - MARKER_DEPTH_BIAS,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    }
    if (task->draw == lock_draw) {
        njFogEnable();
    }
}

/* a fired lock's puff of smoke starts PUFF_BACK behind the gun's muzzle. its
 * speed is the shot's way backward, plus PUFF_GRAVITY times world_gravity. for
 * a character of PLAYER_TAILS_MECH, it is that way less PUFF_GRAVITY times
 * world_gravity */
#define PUFF_BACK 10.0F
#define PUFF_GRAVITY 0.1F

/* the step of a lock that waits on its target, as walker_start_lockon starts
 * it. its marker grows to LOCK_GROWN. until the score task fires the lock, its
 * pos follows the target's center. an empty turret_aim takes that pos.
 *
 * once fired, the lock becomes a shot, and flies on in lock_shot_step. the shot
 * leaves the gun's muzzle, the walker's unk_23c, along gun_dir. it takes
 * walker_lock_shot_shape.
 *
 * the lock ends where it holds no target, or the target's task or its parent
 * ends. it ends too in the game's first two steps, and where the walker's task
 * holds no record. where lock_destroy is then the task's destructor, the step
 * calls it at once, and clears the destructor. a released lock ends where the
 * walker's has_locks is 0. one not released ends where the walker's aiming is 0
 */
static void lock_step(struct task *task)
{
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    struct lock *lock = task->extra.p;
    struct player *walker = player_tasks[lock->slot]->extra.p;
    struct task_work *work = task->work;
    struct task_work *target;
    Float back;

    if (walker == NULL || lock->target == NULL || game_state.steps <= 1 ||
        lock->target->step == task_destroy ||
        (lock->target->parent != NULL &&
         lock->target->parent->step == task_destroy)) {
        task_kill(task);
        if (task->destructor == lock_destroy) {
            lock_destroy(task);
            task->destructor = NULL;
        }
        task->draw = NULL;
        return;
    }
    target = lock->target->work;
    if (lock->grow < LOCK_GROWN) {
        lock->grow++;
    }
    if (lock->fired == 1) {
        work->pos = walker->unk_23c;
        work->scl = walker->gun_dir;
        collision_init(task, &walker_lock_shot_shape, 1, COLLISION_ATTACK);
        task->step = lock_shot_step;
        task->draw = lock_shot_draw;
        if (walker->upgrades & (UPGRADE_BIT(UPGRADE_EWALKER_POWERGUN) |
                                UPGRADE_BIT(UPGRADE_TWALKER_LAZERBALSTER))) {
            sound_play(SOUND_POWER_SHOT, NULL, 0, 0);
        } else {
            sound_play(SOUND_SHOT, NULL, 0, 0);
        }
        /* @hack spelling: back, a local for -1, where -1.0F multiplied in
         * place would do */
        back = -1.0F;
        if (walker->character == PLAYER_TAILS_MECH) {
            spd.x = (work->scl.x * back) - (world_gravity.x * PUFF_GRAVITY);
            spd.y = (work->scl.y * back) - (world_gravity.y * PUFF_GRAVITY);
            spd.z = (work->scl.z * back) - (world_gravity.z * PUFF_GRAVITY);
        } else {
            spd.x = (work->scl.x * back) + (world_gravity.x * PUFF_GRAVITY);
            spd.y = (work->scl.y * back) + (world_gravity.y * PUFF_GRAVITY);
            spd.z = (work->scl.z * back) + (world_gravity.z * PUFF_GRAVITY);
        }
        pos = work->pos;
        pos.x -= work->scl.x * PUFF_BACK;
        pos.y -= work->scl.y * PUFF_BACK;
        pos.z -= work->scl.z * PUFF_BACK;
        walker_puff_create(lock->slot, &spd, &pos);
        walker->by_character.mech.gun_recoil = GUN_RECOIL;
        vibration_start(lock->slot, 3, 25, VIBRATION_UNTIMED);
        return;
    }
    /* @hack spelling: two tests that each kill the task, where one joined by
     * || would do */
    if (lock->released == 1 && walker->by_character.mech.has_locks == 0) {
        task_kill(task);
        return;
    }
    if (lock->released == 0 && walker->by_character.mech.aiming == 0) {
        task_kill(task);
        return;
    }
    collision_get_shape_center(target, target->collision->shapes, &lock->pos);
    if (walker->by_character.mech.turret_aim == NULL) {
        walker->by_character.mech.turret_aim = &lock->pos;
    }
}

/* TRUE where task is a live target. its work must have a collision record,
 * whose first shape lacks SHAPE_UNK_800000. its work's id mustn't be
 * WORK_ID_LOCKON_GUARD. a release counts only the locks of live targets, and
 * only they show a number */
static Bool lock_target_live(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *collision;

    if (work == NULL) {
        return FALSE;
    }
    collision = work->collision;
    /* @hack spelling: task's work read again for its id, where work would
     * do */
    if (collision == NULL || (collision->shapes->flags & SHAPE_UNK_800000) ||
        task->work->id == WORK_ID_LOCKON_GUARD) {
        return FALSE;
    }
    return TRUE;
}

/* the lock-on's reach from the aim's start. the _SQ are squared distances, and
 * the _ANGLE angles off the aim's heading about y. the _LINE are distances from
 * the aim's line */
#define LOCKON_REACH_SQ 160000.0F
#define LOCKON_NEAR_SQ 2500.0F
#define LOCKON_PLAYER_REACH_SQ 250000.0F
#define LOCKON_NEAR_ANGLE ((Angle)(30 * ANGLE_PER_DEGREE))
#define LOCKON_FAR_ANGLE ((Angle)(12 * ANGLE_PER_DEGREE))
#define LOCKON_PLAYER_ANGLE ((Angle)(6 * ANGLE_PER_DEGREE))
#define LOCKON_NEAR_LINE 30.0F
#define LOCKON_FAR_LINE 60.0F
#define LOCKON_PLAYER_LINE 30.0F
#define LOCKON_AIM_RADIUS 0.2F /* the aim's radius, as a capsule */
/* a distance farther than any target's, from which the searches start */
#define LOCKON_NONE 100000000.0F

/* the squared distance from the aim's start, the walker's unk_254, to the
 * center of collision's first shape. it is 0 where the walker can't lock on to
 * the shape. within LOCKON_NEAR_SQ, the center must lie within
 * LOCKON_NEAR_ANGLE and LOCKON_NEAR_LINE of the aim. out to LOCKON_REACH_SQ, it
 * must lie within LOCKON_FAR_ANGLE and LOCKON_FAR_LINE. a record whose task is
 * null, or has no work, gives 0 */
static Float lockon_target_distance(struct collision_work *collision,
                                    struct player *walker)
{
    Angle heading;
    NJS_VECTOR way;
    NJS_LINE line;
    NJS_POINT3 pos;
    struct task_work *work;
    Float distance_sq;
    Angle angle;
    Float distance;

    if (collision->owner == NULL) {
        return 0.0F;
    }
    if (collision->owner->work == NULL) {
        return 0.0F;
    }
    /* @hack spelling: work, a local for the owner's work, where it read in
     * place would do */
    work = collision->owner->work;
    collision_get_shape_center(work, collision->shapes, &pos);
    way = pos;
    njSubVector(&way, &walker->unk_254);
    distance_sq = njScalor2(&way);
    if (distance_sq > LOCKON_REACH_SQ) {
        return 0.0F;
    }
    heading = (Angle)(atan2f(walker->aim.z, walker->aim.x) * ANGLE_PER_RADIAN);
    angle = angle_distance(heading,
                           (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN));
    line.px = walker->unk_254.x;
    line.py = walker->unk_254.y;
    line.pz = walker->unk_254.z;
    line.vx = walker->aim.x;
    line.vy = walker->aim.y;
    line.vz = walker->aim.z;
#ifdef NONMATCHING
    /* this side keeps what the image's frame leaves the step's wall slide:
     * under lockon_find_nearest's call, line.vz lies in the word that
     * player_slide_y stands for */
    player_slide_y = line.vz;
#endif
    distance = njDistanceP2L(&pos, &line, NULL);
    if (distance_sq <= LOCKON_NEAR_SQ) {
        if (angle > LOCKON_NEAR_ANGLE) {
            return 0.0F;
        }
        if (distance > LOCKON_NEAR_LINE) {
            return 0.0F;
        }
    } else {
        if (angle > LOCKON_FAR_ANGLE) {
            return 0.0F;
        }
        if (distance > LOCKON_FAR_LINE) {
            return 0.0F;
        }
    }
    return distance_sq;
}

/* lockon_target_distance for a WORK_ID_STOP_LOCKON_BOX task, whose box lies
 * about the work's pos with half sizes scl. the shape's center must lie within
 * LOCKON_REACH_SQ. it must lie within LOCKON_FAR_LINE of the aim's line too, or
 * within scl's length where longer. the aim, a capsule of LOCKON_AIM_RADIUS
 * from unk_254 along the record's aim, must meet the box */
static Float lockon_box_distance(struct collision_work *collision,
                                 struct player *walker)
{
    NJS_POINT3 pos;
    struct task_work *work;
    Float distance_sq;
    Float distance;
    Float reach;

    if (collision->owner == NULL) {
        return 0.0F;
    }
    if (collision->owner->work == NULL) {
        return 0.0F;
    }
    work = collision->owner->work;
    collision_get_shape_center(work, collision->shapes, &pos);
#ifdef NONMATCHING
    /* this side keeps what the image's frame leaves the step's wall slide:
     * under lockon_find_nearest's call, pos.x lies in the word that
     * player_slide_y stands for. the frames alone give this one: no run of
     * the image was held to it */
    player_slide_y = pos.x;
#endif
    /* @hack spelling: way, line and the box each in a block of their own,
     * where locals of the function would do */
    {
        NJS_VECTOR way;

        way = pos;
        njSubVector(&way, &walker->unk_254);
        distance_sq = njScalor2(&way);
    }
    if (distance_sq > LOCKON_REACH_SQ) {
        return 0.0F;
    }
    {
        NJS_LINE line;

        line.px = walker->unk_254.x;
        line.py = walker->unk_254.y;
        line.pz = walker->unk_254.z;
        line.vx = walker->aim.x;
        line.vy = walker->aim.y;
        line.vz = walker->aim.z;
        distance = njDistanceP2L(&pos, &line, NULL);
    }
    reach = njScalor(&work->scl);
    if (reach < LOCKON_FAR_LINE) {
        reach = LOCKON_FAR_LINE;
    }
    if (distance > reach) {
        return 0.0F;
    }
    {
        NJS_BOX box;
        NJS_CAPSULE aim;

        box.v[0].x = work->pos.x - work->scl.x;
        box.v[0].y = work->pos.y + work->scl.y;
        box.v[0].z = work->pos.z + work->scl.z;
        box.v[1].x = work->pos.x - work->scl.x;
        box.v[1].y = work->pos.y + work->scl.y;
        box.v[1].z = work->pos.z - work->scl.z;
        box.v[2].x = work->pos.x + work->scl.x;
        box.v[2].y = work->pos.y + work->scl.y;
        box.v[2].z = work->pos.z - work->scl.z;
        box.v[3].x = work->pos.x + work->scl.x;
        box.v[3].y = work->pos.y + work->scl.y;
        box.v[3].z = work->pos.z + work->scl.z;
        box.v[4].x = work->pos.x - work->scl.x;
        box.v[4].y = work->pos.y - work->scl.y;
        box.v[4].z = work->pos.z + work->scl.z;
        box.v[5].x = work->pos.x - work->scl.x;
        box.v[5].y = work->pos.y - work->scl.y;
        box.v[5].z = work->pos.z - work->scl.z;
        box.v[6].x = work->pos.x + work->scl.x;
        box.v[6].y = work->pos.y - work->scl.y;
        box.v[6].z = work->pos.z - work->scl.z;
        box.v[7].x = work->pos.x + work->scl.x;
        box.v[7].y = work->pos.y - work->scl.y;
        box.v[7].z = work->pos.z + work->scl.z;
        aim.c1.x = walker->unk_254.x;
        aim.c1.y = walker->unk_254.y;
        aim.c1.z = walker->unk_254.z;
        aim.c2.x = walker->unk_254.x + walker->aim.x;
        aim.c2.y = walker->unk_254.y + walker->aim.y;
        aim.c2.z = walker->unk_254.z + walker->aim.z;
        aim.r = LOCKON_AIM_RADIUS;
        if (njCollisionCheckBC(&box, &aim) == 0) {
            return 0.0F;
        }
    }
    return distance_sq;
}

/* lockon_target_distance for the other player: beyond LOCKON_NEAR_SQ, within
 * LOCKON_PLAYER_REACH_SQ, LOCKON_PLAYER_ANGLE and LOCKON_PLAYER_LINE */
static Float lockon_player_distance(struct collision_work *collision,
                                    struct player *walker)
{
    Angle heading;
    NJS_VECTOR way;
    NJS_LINE line;
    NJS_POINT3 pos;
    struct task_work *work;
    Float distance_sq;
    Angle angle;
    Float distance;

    if (collision->owner == NULL) {
        return 0.0F;
    }
    if (collision->owner->work == NULL) {
        return 0.0F;
    }
    work = collision->owner->work;
    collision_get_shape_center(work, collision->shapes, &pos);
    way = pos;
    njSubVector(&way, &walker->unk_254);
    distance_sq = njScalor2(&way);
    if (distance_sq > LOCKON_PLAYER_REACH_SQ) {
        return 0.0F;
    }
    heading = (Angle)(atan2f(walker->aim.z, walker->aim.x) * ANGLE_PER_RADIAN);
    angle = angle_distance(heading,
                           (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN));
    /* @hack redundant: an else after the return */
    if (distance_sq <= LOCKON_NEAR_SQ) {
        return 0.0F;
    } else if (angle > LOCKON_PLAYER_ANGLE) {
        return 0.0F;
    }
    line.px = walker->unk_254.x;
    line.py = walker->unk_254.y;
    line.pz = walker->unk_254.z;
    line.vx = walker->aim.x;
    line.vy = walker->aim.y;
    line.vz = walker->aim.z;
#ifdef NONMATCHING
    /* this side keeps what the image's frame leaves the step's wall slide:
     * under lockon_find_player's call, line.py lies in the word that
     * player_slide_y stands for. the frames alone give this one: no run of
     * the image was held to it */
    player_slide_y = line.py;
#endif
    distance = njDistanceP2L(&pos, &line, NULL);
    if (distance > LOCKON_PLAYER_LINE) {
        return 0.0F;
    }
    return distance_sq;
}

/* the nearest of the count tasks of list that the walker can lock on to.
 * *distance gets its squared distance. without one, the call returns null, and
 * *distance gets LOCKON_NONE. it passes over a task that ends or whose parent
 * ends. it takes no task without an enemy_work in its motion, none a lock holds
 * and none under ENEMY_HIT. it takes none whose first shape has
 * SHAPE_ID_NO_LOCKON, all of SHAPE_GUARD or SHAPE_OFF */
static struct task *lockon_find_nearest(struct player *walker,
                                        struct task **list, int count,
                                        Float *distance)
{
    struct task *nearest;
    Float nearest_distance;
    struct task_work *work;
    struct collision_work *collision;
    struct task *task;
    struct enemy_work *enemy;
    Float found;
    int i;

    nearest_distance = LOCKON_NONE;
    nearest = NULL;
    for (i = count; i > 0; i--) {
        work = (*list++)->work;
        collision = work->collision;
        if (work->id == WORK_ID_STOP_LOCKON_BOX) {
            found = lockon_box_distance(collision, walker);
        } else {
            found = lockon_target_distance(collision, walker);
        }
        if (found > 0.0F) {
            task = collision->owner;
            enemy = task->motion.p;
            /* @hack spelling: each test an if of its own, where one joined
             * by || would do */
            if (task->step == task_destroy || task->step == NULL) {
                continue;
            }
            if (task->parent != NULL && (task->parent->step == task_destroy ||
                                         task->parent->step == NULL)) {
                continue;
            }
            if (enemy == NULL) {
                continue;
            }
            if (enemy->lock_task != NULL) {
                continue;
            }
            /* @hack type: the shape's id read as a Uint8, where its Sint8
             * would do */
            if ((Uint8)collision->shapes->id == SHAPE_ID_NO_LOCKON) {
                continue;
            }
            if ((collision->shapes->levels & SHAPE_GUARD) == SHAPE_GUARD) {
                continue;
            }
            if (collision->shapes->flags & SHAPE_OFF) {
                continue;
            }
            if (enemy->flags & ENEMY_HIT) {
                continue;
            }
            if (nearest_distance > found) {
                nearest = task;
                nearest_distance = found;
            }
        }
    }
    *distance = nearest_distance;
    return nearest;
}

/* task, the other player's, where the walker can lock on to it. *distance gets
 * its squared distance. where it can't, the call returns null, and *distance
 * gets LOCKON_NONE. the other player must be a walker that no lock holds, and
 * neither missed nor invincible. its task mustn't end. its body must have
 * neither SHAPE_ID_NO_LOCKON nor all of SHAPE_GUARD */
static struct task *lockon_find_player(struct player *walker, struct task *task,
                                       Float *distance)
{
    struct task *found;
    Float found_distance;
    struct collision_work *collision;
    struct player *other;
    Float measured;
    int character;

    found_distance = LOCKON_NONE;
    found = NULL;
    collision = task->work->collision;
    measured = lockon_player_distance(collision, walker);
    /* @hack spelling: the tests break out of a loop that runs once, where
     * one if joined by && would do */
    do {
        other = task->extra.p;
        if (measured > 0.0F) {
            if (task->step == task_destroy || task->step == NULL ||
                other->by_character.mech.locked_by != NULL) {
                break;
            }
            /* @hack type: the shape's id read as a Uint8, where its Sint8
             * would do */
            if ((Uint8)collision->shapes->id == SHAPE_ID_NO_LOCKON) {
                break;
            }
            if ((collision->shapes->levels & SHAPE_GUARD) == SHAPE_GUARD) {
                break;
            }
            character = other->character;
            if (character != PLAYER_EGGMAN_MECH &&
                character != PLAYER_TAILS_MECH) {
                break;
            }
            if (other->flags & (PLAYER_MISSED | PLAYER_INVINCIBLE)) {
                break;
            }
            found = task;
            found_distance = measured;
        }
    } while (0);
    *distance = found_distance;
    return found;
}

/* lock on to the nearest target the walker can, while it aims and holds fewer
 * than WALKER_LOCKS locks. the target is the nearer of the nearest item and the
 * nearest enemy. the other player takes its place where that is nearer still. a
 * nearest of WORK_ID_STOP_LOCKON or WORK_ID_STOP_LOCKON_BOX takes no lock.
 *
 * a lock is a child task of the walker's, whose extra is a lock from syCalloc.
 * the target's lock_task holds the lock, or the other walker's locked_by. the
 * lock joins the walker's list. for WALKER_LOCK_WAIT aiming steps, the walker's
 * lock-on line then runs from the aim's start to the lock's pos. where
 * task_create_child or syCalloc fails, no lock is left. record and walker are
 * the one record, as RECORD_OF says */
static void walker_start_lockon(struct player *record, struct player *walker)
{
    Float other_distance;
    Float distance;
    struct task *target;
    struct task *candidate;
    struct task *child;
    struct lock *lock;
    int slot;

    if (walker->by_character.mech.aiming == 0 ||
        walker->by_character.mech.lock_count >= WALKER_LOCKS) {
        return;
    }
    target = lockon_find_nearest(
        record, collision_items,
        collision_get_last_count(COLLISION_COUNT_ITEMS), &distance);
    candidate = lockon_find_nearest(
        record, collision_enemies,
        collision_get_last_count(COLLISION_COUNT_ENEMIES), &other_distance);
    if (target == NULL) {
        /* @bug logic: locks on to the other player over a nearer enemy, where
         * no item lies in reach. the enemy's distance never replaces
         * LOCKON_NONE, at 0x8c097724, so the player's test at 0x8c09779a
         * measures against LOCKON_NONE */
        target = candidate;
    } else if (candidate != NULL) {
        /* @hack spelling: the distances' test in an if of its own, where one
         * joined by && would do */
        if (distance > other_distance) {
            target = candidate;
            distance = other_distance;
        }
    }
    if (record->slot == 0) {
        candidate = player_tasks[1];
    } else {
        candidate = player_tasks[0];
    }
    if (candidate != NULL) {
        candidate = lockon_find_player(record, candidate, &other_distance);
        if (other_distance > 0.0F && distance > other_distance) {
            target = candidate;
        }
    }
    if (target == NULL) {
        return;
    }
    /* @hack spelling: each id tested in a statement of its own, the second
     * through a label, where one test joined by || would do */
    if (target->work != NULL) {
        if (target->work->id == WORK_ID_STOP_LOCKON) {
            goto stop;
        }
        if (target->work->id == WORK_ID_STOP_LOCKON_BOX) {
        stop:
            target = NULL;
        }
    }
    if (target == NULL) {
        return;
    }
    child = task_create_child(TASK_WORK, lock_step, player_tasks[record->slot]);
    if (child == NULL) {
        return;
    }
    lock = syCalloc(1, sizeof(struct lock));
    if (lock == NULL) {
        task_destroy(child);
        return;
    }
    /* @hack spelling: slot, a local for the record's slot, where the field
     * read at its store would do */
    slot = record->slot;
    child->extra.p = lock;
    lock->slot = slot;
    collision_get_shape_center(target->work, target->work->collision->shapes,
                               &lock->pos);
    lock->target = target;
    if (collision_get_kind(target) == COLLISION_PLAYER) {
        struct player *target_walker = target->extra.p;

        lock_list_add(child, walker);
        target_walker->by_character.mech.locked_by = child;
    } else {
        struct enemy_work *enemy = target->motion.p;

        lock_list_add(child, walker);
        enemy->lock_task = child;
    }
    walker->by_character.mech.laser_on_lock = 1;
    walker->by_character.mech.lock_count++;
    walker->by_character.mech.lock_pos = lock->pos;
    walker->by_character.mech.lock_wait = WALKER_LOCK_WAIT;
    child->draw = lock_draw;
    child->destructor = lock_destroy;
    child->lock_target_name = target->name;
    sound_play(SOUND_LOCK, NULL, 0, 0);
}
