/* dust: puffs that drift and grow as they age, and the tasks that raise them
 * at the players' feet */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "shared/dust/text.inc"

/* a puff starts turned up to TURN degrees either way. it turns up to SPIN
 * degrees a step either way */
#define TURN 10.0F
#define SPIN 0.5F

/* dust_fade_step and dust_swing_step swing a puff's edge_angle up to SWING
 * degrees either side of a quarter turn. a swing from one side to the other
 * takes SWING_STEPS steps. the swing's phase, in degrees, starts at random
 * below 2 * SWING */
#define SWING 8
#define SWING_STEPS 60
#define SWING_RATE (2.0F * SWING / SWING_STEPS)
/* a swinging puff keeps its swing's phase in depth_bias, which the draw reads
 * only under PARTICLE_DEPTH. no group whose step swings sets that flag */
#define SWING_PHASE(particle) ((particle)->depth_bias)

/* density, in dust_rect and the other rect calls, is the puffs to each
 * DENSITY_SPAN units of edge */
#define DENSITY_SPAN 10.0F

/* a sphere's puff lies up to SPHERE_TILT degrees from the sphere's top */
#define SPHERE_TILT 120.0F

/* the players who can have a smoke task and a sand task: 0 and 1 */
#define DUST_PLAYERS 2
/* the task level of a smoke or sand task made without a parent */
#define DUST_TASK_LEVEL 1

/* a smoke or sand task's work keeps its player's slot in tick. its timer
 * counts the steps since the task began, its last ring or the smoke task's
 * last shake. its flags keep the player's work's flags of the step before */
#define SLOT(work) ((work)->tick)
/* the player's animation frame of the step before */
#define LAST_FRAME(work) ((work)->scl.x)
/* the player's vertical speed of the step before, without its sign */
#define LAST_FALL_SPEED(work) ((work)->scl.y)

/* a task's landing, running and cycle rings have the radius RING_RADIUS about
 * the player's pos. a ring's puffs gain SPEED_SHARE of the player's motion spd
 * in x and z. they gain KICK times LAST_FALL_SPEED upward. a ring needs the
 * work's timer above RING_WAIT */
#define RING_RADIUS 1.1F
#define SPEED_SHARE 0.3F
#define KICK 0.1F
#define RING_WAIT 12
/* a landing ring comes on the step the player lands. it has LANDING_PUFFS
 * puffs of size LANDING_SCALE */
#define LANDING_PUFFS 7
#define LANDING_SCALE 2.1F
/* a running ring comes as the animation of a player on the ground starts
 * over. the animation starts over where the frame is 0 or more and below the
 * last step's. the ring needs the player at RUNNING_SPEED or more along its
 * own x. a sand task's ring needs PLAYER_MODE_RUN too. a running ring has
 * RUNNING_PUFFS puffs of size RUNNING_SCALE */
#define RUNNING_SPEED 0.1F
#define RUNNING_PUFFS 3
#define RUNNING_SCALE 2.2F
/* a cycle ring needs the player in PLAYER_MODE_BRAKE, and no running ring on
 * the same step. it comes as the animation starts over. a smoke task's comes in
 * a Sonic's or a Shadow's PLAYER_MODE_SONIC_COMBO_WAIT too. a cycle ring of
 * smoke has CYCLE_PUFFS puffs, which gain CYCLE_SPEED_SHARE in place of
 * SPEED_SHARE. one of sand has SAND_CYCLE_PUFFS, which gain
 * SAND_CYCLE_SPEED_SHARE. either ring's puffs have the size CYCLE_SCALE */
#define CYCLE_PUFFS 4
#define CYCLE_SPEED_SHARE 1.3F
#define SAND_CYCLE_PUFFS 5
#define SAND_CYCLE_SPEED_SHARE 0.6F
#define CYCLE_SCALE 2.2F

/* the smoke task raises plumes in the player's PLAYER_MODE_UNK_38. it raises
 * them too while a Sonic plays ANIM_SINK, or a mech ANIM_SINK_EGGMAN_MECH or
 * ANIM_SINK_TAILS_MECH. a plume comes on each step whose timer is a multiple
 * of PLUME_STEPS, a power of 2. it is a ring of PLUME_PUFFS puffs of size
 * PLUME_SCALE, of the player's radius. its puffs gain PLUME_RISE upward.
 *
 * a plume on a step whose timer is past SHAKE_WAIT also shakes the player's
 * pad. the shake runs at SHAKE_POWER and SHAKE_FREQ, and sets the timer to 0.
 * vibration_start clamps SHAKE_FREQ up to 15 */
