/* Final Rush's rocks. the background starts a boulder at the rockfall's top and
 * throws shards along it. a placement can start either */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "effect/burst.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/flash.h"
#include "stg30/rockfall.h"
#include "stg30/rockfall_art.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#define ROCK_KIND(work) ((Uint32)(work)->scl.x % ROCK_KINDS)
#define ROCK_SHAPES 1
#define PICK_RANGE 100.0F

/* the floor's frame turns about y only where the normal's part across x and z
 * exceeds HEADING_MIN */
#define HEADING_MIN 0.01F
#define PROBE_LIFT 30.0F /* a shard's search, above its pos */
#define PLAYER_PROBE_LIFT 10.0F /* the boulder's search, above player 0 */
/* the steps between the boulder's further calls of find_floor, which do
 * nothing */
#define PROBE_PERIOD 10
/* a waiting rock wakes within scl.y plus MARGIN of a player. a rock rides the
 * height of its turned size plus MARGIN above the floor */
#define MARGIN 10.0F

#define PASSED_Z 7500.0F
/* the brake's reach past the length of the rock's size */
#define BRAKE_NEAR 150.0F
#define BOULDER_BRAKE_STEPS 50
#define BOULDER_BRAKE_RANGE 30.0F
#define SHARD_BRAKE_RANGE 10.0F
#define BRAKE_DRAG 0.9F /* the share of velocity.z a braking step keeps */
/* a rock fades while its timer runs from FADE_STEPS - 1 down to 1. each of
 * those steps keeps FADE_DRAG of its velocity. it isn't drawn while
 * game_state.steps has BLINK_BIT set */
#define FADE_STEPS 30
#define FADE_DRAG 0.98F
#define BLINK_BIT 2
/* the boulder bumps a player it meets while its velocity.z is 0 or less. a
 * bump flashes at player 0, and the boulder rolls back toward +z */
#define BUMP_FLASH_SCALE 8.0F
#define BUMP_FLASH_SPARKS 0
#define BUMP_FLASH_DEPTH (-8.0F)
#define BUMP_REBOUND 0.2F /* the share of its speed along z it keeps */
#define BUMP_BRAKE_STEPS 120
/* the share of each spin angle's low 16 bits that each step keeps */
#define SPIN_DRAG 0.99F
/* the least part of a wall's normal along its axis */
#define WALL_NORMAL 0.9F
#define WALL_REBOUND 0.7F /* the share of its speed a rock keeps off a wall */
/* a landing rock rolls ROLL_RATE angle units a step per unit of speed across x
 * and z */
#define ROLL_RATE 417.42676F
/* a landing step kicks the rock up by KICK_SPEED with the chance KICK_CHANCE */
#define KICK_CHANCE 0.01F
#define KICK_SPEED 3.0F
/* each landing gives the rock LANDING_SPARKS bursts of sparks to shed */
#define LANDING_SPARKS 5
#define SPARK_CHANCE 0.5F /* a step's chance to shed a burst */
#define SPARK_NEAR 200.0F /* how near player 0 or 1 must be for a burst */
#define SPARK_SIZE 3.0F
#define SPARK_SIZE_RANGE 3.0F
/* the spread of the boulder's bursts along the floor frame's x axis */
#define SPARK_SCATTER 100.0F
/* a boulder without a timer sheds shards toward player 0 */
#define SHED_CHANCE 0.03F /* a step's chance to shed a shard */
#define SHED_LIFT 5.0F /* a shed shard's start above the boulder's pos */
#define DRAW_MARGIN 30.0F /* added to a shard's radius for its view test */
#define DRAW_REACH 800.0F
/* the draws in a row that have left a shard out */
#define CULLED_DRAWS(task) ((task)->unk_3c.l)
#define CULLED_DRAWS_MAX 10

NJS_TEXLIST *rockfall_texlists[5] = {
#include "stg30/rockfall/rockfall_texlists.inc"
};
KEPT static NJS_CNK_OBJECT *objects[ROCK_KINDS] = {
#include "stg30/rockfall/objects.inc"
};
KEPT static NJS_VECTOR sizes[ROCK_KINDS] = {
#include "stg30/rockfall/sizes.inc"
};
static Float drag[ROCK_KINDS] = {0.993F, 0.996F, 0.997F, 0.998F};
static Float gravity = -0.12F; /* what each step adds to velocity.y */
static Float dat_8c5f598c = 0.6F;
/* a shed shard flies toward player 0 at launch_speed and up to launch_range
 * more. its velocity adds up to launch_scatter / 2 either way along each axis,
 * and the boulder's velocity */
