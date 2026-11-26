/* Knuckles's route through Cannon's Core, stg38: the stage's init, exit and
 * step, its land and its objects */
#ifndef STG38_CORE_KNUCKLES_H
#define STG38_CORE_KNUCKLES_H
#include <ninja.h>

#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table core_knuckles_land;
extern NJS_TEXLIST core_knuckles_land_texlist;
extern struct object_list core_knuckles_objects;
/* the overlay's init and exit, which stg38/head.c gives overlay_run_init and
 * overlay_run_exit.
 *
 * core_knuckles_init loads the route's land and the textures of its objects
 * and its background. it reads the set file and starts the set manager. it
 * reads the camera files. it sets the back color and the lights, reads
 * stg38_light.bin, and compiles the land's models. it starts the particles,
 * the kill areas and the object lights. it starts the land blocks' task, the
 * background's task and player 0's smoke task. it creates the specks where
 * core_knuckles_specks is null. it registers the stage's motions in the
 * motion table. it fills five hooks of player_hooks, each only where it is
 * null. drop_rings gets ring_scatter, unk_04 dirt_create, add_item_icon
 * item_icon_row_add, ripple ripple_create and splash big_splash_create. it
 * loads the big splash's textures. it reads stg38_fog.bin into
 * core_knuckles_fog and stg38_fogB.bin into core_knuckles_upper_fog, and
 * turns Ninja's fog off. it loads the messages and the sounds, and sets
 * stage_hooks. it turns every switch off. it fades the music in, and plays
 * c_core_2.adx or defers it. it sets game_state.block_grid_loaded.
 *
 * core_knuckles_exit frees the set file and the messages, and releases the
 * textures and the big splash's. it destroys the specks where they live. it
 * stops the set manager, the land, the particles and the object lights. it
 * stops the fog's manager where one runs, and turns Ninja's fog off. it takes
 * the stage's motions out of the motion table. it nulls each of the five
 * hooks only where it still holds the route's own. it leaves stage_hooks, the
 * lights, the switches and game_state.block_grid_loaded as init left them. it
 * leaves the sounds, the music and the kill areas running. it also leaves the
 * land blocks' task, the background's and the smoke task running. the
 * colliders and the fences core_knuckles_step starts run on too */
void core_knuckles_init(void);
void core_knuckles_exit(void);
/* the stage task's first step. it starts the poison colliders and the two
 * fences, and sets core_knuckles_water_side to WATER_SIDE_UNSET. it clears the
 * core flags and shows the route's first message. the later steps start the fog
 * where it isn't running, and play the route's sounds. they show block 11's
 * message once. task needs a work */
void core_knuckles_step(struct task *task);

#endif
