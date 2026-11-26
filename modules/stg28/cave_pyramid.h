/* the pyramid, PYRAMID in Pyramid Cave's object list: a small pyramid a
 * player can pick up and carry. the first time a player holds it, it may
 * release a small animal */
#ifndef STG28_CAVE_PYRAMID_H
#define STG28_CAVE_PYRAMID_H
#include <ninja.h>

struct task;

/* the pyramid's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_pyramid_texlists[];

/* the object list's step for PYRAMID. task needs a work and a motion record.
 * the work's pos places the pyramid, and its ang turns it by njRotateZ,
 * njRotateX and njRotateY. while a player holds the pyramid, it turns with
 * the player. the heap must have room for the pyramid's body: without one,
 * each step reads and writes through null.
 *
 * the low byte of ang.z holds a chance and an animal's kind. the call keeps
 * the byte, and sets ang.z to 0. the pyramid's first hold releases a small
 * animal of that kind by that chance. the chance is 0.05 plus 0.1 times the
 * byte's high four bits. high bits of 0 release none. the low four bits, modulo
 * 3, pick the kind. a hold sets the state of the pyramid's placement to 1. it
 * releases an animal only where the state was 0 */
void cave_pyramid_init(struct task *task);

#endif
