/* Death Chamber's map, MAP in the stage's object list: a map on a wall, of
 * three kinds. its overlay cycles through three pictures. two layers of
 * letters scroll and rest by turns. a dot pulses where the placement puts
 * it */
#ifndef STG25_MAP_H
#define STG25_MAP_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the map's texture lists for texture_load_shared, ended by null. a kind's
 * lies at its index, then come the dot's and the two letter layers' */
extern NJS_TEXLIST *chamber_map_texlists[];

/* compile each kind's map at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the maps
 * draw the builds once one call has compiled all three. while objects_fog_off
 * isn't 0, the call turns the fog off for the builds and on after them */
Uint8 *chamber_map_build(Uint8 *buf);

/* the object list's step for MAP. task needs a work and TASK_EXTRA's record.
 * the work's pos places the map, and its ang.y turns it. the whole part of
 * |scl.z| modulo 3 picks its kind: red, green or blue. scl.x and scl.y place
 * the dot on the map.
 *
 * the call gives task a collision record of one box. the box pushes players,
 * enemies and objects, and resists their push */
void chamber_map_init(struct task *task);

#endif