static Float launch_speed = 3.0F;
static Float launch_range = 6.0F;
static Float launch_scatter = 2.0F;
/* rockfall_attach gives a rock's shape the length of its kind's size as its
 * radius */
/* clang-format off */
static struct collision_shape shard_shape = {
    0, SHAPE_SPHERE, 0, 0, SHAPE_UNTURNED,
    {0.0F, 0.0F, 0.0F}, 50.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
static struct collision_shape boulder_shape = {
    0, SHAPE_BOX2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_UNTURNED | SHAPE_HURTABLE,
    {0.0F, 0.0F, 0.0F}, 500.0F, 1000.0F, 30.0F, 0, {0x3000, 0, 0},
};
/* clang-format on */
static Bool models_built = FALSE;
static struct model_build models[ROCK_KINDS] = {
    {MODEL_BUILD_ALONE, &boulder_texlist, &boulder_object, NULL},
    {MODEL_BUILD_ALONE, &shard_a_texlist, &shard_a_object, NULL},
    {MODEL_BUILD_ALONE, &shard_b_texlist, &shard_b_object, NULL},
    {MODEL_BUILD_ALONE, &shard_c_texlist, &shard_c_object, NULL},
};
static NJS_VECTOR probe_up = {0.0F, 1.0F, 0.0F};
int rockfall_count = 0;
static NJS_VECTOR player_up = {0.0F, 1.0F, 0.0F};

static void rockfall_step(struct task *task);
static void rockfall_destroy(struct task *task);
static void rockfall_draw(struct task *task);

Uint8 *rockfall_build(Uint8 *buffer)
{
    njFogDisable();
    if (model_builds_compile(&buffer, models, ROCK_KINDS) == ROCK_KINDS) {
        models_built = TRUE;
    }
    njFogEnable();
    return buffer;
}

static void find_floor(struct task *task)
{
    struct surface_side *hit;
    struct task_work *work;
    NJS_VECTOR normal;
    Float horizontal;

    work = task->work;
    hit = &ROCKFALL(task)->probe.sides[SURFACE_NEG_Y];
    /* @bug logic: the boulder's further searches every PROBE_PERIOD steps do
     * nothing, since find_floor returns once its probe holds a floor. a rock
     * keeps the floor and walls of its first search that found one. the second
     * test of hit->found below shows the search was meant to run with a floor
     * found. the return is at 0x8c503ef4, and the boulder's call at
     * 0x8c50443c */
    if (hit->found) {
        return;
    }

    /* @hack redundant: a second test of hit->found, which the return above
     * leaves FALSE */
    if (hit->found || ROCK_KIND(work) != BOULDER) {
        /* @hack spelling: the velocity times 1.0F, where the velocity alone
         * would do */
        ROCKFALL(task)->probe.pos.x =
            work->pos.x + (ROCKFALL(task)->velocity.x * 1.0F);
        ROCKFALL(task)->probe.pos.z =
            work->pos.z + (ROCKFALL(task)->velocity.z * 1.0F);
        ROCKFALL(task)->probe.pos.y = work->pos.y + PROBE_LIFT;
    } else {
        ROCKFALL(task)->probe.pos.x = player_work[0]->pos.x;
        ROCKFALL(task)->probe.pos.z = player_work[0]->pos.z;
        ROCKFALL(task)->probe.pos.y = player_work[0]->pos.y + PLAYER_PROBE_LIFT;
    }
    surface_probe_sides(&ROCKFALL(task)->probe);
    if (hit->found) {
        ROCKFALL(task)->floor.x = work->pos.x;
        ROCKFALL(task)->floor.z = work->pos.z;
        ROCKFALL(task)->floor.y = hit->pos;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, hit->ang_z);
        njRotateX(NULL, hit->ang_x);
        njCalcVector(NULL, &probe_up, &normal);
        horizontal = njSqrt((normal.x * normal.x) + (normal.z * normal.z));
        if (horizontal > HEADING_MIN) {
            ROCKFALL(task)->floor_angles.y =
                RADIANS(atan2f(normal.x, normal.z));
        } else {
            ROCKFALL(task)->floor_angles.y = 0;
        }
        ROCKFALL(task)->floor_angles.x = RADIANS(atan2f(
            njSqrt((normal.x * normal.x) + (normal.z * normal.z)), normal.y));
        ROCKFALL(task)->floor_angles.z = 0;
        njPopMatrixEx();
    }
}

