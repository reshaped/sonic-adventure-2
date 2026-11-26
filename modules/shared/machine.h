/* the machine, MECHA in a stage's object list: a box of a machine whose
 * lamp blinks */
#ifndef SHARED_MACHINE_H
#define SHARED_MACHINE_H
#include <ninja.h>

struct task;

/* the machine's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *machine_texlists[];

/* the object list's step for MECHA. unless placement_try_leave(task) returns
 * TRUE, it gives task its own step, draw and destructor. it gives task a
 * collision record of one box, of kind COLLISION_OBJECT. where the heap has
 * no room for the record, the machine goes on without one.
 *
 * task needs a work, whose pos places the machine and whose ang.y turns it.
 * the lamp is white for 20 steps of game_state.steps, then gray for 20 */
void machine_init(struct task *task);

#endif
