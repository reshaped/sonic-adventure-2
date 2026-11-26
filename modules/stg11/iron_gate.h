/* Iron Gate, the stage of STG11. the overlay's head names its start, its end
 * and its task's step */
#ifndef STG11_IRON_GATE_H
#define STG11_IRON_GATE_H
#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

extern struct land_table iron_gate_land;
extern const char iron_gate_land_filenames[343];
extern NJS_TEXLIST iron_gate_terrain_textures;

/* the stage's kinds of object, which the set file's entries index.
 * iron_gate_init loads the textures in iron_gate_pvms before set_start makes
 * the kinds' tasks */
#define IRON_GATE_KINDS 92
extern struct object_kind iron_gate_kinds[IRON_GATE_KINDS];
extern struct texture_pvm iron_gate_pvms[];
extern NJS_TEXLIST **iron_gate_texture_groups[];

/* start the stage. it starts the land, the object list, the particles, the
 * object lights and the kill areas. it reads the land's block grid. it
 * compiles the land's and the objects' models into STAGE_MODEL_BUILD_AREA of
 * system/areas.h. it reads the stage's two fogs, and starts no fog. it starts
 * the background task, BgExec, and player 0's smoke task. it fills each of
 * player_hooks' drop_rings, add_item_icon and draw_blast_ring that is null, and
 * sets stage_hooks */
void iron_gate_init(void);
/* end the stage. it frees the hints, the messages, the block grid and the set
 * file, and releases the textures. it stops the object lights, the fog, the
 * particles, the set manager and the land. it leaves Ninja's fog off. it takes
 * the stage's motions out of the motion table. it clears each of player_hooks'
 * drop_rings, add_item_icon and draw_blast_ring that still holds the stage's
 * call */
void iron_gate_exit(void);
/* the stage task's step, which starts the fog unless it runs already. task
 * needs a work */
void iron_gate_step(struct task *task);
/* the stage's object list, which iron_gate_init gives to set_start */
extern struct object_list iron_gate_objects;

/* the background task, BgExec, or null. iron_gate_init sets it where it is
 * null, and the task's destructor clears it. the task applies the red fog
 * while the shutters' flags call for it, and the stage's fog otherwise */
extern struct task *iron_gate_background;

#endif
