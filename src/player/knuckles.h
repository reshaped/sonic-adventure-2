/* the treasure hunters: Knuckles and Rouge, and Tikal and Chaos 0 in their
 * places in a two-player game. their makers, their animations, and the calls
 * of their task */
#ifndef PLAYER_KNUCKLES_H
#define PLAYER_KNUCKLES_H
#include <sg_xpt.h>

struct player;
struct player_motion;
struct task;
struct task_work;

/* make Knuckles, or Rouge, the player in slot: a task named
 * KnucklesTheEchidna, whose extra is the character's record. in a two-player
 * game, a slot whose game_state.alt_character isn't 0 takes Tikal in
 * Knuckles's place, or Chaos 0 in Rouge's. only a two-player game reads
 * game_state.costume, which picks Knuckles's and Rouge's files. a demo or a
 * two-player game gives the player UPGRADE_KNUCKLES_CLAWS or
 * UPGRADE_ROUGE_NAILS alone. otherwise slot 1 on STAGE_VERSUS_KNUCKLES takes
 * no upgrade.
 *
 * each maker starts the hunt by hunt_start, before it makes the task. where
 * task_create or syCalloc fails, no task is left. a task made starts the
 * slot's action window. the loads go unchecked */
void knuckles_create(int slot);
void rouge_create(int slot);

/* the finds a dig can turn up, by their index in each table of chances */
#define HUNTER_FIND_NOTHING 0
#define HUNTER_FIND_SKULL 1 /* a skull, which flies up from the hunter */
#define HUNTER_FIND_10_RINGS 2
#define HUNTER_FIND_20_RINGS 3
#define HUNTER_FIND_5_RINGS 4
#define HUNTER_FIND_BARRIER 5
#define HUNTER_FIND_BOMB 6 /* an explosion at the hunter */
#define HUNTER_FIND_MAGNETIC_BARRIER 7
#define HUNTER_FIND_SPEED_UP 8
#define HUNTER_FIND_LIFE 9
#define DIG_FINDS 10

/* the three tables of a dig's chances: a one-player game's, a two-player
 * game's, and MISSION_RINGS' in either. each chance from HUNTER_FIND_SKULL on
 * is its find's. HUNTER_FIND_NOTHING takes what they leave, whatever its own */
extern Float dig_find_chances[DIG_FINDS];
extern Float dig_find_chances_two_players[DIG_FINDS];
extern Float dig_find_chances_rings[DIG_FINDS];

/* the hunters' animations that the sources name, beside player/player.h's.
 * Knuckles's table and Rouge's hold each. Tikal plays Knuckles's table, and
 * Chaos 0 plays Rouge's */
#define ANIM_SPIN 11 /* a spin as a ball, paced by anim_speed */
#define ANIM_NO_UPGRADES 30 /* the draws show no upgrade while it plays */
#define ANIM_FALL_JUMP 68 /* a jump from a fall or a hurt */
/* a dig into the ground, the jump out of it, and that jump's fall. then a
 * dig's refusal */
#define ANIM_DIG 90
#define ANIM_DIG_JUMP 92
#define ANIM_DIG_JUMP_FALL 93
#define ANIM_DIG_FAIL 94
/* a dig into a wall, its loop, the climb out of it, and its refusal */
#define ANIM_DIG_WALL 95
#define ANIM_DIG_WALL_LOOP 96
#define ANIM_DIG_WALL_END 98
#define ANIM_DIG_WALL_FAIL 99
/* the climb's: moving along the hunter's x, moving along its -z or its z,
 * and still. hunter_climb_anim picks among them */
