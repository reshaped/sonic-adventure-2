/* the landing, STAGE and STAGE_F in Mad Space's object list: a solid floor.
 * part of it raises dust under a player */
#include <ninja.h>

#include "collision/world.h"
#include "shared/common_object.h"
#include "stg44/landing.h"
#include "stg44/scenery_shapes.h"

NJS_TEXLIST *landing_textures[] = {&landing_texlist, NULL};

void landing_init(struct task *task)
{
    if (solid_object_init_flags(task, &landing_texlist, landing_model->model,
                                NULL, 0, landing_collision,
                                OBJECT_AS_LAND | OBJECT_MAY_LEAVE, 0)) {
        solid_object_create_flags(task, NULL, NULL, NULL, 0,
                                  landing_dust_collision, 0, WORLD_DUSTY);
    }
}
