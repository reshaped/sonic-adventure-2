/* the steps the characters share, and the small tasks a player starts. the
 * steps stand a player, hit it, hang it from ledges, and choose and start its
 * next mode. the tasks turn its head and eyes, show its afterimages, poses and
 * flute, and play its delayed sounds */
#ifndef PLAYER_ACTION_H
#define PLAYER_ACTION_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct explosion;
struct particle;
struct player;
struct player_motion;
struct task;
struct task_work;

/* calls the stages lend the players. each is null until a stage sets it. a
 * stage sets each hook it lends as it starts, where the hook is null. it clears
 * each that still holds its own as it ends */
struct player_hooks {
    /* scatter the rings of the player in slot, as ring_scatter does.
     * player_take_hit calls it unchecked */
    void (*drop_rings)(int slot);
    /* throw a piece of dirt, as dirt_create does. a hunter's dig calls it */
    struct particle *(*unk_04)(NJS_POINT3 *pos, NJS_VECTOR *speed, Float scale);
    /* the hunt's calls: item_icon_row_add, emerald_create, emerald_sign_create,
     * explosion_create and skull_throw_up. a hunter calls add_item_icon
     * unchecked */
    void (*add_item_icon)(int player, int kind);
    struct task *(*create_piece)(NJS_POINT3 *pos, Uint8 kind, Uint8 id);
    struct task *(*show_piece_sign)(int player, int piece);
    struct explosion *(*explode)(NJS_POINT3 *pos);
    struct task *(*throw_skull)(NJS_POINT3 *pos, Float speed);
    /* ripple_create, which player_ripple calls */
    void (*ripple)(NJS_POINT3 *pos, NJS_VECTOR *growth, Float scale);
    /* flash_manager_create_sparks_depth, which a thunder bolt and a hunter's
     * bump call */
    void (*flash)(Float x, Float y, Float z, Float scale, Uint8 sparks,
                  Float depth_bias);
    /* big_splash_create, which player_splash calls */
    void (*splash)(NJS_POINT3 *pos, struct angles *ang, Float size,
                   Float speed);
    /* bomb_hat_draw_ring adds the stage ring to a mech lock-on blast */
    void (*draw_blast_ring)(Float radius, Float fade);
    /* where it returns TRUE, player_stand_anim stands Rouge in
     * ANIM_ROUGE_STAND_216 */
    Bool (*in_shadow)(void);
};
extern struct player_hooks player_hooks;

/* the calls below take a player's work, motion and record. all three must be
 * the same player's, unchecked. a test returns TRUE where it starts a mode or
 * finds what it tests for, and FALSE otherwise. player_test_pull_heavy,
 * player_test_shake_held, player_test_shake_end and player_test_petting
 * change the player on FALSE too, as their comments say. a setter sets the
 * work's mode and the animation that starts it */

/* stand the player on what lies under it. a character's step calls it once a
 * step, with the player's task. it first empties the record's offered actions
 * and its hold_offer.
 *
 * a touched shape of SHAPE_ID_LINE may stand the player on its line, under
 * PLAYER_WORK_ON_SHAPE. none does after a clear, in a mini event or in
 * PLAYER_MODE_EVENT. such a step never clears that flag or writes stood_on.
 *
 * without a line, a player landing on top of other tasks' shapes stands upright
 * on them. it takes PLAYER_WORK_ON_SHAPE. stood_on takes
 * collision_next_below's task, which may be null. it must not be null where
 * the player's first shape has SHAPE_HURT_NO_PUSH, unchecked. no player stands
 * so after a clear, or where its first shape and stood_on's both have
 * SHAPE_HURT_NO_PUSH. nor does one that touches a shape of SHAPE_ID_NO_STAND,
 * or an item or an enemy but no object. a step that stands it on no shape
 * clears the flag and stood_on.
 *
 * a player then on neither ground nor shape may land on the ground below it,
 * under PLAYER_WORK_ON_GROUND. it takes the ground's tilt, and its flags in
 * ground_flags. it lands only under gravity straight down, and on ground no
 * farther below than consts.gravity. the ground must tilt within 45 degrees of
 * the player. no player lands in PLAYER_MODE_UNK_35 or PLAYER_MODE_PLAY_ANIM,
 * as a climbing hunter, or with its pad off.
 *
 * last, PLAYER_REQUEST_RESET stands the player at once, with no blend. in
 * PLAYER_MODE_MISS, a PLAYER_REQUEST_ANIM of ANIM_WIN stops the player and
 * plays it in PLAYER_MODE_PLAY_ANIM. so does one of ANIM_LOSE in a two-player
 * game */
