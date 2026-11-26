/* the fall steps, which move a task's work under gravity and land it. fall.c
 * also defines words that several of the stages' objects share */
#ifndef OBJECT_FALL_H
#define OBJECT_FALL_H
#include <sg_xpt.h>

struct task;
struct task_motion;
struct task_work;

/* bits of a falling work's flags */
/* the work stands where a fall step landed it */
#define FALL_LANDED 0x1
/* a fall step lands the work only on a floor. a wall that its sweep meets
 * moves the work without a landing */
#define FALL_FLOOR_ONLY 0x2
#define FALL_UNK_8 0x8
/* the bits a fall step clears as it ends. fall_step_small leaves them under
 * WORK_HELD_BY_PLAYER. WORK_UNDER and WORK_ON_TOP are collision/collision.h's,
 * which a user of FALL_STEP_BITS includes */
#define FALL_STEP_BITS (FALL_UNK_8 | WORK_UNDER | WORK_ON_TOP)
/* fall_set_kind adds FALL_OFFERABLE and the kind's own bit. a player takes a
 * touched task for its offer only where the task's work has FALL_OFFERABLE.
 * each kind's bit has the value of the action flag player_action_flags gives
 * for the kind */
#define FALL_OFFERABLE 0x40
#define FALL_UNK_200 0x200 /* FALL_KIND_NORMAL's and FALL_KIND_HEAVY's */
/* FALL_KIND_11's: a player grabs such a task to push it, as
 * PLAYER_MODE_GRAB_PUSHABLE says */
#define FALL_PUSHABLE 0x400
#define FALL_SHAKABLE 0x800 /* FALL_KIND_SHAKE's */
#define FALL_BURIED 0x1000 /* FALL_KIND_BURIED's */

/* the work ids fall_set_kind gives. a player handles a task by its id, as
 * player_action_flags reads it. a player lifts and throws a FALL_KIND_NORMAL
 * task. it lifts a FALL_KIND_HEAVY one only where
 * player_offer_target_actions offers PLAYER_MODE_PICK_UP for it. otherwise it
 * pulls at the task without lifting it. player_shake_touched pushes a
 * FALL_KIND_SHAKE one. a player who pulls a FALL_KIND_BURIED task out of the
 * ground gives it FALL_KIND_NORMAL's id and FALL_UNK_200 */
#define FALL_KIND_NORMAL 10
#define FALL_KIND_11 11
#define FALL_KIND_HEAVY 12
#define FALL_KIND_SHAKE 13
#define FALL_KIND_BURIED 14

/* set work's id to kind, one of the FALL_KIND_ ids. add FALL_OFFERABLE and the
 * kind's bit to work's flags. it returns 1. where motion is null or kind is no
 * FALL_KIND_ id, it changes nothing and returns 0 */
int fall_set_kind(struct task_work *work, struct task_motion *motion, int kind);

/* the moving fall steps, each one step of task's work. each needs task's
 * motion, unchecked. each step subtracts its gravity from spd.y, which stays
 * within 6 of 0. it slows spd.x and spd.z a little. a landed work keeps a
 * tenth of spd.x and spd.z, or 0 where that tenth lies within 1 of 0.
 *
 * a step sweeps a sphere above pos along spd plus acc, by world_sweep_ball.
 * where the sweep meets a floor, or any surface without FALL_FLOOR_ONLY, the
 * work lands. pos then moves to put the sphere where the sweep stopped. ang
 * takes the tilt of the surface the sweep met. a wall under FALL_FLOOR_ONLY
 * moves pos the same way, without a landing. a sweep that meets nothing
 * moves pos by spd alone. where the work doesn't land, ang.x and ang.z become
 * 0. the step then calls collision_register, and clears FALL_STEP_BITS and
 * acc */

/* the sphere's radius is 2.5, with its center 2.5 above pos. the gravity is
 * 0.02. the step writes it to acc.y in place of the caller's, or 0 once
 * landed. a wall under FALL_FLOOR_ONLY also cuts spd.x and spd.z to a tenth.
 * a work under WORK_HELD_BY_PLAYER only loses FALL_LANDED */
void fall_step_small(struct task *task);
/* the sphere's radius is 5, with its center 5 above pos, and the gravity is
 * 0.1. a landed work moves only while acc isn't 0. a work under
 * WORK_HELD_BY_PLAYER still moves, but the step skips collision_register */
void fall_step_medium(struct task *task);
/* the sphere's radius is 5, with its center 15 above pos, and the gravity is
 * 0.1. a landed work doesn't move. a work under WORK_HELD_BY_PLAYER doesn't
 * move, and loses FALL_LANDED. the step then skips collision_register */
void fall_step_large(struct task *task);
/* call collision_register, and clear FALL_STEP_BITS, and acc where task has
 * a motion. the work doesn't move */
void fall_step_fixed(struct task *task);

/* the request every dummy object serves. the program writes only 0 to
 * command, count, show and pending. only a writer from outside the program,
 * such as a debugger, makes a request */
struct dummy_request {
    /* a command in the low half, and in the high half the id of what it
     * loads. the step that serves a command sets command to 0 */
    Uint32 command;
    /* FALSE while a dummy object's step serves a command. a command's end
     * sets it to TRUE, and so does a dummy object's start */
    Bool ready;
    Sint32 count; /* how many textures a texture command makes room for */
    Bool show; /* whether the dummy objects draw what they have loaded */
    Sint32 unk_10;
    /* while pending is TRUE, a dummy object's step frees what the dummy
     * objects loaded. it then serves commands until pending is FALSE */
    Bool pending;
    Bool busy; /* TRUE while a dummy object's step serves commands */
};

/* Source buffer for dummy-object chunks, PVMs and texture animations. */
extern Uint8 *dummy_object_buffer;
extern struct dummy_request dummy_request;
extern Sint32 dummy_object_count; /* how many dummy objects live */
/* how many fronts of out-of-range safes stg08's cash boxes keep */
extern Sint32 far_safe_count;
/* bit 0 is set once the dynamite's dynamite_init has emptied the dynamite's
 * table of charges */
extern Sint32 dynamite_flags;
/* bits the stages' objects keep. the low byte holds a bit for each flute
 * stand, by the stand's id. a player's play at a stand sets its bit. the Chao
 * boxes record their releases in 0x100 and 0x200 */
extern Uint32 kept_object_bits;
/* the Chao boxes' bits of kept_object_bits: the first box's release of four
 * animals, and the second's of one */
#define CHAO_BOX_FOUR_ANIMALS 0x100
#define CHAO_BOX_LAST_ANIMAL 0x200
/* clear the switch flags, dummy_request, far_safe_count and dynamite_flags.
 * off the relay's stages, clear kept_object_bits's low byte too */
void object_states_clear(void);
/* zero kept_object_bits */
void kept_object_bits_clear(void);

#endif
