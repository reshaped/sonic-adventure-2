/* the elevator, ELV in Iron Gate's object list. its platform carries the
 * players in a straight line, between where it stands and its end point */
#ifndef STG11_ELEVATOR_H
#define STG11_ELEVATOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the elevator and of its dust,
 * ended by null */
extern NJS_TEXLIST *iron_gate_elevator_texlists[];

/* the tasks of the elevators that the bridge watches, or null for none. an
 * elevator's init takes the first null entry, and none where every entry is
 * taken. its destructor clears each entry it holds */
#define ELEVATORS 5
extern struct task *iron_gate_elevator_tasks[ELEVATORS];
/* in an elevator's work's submode: the platform slides */
#define ELEVATOR_SLIDING 1

/* the object list's step for ELV. it does nothing where
 * placement_try_leave(task) returns TRUE. where the heap can't give its record,
 * it does nothing, and the next step tries again. otherwise it gives task the
 * elevator's step, draw and destructor. it puts a copy of the platform's
 * collision object in the collision world, under WORLD_SOLID and WORLD_MOVING.
 * the copy lies 40 below the platform. it gives task a collision record of kind
 * COLLISION_OBJECT. the record holds a box for each of the platform's three
 * rails and its gate. the heap must have room for that record. without it, each
 * step writes through null. each step does nothing where
 * placement_try_leave(task) returns TRUE.
 *
 * task needs a work, and TASK_FORCE's records, by which the platform carries
 * players 0 and 1. the work's pos places the platform at home, and its ang.y
 * turns it. its scl is the end point. a player who boards the platform after it
 * stood empty 120 steps sends it to its other end. it slides 0.3 a step. the
 * rails and the gate rise for the slide. the gate, on the platform's +x side,
 * drops at either end, and the rails at home. the platform bobs as it goes.
 *
 * where bit 0 of ang.x is set, the platform slides once. it starts at its end
 * where bit 0 of the placement's state is set. it never slides where bit 1 is
 * set. the destructor sets bit 0 where the platform is at its end. it sets bit
 * 1 where the platform slides no more */
void iron_gate_elevator_init(struct task *task);

#endif