Bool rockfall_attach(struct task *task)
{
    struct task_work *work;
    NJS_VECTOR normal;
    Float horizontal;
#ifdef NONMATCHING
    /* the record this side's attach below makes before it sets task->motion */
    struct rockfall *rock;
#endif

    work = task->work;
    /* @hack redundant: a block no path reaches. SHC still places its strings,
     * the SET editor's captions, in this source's constants */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- MODNUM(0-3)";
        (void)"<- STCOL SIZE";
        (void)"<- WAIT TIME";
    }
#ifdef NONMATCHING
    /* this side attaches only a rock it can finish, and otherwise kills the
     * task and returns FALSE. it fails without player 0's work or record, which
     * repairs the reads that the tag in rockfall_step names. it fails too for a
     * kind whose size has no positive x, as a build without the disc's data
     * has, and where syCalloc or collision_init fails */
    if (sizes[ROCK_KIND(work)].x <= 0.0F || player_work[0] == NULL ||
        player_records[0] == NULL) {
        goto failed;
    }
    rock = syCalloc(1, sizeof(*rock));
    if (rock == NULL) {
        goto failed;
    }
    if (!collision_init(
            task, ROCK_KIND(work) == BOULDER ? &boulder_shape : &shard_shape,
            ROCK_SHAPES, COLLISION_OBJECT)) {
        syFree(rock);
        goto failed;
    }
    task->motion.p = rock;
#else
    task->motion.p = syCalloc(1, sizeof(struct rockfall));
    if (task->motion.p == NULL) {
        return FALSE;
    }
    task->draw = rockfall_draw;
    task->step = rockfall_step;
    task->destructor = rockfall_destroy;
    rockfall_count++;
    collision_init(task,
                   ROCK_KIND(work) == BOULDER ? &boulder_shape : &shard_shape,
                   ROCK_SHAPES, COLLISION_OBJECT);
#endif
    if (work->collision != NULL) {
        work->collision->shapes->radius = njScalor(&sizes[ROCK_KIND(work)]);
    }
    collision_fit_radius(task);
    njUnitMatrix(&ROCKFALL(task)->rotation);
    njRotateXYZ(&ROCKFALL(task)->rotation, work->ang.x, work->ang.y,
                work->ang.z);
    ROCKFALL(task)->floor.x = player_work[0]->pos.x;
    ROCKFALL(task)->floor.z = player_work[0]->pos.z;
    ROCKFALL(task)->floor.y = player_records[0]->ground.below_y;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, player_records[0]->ground.below_ang_z);
    njRotateX(NULL, player_records[0]->ground.below_ang_x);
    njCalcVector(NULL, &player_up, &normal);
    horizontal = njSqrt((normal.x * normal.x) + (normal.z * normal.z));
    if (horizontal > HEADING_MIN) {
        ROCKFALL(task)->floor_angles.y = RADIANS(atan2f(normal.x, normal.z));
    } else {
        ROCKFALL(task)->floor_angles.y = 0;
    }
    ROCKFALL(task)->floor_angles.x = RADIANS(atan2f(
        njSqrt((normal.x * normal.x) + (normal.z * normal.z)), normal.y));
    ROCKFALL(task)->floor_angles.z = 0;
    njPopMatrixEx();
    find_floor(task);
    ROCKFALL(task)->size = sizes[ROCK_KIND(work)];
    ROCKFALL(task)->drag = drag[ROCK_KIND(work)];
#ifdef NONMATCHING
    /* this side sets the callbacks and counts the rock only once the attach
     * can't fail */
    task->draw = rockfall_draw;
    task->step = rockfall_step;
    task->destructor = rockfall_destroy;
    rockfall_count++;
