/* Eggman's route through Cannon's Core, stg35: the stage's init, exit and
 * step, its land and its objects */
#ifndef STG35_CORE_EGGMAN_H
#define STG35_CORE_EGGMAN_H
#include "collision/world.h"
#include "object/set.h"

struct task;

/* the route's land and its object list, which core_eggman_init starts */
extern struct land_table core_eggman_land;
extern struct object_list core_eggman_objects;
/* the overlay's init and exit, which stg35/head.c's stg35_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * core_eggman_init loads and starts the route. it starts three tasks: the
 * land blocks', the background's and player 0's smoke. it lends player_hooks
 * five hooks where each is null: drop_rings, add_item_icon, ripple, splash
 * and draw_blast_ring. init plays the route's music, or defers it as
 * music_play_or_defer does.
 *
 * core_eggman_exit frees the SET files, the hints, the messages and the
 * textures. it stops the set manager, the land and the particles. it also
 * stops the fog and the object lights. it sets each lent hook back to null
 * where it still holds the stage's own. it leaves stage_hooks, the lights,
 * Ninja's back color and clip planes, and game_state.block_grid_loaded as init
 * set them. it leaves the sounds and the music as they are. the paths, the
 * kill areas and the three tasks run on */
void core_eggman_init(void);
void core_eggman_exit(void);
/* the stage task's first step. it starts the barriers' colliders and clears
 * the core flags. it shows the route's first message if camera 0's
 * land_block_mask holds block 1. the later steps play the route's hum. stage
 * needs a work */
void core_eggman_step(struct task *stage);

#endif
