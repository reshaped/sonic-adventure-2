/* the wide cell door, CAGEC in Prison Lane's object list. it is
 * stg09/cage_door.h's cell door, 157 wide where that door is 88.
 * wide_cage_door.c compiles cage_door.c a second time */
#ifndef STG09_WIDE_CAGE_DOOR_H
#define STG09_WIDE_CAGE_DOOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the wide cell front and of its
 * dust, ended by null */
extern NJS_TEXLIST *wide_cage_door_texlists[];

/* the object list's step for CAGEC, which does what cage_door_init does for
 * the wide door. its sill and door are 157 wide, and its posts stand beside
 * them */
void wide_cage_door_init(struct task *task);

#endif