#endif
    return TRUE;
#ifdef NONMATCHING
failed:
    /* this side kills the task, so that rockfall_init can't attach it at its
     * next step. a rock that the background or a boulder starts gets its
     * velocity only where the attach returns TRUE */
    task_kill(task);
    return FALSE;
#endif
}

void rockfall_init(struct task *task)
{
    rockfall_attach(task);
}

static void rockfall_destroy(struct task *task)
{
    /* @hack redundant: the read of task->work, which nothing uses */
    struct task_work *work = task->work;

    (void)work;
    syFree(task->motion.p);
    task->motion.p = NULL;
    rockfall_count--;
}

static void rockfall_step(struct task *task)
{
    struct task_work *work;
    struct surface_side *hit;
    struct task *shard;
#ifdef NONMATCHING
#else
    /* @hack spelling: axis, from and origin step through vectors' three floats,
     * where the fields by name would do */
    Float *axis;
    Float *from;
    Float *origin;
#endif
    union rock_spin_angle *spin;
    Float floor_clearance;
    Float margin;
    Float angular_speed;
    /* @hack spelling: unit, push and pop, locals for njUnitMatrix,
     * njPushMatrixEx and njPopMatrixEx, where the calls by name would do */
    void (*unit)(NJS_MATRIX *matrix);
    Bool (*push)(void);
    Bool (*pop)(void);
    Float speed_scale;
    Angle heading;
    Angle roll;
    NJS_POINT3 spark_pos;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    NJS_VECTOR rotated_size;
    NJS_VECTOR size;

    unit = njUnitMatrix;
    work = task->work;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it kills a rock
     * without player 0's work or record */
    if (player_work[0] == NULL || player_records[0] == NULL) {
        task_kill(task);
        return;
    }
#else
    /* @bug memory: a rock reads player_work[0] untested, though rockfall_step
     * tests it for null at 0x8c504c8a before it aims a shed shard. this read is
     * at 0x8c5043c6, and find_floor and rockfall_attach read it untested too */
#endif
    if (player_work[0]->pos.z < PASSED_Z) {
        task_kill(task);
        return;
    }
    /* @hack spelling: margin, a local for MARGIN, where MARGIN would do */
    margin = MARGIN;
    if (work->mode == ROCK_WAITING &&
        player_near(&work->pos, work->scl.y + margin)) {
        work->mode = ROCK_MOVING;
    }
    if (work->mode != ROCK_MOVING) {
        return;
    }
    if (!ROCKFALL(task)->probe.sides[SURFACE_NEG_Y].found ||
        (ROCK_KIND(work) == BOULDER && game_state.steps % PROBE_PERIOD == 0)) {
        find_floor(task);
    }
    if (ROCKFALL(task)->delay < (Sint32)work->scl.z) {
        ROCKFALL(task)->delay++;
        return;
    }
    if (player_near(&work->pos, njScalor(&ROCKFALL(task)->size) + BRAKE_NEAR)) {
        if (ROCK_KIND(work) == BOULDER) {
            ROCKFALL(task)->brake_steps =
                (Sint32)(rand() * (BOULDER_BRAKE_RANGE * RAND_SCALE)) +
                BOULDER_BRAKE_STEPS;
        } else {
            ROCKFALL(task)->brake_steps =
                (Sint32)(rand() * (SHARD_BRAKE_RANGE * RAND_SCALE));
        }
    }
    ROCKFALL(task)->velocity.y += gravity;

#ifdef NONMATCHING
    /* the plain form of the hack tagged at axis's declaration */
    ROCKFALL(task)->velocity.x *= ROCKFALL(task)->drag;
    ROCKFALL(task)->velocity.y *= ROCKFALL(task)->drag;
    ROCKFALL(task)->velocity.z *= ROCKFALL(task)->drag;
#else
    axis = &ROCKFALL(task)->velocity.x;
    *axis++ *= ROCKFALL(task)->drag;
    *axis++ *= ROCKFALL(task)->drag;
    *axis *= ROCKFALL(task)->drag;
#endif
    if (work->timer >= 1 && work->timer < FADE_STEPS) {
#ifdef NONMATCHING
        /* the plain form of the hack tagged at axis's declaration */
        ROCKFALL(task)->velocity.x *= FADE_DRAG;
        ROCKFALL(task)->velocity.y *= FADE_DRAG;
        ROCKFALL(task)->velocity.z *= FADE_DRAG;
#else
        axis = &ROCKFALL(task)->velocity.x;
        *axis++ *= FADE_DRAG;
        *axis++ *= FADE_DRAG;
        *axis *= FADE_DRAG;
#endif
    }
    if (ROCK_KIND(work) == BOULDER && collision_next_player(task) &&
        ROCKFALL(task)->velocity.z <= 0.0F) {
        flash_manager_create_sparks_depth(
            player_work[0]->pos.x, player_work[0]->pos.y, player_work[0]->pos.z,
            BUMP_FLASH_SCALE, BUMP_FLASH_SPARKS, BUMP_FLASH_DEPTH);
        ROCKFALL(task)->velocity.z =
            _builtin_fabsf(ROCKFALL(task)->velocity.z) * BUMP_REBOUND;
        ROCKFALL(task)->brake_steps = BUMP_BRAKE_STEPS;
    }
    if (ROCKFALL(task)->brake_steps) {
        ROCKFALL(task)->velocity.z *= BRAKE_DRAG;
        ROCKFALL(task)->brake_steps--;
    }
    push = njPushMatrixEx;
    push();
    njSetMatrix(NULL, &ROCKFALL(task)->rotation);
    njRotateEx(&ROCKFALL(task)->spin.angles.x, XYZ_TURN);
    njGetMatrix(&ROCKFALL(task)->rotation);
    pop = njPopMatrixEx;
    pop();

    /* @hack redundant: a block no path reaches, which nothing needs */
    if (0) {
        push();
        unit(NULL);
        unit(NULL);
        pop();
        models_built = TRUE;
        models_built = TRUE;
    }

    /* @hack spelling: each angle's low half through spin and angular_speed,
     * where each axis[i].low scaled in place would do */
    spin = &ROCKFALL(task)->spin.axis[0];
    angular_speed = spin->low;
    spin->low = angular_speed * SPIN_DRAG;
    spin = &ROCKFALL(task)->spin.axis[1];
    angular_speed = spin->low;
    spin->low = angular_speed * SPIN_DRAG;
    spin = &ROCKFALL(task)->spin.axis[2];
    angular_speed = spin->low;
    spin->low = angular_speed * SPIN_DRAG;

#ifdef NONMATCHING
    /* the plain form of the hack tagged at axis's declaration */
    work->pos.x += ROCKFALL(task)->velocity.x;
    work->pos.y += ROCKFALL(task)->velocity.y;
    work->pos.z += ROCKFALL(task)->velocity.z;
#else
    from = &work->pos.x;
    axis = &ROCKFALL(task)->velocity.x;
    *from += *axis;
    from++;
    axis++;
    *from += *axis;
    from++;
    axis++;
    *from += *axis;
#endif
    hit = &ROCKFALL(task)->probe.sides[SURFACE_NEG_X];
    if (work->pos.x < hit->pos && hit->found && hit->normal.x > WALL_NORMAL &&
        !(hit->flags & GROUND_DEFLECTS)) {
        work->pos.x = hit->pos;
        ROCKFALL(task)->velocity.x =
            _builtin_fabsf(ROCKFALL(task)->velocity.x) * WALL_REBOUND;
    }
    hit = &ROCKFALL(task)->probe.sides[SURFACE_POS_X];
    if (work->pos.x > hit->pos && hit->found && hit->normal.x < -WALL_NORMAL &&
        !(hit->flags & GROUND_DEFLECTS)) {
        work->pos.x = hit->pos;
        ROCKFALL(task)->velocity.x =
            -(_builtin_fabsf(ROCKFALL(task)->velocity.x) * WALL_REBOUND);
    }
    hit = &ROCKFALL(task)->probe.sides[SURFACE_NEG_Z];
    if (work->pos.z < hit->pos && hit->found && hit->normal.z > WALL_NORMAL &&
        !(hit->flags & GROUND_DEFLECTS)) {
        work->pos.z = hit->pos;
        ROCKFALL(task)->velocity.z =
            _builtin_fabsf(ROCKFALL(task)->velocity.z) * WALL_REBOUND;
    }
    hit = &ROCKFALL(task)->probe.sides[SURFACE_POS_Z];
    if (work->pos.z > hit->pos && hit->found && hit->normal.z < -WALL_NORMAL &&
        !(hit->flags & GROUND_DEFLECTS)) {
        work->pos.z = hit->pos;
        ROCKFALL(task)->velocity.z =
            -(_builtin_fabsf(ROCKFALL(task)->velocity.z) * WALL_REBOUND);
    }
    /* the rock's pos and velocity, and its size turned by rotation, in the
     * floor's frame */
    push();
    unit(NULL);
    njRotateX(NULL, -ROCKFALL(task)->floor_angles.x);
    njRotateY(NULL, -ROCKFALL(task)->floor_angles.y);
    njTranslate(NULL, -ROCKFALL(task)->floor.x, -ROCKFALL(task)->floor.y,
                -ROCKFALL(task)->floor.z);
    njCalcPoint(NULL, &work->pos, &pos);
    njCalcVector(NULL, &ROCKFALL(task)->velocity, &spd);
    njMultiMatrix(NULL, &ROCKFALL(task)->rotation);
    size = ROCKFALL(task)->size;
    size.y = 0.0F;
    njCalcVector(NULL, &size, &rotated_size);
    pop();
    floor_clearance = _builtin_fabsf(rotated_size.y) + margin;
    if (floor_clearance > pos.y) {
        heading = RADIANS(atan2f(ROCKFALL(task)->velocity.x,
                                 ROCKFALL(task)->velocity.z)) -
                  work->ang.y;
        roll =
            njSqrt((ROCKFALL(task)->velocity.x * ROCKFALL(task)->velocity.x) +
                   (ROCKFALL(task)->velocity.z * ROCKFALL(task)->velocity.z)) *
            ROLL_RATE;
        push();
        unit(NULL);
        njRotateY(NULL, heading);
        njRotateX(NULL, roll);
        njRotateY(NULL, -heading);
        current_matrix_angles_xyz(&ROCKFALL(task)->spin.angles.x,
                                  &ROCKFALL(task)->spin.angles.y,
                                  &ROCKFALL(task)->spin.angles.z);
        pop();
        /* @bug logic: a landing never bounces the rock. spd.y takes
         * floor_clearance less pos.y after pos.y takes floor_clearance, so it
         * is always 0, and only the rare kick lifts the rock. the FSUB at
         * 0x8c504988 subtracts the clearance from itself */
        pos.y = floor_clearance;
        spd.y = floor_clearance - pos.y;
        if (RANDOM_UNIT() < KICK_CHANCE) {
            spd.y += KICK_SPEED;
        }
        ROCKFALL(task)->sparks = LANDING_SPARKS;
    }
    push();
    unit(NULL);
    njTranslateEx(&ROCKFALL(task)->floor);
    njRotateY(NULL, ROCKFALL(task)->floor_angles.y);
    njRotateX(NULL, ROCKFALL(task)->floor_angles.x);
    njCalcPoint(NULL, &pos, &work->pos);
    pos.y -= floor_clearance;
    spark_pos = pos;
    if (ROCK_KIND(work) == BOULDER) {
        spark_pos.x += (RANDOM_UNIT() - 0.5F) * SPARK_SCATTER;
    }
    njCalcPoint(NULL, &spark_pos, &spark_pos);
    njCalcVector(NULL, &spd, &ROCKFALL(task)->velocity);
    pop();
    if (ROCKFALL(task)->sparks && RANDOM_UNIT() < SPARK_CHANCE &&
        player_near(&work->pos, SPARK_NEAR)) {
        burst_create_wobbling(&spark_pos, &ROCKFALL(task)->velocity,
                              (rand() * (SPARK_SIZE_RANGE * RAND_SCALE)) +
                                  SPARK_SIZE);
        ROCKFALL(task)->sparks--;
    }
    if (work->timer >= 1) {
        work->timer--;
        if (work->timer <= 0 || CULLED_DRAWS(task) > CULLED_DRAWS_MAX) {
            task_kill(task);
        } else {
            collision_register(task);
        }
    } else {
        collision_register(task);
        if (ROCK_KIND(work) == BOULDER && RANDOM_UNIT() < SHED_CHANCE) {
            shard = task_create(TASK_WORK, ROCK_LEVEL, rockfall_init,
                                "ObjectBurst");
            if (shard != NULL) {
                shard->work->pos = work->pos;
                shard->work->pos.y += SHED_LIFT;
                shard->work->scl = work->scl;
                shard->work->ang = work->ang;
                shard->work->scl.x =
                    ((Sint32)(rand() * (PICK_RANGE * RAND_SCALE)) %
                     SHARD_KINDS) +
                    FIRST_SHARD;
                shard->work->timer =
                    (Sint32)(rand() * (SHARD_LIFE_RANGE * RAND_SCALE)) +
                    SHARD_LIFE;
                if (rockfall_attach(shard)) {
                    ROCKFALL(shard)->velocity = ROCKFALL(task)->velocity;
                    if (player_work[0] != NULL) {
#ifdef NONMATCHING
                        /* the plain form of the hack tagged at axis's
                         * declaration */
                        pos.x = player_work[0]->pos.x - work->pos.x;
                        pos.y = player_work[0]->pos.y - work->pos.y;
                        pos.z = player_work[0]->pos.z - work->pos.z;
#else
                        axis = &pos.x;
                        from = &player_work[0]->pos.x;
                        origin = &work->pos.x;
                        *axis++ = *from++ - *origin++;
                        *axis++ = *from++ - *origin++;
                        *axis = *from - *origin;
#endif
                        speed_scale =
                            (launch_speed +
                             (launch_range * (Float)rand() * RAND_SCALE)) /
                            njScalor(&pos);

#ifdef NONMATCHING
                        /* the plain form of the hack tagged at axis's
                         * declaration */
                        pos.x *= speed_scale;
                        pos.y *= speed_scale;
                        pos.z *= speed_scale;
#else
                        axis = &pos.x;
                        *axis++ *= speed_scale;
                        *axis++ *= speed_scale;
                        *axis *= speed_scale;
#endif
                        pos.x += (RANDOM_UNIT() - 0.5F) * launch_scatter;
                        pos.y += (RANDOM_UNIT() - 0.5F) * launch_scatter;
                        pos.z += (RANDOM_UNIT() - 0.5F) * launch_scatter;

#ifdef NONMATCHING
                        /* the plain form of the hack tagged at axis's
                         * declaration */
                        ROCKFALL(shard)->velocity.x += pos.x;
                        ROCKFALL(shard)->velocity.y += pos.y;
                        ROCKFALL(shard)->velocity.z += pos.z;
#else
                        axis = &ROCKFALL(shard)->velocity.x;
                        from = &pos.x;
                        *axis++ += *from++;
                        *axis++ += *from++;
                        *axis += *from;
#endif
                        shard->work->mode = ROCK_MOVING;
                    }
                }
            }
        }
    }
}

