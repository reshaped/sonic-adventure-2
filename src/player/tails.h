/* Tails on foot, the player of PLAYER_TAILS outside his walker. his create,
 * and the moves his step and draw call */
#ifndef PLAYER_TAILS_H
#define PLAYER_TAILS_H
#include <sg_xpt.h>

struct player;
struct player_motion;
struct task;
struct task_work;

/* the size in bytes of Tails's record, which his task's extra points to. his
 * own fields are struct player's by_character.tails */
#define TAILS_RECORD_SIZE 0x38c

/* the bits of his record's state, which nothing reads. tails_fly_start sets
 * TAILS_FLYING, as does his answer to PLAYER_REQUEST_HOVER. it clears as he
 * lands from PLAYER_MODE_FALL or PLAYER_MODE_TAILS_FLY. it clears too as he
 * takes any request or action in PLAYER_MODE_TAILS_FLY. that includes
 * PLAYER_REQUEST_FLY, which starts the flight again. a request taken in
 * PLAYER_MODE_TAILS_HOVER leaves it set. TAILS_ATTACKING is set as his tail
 * attack starts. it clears as the attack's animation or a request ends it. a
 * hit clears neither */
#define TAILS_FLYING 0x1
#define TAILS_ATTACKING 0x2
/* in his record's unk_30b, which fun_8c08dc00 and fun_8c08dc24 set. while it
 * is set, his draw sets unk_310 to 1 */
#define TAILS_30B_UNK_1 0x1

/* his slots of model_table that MILESMDL.PRS fills and tails_create reads.
 * his look task turns TAILS_HEAD and his eyes. each of his tails is a jiggle
 * of TAILS_TAIL. in flight and in a fast run it eases to TAILS_TAIL_SHAPE_A,
 * and in his tail attack to TAILS_TAIL_SHAPE_B */
#define TAILS_HEAD 211
#define TAILS_EYE_0 212
#define TAILS_EYE_1 213
#define TAILS_TAIL 222
#define TAILS_TAIL_SHAPE_A 228
#define TAILS_TAIL_SHAPE_B 229

/* his step probes the ground below a point this far up his own y from his
 * pos */
#define TAILS_BEHIND_HEIGHT 2.5F
/* what his record's unk_310 becomes as he lands from PLAYER_MODE_TAILS_FLY */
#define TAILS_UNK_310_LANDED 0.8F

/* create Tails's task, named MilesTailsPrower, as the player in slot. it loads
 * his textures, models and motions, and starts his look task and the slot's
 * action window. it makes a jiggle for each of his tails. where task_create or
 * syCalloc fails, no task is left. the loads and jiggle_create go unchecked */
void tails_create(int slot);

/* his task's destructor. it frees his tails' jiggles, unregisters his slot
 * and stops the sounds keyed to his work. it then releases his textures, his
 * character's life icon, and his model and motion files */
void tails_destroy(struct task *task);
/* his task's draw. it draws his body, posed by his animation, or in
 * PLAYER_MODE_EVENT by his event's motion. his two tails draw at his body's
 * tails node. his shadows fall at its nodes, as modifier volumes. as his body
 * draws, the record keeps its nodes' places and axes. the held task then goes
 * to his hands, where player_action_flags gives it ACTION_FLAG_200. his body
 * and tails blink as a hit's timer counts down. while they are out, the held
 * task goes where the last draw left his hands. the jiggles must not be null,
 * and the held task must have a work, unchecked */
void tails_draw(struct task *task);

/* a call below that takes a work, a motion or a record takes Tails's. all must
 * be the one player's, unchecked. where a call takes tails too, tails must be
 * the record again */

/* place his body's shape center_height up his own y from his pos. in
 * PLAYER_MODE_TAILS_TAIL_ATTACK, it also sets the record's tip_plus_z and
 * tip_minus_z, where tails_set_shapes puts the tips of his tails. they spin a
 * whole turn about his y over the attack's motion. the work must have a
 * collision record, unchecked */
void tails_place_shapes(struct task_work *work, struct player *record,
                        struct player *tails);
/* step his tails' jiggles, and his record's spin, spin_speed and tail_tilt.
 * each tail's param becomes his height above the ground that his last behind
 * probe found. the call keeps his pos and heading in last_pos and last_ang_y,
 * for the next step's. the jiggles must not be null, unchecked */
void tails_move_tail_jiggles(struct task_work *work, struct player *record,
                             struct player *tails);
/* PLAYER_MODE_FREE_MOVE's step, which moves his pos through anything. while X
 * is held, the stick moves him down or up. otherwise the stick of slot 0's pad
 * moves him across his own camera's view, whatever his slot. a press of B
 * places him again by stage_place_player */
