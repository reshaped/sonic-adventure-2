/* sparkles: particles of the textures kiran01 to kiran04 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/particle.h"
#include "shared/sparkle.h"
#include "shared/sparkle/text.inc"
#include "system/angle.h"
#include "system/util.h"

#define SPARKLE_FRAMES 4

/* a sparkle starts turned up to TURN degrees either way. it turns up to SPIN
 * degrees a step either way */
#define TURN 10.0F
#define SPIN 0.5F

/* a sparkle's edge_angle swings SWING degrees either side of a quarter turn,
 * once every SWING_STEPS steps */
#define SWING 10.0F
#define SWING_STEPS 120
#define SWING_RATE (FULL_TURN / SWING_STEPS)
/* a sparkle keeps its swing's phase, an Angle, in its depth_bias. the system
 * reads depth_bias only under a group's PARTICLE_DEPTH flag, which
 * sparkle_group lacks */
/* @hack type: the phase read through a record laid over the particle, where
 * *(Angle *)&particle->depth_bias would do */
struct sparkle_view {
    Uint8 head[sizeof(struct particle) - sizeof(Float)];
    Angle phase;
};
#define PHASE(particle) (((struct sparkle_view *)(particle))->phase)

static int sparkle_step(struct particle_group *group,
                        struct particle *particle);

static NJS_TEXNAME sparkle_texnames[SPARKLE_FRAMES] = {
#include "shared/sparkle/sparkle_texnames.inc"
};
NJS_TEXLIST sparkle_texlist = {
#include "shared/sparkle/sparkle_texlist.inc"
};

/* with tmpl null, particle_create copies the system's own template, whose
 * color is opaque white */
/* clang-format off */
static struct particle_group sparkle_group = {
    PARTICLE_BLEND_ADD, &sparkle_texlist, 0, SPARKLE_FRAMES,
    0.07F, 0.98F, -0.01F, 0.005F, sparkle_step, 40000.0F, NULL, 0, NULL, NULL,
};
/* clang-format on */

struct particle *sparkle_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale)
{
    struct particle *sparkle;

    sparkle = particle_create(&sparkle_group);
    if (sparkle != NULL) {
        sparkle->pos = *pos;
        sparkle->spd = *spd;
        sparkle->scale = scale;
        sparkle->ang = RANDOM_ANGLE(TURN);
        sparkle->ang_spd = RANDOM_ANGLE(SPIN);
        PHASE(sparkle) = 0;
    }
    return sparkle;
}

/* particle_move, but it also swings the sparkle's edge_angle. where
 * particle_move would end the sparkle, it halves the alpha instead. it ends
 * the sparkle once the alpha is 0 */
static int sparkle_step(struct particle_group *group, struct particle *particle)
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
    particle->edge_angle =
        (Angle)(njSin(PHASE(particle)) * (SWING * ANGLE_PER_DEGREE)) +
        QUARTER_TURN;
    PHASE(particle) += SWING_RATE;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        particle->color = (particle->color & ~COLOR_ALPHA) |
                          ((particle->color >> 1) & COLOR_ALPHA);
        if (!(particle->color & COLOR_ALPHA)) {
            return 0;
        }
    }
    return 1;
}
