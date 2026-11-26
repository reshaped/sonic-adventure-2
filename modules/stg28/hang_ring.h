/* the hanging ring, HANGRING in Pyramid Cave's object list: a ring turning on
 * an axle between two ends */
#ifndef STG28_HANG_RING_H
#define STG28_HANG_RING_H
#include <ninja.h>

struct task;

/* the ring's, the ends' and the axle's texture lists for texture_load_shared,
 * ended by null */
extern NJS_TEXLIST *cave_hang_ring_texlists[];

/* the object list's step for HANGRING. task needs a work. its pos places the
 * ring's center, and its ang.y and ang.z turn the whole by njRotateY and
 * njRotateZ. the axle runs along x through the ring. ang.x turns the ring and
 * the axle about it, and each step adds scl.z * 256 to ang.x. scl.x lengthens
 * each half of the axle, and moves its end out as far.
 *
 * task's collision record, of kind COLLISION_OBJECT, holds the axle and the
 * ends. five child tasks hold the ring's ten cylinders. each has a work and a
 * record of the same kind. the call makes no more children once one can't be
 * made */
void cave_hang_ring_init(struct task *task);

#endif
