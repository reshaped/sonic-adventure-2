/* tasks: the game's objects */
#ifndef SYSTEM_TASK_H
#define SYSTEM_TASK_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct collision_work;
struct placement;
struct task;
struct task_work;

/* task_levels has a head for each level. task_create links root tasks into
 * levels 0 to 6, since its level 7 means TASK_UNLINKED */
#define TASK_LEVELS 8
/* task_create's level for a task in no level, to link as a child */
#define TASK_UNLINKED 7

/* task_create's flags: the records it allocates with a task, zeroed, from
 * the Shinobi heap. task_destroy frees the records in work, motion, force
 * and extra with syFree. a record a task puts in one of them must come from
 * that heap too. a task that keeps a value in one must clear it in its
 * destructor */
#define TASK_MOTION 0x1
#define TASK_WORK 0x2
#define TASK_FORCE 0x4
#define TASK_EXTRA 0x8

/* TASK_FORCE allocates TASK_FORCES of struct task_force, and TASK_EXTRA a
 * record of TASK_EXTRA_SIZE bytes */
#define TASK_FORCES 2
#define TASK_EXTRA_SIZE 16

/* TASK_MOTION's record, as far as the sources read it */
struct task_motion {
    NJS_VECTOR spd; /* the task's velocity */
    /* what a fall step adds to spd for the move it tests */
    NJS_VECTOR acc;
    Uint8 unk_18[0x18];
    /* while a player holds the task, how far the player's draw turns the
     * nodes that hold it */
    Sint32 hold_turn;
    Uint8 unk_34[0x0c];
};
/* task's TASK_MOTION record, where its motion holds one */
#define TASK_MOTION_OF(task) ((struct task_motion *)(task)->motion.p)
/* one of TASK_FORCE's records: how far the task's object turned and moved
 * since the last step. a player riding the object turns and moves with it. the
 * player in slot 0 rides by the first record, and every other player by the
 * second. world_sweep_ball and camera/probe.c read only the first */
struct task_force {
    /* moves a riding player, whose task and work these are, or null */
    void (*apply)(struct task *task, struct task_work *work,
                  struct task_force *force);
    struct angles turn;
    NJS_VECTOR speed;
};
/* task's TASK_FORCE records, where its force holds them */
#define TASK_FORCES_OF(task) ((struct task_force *)(task)->force.p)

/* the values of a task_work's id that the sources name. other tasks test a
 * task's id to tell what the task is. fall_set_kind's kinds, 10 to 14, are
 * ids too */
enum work_id {
    WORK_ID_NONE = 0, /* the id task_create gives a work */
    WORK_ID_PLAYER = 1, /* the id player_register gives a player */
    /* a player's work that player_take_hit never hurts. nothing in the tree
     * gives a work this id */
    WORK_ID_NO_HIT = 2,
    /* a STAGE_VERSUS stage's player in slot 1, in place of WORK_ID_PLAYER */
    WORK_ID_RIVAL = 3,
    /* a task acting for the player whose slot is its work's submode */
    WORK_ID_PLAYER_PROXY = 5,
    /* an attack other than a player's own body */
    WORK_ID_ATTACK = 6,
    /* the blast a mech's lock-on shot becomes at its target */
    WORK_ID_LOCKON_BLAST = 7,
    /* a target that a mech's lock-on shot hurts and glances off, with no
     * blast */
    WORK_ID_LOCKON_GUARD = 8,
    /* an object that a homing attack strikes as it strikes an enemy */
    WORK_ID_HOMING_TARGET = 9,
    WORK_ID_RING = 17,
    /* a stop to a running player, as player_collide says */
    WORK_ID_STOP_RUN = 18,
    WORK_ID_SOAP_SWITCH = 20,
    /* a STOPLSD's, which stops a player dashing along a line of rings */
    WORK_ID_STOP_LSD = 21,
    /* a stop to a mech's lock-on, as a STOPLOCKON is. a mech whose nearest
     * target is one takes no lock */
    WORK_ID_STOP_LOCKON = 22,
    /* a WORK_ID_STOP_LOCKON that a mech measures as a box about its pos, of
     * half sizes scl */
    WORK_ID_STOP_LOCKON_BOX = 23,
    /* a ramp, which throws a fast Sonic or Shadow on along its angles */
    WORK_ID_RAMP = 24,
    /* Dry Lagoon's ferrying turtle, KAME, from its first stop on */
    WORK_ID_TURTLE = 25,
    /* Cannon's Core's CLEAR SW, which sinks once player 0 stands on a task
     * of this id */
    WORK_ID_CLEAR_SWITCH = 26,
    /* Hidden Base's KAITEN, which tests whether player 0 stands on a task of
     * this id. no stage holds both it and CLEAR SW */
    WORK_ID_SWING = 26,
    /* the three platforms on BOSS_GOLEM's Egg Golem. the golem's lowest
     * sensor collides only while player 0 stands on _B or _C */
    WORK_ID_GOLEM_PLATFORM_A = 27,
    WORK_ID_GOLEM_PLATFORM_B = 28,
    WORK_ID_GOLEM_PLATFORM_C = 29,
    /* the three platforms on the head of BOSS_GOLEM's Egg Golem. each
     * carries a target for a homing attack */
    WORK_ID_GOLEM_HEAD_PLATFORM_A = 30,
    WORK_ID_GOLEM_HEAD_PLATFORM_B = 31,
    WORK_ID_GOLEM_HEAD_PLATFORM_C = 32,
    /* a sensor of BOSS_GOLEM's Egg Golem. its object is a copy of a node's
     * model in the collision world, and rides the node */
    WORK_ID_GOLEM_SENSOR = 33
};

