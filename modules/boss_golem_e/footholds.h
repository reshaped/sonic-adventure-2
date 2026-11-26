/* the Egg Golem's ring of footholds in BOSS_GOLEM_E.PRS. each call but
 * golem_e_foothold_hit does what boss_golem/footholds.h says of its golem_
 * twin. a foothold's state is an enum golem_foothold_state of that header */
#ifndef BOSS_GOLEM_E_FOOTHOLDS_H
#define BOSS_GOLEM_E_FOOTHOLDS_H
#include <ninja.h>
#include <sg_xpt.h>

/* the height of a whole foothold, and of a shock wave on one */
extern Float golem_e_floor_y;

Uint8 *golem_e_footholds_build(Uint8 *buf);
#ifdef NONMATCHING
/* this side returns whether it made every foothold, so that the stage's step
 * can try again */
Bool golem_e_footholds_create(void);
#else
void golem_e_footholds_create(void);
#endif
void golem_e_footholds_open_gaps(void);
void golem_e_foothold_break(int slot);
/* hit the foothold at slot, if there is one. a whole one sinks to
 * FOOTHOLD_SUNK with a wobble. one at FOOTHOLD_SUNK breaks where slot is
 * even, and stays where it is odd. a hit leaves any other state as it is */
void golem_e_foothold_hit(int slot);
void golem_e_footholds_restore(void);
int golem_e_foothold_state(int slot);
int golem_e_footholds_lowest_state(void);
int golem_e_footholds_lowest_near(int slot);

#endif
