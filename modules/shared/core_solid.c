#include <ninja.h>

#include "shared/common_object.h"
#include "shared/core_solid.h"

NJS_TEXLIST *core_solid_texlists[] = {&core_solid_texlist, NULL};

void core_solid_init(struct task *task)
{
    solid_object_init(task, &core_solid_texlist, &core_solid_model, NULL, 0,
                      &core_solid_collision, OBJECT_AS_LAND | OBJECT_MAY_LEAVE);
}
