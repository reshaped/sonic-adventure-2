/* the ship, SHIP in Pyramid Cave's object list: a ship whose motion loops.
 * its sound fades with player 0's distance */
#ifndef STG28_SHIP_H
#define STG28_SHIP_H
#include <ninja.h>

struct task;

/* the kinds' texture lists for texture_load_shared, NORMAL's and EGGMAN's,
 * ended by null */
extern NJS_TEXLIST *cave_ship_texlists[];

/* the object list's step for SHIP. task needs a work. its pos places the
 * ship, and its ang.y turns it. scl.x plus 1 scales it. the whole part of
 * |scl.z| modulo 2 picks its kind: 0 is NORMAL, and 1 EGGMAN. the ship has no
 * collision record.
 *
 * the kind's motion advances (scl.y + 1) * 0.667 frames a step, and wraps at
 * either end. sound 10 of the stage's list plays while player 0 lies within
 * 3500 of pos. its volume is 100 within 1500, and falls in a line to -10 at
 * 3500. player 0's work must not be null, unchecked */
void cave_ship_init(struct task *task);

#endif
