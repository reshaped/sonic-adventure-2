/* the key door, KEYDOOR in Pyramid Cave's object list: a panel in a frame. the
 * panel rises once the keyhole of its color sinks. a player who passes the
 * door shuts the panel for good */
#ifndef STG28_KEY_DOOR_H
#define STG28_KEY_DOOR_H
#include <ninja.h>

struct task;

/* the door's texture lists for texture_load_shared, by color, each ended by
 * null: the panel's and the frame's */
extern NJS_TEXLIST *cave_key_door_panel_texlists[];
extern NJS_TEXLIST *cave_key_door_frame_texlists[];

/* the object list's step for KEYDOOR. task needs a work, a motion record and
 * TASK_FORCE's records. its pos places the door, and its ang.y turns it.
 * KEY_COLOR gives its color, as key.h says.
 *
 * the call gives task a record at task->extra and two children. one holds
 * the door's sill and panel. the other holds a box past the door, whose touch
 * by a player shuts the panel. where any of the three can't be allocated, the
 * call runs again on the next step. the children it made stay, and the next
 * call makes both anew.
 *
 * the players collide with a copy of the panel's shape in the collision
 * world, which the record holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while a
 * player lies within 81 of the panel's pos.
 *
 * the works of player 0 and of the current camera's player must not be null,
 * unchecked */
void cave_key_door_init(struct task *task);

#endif
