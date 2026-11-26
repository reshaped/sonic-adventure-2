/* Meteor Herd, stg32: the stage's init, exit and step, and its background */
#ifndef STG32_METEOR_HERD_H
#define STG32_METEOR_HERD_H

#include <ninja.h>
#include "collision/world.h"
#include "object/set.h"

struct task;

extern struct land_table meteor_herd_land;
extern const char meteor_herd_land_filenames[706];
extern NJS_TEXLIST meteor_herd_land_texlist;

/* the stage's object list, which meteor_herd_init gives to set_start */
extern struct object_list meteor_herd_objects;
/* the overlay's init, which stg32/head.c's stg32_head gives overlay_run_init.
 * it starts the stage's land, objects, lights and sounds, and its music, paths
 * and kill areas. it starts the background's task, BgExec, and as its
 * children a footprints task for each player. it compiles the land's, the
 * background's and the objects' models into STAGE_MODEL_BUILD_AREA of
 * system/areas.h. it adds the stage's motions to the motion table. it sets
 * stage_hooks. it fills each of player_hooks' drop_rings, add_item_icon,
 * unk_04, create_piece, show_piece_sign and ripple that is null. it leaves
 * Ninja's fog on */
void meteor_herd_init(void);
/* the overlay's exit, which stg32_head gives overlay_run_exit. it frees the
 * set file, the hints and the messages, and releases the textures. it stops the
 * land, the set manager, the particles, the object builds and the fog. it
 * leaves Ninja's fog off. it takes the stage's motions out of the motion table.
 * it clears each player_hooks entry that meteor_herd_init fills and that still
 * holds the stage's own */
void meteor_herd_exit(void);
/* the stage task's step. in a one-player game it makes parts of
 * meteor_herd_land solid by player 0's height. it also sends fireballs at the
 * surfaces near player 0, and at points of the land. in any game it starts
 * meteors falling about camera 0. it flares bursts about player 0 while a
 * player falls out of the stage. task needs a work */
void meteor_herd_step(struct task *task);

#endif
