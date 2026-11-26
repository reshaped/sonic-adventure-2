/* Pyramid Cave, stg28: the stage's init, exit and step, and its background.
 * the background picks the land blocks camera 0 draws, and draws a far wall
 * and eight lamps */
#ifndef STG28_PYRAMID_CAVE_H
#define STG28_PYRAMID_CAVE_H

#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table pyramid_cave_land;
extern NJS_TEXLIST pyramid_cave_land_texlist;

/* the overlay's init and exit, which stg28/head.c's stg28_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * pyramid_cave_init starts the stage's land, objects and sounds, and its music,
 * paths and kill areas. it starts the land blocks' task and the background's.
 * it fills player_hooks' drop_rings and add_item_icon with the stage's own
 * where they are null, and sets stage_hooks. it registers the stage's motions
 * in the motion table, and sets game_state.block_grid_loaded.
 *
 * the sounds, the music, the paths, the kill areas and the two tasks run on
 * after pyramid_cave_exit. the call frees and stops the rest of what init
 * started. it nulls drop_rings and add_item_icon only where they still hold the
 * stage's own. it leaves stage_hooks and game_state.block_grid_loaded as init
 * left them */
void pyramid_cave_init(void);
void pyramid_cave_exit(void);
/* the step of the stage's task, which does nothing */
void pyramid_cave_step(struct task *task);

extern struct object_list pyramid_cave_objects;
/* the fires' texture list. it names sixteen textures of the PVM
 * stg28_fire_red, dz_pc_hi_red01 to dz_pc_hi_red16. the fires of the stage's
 * objects show them in turn */
extern NJS_TEXLIST pyramid_cave_fire_texlist;

#endif
