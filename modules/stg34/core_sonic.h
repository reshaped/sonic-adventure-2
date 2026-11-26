/* Sonic's route through Cannon's Core, stg34: the stage's init, exit and step,
 * its land and its objects */
#ifndef STG34_CORE_SONIC_H
#define STG34_CORE_SONIC_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

extern struct land_table core_sonic_land;
extern const char core_sonic_land_filenames[690];
extern NJS_TEXLIST core_sonic_land_texlist;
extern NJS_TEXLIST **core_sonic_bg_groups[];
extern struct texture_pvm core_sonic_bg_pvms[];
extern struct object_list core_sonic_objects;
/* the overlay's init and exit, which stg34/head.c's stg34_head gives
 * overlay_run_init and overlay_run_exit.
 *
 * core_sonic_init loads the route's land and the textures of its objects and
 * its background. it reads the set file and starts the set manager. it reads
 * the camera files. it sets the lights, reads stg34_light.bin, and compiles
 * the land's models. it starts the particles, the paths, the kill areas and
 * the object lights. it starts the land blocks' task and the background's. it
 * registers the stage's motions in the motion table. it fills player_hooks'
 * drop_rings and add_item_icon with the stage's own where they are null. it
 * sets stage_hooks. it turns Ninja's fog on. it loads the hints, the messages
 * and the sounds. it fades the music in. it sets game_state.block_grid_loaded.
 *
 * core_sonic_exit frees the set file, the hints and the messages, and
 * releases the textures. it stops the set manager, the land and the
 * particles. it also stops the object lights, and the fog's manager where one
 * runs. it turns Ninja's fog off. it takes the stage's motions out of the
 * motion table. it nulls drop_rings and add_item_icon only where they still
 * hold the stage's own. it leaves stage_hooks, the lights and
 * game_state.block_grid_loaded as init left them. it leaves the sounds, the
 * music, the paths and the kill areas running. it also leaves the land
 * blocks' task and the background's running */
void core_sonic_init(void);
void core_sonic_exit(void);
/* the stage task's first step. it starts the background's colliders and the
 * entry's trigger. the trigger turns player 0's pad off until a Sonic touches
 * it. the step starts the music, clears the core flags, and shows the route's
 * first message. the later steps play the route's sounds and change the music
 * at the drop. they show the drop's message once. task needs a work */
void core_sonic_step(struct task *task);

#endif
