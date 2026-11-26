/* Eggman on foot, the player of PLAYER_EGGMAN outside his walker. his create,
 * and the moves his step and draw call */
#ifndef PLAYER_EGGMAN_H
#define PLAYER_EGGMAN_H
#include <sg_xpt.h>

struct player;
struct player_motion;
struct task;
struct task_work;

/* the size in bytes of Eggman's record, which his task's extra points to. his
 * own fields are struct player's by_character.eggman */
#define EGGMAN_RECORD_SIZE 0x318

/* create Eggman's task, named Eggman, as the player in slot. it loads his
 * textures, models and motions, and starts the slot's action window. where
 * task_create or syCalloc fails, no task is left. the loads go unchecked */
void eggman_create(int slot);

/* the calls below take Eggman's work, motion and record. all three must be the
 * same player's, unchecked. where a call takes eggman too, eggman must be the
 * record again */

/* answer the request that PLAYER_WORK_REQUEST flags in the work's submode, as
 * Eggman holding a task, and return TRUE. it returns FALSE where no request
 * waits, or for one he has no answer to. PLAYER_REQUEST_JOIN_PATH returns FALSE
 * too. it sets PLAYER_WORK_ON_PATH where he is in PLAYER_MODE_UNK_19, or runs
 * above running_speed. PLAYER_WORK_REQUEST stays set.
 *
 * PLAYER_REQUEST_BOUNCE, PLAYER_REQUEST_LAUNCH, PLAYER_REQUEST_UPDRAFT,
 * PLAYER_REQUEST_RELEASE and PLAYER_REQUEST_HAND keep the hold. every other
 * answer that returns TRUE lets go first, by player_release_held. so held must
 * not be null under PLAYER_WORK_HOLDING, unchecked. PLAYER_REQUEST_HAND makes
 * handed the held task, and handed must not be null, unchecked. the task held
 * before keeps WORK_HELD_BY_PLAYER */
Bool eggman_take_held_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *eggman);
/* at a press of the action button, take an action of the record's list. take
 * its action, the current one, where the list holds it, and else the list's
 * first. action_taken records the action taken. an action with a setter here
 * starts, and the call returns TRUE. an action without one, as the flute's
 * actions are, starts nothing, and the call returns FALSE. without a press, or
 * with the list empty, nothing changes and the call returns FALSE. the record's
 * slot must be 0 or 1, unchecked, and eggman goes unread */
Bool eggman_take_action(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *eggman);
/* offer PLAYER_MODE_EGGMAN_ATTACK by OFFER, and return TRUE. it offers it only
 * to Eggman standing or running on the ground or a shape and holding nothing.
 * it returns FALSE otherwise */
Bool eggman_offer_attack(struct task_work *work, struct player *record);

/* the steps of his pick-ups. each steps him toward his offer first, by
 * player_approach_touched. he takes hold of hold_offer only where
 * player_face_offer grades it PLAYER_WALL_HEAD_ON. hold_offer must not be null
 * there, unchecked. taking hold makes it the held task, with
 * WORK_HELD_BY_PLAYER in its work. it plays a sound and a voice. his work takes
 * PLAYER_WORK_HOLDING and loses PLAYER_WORK_UNK_8.
 *
 * eggman_test_pick_up is PLAYER_MODE_PICK_UP's, and returns FALSE before a
 * frame of its animation. from that frame on, it takes hold of an offer that
 * player_action_flags gives ACTION_FLAG_200. it then starts
 * PLAYER_MODE_STAND_HOLDING and returns TRUE. where it can't, it stands him by
 * player_stand and returns FALSE.
 *
 * eggman_test_pull is PLAYER_MODE_PULL_BURIED's. as a pass of ANIM_PULL's loop
 * starts, the pull goes on to ANIM_PULL_LIFT where he takes his stick. without
 * the stick, it goes on by chance. in ANIM_PULL_LIFT, holding nothing, he takes
 * hold of an offer that player_action_flags gives ACTION_FLAG_1000. its work
 * becomes FALL_KIND_NORMAL, with FALL_UNK_200. where he can't, he stands, as he
 * does where he holds a task that player_action_flags gives nothing. in
 * ANIM_HOLD_STAND, which follows the lift, ACTION_FLAG_200 starts
 * PLAYER_MODE_STAND_HOLDING, and the call returns TRUE. it returns FALSE
 * otherwise, after a taking of hold too */
Bool eggman_test_pick_up(struct task_work *work, struct player_motion *motion,
                         struct player *record);
Bool eggman_test_pull(struct task_work *work, struct player_motion *motion,
                      struct player *record);

/* the steps of his throws and of setting a task down. each lets go of the held
 * task in the frames of its animation that release it. it returns TRUE then,
 * and FALSE outside them. letting go clears WORK_HELD_BY_PLAYER in the task's
 * work, clears PLAYER_WORK_HOLDING and empties held. it plays a voice, and a
 * throw's sound too. a throw sets the task's motion's spd, but a task without a
 * work is let go unthrown. where held is null, each clears PLAYER_WORK_HOLDING
 * and returns TRUE, or returns FALSE where the flag was clear.
 *
 * a throw ahead gives the task a fixed speed in his own axes. a throw at a
 * target keeps his heading, and aims only its rise, at the target's first
 * shape.
 *
 * eggman_test_throw throws in ANIM_THROW_AT, ANIM_THROW, ANIM_AIR_THROW_AT and
 * ANIM_AIR_THROW, and sets the task down by player_set_down in ANIM_SET_DOWN.
 * the throws at a target throw from his pos at the first of his near targets. a
 * nearer item or enemy whose id isn't FALL_KIND_NORMAL takes its place. they
 * return FALSE while he has none. the other two throw ahead.
 *
 * eggman_test_throw_at_touch is PLAYER_MODE_THROW_AT_TOUCH's, in
 * ANIM_THROW_AT_TOUCH. it throws from the held task's pos at the task of a
 * touched shape of SHAPE_ID_FACED. where he touches none, it throws ahead. task
 * must be his, and motion goes unread */
Bool eggman_test_throw(struct task_work *work, struct player *record);
Bool eggman_test_throw_at_touch(struct task *task, struct task_work *work,
                                struct player_motion *motion,
                                struct player *record);

/* keep the task he holds while player_action_flags gives it ACTION_FLAG_200.
 * keep it too while his collision record's latest touch is of it. otherwise let
 * go of it, by player_release_held. where held is then null, he stands on the
 * ground or a shape. in the air he falls, in PLAYER_MODE_FALL. with held null,
 * it does nothing. work must have a collision record, unchecked */
void eggman_check_hold(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* set the held task's pos by his hands, where player_action_flags gives the
 * task ACTION_FLAG_200. it reads the places his model's draw keeps in the
 * record, so it must follow that draw */
void eggman_place_held(struct task_work *work, struct player *record);
/* set his collision shapes for the step, where work has a collision record. his
 * first shape takes his body's radius. his body's shapes attack and guard at
 * their highest level under PLAYER_INVINCIBLE. they attack and guard at 1 under
 * PLAYER_WORK_BALL, and at 0 otherwise. his attack's two shapes are off outside
 * PLAYER_MODE_EGGMAN_ATTACK. in it, the first is on at his hand's unk_1e8
 * through his standing attack. the second is on at his foot's unk_200 through
 * his running one */
void eggman_set_shapes(struct task_work *work, struct player *record);

#endif
