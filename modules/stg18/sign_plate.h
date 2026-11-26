/* the sign plate, BOARD in Dry Lagoon's object list: a plate on a post, in
 * three kinds */
#ifndef STG18_SIGN_PLATE_H
#define STG18_SIGN_PLATE_H
#include <ninja.h>

struct task;

/* the plate's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *sign_plate_texlists[];

/* the object list's step for BOARD. unless placement_try_leave(task) returns
 * TRUE, it gives task the plate's own step, draw and destructor.
 *
 * task needs a work. its pos places the plate, and its ang turns it about z,
 * then x, then y. where (Sint32)scl.y is odd, the plate turns a further
 * -QUARTER_TURN about x. (Sint32)|scl.z| % 3 picks the kind, of those named
 * UP BRIGHT, UP DARK and LOW in that order. scl.x + 1 scales the plate in x
 * and y */
void sign_plate_init(struct task *task);

#endif
