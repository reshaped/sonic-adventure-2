/* what Tails's and Eggman's mechs fire, the effects they make, and their
 * gauge */
#ifndef PLAYER_WALKER_WEAPONS_H
#define PLAYER_WALKER_WEAPONS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
struct ground_probe;

/* the id of the laser's shape. player/action.c looks for it to give the
 * laser's hit LASER_DAMAGE */
#define WALKER_LASER_SHAPE 8

/* each thing a call makes is a task, made only where the heap gives it. a
 * shot, what a call fires, writes through null where the heap can't give it a
 * collision record. a bullet and a shell do too where it can't give their
 * struct enemy_work. a slot a call takes must hold a mech while what the call
 * makes lasts, unchecked. once game_state.winner_posed is 1, what a call makes
 * ends. a tracer, and a missile not yet set off, don't test it */

/* make a tracer between from and to. it is a streak that fades out over 14
 * steps, added to what lies behind it. it is a task named exec in level 3. it
 * is paler where the player has UPGRADE_TWALKER_LAZERBALSTER or
 * UPGRADE_EWALKER_POWERGUN */
void walker_tracer_create(int player, NJS_POINT3 *from, NJS_POINT3 *to);

/* fire a bullet from pos that moves by spd each step. spd must not be the zero
 * vector, unchecked. the bullet is parent's child, or for a null parent a task
 * named WalkerBulletExecutor in level 3. gunfire_create makes a flash at pos,
 * even where the heap can't give the bullet.
 *
 * the bullet's shape hurts what it touches, but the players through the
 * bullet's first two steps. the bullet meets the stage by enemy_meet_stage, as
 * an enemy does. it hits where it lands, meets a wall, touches the laser's
 * shape or is pushed by another shape. it hits too where its work's flags hold
 * WORK_HURT or WORK_UNK_8. the step after a hit gives off powder, makes a flash
 * and a sound, and ends the bullet. the powder is that of the player parent
 * belongs to, or player 0's. the bullet ends too once placement_try_leave finds
 * it out of range, and while game_state.steps is 1 or less. it draws with the
 * textures of player 0's mech */
void walker_bullet_create(struct task *parent, NJS_VECTOR *spd,
                          NJS_POINT3 *pos);
/* fire a shell as walker_bullet_create fires a bullet, without the flash at
 * pos. a shell without a parent is named WalkerBazookaExecutor. it makes
 * gunfire_create_brief's flash at each move. its hit makes an explosion in
 * place of the bullet's flash. it draws as the shell of player 0's mech, and
 * draws nothing where player 0 is no mech */
void walker_bazooka_create(struct task *parent, NJS_VECTOR *spd,
                           NJS_POINT3 *pos);

/* make a line between from and to as parent's child, added to what lies behind
 * it. parent must be a mech's task, unchecked. the line's color dims as
 * parent's lock_window falls, from a dim red above 120. each of the line's
 * first five steps dims it further, and its sixth step ends it. it ends too
 * while game_state.steps is 1 or less */
void walker_lockon_line_create(struct task *parent, NJS_POINT3 *from,
                               NJS_POINT3 *to);

/* the puffs: flashes, smoke and dust. each puff is a task named exec in level
 * 5, drawn as a quad that faces the camera. it shows eight textures of the
 * effects_texlist of player's mech in turn, four steps each. its 32nd step ends
 * it. a run of puffs next to each other in the level draws with the first
 * one's list. each step moves a puff by its speed, and then keeps 0.95 of the
 * speed */

/* make a flash behind pos and smoke at pos. the flash flies back against spd
 * and gives off powder. the smoke drifts along spd. spd must not be the zero
 * vector, unchecked */
void walker_back_blast_create(int player, NJS_VECTOR *spd, NJS_POINT3 *pos);
/* make a flash and two puffs of smoke at pos. the flash moves along spd and
 * gives off powder. the smoke drifts along spd. one puff rises against
 * world_gravity, and the other sinks with it */
void walker_muzzle_blast_create(int player, NJS_VECTOR *spd, NJS_POINT3 *pos);
/* make a puff of dust at pos. it starts at a speed of spd less 0.08 of
 * world_gravity */
void walker_puff_create(int player, NJS_VECTOR *spd, NJS_POINT3 *pos);
/* make dust on the ground below pos, at ground's below_y. where that lies
 * within 30 of pos, it makes two puffs. where it lies farther, but within 40,
 * it makes one smaller puff. each puff moves out along the ground's tilt, at a
 * random heading */
void walker_ground_dust_create(int player, NJS_POINT3 *pos,
                               struct ground_probe *ground);
/* make two puffs of dust at pos. each moves at a random 0.35 up to 0.5 of
 * spd */
void walker_foot_dust_create(int player, NJS_POINT3 *pos, NJS_VECTOR *spd);

/* walker_jet_create's kinds. the flame's life starts at 1, and each step takes
 * its fade. JET_SMALL fades by 0.17 a step, and is 0.12 up to 0.15 in size.
 * JET_LARGE fades by 0.12, at 0.27 up to 0.32. JET_LASTING, which no call
 * makes, and any other kind fade slowest, by 0.1, at 0.15 up to 0.19.
 * JET_SCALED fades by 0.25, at the call's scale times 0.12 up to 0.15 */
#define JET_SMALL 0
#define JET_LARGE 1
#define JET_LASTING 2
#define JET_SCALED 3

