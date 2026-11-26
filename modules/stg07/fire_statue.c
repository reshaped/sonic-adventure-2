/* the fire statue, FIRZIZ and firziz in Aquatic Mine's object list. it breathes
 * puffs of fire between rests. a hit pushes a part of it out, and the fire
 * stops until the part slides back in.
 *
 * it behaves as stg05/fire_statue.c's statue does, but for three changes. it
 * sounds as it fires. it puffs only above the water. after a hit, it fires
 * again only with its mouth above the water */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "sound/sound.h"
#include "stg07/fire_statue.h"
#include "stg07/water.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/fire_statue/text.inc"

#include "shared/fire_statue_art.h"

/* the statue's record, in task->motion */
struct fire_statue {
    Angle mouth; /* the mouth's turn about x, from 0, shut, to MOUTH_OPEN */
    Float push; /* how far the part stands out, at most PUSH_OUT */
    Sint32 wait; /* the steps left before the part starts to slide back */
};

#define STATUE(task) ((struct fire_statue *)(task)->motion.p)

/* the statue's phases, in its work's mode. MODE_START rests as MODE_REST does,
 * from the work's timer of 0 */
enum fire_statue_mode { MODE_START, MODE_FIRE, MODE_REST };

/* in ang.x: the statue never fires, and its part stays out */
#define NO_FIRE 0x1
/* ang.z's low byte: a fire puffs once every INTERVAL(work) + 1 steps */
#define INTERVAL(work) ((Uint8)(work)->ang.z)
/* a phase's timer starts at scl.y + PHASE_STEPS and counts down to 0. a fire's
 * counts once the mouth is open and settled. a rest's counts once the mouth is
 * shut and the part is in. the phase ends at the step that finds it 0 */
#define PHASE_STEPS 120
/* a hit pushes the part PUSH_OUT out. it stays out WAIT_STEPS steps, then
 * slides back by PUSH_BACK a step */
#define PUSH_OUT 3.0F
#define WAIT_STEPS 300
#define PUSH_BACK 0.3F
/* the part draws turned PART_TILT degrees about x, and slides out along its
 * turned z */
#define PART_TILT (-25)
/* the mouth opens MOUTH_OPEN_STEP a step and shuts MOUTH_CLOSE_STEP a step.
 * each fire but the first starts SETTLE_STEPS steps after the mouth is open */
#define MOUTH_OPEN NJM_DEG_ANG(35)
#define MOUTH_OPEN_STEP NJM_DEG_ANG(0.8)
#define MOUTH_CLOSE_STEP NJM_DEG_ANG(1)
#define SETTLE_STEPS 30
/* the statue's shapes. the step moves the part's shape with the part */
#define STATUE_SHAPES 2
#define PART_SHAPE 1
/* the share of its speed a puff keeps each step */
#define PUFF_DRAG 0.96F
/* a puff's frame picks its texture of fire_texlist, whose PUFF_FRAMES
 * textures it shows in turn. its first frame lies below PUFF_FRAME_RANGE. the
 * puff ends once its frame reaches PUFF_FRAMES, or once its shape touches
 * anything */
#define PUFF_FRAMES 10.0F
#define PUFF_FRAME_RANGE 0.5F
/* a puff's quad starts turned up to half PUFF_TURN_RANGE either way. its
 * edge_angle is a quarter turn, give or take half PUFF_EDGE_RANGE */
#define PUFF_TURN_RANGE (FULL_TURN / 3.0F)
#define PUFF_EDGE_RANGE (FULL_TURN / 36.0F)
/* a puff is opaque white but for its blue byte, which makes it a yellow white.
 * the byte runs from PUFF_BLUE up to PUFF_BLUE_RANGE more, and stops at
 * PUFF_BLUE_MOST */