#define ANIM_CLIMB_X 100
#define ANIM_CLIMB_MINUS_Z 101
#define ANIM_CLIMB_Z 102
#define ANIM_CLIMB_STILL 103
#define ANIM_CLIMB_OVER 104 /* up over a wall's top, then ANIM_FALL */
#define ANIM_GLIDE 105
/* the grab of a wall, which ends a glide into it and starts a climb */
#define ANIM_GRAB_WALL 106
#define ANIM_GLIDE_LAND 107
/* a jump out of a glide. a press of jump doesn't glide while it plays */
#define ANIM_GLIDE_JUMP 108
/* the attack chains': on the ground, then on the run */
#define ANIM_ATTACK_1 109
#define ANIM_ATTACK_2 110
#define ANIM_ATTACK_3 111
#define ANIM_RUN_ATTACK_1 112
#define ANIM_RUN_ATTACK_2 113
#define ANIM_RUN_ATTACK_3 114
#define ANIM_DIVE 115
#define ANIM_DIVE_LAND 116 /* a dive's landing, except for a Rouge look */
/* PLAYER_MODE_HUNTER_SWIM's, and PLAYER_MODE_HUNTER_WATER_DIVE's */
#define ANIM_SWIM 117
#define ANIM_WATER_DIVE 118
/* PLAYER_MODE_HUNTER_FLOAT's. while the record's unk_224 lies under the
 * surface, the float plays ANIM_FLOAT_UNDER_START and then ANIM_FLOAT_UNDER.
 * it plays ANIM_FLOAT while unk_224 lies above */
#define ANIM_FLOAT_UNDER_START 120
#define ANIM_FLOAT_UNDER 121
#define ANIM_FLOAT 122
/* the knock back that knocks the hunter down, where request_arg asks for
 * ANIM_KNOCKED_DOWN, and its end */
#define ANIM_KNOCKED_DOWN 123
#define ANIM_KNOCKED_DOWN_END 124
#define ANIM_SPIRAL 127
/* the pose each hunter strikes as its earthflame or its power flash
 * starts, the sunglasses' and the earthquake's */
#define ANIM_KNUCKLES_SPECIAL 134
#define ANIM_KNUCKLES_SUNGLASSES 135
#define ANIM_KNUCKLES_EARTHQUAKE 138
#define ANIM_ROUGE_SPECIAL 176
#define ANIM_ROUGE_SUNGLASSES 177
#define ANIM_ROUGE_EARTHQUAKE 178
/* the entry of Rouge's table whose motion her ANIM_DIVE plays */
#define ANIM_ROUGE_DIVE 163
#define ANIM_TURTLE_STAND 211 /* a stand on a WORK_ID_TURTLE task */
#define ANIM_ROUGE_RIDE_SWIMMER 212

/* the actions of the sunglasses and of the two-player attacks, whose setters
 * start no mode of their number */
#define ACTION_SUNGLASSES 82
#define ACTION_EARTHQUAKE 83
#define ACTION_EARTHFLAME 84
#define ACTION_POWER_FLASH 85

/* the calls below are the hunters' task's. each takes one hunter's work,
 * motion and record, unchecked. where a call takes hunter too, hunter must be
 * the record again. a test or an offer returns TRUE where it starts a mode or
 * offers an action. it returns FALSE otherwise */

/* leave a fading copy of the hunter, as player_afterimage does.
 * hunter_spin_afterimage leaves one each second step of the dive, from
 * the character's spin model where it is loaded. hunter_afterimage_slow
 * leaves one each fourth step of the spiral, of the animation playing. its
 * model is the character's afterimage model in place of the body, where
 * that is loaded */
void hunter_spin_afterimage(struct task_work *work, struct player *record,
                            struct player *hunter);
void hunter_afterimage_slow(struct task_work *work, struct player *record,
                            struct player *hunter);

/* the draws of the hunters' task. knuckles_draw draws a Knuckles or a Tikal,
 * and rouge_draw a Rouge or a Chaos 0. each draws the body with its jiggles,
 * its upgrades and its modifier volumes. each keeps the places of the body's
 * nodes in the record, unk_1e8 to unk_230. it keeps their axes there too,
 * unk_26c to unk_2cc. it then puts the held task at the hands, where
 * player_action_flags gives the task ACTION_FLAG_200.
 *
 * in a split screen, the other player's camera leaves out a hunter beyond
 * its view. while game_state.solo_player isn't 0, only that player's task
 * draws, and the body doesn't blink. otherwise it blinks as a hit's timer
 * counts down. outside PLAYER_MODE_PLAY_ANIM the work must have a collision
 * record, unchecked */
void knuckles_draw(struct task *task);
void rouge_draw(struct task *task);

/* a step of PLAYER_MODE_PATH_HANG or PLAYER_MODE_PATH_HANG_GO, the hang
 * along a path. it moves the work's pos down the hunter's up by the record's
 * height. while the record's drive isn't 0, the hang's frame steps toward the
 * next hold. it steps up for a spd.x above 0, and down for one below. a hold
 * the hunter reaches without the stick stops it there, in
 * PLAYER_MODE_PATH_HANG with spd.x 0. while drive is 0, the frame stays, and a
 * hunter at a hold stops there the same way */
