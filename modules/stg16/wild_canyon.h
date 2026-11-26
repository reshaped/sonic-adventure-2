/* Wild Canyon, stg16: the stage's init, exit and step */
#ifndef STG16_WILD_CANYON_H
#define STG16_WILD_CANYON_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table wild_canyon_land;
extern const char wild_canyon_land_filenames[548];
extern NJS_TEXLIST wild_canyon_land_texlist;

struct task;

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * wild_canyon_init starts the stage. among the rest, it starts the land, the
 * set manager and the kill area. it starts the stage's three tasks and each
 * player's smoke task. it gives the hunt its hints, and
 * EMERALD_KIND_A_PATHMOVE's pieces their paths. it fills eight of player_hooks'
 * calls with the stage's own, each only where it is empty. it sets
 * stage_hooks too.
 *
 * wild_canyon_exit ends the stage. it frees the set file, the hint file, the
 * messages and the hint box's hints. it releases the textures. it stops the
 * set manager, the object lights, the fog, the particles and the land. it
 * takes away the hunt's hints and the pieces' paths. it takes the stage's
 * motions out of the motion table. it empties each of the eight calls that
 * still holds the stage's own */
void wild_canyon_init(void);
void wild_canyon_exit(void);
/* the stage task's first step. it gives the task a step that does nothing
 * but read the work's mode. task needs a work */
void wild_canyon_step(struct task *task);
/* the stage's object list, which wild_canyon_init gives to set_start */
extern struct object_list wild_canyon_objects;

#endif
