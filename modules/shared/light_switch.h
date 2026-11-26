/* the light switch, LIGHT SW in a stage's object list. its volume gives a
 * player inside it the light the switch names */
#ifndef SHARED_LIGHT_SWITCH_H
#define SHARED_LIGHT_SWITCH_H

struct task;

/* what a switch does to a player's light, by the low byte of the switch's
 * ang.y. a player inside takes the switch's light. light_camera indexes the
 * light the stage selects for the players it places */
enum light_switch_type {
    LIGHT_SWITCH_AREA, /* leaving restores the light from before entering */
    LIGHT_SWITCH_SWITCH, /* leaving keeps the switch's light */
    LIGHT_SWITCH_AREADEF, /* leaving selects the light light_camera indexes */
    LIGHT_SWITCH_TYPES
};

/* a switch's volume about its work's pos, by the low two bits of the switch's
 * ang.z. a shape of 3 holds no player */
enum light_switch_shape {
    LIGHT_SWITCH_CUBE, /* half sizes scl.x, scl.y and scl.z */
    LIGHT_SWITCH_SPHERE, /* radius scl.x */
    LIGHT_SWITCH_CYLINDER /* along y, radius scl.x and half height scl.y */
};

/* the object list's step for LIGHT SW. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->extra, and its own step, draw
 * and destructor. where the heap can't give the record, light_switch_init
 * stays task's step and tries again on its next step.
 *
 * task needs a work, whose pos places the volume. the low byte of ang.x is
 * the index of the switch's light. a low byte of ang.y that names no
 * light_switch_type counts as LIGHT_SWITCH_AREA. the call clears ang.x and
 * the low bytes of ang.y and ang.z. the rest of ang.y and ang.z turns a cube
 * or a cylinder */
void light_switch_init(struct task *task);

#endif
