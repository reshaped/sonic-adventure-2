/* the flames: particles of fire that rise and grow through ten textures */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/particle.h"
#include "shared/flame.h"
#include "shared/flame/text.inc"
#include "system/util.h"

/* a flame starts turned up to TURN degrees either way. it turns up to SPIN
 * degrees a step either way */
#define TURN 40.0F
#define SPIN 0.25F

static int flame_step(struct particle_group *group, struct particle *particle);

static NJS_TEXNAME flame_texnames[FLAME_FRAMES] = {
#include "shared/flame/flame_texnames.inc"
};
NJS_TEXLIST flame_texlist = {
#include "shared/flame/flame_texlist.inc"
};

/* clang-format off */
static struct particle_group flame_group = {
    PARTICLE_BLEND_ADD, &flame_texlist, 0, FLAME_FRAMES,
    0.5F, 0.96F, 0.04F, 0.22F, flame_step, 75000.0F, NULL, 0, NULL, NULL,
};
/* clang-format on */

struct particle *flame_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale)
{
    struct particle *flame;

    flame = particle_create(&flame_group);
    if (flame != NULL) {
        flame->pos = *pos;
        flame->spd = *spd;
        flame->scale = scale;
        flame->ang = RANDOM_ANGLE(TURN);
        flame->ang_spd = RANDOM_ANGLE(SPIN);
        /* @hack redundant: the frame set to 0, which particle_create gave it
         * already from the group's first_frame */
        flame->frame = 0.0F;
    }
    return flame;
}

/* particle_move, but a frame that reaches the group's frames stays there.
 * particle_move sets it back to frames - 1 */
static int flame_step(struct particle_group *group, struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        return 0;
    }
    return 1;
}
