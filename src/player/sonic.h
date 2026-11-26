/* Sonic and Shadow, and Amy and Metal Sonic in their places in a two-player
 * game. this declares their makers and the calls of their task */
#ifndef PLAYER_SONIC_H
#define PLAYER_SONIC_H
#include <ninja.h>
#include <sg_xpt.h>

struct player;
struct player_motion;
struct task;
struct task_work;

/* make Sonic, or Shadow, the player in slot: a task named SonicTheHedgehog,
 * whose extra is the character's record. in a two-player game, a slot whose
 * game_state.alt_character isn't 0 takes Amy in Sonic's place, or Metal Sonic
 * in Shadow's. only a two-player game reads game_state.costume, which picks
 * Sonic's and Shadow's files.
 *
 * a demo gives a Sonic or an Amy UPGRADE_SONIC_SHOES alone, and a Shadow or a
 * Metal Sonic UPGRADE_SHADOW_SHOES alone. a two-player game gives a Sonic or a
 * Shadow its shoes alone too. it gives an Amy or a Metal Sonic no upgrade.
 * otherwise slot 1 on STAGE_VERSUS_SONIC_1 or STAGE_VERSUS_SONIC_2 takes no
 * upgrade, and any other player keeps those player_register gives it.
 *
 * where task_create or syCalloc fails, no task is left. collision_init,
 * jiggle_create and the loads go unchecked */
void sonic_create(int slot);
void shadow_create(int slot);

/* leave an afterimage of the player at each step whose game_state.steps is
 * even. it shows the animation playing, at its frame. the far body's model
 * takes the near body's place, where it is loaded. nothing calls it */
void sonic_pose_afterimage(struct task_work *work, struct player *record,
                           struct player *sonic);

/* the speed two of a light dash's ends hold the player to. one is a collision
 * other than a crash, which holds the motion's spd and the record's to it. it
 * stops either one that is no faster. a dash of one ring or none that
 * finds no ring near holds the record's spd to it. the dash's other ends take
 * a constant of the same value */
extern Float light_dash_exit_speed;

/* the versus actions, offered in a two-player game and on
 * STAGE_VERSUS_SONIC_2. unlike the others, their numbers name no mode. each
 * spends a ring level */
#define SONIC_ACTION_SPEED_UP 84
#define SONIC_ACTION_WIND_CUTTER 85
#define SONIC_ACTION_TIME_STOP 86

/* what sonic_test_action returns */
#define SONIC_ACTION_NONE 0
#define SONIC_ACTION_TAKEN 1
#define SONIC_ACTION_VERSUS 2 /* it took a versus action */

/* the calls below are the task's. each takes one player's work, motion and
 * record, unchecked. where a call takes sonic too, sonic must be the same
 * pointer as record. a test or an offer returns TRUE where it starts a mode or
 * offers an action. it returns FALSE otherwise, unless its comment says
 * else */

/* the light dash's step. it steers the player at the nearest ring ahead in
 * reach. where none is in reach it rolls the player on. a dash of one ring or
 * none with no ring farther out then runs out. its record's spd is held to
 * light_dash_exit_speed. the work must have a collision record with a shape,
 * unchecked */
void sonic_light_dash(struct task_work *work, struct player_motion *motion,
                      struct player *record, struct player *sonic);
/* the light attack's target, or null. in PLAYER_MODE_SONIC_LIGHT_ATTACK it
 * first sets steps to 0. the target is the nearest of the rival and the items
 * and enemies among the player's near targets. without a rival, an item or an
 * enemy must lie within 400. there is none on STAGE_CHAO, nor without near
 * targets */
struct task_work *sonic_light_attack_seek(struct task_work *work,
                                          struct player *record,
                                          struct player *sonic);
/* the light attack's step. it flies straight on, then at the target
 * sonic_light_attack_seek finds, which the stick pulls the player toward
 * harder. it returns FALSE where there is no target, and the player then
 * slows along the ground */
Bool sonic_light_attack(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic);
/* at a press of jump, jump. a player holding a task that player_action_flags
 * gives ACTION_FLAG_200 jumps with it, in PLAYER_MODE_JUMP_HOLDING. one holding
 * any other task lets go of it first. a player not under PLAYER_WORK_BALL and
 * past sprint_speed jumps off a WORK_ID_RAMP task in
 * PLAYER_MODE_SONIC_RAMP_JUMP. the ramp throws it along its angles. any other
 * jump takes PLAYER_MODE_JUMP and the force of the task stood on, but an Egg
 * Golem part's. that jump starts homing_hits at 0. a ground_task must have
 * a work, unchecked */
