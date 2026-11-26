/* SIDEWIND and WIND in Dry Lagoon's object list, currents that act on player
 * 0. side_current.h and rising_current.h hold their code. SIDEWIND blows along
 * its ang.y. WIND's radius runs from scl.x at its foot to scl.z at its top */
#ifndef STG18_CURRENT_H
#define STG18_CURRENT_H

struct task;

void lagoon_current_init(struct task *task);
void lagoon_updraft_init(struct task *task);

#endif
