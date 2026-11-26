/* the road objects and signs, ROADOBJ, SIGNS and SIGNS_F in City Escape's
 * object list */
#ifndef STG13_ROAD_OBJECTS_H
#define STG13_ROAD_OBJECTS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the road objects' and signs' texture lists for texture_load_shared, ended
 * by null */
extern NJS_TEXLIST *road_objects_texlists[];

/* the object list's steps for ROADOBJ, and for SIGNS and SIGNS_F. unless
 * placement_try_leave(task) returns TRUE, each gives task its step and
 * destructor, and its kind's draw and shapes.
 *
 * task needs a work. ROADOBJ has 16 kinds and SIGNS 8. the low byte of ang.x,
 * modulo that count, picks the kind. the init leaves the kind's index in
 * ang.x. pos places the model, and ang.y turns it. ROADOBJ's kinds 10 to 15
 * also stretch it along its y and z by scl.y + 1. a placed task's work takes
 * its placement's state as its submode, and the destructor gives the state
 * back */
void road_object_init(struct task *task);
void sign_init(struct task *task);
/* compile at buf each kind's model that isn't built already, for
 * njDirectDrawObject. a build that would reach DEMO_AREA of system/areas.h
 * isn't made, and its kind draws its chunk model. returns buf past the builds
 * it made */
Uint8 *road_objects_build(Uint8 *buf);
/* road_objects_lookup reads 9 kinds of its own. they take the models of signs 3
 * to 7 and of road objects 0 and 4 to 6. road_objects_lookup_count returns 9.
 *
 * road_objects_lookup sets *texlist, *object and *built to the texture list,
 * model and build of kind index. it returns 1. *built is null where
 * road_objects_build didn't build the kind. an index of 9 or more returns 0
 * and sets nothing. index must not be negative, unchecked */
int road_objects_lookup_count(void);
int road_objects_lookup(int index, NJS_TEXLIST **texlist,
                        NJS_CNK_OBJECT **object, NJS_DIRECT_HEAD **built);

#endif
