/* the init of BRIDGE and of BRIDGE_DYNA, which dynamite_bridge.c includes once
 * for each. BRIDGE_CALLBACK names that bridge's callbacks. dynamite_bridge.h
 * gives the init's contract */

/* the copy's size class is that of its model's radius plus CLASS_MARGIN */
#define CLASS_MARGIN 15.0F

void BRIDGE_CALLBACK(init)(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *node;
    Float radius;
    Uint32 flags;
#ifdef COLONY_BRIDGE
    NJS_POINT3 light;
#endif

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct bridge));
    if (!task->motion.p) {
        return;
    }
    task->draw = BRIDGE_CALLBACK(draw);
    task->step = BRIDGE_CALLBACK(step);
    task->destructor = BRIDGE_CALLBACK(destroy);
    work->mode = BRIDGE_RESTING;
    BRIDGE(task)->collision = dynamite_bridge_collision;
    node = &BRIDGE(task)->collision;
    node->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    node->evalflags |= NJD_EVAL_BREAK;
    node->child = NULL;
    node->sibling = NULL;
    node->ang[2] = 0;
    node->ang[0] = 0;
    node->ang[1] = work->ang.y;
    node->pos[0] = work->pos.x;
    node->pos[1] = work->pos.y;
    node->pos[2] = work->pos.z;
    radius = node->model->r + CLASS_MARGIN;
    flags = WORLD_SOLID;
    if (!(radius > WORLD_SIZE_CLASS_1_MAX)) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, node);
#ifdef COLONY_BRIDGE
    colony_light_zone_create(task);
    if (task->child) {
        light = work->pos;
        solid_object_create_flags(task->child, NULL, NULL, NULL, 0,
                                  dynamite_bridge_collision.child, 0,
                                  GROUND_CRASHES);
        solid_object_create_flags(task->child, NULL, NULL, NULL, 0,
                                  dynamite_bridge_collision.child->sibling, 0,
                                  GROUND_CRASHES);
        light.y += LIGHT_HEIGHT;
        task->child->work->pos = light;
    }
#else
    solid_object_create_flags(task, NULL, NULL, NULL, 0,
                              dynamite_bridge_collision.child, 0,
                              GROUND_CRASHES);
    solid_object_create_flags(task, NULL, NULL, NULL, 0,
                              dynamite_bridge_collision.child->sibling, 0,
                              GROUND_CRASHES);
#endif
}

#undef CLASS_MARGIN
