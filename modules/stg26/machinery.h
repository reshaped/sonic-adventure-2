/* Egg Quarters' fans and machinery, FAN and MECHA */
#ifndef STG26_MACHINERY_H
#define STG26_MACHINERY_H
#include <ninja.h>

struct task;

/* each object's null-ended texture lists */
extern NJS_TEXLIST *quarters_fan_texlists[];
extern NJS_TEXLIST *quarters_machinery_texlists[];

/* FAN, a fan facing ang.y that spins by ang.z a step, from a random start.
 * it scales by scl.x + 1 */
void quarters_fan_init(struct task *task);

/* MECHA, a curved section or a straight one, as the whole part of |scl.z| is
 * even or odd. it faces ang.y */
void quarters_machinery_init(struct task *task);

#endif
