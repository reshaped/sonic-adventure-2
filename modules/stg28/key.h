/* the key, KEY in Pyramid Cave's object list. a player carries it to the
 * keyhole of its color, where it locks. the keyhole then sinks, and the key
 * door of that color opens */
#ifndef STG28_KEY_H
#define STG28_KEY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the colors that pair a key with its keyhole and its key door. KEY_COLOR
 * gives a task's color, the whole part of |scl.z| modulo 2 of its work */
enum key_color { KEY_BLUE, KEY_GREEN, KEY_COLORS };
#define KEY_COLOR(work) ((int)njAbs((work)->scl.z) % KEY_COLORS)

/* by color, the live key, keyhole and key door, or null. each one's init and
 * step set its entry, and its destructor clears it. key.c defines them */
extern struct task *cave_keys[KEY_COLORS];
extern struct task *cave_keyholes[KEY_COLORS];
extern struct task *cave_key_doors[KEY_COLORS];

/* a key's modes, in its record's mode */
enum key_mode {
    KEY_MODE_LOOSE, /* it lies, or a player carries it */
    KEY_MODE_FLY, /* it flies to its keyhole */
    KEY_MODE_SINK, /* it sinks into the keyhole */
    KEY_MODE_LOCKED, /* it stays in the keyhole */
    KEY_MODE_RETURN, /* it goes back to its stand */
    KEY_MODES
};

/* a key's record, at task->extra */
struct key {
    Sint32 mode;
    Sint32 next_mode; /* the mode the next step takes, or KEY_NO_MODE */
    Sint32 timer; /* the steps since the key took its mode */
    NJS_VECTOR spd; /* the flight's and the sink's speed */
    NJS_POINT3 home; /* the stand's pos, where the key starts */
    Uint8 unk_24[0x0c];
    /* the key's alpha. key_alloc sets it to 1 and nothing lowers it, so the
     * draw's fade never runs */
    Float alpha;
    Sint32 last_flags; /* the work's flags as the last step left them */
};
#define KEY_NO_MODE (-1)
#define KEY(task) ((struct key *)(task)->extra.p)

/* the key's texture lists for texture_load_shared, ended by null. the blue and
 * the green key's come first, then the blue and the green stand's */
extern NJS_TEXLIST *cave_key_texlists[];

/* the object list's step for KEY. task needs a work and a motion record. the
 * work's pos places the key's stand, where the key starts.
 *
 * the call gives task a record at task->extra, and a child that stays at the
 * key's pos. the child's attack hurts enemies and objects. where either can't
 * be allocated, the call runs again on the next step. it kills a child made
 * for a failed record. the destructor saves the key's mode as the state of
 * task's placement. player 0's work must not be null, unchecked */
void cave_key_init(struct task *task);

#endif
