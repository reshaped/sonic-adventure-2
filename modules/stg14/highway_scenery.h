/* Radical Highway's scenery: TOLLGATE, FORKED and PANEL in its object list.
 * each stands still and draws a model */
#ifndef STG14_HIGHWAY_SCENERY_H
#define STG14_HIGHWAY_SCENERY_H
#include <ninja.h>

struct task;

/* the scenery's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *highway_scenery_texlists[];

/* the object list's steps for TOLLGATE, FORKED and PANEL. each makes task an
 * object of shared/common_object.h under OBJECT_MAY_LEAVE. task needs a work,
 * whose pos places the object and whose ang.y turns it.
 *
 * TOLLGATE is a solid object, which the players stand on, and it collides as
 * six spheres too. FORKED and PANEL are model objects, and FORKED collides as
 * a cylinder, a box and a sphere. TOLLGATE and PANEL draw under
 * OBJECT_AS_LAND */
void tollgate_init(struct task *task);
void forked_init(struct task *task);
void panel_init(struct task *task);

#endif
