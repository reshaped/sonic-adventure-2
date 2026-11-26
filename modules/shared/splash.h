/* splash: particles of water, and the tasks that ring the players' feet with
 * them in shallow water */
#ifndef SHARED_SPLASH_H
#define SHARED_SPLASH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

struct task;

/* splash_a01 to splash_a08: a splash's SPLASH_FRAMES frames, in the order it
 * shows them */
#define SPLASH_FRAMES 8
extern NJS_TEXLIST splash_texlist;

/* make a splash at pos, moving at spd, of size scale, and return its
 * particle. each returns null where the heap can't give one. a splash slows
 * and falls, and ends once it has shown splash_texlist's last texture.
 *
 * splash_create's splash is opaque white, lasts 41 steps and shrinks.
 * splash_violet_create's is the pale violet 0xffa090ff. it starts at a
 * random frame below 2, lasts 13 to 16 steps and grows */
struct particle *splash_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale);
struct particle *splash_violet_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                      Float scale);

/* make player's splash task, which rings the player's feet with splashes in
 * shallow water, and return it. the task is parent's child, or at task level
 * 1 where parent is null. it ends once its player has no work.
 *
 * player must not be below 0. the call returns null for a player above 1, and
 * for one that has a splash task already. it also returns null where the
 * heap can't give the task. once player passes those two tests, the call sets
 * every task's splash color by the current stage. it does so even where the
 * heap then fails */
struct task *splash_task_create(int player, struct task *parent);

#endif
