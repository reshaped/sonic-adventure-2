/* the weed, WEED in Pyramid Cave's object list: a plant that sways once as
 * player 0 runs past */
#ifndef STG28_WEED_H
#define STG28_WEED_H
#include <ninja.h>

struct task;

/* the kinds' texture lists for texture_load_shared, SMALL's and LARGE's,
 * ended by null */
extern NJS_TEXLIST *cave_weed_texlists[];

/* the object list's step for WEED. task needs a work. its pos places the
 * weed, and its angles turn it. scl.x plus 1 scales it. the whole part of
 * |scl.z| modulo 2 picks its kind: 0 is SMALL, and 1 LARGE. the weed has no
 * collision record.
 *
 * a weed at rest sways where player 0 runs near it on the ground, faster than
 * 0.2. near is within 4 times the scale of pos across x and z. player 0
 * must also stand from 25 below pos to 2 times the scale above it. for a
 * LARGE weed the factors are 8 and 4. the sway plays the kind's shape motion
 * once. it advances by the whole part of (scl.y + 1) frames a step. player 0's
 * work must not be null, unchecked */
void cave_weed_init(struct task *task);

#endif
