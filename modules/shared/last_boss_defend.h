/* the guard flare of the two last fights. it flashes and sparks where a boss
 * pushes a player off */
#ifndef SHARED_LAST_BOSS_DEFEND_H
#define SHARED_LAST_BOSS_DEFEND_H
#include <ninja.h>

struct task;

/* the flash's and the sparks' textures, which each fight's arena loads */
extern NJS_TEXLIST defend_texlist;

/* start a guard flare at position: a flash, and sparks for 30 steps. it faces
 * along direction, or up where direction is null. it returns its emitter task,
 * or null where the heap runs out. it also returns null from when an emitter
 * makes its flash until that emitter's task ends */
struct task *last_boss_defend(NJS_POINT3 *position, NJS_VECTOR *direction);

#endif
