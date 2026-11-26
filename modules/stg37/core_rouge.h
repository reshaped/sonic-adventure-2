/* Rouge's route through Cannon's Core, stg37: the stage's init, exit and step,
 * its land and its objects */
#ifndef STG37_CORE_ROUGE_H
#define STG37_CORE_ROUGE_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table core_rouge_land;
extern NJS_TEXLIST core_rouge_land_texlist;
extern struct object_list core_rouge_objects;
/* the overlay's init and exit, which stg37/head.c gives overlay_run_init and
 * overlay_run_exit.
 *
 * core_rouge_init loads the route's land and the textures of its objects and
 * its background. it reads the set file and starts the set manager. it reads
 * the camera files. it sets the lights, reads stg37_light.bin, and compiles
 * the land's models. it starts the particles, the paths and the kill areas. it
 * starts the land blocks' task, the background's task and player 0's sand
 * task. it registers the stage's motions in the motion table. it fills
 * player_hooks' drop_rings, unk_04, add_item_icon, ripple and splash with
 * ring_scatter, dirt_create, item_icon_row_add, ripple_create and
 * big_splash_create, each only where the hook is null. it loads the big
 * splash's textures. it reads stg37_fog.bin into core_rouge_fog and
 * stg37_fogB.bin into core_rouge_upper_fog, and turns Ninja's fog off. it loads
 * the messages and the sounds, and sets stage_hooks. it turns every switch off.
 * it fades the music in, and plays c_core_2.adx or defers it. it sets
 * game_state.block_grid_loaded.
 *
 * core_rouge_exit frees the set file and the messages, and releases the
 * textures and the big splash's. it stops the set manager, the land and the
 * particles, and the fog's manager where one runs. it turns Ninja's fog off.
 * it takes the stage's motions out of the motion table. it nulls each of the
 * five hooks only where it still holds the route's own. it leaves stage_hooks,
 * the lights, the switches and game_state.block_grid_loaded as init left
 * them. it leaves the sounds, the music, the paths and the kill areas running.
 * it also leaves the land blocks' task, the background's and the sand task
 * running */
void core_rouge_init(void);
void core_rouge_exit(void);
/* the stage task's first step. it starts the shaft's poison and fall
 * colliders, the clear camera and the pool's cover. it clears the core flags
 * and shows the route's first message. the later steps play the route's
 * ambience. task needs a work */
void core_rouge_step(struct task *task);

#endif