/* make a flame of the jet of the mech in slot player. it starts at pos and
 * moves by spd each step. the flame is the slot's task's child. where the slot
 * holds no task, it is a task named WalkerJetExecutor in level 3. the flame is
 * a quad that faces the camera, added to what lies behind it. it shows one of
 * four textures, at random. its color darkens and its alpha falls as its life
 * fades. it holds still and keeps its life while game_state.time_stopped has
 * bit player set. the step that finds its life at 0.05 or below destroys it. a
 * flame without a parent also ends once placement_try_leave says it has left
 * range. it reads through two nulls where the slot holds no task: the slot's
 * task and its own parent */
void walker_jet_create(int player, int kind, NJS_POINT3 *pos, NJS_VECTOR *spd,
                       Float scale);

/* draw the gauge of the mech in slot player at the foot of the screen. it is a
 * frame, a bar, and a meter above the bar. a full gauge is 10, or 12 with
 * UPGRADE_EWALKER_EXTRASHIELD. the bar's length is the gauge's share of a full
 * one. the bar is black at 0 or below, then red up to 2, yellow up to 6, cyan
 * up to 10 and blue above. the meter is as long as a full bar. its orange runs
 * from the bar's start across the gauge's fraction of the meter, and black
 * fills the rest. the fraction is the gauge less the gauge rounded toward 0. a
 * gauge below 0 that isn't whole has a fraction below 0, which runs the orange
 * left of the bar's start. a two-player game draws slot 0's gauge on the left
 * half of the screen, and any other's on the right.
 *
 * nothing draws while game_state.unk_0b is 0, game_state.cleared or
 * game_state.mini_event_running is 1, or game_state.solo_player isn't 0.
 * nothing draws either while the slot's work has an id other than
 * WORK_ID_PLAYER. a draw leaves the fog on */
void walker_gauge_draw(int player);

/* fire the laser of the mech in slot player, as the child of the slot's task.
 * the laser is one of a mech's special moves in a two-player game. its shape
 * is a capsule of id WALKER_LASER_SHAPE, 1000 long between its round ends. it
 * runs from the mech's laser_origin along its laser_dir, which must be a unit
 * vector, unchecked. while the slot's work stays in PLAYER_MODE_MECH_LASER, the
 * laser follows the two. its radius then widens from 5 toward 35 by 0.5 a step.
 * once the work leaves the mode, the laser fades for good. its shape stops
 * counting, and its radius falls by 1 a step to 0, where it ends. it ends too
 * once the slot holds no work, and while the stage is in STAGE_FADE_IN */
void walker_laser_create(int player);

/* the homing shots: a volley's shots, a seeker and the missiles. victim is the
 * slot of the player a shot flies at. a shot's shape counts from the sixth step
 * of its flight. the shot ends where its shape touches another. it ends too
 * once placement_try_leave_by finds it out of a range of 500 */

/* fire a volley of 16 shots from pos at the victim, as parent's children. a
 * null parent fires none, and the first shot the heap can't give ends the
 * volley. dir must be a unit vector, and not straight up or down, unchecked.
 * shot i sets off 33.75 to 56.25 degrees off dir, turned i sixteenths of a turn
 * about it. it homes, with swerves, on the center of the victim's first shape
 * as it was at the start. within 4 of where that center is now, it sets
 * WORK_HURT in the victim's work and ends. it ends too while game_state.steps
 * is 1 or less. it leaves a tracer at each move */
void walker_volley_create(struct task *parent, int victim, NJS_VECTOR *dir,
                          NJS_POINT3 *pos);
/* fire one seeker from pos along dir at the victim. nothing calls it. dir must
 * not be the zero vector, unchecked. the seeker is parent's child. for a null
 * parent it is a task named exec in level 3. that seeker ends at its first
 * step. a seeker homes on the center of the victim's first shape. its speed
 * eases from 2 toward 5 by 0.04 a step. it draws as a debug sphere, and its
 * shape takes MECH_STRIKE_SHAPE's id. it ends too once the victim's slot
 * holds no work */
void walker_seeker_create(struct task *parent, int victim, NJS_VECTOR *dir,
                          NJS_POINT3 *pos);
/* fire 18 missiles at the victim, as parent's children. a null parent fires
 * none, and a missile the heap can't give is passed over. the missiles set off
 * one a step, missile i at its step i + 1. missile i sets off from
 * missile_points[i % 6] of the mech parent belongs to, along the mech's
 * missile_dir. it homes on where the victim stood 2 steps before, as
 * player_get_past gives it. within 4 of that mark it sets WORK_HURT in the
 * victim's work and ends.
 *
 * to burst is to make an explosion and end. a missile bursts within 3 of the
 * land on any of its six sides. it bursts too where its shape touches another,
 * and at the 422nd step of its flight. on STAGE_VERSUS_MECH_2 the land doesn't
 * burst it. it bursts there outside a cylinder of radius 190 about the y axis.
 * it ends without a burst while the stage is in STAGE_FADE_IN. it leaves a
 * tracer at each move */
void walker_missiles_create(struct task *parent, int victim);

/* make a ricochet: a lock-on shot that glances off at pos. it is a task named
 * exec in level 3. each step moves it by 3 times dir turned at random, which
 * the call picks. the turn about y is 202.5 to 225 degrees. the turns about x
 * and z are up to 22.5 degrees either way. it draws as a sprite, and hurts
 * nothing. where the player has UPGRADE_TWALKER_LAZERBALSTER or
 * UPGRADE_EWALKER_POWERGUN, the sprite takes a cell of its character's own. the
 * shot leaves a tracer at each move. it ends once placement_try_leave_by finds
 * it out of a range of 400 */
void walker_ricochet_create(int player, NJS_POINT3 *pos, NJS_VECTOR *dir);

#endif