struct task_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 timer;
    struct angles ang;
    NJS_POINT3 pos;
    NJS_POINT3 scl;
    struct collision_work *collision;
};

/* one of struct task's words that a kind of task may fill its own way. a
 * source names the member that holds what it keeps there */
union task_word {
    void *p; /* a record */
    struct task *task;
    Sint32 l;
    Uint32 u;
    Float f;
    Sint16 w[2];
    Sint8 b[4];
};

/* a node in a tree of tasks. next and prev link a root task into its level's
 * ring, and a child into the ring of its parent's children. a task alone in
 * its ring is its own next and prev */
struct task {
    struct task *next;
    struct task *prev;
    struct task *parent;
    struct task *child; /* the first of its children, or null */
    void (*step)(struct task *task);
    void (*draw)(struct task *task);
    void (*destructor)(struct task *task);
    struct placement *placement; /* the placement that made it, or null */
    struct task_work *work;
    /* TASK_MOTION's, TASK_FORCE's and TASK_EXTRA's records, or a record or
     * value of the task's own in their place */
    union task_word motion;
    union task_word force;
    union task_word extra;
    const char *name; /* task_create's name, or null */
    /* the original copies the uninitialized name word here. The port sets
     * it to null. Nothing reads it. */
    const char *unk_34;
    /* the name of the task a mech's lock-on targets, which
     * walker_start_lockon keeps in the lock's task. nothing reads it */
    const char *lock_target_name;
    /* a word each kind of task uses its own way. task_create sets it to 0 */
    union task_word unk_3c;
};

/* each level's first task, or null for an empty level */
extern struct task *task_levels[TASK_LEVELS];
/* the step or draw a walk is calling, or the destructor task_destroy is
 * calling. it is null outside them. task_destroy leaves it null, so a step
 * or draw that destroys a task reads null from then on */
extern void (*task_current_call)(struct task *task);

/* the step task_create gives a task created with a null one */
void task_idle(struct task *task);
/* empty the levels without destroying their tasks */
void task_init(void);
/* a task from the Shinobi heap, with the records flags asks for, at the tail
 * of level's ring. a TASK_UNLINKED task goes into no ring, and the caller
 * sets its next and prev. a null step makes task_idle its step. returns null
 * when the heap runs out. where the task was allocated but a record wasn't,
 * task_create destroys the task before it returns. for a TASK_UNLINKED task,
 * the original destroy writes through the unset next and prev. The port
 * initializes an unlinked task as a singleton until the caller links it. */
struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name);
/* destroy task now. its destructor runs first, with task_current_call set to
 * it, while the task's records and children remain. task_destroy then destroys
 * the children, unlinks the task, frees its records and clears PLACEMENT_LIVE
 * in its placement. the task returns to the heap, and every pointer to it or
 * its records dangles.
 *
 * the original walk reads each task's next before its step or draw. so a
 * step or draw must not destroy the next task in its ring. a draw must not
 * destroy the first task of its walk either. that walk stops only when it
 * comes back to the first task.
 *
 * where a step destroys its own task, as task_kill's does, its walk reads child
 * from the freed block. task_destroy nulled child before the free. so the walk
 * is safe only while the heap leaves that word null. The port updates each
 * active walk when a task is removed and detaches its sound slots before
 * freeing storage that a sound update follows. */
void task_destroy(struct task *task);
/* destroy task on its next step, by making task_destroy its step */
void task_kill(struct task *task);
/* run the step of each task of level, from the head along next. each step is
 * followed by the steps of the task's children, depth first. the walk ends
 * where the next task is the level's head, or the level is empty */
void task_step_level(int level);
/* destroy every task of level. The port drains the current head. The
 * original reads each task's next before destroying it, so a destructor
 * must preserve the next task and a singleton's destructor must leave the
 * level empty. */
void task_destroy_level(int level);
/* task_step_level for levels 0 to 5, in order */
void task_step_all(void);
/* draw levels 0 to 5, each task before its children. levels 2 to 5 are left
 * out for the cameras game_state.hidden_cameras holds */
void task_draw_all(void);
void task_destroy_levels_2_to_5(void);
void task_destroy_all(void);
/* create an unnamed TASK_UNLINKED task as task_create does, and link it as
 * parent's last child. where both have a work, the child starts at the
 * parent's pos and angles. returns the child, or null where task_create
 * fails. parent must not be null, unchecked */
struct task *task_create_child(Uint16 flags, void (*step)(struct task *task),
                               struct task *parent);
/* destroy task's children now, unless task's step is task_destroy_children.
 * then it destroys none, and once task has no children, it makes
 * task_destroy task's step */
void task_destroy_children(struct task *task);

#endif