#define PUFF_COLOR 0xffffff00
#define PUFF_BLUE 160
#define PUFF_BLUE_RANGE 80.0F
#define PUFF_BLUE_MOST 255
#define PUFF_LEVEL 3
#define PUFF_SHAPES 1
/* a puff starts only where the mouth lies more than WATER_CLEARANCE above the
 * water. after a hit, the fire starts again only where the mouth clears the
 * water by as much. for that test the mouth lies MOUTH_HEIGHT above the
 * statue's pos */
#define WATER_CLEARANCE 1.0F
#define MOUTH_HEIGHT 10.0F
/* each step of a fire, once the mouth is open and settled, plays FIRE_SOUND at
 * the statue. the sound is of the stage's list, and the work is its key. each
 * call gives the sound FIRE_SOUND_UPDATES anew, in sound updates */
#define FIRE_SOUND 4
#define FIRE_SOUND_PRIORITY 1
#define FIRE_SOUND_VOLUME 10
#define FIRE_SOUND_UPDATES 30

static void fire_statue_step(struct task *task);
static void fire_statue_destroy(struct task *task);
static void fire_statue_puff(struct task *task);
static void fire_statue_draw(struct task *task);
static struct task *fire_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float size);
static void fire_step(struct task *task);
static void fire_destroy(struct task *task);
static void fire_draw(struct task *task);

/* what a puff's frame and its size gain each step */
static Float fire_frame_step = 0.5F;
static Float fire_growth = 0.22F;
NJS_TEXLIST *fire_statue_texlists[3] = {
#include "stg07/fire_statue/fire_statue_texlists.inc"
};
/* the body, and the part, which hurts players and which an attack can hurt */
/* clang-format off */
static struct collision_shape fire_statue_shapes[STATUE_SHAPES] = {
    {0, SHAPE_CAPSULE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 9.8F, 0.0F}, 5.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 8.0F, -4.5F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a puff leaves from fire_mouth at fire_speed a step, both in the statue's
 * axes. scl.x + 1 scales the speed */
static NJS_POINT3 fire_mouth = {0.0F, 9.0F, 6.0F};
static NJS_VECTOR fire_speed = {0.0F, 0.0F, 1.5F};
/* a puff's first size, half its width: fire_size_least plus up to
 * fire_size_range */
static Float fire_size_least = 2.2F;
static Float fire_size_range = 0.8F;
/* a puff's speed gains up to half fire_spread either way along each axis as it
 * leaves. the puff spins up to half fire_spin degrees a step either way. its
 * upward speed gains fire_rise a step */
static Float fire_spread = 0.1F;
static Float fire_spin = 3.0F;
static Float fire_rise = 0.05F;
/* clang-format off */
static struct collision_shape fire_shapes[PUFF_SHAPES] = {
    {0, SHAPE_SPHERE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

void fire_statue_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct fire_statue));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = fire_statue_draw;
    task->step = fire_statue_step;
    task->destructor = fire_statue_destroy;
    work->mode = MODE_START;
    collision_init(task, fire_statue_shapes, STATUE_SHAPES, COLLISION_OBJECT);
    work->tick = 0;
}

static void fire_statue_destroy(struct task *task)
{
    /* @hack spelling: statue, a copy of task, where task would do */
    struct task *statue = task;

    syFree(statue->motion.p);
    statue->motion.p = NULL;
}

static void fire_statue_puff(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &fire_mouth, &pos);
    njCalcVector(NULL, &fire_speed, &spd);
    njPopMatrix(1);
    spd.x = (spd.x * (work->scl.x + 1.0F)) +
            (fire_spread * ((rand() * RAND_SCALE) - 0.5F));
    spd.y = (spd.y * (work->scl.x + 1.0F)) +
            (fire_spread * ((rand() * RAND_SCALE) - 0.5F));
    spd.z = (spd.z * (work->scl.x + 1.0F)) +
            (fire_spread * ((rand() * RAND_SCALE) - 0.5F));
    if (pos.y > water_height(&pos) + WATER_CLEARANCE) {
        fire_create(&pos, &spd,
                    (fire_size_range * rand() * RAND_SCALE) + fire_size_least);
    }
}

