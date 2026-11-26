/* the container, MHCONT and MHCONT_F in Metal Harbor's object list. it slides
 * to and fro along a rail, and rebounds at each end */
#ifndef STG10_CONTAINER_H
#define STG10_CONTAINER_H
#include <ninja.h>

struct task;

/* the object list's step for MHCONT and MHCONT_F. unless
 * placement_try_leave(task) returns TRUE, it gives task a record in
 * task->motion. it also gives task the container's own step, draw and
 * destructor. where the heap can't give the record, this stays task's step and
 * tries again on the next. the init puts a copy of the container's collision
 * object in the collision world. it gives task a child, where the heap allows,
 * whose box pushes players, enemies and objects.
 *
 * task needs a work and TASK_FORCE's records. pos places the rail's middle,
 * and ang.y turns the rail. each end lies scl.x from the middle along the
 * rail's z. the container's top speed is scl.y + 0.1. it starts in the middle
 * and slides toward +z. at each end it rebounds, settles for 31 steps, and
 * slides back. it plays a sound as it meets an end and as it leaves one. a
 * scl.x of 10 or less silences it.
 *
 * each step moves the copy and the child with the container. it puts how far
 * the copy moved in both force records' speed. the world collects the copy
 * only while a player is near. a player is near within the model's radius plus
 * 5 of its center */
void container_init(struct task *task);

/* the container's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *container_texlists[];

#endif