Bool sonic_test_jump(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *sonic);
/* answer the request that PLAYER_WORK_REQUEST flags in the work's submode.
 * sonic_test_request answers for a player holding nothing, and
 * sonic_test_request_holding for one holding a task. most of the second's
 * answers let go of the task first. each returns TRUE where it answers, and
 * FALSE without PLAYER_WORK_REQUEST or for a request it has no answer to.
 * neither clears PLAYER_WORK_REQUEST.
 *
 * PLAYER_REQUEST_LAUNCH_SPEED returns TRUE in any mode. it changes the mode
 * only in the combo, which runs on, and on a rail, which falls.
 * PLAYER_REQUEST_JOIN_PATH returns FALSE. above running_speed it sets
 * PLAYER_WORK_ON_PATH. it sets it too in PLAYER_MODE_RUN or PLAYER_MODE_ROLL
 * for a player holding nothing, and in PLAYER_MODE_UNK_19 for one holding a
 * task.
 *
 * only sonic_test_request takes a rail. PLAYER_REQUEST_RAIL_PATH and
 * PLAYER_REQUEST_RAIL_PATH_BENEATH return FALSE where path_distance lies off
 * the record's path. the second returns TRUE, changing nothing, where the
 * motion of the ride beneath a rail isn't loaded */
Bool sonic_test_request(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic);
Bool sonic_test_request_holding(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record, struct player *sonic);
/* at a press of the action button, take an action of the record's list. it
 * takes the record's action, the current one, where the list holds it, and
 * else the list's first. action_taken records the action. with the button up,
 * it starts the light attack where that is the list's first. it returns
 * SONIC_ACTION_VERSUS for a versus action, and SONIC_ACTION_TAKEN for any
 * other it starts. it returns SONIC_ACTION_NONE for an action it has no start
 * for */
int sonic_test_action(struct task_work *work, struct player_motion *motion,
                      struct player *record, struct player *sonic);
/* the combo's step. TRUE where it changes the mode. a press of the action
 * button in a move chains the next. a press of jump leaves the move. in
 * PLAYER_MODE_SONIC_COMBO_WAIT, and outside the combo, a long hold of the
 * button spins. a shorter one let go starts the combo, and so does a low
 * ceiling. for a player holding a task or off the ground, it only sets
 * hold_steps to 0 */
Bool sonic_combo(struct task_work *work, struct player_motion *motion,
                 struct player *record, struct player *sonic);

/* the offers of the task's actions. each adds its action to the record's
 * actions, as OFFER does.
 *
 * sonic_offer_light_dash offers the light dash to a player holding nothing,
 * with either character's shoes. its mode must be below
 * PLAYER_MODE_SONIC_LIGHT_DASH. a ring must lie in the dash's reach.
 *
 * sonic_offer_light_attack offers the light attack under PLAYER_WORK_CHARGED,
 * where sonic_light_attack_seek finds a target. with the action button up it
 * clears PLAYER_WORK_CHARGED, and offers all the same.
 *
 * sonic_offer_magic_hand offers the magic hand with UPGRADE_SONIC_MAGICWRIST,
 * in PLAYER_MODE_STAND, PLAYER_MODE_RUN or PLAYER_MODE_FALL. an enemy with a
 * motion must lie within 400. to an empty list it adds ACTION_SKIPPED first.
 *
 * sonic_offer_hold_handle offers PLAYER_MODE_HOLD_HANDLE where that is the
 * record's action, on the ground, standing or running.
 *
 * sonic_offer_versus_actions offers the first of SONIC_ACTION_TIME_STOP,
 * SONIC_ACTION_WIND_CUTTER and SONIC_ACTION_SPEED_UP whose ring level the
 * player holds. it offers them only in a two-player game or on
 * STAGE_VERSUS_SONIC_2, before a clear. the player must stand or run, and its
 * ring_level_timer must be 0 or less */
Bool sonic_offer_light_dash(struct task_work *work, struct player *record);
Bool sonic_offer_light_attack(struct task_work *work, struct player *record,
                              struct player *sonic);
Bool sonic_offer_magic_hand(struct task_work *work, struct player *record);
Bool sonic_offer_hold_handle(struct task_work *work, struct player *record);
Bool sonic_offer_versus_actions(struct task_work *work, struct player *record);

/* the step of PLAYER_MODE_SONIC_SPIN. TRUE where the spin ends. with either
 * character's sparkle, a spin that has lasted long enough charges the light
 * attack, and ends. any spin ends at a later count of steps. an end stands the
 * player, or runs it on where spd.x is above 0. a release of the action
 * button rolls the player off at the charge anim_speed holds. otherwise the
 * charge grows */
Bool sonic_spin(struct task_work *work, struct player_motion *motion,
                struct player *record, struct player *sonic);
/* at a press of the action button with UPGRADE_SONIC_RUBBERUNIT, bounce: dive
 * in PLAYER_MODE_SONIC_BOUNCE. sonic_test_bounce_again dives faster, and
 * leaves steps as it is */
Bool sonic_test_bounce(struct task_work *work, struct player *record,
                       struct player *sonic);
Bool sonic_test_bounce_again(struct task_work *work, struct player *record);

/* the test of PLAYER_MODE_PICK_UP. each step turns the player toward its offer,
 * as player_approach_touched does. from a frame of the reach on, the player
 * takes hold of hold_offer where it can. player_face_offer must grade the offer
 * PLAYER_WALL_HEAD_ON, and player_action_flags must give it ACTION_FLAG_200. it
 * then holds the task in PLAYER_MODE_STAND_HOLDING, and the call returns TRUE.
 * from that frame on, a step that takes nothing stands the player */