void player_ground_step(struct task *task, struct task_work *work,
                        struct player_motion *motion, struct player *record);
/* take a hit, and return TRUE where it counts. a hit comes from WORK_HURT, or
 * from WORLD_HURTS ground. the ground hurts only while the player's first shape
 * guards below 3, its highest level. nothing hits after a clear, a work of
 * WORK_ID_NO_HIT, or a player in PLAYER_MODE_MISS, PLAYER_MODE_DROWN or
 * PLAYER_MODE_UNK_38. no object, item or enemy hurts on STAGE_VERSUS_SONIC_2.
 * where a mech touches no item, enemy or player, its own attack touched last
 * spares it the hit. record may be null only where the work lacks WORK_HURT,
 * and then nothing hits.
 *
 * a hit shakes the pad, and sets the work's timer to PLAYER_HIT_STEPS. it
 * doesn't count against a player with UPGRADE_UNK_15, whose spd.x it halves, or
 * one under PLAYER_MISSED. any other player is thrown back in PLAYER_MODE_HURT,
 * and loses its barrier, or else its rings by player_hooks.drop_rings. one that
 * touches a shape of SHAPE_ID_THROWS is thrown away from that shape's task
 * instead, in PLAYER_MODE_UNK_43.
 *
 * a mech's gauge loses the hit's damage, and the mech misses once it runs out.
 * any other WORK_ID_RIVAL player's gauge loses 1. a player misses in
 * PLAYER_MODE_MISS where its PLAYER_REQUEST_MISS waits. so does a rival whose
 * gauge runs out while player 0 hasn't missed. the player says a line, or makes
 * a sound, by its kind and the stage */
Bool player_take_hit(struct task_work *work, struct player *record);

/* the ledges. a ledge is in reach only where gravity points straight down and
 * the player isn't a ball. the ground must lie 5 or more below the player. the
 * ledge ahead must tilt within 10 degrees of level. its top must lie from 3
 * below to 1 above the player's head. for Tails it must lie from 5 below to 1
 * below his head. its surface must have no flag that refuses a hang. no ledge
 * is in Eggman's reach.
 *
 * player_test_ledge is TRUE where a ledge is in reach, the player isn't rising
 * and stick_lock is 0. the player must then take its stick toward the wall
 * below the ledge. it may instead take no stick while it faces the wall.
 * player_hang_ledge hangs the player against the wall in
 * PLAYER_MODE_LEDGE_HANG, with its character's sound or line. it does nothing
 * where the player touches no wall.
 *
 * player_test_ledge_hang lets a hanging player fall where the ledge leaves its
 * reach. it falls too where the stick turns away from the wall. a jump, or a
 * stick toward the wall, climbs in PLAYER_MODE_LEDGE_CLIMB. player_onto_ledge
 * moves the player up to the ledge's top and ahead onto it */
Bool player_test_ledge(struct task_work *work, struct player_motion *motion,
                       struct player *record);
void player_hang_ledge(struct task_work *work, struct player_motion *motion,
                       struct player *record);
Bool player_test_ledge_hang(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);
void player_onto_ledge(struct task_work *work, struct player_motion *motion,
                       struct player *record);

/* stand the player in PLAYER_MODE_STAND, clear mode_steps, and choose its
 * animation by player_stand_anim. a player that poses, in PLAYER_MODE_EVENT
 * while game_state.solo_player names it, is left as it is */
void player_stand(struct task_work *work, struct player_motion *motion,
                  struct player *record);
/* choose a standing player's animation, and count its steps in mode_steps. on
 * ground within 25 degrees of level, or WORLD_UPRIGHT ground, the player
 * stands. after 900 steps it plays one of its character's three idles, and
 * player 0 may say a line. on a slope up to 45 degrees it leans uphill or
 * downhill. it walks in place there while its heading turns toward its aim. on
 * a steeper one its animation stays. mode_steps counts one a step, or through
 * an idle one every other step. a landing, a climb and a turn play on,
 * untouched and uncounted. so do a rise out of a ball and a hunter's landing
 * from a dive.
 *
 * player_hold_stand_anim chooses a holding player's stand or lean by the
 * ground's tilt alone. it has no idles, walks in place for no turn and counts
 * nothing. a slope of WORLD_UPRIGHT ground leaves its animation */
void player_stand_anim(struct task_work *work, struct player_motion *motion,
                       struct player *record);
void player_hold_stand_anim(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);
/* choose a running player's animation by its speed, as ANIM_WALK's comment
 * says. a player that holds a task takes ANIM_HOLD_RUN or ANIM_HOLD_WALK. the
 * speed counts the record's slip. it eases the record's lean toward the turn of
 * the last three steps, or toward 0 below running_speed. it sets anim_speed
 * too. a rise out of a ball or a turn plays on untouched */