void tails_free_move(struct task_work *work, struct player *record);
/* at a press of jump, jump and return TRUE, and return FALSE otherwise. a held
 * task that player_action_flags gives ACTION_FLAG_200 goes up with him, in
 * PLAYER_MODE_JUMP_HOLDING. he lets go of any other, and jumps in
 * PLAYER_MODE_JUMP. where the task he stands on has a force, its speed then
 * becomes his motion's acc */
Bool tails_test_jump(struct task_work *work, struct player_motion *motion,
                     struct player *record);

/* answer the request that PLAYER_WORK_REQUEST flags in the work's submode.
 * tails_take_request answers as a Tails holding nothing, and
 * tails_take_held_request as one holding a task. each returns TRUE where it
 * answers. each returns FALSE without PLAYER_WORK_REQUEST, or for a request it
 * has no answer to. PLAYER_WORK_REQUEST stays set either way.
 *
 * PLAYER_REQUEST_JOIN_PATH returns FALSE, and sets PLAYER_WORK_ON_PATH above
 * running_speed. it sets it in one mode besides: PLAYER_MODE_RUN for
 * tails_take_request, and PLAYER_MODE_UNK_19 for tails_take_held_request. in
 * his tail attack above running_speed, tails_take_request ends the attack in
 * PLAYER_MODE_RUN, and returns TRUE.
 *
 * PLAYER_REQUEST_FLY starts his flight by tails_fly_start.
 * PLAYER_REQUEST_HOVER starts PLAYER_MODE_TAILS_HOVER, with TAILS_FLYING.
 * PLAYER_REQUEST_HAND makes handed the held task. handed must have a work,
 * unchecked.
 *
 * tails_take_held_request keeps the held task through seven answers: those to
 * PLAYER_REQUEST_FLY, PLAYER_REQUEST_HOVER, PLAYER_REQUEST_BOUNCE,
 * PLAYER_REQUEST_LAUNCH, PLAYER_REQUEST_UPDRAFT, PLAYER_REQUEST_RELEASE and
 * PLAYER_REQUEST_HAND. every other answer lets go of the task first, by
 * player_release_held. its PLAYER_REQUEST_HAND replaces the held task, which
 * keeps WORK_HELD_BY_PLAYER */
Bool tails_take_request(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *tails);
Bool tails_take_held_request(struct task_work *work,
                             struct player_motion *motion,
                             struct player *record, struct player *tails);
/* at a press of the action button, take an action of the record's list. take
 * its action, the current one, where the list holds it, and else the list's
 * first. action_taken records the action taken. an action with a setter here
 * starts, and the call returns TRUE. an action without one starts nothing, and
 * the call returns FALSE. without a press, or with the list empty, nothing
 * changes and the call returns FALSE. tails goes unread */
Bool tails_take_action(struct task_work *work, struct player_motion *motion,
                       struct player *record, struct player *tails);

/* the steps of his pick-ups. each steps him toward his offer first, by
 * player_approach_touched. he takes hold of hold_offer only where
 * player_face_offer grades it PLAYER_WALL_HEAD_ON. hold_offer must have a work
 * there, unchecked. taking hold makes it the held task, with
 * WORK_HELD_BY_PLAYER in its work. it plays a voice. his work takes
 * PLAYER_WORK_HOLDING and loses PLAYER_WORK_UNK_8.
 *
 * tails_test_pick_up is PLAYER_MODE_PICK_UP's. it returns TRUE only where it
 * takes hold, and FALSE otherwise. it waits for a frame of its animation. from
 * that frame on, it takes hold of an offer that player_action_flags gives
 * ACTION_FLAG_200. it plays a sound as it does, and starts
 * PLAYER_MODE_STAND_HOLDING. where it can't, it stands him by player_stand.
 *
 * tails_test_pull is PLAYER_MODE_PULL_BURIED's. it returns TRUE only where it
 * starts PLAYER_MODE_STAND_HOLDING, and FALSE otherwise, after a taking of hold
 * too. as a pass of ANIM_PULL's loop starts, the pull goes on to
 * ANIM_PULL_LIFT. it goes on always with the stick pushed, and by chance
 * without it. in ANIM_PULL_LIFT, holding nothing, he takes hold of an offer
 * that player_action_flags gives ACTION_FLAG_1000. its work's id becomes
 * FALL_KIND_NORMAL, and its flags take FALL_UNK_200. where he can't take hold,
 * he stands. he stands too where he holds a task that player_action_flags gives
 * nothing. in ANIM_HOLD_STAND, which follows the lift, ACTION_FLAG_200 starts
 * PLAYER_MODE_STAND_HOLDING */
