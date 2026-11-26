/* the turntable, TERMINAL in Pumpkin Hill's object list. it turns among three
 * ways. it sends a ghost train of its own along the way it turns to */
#ifndef STG05_TURNTABLE_H
#define STG05_TURNTABLE_H
#include <ninja.h>

struct task;

/* the table's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *turntable_texlists[];

/* the object list's step for TERMINAL. it gives task the table's step, draw
 * and destructor. it gives task a record in task->motion from syCalloc, which
 * the destructor frees. where syCalloc fails, the call does nothing, and the
 * next step tries again. the table never leaves at its placement's range.
 *
 * task needs a work and TASK_FORCE's records. the records carry players 0 and
 * 1 as the table turns under them. the work's pos places the table. ang.x,
 * ang.y and ang.z are the angles of ways 0, 1 and 2 about y. scl.x, scl.y and
 * scl.z times 10 are their lengths. way n's switch is switch n + 1. its train
 * is the ghost train of id n + 1.
 *
 * the table starts on way 1. each step it turns its way's switch on and the
 * other two off. once the table has sent its train, another way's switch
 * turned on makes that way the table's. at the start and on each new way, the
 * table fades out the ways' trains. where it has a train of its own, it next
 * fades that train out and kills it. it then turns to the way, 1.5 degrees a
 * step. once it points along the way, it sends a new train of its own along
 * it. the train's id is GHOST_TRAIN_TURNTABLE. on way 1 that train runs to and
 * fro in place of the way's train. on another way it fades out at the end of
 * its line. the way's train then fades in.
 *
 * the players stand on a copy of the table's collision model in the collision
 * world. the copy collides only while a player lies within its model's radius
 * plus 5 of it */
void turntable_init(struct task *task);

#endif
