/* Metal Harbor's scenery: COVER, DAMMY, CBRI, NET and ISLAND in its object
 * list. each stands still and draws a model */
#ifndef STG10_SCENERY_H
#define STG10_SCENERY_H
#include <ninja.h>

struct task;

/* the scenery's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *harbor_scenery_texlists[];

/* the object list's steps for the five. each sets task up under
 * OBJECT_MAY_LEAVE. COVER is a solid object of shared/common_object.h, which
 * the players stand on. the other four are model objects of that header. DAMMY
 * collides as two capsules, and CBRI as a box. task needs a work, whose pos
 * places the object and whose ang.y turns it */
void cover_init(struct task *task);
void dammy_init(struct task *task);
void cbri_init(struct task *task);
void net_init(struct task *task);
void island_init(struct task *task);

#endif
