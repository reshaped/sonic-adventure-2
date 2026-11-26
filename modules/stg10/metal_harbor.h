/* Metal Harbor, the stage of STG10. the overlay's head names its start, its
 * end and its task's step */
#ifndef STG10_METAL_HARBOR_H
#define STG10_METAL_HARBOR_H
#include "collision/world.h"
#include "object/set.h"
#include "system/texture.h"

struct task;

/* the land, and the texture list its models draw with */
extern struct land_table metal_harbor_land;
extern const char metal_harbor_land_filenames[505];
extern NJS_TEXLIST metal_harbor_terrain_textures;

/* the stage's kinds of object, which the set file's entries index.
 * metal_harbor_init loads the textures in metal_harbor_object_pvms before
 * set_start makes the kinds' tasks */
#define METAL_HARBOR_KINDS 93
extern struct object_kind metal_harbor_kinds[METAL_HARBOR_KINDS];
extern struct texture_pvm metal_harbor_object_pvms[];
extern NJS_TEXLIST **metal_harbor_object_texture_groups[];

/* start the stage. it starts the land, the object list, the particles, the
 * paths and the kill areas. it compiles the land's and the objects' models,
 * container_0_build's and missile_bridges_build's into
 * STAGE_MODEL_BUILD_AREA of system/areas.h. it reads the stage's two fogs,
 * and leaves Ninja's fog off. it starts the background task, BgExec, with
 * player 0's footsteps. it fills each of player_hooks' drop_rings,
 * add_item_icon and splash that is null, and sets stage_hooks */
void metal_harbor_init(void);
/* end the stage. it frees the hints, the messages and the set file, and
 * releases the textures, the splash's among them. it stops the fog, the
 * particles, the set manager and the land, and leaves Ninja's fog off. it
 * takes the stage's motions out of the motion table. it clears each of
 * player_hooks' drop_rings, add_item_icon and splash that still holds the
 * stage's call */
void metal_harbor_exit(void);
/* the stage task's step, which starts the fog unless it runs already. the
 * rest runs while the work's mode is 0, as task_create leaves it. a press of
 * Y on player 1's pad sends player 0 PLAYER_REQUEST_BOARD. camera 0's angle of
 * view heads for 70 degrees, plus up to 6 with player 0's speed. task needs a
 * work */
void metal_harbor_step(struct task *task);
/* the stage's object list, which metal_harbor_init gives to set_start */
extern struct object_list metal_harbor_objects;

#endif
