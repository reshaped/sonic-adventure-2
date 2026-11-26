/* the waterweed, MIZUKUSA in Aquatic Mine's object list. it sways in the
 * water */
#ifndef STG07_WATERWEED_H
#define STG07_WATERWEED_H
#include <ninja.h>

struct task;

/* the texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *waterweed_texlists[];

/* the object list's step for MIZUKUSA. it does nothing where task leaves at
 * its placement's range. otherwise it gives task the waterweed's step, draw
 * and destructor, and starts its sway at a random frame.
 *
 * task needs a work, whose pos places the waterweed. its ang turns the
 * waterweed in the order z, y, x. each later step tests the range as the first
 * did */
void waterweed_init(struct task *task);

#endif