#define PLUME_STEPS 8
#define PLUME_PUFFS 4
#define PLUME_SCALE 4.0F
#define PLUME_RISE 0.4F
#define SHAKE_WAIT 64
#define SHAKE_POWER 3
#define SHAKE_FREQ 7
/* the sound a plume plays on each of four stages. each id picks a sound of
 * place 5 of sound/sound.h's sound lists. the stage keeps its own sounds
 * there */
#define SAND_OCEAN_PLUME_SOUND 6
#define HIDDEN_BASE_PLUME_SOUND 7
#define GOLEM_PLUME_SOUND 13
#define GOLEM_E_PLUME_SOUND 15
#define SOUND_PRIORITY 1
#define SOUND_DURATION 9

static int dust_fade_step(struct particle_group *group,
                          struct particle *particle);
static int dust_swing_step(struct particle_group *group,
                           struct particle *particle);
static int dust_steady_step(struct particle_group *group,
                            struct particle *particle);
static int dust_steady_gravity_step(struct particle_group *group,
                                    struct particle *particle);
static void smoke_task_forget(struct task *task);
static void smoke_task_step(struct task *task);
static void sand_task_forget(struct task *task);
static void sand_task_step(struct task *task);

static NJS_TEXNAME dust_texnames[DUST_FRAMES] = {
#include "shared/dust/dust_texnames.inc"
};
NJS_TEXLIST dust_texlist = {
#include "shared/dust/dust_texlist.inc"
};

static struct particle dust_smoke_template = {
#include "shared/dust/dust_smoke_template.inc"
};
static struct particle_group dust_smoke_group = {
#include "shared/dust/dust_smoke_group.inc"
};
static struct particle dust_sand_template = {
#include "shared/dust/dust_sand_template.inc"
};
static struct particle_group dust_sand_group = {
#include "shared/dust/dust_sand_group.inc"
};
/* the groups from dust_group on have no template. their puffs take the
 * particle system's own, whose color is opaque white */
static struct particle_group dust_group = {
#include "shared/dust/dust_group.inc"
};
static struct particle_group dust_short_group = {
#include "shared/dust/dust_short_group.inc"
};
static struct particle_group dust_steady_group = {
#include "shared/dust/dust_steady_group.inc"
};
static struct particle_group dust_steady_gravity_group = {
#include "shared/dust/dust_steady_gravity_group.inc"
};

/* each player's smoke task, or null */
static struct task *smoke_tasks[DUST_PLAYERS] = {
#include "shared/dust/smoke_tasks.inc"
};
/* each player's sand task, or null */
static struct task *sand_tasks[DUST_PLAYERS] = {
#include "shared/dust/sand_tasks.inc"
};

struct particle *dust_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale)
{
    struct particle *puff;

    puff = particle_create(&dust_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

struct particle *dust_smoke_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                   Float scale)
{
    struct particle *puff;

    puff = particle_create(&dust_smoke_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

struct particle *dust_sand_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale)
{
    struct particle *puff;

    puff = particle_create(&dust_sand_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

struct particle *dust_short_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                   Float scale)
{
    struct particle *puff;

    puff = particle_create(&dust_short_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2.0F * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

struct particle *dust_steady_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                    Float scale, Float depth_bias)
{
    struct particle *puff;

    puff = particle_create(&dust_steady_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        puff->depth_bias = depth_bias;
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

struct particle *dust_steady_gravity_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, Float depth_bias)
{
    struct particle *puff;

