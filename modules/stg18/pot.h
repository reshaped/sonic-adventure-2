/* TSUBO in Dry Lagoon's object list: a pot that a Rouge's attack breaks into
 * six pieces */
#ifndef STG18_POT_H
#define STG18_POT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the pot's texture lists for texture_load_shared, the whole pot's and its
 * pieces', ended by null */
extern NJS_TEXLIST *pot_texlists[];

/* build the pot's model at buf, if it isn't built yet and its texture list
 * is loaded. it returns buf past the build. the pot draws from its build once
 * built, and from its model before. where objects_fog_off isn't 0, the call
 * builds with the fog off, then turns the fog on */
Uint8 *pot_build(Uint8 *buf);

/* the object list's step for TSUBO. unless placement_try_leave(task) returns
 * TRUE, it gives task the pot's own step, draw and destructor. a pot whose
 * placement's state isn't 0 is broken, and draws nothing. any other takes a
 * collision record of a box and a cylinder, of kind COLLISION_OBJECT. the
 * heap must have room for the record, unchecked.
 *
 * task needs a work and a placement. the work's pos places the pot, and its
 * ang.y turns it. a pot breaks once a Rouge that glides, punches, spirals or
 * dives touches it. its pieces fly from it and end on the floor under it. the
 * break sets the placement's state to 1, and shakes that player's pad. it
 * plays sound 7 with task as its key */
void pot_init(struct task *task);

#endif