void player_run_anim(struct task_work *work, struct player_motion *motion,
                     struct player *record);

/* TRUE where the player takes no stick and spd.x is 0. the player then stands.
 * one that holds a task takes PLAYER_MODE_UNK_17 where player_action_flags
 * gives ACTION_FLAG_800. with ACTION_FLAG_400 it takes
 * PLAYER_MODE_GRAB_PUSHABLE, and otherwise PLAYER_MODE_STAND_HOLDING */
Bool player_test_stand(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* TRUE where the player stands on neither ground nor a shape, and then it takes
 * a mode. a player running off level ground at trot_speed or more teeters over
 * a drop of 60 or more. it teeters in PLAYER_MODE_TEETER, back where it stood
 * two steps before. over a shorter drop, or under water, it falls in
 * PLAYER_MODE_FALL, keeping what it holds. no mech teeters, nor does a player
 * off moving ground or against a wall. nor does one with its pad off or its
 * stick locked.
 *
 * any other player under PLAYER_WORK_BALL takes PLAYER_MODE_JUMP. one that
 * holds a task to which player_action_flags gives ACTION_FLAG_200 takes
 * PLAYER_MODE_FALL_HOLDING. the rest let go and fall, in PLAYER_MODE_FALL */
Bool player_test_fall(struct task_work *work, struct player_motion *motion,
                      struct player *record);
/* TRUE where spd.x is below trot_speed. the player then rises out of any ball,
 * in ANIM_STAND_UP. it stands where spd.x isn't above 0, and runs otherwise */
Bool player_test_slow(struct task_work *work, struct player_motion *motion,
                      struct player *record);
/* TRUE where the player takes its stick and presses no button of
 * pad_grab_buttons this step. the player then pushes. where
 * player_action_flags gives ACTION_FLAG_800, it takes PLAYER_MODE_UNK_22, whose
 * step pushes the FALL_KIND_SHAKE offer by player_shake_touched. a player in
 * slot 3 or 2 then says a line. any other push takes PLAYER_MODE_UNK_0E where
 * player_stick_into_offer grades the stick PLAYER_WALL_HEAD_ON. it takes
 * PLAYER_MODE_UNK_20 at any other grade */
Bool player_test_push(struct task_work *work, struct player_motion *motion,
                      struct player *record);
/* TRUE where the work lacks PLAYER_WORK_HOLDING. the player then stands, or
 * falls in the air, and lets go of held where held isn't null */
Bool player_test_hold_end(struct task_work *work, struct player_motion *motion,
                          struct player *record);
/* let go of what the player holds, under PLAYER_WORK_HOLDING, and empty held.
 * where player_action_flags gives ACTION_FLAG_200, it throws the task ahead and
 * up, at 1 along each. where it gives ACTION_FLAG_400, it sets the task down.
 * held must not be null under PLAYER_WORK_HOLDING, unchecked */
void player_release_held(struct task_work *work, struct player_motion *motion,
                         struct player *record);
/* the test of player_pull_heavy's pull, which never lifts the task. it turns
 * and steps the player toward its offer, by player_approach_touched, and
 * counts mode_steps. once mode_steps reaches 300, ANIM_PULL goes on to
 * ANIM_PULL_END and then ANIM_STAND. a press of the action button does so
 * sooner. once the next animation is ANIM_STAND, the player stands. the call
 * returns FALSE */
Bool player_test_pull_heavy(struct task_work *work,
                            struct player_motion *motion,
                            struct player *record);
/* the test for a shake of the held task. while a button of pad_grab_buttons
 * is held, the call reads the held task. held must not be null then,
 * unchecked. the call is FALSE where that task's id isn't object/fall.h's
 * FALL_KIND_NORMAL. otherwise it is TRUE where the player takes its stick,
 * and then clears mode_steps. from 60 on, each call counts mode_steps up by
 * one. the call is TRUE once the count is 120 or more. with no such button
 * held, the call clears mode_steps and returns FALSE */
Bool player_test_shake_held(struct task_work *work, struct player *record);
/* the test for the end of a shake, of the held task or of a FALL_KIND_SHAKE
 * offer. TRUE where mode_steps is 0 and the player takes no stick. with
 * mode_steps other than 0, it is TRUE where no button of pad_grab_buttons is
 * held. it then clears mode_steps. while one is held, mode_steps counts down.
 * once the count goes below 0, the call sets it to 60 and is TRUE */
Bool player_test_shake_end(struct task_work *work, struct player *record);
/* TRUE where a task with a shape of SHAPE_ID_PET lies ahead at the player's
 * height. it stays TRUE for 60 steps, and then while the action button is held.
 * mode_steps counts the 60. over them the player turns toward the task. it
 * steps up to 1 a step toward a distance of 4.5 from it, or 5.5 for Tails */
Bool player_test_petting(struct task_work *work, struct player_motion *motion,
                         struct player *record);

/* an action the action window steps past whenever it is current, so that the
 * next one shows */
#define ACTION_SKIPPED 3
/* add action to the record's offered actions. they keep PLAYER_ACTIONS, and
 * each offer past them overwrites the oldest */
#define OFFER(record, action)                                                  \
    ((record)->actions[(record)->action_count++ & (PLAYER_ACTIONS - 1)] =      \
         (action))

/* the offers. each adds its actions by OFFER, and returns TRUE where it added
 * one. an offer of the record's action, the current one, adds it only where it
 * is current.
 *
 * player_offer_hint offers PLAYER_MODE_ASK_HINT to a player that stands or
 * runs on the ground, where it is current, as a hint box makes it.
 * player_offer_flute offers a standing player PLAYER_MODE_PLAY_FLUTE or
 * PLAYER_MODE_PLAY_FLUTE_AGAIN. without its character's MELODY upgrade, it
 * offers PLAYER_MODE_CANT_PLAY_FLUTE where that or PLAYER_MODE_PLAY_FLUTE is
 * current. player_offer_fall offers PLAYER_MODE_FALL in the air.
 * player_offer_release offers PLAYER_MODE_LET_GO in PLAYER_MODE_GRAB_PUSHABLE
 * or PLAYER_MODE_UNK_17, where player_action_flags gives ACTION_FLAG_400 or
 * ACTION_FLAG_800. each character on foot takes that action by
 * player_release_and_stand. player_offer_call offers a standing player
 * PLAYER_MODE_UNK_2A, after ACTION_SKIPPED where nothing was offered before.
 * chao_try_player_call reads player 0 in that mode as a call. a Chao pipe and
 * a hidden animal offer the same mode as their own action. each lets out an
 * animal for a player who takes it.
 *
 * player_offer_target_actions offers a player on the ground the modes of the
 * flags player_action_flags gives. player_face_offer must grade the offer other
 * than PLAYER_WALL_NONE. ACTION_FLAG_400 offers PLAYER_MODE_GRAB_PUSHABLE,
 * ACTION_FLAG_800 PLAYER_MODE_UNK_17 and ACTION_FLAG_1000
 * PLAYER_MODE_PULL_BURIED. ACTION_FLAG_200 offers PLAYER_MODE_PICK_UP. a task
 * with a shape of SHAPE_ID_PET ahead at the player's height adds
 * PLAYER_MODE_PET_CHAO before it. it offers PLAYER_MODE_PULL_HEAVY in place of
 * PLAYER_MODE_PICK_UP where the offer isn't FALL_KIND_NORMAL. it doesn't where
 * a task of SHAPE_ID_PET, or one that collision_nearest_unk_9_within finds,
 * lies ahead. it offers nothing to a player that holds a task. nor does it in
 * the modes from PLAYER_MODE_PLAY_ANIM up to PLAYER_MODE_UNK_3B. nor does it
 * offer a hunter anything in the modes from PLAYER_MODE_HUNTER_DIG to
 * PLAYER_MODE_HUNTER_RUN_ATTACK_3. outside STAGE_CHAO, it offers nothing in
 * water deeper than WATER_DEEP.
 *
 * player_offer_held_actions offers only to a player whose held task gives
 * ACTION_FLAG_200. it offers PLAYER_MODE_AIR_THROW in the air. on the ground it
 * offers PLAYER_MODE_UNK_14 below trot_speed, and PLAYER_MODE_THROW otherwise.
 * a task whose first shape has SHAPE_ID_NO_SET_DOWN takes PLAYER_MODE_THROW at
 * any speed */
Bool player_offer_hint(struct task_work *work, struct player *record);
Bool player_offer_flute(struct task_work *work, struct player *record);
Bool player_offer_fall(struct task_work *work, struct player *record);
Bool player_offer_target_actions(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
Bool player_offer_release(struct task_work *work, struct player *record);
Bool player_offer_held_actions(struct task_work *work, struct player *record);
Bool player_offer_call(struct task_work *work, struct player *record);

/* the setters. player_fall clears the record's spd and leaves the motion's.
 * player_hold holds hold_offer at once, without a lift, or else the task the
 * player's collision hit last. that hit must not be null then, unchecked.
 * player_play_flute and player_play_flute_again start the flute's task. On the
 * port the flute remains visible during the second animation and both tips fade
 * back in. player_pet_chao starts the petting's task. player_start_push turns
 * the player's aim toward its offer, or else against the normal
 * collision_get_normal gives.
 *
 * player_pick_up starts PLAYER_MODE_PICK_UP at an ACTION_FLAG_200 offer.
 * player_pull_heavy starts PLAYER_MODE_PULL_HEAVY at a FALL_KIND_HEAVY offer,
 * which the pull never lifts. player_pull_buried starts
 * PLAYER_MODE_PULL_BURIED, which pulls a FALL_KIND_BURIED offer out of the
 * ground. the three cap spd.x at trot_speed. player_grab_shakable starts
 * PLAYER_MODE_UNK_17 at a FALL_KIND_SHAKE offer. player_call plays its
 * character's sound 20 steps later, or 42 for Eggman */
void player_start_push(struct task_work *work, struct player_motion *motion,
                       struct player *record);
void player_ask_hint(struct task_work *work, struct player *record);
void player_play_flute(struct task_work *work, struct player *record);
void player_play_flute_again(struct task_work *work, struct player *record);
void player_cant_play_flute(struct task_work *work, struct player *record);
void player_fall(struct task_work *work, struct player *record);
void player_hold(struct task_work *work, struct player *record);
void player_pick_up(struct task_work *work, struct player *record);
void player_pull_buried(struct task_work *work, struct player *record);
void player_grab_shakable(struct task_work *work, struct player *record);
void player_air_throw_held(struct task_work *work, struct player *record);
void player_throw_held(struct task_work *work, struct player *record);
void player_throw_at_touch(struct task_work *work, struct player *record);
void player_set_down_held(struct task_work *work, struct player_motion *motion,
                          struct player *record);
void player_release_and_stand(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);
void player_call(struct task_work *work, struct player *record);
void player_pull_heavy(struct task_work *work, struct player *record);
void player_pet_chao(struct task_work *work, struct player *record);

/* start the look task of the player in slot, as a child of its task. nodes[0]
 * is the head, and nodes[1] and nodes[2] the eyes. the look turns them toward
 * what the record's look_mode says, or ahead. it clears each node's
 * NJD_EVAL_UNIT_ANG for good, and the task's end puts back their angles. it
 * returns TRUE, or FALSE where task_create_child or syCalloc fails, which
 * leaves no task */
Bool player_look_start(int slot, NJS_CNK_OBJECT **nodes);

/* in a one-player game, leave a copy of object that fades out over 20 steps.
 * motion poses it at frame, where motion isn't null. the copy draws where
 * matrix puts it, or where the current matrix does for a null matrix. it ends
 * early once the slot has no work */
void player_afterimage(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                       NJS_TEXLIST *texlist, Float frame, NJS_MATRIX *matrix,
                       int slot);
/* ripple the water at pos by player_hooks.ripple, at random one call in about
 * seven. nothing ripples where the hook is null or the player in slot stands
 * still. nor does anything where pos lies above the player's head. ang is
 * unused */
void player_ripple(int slot, NJS_POINT3 *pos, struct angles *ang);
/* splash at pos, turned by ang, by player_hooks.splash, and play the splash's
 * sound. nothing splashes where the hook is null, or the player in slot moves
 * at 1 or slower. the splash's size is 0 within 8 of the ground below the
 * player, and its character's otherwise */
void player_splash(int slot, NJS_POINT3 *pos, struct angles *ang);

/* play sound id, or voice id, delay steps from now. delay must be 1 or more */
void player_sound_after(int id, int delay);
void player_voice_after(int id, int delay);
/* play sound id at the player in slot each step. it stops once the slot's work
 * is gone or leaves mode */
void player_sound_in_mode(int id, int slot, Sint16 mode);

/* pose the player in slot: it plays anim in PLAYER_MODE_EVENT, alone and on the
 * whole screen. the camera meanwhile plays its character's camera motion about
 * it. Tails and Eggman pose with no camera motion. the pose ends once the
 * player leaves the mode or the animation, and then requests
 * PLAYER_REQUEST_RELEASE */
void player_pose(int slot, int anim);
/* in a two-player game, on a stage up to STAGE_STG_END, hold both players in
 * PLAYER_MODE_EVENT until play starts. then pose each in turn for 122 steps,
 * player 0 first. Amy poses 60 steps more unless she won the last round. last,
 * release both, place them for play and restart the camera */
void player_pose_two_start(void);

#endif
