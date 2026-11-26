#include <sg_xpt.h>

#include "chao/body_collision.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "system/task.h"

/* fall_set_kind's bits for FALL_KIND_NORMAL, under which a player can lift
 * the Chao */
#define LIFTABLE (FALL_OFFERABLE | FALL_UNK_200)

void garden_enable_objects(void)
{
    garden_objects_enabled = TRUE;
}

void garden_disable_objects(void)
{
    garden_objects_enabled = FALSE;
}

Bool garden_objects_running(struct task *object)
{
    return garden_objects_enabled;
}

void chao_enable_push(struct task *chao)
{
    struct collision_shape *shape = collision_get_shape(chao, CHAO_PUSH_SHAPE);

    shape->push |= PUSH_ALL_BUT_PICKUPS;
}

void chao_disable_push(struct task *chao)
{
    struct collision_shape *shape = collision_get_shape(chao, CHAO_PUSH_SHAPE);

    shape->push &= ~PUSH_ALL_BUT_PICKUPS;
}

void chao_resist_displacement(struct task *chao)
{
    struct collision_shape *shape = collision_get_shape(chao, CHAO_PUSH_SHAPE);

    shape->push |= PUSH_FIRM(PUSH_ALL_BUT_PICKUPS);
}

void chao_allow_displacement(struct task *chao)
{
    struct collision_shape *shape = collision_get_shape(chao, CHAO_PUSH_SHAPE);

    shape->push &= ~PUSH_FIRM(PUSH_ALL_BUT_PICKUPS);
}

void chao_enable_body_shapes(struct task *chao)
{
    chao->work->flags |= LIFTABLE;
    collision_shape_on(chao, CHAO_CONTACT_SHAPE);
    collision_shape_on(chao, CHAO_SWIM_SHAPE);
}

void chao_disable_body_shapes(struct task *chao)
{
    chao->work->flags &= ~LIFTABLE;
    collision_shape_off(chao, CHAO_CONTACT_SHAPE);
    collision_shape_off(chao, CHAO_SWIM_SHAPE);
}

void chao_swim_shape_off(struct task *chao)
{
    collision_shape_off(chao, CHAO_SWIM_SHAPE);
}

void chao_disable_swim_shape(struct task *chao)
{
    collision_shape_off(chao, CHAO_SWIM_SHAPE);
}
