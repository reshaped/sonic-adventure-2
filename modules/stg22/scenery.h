/* Crazy Gadget's scenery set objects */
#ifndef STG22_SCENERY_H
#define STG22_SCENERY_H
#include <ninja.h>

struct task;

/* ARROW, a sign facing ang.y. its chevrons step one row along the texture
 * every 18 steps */
void gadget_arrow_init(struct task *task);
extern NJS_TEXLIST *gadget_arrow_texlists[];

/* CAPSULE, two capsules that rise 180 from pos, over and over, half a cycle
 * apart. the whole part of scl.x is the cycle's length in steps, at least 1 */
void gadget_capsule_init(struct task *task);
extern NJS_TEXLIST *gadget_capsule_texlists[];

/* LIGHT_A, a lamp panel scaled by scl.x + 1 and scl.y + 1. LIGHT_BCDE draws
 * one of signs B to E, as scl.x picks from 0 to 3. its panels light in turn.
 * each flickers among its texture lists at random */
void gadget_lamp_init(struct task *task);
void gadget_sign_init(struct task *task);
extern NJS_TEXLIST *gadget_lamp_texlists[];
extern NJS_TEXLIST *gadget_sign_texlists[];

/* MECHA, a machine whose six pistons each pump at a random speed */
void gadget_machine_init(struct task *task);
extern NJS_TEXLIST *gadget_machine_texlists[];

#endif
