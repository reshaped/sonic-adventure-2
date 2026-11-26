/* the Egg Golem's ring of footholds in BOSS_GOLEM.PRS, one under each of the
 * arena's slots. golem_footholds_create must have run before any call but
 * golem_footholds_build */
#ifndef BOSS_GOLEM_FOOTHOLDS_H
#define BOSS_GOLEM_FOOTHOLDS_H
#include <ninja.h>
#include <sg_xpt.h>

/* a foothold's state, in both fights. each hit that sinks a foothold sinks it
 * GOLEM_FOOTHOLD_DROP further, to the next state. a broken foothold is gone */
enum golem_foothold_state {
    FOOTHOLD_WHOLE, /* at golem_floor_y */
    FOOTHOLD_SUNK,
    FOOTHOLD_SUNK_TWICE,
    FOOTHOLD_GONE,
    FOOTHOLD_STATES
};

/* the footholds' distance from the arena's center, and how far a foothold
 * sinks for each state past FOOTHOLD_WHOLE */
#define GOLEM_FOOTHOLD_RADIUS 210.0F
#define GOLEM_FOOTHOLD_DROP 15.0F

/* the height of a whole foothold, and of a shock wave on one */
extern Float golem_floor_y;

/* build the two foothold models at buf, for njDirectDrawObject. the call
 * passes over a model built already, and one whose texture list isn't loaded.
 * it returns buf past the builds it made. while objects_fog_off isn't 0, the
 * call turns the fog off for the builds, and on after them. once one call has
 * built both, a foothold draws from the builds. it draws without them while
 * its sink's motion plays or it fades in */
Uint8 *golem_footholds_build(Uint8 *buf);
/* create the twelve footholds, whole, from the heap and the collision world's
 * pool. where one of a foothold's allocations fails, the call or the
 * foothold's step writes through null */
#ifdef NONMATCHING
/* this side fills only the empty slots. where an allocation fails, it returns
 * FALSE with every slot as it was. it returns TRUE otherwise */
Bool golem_footholds_create(void);
#else
void golem_footholds_create(void);
#endif
/* make the footholds at slots 2 and 10 gone, as a break does but without its
 * pieces */
void golem_footholds_open_gaps(void);
/* break the foothold at slot, where slot is even. its pieces fly, and it is
 * gone. that foothold must be there. an odd slot's foothold stays as it is */
void golem_foothold_break(int slot);
/* hit the foothold at slot, if there is one. a hit sinks it to the next state
 * with a wobble. the hit that would make it FOOTHOLD_GONE breaks it where the
 * slot is even. on an odd slot, the foothold stays FOOTHOLD_SUNK_TWICE. the
 * foothold must not be gone. a gone one takes a state past FOOTHOLD_GONE
 * and wobbles back into view */
void golem_foothold_hit(int slot);
/* bring every foothold back whole over 240 steps. a sunk one rises to
 * golem_floor_y, and a gone one fades in there. every slot must hold a
 * foothold */
void golem_footholds_restore(void);
/* the state of the foothold at slot, or FOOTHOLD_GONE where there is none */
int golem_foothold_state(int slot);
/* the lowest state of the twelve footholds, the least sunk. an empty slot
 * counts as FOOTHOLD_GONE */
int golem_footholds_lowest_state(void);
/* the lowest state of the footholds at slot and on either side of it. an
 * empty slot counts as FOOTHOLD_GONE */
int golem_footholds_lowest_near(int slot);

#endif