static void rockfall_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_MODEL *model;
    int kind;
    NJS_POINT3 preview;

    if (work->timer >= 1 && work->timer < FADE_STEPS &&
        (game_state.steps & BLINK_BIT)) {
        return;
    }

    /* @hack redundant: a block no path reaches, and its preview, which nothing
     * needs */
    if (0) {
        preview = work->pos;
        njCalcPoint(NULL, &work->pos, &preview);
    }
    kind = ROCK_KIND(work);
    model = objects[kind]->model;
    /* @hack spelling: ROCK_KIND(work) taken again, where kind would do */
    if (ROCK_KIND(work) == BOULDER ||
        sphere_in_front_within(&work->pos, model->r + DRAW_MARGIN,
                               DRAW_REACH)) {
        CULLED_DRAWS(task) = 0;
    } else {
        CULLED_DRAWS(task)++;
        return;
    }
    njSetTexture(rockfall_texlists[kind]);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njMultiMatrix(NULL, &ROCKFALL(task)->rotation);
    njTranslate(NULL, -model->center.x, -model->center.y, -model->center.z);
    if (models_built) {
        njDirectDrawObject(models[kind].built);
    } else {
        njFogDisable();
        njCnkEasyDrawModel(model);
        njFogEnable();
    }
    njPopMatrixEx();
}
