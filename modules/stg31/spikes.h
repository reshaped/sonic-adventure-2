/* TOGE, Green Hill's spikes */
#ifndef STG31_SPIKES_H
#define STG31_SPIKES_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the spikes, ended by null */
extern NJS_TEXLIST *green_hill_spike_texlists[];

extern NJS_TEXLIST green_hill_spikes_texlist;

/* the object list's step for TOGE. it does nothing where task leaves at its
 * placement's range. otherwise it makes task the spikes, a box at its work's
 * pos. task needs a work. the spikes draw turned by ang.y alone, and the box
 * takes ang as collision.h says.
 *
 * the box attacks a player who touches it while player 0 lies more than 15
 * above that pos. such a touch plays GREEN_HILL_SPIKES while player 0 holds
 * no rings. it plays nothing less than 30 steps after the last such touch,
 * played or not */
void green_hill_spikes_init(struct task *task);

#endif