static void fire_statue_step(struct task *task)
{
    struct task_work *work = task->work;
    Float *part;

    if (placement_try_leave(task)) {
        return;
    }
    if ((work->ang.x & NO_FIRE) || (work->flags & WORK_HURT)) {
        STATUE(task)->push = PUSH_OUT;
        work->mode = MODE_REST;
        work->timer = (Sint32)work->scl.y + PHASE_STEPS;
        STATUE(task)->wait = WAIT_STEPS;
    } else if (STATUE(task)->wait) {
        STATUE(task)->wait--;
    } else if (STATUE(task)->push > 0.0F) {
        STATUE(task)->push -= PUSH_BACK;
        /* @bug logic: where the mouth isn't clear of the water as the part
         * slides in, push stays below 0. only the start of a fire sets it
         * back to 0 (0x8c500980). a rest's timer counts only while push is 0
         * (0x8c500a80). so the statue never fires again until a hit pushes
         * the part out */
        if (STATUE(task)->push <= 0.0F &&
            water_height(&work->pos) + WATER_CLEARANCE <
                work->pos.y + MOUTH_HEIGHT) {
            STATUE(task)->push = 0.0F;
            work->mode = MODE_FIRE;
            work->timer = (Sint32)work->scl.y + PHASE_STEPS;
            work->tick = SETTLE_STEPS;
        }
    }
    part = fire_statue_object->child->pos;
    work->collision->shapes[PART_SHAPE].center.x = part[0];
    work->collision->shapes[PART_SHAPE].center.y = part[1];
    work->collision->shapes[PART_SHAPE].center.z = part[2] + STATUE(task)->push;
    collision_register(task);
    if (work->mode == MODE_FIRE) {
        if (STATUE(task)->mouth < MOUTH_OPEN) {
            STATUE(task)->mouth += MOUTH_OPEN_STEP;
            if (STATUE(task)->mouth > MOUTH_OPEN) {
                STATUE(task)->mouth = MOUTH_OPEN;
            }
        } else if (work->tick) {
            work->tick--;
        } else {
            if (game_state.steps % (INTERVAL(work) + 1) == 0) {
                fire_statue_puff(task);
            }
            sound_play_at_for(FIRE_SOUND, work, FIRE_SOUND_PRIORITY,
                              FIRE_SOUND_VOLUME, FIRE_SOUND_UPDATES,
                              &work->pos);
            if (work->timer) {
                work->timer--;
            } else {
                work->mode = MODE_REST;
                /* @bug logic: the rest takes its length from scl.y, as the
                 * fire does, where the set editor's labels give it scl.z,
                 * FIRE OFF FRAME(+120). the step reads scl.y at 0x8c500a44.
                 * the labels are the claim's only evidence, and no code reads
                 * scl.z */
                work->timer = (Sint32)work->scl.y + PHASE_STEPS;
                work->tick = SETTLE_STEPS;
            }
        }
    } else if (STATUE(task)->mouth > 0) {
        STATUE(task)->mouth -= MOUTH_CLOSE_STEP;
        if (STATUE(task)->mouth < 0) {
            STATUE(task)->mouth = 0;
        }
    } else if (STATUE(task)->push == 0.0F) {
        if (work->timer) {
            work->timer--;
        } else {
            work->timer = (Sint32)work->scl.y + PHASE_STEPS;
            work->mode = MODE_FIRE;
        }
    }
    /* @hack redundant: a test of NO_FIRE that no path reaches */
    if (0) {
        if (work->ang.x & NO_FIRE) {
        }
    }
}

