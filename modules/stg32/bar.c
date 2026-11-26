/* the bar, BAR in Meteor Herd's object list: a solid object the players stand
 * on */
#include <ninja.h>

#include "shared/common_object.h"
#include "stg32/bar.h"
#include "stg32/scenery_shapes.h"

NJS_TEXLIST *meteo_bar_textures[] = {&meteo_bar_texlist, NULL};

void meteo_bar_init(struct task *task)
{
    solid_object_init(task, &meteo_bar_texlist, meteo_bar_model->model, NULL, 0,
                      &meteo_bar_collision, OBJECT_AS_LAND | OBJECT_MAY_LEAVE);
}
