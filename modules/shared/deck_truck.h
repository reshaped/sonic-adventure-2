/* TRUCK, the carrier stages' truck, which rides a rail */
#ifndef SHARED_DECK_TRUCK_H
#define SHARED_DECK_TRUCK_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST *deck_truck_texlists[];

/* TRUCK's init. the truck rides its rail, as shared/deck_rail.h says, along
 * its local z. a hit on its weak point throws it up, damaged, and the
 * placement's state keeps it damaged */
void deck_truck_init(struct task *task);

#endif
