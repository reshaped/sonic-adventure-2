/* the signal, SIGNAL and SIGNAL_F in Metal Harbor's object list. it is the
 * machine of shared/machine.h, with a model and a shape of its own */
#ifndef STG10_SIGNAL_H
#define STG10_SIGNAL_H
#include <ninja.h>

struct task;

/* the signal's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *signal_texlists[];

/* the object list's step for SIGNAL and SIGNAL_F. it sets task up as
 * machine_init does, with the signal's own model. the signal collides as a
 * capsule in place of the machine's box */
void signal_init(struct task *task);

#endif
