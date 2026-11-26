#include "chaostgdark/props.h"
#include "chaostgdark/scenery.h"
#include "collision/collision.h"
#include "system/angle.h"
#include "system/task.h"

#define GARDEN_PROP_TEXLIST dark_garden_texlist
#define GARDEN_SHADOW_Y (-0.1F)
#include "shared/garden_prop.h"

/* clang-format off */
#define BOX_SHAPE(center_y, half_x, half_y, half_z)                            \
    {GARDEN_PROP_ID, SHAPE_BOX, GARDEN_PROP_PUSH, GARDEN_PROP_GUARD, 0,        \
     {0.0F, (center_y), 0.0F}, (half_x), (half_y), (half_z), 0,                \
     {0, QUARTER_TURN, 0}}
/* clang-format on */
static const struct collision_shape short_fence_shape =
    BOX_SHAPE(7.0F, 6.0F, 7.0F, 0.3F);
static const struct collision_shape medium_fence_shape =
    BOX_SHAPE(5.0F, 5.26F, 15.0F, 0.8F);
static const struct collision_shape long_fence_shape =
    BOX_SHAPE(10.0F, 14.25F, 10.0F, 0.8F);
static const struct collision_shape grave_a_shape =
    BOX_SHAPE(5.8F, 4.5F, 5.8F, 1.5F);
static const struct collision_shape grave_b_shape =
    BOX_SHAPE(7.0F, 4.0F, 7.0F, 1.5F);

void dark_long_fence_init(struct task *task)
{
    collision_init(task, &long_fence_shape, 1, COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &dark_long_fence, NULL);
}

void dark_medium_fence_init(struct task *task)
{
    collision_init(task, &medium_fence_shape, 1, COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &dark_medium_fence, NULL);
}

void dark_short_fence_init(struct task *task)
{
    collision_init(task, &short_fence_shape, 1, COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &dark_short_fence, NULL);
}

void dark_grave_a_init(struct task *task)
{
    collision_init(task, &grave_a_shape, 1, COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &dark_grave_a,
                      &dark_grave_a_shadow);
}

void dark_grave_b_init(struct task *task)
{
    collision_init(task, &grave_b_shape, 1, COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &dark_grave_b,
                      &dark_grave_b_shadow);
}

void dark_stump_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &dark_stump, NULL);
}

void dark_kuwa_a_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &dark_kuwa_a, NULL);
}

void dark_grass_b_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &dark_grass_b, NULL);
}
