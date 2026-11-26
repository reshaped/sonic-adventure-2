/* Lost Colony: its land, its setup, exit and step, and two of its objects */
#ifndef STG27_LOST_COLONY_H
#define STG27_LOST_COLONY_H

#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table lost_colony_land;
extern const char lost_colony_land_filenames[489];
extern NJS_TEXLIST lost_colony_land_texlist;
extern NJS_TEXLIST colony_background_texlist;
struct task;

extern struct object_list lost_colony_objects;

void lost_colony_init(void);
void lost_colony_exit(void);
/* the stage's step. task needs a work. it sets the fog from a light level.
 * it then adds colony_flash and dynamite_blasts to the level, and sets each
 * to 0 */
void lost_colony_step(struct task *task);

/* the drum's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *spinning_drum_texlists[];
/* CONT, a drum that lies along its x axis, turned by ang.y. task needs a
 * work. the drum spins about its length by scl.x degrees a step. it pushes
 * all but pickups, and the shapes it pushes can't push it */
void spinning_drum_init(struct task *task);

/* STEP, the platform of shared/engine_step.h as Lost Colony builds it. it also
 * carries a light volume of stg27/light.h */
void colony_step_init(struct task *task);

#endif
