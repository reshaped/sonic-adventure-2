/* Egg Quarters' two propellers, PROPELLER and PROPELLER2 */
#ifndef STG26_PROPELLERS_H
#define STG26_PROPELLERS_H
#include <ninja.h>

struct task;

enum { PROPELLER_COLORS = 3 };

/* the whole part of |scl.x| picks red, green or blue, mod 3. scl.z + 1
 * scales the model. a propeller collides only where scl.y is 0: PROPELLER by
 * a capsule, PROPELLER2 by a sphere */
void quarters_propeller_init(struct task *task);
void quarters_propeller2_init(struct task *task);
extern NJS_TEXLIST *quarters_propeller_texlists[];
extern NJS_TEXLIST *quarters_propeller2_texlists[];

#endif
