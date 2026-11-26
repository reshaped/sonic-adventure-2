/* the scorpion, SCORPION in Death Chamber's object list: a figure of a
 * scorpion, of six kinds */
#ifndef STG25_SCORPION_H
#define STG25_SCORPION_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the scorpion's texture lists for texture_load_shared, a kind's at its
 * index, ended by null */
extern NJS_TEXLIST *chamber_scorpion_texlists[];

/* compile each kind's model at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the
 * scorpions draw the builds once one call has compiled all six. while
 * objects_fog_off isn't 0, the call turns the fog off for the builds and on
 * after them */
Uint8 *chamber_scorpion_build(Uint8 *buf);

/* the object list's step for SCORPION. task needs a work. its pos places the
 * scorpion, and its ang turns it by njRotateZ, njRotateX and njRotateY. where
 * the whole part of scl.y is odd, it then turns by minus a quarter turn about
 * x. scl.x plus 1 scales the scorpion along x and y. the whole part of |scl.z|
 * modulo 6 picks its kind. kinds 0 to 2 are red, green and blue, and kinds 3
 * to 5 the same colors dark */
void chamber_scorpion_init(struct task *task);

#endif
