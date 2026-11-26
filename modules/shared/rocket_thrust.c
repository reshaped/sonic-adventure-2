/* puffs of thrust: particles of the textures roketthrust01 to roketthrust08 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/particle.h"
#include "shared/rocket_thrust.h"
#include "system/angle.h"
#include "system/util.h"

/* the puffs' frames, one for each texture */
#define PUFF_FRAMES 8
/* a puff starts turned by up to TURN degrees either way. its quad's second
 * edge lies a quarter turn from its first, give or take EDGE_TURN degrees.
 * it turns by up to SPIN degrees a step either way */
#define TURN 10.0F
#define EDGE_TURN 5.0F
#define SPIN 0.5F

/* a settling puff keeps its floor in depth_bias, which the draw reads only
 * under PARTICLE_DEPTH. no group here sets that flag */
#define FLOOR(puff) ((puff)->depth_bias)

/* a falling puff less than SETTLE_BAND above its floor levels out. a step
 * there shrinks its speed in y to SETTLE_FALL of itself. the step then scales
 * the speed to SETTLE_SLOW of its length before the shrink. the SETTLE_WIDE_
 * constants are settle_wide_group's */
#define SETTLE_BAND 10.0F
#define SETTLE_FALL 0.94F
#define SETTLE_SLOW 0.99F
#define SETTLE_WIDE_BAND 50.0F
#define SETTLE_WIDE_FALL 0.93F
#define SETTLE_WIDE_SLOW 0.995F
/* a puff that a step takes below its floor goes back onto it. there it
 * slides level at the speed it had. where its level speed is under STILL, the
 * puff first takes a random level direction. that direction's x and z are each
 * drawn within NUDGE / 2 of 0 */
#define STILL 0.001F
#define NUDGE 0.1F

static int rocket_thrust_move(struct particle_group *group,
                              struct particle *particle);
static int rocket_thrust_settle(struct particle_group *group,
                                struct particle *particle);
static int rocket_thrust_settle_wide(struct particle_group *group,
                                     struct particle *particle);

#include "shared/rocket_thrust/text.inc"

static NJS_TEXNAME rocket_thrust_texname[PUFF_FRAMES] = {
#include "shared/rocket_thrust/rocket_thrust_texname.inc"
};
NJS_TEXLIST rocket_thrust_texlist = {rocket_thrust_texname, PUFF_FRAMES};

/* the groups have no template. their puffs take the particle system's own,
 * whose color is opaque white */
/* clang-format off */
static struct particle_group move_group = {
    PARTICLE_BLEND_ADD, &rocket_thrust_texlist, 0, PUFF_FRAMES,
    0.5F, 0.93F, 0.007F, 0.07F, rocket_thrust_move, 22500.0F,
    NULL, 0, NULL, NULL,
};
static struct particle_group settle_group = {
    PARTICLE_BLEND_ADD, &rocket_thrust_texlist, 0, PUFF_FRAMES,
    0.6F, 0.95F, 0.008F, 0.07F, rocket_thrust_settle, 22500.0F,
    NULL, 0, NULL, NULL,
};
static struct particle_group settle_wide_group = {
    PARTICLE_BLEND_ADD, &rocket_thrust_texlist, 0, PUFF_FRAMES,
    0.35F, 0.86F, 0.3F, 0.2F, rocket_thrust_settle_wide, 40000.0F,
    NULL, 0, NULL, NULL,
};
/* clang-format on */
/* the share of its speed in y that a puff of settle_wide_group keeps at its
 * floor. nothing writes it, so the speed turns level there */
static Float floor_spd_y_share = 0.0F;

struct particle *rocket_thrust_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                      Float scale)
{
    struct particle *puff;

    puff = particle_create(&move_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->edge_angle = RANDOM_ANGLE(EDGE_TURN) + QUARTER_TURN;
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        puff->frame += rand() * RAND_SCALE;
    }
    return puff;
}

