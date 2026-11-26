/* the mine truck, MITRUCK in Aquatic Mine's object list. an attack breaks it
 * into pieces */
#ifndef STG07_MINE_TRUCK_H
#define STG07_MINE_TRUCK_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the truck and of its pieces,
 * ended by null */
extern NJS_TEXLIST *mine_truck_texlists[];

/* the object list's step for MITRUCK. it does nothing where task leaves at
 * its placement's range. where the heap can't give its record, it does
 * nothing, and the next step tries again. otherwise it gives task the truck's
 * step, draw and destructor, and a collision record of kind COLLISION_OBJECT.
 * it puts a copy of its collision object in the collision world.
 *
 * task needs a work, whose pos places the truck. its ang turns the truck in
 * the order z, y, x. each later step tests the range as the first did. the
 * heap must have room for the collision record. without it, the truck's step
 * reads through null.
 *
 * an attack that touches the truck breaks it, as player_attacks_task tells
 * one. the truck then throws a cloud of dust and bounces player 0. it
 * scores 20 points and plays the stage's sound 5. two steps later it breaks
 * into seven pieces. three steps after the pieces fly, the truck ends, by
 * placement_kill where it has a placement */
void mine_truck_init(struct task *task);

#endif