static void fire_statue_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODE(NOFIRE)";
        (void)"<- MODE(NORMAL)";
        (void)"<- ANGLE Y";
        (void)"<- INTERVAL";
        (void)"<- FIRE SPEED(+1)";
        (void)"<- FIRE ON FRAME(+120)";
        (void)"<- FIRE OFF FRAME(+120)";
    }
    njSetTexture(&fire_statue_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    part = fire_statue_object;
    njCnkEasyDrawModel(part->model);
    part = part->child;
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)part->pos);
    njRotateX(NULL, NJM_DEG_ANG(PART_TILT));
    njTranslate(NULL, 0.0F, 0.0F, STATUE(task)->push);
    njCnkEasyDrawModel(part->model);
    njPopMatrix(1);
    part = part->sibling;
    njTranslateV(NULL, (NJS_VECTOR *)part->pos);
    njRotateX(NULL, STATUE(task)->mouth);
    njCnkEasyDrawModel(part->model);
    njPopMatrix(1);
}

/* a puff's task keeps three values in pointer fields, which point at nothing.
 * they are its size, its frame and its color. the size is half the quad's width
 */
/* @hack type: the size read as a Float through motion's address, where
 * (task)->motion.f would do */
#define SIZE(task) (*(Float *)&(task)->motion)
/* @hack type: the frame read as a Float through force's address, where
 * (task)->force.f would do */
#define FRAME(task) (*(Float *)&(task)->force)
#define COLOR(task) ((Uint32)(task)->extra.l)
/* a puff's work: scl is its speed a step, and ang.x its spin a step. ang.z is
 * its quad's turn, and ang.y its quad's edge_angle */

static struct task *fire_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float size)
{
    struct task *task;
    struct task_work *work;
    Uint32 blue;

    task = task_create(TASK_WORK, PUFF_LEVEL, fire_step, FIRE_STATUE_TASK_NAME);
    if (task == NULL) {
        return task;
    }
    work = task->work;
    SIZE(task) = size;
    work->scl = *spd;
    work->ang.z = ((rand() * RAND_SCALE) - 0.5F) * PUFF_TURN_RANGE;
    work->ang.x =
        (Angle)(((rand() * RAND_SCALE) - 0.5F) * fire_spin * ANGLE_PER_DEGREE);
    work->pos = *pos;
    work->ang.y = (Angle)(((rand() * RAND_SCALE) - 0.5F) * PUFF_EDGE_RANGE) +
                  QUARTER_TURN;
    task->destructor = fire_destroy;
    task->step = fire_step;
    task->draw = fire_draw;
    FRAME(task) = rand() * (PUFF_FRAME_RANGE * RAND_SCALE);
    blue = (Uint32)(rand() * (PUFF_BLUE_RANGE * RAND_SCALE)) + PUFF_BLUE;
    if (blue > PUFF_BLUE_MOST) {
        blue = PUFF_BLUE_MOST;
    }
    task->extra.l = (blue | PUFF_COLOR);
    collision_init(task, fire_shapes, PUFF_SHAPES, COLLISION_OBJECT);
    return task;
}

static void fire_step(struct task *task)
{
    struct task_work *work = task->work;

    work->pos.x += work->scl.x;
    work->pos.y += work->scl.y;
    work->pos.z += work->scl.z;
    work->ang.z += work->ang.x;
    SIZE(task) += fire_growth;
    work->scl.x *= PUFF_DRAG;
    work->scl.y = (work->scl.y * PUFF_DRAG) + fire_rise;
    work->scl.z *= PUFF_DRAG;
    FRAME(task) += fire_frame_step;
    if (FRAME(task) >= PUFF_FRAMES) {
        task_destroy(task);
    } else if (work->collision != NULL) {
        if (work->collision->flags & COLLISION_TOUCHED) {
            task_destroy(task);
        } else {
            collision_register(task);
        }
    }
}

/* task_destroy frees task->motion, task->force and task->extra once the
 * destructor returns. a puff's three hold values, not blocks of the heap, so
 * the destructor clears them */
static void fire_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
    task->extra.p = NULL;
}

static void fire_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&fire_texlist);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njFogDisable();
    particle_draw_quad(&work->pos, FRAME(task), work->ang.y, work->ang.z,
                       SIZE(task), SIZE(task), COLOR(task), 0, 0.0F, 0.0F);
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}
