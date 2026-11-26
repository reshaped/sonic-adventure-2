/* the fluorescent lamp, FLUO in Prison Lane's object list: a ceiling lamp
 * that flickers. stg11/fluorescent_lamp.h is the same object compiled a
 * second time */
#ifndef SHARED_FLUORESCENT_LAMP_H
#define SHARED_FLUORESCENT_LAMP_H
#include <ninja.h>

struct task;

/* the lamp's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *fluorescent_lamp_texlists[];

/* the object list's step for FLUO. unless placement_try_leave returns TRUE
 * for task, it gives task the lamp's own step, draw and destructor. it also
 * gives task a collision record of one sphere, which the lamp's step never
 * passes to collision_register. where the heap can't give the record, the
 * lamp goes on without one.
 *
 * task needs a work. its pos places the lamp, and its ang turns it by
 * njRotateZ, njRotateY, then njRotateX. the lamp starts lit, and goes dark at
 * its 52nd step. each dark spell then lasts 2 or 3 steps, and each lit one 2
 * to 101, at random. scl.z holds the lamp's brightness, from 0 to 1. the
 * brightness moves a quarter a step toward 1 while lit, and toward 0 while
 * dark */
void fluorescent_lamp_init(struct task *task);

#endif
