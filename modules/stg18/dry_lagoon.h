/* Dry Lagoon, stg18: the stage's init, exit and step */
#ifndef STG18_DRY_LAGOON_H
#define STG18_DRY_LAGOON_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table dry_lagoon_land;
extern const char dry_lagoon_land_filenames[583];
extern NJS_TEXLIST dry_lagoon_land_texlist;

struct task;

extern struct object_kind dry_lagoon_kinds[98];

/* the overlay's init and exit, which its head gives overlay_run_init and
 * overlay_run_exit.
 *
 * dry_lagoon_init starts the stage. among the rest, it starts the land, the
 * set manager and the kill area. it starts the tasks BgClip and BgExec,
 * player 0's sand task and the drifting debris. it gives the hunt its hints,
 * and EMERALD_KIND_A_PATHMOVE's pieces their paths. it fills seven of
 * player_hooks' calls with the stage's own, each only where it is empty. it
 * sets stage_hooks too.
 *
 * dry_lagoon_exit ends the stage. it frees the set file, the hint file, the
 * messages and the hint box's hints. it releases the textures. it stops the
 * set manager, the fog, the particles and the land, and destroys the
 * drifting debris. it takes away the hunt's hints and the pieces' paths. it
 * takes the stage's motions out of the motion table. it empties each of
 * the seven calls that still holds the stage's own */
void dry_lagoon_init(void);
void dry_lagoon_exit(void);
/* the stage task's step. each step starts the fog with no setting, where it
 * isn't running. task needs a work */
void dry_lagoon_step(struct task *task);
/* the stage's object list, which dry_lagoon_init gives to set_start */
extern struct object_list dry_lagoon_objects;

#endif
