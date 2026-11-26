/* the stops, STOPLSD and STOPLOCKON in a stage's object list. a set file
 * places each as a task that holds a collision record. neither draws
 * anything */
#ifndef SHARED_STOP_H
#define SHARED_STOP_H

struct task;

/* the object list's steps for STOPLSD and STOPLOCKON. each does nothing
 * where placement_try_leave(task) returns TRUE. otherwise it gives task's work
 * a collision record of one shape, which the stop's step registers each step.
 * task needs a work, whose pos places the stop.
 *
 * a STOPLSD's record is a COLLISION_OBJECT, and its work's id is
 * WORK_ID_STOP_LSD. the low bit of ang.z picks its shape. 0 picks a box of
 * half sizes scl, and 1 a sphere of radius scl.x. stop_lsd_init clears ang.x
 * and the low bytes of ang.y and ang.z. ang.y and then ang.z turn the box. the
 * heap must have room for the record, unchecked.
 *
 * a STOPLOCKON's record is a COLLISION_ITEM, a sphere of radius 1 at pos. its
 * work's id is WORK_ID_STOP_LOCKON_BOX where the low bit of ang.x is set, and
 * WORK_ID_STOP_LOCKON otherwise. stop_lockon_init also gives task's motion an
 * enemy_work. where the heap has no room for it, the stop goes on without
 * one. its destructor then reads the enemy_work that isn't there */
void stop_lsd_init(struct task *task);
void stop_lockon_init(struct task *task);

#endif
