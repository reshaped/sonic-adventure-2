#include <sg_xpt.h>

#include "chao/motion.h"
#include "chao/work.h"
#include "draw/motion.h"
#include "system/task.h"

/* @hack spelling: each call below passes a local that holds MOTION(chao),
 * where MOTION(chao) itself would do */
#define MOTION(chao) (&((struct chao_work *)(chao)->work)->motion)

void chao_motion_init(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;

    motion_init(&work->motion, chao_motion_table);
}

void chao_motion_step(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct motion_control *motion = &work->motion;

    if (work->flags & CHAO_ANIMATES) {
        motion_step(motion);
    }
}

void chao_link_motion(struct task *chao, Sint32 index)
{
    struct motion_control *motion = MOTION(chao);

    motion_link(motion, index);
}

void chao_link_motion_frames(struct task *chao, Sint32 index, Uint16 frames)
{
    struct motion_control *motion = MOTION(chao);

    motion_link_frames(motion, index, frames);
}

void chao_link_motion_synced(struct task *chao, Sint32 index)
{
    struct motion_control *motion = MOTION(chao);

    motion_link_synced(motion, index);
}

void chao_set_motion(struct task *chao, Sint32 index)
{
    struct motion_control *motion = MOTION(chao);

    motion_set(motion, index);
}

void chao_set_motion_speed(struct task *chao, Float scale)
{
    struct motion_control *motion = MOTION(chao);

    motion_set_speed(motion, scale);
}

void chao_set_motion_frame(struct task *chao, Float frame)
{
    struct motion_control *motion = MOTION(chao);

    motion_set_frame(motion, frame);
}

void chao_set_motion_share(struct task *chao, Float share)
{
    struct motion_control *motion = MOTION(chao);

    motion_set_share(motion, share);
}

Float chao_motion_frame(struct task *chao)
{
    struct motion_control *motion = MOTION(chao);

    return motion_frame(motion);
}

Float chao_motion_share(struct task *chao)
{
    struct motion_control *motion = MOTION(chao);

    return motion_share(motion);
}

Sint32 chao_motion_index(struct task *chao)
{
    struct motion_control *motion = MOTION(chao);

    return motion_index(motion);
}

int chao_motion_pose(struct task *chao)
{
    struct motion_control *motion = MOTION(chao);

    return motion_pose(motion);
}

Bool chao_motion_ended(struct task *chao)
{
    struct motion_control *motion = MOTION(chao);

    return motion_ended(motion);
}

Bool chao_motion_held(struct task *chao)
{
    struct motion_control *motion = MOTION(chao);

    return motion_held(motion);
}

void chao_set_motion_table(struct task *chao,
                           const struct motion_table_entry *table)
{
    struct motion_control *motion = MOTION(chao);

    motion_set_table(motion, table);
}
