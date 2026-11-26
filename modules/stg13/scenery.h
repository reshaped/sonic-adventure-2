/* City Escape's scenery: seven kinds of object that stand still and draw a
 * model each */
#ifndef STG13_SCENERY_H
#define STG13_SCENERY_H
#include <ninja.h>

struct task;

/* the scenery's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *scenery_texlists[];

/* the object list's steps for SBLG, GREEN_B, PALM, SLIDER, BAR, FENCES and
 * FENCEL. each sets task up under OBJECT_MAY_LEAVE. GREEN_B is a solid object
 * of shared/common_object.h, which the players stand on. the other six are
 * model objects of that header. FENCES is the short fence and FENCEL the long.
 * each fence collides as a box with a rail along its top. task needs a work,
 * whose pos places the object and whose ang.y turns it */
void sblg_init(struct task *task);
void green_b_init(struct task *task);
void palm_init(struct task *task);
void slider_init(struct task *task);
void bar_init(struct task *task);
void fence_short_init(struct task *task);
void fence_long_init(struct task *task);

#endif
