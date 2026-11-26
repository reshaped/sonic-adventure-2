/* the elevator, ELV in Prison Lane's object list. its platform carries the
 * players in a straight line, between where it stands and its end point */
#ifndef STG09_ELEVATOR_H
#define STG09_ELEVATOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the elevator and of its dust,
 * ended by null */
extern NJS_TEXLIST *elevator_texlists[];

/* the tasks of the elevators that the wall gate and the bridge watch, or null
 * for none. an elevator's init takes each null entry, and its destructor
 * clears each entry it holds */
#define ELEVATORS 5
extern struct task *elevator_tasks[ELEVATORS];
/* in an elevator's work's submode: the platform slides */
#define ELEVATOR_SLIDING 1

/* the object list's step for ELV. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the elevator's step,
 * draw and destructor. it puts a copy of the platform's collision object in
 * the collision world, under WORLD_SOLID and WORLD_MOVING. it gives task a
 * collision record of kind COLLISION_OBJECT, of the rails' three boxes and
 * the gate's three. the heap must have room for that record. without it, each
 * step writes through null.
 *
 * task needs a work, and TASK_FORCE's records, by which the platform carries
 * players 0 and 1. the work's pos places the platform at home, and its ang.y
 * turns it. its scl is the end point. each later step tests the range as the
 * first did. a player who boards the platform after it stood empty 120 steps
 * sends it to its other end. the rails and the gate rise for the slide. the
 * gate drops at either end, and the rails at home.
 *
 * where bit 0 of ang.x is set, the platform slides once. it starts at its end
 * where bit 0 of the placement's state is set. it never slides where bit 1 is
 * set. the destructor sets bit 0 where the platform is at its end. it sets bit
 * 1 where the platform slides no more */
void elevator_init(struct task *task);

#endif
