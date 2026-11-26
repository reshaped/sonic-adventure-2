/* the keyhole, KEYHOLE in Pyramid Cave's object list: a plate on a base, with
 * three lamps. once the key of its color locks in it, the plate sinks with the
 * key. the key door of its color then opens */
#ifndef STG28_KEYHOLE_H
#define STG28_KEYHOLE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a keyhole's modes, in its record's mode */
enum keyhole_mode {
    KEYHOLE_MODE_WAIT, /* it waits for the key of its color to lock in it */
    KEYHOLE_MODE_SINK, /* the plate sinks, with the key */
    KEYHOLE_MODE_SUNK,
    KEYHOLE_MODES
};

/* a keyhole's record, at task->extra */
struct keyhole {
    Sint32 mode;
    Sint32 next_mode; /* the mode the next step takes, or KEYHOLE_NO_MODE */
    Sint32 timer; /* the steps since the keyhole took its mode */
    /* the plate's offset from the work's pos in y, from 0 down to -7.5 */
    Float sink;
    /* the copy of the plate's shape in the collision world */
    NJS_OBJECT *object;
};
#define KEYHOLE_NO_MODE (-1)
#define KEYHOLE(task) ((struct keyhole *)(task)->extra.p)

/* the keyhole's texture lists for texture_load_shared, each array ended by
 * null. the plate's and the base's hold one for each color. the lamps' hold
 * one for each lamp */
extern NJS_TEXLIST *cave_keyhole_plate_texlists[];
extern NJS_TEXLIST *cave_keyhole_base_texlists[];
extern NJS_TEXLIST *cave_keyhole_lamp_texlists[];

/* the object list's step for KEYHOLE. task needs a work and TASK_FORCE's
 * records. its pos places the keyhole, and its ang.y turns it. KEY_COLOR
 * gives its color, as key.h says. where the heap can't give task a record at
 * task->extra, the call runs again on the next step.
 *
 * the players collide with a copy of the plate's shape in the collision
 * world, which the record holds. world_pool_alloc lends the copy, and the
 * call writes through null where the pool has none. player 0's work must not
 * be null, unchecked */
void cave_keyhole_init(struct task *task);

#endif
