/* ring sparkles: particles of the textures ring01 to ring08 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/particle.h"
#include "shared/ring_sparkle.h"
#include "shared/ring_sparkle/text.inc"
#include "system/angle.h"
#include "system/util.h"

/* a sparkle starts turned up to TURN degrees either way, or LONG_TURN for
 * ring_sparkle_create_long's. it turns up to SPIN degrees a step either way */
#define TURN 5.0F
#define LONG_TURN 7.5F
#define SPIN 0.5F
/* a sparkle's frame starts at random below HEAD_START */
#define HEAD_START 0.5F

/* a sparkle's edge_angle swings SWING degrees either side of a quarter turn,
 * once every SWING_STEPS steps */
#define SWING 10.0F
#define SWING_STEPS 120
#define SWING_RATE (FULL_TURN / SWING_STEPS)
/* a sparkle keeps its swing's phase, an Angle, in depth_bias, which the draw
 * reads only under PARTICLE_DEPTH. no group here sets that flag */
/* @hack type: the phase read through a record laid over the particle, where
 * *(Angle *)&particle->depth_bias would do */
struct ring_sparkle_view {
    Uint8 head[sizeof(struct particle) - sizeof(Float)];
    Angle phase;
};
#define PHASE(particle) (((struct ring_sparkle_view *)(particle))->phase)

static int ring_sparkle_long_step(struct particle_group *group,
                                  struct particle *particle);
static int ring_sparkle_step(struct particle_group *group,
                             struct particle *particle);

static NJS_TEXNAME ring_sparkle_texnames[RING_SPARKLE_FRAMES] = {
#include "shared/ring_sparkle/ring_sparkle_texnames.inc"
};
NJS_TEXLIST ring_sparkle_texlist = {
#include "shared/ring_sparkle/ring_sparkle_texlist.inc"
};

/* with tmpl null, particle_create copies the system's own template, whose
 * color is opaque white */
/* clang-format off */
static struct particle_group ring_sparkle_long_group = {
    PARTICLE_BLEND_ADD, &ring_sparkle_texlist, 0, RING_SPARKLE_FRAMES,
    0.09F, 0.98F, -0.01F, 0.001F, ring_sparkle_long_step, 40000.0F,
    NULL, 0, NULL, NULL,
};
static struct particle_group ring_sparkle_group = {
    PARTICLE_BLEND_ADD, &ring_sparkle_texlist, 0, RING_SPARKLE_FRAMES,
    0.4F, 0.97F, -0.0015F, 0.005F, ring_sparkle_step, 90000.0F,
    NULL, 0, NULL, NULL,
};
/* clang-format on */

struct particle *ring_sparkle_create_long(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                          Float scale)
{
    struct particle *sparkle;

    sparkle = particle_create(&ring_sparkle_long_group);
    if (sparkle != NULL) {
        sparkle->pos = *pos;
        sparkle->spd = *spd;
        sparkle->scale = scale;
        sparkle->ang = RANDOM_ANGLE(LONG_TURN);
        sparkle->ang_spd = RANDOM_ANGLE(SPIN);
        PHASE(sparkle) = 0;
        sparkle->frame = rand() * (HEAD_START * RAND_SCALE);
    }
    return sparkle;
}

/* ring_sparkle_long_group's step, a copy of ring_sparkle_step */
static int ring_sparkle_long_step(struct particle_group *group,
                                  struct particle *particle)
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

struct particle *ring_sparkle_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                     Float scale)
{
    struct particle *sparkle;

    sparkle = particle_create(&ring_sparkle_group);
    if (sparkle != NULL) {
        sparkle->pos = *pos;
        sparkle->spd = *spd;
        sparkle->scale = scale;
        sparkle->ang = RANDOM_ANGLE(TURN);
        sparkle->ang_spd = RANDOM_ANGLE(SPIN);
        PHASE(sparkle) = 0;
        sparkle->frame = rand() * (HEAD_START * RAND_SCALE);
    }
    return sparkle;
}

/* particle_move, but it also swings the sparkle's edge_angle. where
 * particle_move would end the sparkle, it halves the alpha instead. it ends
 * the sparkle once the alpha is 0 */
static int ring_sparkle_step(struct particle_group *group,
                             struct particle *particle)
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
