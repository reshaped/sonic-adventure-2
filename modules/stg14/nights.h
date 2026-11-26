/* NIGHTS in Radical Highway's object list: a model of six parts that light in
 * turn */
#ifndef STG14_NIGHTS_H
#define STG14_NIGHTS_H
#include <ninja.h>

struct task;

/* the model's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *nights_texlists[];

/* the object list's step for NIGHTS. unless placement_try_leave(task) returns
 * TRUE, it gives task->motion a record, which nothing reads. it also gives
 * task the model's own step, draw and destructor. where the heap can't give
 * the record, this stays task's step and tries again on the next.
 *
 * task needs a work. pos places the model, and ang.y plus the work's timer
 * turn it. each step adds ang.x to the timer. the parts light one more every
 * 32 steps, from none to all six. then none is lit for 32 steps, and all
 * six for 31. the cycle then starts again from none. while
 * game_state.time_stopped isn't 0, the model neither turns nor changes */
void nights_init(struct task *task);

#endif
