/* the Egg Golem's tactics in BOSS_GOLEM.PRS. they time its attacks, aim and
 * turn it, hover it and make a blow's shock wave. boss_golem_e/tactics.h
 * declares the same calls for BOSS_GOLEM_E.PRS.
 *
 * each call but golem_player_facing_up and golem_holding reads golem, which
 * must not be null. a call that reads player 0's place reads player 0's work,
 * which must not be null. of the calls that take a task, golem_wave_create
 * makes its wave the task's child, and golem_holding reads it. the others
 * don't read it */
#ifndef BOSS_GOLEM_TACTICS_H
#define BOSS_GOLEM_TACTICS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* create a shock wave over the foothold at golem's slot, as parent's child, and
 * return its task. parent must be a live task, and state that foothold's. the
 * wave stands at golem_floor_y, GOLEM_FOOTHOLD_DROP lower for each state past
 * FOOTHOLD_WHOLE. a gone foothold's stands as FOOTHOLD_SUNK_TWICE's. its
 * ring spreads and its column rises, and both fade. it hurts players, and
 * kills its task at its 54th step. where the heap can't give the task or its
 * record, the call writes through null */
struct task *golem_wave_create(struct task *parent, int state);

/* set golem's rest_wait from its power and the misses and retries so far. the
 * wait is 90 steps and 15 for each unit of power, truncated to whole steps.
 * each miss and retry adds 90 */
void golem_set_rest_wait(void);
/* TRUE where golem has rested long enough to attack: its rest_steps pass the
 * wait. the wait is rest_wait while player 0 isn't near, and the near wait
 * while it is. FALSE while player 0 is low, or while up_steps falls short of
 * the up wait.
 *
 * the near wait is 12 seconds, and 1.5 more for each miss and retry so far.
 * it is 1 second less for each unit of power the golem has lost. the up wait
 * is 2.5 seconds, and 0.5 more for each miss and retry. it is 0.25 seconds
 * less for each unit of power lost. a second is 60 steps */
Bool golem_may_attack(struct task *task);
/* set golem's turn to turn, an enum golem_turn. a turn to GOLEM_TURN_TO_HAND_A
 * or GOLEM_TURN_TO_SLAM also aims GOLEM_HAND_A by golem_aim_slot, and one to
 * GOLEM_TURN_TO_HAND_B aims GOLEM_HAND_B. a turn outside the enum changes
 * nothing */
void golem_set_turn(struct task *task, int turn);
/* aim hand at the slot nearest player 0, by golem_set_slot. above
 * power_low, GOLEM_HAND_A aims at the slot before that one, and GOLEM_HAND_B
 * at the slot after it */
void golem_aim_slot(struct task *task, int hand);
/* turn golem's heading as its turn says, by 0x100 at most */
void golem_turn(struct task *task);
/* set golem's slot to slot, and its aim_heading to the heading that puts
 * hand over the slot. at power_low or below, the heading is the slot's
 * own */
void golem_set_slot(struct task *task, int slot, int hand);
/* TRUE where player 0 faces toward the slots of higher number. its facing,
 * as a heading, is QUARTER_TURN less its work's ang.y */
Bool golem_player_facing_up(void);
/* set golem's lift for the action it runs. the golem hops in
 * GOLEM_ACTION_FACE_PLAYER and the two GOLEM_ACTION_AIM_ actions. in
 * GOLEM_ACTION_REST it bobs while player 0 is near, and keeps its lift while
 * player 0 isn't. the bob is higher once golem's action_steps pass the near
 * wait less 300. in any other action the lift is 0 */
void golem_hover(struct task *task);
/* TRUE while the frame of the golem's motion lies above 25 and below 125. in a
 * motion that holds the hands down, those are the frames that hold them. task
 * is the golem's: its extra must hold the boss record whose block holds the
 * motion */
Bool golem_holding(struct task *task);
/* count golem's up_steps, or set them to 0 while player 0 is low */
void golem_count_up_steps(struct task *task);

#endif