Bool tails_test_pick_up(struct task_work *work, struct player_motion *motion,
                        struct player *record);
Bool tails_test_pull(struct task_work *work, struct player_motion *motion,
                     struct player *record);

/* the steps of his throws and of setting a task down. each lets go of the held
 * task in the frames of its animation that release it. it returns TRUE in those
 * frames, and FALSE outside them. letting go clears WORK_HELD_BY_PLAYER in the
 * task's work and PLAYER_WORK_HOLDING in his, and empties held. it plays a
 * voice, and a throw's sound too. a throw sets the task's motion's spd, but a
 * task without a work is let go unthrown. where held is null, each clears
 * PLAYER_WORK_HOLDING and returns TRUE. it returns FALSE there where the flag
 * was clear.
 *
 * a throw ahead gives the task a fixed speed in his own axes. a throw at a
 * target keeps his heading, and aims only its rise, at the target's first
 * shape.
 *
 * tails_test_throw throws ahead in ANIM_THROW and ANIM_AIR_THROW. in
 * ANIM_THROW_AT and ANIM_AIR_THROW_AT it throws at a target, from his pos. the
 * target is the first of his near targets. a nearer item or enemy after it
 * takes its place, where its id isn't FALL_KIND_NORMAL. in those two animations
 * the call returns FALSE while he has no near target. in ANIM_SET_DOWN it sets
 * the task down by player_set_down.
 *
 * tails_test_throw_at is PLAYER_MODE_THROW_AT_TOUCH's, in ANIM_THROW_AT_TOUCH.
 * it throws at the task of a touched shape of SHAPE_ID_FACED, from the held
 * task's pos. where he touches none, it throws ahead. task must be his, and
 * motion goes unread */
Bool tails_test_throw(struct task_work *work, struct player *record);
Bool tails_test_throw_at(struct task *task, struct task_work *work,
                         struct player_motion *motion, struct player *record);
/* keep the held task while player_action_flags gives it ACTION_FLAG_200. keep
 * it too while his collision record's latest touch is of it. let go of it
 * otherwise, by player_release_held. where held is then null, he stands on the
 * ground or a shape. in the air he falls, in PLAYER_MODE_FALL. with held null,
 * it does nothing. the work must have a collision record, unchecked */
void tails_drop_held(struct task_work *work, struct player_motion *motion,
                     struct player *record);

/* at a press of the action button off a path, start his tail attack,
 * PLAYER_MODE_TAILS_TAIL_ATTACK, and return TRUE. it returns FALSE otherwise,
 * and changes nothing */
Bool tails_test_tail_attack(struct task_work *work, struct player *record,
                            struct player *tails);
/* start his flight, PLAYER_MODE_TAILS_FLY, with TAILS_FLYING and a voice.
 * spd.y becomes 0, and PLAYER_WORK_ON_PATH, PLAYER_WORK_ATTACK and
 * PLAYER_WORK_BALL clear. motion goes unread */
void tails_fly_start(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *tails);
/* a step of his flight. his flap counts on, faster by climb, and its sound
 * plays as each flap ends. under PLAYER_WORK_NO_CONTROL nothing else changes.
 * otherwise the call clears climb, and sets the record's accel, in his own
 * axes. it sets motion's aim_y to the stick's heading, and turns him toward
 * aim_y. without the stick, aim_y becomes his heading. above running_speed, a
 * heading nearly behind him brakes him, and leaves aim_y as it was */
void tails_fly(struct task_work *work, struct player_motion *motion,
               struct player *record, struct player *tails);
/* set his collision shapes for the step. his body's shape attacks and guards
 * at 1 under PLAYER_WORK_BALL, and at 0 otherwise, whatever PLAYER_INVINCIBLE.
 * his other shapes are off, except in two modes. in his tail attack the tips
 * of his tails are on, at tip_plus_z and tip_minus_z. in his flight a shape
 * above him is on, and his body's shape narrows. the work must have a
 * collision record, unchecked */
void tails_set_shapes(struct task_work *work, struct player *record,
                      struct player *tails);

/* set TAILS_30B_UNK_1 in the unk_30b of the player in slot, read as Tails's
 * record whatever the player's character. each returns FALSE where the slot's
 * task has no extra, and TRUE otherwise. the slot must hold a task, unchecked.
 * the two are the same, and nothing calls either */
Bool fun_8c08dc00(int slot);
Bool fun_8c08dc24(int slot);

#endif
