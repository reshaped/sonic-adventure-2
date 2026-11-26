/* the road furniture of Radical Highway's object list: its props, sign poles
 * and arrows */
#ifndef STG14_ROAD_PROPS_H
#define STG14_ROAD_PROPS_H
#include <ninja.h>

struct task;

/* the props', the poles' and the arrows' texture lists for
 * texture_load_shared, each ended by null */
extern NJS_TEXLIST *highway_prop_texlists[];
extern NJS_TEXLIST *highway_pole_texlists[];
extern NJS_TEXLIST *highway_arrow_texlists[];

/* the object list's steps, each for the entries named beside it. each does no
 * more where placement_try_leave(task) returns TRUE. otherwise it gives task
 * its model and texture list, a step, a draw and a destructor.
 *
 * task needs a work, whose pos places the model. the model's number is ang.x's
 * low byte modulo the kind's count. the count is 10 for the props, 3 for the
 * poles and 2 for the arrows. an ARROWH3 is always arrow 0. prop 8's model is
 * null, so it only collides.
 *
 * ang.y turns a prop or a pole. prop 7 then turns about x by ang.z. ang.z and
 * then ang.y turn an arrow. an ARROWH3 then turns about x by ang.x.
 *
 * arrow 0 has a solid child, which the players stand on. the child takes the
 * arrow's pos, angles and scl. an ARROW sets its ang.x to 0 first. a prop, a
 * pole or an ARROW whose model has shapes gives task a collision of them. its
 * kind is COLLISION_OBJECT */
void highway_prop_init(struct task *task); /* ROBJ, ROBJ_S and ROBJ_M */
void highway_pole_init(struct task *task); /* POLE */
void highway_arrow_init(struct task *task); /* ARROW */
void highway_arrow_h3_init(struct task *task); /* ARROWH3 */

#endif
