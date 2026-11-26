/* shared/last_boss_arena.h's calls that measure and aim from a task's pos.
 * each fight's arena.c includes this once */
#include "shared/last_boss_arena.h"
#include "system/task.h"

#define SQUARE(value) ((value) * (value))
Float last_boss_target_distance(struct task *task, const NJS_POINT3 *target)
{
    const NJS_POINT3 *from = &task->work->pos;
    Float squared = SQUARE(target->x - from->x) + SQUARE(target->y - from->y) +
                    SQUARE(target->z - from->z);
    return njSqrt(squared);
}

Float last_boss_target_distance2(struct task *task, const NJS_POINT3 *target)
{
    const NJS_POINT3 *from = &task->work->pos;
    Float squared = SQUARE(target->x - from->x) + SQUARE(target->y - from->y) +
                    SQUARE(target->z - from->z);
    return squared;
}

Angle last_boss_target_yaw(struct task *task, const NJS_POINT3 *target)
{
    const NJS_POINT3 *from = &task->work->pos;
    return (Angle)(atan2f(target->x - from->x, target->z - from->z) *
                   ANGLE_PER_RADIAN);
}

Angle last_boss_target_pitch(struct task *task, const NJS_POINT3 *target,
                             Angle offset)
{
    const NJS_POINT3 *from = &task->work->pos;
    Float horizontal =
        njSqrt(SQUARE(target->x - from->x) + SQUARE(target->z - from->z));
    Angle pitch =
        (Angle)(atan2f(horizontal, target->y - from->y) * ANGLE_PER_RADIAN);
    return pitch + offset;
}

Angle last_boss_face_target(struct task *task, const NJS_POINT3 *target,
                            Angle step)
{
    struct task_work *work = task->work;
    Angle yaw = last_boss_target_yaw(task, target);
    work->ang.y = angle_approach(work->ang.y, yaw, step);
    return angle_distance(yaw, work->ang.y);
}
