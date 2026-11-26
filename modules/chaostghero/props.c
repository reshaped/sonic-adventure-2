#include "chaostghero/land.h"
#include "chaostghero/props.h"
#include "collision/collision.h"
#include "system/task.h"

#define GARDEN_PROP_TEXLIST hero_garden_texlist
#include "shared/garden_prop.h"

#define PILLAR_RADIUS 5.0F

/* clang-format off */
#define PILLAR_SHAPE(form, flags, center_y, size_y)                            \
    {GARDEN_PROP_ID, (form), GARDEN_PROP_PUSH, GARDEN_PROP_GUARD, (flags),     \
     {0.0F, (center_y), 0.0F}, PILLAR_RADIUS, (size_y), 0.0F, 0,               \
     {0, 0, 0}}
/* clang-format on */
static const struct collision_shape pillar_shape =
    PILLAR_SHAPE(SHAPE_CAPSULE, 0, 20.0F, 20.0F);
static const struct collision_shape broken_pillar_shapes[] = {
    PILLAR_SHAPE(SHAPE_CAPSULE, SHAPE_TURN_YZX, 14.0F, 10.0F),
    PILLAR_SHAPE(SHAPE_SPHERE, SHAPE_TURN_YZX, 3.0F, 10.0F),
    PILLAR_SHAPE(SHAPE_SPHERE, SHAPE_TURN_YZX, 10.0F, 9.0F),
    PILLAR_SHAPE(SHAPE_SPHERE, SHAPE_TURN_YZX, 15.0F, 10.0F),
    PILLAR_SHAPE(SHAPE_SPHERE, SHAPE_TURN_YZX, 20.0F, 10.0F),
};

void hero_pillar_init(struct task *task)
{
    collision_init(task, &pillar_shape, 1, COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &hero_pillar, NULL);
}

void hero_broken_pillar_init(struct task *task)
{
    collision_init(task, broken_pillar_shapes,
                   sizeof(broken_pillar_shapes) /
                       sizeof(broken_pillar_shapes[0]),
                   COLLISION_OBJECT);
    GARDEN_PROP_PLACE(task, garden_prop_solid_step, &hero_broken_pillar, NULL);
}

void hero_red_flowers_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &hero_red_flowers, NULL);
}

void hero_purple_flowers_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &hero_purple_flowers, NULL);
}

void hero_grass_a_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &hero_grass_a, NULL);
}

void hero_grass_b_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &hero_grass_b, NULL);
}

void hero_grass_c_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &hero_grass_c, NULL);
}

void hero_tower_init(struct task *task)
{
    GARDEN_PROP_PLACE(task, garden_prop_step, &hero_tower, NULL);
}
