/* the door a flute stand opens, KDDOOR in a stage's object list. DOORT is
 * the same door lying flat. the door slides open once its stand is played */
#ifndef SHARED_KNUDAI_DOOR_H
#define SHARED_KNUDAI_DOOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the door, its dust and the ring
 * sparkles, ended by null. nothing of the door draws with the sparkles' list */
extern NJS_TEXLIST *knudai_door_texlists[];

/* bits of a door's ang.x. KNUDAI_DOOR_HORZ lays the door on its side to
 * slide along +x. KNUDAI_DOOR_HORZ2 has a door on its side slide along -x
 * instead */
#define KNUDAI_DOOR_HORZ 0x100
#define KNUDAI_DOOR_HORZ2 0x200

/* the object list's steps for KDDOOR and DOORT. task needs a work. its pos
 * places the door, its ang.y turns it, and its scl plus 1 scales its model. its
 * ang.x holds the id of the door's stand, and the bits above.
 * knudai_door_flat_init sets task->unk_3c to TRUE, which lays the door flat,
 * then calls knudai_door_init.
 *
 * a door whose placement's state isn't 0 has started to open before.
 * knudai_door_init only calls placement_kill(task) for it. for any other door,
 * the call does nothing where placement_try_leave(task) returns TRUE. otherwise
 * it takes a record in task->motion, and gives task the door's step, draw and
 * destructor. where the heap can't give the record, task keeps this step to try
 * again. a door the heap can't give a collision record works without one.
 *
 * the door starts to open once its stand is played, and then sets its
 * placement's state to 1. it slides up, or along its z where it lies flat. it
 * kills its task once it has slid its model's height, (scl.y + 1) * 55 */
void knudai_door_init(struct task *task);
void knudai_door_flat_init(struct task *task);

#endif
