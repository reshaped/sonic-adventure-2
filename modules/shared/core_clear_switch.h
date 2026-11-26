/* the clear switch, CLEAR SW, of Rouge's and Knuckles's routes in Cannon's
 * Core. GOAL DOOR starts its sequence once the switch's mode is SWITCH_DOWN */
#ifndef SHARED_CORE_CLEAR_SWITCH_H
#define SHARED_CORE_CLEAR_SWITCH_H
#include <ninja.h>

struct task;
enum clear_switch_mode { SWITCH_WAITING, SWITCH_SINKING, SWITCH_DOWN };
#define SWITCH_NO_CHANGE (-1)

/* the switch's state, at its task's extra */
struct clear_switch {
    enum clear_switch_mode mode;
    int next_mode; /* the mode the next step enters, or SWITCH_NO_CHANGE */
    int timer; /* the steps since the switch entered its mode */
    Float height; /* the plate's height above pos, 0 or below */
    NJS_OBJECT *
        collision; /* the switch's object in the world, from the world's pool */
};

#define CLEAR_SWITCH(task) ((struct clear_switch *)(task)->extra.p)
/* the record of the task's force that carries the switch's rider */
#define FORCE(task) ((struct task_force *)(task)->force.p)

/* the latest switch's task. it is null before the first switch, and once a
 * switch is destroyed */
extern struct task *core_clear_switch;
/* the steps the plate takes to sink */
extern int core_switch_sink_steps;
/* SWITCH_SINKING's step, and SWITCH_DOWN's entry and step, which
 * core_switch_motion.c defines */
void core_switch_sink(struct task *task);
void core_switch_enter_down(struct task *task);
void core_switch_hold(struct task *task);

#endif