void hunter_climb_step(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* start a glide. spd.x becomes at least running_speed, and spd.y and
 * glide_roll become 0. the character's sound of a glide's start plays. the
 * caller sets the mode */
void hunter_start_glide(struct player *record, struct player *hunter);
/* a fall turns to a glide once jump has been held for more than GLIDE_HOLD
 * steps. a scrape adds GLIDE_SCRAPE to the record's glide_scrape, and the
 * glide falls once that reaches GLIDE_SCRAPE_END. a glide's draw eases
 * glide_roll by up to GLIDE_ROLL_STEP a step */
#define GLIDE_HOLD 60
#define GLIDE_SCRAPE 12
#define GLIDE_SCRAPE_END 60
#define GLIDE_ROLL_STEP 0x800
/* jump, in PLAYER_MODE_JUMP with jump_steps at 0. from PLAYER_MODE_FALL or
 * PLAYER_MODE_HURT the jump plays ANIM_FALL_JUMP, and from
 * PLAYER_MODE_HUNTER_GLIDE ANIM_GLIDE_JUMP, each keeping spd.y. from any other
 * mode it plays ANIM_JUMP with spd.y at jump_speed. a Knuckles or a Rouge look
 * then cries out, except in deep water. every jump except a glide's plays the
 * character's sound of a jump, or in deep water a splash. where the ground_task
 * has a force, the motion's acc takes its speed */
void hunter_jump(struct task_work *work, struct player_motion *motion,
                 struct player *record);
/* at a press of jump, jump as hunter_jump does. a hunter holding a task
 * that player_action_flags gives ACTION_FLAG_200 jumps with it, in
 * PLAYER_MODE_JUMP_HOLDING. one holding any other task lets go of it first */
Bool hunter_test_jump(struct task_work *work, struct player_motion *motion,
                      struct player *record);
/* at a press of jump, leap off the wall the hunter climbs. it turns level,
 * heading along its up as it was. its pos moves 5 out along that up, then 5
 * down. it then jumps as hunter_jump does, from a spd.x of a share of
 * jump_speed, with jump_steps at jump_hold_steps */
Bool hunter_test_wall_jump(struct task_work *work, struct player_motion *motion,
                           struct player *record);

/* answer the request that PLAYER_WORK_REQUEST flags in the work's submode.
 * hunter_take_request answers for a hunter holding nothing, and
 * hunter_take_held_request for one holding a task. each returns TRUE where
 * it answers. each returns FALSE without PLAYER_WORK_REQUEST, and for a
 * request it has no answer to. PLAYER_WORK_REQUEST stays set.
 *
 * PLAYER_REQUEST_JOIN_PATH returns FALSE too. first it sets
 * PLAYER_WORK_ON_PATH above running_speed. it sets it too in PLAYER_MODE_RUN
 * for a hunter holding nothing. it sets it in PLAYER_MODE_UNK_19 for one
 * holding a task.
 *
 * six answers keep the held task: PLAYER_REQUEST_BOUNCE's,
 * PLAYER_REQUEST_LAUNCH's, PLAYER_REQUEST_UPDRAFT's, PLAYER_REQUEST_RELEASE's,
 * PLAYER_REQUEST_HAND's and PLAYER_REQUEST_BREATHE's. every other answer lets
 * go of it first, by player_release_held. PLAYER_REQUEST_HAND makes handed
 * the held task, and handed must not be null, unchecked. the task held
 * before keeps WORK_HELD_BY_PLAYER */
Bool hunter_take_request(struct task_work *work, struct player_motion *motion,
                         struct player *record, struct player *hunter);
Bool hunter_take_held_request(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record, struct player *hunter);
/* at a press of the action button, take an action of the record's list. it
 * takes the record's action, the current one, where the list holds it, and
 * else the list's first. action_taken records the action. an action with a
 * setter here starts, and the call returns TRUE. without a press, a hunter in
 * PLAYER_MODE_HUNTER_DIVE whose list holds PLAYER_MODE_HUNTER_DIG digs. it
 * stops the dive's sound, and the call returns TRUE. the call returns FALSE
 * otherwise */
Bool hunter_take_action(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *hunter);

/* offer the dive, PLAYER_MODE_HUNTER_DIVE, to a hunter in the air in
 * PLAYER_MODE_JUMP or PLAYER_MODE_HUNTER_GLIDE. it offers none in deep water,
 * or in ANIM_UNK_23 */
Bool hunter_offer_dive(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* the test of PLAYER_MODE_PICK_UP. each step turns the hunter toward its offer,
 * as player_approach_touched does. the hands take the task from a frame of the
 * animation on. from there the hunter takes hold of an offer that
 * player_action_flags gives ACTION_FLAG_200. player_face_offer must grade the
 * offer PLAYER_WALL_HEAD_ON. the hunter then holds it in
 * PLAYER_MODE_STAND_HOLDING, and the call returns TRUE. where it can't, it
 * stands */
Bool hunter_test_pick_up(struct task_work *work, struct player_motion *motion,
                         struct player *record);
/* the test of PLAYER_MODE_PULL_BURIED, a pull of a task out of the ground. it
 * turns the hunter toward its offer, as hunter_test_pick_up does. as a pass of
 * ANIM_PULL's loop starts, the stick moves the pull on to ANIM_PULL_LIFT.
 * without the stick, a chance does. in ANIM_PULL_LIFT, a hunter holding nothing
 * takes hold of a head-on offer that player_action_flags gives
 * ACTION_FLAG_1000. the task's work takes FALL_UNK_200, and its id becomes
 * FALL_KIND_NORMAL. where the hunter can't, it stands. so does one holding a
 * task that player_action_flags gives nothing. in ANIM_HOLD_STAND, which
 * follows the lift, ACTION_FLAG_200 starts PLAYER_MODE_STAND_HOLDING, and the
 * call returns TRUE. it returns FALSE otherwise, after a taking of hold too */
Bool hunter_test_pull(struct task_work *work, struct player_motion *motion,
                      struct player *record);

/* let go of the held task in the frames of its throw that release it, and
 * return TRUE. ANIM_THROW_AT and ANIM_AIR_THROW_AT throw it at the first of the
 * hunter's near targets. a nearer item or enemy whose id isn't FALL_KIND_NORMAL
 * takes its place. they let go of nothing while the hunter has no target.
 * ANIM_THROW and ANIM_AIR_THROW throw it ahead, and ANIM_SET_DOWN sets it
 * down. a task without a work is let go unthrown. with held null, the call
 * clears PLAYER_WORK_HOLDING and returns TRUE. it returns FALSE there where
 * the flag was clear */
Bool hunter_test_throw(struct task_work *work, struct player_motion *motion,
                       struct player *record);
/* ANIM_THROW_AT_TOUCH's throw, as hunter_test_throw throws. it throws at
 * the task of a touched shape of SHAPE_ID_FACED, or ahead where the hunter
 * touches none. task must be the hunter's */
Bool hunter_test_throw_at(struct task *task, struct task_work *work,
                          struct player_motion *motion, struct player *record);
/* keep the held task while player_action_flags gives it ACTION_FLAG_200, or
 * while the hunter's collision last hit it. let go of it otherwise. a hunter
 * that then holds nothing stands on the ground, or falls in the air. with
 * held null, it does nothing. the work must have a collision record,
 * unchecked */
void hunter_drop_held(struct task_work *work, struct player_motion *motion,
                      struct player *record);
/* keep the stick's heading in the hunter's stick_history, and find a full
 * circle of it there. it offers only in PLAYER_MODE_STAND, PLAYER_MODE_RUN,
 * PLAYER_MODE_BRAKE, PLAYER_MODE_HUNTER_DIG_FAIL and PLAYER_MODE_SLIDE. after
 * a circle it offers the spiral, PLAYER_MODE_HUNTER_SPIRAL. otherwise it
 * offers PLAYER_MODE_HUNTER_ATTACK_1 up to running_speed, and
 * PLAYER_MODE_HUNTER_RUN_ATTACK_1 above it */
Bool hunter_offer_attack(struct task_work *work, struct player *record,
                         struct player *hunter);

/* a step of the attack chains, PLAYER_MODE_HUNTER_ATTACK_1 to
 * PLAYER_MODE_HUNTER_RUN_ATTACK_3. outside the third attacks it does nothing
 * in the air, while holding a task, or in the dive. otherwise it counts the
 * attack's steps. in a first or a second attack, a press of the action
 * button asks for the next. a release before a press ends the
 * chain. the first attack goes on as its animation ends, and the second as
 * its animation is about to. PLAYER_MODE_HUNTER_ATTACK_3 rises, and ends once
 * spd.y is 0 or below. PLAYER_MODE_HUNTER_RUN_ATTACK_3 lasts while the button
 * is held, between a least and a most of steps. an attack ends in a run on
 * the ground, or in a fall. TRUE where it changes the mode */
Bool hunter_attack_step(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *hunter);

/* what the glide's and the climb's probes find. each sweeps a ball whose
 * radius is half the hunter's height along the motion's spd. it starts that
 * radius ahead of the center of the hunter's first collision shape, along
 * ang.y */
#define HUNTER_PROBE_CLEAR 0 /* the ball meets nothing */
/* the ball meets what the hunter can't grab: an object of WORLD_MOVING,
 * GROUND_DEFLECTS or WORLD_NO_CLIMB, a floor or a ceiling. a wall that
 * player_collide doesn't stop the hunter at counts too */
#define HUNTER_PROBE_BLOCKED 1
/* the ball meets a wall that player_collide stops the hunter at */
#define HUNTER_PROBE_WALL 2
#define HUNTER_PROBE_FLAT 3 /* the ball meets an object of WORLD_FLAT */
/* hunter_probe_glide turns the hunter off what blocks it, or moves it to
 * rest against a ceiling. it returns HUNTER_PROBE_CLEAR while COLLISION_PUSHED
 * pushes the hunter. hunter_probe_climb finds the same without moving the
 * hunter, and returns HUNTER_PROBE_BLOCKED while it is pushed. each reads
 * through null for a work without a collision record.
 *
 * hunter_probe_steep sets a still ball at nine points on a grid about that
 * shape's center. the ball's radius is the hunter's height. it returns TRUE
 * where the ball meets more than one surface at a point, or a steep one. the
 * work must have a collision record, unchecked */
#define HUNTER_STEEP_Y 0.86F /* a steep surface's normal has a y below it */
int hunter_probe_glide(struct task_work *work, struct player_motion *motion,
                       struct player *record, struct player *hunter);
int hunter_probe_climb(struct task_work *work, struct player_motion *motion,
                       struct player *record);
Bool hunter_probe_steep(struct task_work *work, struct player_motion *motion,
                        struct player *record);
/* what each step of a climb at which hunter_probe_steep returns TRUE sets
 * climb_grace to, as struct player's hunter arm says */
#define CLIMB_GRACE 6

/* pick a climbing hunter's animation by its spd. it is ANIM_CLIMB_STILL off the
 * wall or still. it is ANIM_CLIMB_X while the hunter moves more along x than
 * along z. otherwise it is ANIM_CLIMB_Z or ANIM_CLIMB_MINUS_Z by z's sign, with
 * anim_speed at z's size. nothing changes in ANIM_GRAB_WALL, ANIM_CLIMB_OVER or
 * ANIM_DIG_WALL_END */
void hunter_climb_anim(struct task_work *work, struct player *record);
/* set the hunter's collision shapes for the step. the animation asked for
 * turns on the spheres it needs, and the others stay off. the body's shape
 * takes its attack and guard from the mode and the work's flags. the collision
 * record's radius then takes the reach of its farthest shape, with a margin.
 * nothing changes for a work without a collision record */
void hunter_set_shapes(struct task_work *work, struct player *record);
/* offer the sunglasses, ACTION_SUNGLASSES, in PLAYER_MODE_STAND while the
 * player's bit of game_state.sg_players is clear. it offers them to a
 * Knuckles with UPGRADE_KNUCKLES_SUNGLASS or a Rouge with
 * UPGRADE_ROUGE_SCOPE */
Bool hunter_offer_sunglasses(struct task_work *work, struct player *record);
/* offer the attack of the player's highest ring level, in a two-player game
 * or on STAGE_VERSUS_KNUCKLES. a level is earned where its
 * game_state.ring_levels byte is 1. ACTION_POWER_FLASH is the third's,
 * ACTION_EARTHFLAME the second's, and ACTION_EARTHQUAKE the first's. none is
 * offered while the player's ring_level_timer is above 0, after a clear, or
 * outside PLAYER_MODE_STAND and PLAYER_MODE_RUN */
Bool hunter_offer_ring_attack(struct task_work *work, struct player *record);

/* the step and the destructor of the power flash's task,
 * Knuckles2PPowerFlashMan, which hunter_power_flash starts. once the flash
 * shows, it takes the other player's pad, and that player's presses shorten
 * it. it ends once its time is up, the other player's slot is empty, or the
 * round is cleared. the destructor stops the flash's sound. it gives the pad
 * back, except after a clear */
void hunter_power_flash_step(struct task *task);
void hunter_power_flash_end(struct task *task);

/* start the power flash at the other of slots 0 and 1. it spends the
 * player's third ring level, ring_levels[slot][RING_LEVEL_POWER_FLASH]. the
 * hunter poses, and a Knuckles or a Rouge look says its voice. where
 * task_create fails, only the ring level is spent */
void hunter_power_flash(struct task_work *work, struct player_motion *motion,
                        struct player *record, struct player *hunter);

/* the digs' setters. each says its voice for a Knuckles or a Rouge look.
 * hunter_dig and hunter_dig_wall stop the hunter, and start a dig at depth 0.
 * hunter_dig starts PLAYER_MODE_HUNTER_DIG, in ANIM_DIG unless the hunter
 * dives. hunter_dig_wall starts PLAYER_MODE_HUNTER_WALL_DIG in ANIM_DIG_WALL.
 * hunter_dig_fail and hunter_dig_wall_fail zero the record's spd, but not the
 * motion's. they start PLAYER_MODE_HUNTER_DIG_FAIL in ANIM_DIG_FAIL, and
 * PLAYER_MODE_HUNTER_WALL_DIG_FAIL in ANIM_DIG_WALL_FAIL. ANIM_CLIMB_STILL
 * follows the second */
void hunter_dig(struct task_work *work, struct player_motion *motion,
                struct player *record, struct player *hunter);
void hunter_dig_wall(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *hunter);
void hunter_dig_fail(struct task_work *work, struct player *record);
void hunter_dig_wall_fail(struct task_work *work, struct player *record);
/* offer a dig to a hunter with UPGRADE_KNUCKLES_CLAWS or UPGRADE_ROUGE_NAILS.
 * it must stand, run or climb at a spd.x of 2 or less, or dive. it offers
 * PLAYER_MODE_HUNTER_DIG, or in a climb PLAYER_MODE_HUNTER_WALL_DIG, where
 * what the hunter stands on takes a dig. where that doesn't, it offers the
 * refusal, PLAYER_MODE_HUNTER_DIG_FAIL or PLAYER_MODE_HUNTER_WALL_DIG_FAIL.
 * it offers nothing off the ground, or while ANIM_GRAB_WALL is asked for.
 *
 * a shape takes a dig where the hunter's collision last hit a record whose
 * first shape has SHAPE_DIGGABLE. ground takes one under
 * PLAYER_WORK_ON_GROUND alone, where it is WORLD_DUSTY and not WORLD_MOVING.
 * on STAGE_SECURITY_HALL, such ground with a ground_task takes it only near
 * a spot above that task's pos. the task must then have a work,
 * unchecked */
Bool hunter_offer_dig(struct task_work *work, struct player_motion *motion,
                      struct player *record, struct player *hunter);
/* offer Rouge's own modes. she is offered PLAYER_MODE_ROUGE_RIDE_FERRY where
 * she stands or runs on the ground of a WORK_ID_TURTLE task. failing that, she
 * is offered PLAYER_MODE_ROUGE_RIDE_SWIMMER where she floats, swims or dives
 * and touches a swimming turtle's shape. the call returns TRUE where it offers
 * one, and FALSE for any other character. for a Rouge offered neither it
 * returns no value, which its one caller doesn't read.
 *
 * rouge_start_ride_swimmer is PLAYER_MODE_ROUGE_RIDE_SWIMMER's setter. it
 * stops her and starts the mode in ANIM_ROUGE_RIDE_SWIMMER */
Bool rouge_offer_actions(struct task_work *work, struct player *record);
void rouge_start_ride_swimmer(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record);

/* set the motion's acc to keep a floating hunter at the water's surface. the
 * surface is the above_y of the record's ground. the hunter rises toward a
 * surface above its head. it sinks by its gravity toward one below its feet.
 * between them the two mix by its depth. its spd.y brakes each rise,
 * and a random part of up to 0.0001 is added */
void hunter_float(struct task_work *work, struct player_motion *motion,
                  struct player *record);
/* the swim tests. deep water is WORLD_FLAT_THIN in the above_flags of the
 * record's ground, as collision/world.h says. its above_y must lie more than
 * WATER_DEEP from its below_y. hunter_in_deep_water tests for it alone.
 *
 * hunter_can_swim_off_ground is TRUE off the ground in deep water. the surface
 * must lie above the center of the record's first collision shape. it is FALSE
 * while the hunter holds an action button, unless it has just pressed jump.
 *
 * hunter_test_swim starts PLAYER_MODE_HUNTER_FLOAT in ANIM_FLOAT, in deep
 * water, on any stage except STAGE_CHAO. off the ground, any depth will do.
 * on the ground, the surface must lie more than 4 above that center. it starts
 * none while the hunter holds an action button. nor does it while the hunter
 * holds jump in the rise of PLAYER_MODE_JUMP */
Bool hunter_can_swim_off_ground(struct task_work *work,
                                struct player_motion *motion,
                                struct player *record);
Bool hunter_in_deep_water(struct task_work *work, struct player_motion *motion,
                          struct player *record);
Bool hunter_test_swim(struct task_work *work, struct player_motion *motion,
                      struct player *record);

/* turn up the find of a dig. a roll takes it from one table of chances. on
 * MISSION_RINGS it is dig_find_chances_rings, and in any other two-player game
 * dig_find_chances_two_players. it takes dig_find_chances otherwise. a dig
 * within 10 of the record's dig_end, where the last dig ended, finds
 * nothing. a find of rings adds them by ring_add, and to
 * game_state.ring_total and snapshot_current's.
 *
 * each find except nothing and a skull adds its item's icon by
 * player_hooks.add_item_icon, which must be set, unchecked. a Knuckles or a
 * Rouge look says a voice for each find except the barriers and a speed-up.
 * rings take no voice in a two-player game.
 *
 * nothing happens on STAGE_SECURITY_HALL, STAGE_BOSS_BIGBOGY and STAGE_CHAO.
 * nor does it where the hunter's collision has COLLISION_TOUCHED and a shape
 * it touched lacks SHAPE_DIGGABLE. each task it touched must have a work and
 * a collision record, unchecked */
void hunter_dig_find(struct task_work *work, struct player_motion *motion,
                     struct player *record, struct player *hunter);

/* TRUE where no kill area has a surface straight below or above the
 * hunter's pos, as polygon_find_model_heights finds one. only an area that
 * kills the hunter's character counts. FALSE where kill_areas is null */
Bool hunter_outside_kill_areas(struct task_work *work, struct player *record);
/* grab the wall the record's wall_normal gives, in PLAYER_MODE_HUNTER_CLIMB
 * and ANIM_GRAB_WALL. the hunter stops, takes PLAYER_WORK_ON_GROUND and loses
 * PLAYER_WORK_ATTACK. its ang.x and ang.z tilt it onto the wall, and it
 * moves to the point its ball hit. first a wall_normal.y within 0.001 of 0
 * becomes 0, and wall_normal is scaled to length 1. its x and z must not
 * both be 0, unchecked, nor the record's ball null. on STAGE_BOSS_FDOG the
 * grab plays a sound */
void hunter_grab_wall(struct task_work *work, struct player_motion *motion,
                      struct player *record);
/* on STAGE_VERSUS_KNUCKLES, bump the hunter back where collision_next_player
 * finds a player that its collision touched, and return TRUE. it tests only
 * while the other of slots 0 and 1 has a work. that work must lack WORK_HURT,
 * and be in neither PLAYER_MODE_HURT nor PLAYER_MODE_MISS.
 * collision_restart_scans runs before the test.
 *
 * the bump starts PLAYER_MODE_JUMP in ANIM_UNK_23, without
 * PLAYER_WORK_ATTACK. the hunter turns along its motion's level speed where
 * that isn't 0. it flies back and up in its own axes. player_hooks.flash, where
 * it is set, flashes at the record's unk_1e8 */
Bool hunter_test_bump(struct task_work *work, struct player_motion *motion,
                      struct player *record);

#endif
