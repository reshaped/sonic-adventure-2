/* the street's props, ROBJ and POLEOBJ in Mission Street's object list. some
 * kinds of prop wobble or break when hit */
#ifndef SHARED_STREET_PROP_H
#define SHARED_STREET_PROP_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the props' texture lists for texture_load_shared, ended by null. they cover
 * the kinds, their wrecks and pieces, the papers and the glow */
extern NJS_TEXLIST *street_prop_texlists[];

/* compile the model of every kind but ROBJ's kind 6 at buf, for
 * njDirectDrawObject. then compile every wreck's model and pieces. a model that
 * would reach DEMO_AREA of system/areas.h isn't built, and draws without a
 * build. returns buf past the last build */
Uint8 *street_prop_build(Uint8 *buf);
/* the object list's steps for ROBJ and POLEOBJ, street_prop_init and
 * street_pole_init. unless placement_try_leave(task) returns TRUE, each gives
 * task the props' step and destructor, and its kind's draw and shapes.
 *
 * task needs a work. scl.x picks the kind, held to the kinds there are. pos
 * places the prop, and ang.y turns it. ROBJ's kind 19 is scaled by scl.y
 * along x and by scl.z along y. the placement keeps the prop's state from
 * one task to the next. the state says whether the prop has been hit, and
 * which of a pole's parts have broken */
void street_prop_init(struct task *task);
void street_pole_init(struct task *task);

#endif
