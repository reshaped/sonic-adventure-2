/* the springs a flute stand calls up, KDSPRING and KDSPRINGB in a stage's
 * object list. a spring appears once its stand is played */
#ifndef SHARED_KNUDAI_SPRING_H
#define SHARED_KNUDAI_SPRING_H

struct task;

/* the object list's steps for KDSPRING and KDSPRINGB. each does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the
 * object's step and destructor. task needs a work. its scl.z, cut to a whole
 * number, holds the id of the object's stand.
 *
 * the object waits until knudai_played returns 1 for its stand, and then sets
 * its placement's state to 1. it then makes a spring of shared/spring.h as
 * task's child. the spring is a task of spring_a_step for KDSPRING, and of
 * spring_b_step for KDSPRINGB. where task_create_child fails, the next step
 * tries again. the object makes one spring at most. the spring takes task's
 * pos, ang and scl, and grows from 1/40 of its size. the heap must have room
 * for the spring's collision record, unchecked.
 *
 * task ends once it leaves range, and the spring ends with it. once the
 * spring is made, task ends too at a step where its work's mode isn't 0. the
 * object writes only 0 to the mode, and the spring never writes it */
void knudai_spring_init(struct task *task);
void knudai_spring_b_init(struct task *task);

#endif
