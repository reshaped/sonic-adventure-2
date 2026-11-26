/* the seagulls, KAMOME in Metal Harbor's object list: a flock that flies
 * about a box. each bird steers toward targets of its own */
#ifndef STG10_SEAGULLS_H
#define STG10_SEAGULLS_H
#include <ninja.h>

struct task;

/* the birds' texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *seagulls_texlists[];

/* the object list's step for KAMOME. unless placement_try_leave(task) returns
 * TRUE, it gives task a record in task->motion, which points at room for the
 * birds. it also gives task the flock's own step, draw and destructor. where
 * the heap can't give either, this stays task's step and tries again on the
 * next.
 *
 * task needs a work. the box's middle is pos, ang.y turns it, and it reaches
 * scl from the middle along each axis. the low byte of ang.x is the count of
 * birds less 1. each step plays a cry at a random point of the box with a
 * chance of 0.03 */
void seagulls_init(struct task *task);

#endif
