/* Green Hill, stg31: the stage's init, exit and step, its land and its
 * objects */
#ifndef STG31_GREEN_HILL_H
#define STG31_GREEN_HILL_H
#include <ninja.h>

/* how many kinds green_hill_kinds holds. the stage's set files index it */
#define GREEN_HILL_KINDS 76

#include "collision/world.h"
#include "object/set.h"

extern struct land_table green_hill_land;
extern const char green_hill_land_filenames[195];
extern NJS_TEXLIST green_hill_land_texlist;

struct task;

extern struct object_list green_hill_objects;
extern struct object_kind green_hill_kinds[GREEN_HILL_KINDS];

/* the overlay's init and exit, which stg31/head.c's stg31_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * green_hill_init starts the stage's land, objects and particles. it starts
 * its music, paths and kill areas, and its background's task, BgExec. it loads
 * the stage's textures, lights, sounds, hints and messages. it turns Ninja's
 * fog on, and sets stage_hooks. where player_hooks' drop_rings is null, it
 * sets it to ring_scatter. it sets a null add_item_icon to item_icon_row_add,
 * and a null splash to big_splash_create.
 *
 * green_hill_exit frees the set file, the hints, the messages and the
 * textures. it stops the land, the objects and the particles, and turns the
 * fog off. where drop_rings, add_item_icon or splash holds the call init
 * gives it, exit nulls it. the music, the paths, the kill areas and the sounds
 * run on after exit. so do the background's task and the rides'. stage_hooks
 * keeps what init set */
void green_hill_init(void);
void green_hill_exit(void);
/* the step of the stage's task. where no rides' task lives, it creates one,
 * ObjSetManExec, and runs that task's first step at once. that task makes
 * the three rides of stg31/path_ride.h whenever it has no children. task
 * needs a work */
void green_hill_step(struct task *task);

#endif