    puff = particle_create(&dust_steady_gravity_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        puff->depth_bias = depth_bias;
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

/* the distance from x to the nearest multiple of 2 * half. as x grows, the
 * distance climbs from 0 to half and falls back. half must not be 0 */
static int triangle_wave(Uint32 x, Uint32 half)
{
    Uint32 period = half * 2;

    x = x % period;
    if (x >= half) {
        x = period - x;
    }
    return x;
}

/* particle_move with the swing, but the puff stays at its last texture as it
 * fades. each time the frame reaches the group's frames, the step halves the
 * alpha. it sets the frame back to frames - 1. it ends the puff once the alpha
 * is 0 */
static int dust_fade_step(struct particle_group *group,
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
        QUARTER_TURN +
        (Angle)((triangle_wave((Uint32)SWING_PHASE(particle), 2 * SWING) -
                 SWING) *
                ANGLE_PER_DEGREE);
    SWING_PHASE(particle) += SWING_RATE;
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

/* particle_move with the swing */
static int dust_swing_step(struct particle_group *group,
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
        QUARTER_TURN +
        (Angle)((triangle_wave((Uint32)SWING_PHASE(particle), 2 * SWING) -
                 SWING) *
                ANGLE_PER_DEGREE);
    SWING_PHASE(particle) += SWING_RATE;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

/* the same step as particle_move */
static int dust_steady_step(struct particle_group *group,
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

/* dust_steady_step, but the group's gravity speeds the puff against
 * world_gravity, not up y */
static int dust_steady_gravity_step(struct particle_group *group,
                                    struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x = (particle->spd.x * group->friction) -
                      (group->gravity * world_gravity.x);
    particle->spd.y = (particle->spd.y * group->friction) -
                      (group->gravity * world_gravity.y);
    particle->spd.z = (particle->spd.z * group->friction) -
                      (group->gravity * world_gravity.z);
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

void dust_rect(NJS_POINT3 *pos, Float half_x, Float half_z, struct angles *ang,
               Float scale, Float density, Float speed)
{
    NJS_POINT3 edge;
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Float factor;
    int across;
    int deep;
    int i;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateZ(NULL, ang->z);
    njRotateY(NULL, ang->y);
    njRotateX(NULL, ang->x);
    /* @hack spelling: density reused for the space between puffs, where a
     * local of its own would do */
    density = DENSITY_SPAN / density;
    across = half_x * 2.0F / density;
    deep = half_z * 2.0F / density;
    edge.y = 0.0F;
    for (i = 0; i < across; i++) {
        edge.x = (i * density) - half_x;
        edge.z = half_z;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_create(&point, &velocity, scale);
        edge.z = -half_z;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_create(&point, &velocity, scale);
    }
    for (i = 0; i < deep; i++) {
        edge.z = (i * density) - half_z;
        edge.x = half_x;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_create(&point, &velocity, scale);
        edge.x = -half_x;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_create(&point, &velocity, scale);
    }
    njPopMatrixEx();
}

void dust_smoke_rect(NJS_POINT3 *pos, Float half_x, Float half_z,
                     struct angles *ang, Float scale, Float density,
                     Float speed)
{
    NJS_POINT3 edge;
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Float factor;
    int across;
    int deep;
    int i;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateZ(NULL, ang->z);
    njRotateY(NULL, ang->y);
    njRotateX(NULL, ang->x);
    /* @hack spelling: density reused for the space between puffs, where a
     * local of its own would do */
    density = DENSITY_SPAN / density;
    across = half_x * 2.0F / density;
    deep = half_z * 2.0F / density;
    edge.y = 0.0F;
    for (i = 0; i < across; i++) {
        edge.x = (i * density) - half_x;
        edge.z = half_z;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_smoke_create(&point, &velocity, scale);
        edge.z = -half_z;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_smoke_create(&point, &velocity, scale);
    }
    for (i = 0; i < deep; i++) {
        edge.z = (i * density) - half_z;
        edge.x = half_x;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_smoke_create(&point, &velocity, scale);
        edge.x = -half_x;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_smoke_create(&point, &velocity, scale);
    }
    njPopMatrixEx();
}

void dust_short_rect(NJS_POINT3 *pos, Float half_x, Float half_z,
                     struct angles *ang, Float scale, Float density,
                     Float speed)
{
    NJS_POINT3 edge;
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Float factor;
    int across;
    int deep;
    int i;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateY(NULL, ang->y);
    njRotateZ(NULL, ang->z);
    /* @hack spelling: density reused for the space between puffs, where a
     * local of its own would do */
    density = DENSITY_SPAN / density;
    across = half_x * 2.0F / density;
    deep = half_z * 2.0F / density;
    edge.y = 0.0F;
    for (i = 0; i < across; i++) {
        edge.x = (i * density) - half_x;
        edge.z = half_z;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_short_create(&point, &velocity, scale);
        edge.z = -half_z;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_short_create(&point, &velocity, scale);
    }
    for (i = 0; i < deep; i++) {
        edge.z = (i * density) - half_z;
        edge.x = half_x;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_short_create(&point, &velocity, scale);
        edge.x = -half_x;
        njCalcPoint(NULL, &edge, &point);
        njCalcVector(NULL, &edge, &velocity);
        factor = speed / njScalor(&velocity);
        velocity.x *= factor;
        velocity.y *= factor;
        velocity.z *= factor;
        dust_short_create(&point, &velocity, scale);
    }
    njPopMatrixEx();
}

void dust_sphere(NJS_POINT3 *pos, Float radius, Float speed, Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    NJS_VECTOR spd;
    NJS_VECTOR velocity;

    spd.x = 0.0F;
    spd.y = speed;
    spd.z = 0.0F;
    offset.x = 0.0F;
    offset.y = radius;
    offset.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateY(NULL, (Angle)((Float)rand() * (2 * FULL_TURN * RAND_SCALE)));
    njRotateX(NULL, (Angle)((((Float)rand() * RAND_SCALE) - 0.5F) *
                            (2 * SPHERE_TILT * ANGLE_PER_DEGREE)));
    njCalcPoint(NULL, &offset, &point);
    njCalcVector(NULL, &spd, &velocity);
    dust_create(&point, &velocity, scale);
    njPopMatrixEx();
}

void dust_short_sphere(NJS_POINT3 *pos, Float radius, Float speed, Float scale)
{
    NJS_POINT3 offset;
    NJS_POINT3 point;
    NJS_VECTOR spd;
    NJS_VECTOR velocity;

    spd.x = 0.0F;
    spd.y = speed;
    spd.z = 0.0F;
    offset.x = 0.0F;
    offset.y = radius;
    offset.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateY(NULL, (Angle)((Float)rand() * (2 * FULL_TURN * RAND_SCALE)));
    njRotateX(NULL, (Angle)((((Float)rand() * RAND_SCALE) - 0.5F) *
                            (2 * SPHERE_TILT * ANGLE_PER_DEGREE)));
    njCalcPoint(NULL, &offset, &point);
    njCalcVector(NULL, &spd, &velocity);
    dust_short_create(&point, &velocity, scale);
    njPopMatrixEx();
}

void dust_ring(NJS_POINT3 *pos, Float radius, Float scale, int count)
{
    NJS_VECTOR ring_velocity = {
#include "shared/dust/ring_velocity.inc"
    };
    NJS_POINT3 ring_offset = {
#include "shared/dust/ring_offset.inc"
    };
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Angle step;
    Angle angle;
    /* @hack type: i, a Uint8, where an int would do for a count below 256 */
    Uint8 i;

    step = FULL_TURN / count;
    angle = (Float)rand() * (FULL_TURN * RAND_SCALE);
    ring_offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    ring_velocity.y *= scale;
    ring_velocity.z *= scale;
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < count; i++) {
        /* @bug logic: the puffs don't lie a step apart around the ring, since
         * the turns add up twice. angle already sums the steps, and each
         * njRotateY adds angle to the matrix's turn, which the loop never sets
         * back (the JSR to njRotateY at 0x1242 of the object's code) */
        njRotateY(NULL, angle);
        njCalcPoint(NULL, &ring_offset, &point);
        njCalcVector(NULL, &ring_velocity, &velocity);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        dust_create(&point, &velocity, scale);
        angle += step;
    }
    njPopMatrixEx();
}

void dust_short_ring(NJS_POINT3 *pos, Float radius, Float scale, int count)
{
    NJS_VECTOR short_ring_velocity = {
#include "shared/dust/short_ring_velocity.inc"
    };
    NJS_POINT3 short_ring_offset = {
#include "shared/dust/short_ring_offset.inc"
    };
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Angle step;
    Angle angle;
    /* @hack type: i, a Uint8, where an int would do for a count below 256 */
    Uint8 i;

    step = FULL_TURN / count;
    angle = (Float)rand() * (FULL_TURN * RAND_SCALE);
    short_ring_offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    short_ring_velocity.y *= scale;
    short_ring_velocity.z *= scale;
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < count; i++) {
        /* @bug logic: the turns add up twice, as in dust_ring (the JSR to
         * njRotateY at 0x13ac of the object's code) */
        njRotateY(NULL, angle);
        njCalcPoint(NULL, &short_ring_offset, &point);
        njCalcVector(NULL, &short_ring_velocity, &velocity);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        dust_short_create(&point, &velocity, scale);
        angle += step;
    }
    njPopMatrixEx();
}

/* dust_ring, but of dust_smoke_create's puffs, which move out at
 * its slower outward speed times scale. each puff's speed gains base. angle
 * gains a random part below half a step before each puff */
static void dust_smoke_ring(NJS_POINT3 *pos, NJS_VECTOR *base, Float radius,
                            Float scale, int count)
{
    NJS_VECTOR smoke_ring_velocity = {
#include "shared/dust/smoke_ring_velocity.inc"
    };
    NJS_POINT3 smoke_ring_offset = {
#include "shared/dust/smoke_ring_offset.inc"
    };
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Angle step;
    Angle angle;
    int i;

    step = FULL_TURN / count;
    angle = (Float)rand() * (FULL_TURN * RAND_SCALE);
    smoke_ring_offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    smoke_ring_velocity.y *= scale;
    smoke_ring_velocity.z *= scale;
    for (i = 0; i < count; i++) {
        angle += (Float)step * rand() * (0.5F * RAND_SCALE);
        /* @bug logic: the turns add up twice, as in dust_ring (the JSR to
         * njRotateY at 0x14d4 of the object's code) */
        njRotateY(NULL, angle);
        njCalcPoint(NULL, &smoke_ring_offset, &point);
        njCalcVector(NULL, &smoke_ring_velocity, &velocity);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        velocity.x += base->x;
        velocity.y += base->y;
        velocity.z += base->z;
        dust_smoke_create(&point, &velocity, scale);
        angle += step;
    }
    njPopMatrixEx();
}

/* dust_smoke_ring, but of dust_sand_create's puffs */
static void dust_sand_ring(NJS_POINT3 *pos, NJS_VECTOR *base, Float radius,
                           Float scale, int count)
{
    NJS_VECTOR sand_ring_velocity = {
#include "shared/dust/sand_ring_velocity.inc"
    };
    NJS_POINT3 sand_ring_offset = {
#include "shared/dust/sand_ring_offset.inc"
    };
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Angle step;
    Angle angle;
    int i;

    step = FULL_TURN / count;
    angle = (Float)rand() * (FULL_TURN * RAND_SCALE);
    sand_ring_offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    sand_ring_velocity.y *= scale;
    sand_ring_velocity.z *= scale;
    for (i = 0; i < count; i++) {
        angle += (Float)step * rand() * (0.5F * RAND_SCALE);
        /* @bug logic: the turns add up twice, as in dust_ring (the JSR to
         * njRotateY at 0x166e of the object's code) */
        njRotateY(NULL, angle);
        njCalcPoint(NULL, &sand_ring_offset, &point);
        njCalcVector(NULL, &sand_ring_velocity, &velocity);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        velocity.x += base->x;
        velocity.y += base->y;
        velocity.z += base->z;
        dust_sand_create(&point, &velocity, scale);
        angle += step;
    }
    njPopMatrixEx();
}

void dust_smoke_arc(NJS_POINT3 *pos, Float radius, Float scale, int count)
{
    NJS_VECTOR smoke_arc_velocity = {
#include "shared/dust/smoke_arc_velocity.inc"
    };
    NJS_POINT3 smoke_arc_offset = {
#include "shared/dust/smoke_arc_offset.inc"
    };
    NJS_POINT3 point;
    NJS_VECTOR velocity;
    Angle step;
    Angle angle;
    /* @hack type: i, a Uint8, where an int would do for a count below 256 */
    Uint8 i;

    step = HALF_TURN / count;
    angle =
        camera_views[camera_current_index]->ang.y + (step / 2) - QUARTER_TURN;
    smoke_arc_offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    smoke_arc_velocity.y *= scale;
    smoke_arc_velocity.z *= scale;
    njRotateY(NULL, angle);
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < count; i++) {
        njCalcPoint(NULL, &smoke_arc_offset, &point);
        njCalcVector(NULL, &smoke_arc_velocity, &velocity);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        dust_smoke_create(&point, &velocity, scale);
        njRotateY(NULL, step);
    }
    njPopMatrixEx();
}

struct task *smoke_task_create(int player, struct task *parent)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work;

    if (player < DUST_PLAYERS && smoke_tasks[player] == NULL) {
        if (parent != NULL) {
            task = task_create_child(TASK_WORK, smoke_task_step, parent);
        } else {
            task = task_create(TASK_WORK, DUST_TASK_LEVEL, smoke_task_step,
                               SMOKE_TASK_NAME);
        }
        if (task != NULL) {
            work = task->work;
            task->destructor = smoke_task_forget;
            smoke_tasks[player] = task;
            SLOT(work) = player;
            return task;
        }
    }
    return NULL;
}

/* empty the player's entry of smoke_tasks where it holds task, so the player
 * can have another. sand_task_forget does the same for sand_tasks */
static void smoke_task_forget(struct task *task)
{
    if (smoke_tasks[SLOT(task->work)] == task) {
        smoke_tasks[SLOT(task->work)] = NULL;
    }
}

static void smoke_task_step(struct task *task)
{
    struct task_work *work;
    struct task_work *player_task_work;
    struct player *record;
    NJS_VECTOR spd;
    Float frame;
    int player;

    work = task->work;
    player = SLOT(work);
    /* @hack spelling: the assignments inside the test, where a statement of
     * its own for each would do */
    if ((player_task_work = player_work[player]) == NULL ||
        (record = player_records[player]) == NULL) {
        task_destroy(task);
        return;
    }
    frame = record->anim.frame;
    if (!(record->ahead.above_flags & WORLD_FLAT) &&
        (record->ahead.below_flags & WORLD_DUSTY)) {
        if (((work->flags ^ player_task_work->flags) & player_task_work->flags &
             PLAYER_WORK_ON_GROUND) &&
            work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * KICK;
            spd.z *= SPEED_SHARE;
            dust_smoke_ring(&player_task_work->pos, &spd, RING_RADIUS,
                            LANDING_SCALE, LANDING_PUFFS);
            work->timer = 0;
        }
        if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
            record->spd.x >= RUNNING_SPEED && frame >= 0.0F &&
            LAST_FRAME(work) > frame && work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * KICK;
            spd.z *= SPEED_SHARE;
            dust_smoke_ring(&player_task_work->pos, &spd, RING_RADIUS,
                            RUNNING_SCALE, RUNNING_PUFFS);
            work->timer = 0;
        } else if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
                   (player_task_work->mode == PLAYER_MODE_BRAKE ||
                    /* @hack spelling: PLAYER_MODE_SONIC_COMBO_WAIT tested for
                     * Sonic and for Shadow apart, where one test for either
                     * would do */
                    (record->character == PLAYER_SONIC &&
                     player_task_work->mode == PLAYER_MODE_SONIC_COMBO_WAIT) ||
                    (record->character == PLAYER_SHADOW &&
                     player_task_work->mode == PLAYER_MODE_SONIC_COMBO_WAIT)) &&
                   frame >= 0.0F && LAST_FRAME(work) > frame &&
                   work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= CYCLE_SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * KICK;
            spd.z *= CYCLE_SPEED_SHARE;
            dust_smoke_ring(&player_task_work->pos, &spd, RING_RADIUS,
                            CYCLE_SCALE, CYCLE_PUFFS);
            work->timer = 0;
        }
    }
    if ((record->character == PLAYER_SONIC &&
         record->anim.current == ANIM_SINK) ||
        (record->character == PLAYER_EGGMAN_MECH &&
         record->anim.current == ANIM_SINK_EGGMAN_MECH) ||
        (record->character == PLAYER_TAILS_MECH &&
         record->anim.current == ANIM_SINK_TAILS_MECH) ||
        player_task_work->mode == PLAYER_MODE_UNK_38) {
        if (!(work->timer & (PLUME_STEPS - 1))) {
            spd.x = 0.0F;
            spd.y = PLUME_RISE;
            spd.z = 0.0F;
            dust_smoke_ring(&player_task_work->pos, &spd, record->consts.radius,
                            PLUME_SCALE, PLUME_PUFFS);
            switch (stage_current_id()) {
            case STAGE_SAND_OCEAN:
                sound_play_at_for(SAND_OCEAN_PLUME_SOUND, work, SOUND_PRIORITY,
                                  SOUND_LOUDEST, SOUND_DURATION, &work->pos);
                break;
            case STAGE_HIDDEN_BASE:
                sound_play_at_for(HIDDEN_BASE_PLUME_SOUND, work, SOUND_PRIORITY,
                                  SOUND_LOUDEST, SOUND_DURATION, &work->pos);
                break;
            case STAGE_BOSS_GOLEM:
                sound_play_at_for(GOLEM_PLUME_SOUND, work, SOUND_PRIORITY,
                                  SOUND_LOUDEST, SOUND_DURATION, &work->pos);
                break;
            case STAGE_BOSS_GOLEM_E:
                sound_play_at_for(GOLEM_E_PLUME_SOUND, work, SOUND_PRIORITY,
                                  SOUND_LOUDEST, SOUND_DURATION, &work->pos);
                break;
            }
            if (work->timer > SHAKE_WAIT) {
                vibration_start(player, SHAKE_POWER, SHAKE_FREQ, 0);
                work->timer = 0;
            }
        }
    }
    work->flags = player_task_work->flags;
    LAST_FALL_SPEED(work) = njAbs(player_motion[player]->spd.y);
    work->timer++;
    LAST_FRAME(work) = frame;
}

struct task *sand_task_create(int player, struct task *parent)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work;

