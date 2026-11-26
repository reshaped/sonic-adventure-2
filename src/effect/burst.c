/* bursts: particles that show the textures bomb03 to bomb14 in turn */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/burst.h"
#include "effect/particle.h"
#include "system/angle.h"
#include "system/util.h"

#define BURST_TEXTURES 12
/* a burst whose quad passes this size on the screen isn't drawn, as
 * size_limit of effect/particle.h says */
#define BURST_SIZE_LIMIT 75000.0F

/* a burst starts turned at random up to START_DEGREES either way. a wobbling
 * burst then spins at a random rate, up to WOBBLE_SPIN degrees a step either
 * way. the others spin up to SPIN */
#define START_DEGREES 40.0F
#define WOBBLE_SPIN 0.9F
#define SPIN 0.05F

/* a wobbling burst's group lacks PARTICLE_DEPTH, so the burst keeps its
 * wobble's phase, in degrees, in depth_bias. the angle between the quad's
 * edges swings WOBBLE_DEGREES either way of a quarter turn. a swing from one
 * end to the other takes WOBBLE_STEPS steps */
#define WOBBLE_DEGREES 8
#define WOBBLE_STEPS 60
#define WOBBLE_PHASE_STEP ((2 * WOBBLE_DEGREES) / (Float)WOBBLE_STEPS)

static int burst_wobble_step(struct particle_group *group,
                             struct particle *particle);
static int burst_step(struct particle_group *group, struct particle *particle);

void (*burst_hook)(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale, Float unk_3);

#include "effect/burst/text.inc"

static NJS_TEXNAME burst_texname[BURST_TEXTURES] = {
#include "effect/burst/burst_texname.inc"
};
NJS_TEXLIST burst_texlist = {burst_texname, BURST_TEXTURES};

/* clang-format off */
static struct particle_group wobbling_burst_group = {
    PARTICLE_BLEND_ADD, &burst_texlist, 0, BURST_TEXTURES,
    0.3F, 0.93F, 0.0F, 0.1F, burst_wobble_step,
    BURST_SIZE_LIMIT, NULL, 0, NULL, NULL,
};
static struct particle_group burst_group = {
    PARTICLE_BLEND_ADD | PARTICLE_DEPTH, &burst_texlist, 0, BURST_TEXTURES,
    0.2F, 0.93F, 0.0F, 0.1F, burst_step,
    BURST_SIZE_LIMIT, NULL, 0, NULL, NULL,
};
static struct particle_group fast_burst_group = {
    PARTICLE_BLEND_ADD | PARTICLE_DEPTH, &burst_texlist, 0, BURST_TEXTURES,
    0.7F, 0.9F, 0.0F, 1.0F, burst_step,
    BURST_SIZE_LIMIT, NULL, 0, NULL, NULL,
};
/* clang-format on */

struct particle *burst_create_wobbling(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                       Float scale)
{
    struct particle *particle;

    particle = particle_create(&wobbling_burst_group);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = RANDOM_ANGLE(START_DEGREES);
        particle->ang_spd = RANDOM_ANGLE(WOBBLE_SPIN);
        particle->depth_bias =
            (Float)rand() * ((2 * WOBBLE_DEGREES) * RAND_SCALE);
        particle->frame += (Float)rand() * RAND_SCALE;
        if (burst_hook != NULL) {
            burst_hook(pos, spd, scale, 0.0F);
        }
    }
    return particle;
}

struct particle *burst_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale,
                              Float depth_bias)
{
    struct particle *particle;

    particle = particle_create(&burst_group);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = RANDOM_ANGLE(START_DEGREES);
        particle->ang_spd = RANDOM_ANGLE(SPIN);
        particle->depth_bias = depth_bias;
        particle->frame += (Float)rand() * RAND_SCALE;
        if (burst_hook != NULL) {
            burst_hook(pos, spd, scale, 0.0F);
        }
    }
    return particle;
}

struct particle *burst_create_fast(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                   Float scale, Float depth_bias)
{
    struct particle *particle;

    particle = particle_create(&fast_burst_group);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = RANDOM_ANGLE(START_DEGREES);
        particle->ang_spd = RANDOM_ANGLE(SPIN);
        particle->depth_bias = depth_bias;
        particle->frame += (Float)rand() * RAND_SCALE;
        if (burst_hook != NULL) {
            burst_hook(pos, spd, scale, 0.0F);
        }
    }
    return particle;
}

/* a triangle wave's height at along. the wave climbs from 0 to height at a
 * slope of 1. it then falls back as fast, and repeats */
static Uint32 triangle_wave(Uint32 along, Uint32 height)
{
    Uint32 place;

    place = along % (height * 2);
    if (place >= height) {
        place = (height * 2) - place;
    }
    return place;
}

/* burst_step, which also swings the angle between the quad's edges by the
 * wobble's phase */
static int burst_wobble_step(struct particle_group *group,
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
        (Angle)(((int)triangle_wave((Uint32)particle->depth_bias,
                                    2 * WOBBLE_DEGREES) -
                 WOBBLE_DEGREES) *
                ANGLE_PER_DEGREE) +
        QUARTER_TURN;
    particle->depth_bias += WOBBLE_PHASE_STEP;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        return 0;
    }
    return 1;
}

/* particle_move of effect/particle.h, but where it ends a burst, it leaves
 * frame as it is. particle_move sets it to frames - 1 */
static int burst_step(struct particle_group *group, struct particle *particle)
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
