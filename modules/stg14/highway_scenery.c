/* Radical Highway's scenery: TOLLGATE, FORKED and PANEL in its object list.
 * each stands still and draws a model */
#include <ninja.h>

#include "collision/collision.h"
#include "shared/common_object.h"
#include "stg14/highway_scenery.h"
#include "stg14/highway_scenery/text.inc"

#define TOLLGATE_MODE (OBJECT_AS_LAND | OBJECT_MAY_LEAVE)
#define FORKED_MODE OBJECT_MAY_LEAVE
#define PANEL_MODE (OBJECT_AS_LAND | OBJECT_MAY_LEAVE)
#define TOLLGATE_SHAPES 6
#define FORKED_SHAPES 3

/* clang-format off */
#include "stg14/road/tollgate_art.h"
#include "stg14/road/forked_art.h"
#include "stg14/road/panel_art.h"
/* clang-format on */

NJS_TEXLIST *highway_scenery_texlists[4] = {
#include "stg14/highway_scenery/highway_scenery_texlists.inc"
};
/* clang-format off */
static struct collision_shape tollgate_shapes[TOLLGATE_SHAPES] = {
#include "stg14/highway_scenery/tollgate_shapes.inc"
};
static struct collision_shape forked_shapes[FORKED_SHAPES] = {
#include "stg14/highway_scenery/forked_shapes.inc"
};
/* clang-format on */

void tollgate_init(struct task *task)
{
    solid_object_init(task, &tollgate_texlist, &tollgate_model, tollgate_shapes,
                      TOLLGATE_SHAPES, &tollgate_collision, TOLLGATE_MODE);
}

void forked_init(struct task *task)
{
    model_object_init(task, &forked_texlist, &forked_model, forked_shapes,
                      FORKED_SHAPES, FORKED_MODE);
}

void panel_init(struct task *task)
{
    model_object_init(task, &highway_panel_texlist, &highway_panel_model, NULL,
                      0, PANEL_MODE);
}
