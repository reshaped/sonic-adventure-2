/* FIREBALL in the object list of the second fight of Sonic and Shadow. it is
 * a burning rock that breaks once a player comes near */
#ifndef STG42_FIREBALL_H
#define STG42_FIREBALL_H
#include <ninja.h>

struct task;

/* the fireball's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *fireball_texture_group[];

/* the object list's step for FIREBALL. task needs a work, whose pos places
 * the fireball. the fireball spins, sheds sparks, and hurts a player it
 * touches. it keeps the camera out of a sphere of radius 55 about pos. the
 * touch breaks it. so does the 61st step after a player comes within
 * scl.x + 30 of pos. the break throws twenty pieces and kills the placement.
 * where the heap can't give the fireball's collision record, the break reads
 * through null */
void fireball_init(struct task *task);

#endif
