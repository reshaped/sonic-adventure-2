/* Egg Quarters, the stage of STG26: its init, exit and step, and its
 * background. the background moves the pillars and plays the pools' sound. it
 * draws the bands, the water and the modifier models */
#ifndef STG26_EGG_QUARTERS_H
#define STG26_EGG_QUARTERS_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

extern struct land_table quarters_land;
extern NJS_TEXLIST egg_quarters_land_texlist;
extern NJS_TEXLIST quarters_water_texlist;

struct task;

/* the overlay's init and exit, which stg26/head.c's stg26_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * egg_quarters_init compiles the land's and the objects' models into
 * STAGE_MODEL_BUILD_AREA of system/areas.h. it starts the guards' radar. it
 * fills each null call of player_hooks with the stage's own. it sets
 * stage_hooks, whatever they held.
 *
 * egg_quarters_exit nulls each call of player_hooks that still holds the
 * stage's own. it leaves stage_hooks as init left them */
void egg_quarters_init(void);
void egg_quarters_exit(void);
/* the stage task's first step, which starts the fog and the guards. the
 * steps after it do nothing. task needs a work */
void egg_quarters_step(struct task *task);

#define EGG_QUARTERS_KINDS 94
extern struct object_kind egg_quarters_kinds[EGG_QUARTERS_KINDS];
/* the stage's object list, which egg_quarters_init gives to set_start */
extern struct object_list egg_quarters_objects;

#endif
