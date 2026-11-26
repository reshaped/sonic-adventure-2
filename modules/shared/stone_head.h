/* the stone head, STONEHEAD in Wild Canyon's object list: a rail with a stone
 * head on it. a player who holds the head pushes it along the rail */
#ifndef SHARED_STONE_HEAD_H
#define SHARED_STONE_HEAD_H
#include <ninja.h>

struct task;

/* the head's and the rail's texture lists for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *stone_head_texlists[];

/* the object list's step for STONEHEAD, the rail. it does nothing where
 * half_try_leave or placement_try_leave returns TRUE for task. otherwise it
 * puts the rail's ends in a record from the Shinobi heap, at task->extra. it
 * then makes the head as task's child. where either allocation fails, it
 * destroys task.
 *
 * task needs a work. its pos and ang.y place the rail. the rail is 1 + scl.x
 * times its model's size across, and 1 + scl.y times that along. the head is
 * 1 + scl.x times its model's size. its place on the rail runs from 0 at the
 * start to 255 at the end. it starts at the placement's state, or where that is
 * 0, at the low byte of ang.z. the rail's destructor keeps the head's place,
 * held to 1 to 255, as the placement's state. task's child must then still be
 * the head, unchecked.
 *
 * the head lies in the collision world, in an object world_pool_alloc lends.
 * where the pool has none to lend, the head writes through null. the heap
 * must have room for the head's collision record, unchecked */
void stone_head_init(struct task *task);

#endif
