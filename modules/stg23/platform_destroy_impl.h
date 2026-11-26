/* platform_destroy, a task's destructor. the task's motion holds null, or an
 * object from world_pool_alloc that the task gave world_add. the destructor
 * takes the object out of the world, frees it to the pool, and nulls motion.
 * the includer includes collision/world.h and system/task.h first */
#ifndef STG23_PLATFORM_DESTROY_IMPL_H
#define STG23_PLATFORM_DESTROY_IMPL_H

static void platform_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

#endif