/* move_group's step, a copy of particle_move */
static int rocket_thrust_move(struct particle_group *group,
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
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

struct particle *rocket_thrust_settle_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                             Float scale, Float floor)
{
    struct particle *puff;

    puff = particle_create(&settle_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        /* @hack spelling: ang_spd set before edge_angle, where after it, as
         * rocket_thrust_create sets them, would do */
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        puff->edge_angle = RANDOM_ANGLE(EDGE_TURN) + QUARTER_TURN;
        FLOOR(puff) = floor;
        puff->frame += rand() * RAND_SCALE;
    }
    return puff;
}

/* move the puff as rocket_thrust_move does, then settle it at its floor */
static int rocket_thrust_settle(struct particle_group *group,
                                struct particle *particle)
{
    Float length;
    Float scale;

    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    if (particle->spd.y < 0.0F &&
        FLOOR(particle) + SETTLE_BAND > particle->pos.y) {
        length = njScalor(&particle->spd);
        particle->spd.y *= SETTLE_FALL;
        length *= SETTLE_SLOW;
        scale = length / njScalor(&particle->spd);
        particle->spd.x *= scale;
        particle->spd.y *= scale;
        particle->spd.z *= scale;
    }
    if (particle->pos.y < FLOOR(particle)) {
        length = njScalor(&particle->spd);
        particle->spd.y = 0.0F;
        /* @hack spelling: scale reused for the level speed it divides, where
         * a local of its own would do */
        scale = njScalor(&particle->spd);
        if (scale < STILL) {
            particle->spd.x = ((rand() * RAND_SCALE) - 0.5F) * NUDGE;
            particle->spd.z = ((rand() * RAND_SCALE) - 0.5F) * NUDGE;
            scale = njScalor(&particle->spd);
        }
        scale = length / scale;
        particle->spd.x *= scale;
        particle->spd.y *= scale;
        particle->spd.z *= scale;
        particle->pos.y = FLOOR(particle);
    }
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

struct particle *rocket_thrust_settle_wide_create(NJS_POINT3 *pos,
                                                  NJS_VECTOR *spd, Float scale,
                                                  Float floor)
{
    struct particle *puff;

    puff = particle_create(&settle_wide_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        /* @hack spelling: ang_spd set before edge_angle, where after it, as
         * rocket_thrust_create sets them, would do */
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        puff->edge_angle = RANDOM_ANGLE(EDGE_TURN) + QUARTER_TURN;
        FLOOR(puff) = floor;
        puff->frame += rand() * RAND_SCALE;
    }
    return puff;
}

/* step as rocket_thrust_settle does, with the SETTLE_WIDE_ constants. at the
 * floor, keep floor_spd_y_share of the speed in y */
static int rocket_thrust_settle_wide(struct particle_group *group,
                                     struct particle *particle)
{
    Float length;
    Float scale;

    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    if (particle->spd.y < 0.0F &&
        FLOOR(particle) + SETTLE_WIDE_BAND > particle->pos.y) {
        length = njScalor(&particle->spd);
        particle->spd.y *= SETTLE_WIDE_FALL;
        length *= SETTLE_WIDE_SLOW;
        scale = length / njScalor(&particle->spd);
        particle->spd.x *= scale;
        particle->spd.y *= scale;
        particle->spd.z *= scale;
    }
    if (particle->pos.y < FLOOR(particle)) {
        length = njScalor(&particle->spd);
        particle->spd.y *= floor_spd_y_share;
        /* @hack spelling: scale reused for the speed it divides, where a
         * local of its own would do */
        scale = njScalor(&particle->spd);
        if (scale < STILL) {
            particle->spd.x = ((rand() * RAND_SCALE) - 0.5F) * NUDGE;
            particle->spd.z = ((rand() * RAND_SCALE) - 0.5F) * NUDGE;
            scale = njScalor(&particle->spd);
        }
        scale = length / scale;
        particle->spd.x *= scale;
        particle->spd.y *= scale;
        particle->spd.z *= scale;
        particle->pos.y = FLOOR(particle);
    }
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}
