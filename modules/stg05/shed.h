/* the shed, PPSHED in Pumpkin Hill's object list. an attack or a missile
 * breaks it into pieces */
#ifndef STG05_SHED_H
#define STG05_SHED_H

#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the shed and of its pieces,
 * ended by null */
extern NJS_TEXLIST *shed_texlists[];

/* the object list's step for PPSHED. it does nothing where task leaves at its
 * placement's range. otherwise the call gives task the shed's step, draw and
 * destructor. it also gives task a collision record of kind COLLISION_OBJECT,
 * and a target among the missiles'.
 *
 * task needs a work laid out as struct breakable_work. its pos places the
 * shed, and its ang.y turns it. the low five bits of ang.z are its id among
 * the missiles' targets. each later step tests the range as the first did.
 *
 * an attack that hurts the shed breaks it, and so does a missile of its id.
 * the shed then throws a cloud of dust. it bounces player 0 as
 * enemy_bounce_player does, whichever player broke it. two steps later it
 * breaks into pieces. it plays a sound at each step of the break. three steps
 * after the pieces fly, the shed ends, and kills its placement where it has
 * one */
void shed_init(struct task *task);

#endif