    if (player < DUST_PLAYERS && sand_tasks[player] == NULL) {
        if (parent != NULL) {
            task = task_create_child(TASK_WORK, sand_task_step, parent);
        } else {
            task = task_create(TASK_WORK, DUST_TASK_LEVEL, sand_task_step,
                               SAND_TASK_NAME);
        }
        if (task != NULL) {
            work = task->work;
            task->destructor = sand_task_forget;
            sand_tasks[player] = task;
            SLOT(work) = player;
            return task;
        }
    }
    return NULL;
}

static void sand_task_forget(struct task *task)
{
    if (sand_tasks[SLOT(task->work)] == task) {
        sand_tasks[SLOT(task->work)] = NULL;
    }
}

static void sand_task_step(struct task *task)
{
    struct task_work *work;
    struct task_work *player_task_work;
    NJS_VECTOR spd;
    Float frame;
    int player;

    work = task->work;
    player = SLOT(work);
    player_task_work = player_work[player];
    if (player_task_work == NULL) {
        task_destroy(task);
        return;
    }
    /* @bug memory: reads the player's record with no test for null, where
     * smoke_task_step tests it (FMOV.S @R3,FR15 at 0x1df2 of the object's
     * code) */
    frame = player_records[player]->anim.frame;
    if ((player_records[player]->ahead.above_flags & WORLD_FLAT) &&
        (player_records[player]->ahead.below_flags & WORLD_DUSTY)) {
        if (((work->flags ^ player_task_work->flags) & player_task_work->flags &
             PLAYER_WORK_ON_GROUND) &&
            work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * KICK;
            spd.z *= SPEED_SHARE;
            dust_sand_ring(&player_task_work->pos, &spd, RING_RADIUS,
                           LANDING_SCALE, LANDING_PUFFS);
            work->timer = 0;
        }
        if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
            player_task_work->mode == PLAYER_MODE_RUN &&
            player_records[player]->spd.x >= RUNNING_SPEED && frame >= 0.0F &&
            LAST_FRAME(work) > frame && work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * KICK;
            spd.z *= SPEED_SHARE;
            dust_sand_ring(&player_task_work->pos, &spd, RING_RADIUS,
                           RUNNING_SCALE, RUNNING_PUFFS);
            work->timer = 0;
        } else if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
                   player_task_work->mode == PLAYER_MODE_BRAKE &&
                   frame >= 0.0F && LAST_FRAME(work) > frame &&
                   work->timer > RING_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= SAND_CYCLE_SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * KICK;
            spd.z *= SAND_CYCLE_SPEED_SHARE;
            dust_sand_ring(&player_task_work->pos, &spd, RING_RADIUS,
                           CYCLE_SCALE, SAND_CYCLE_PUFFS);
            work->timer = 0;
        }
    }
    /* @hack redundant: a store of twice frame to LAST_FRAME(work), in a block
     * no path reaches */
    if (0) {
        LAST_FRAME(work) = frame + frame;
    }
    work->flags = player_task_work->flags;
    LAST_FALL_SPEED(work) = njAbs(player_motion[player]->spd.y);
    work->timer++;
    LAST_FRAME(work) = frame;
}
