/* Tails's route through Cannon's Core, stg36: the stage's init, exit and
 * step, its land and its objects */
#ifndef STG36_CORE_TAILS_H
#define STG36_CORE_TAILS_H
#include "collision/world.h"
#include "object/set.h"

struct task;

/* the route's land and its object list, which core_tails_init starts */
extern struct land_table core_tails_land;
extern struct object_list core_tails_objects;
/* the overlay's init and exit, which stg36/head.c's stg36_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * core_tails_init loads and starts the route. its tasks include the land
 * blocks', the background's and player 0's smoke. it lends player_hooks
 * three hooks where each is null: drop_rings, add_item_icon and
 * draw_blast_ring. init plays the route's music, or defers it as
 * music_play_or_defer does.
 *
 * core_tails_exit frees the SET files' block, the hints, the messages and the
 * textures. it stops the set manager, the land, the particles and the fog. it
 * sets each lent hook back to null where it still holds the stage's own. it
 * leaves stage_hooks, the lights, Ninja's back color and
 * game_state.block_grid_loaded as init set them. it leaves the sounds and the
 * music as they are. the paths, the kill areas, the smoke and the land
 * blocks' and background's tasks run on */
void core_tails_init(void);
void core_tails_exit(void);
/* the stage task's first step. it starts the floor's collider, clears the core
 * flags and plays the route's first voice. the later steps play the route's
 * sounds */
void core_tails_step(struct task *task);

#endif
