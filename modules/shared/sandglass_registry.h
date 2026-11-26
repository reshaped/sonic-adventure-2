/* a stage's sandglasses, kept for other objects to find by id, read, hold
 * and throw */
#ifndef SHARED_SANDGLASS_REGISTRY_H
#define SHARED_SANDGLASS_REGISTRY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the most sandglasses the table holds */
#define SANDGLASS_SLOTS 32

/* the table of registered sandglasses: SANDGLASS_SLOTS slots, each a task or
 * null. sandglass_count counts the tasks. the table is null while
 * sandglass_count is 0 */
extern struct task **sandglasses;
extern Sint32 sandglass_count;

/* where a mode waits in task's next_mode, make it the work's mode and clear
 * next_mode. a sandglass's step calls it first */
void sandglass_take_mode(struct task *task);
/* make mode the mode that task's next step takes */
void sandglass_set_mode(struct task *task, int mode);

/* enter task in the table's first empty slot, and return the slot. the call
 * allocates the table where it is null. it returns null, entering nothing,
 * for a null task or a full table. it frees a table it leaves empty. where
 * syCalloc can't give the table, the call writes the slots through null */
struct task **sandglass_register(struct task *task);
/* clear task's slot, where the table holds task. it frees the table once
 * sandglass_count is 0 */
void sandglass_unregister(struct task *task);

/* the registered sandglass whose id is id, or null. a sandglass's id is the
 * low byte of its work's ang.z. where several share id, it is the first in
 * the table */
struct task *sandglass_find(int id);
/* whether the sandglass that sandglass_find(id) returns is in
 * SANDGLASS_MODE_RUNNING. it is FALSE where there is none */
Bool sandglass_running(int id);
/* the steps left of the run of the sandglass that sandglass_find(id)
 * returns. it is 0 where that sandglass isn't in SANDGLASS_MODE_RUNNING, or
 * there is none */
Float sandglass_time_left(int id);

/* take a standing sandglass in hand at *pos. the holder faces ang. it returns
 * TRUE, and the sandglass's next step takes SANDGLASS_MODE_HELD. for a
 * sandglass in any other mode, it returns FALSE and changes nothing */
Bool sandglass_pick_up(struct task *task, NJS_POINT3 *pos, Angle ang);
/* move a held sandglass to *pos, turned with its holder, who now faces ang.
 * a sandglass in any other mode stays */
void sandglass_carry(struct task *task, NJS_POINT3 *pos, Angle ang);
/* throw a held sandglass. ang is its holder's heading, and the sandglass
 * flies back against it, turning over toward its flight. its next step takes
 * SANDGLASS_MODE_THROWN. a sandglass in any other mode stays */
void sandglass_throw(struct task *task, Angle ang);

#endif
