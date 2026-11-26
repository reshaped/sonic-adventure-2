/* dirt: particles of earth, thrown up and left to fall */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/particle.h"
#include "shared/dirt.h"
#include "system/angle.h"
#include "system/util.h"

/* a piece's frame starts at random below HEAD_START and climbs FRAME_STEP a
 * step. the piece ends once its frame reaches 1. in floats, LIFE_STEPS of
 * those steps sum to just under 1. a piece thus lasts 49 to 61 steps */
#define LIFE_STEPS 60
#define FRAME_STEP (1.0F / LIFE_STEPS)
#define HEAD_START 0.2F
/* a piece starts turned up to TURN degrees either way, which is any way. it
 * turns up to SPIN degrees a step either way */
#define TURN 180.0F
#define SPIN 0.4F
#define DIRT_COLOR 0xff4c7f7f

#define FRICTION 0.98F
#define GRAVITY (-0.12F)
#define SCALE_STEP (-0.001F)

#include "shared/dirt/text.inc"

static NJS_TEXNAME dirt_texname[1] = {
#include "shared/dirt/dirt_texname.inc"
};
NJS_TEXLIST dirt_texlist = {dirt_texname, 1};

/* the pieces' template, for their color, edge_angle, flags and depth_bias.
 * particle_create and dirt_create set the other fields */
/* clang-format off */
static struct particle dirt_template = {
    0, 1.0F, {0.0F, 0.0F, 0.0F}, DIRT_COLOR, 0.0F, QUARTER_TURN,
    {0.0F, 0.0F, 0.0F}, 0, NULL, 0, 0.0F,
};
static struct particle_group dirt_group = {
    PARTICLE_BLEND_ALPHA, &dirt_texlist, 0, 1,
    FRAME_STEP, FRICTION, GRAVITY, SCALE_STEP, particle_move, 0.0F,
    &dirt_template, 0, NULL, NULL,
};
/* clang-format on */

struct particle *dirt_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale)
{
    struct particle *dirt;

    dirt = particle_create(&dirt_group);
    if (dirt != NULL) {
        dirt->pos = *pos;
        dirt->spd = *spd;
        dirt->scale = scale;
        dirt->ang = RANDOM_ANGLE(TURN);
        dirt->frame = (Float)rand() * (HEAD_START * RAND_SCALE);
        dirt->ang_spd = RANDOM_ANGLE(SPIN);
    }
    return dirt;
}