Bool sonic_test_pick_up(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* the test of PLAYER_MODE_PULL_BURIED, a pull of a task out of the ground. it
 * turns the player toward its offer, as sonic_test_pick_up does. as a pass of
 * ANIM_PULL's loop starts, the stick moves the pull on to ANIM_PULL_LIFT.
 * without the stick, a chance does. in ANIM_PULL_LIFT, a player holding nothing
 * takes hold of a head-on offer that player_action_flags gives
 * ACTION_FLAG_1000. the task's work takes FALL_UNK_200, and its id becomes
 * FALL_KIND_NORMAL. where the player can't, it stands. so does one holding a
 * task that player_action_flags gives nothing. in ANIM_HOLD_STAND, which
 * follows the lift, ACTION_FLAG_200 starts PLAYER_MODE_STAND_HOLDING, and the
 * call returns TRUE. it returns FALSE otherwise, after a taking of hold too */
Bool sonic_test_pull(struct task_work *work, struct player_motion *motion,
                     struct player *record);
/* let go of the held task in the frames of its throw that release it, and
 * return TRUE. ANIM_THROW_AT and ANIM_AIR_THROW_AT throw it at the player's
 * first near target. a nearer item or enemy whose id isn't FALL_KIND_NORMAL
 * takes its place. they let go of nothing while the player has no near
 * target. ANIM_THROW and ANIM_AIR_THROW throw it ahead, and ANIM_SET_DOWN sets
 * it down. a task without a work is let go unthrown. with held null, the call
 * clears PLAYER_WORK_HOLDING and returns TRUE. it returns FALSE there where
 * the flag was clear */
Bool sonic_throw(struct task_work *work, struct player_motion *motion,
                 struct player *record);
/* ANIM_THROW_AT_TOUCH's throw, as sonic_throw throws. it throws at the task of
 * a touched shape of SHAPE_ID_FACED, or ahead where the player touches none.
 * task must be the player's */
Bool sonic_throw_at_mark(struct task *task, struct task_work *work,
                         struct player_motion *motion, struct player *record);
/* keep the held task while player_action_flags gives it ACTION_FLAG_200, or
 * while the player's collision last hit it. let go of it otherwise. a player
 * that then holds nothing stands on the ground, or falls in the air. with held
 * null, it does nothing. the work must have a collision record, unchecked */
void sonic_drop_held(struct task_work *work, struct player_motion *motion,
                     struct player *record);
/* put the held task between the hands, or at a hand in a throw. it does so
 * where player_action_flags gives the task ACTION_FLAG_200 */
void sonic_hold_place(struct task_work *work, struct player *record);

/* the rail's step. it stands the player on the record's path, facing along
 * it, and sets the record's accel. the action button crouches the player. it
 * plays the rail's sounds, leans the animation and totters a lean too far.
 * steps counts the steps on the rail, and ball_steps the steps of a totter.
 * the record's path must not be null, unchecked */
void sonic_rail(struct task_work *work, struct player_motion *motion,
                struct player *record, struct player *sonic);
/* at a press of jump, jump off the soap switch the player last touched into a
 * trick. it needs a speed past trot_speed, and the trick takes
 * PLAYER_MODE_SONIC_SOAP_JUMP. the player's speed picks the trick's grade and
 * its speeds. the switch's scl.z or scl.y changes a grade A or B trick's spd.x.
 * a trick plays its camera motion where the switch's tick is 0. the switch's
 * timer becomes stick_lock */
Bool sonic_test_soap_switch(struct task_work *work, struct player *record,
                            struct player *sonic);

/* the step of a ride beneath a rail, the record's path, which must not be
 * null, unchecked. the player hangs upside down at the place on the path
 * nearest it. it tilts as the path tilts there. it heads along the path the way
 * nearer its heading. the record's accel takes the motion's acc and a pull
 * along world_gravity. on STAGE_CRAZY_GADGET and STAGE_RELAY_SONIC the ride
 * sounds by speed. sonic isn't read */
void sonic_rail_beneath(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *sonic);
/* move work's pos down the player's own y axis, out from under a rail as its
 * ride ends */
void sonic_drop(struct task_work *work);

/* the board's effects, once a step. above a speed it throws the board's
 * sparks, except from ground of WORLD_FLAT, WORLD_DUSTY or GROUND_SLIPPERY. on
 * STAGE_CITY_ESCAPE a lean far enough plays the lean's sound as they fly. it
 * leans the animation by drive. the motion's spd must not be the zero vector
 * where the sparks fly, unchecked */
void sonic_board_sparks_and_lean(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record);
/* answer the request of a player on its board. it returns TRUE for
 * PLAYER_REQUEST_ANIM, PLAYER_REQUEST_SCARE, PLAYER_REQUEST_RESET,
 * PLAYER_REQUEST_MISS and PLAYER_REQUEST_RELEASE. it returns FALSE for any
 * other request, and without PLAYER_WORK_REQUEST. PLAYER_REQUEST_JOIN_PATH
 * sets PLAYER_WORK_ON_PATH above running_speed */
Bool sonic_board_take_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);

#endif
