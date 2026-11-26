/* what a stage object's test sets and reads of object_world.c, which stands
 * in for the heap, the collision world and the set */
#ifndef TESTS_STAGES_OBJECT_WORLD_H
#define TESTS_STAGES_OBJECT_WORLD_H
#include <ninja.h>

#include "object/placement.h"
#include "system/task.h"

/* the level object_start makes an object at */
#define OBJECT_LEVEL 2

/* the blocks syMalloc and syCalloc have given and syFree hasn't taken back.
 * a block syMalloc gives holds 0xa5 bytes */
extern int heap_live;
/* the allocation of object_start's step that fails, counted from 1, or 0 for
 * none. object_start clears it once the step ends */
extern int heap_fail_at;
/* how many more objects world_pool_alloc lends before it returns null */
extern int pool_left;
/* the objects world_pool_alloc has lent and world_pool_free hasn't taken
 * back */
extern int pool_lent;
/* the objects world_add has put in the world and world_remove hasn't taken
 * out */
extern int world_live;
/* TRUE where collision_init gives no record */
extern Bool collision_fails;
/* the vector the last njTranslateEx took */
extern NJS_POINT3 translation;
/* the bytes object_world's collision_init gives past a record's shapes, which
 * hold COLLISION_TAIL_BYTE until something writes past the last shape */
#define COLLISION_TAIL 16
#define COLLISION_TAIL_BYTE 0xc3

/* make a task for init at OBJECT_LEVEL with the records flags asks, give it
 * placement as set.c does, and step the level once, which runs init. it
 * returns the task, or null where the step destroyed it */
struct task *object_start(Uint16 flags, void (*init)(struct task *task),
                          struct placement *placement);
/* whether the record collision_init gave task still holds its tail */
Bool collision_tail_intact(const struct task *task);

#endif
