/* the harbor's tank, TANK in Metal Harbor's object list: a solid tank of two
 * parts. the work's scl.y raises the upper one */
#ifndef STG10_HARBOR_TANK_H
#define STG10_HARBOR_TANK_H
#include <ninja.h>

struct task;

/* the tank's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *harbor_tank_texlists[];

/* the object list's step for TANK. it sets task up as stopper_init of
 * shared/stopper.h does, with the tank's own collision node. the tank draws
 * its upper part scl.y above its base, and has no lamp */
void harbor_tank_init(struct task *task);

#endif
