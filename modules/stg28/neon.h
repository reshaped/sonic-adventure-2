/* the neon, NEON in Pyramid Cave's object list: a sign that spins on a stand.
 * its light flickers at random */
#ifndef STG28_NEON_H
#define STG28_NEON_H
#include <ninja.h>

struct task;

/* the neon's texture lists for texture_load_shared, the light's, the stand's
 * and the sign's, ended by null */
extern NJS_TEXLIST *cave_neon_texlists[];

/* the object list's step for NEON. task needs a work. its pos places the
 * neon, and the sign starts at its ang.y. the sign turns about y by about
 * ang.z a step. the turn swings by a fifth either way over 256 steps. the call
 * sets ang.x at random.
 *
 * each step that the light shines steady starts a flicker with a chance of 1
 * in 100. the light dims, then rises partway and dims again until, at random,
 * it rises whole */
void cave_neon_init(struct task *task);

#endif
