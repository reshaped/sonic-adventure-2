/* Crazy Gadget, the stage of STG22: its init, exit and step */
#ifndef STG22_CRAZY_GADGET_H
#define STG22_CRAZY_GADGET_H

#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"

struct task;

#define CRAZY_GADGET_KINDS 79

/* world_gravity points along an axis where the size of its component on that
 * axis passes GRAVITY_ON_AXIS */
#define GRAVITY_ON_AXIS 0.9F

/* the stage's object list, which crazy_gadget_init gives to set_start */
extern struct object_list crazy_gadget_objects;
extern struct object_kind crazy_gadget_kinds[CRAZY_GADGET_KINDS];
/* the overlay's init and exit, which stg22/head.c's stg22_head holds for
 * overlay_run_init and overlay_run_exit.
 *
 * crazy_gadget_init starts the stage, its background task BgExec and its
 * music. it compiles the land's, the objects', the cage's and the rail
 * marker's models into STAGE_MODEL_BUILD_AREA of system/areas.h. it fills
 * each of player_hooks' drop_rings and add_item_icon that is null, and sets
 * stage_hooks.
 *
 * crazy_gadget_exit frees the set file, the hints and the messages, and
 * releases the textures. it stops the set manager, the fog, the particles and
 * the land, and leaves Ninja's fog off. it clears each of drop_rings and
 * add_item_icon that still holds the stage's call. it leaves stage_hooks set,
 * and doesn't destroy BgExec */
void crazy_gadget_init(void);
void crazy_gadget_exit(void);
/* the stage task's step. it sets world_gravity_ang_x, world_gravity_ang_z
 * and the collision checks' up axis from world_gravity. it makes the event
 * task, ObjSetManExec, where none lives. task needs a work */
void crazy_gadget_step(struct task *task);

/* TRUE where point and the current camera are both inside the room or both
 * outside it, else FALSE. the room is the box visibility.c tests, and a point
 * on its faces is outside */
Bool gadget_visible_from_camera(NJS_POINT3 *point);

#endif
