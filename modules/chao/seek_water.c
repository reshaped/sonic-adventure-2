/* a garden Chao's hop forward, and its walk to water when thirsty */
#include <ninja.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/face.h"
#include "chao/gait.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/sound.h"
#include "chao/spawn_grid.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* the voice of a Chao setting off for water, and its rare other */
#define THIRSTY_VOICE 0x6043
#define RARE_THIRSTY_VOICE 0x6044

int chao_hop_forward(struct task *chao)
{
    struct mover *mover = chao->motion.p;
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = &work->behavior;
    Angle yaw;
    switch (behavior->mode) {
    case 0:
        chao_set_timed_eye(chao, CHAO_EYE_DISTRESSED, CHAO_FACE_HOLD);
        chao_set_timed_mouth(chao, 2, CHAO_FACE_HOLD);
        chao_link_motion(chao, CHAO_ANIM_HABATOBI_JUMP);
        yaw = chao->work->ang.y;
        mover->spd.x = njSin(yaw) * 0.4F;
        mover->spd.y = 0.35F;
        mover->spd.z = njCos(yaw) * 0.4F;
        behavior->timer = 0;
        behavior->mode++;
        break;
    case 1:
        if (mover->flags & MOVER_ON_FLOOR) {
            chao_set_behavior(chao, chao_choose_destination);
        }
        break;
    }
    mover->acc.y = -(mover->gravity * 0.45F);
    return BEHAVIOR_CONTINUE;
}
int chao_seek_water(struct task *chao)
{
    struct mover *mover = chao->motion.p;
    garden_spawn_pick(TERRAIN_WATER, &mover->target);
    /* rand() * RAND_SCALE is 0 only where rand() returns 0, so
     * RARE_THIRSTY_VOICE almost never sounds */
    if (rand() * RAND_SCALE != 0.0F) {
        sound_play_at(THIRSTY_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                      &chao->work->pos);
    } else {
        sound_play_at(RARE_THIRSTY_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                      &chao->work->pos);
    }
    chao_start_skill_walk(chao);
    return BEHAVIOR_CONTINUE;
}
