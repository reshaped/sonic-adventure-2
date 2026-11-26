/* NOCOLSKULL, the skull without collision, in Aquatic Mine's and Egg
 * Quarters' object lists */
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/skull.h"

void nocolskull_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        model_object_init(task, &skull_texlist, &skull_model, NULL, 0,
                          OBJECT_MAY_LEAVE);
    }
}
