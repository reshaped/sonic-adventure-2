/* the machine, MECHA in Iron Gate's object list. it is the machine of
 * shared/machine.h, with a model and a box of its own */
#ifndef STG11_MACHINE_H
#define STG11_MACHINE_H
#include <ninja.h>

struct task;

/* the machine's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *iron_gate_machine_texlists[];

/* the object list's step for MECHA. it sets task up as machine_init does,
 * with this machine's model. its box stands 40 high on the work's pos, where
 * machine_init's stands 20 high, centered on it */
void iron_gate_machine_init(struct task *task);

#endif
