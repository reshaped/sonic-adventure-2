/* the mech bosses' laser beam: pulses that run from the laser's origin to its
 * spot on the ground. Big Foot, Hot Shot and Flying Dog fire it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "object/mover.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/boss_gunfire.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_laser_beam.h"
#include "shared/mech_boss_sounds.h"
#include "shared/spark.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* how a pulse draws */
enum laser_pulse_look {
    LOOK_LINE, /* a line from the pulse's tail to its tip */
    LOOK_SPRITES, /* three sprites along the pulse, and LOOK_MODEL's model */
    LOOK_MODEL /* laser_pulse_object, from the tail toward the target */
};

/* how the beam fires its pulses, and how they move and draw */
struct laser_beam_params {
    Sint8 look; /* an enum laser_pulse_look */
    Uint8 interval; /* the beam fires a pulse every interval + 1 steps */
    /* the share of the line from the origin to the target that a pulse's
     * tail moves each step */
    Float speed;
    Float length; /* the share of that line from a pulse's tail to its tip */
    Float rise; /* how far each pulse shifts the origin along y a step */
    Uint32 unk_10;
    Angle spread; /* the turn about y between successive pulses' shifts */
    Angle turn; /* what the beam adds to turn_at each step */
};

/* a pulse's work, laid over the struct task_work that task_create_child
 * allocates */
struct laser_pulse_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    Uint8 steps; /* the steps the pulse has run */
    Sint16 flags;
    Uint16 timer;
    NJS_POINT3 *from; /* the beam's origin */
    NJS_POINT3 *to; /* the beam's target */
    Angle ang; /* the turn about y of the pulse's shifts of from and to */
    NJS_POINT3 pos; /* the pulse's tail */
};

/* a pulse's TASK_MOTION record, laid over struct task_motion */
struct laser_pulse_motion {
    NJS_VECTOR spd; /* what the tail moves in a step */
    NJS_POINT3 tip;
};

/* the beam's work, laid over the struct task_work that task_create_child
 * allocates */
struct laser_beam_work {
    Sint8 next; /* the pulse to fire next, from 0 to PULSES - 1 */
    Sint8 submode;
    Uint8 id;
    Uint8 interval;
    Sint16 flags;
    Uint16 timer; /* the steps since the beam last fired */
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    /* the address, as an integer, of the angle that turns the fire's aim
     * about y. create points it at the parent's work's ang.y */
    Sint32 turn_at;
};

/* the beam's TASK_MOTION record, laid over struct task_motion */
struct laser_beam_motion {
    Uint8 unk_00[0x18];
    struct mover *mover;
};

#define PULSE(task) ((struct laser_pulse_work *)(task)->work)
#define BEAM(task) ((struct laser_beam_work *)(task)->work)

#define PULSES 3
/* the ground's height. a pulse puts the target on it, and ends once its tail
 * passes below it */
#define GROUND_Y 0.0F
/* each step, a pulse moves the target TARGET_SHIFT further than the origin */
#define TARGET_SHIFT 2.0F
#define SPRITES 3
#define SPRITE_WIDTH 0.05F
#define SPRITE_HEIGHT 0.1F
#define MODEL_SCALE_Y 0.8F
/* a random Float, at least 0 and below range */
#define ROLL(range) (rand() * ((range) * RAND_SCALE))
/* the beam throws sparks with a pulse at odds of SPARK_UNDER in SPARK_ROLL. it
 * makes a fire at odds of FIRE_UNDER in FIRE_ROLL */
#define SPARK_ROLL 10.0F
#define SPARK_UNDER 2.0F
#define FIRE_ROLL 3.0F
#define FIRE_UNDER 1.0F
#define SPARKS 6
#define SPARK_PART 0.5F
#define SPARK_DROP 0.2F
/* the fire stands at (FIRE_X, FIRE_Y, 0) from the origin, unturned */
#define FIRE_X (-2.0F)
#define FIRE_Y (-0.1F)
#define FIRE_VOLUME 50
#define BEAM_SOUND_STEPS 5

static struct laser_beam_params laser_beam_params = {
    LOOK_MODEL, 2, 0.07F, 0.4F, -0.2F, 0, THIRD_TURN, 0x1000,
};
/* how far each pulse shifts the origin a step: along x, turned about y by the
 * pulse's ang */
static Float laser_pulse_shift = 2.0F;
static NJS_TEXANIM laser_pulse_cell = {
#include "shared/mech_boss_laser_beam/laser_pulse_cell.inc"
};
static NJS_SPRITE laser_pulse_sprite = {
#include "shared/mech_boss_laser_beam/laser_pulse_sprite.inc"
};

static void laser_pulse_destroy(struct task *task);
static void laser_pulse_step(struct task *task);
static void laser_pulse_draw(struct task *task);
static void laser_beam_destroy(struct task *task);
static void laser_beam_step(struct task *task);

static struct task *laser_pulse_create(struct task *parent, NJS_POINT3 *from,
                                       NJS_POINT3 *to, Angle ang)
{
    struct task *task;
    struct laser_pulse_work *work;

    if (parent != NULL) {
        task = task_create_child(TASK_MOTION | TASK_WORK, laser_pulse_step,
                                 parent);
        if (task == NULL) {
            return NULL;
        }
        work = PULSE(task);
        work->from = from;
        work->to = to;
        work->pos = *from;
        work->ang = ang;
        task->draw = laser_pulse_draw;
        task->destructor = laser_pulse_destroy;
        return task;
    }
#ifdef NONMATCHING
    /* GCC can't see that no path gets here: laser_beam_step, the one caller,
     * passes its own task */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void laser_pulse_destroy(struct task *task)
{
}

static void laser_pulse_step(struct task *task)
{
    struct laser_pulse_work *work = PULSE(task);
    struct laser_pulse_motion *motion = task->motion.p;

    /* @hack spelling: the step's body under a test of both flags, where a
     * return on either would do */
    if (!game_state.time_stopped && !game_state.unk_09) {
        NJS_POINT3 *to = work->to;
        NJS_POINT3 *from = work->from;
        NJS_VECTOR line;
        NJS_VECTOR zero = {0.0F, 0.0F, 0.0F};
        NJS_POINT3 shift;

        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, work->ang);
        njTranslate(NULL, laser_pulse_shift, laser_beam_params.rise, 0.0F);
        njCalcPoint(NULL, &zero, &shift);
        njPopMatrix(1);
        from->x += shift.x;
        from->y += shift.y;
        from->z += shift.z;
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, work->ang);
        njTranslate(NULL, laser_pulse_shift + TARGET_SHIFT, 0.0F, 0.0F);
        njCalcPoint(NULL, &zero, &shift);
        njPopMatrix(1);
        to->x += shift.x;
        to->y = GROUND_Y;
        to->z += shift.z;
        line.x = to->x - from->x;
        line.y = to->y - from->y;
        line.z = to->z - from->z;
        motion->spd.x = line.x * laser_beam_params.speed;
        motion->spd.y = line.y * laser_beam_params.speed;
        motion->spd.z = line.z * laser_beam_params.speed;
        work->pos.x = from->x + (motion->spd.x * work->steps);
        work->pos.y = from->y + (motion->spd.y * work->steps);
        work->pos.z = from->z + (motion->spd.z * work->steps);
        motion->tip.x = work->pos.x + (line.x * laser_beam_params.length);
        motion->tip.y = work->pos.y + (line.y * laser_beam_params.length);
        motion->tip.z = work->pos.z + (line.z * laser_beam_params.length);
        if (motion->tip.y < GROUND_Y) {
            motion->tip = *to;
            motion->tip.y = GROUND_Y;
        }
        work->steps++;
        if (work->pos.y < GROUND_Y) {
            task_kill(task);
        }
    }
}

static void laser_pulse_draw(struct task *task)
{
    struct laser_pulse_work *work = PULSE(task);
    struct laser_pulse_motion *motion = task->motion.p;
    /* @hack redundant: tail, a pointer nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    NJS_POINT3 *tail = &work->pos;
    NJS_SPRITE *sprite;
    Float alpha;
    Sint8 i;

    switch (laser_beam_params.look) {
    /* @hack spelling: points and colors declared in a block of their own,
     * where the function's top would do */
    case LOOK_LINE: {
        NJS_POINT3 points[2];
        /* SHC refuses braces around each color, with error 2129 */
        /* NOLINTNEXTLINE(clang-diagnostic-missing-braces) */
        NJS_COLOR colors[2] = {0xffffff80, 0xffffff80};

        points[0] = work->pos;
        points[1] = motion->tip;
        line_queue(points, colors, LINE_TRANSLUCENT);
        break;
    }
    case LOOK_SPRITES:
        alpha = 1.0F;
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        constant_material_set(alpha, alpha, alpha, alpha);
        njSetTexture(&jet1_texlist);
        sprite = &laser_pulse_sprite;
        sprite->sx = SPRITE_WIDTH;
        sprite->sy = SPRITE_HEIGHT;
        for (i = 0; i < SPRITES; i++) {
            Float along;

            along = laser_beam_params.length * i;
            sprite->p = work->pos;
            sprite->p.x += motion->spd.x * along;
            sprite->p.y += motion->spd.y * along;
            sprite->p.z += motion->spd.z * along;
            njDrawSprite3D(sprite, 0,
                           NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE |
                               NJD_SPRITE_COLOR);
        }
        alpha = 0.0F;
        constant_material_set(alpha, alpha, alpha, alpha);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
        /* fall through */
    /* @hack spelling: to, from, line, ang, x, z and across declared in a
     * block of their own, where the function's top would do */
    case LOOK_MODEL: {
        NJS_POINT3 *to = work->to;
        NJS_POINT3 *from = work->from;
        NJS_VECTOR line;
        struct angles ang;
        /* @hack spelling: x and z, copies of line.x and line.z, where line's
         * read in place would do */
        Float x;
        Float z;
        Float across;

        line.x = to->x - from->x;
        line.y = to->y - from->y;
        line.z = to->z - from->z;
        x = line.x;
        z = line.z;
        across = njSqrt((x * x) + (z * z));
        ang.y = atan2f(line.x, line.z) * ANGLE_PER_RADIAN;
        ang.x = atan2f(line.y, across) * ANGLE_PER_RADIAN;
        njPushMatrixEx();
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, ang.y + QUARTER_TURN);
        njRotateZ(NULL, -ang.x);
        njScale(NULL, 1.0F, MODEL_SCALE_Y, 1.0F);
        njCnkEasyDrawObject(&laser_pulse_object);
        njPopMatrixEx();
        break;
    }
    }
}

struct task *mech_boss_laser_beam_create(struct task *parent,
                                         struct mech_boss_laser_beam_spec *spec)
{
    struct task *task;
    struct laser_beam_work *work;
    struct laser_beam_motion *motion;

    if (parent != NULL) {
        task =
            task_create_child(TASK_MOTION | TASK_WORK, laser_beam_step, parent);
        work = BEAM(task);
        motion = task->motion.p;
        work->interval = laser_beam_params.interval;
        work->from = spec->origin;
        work->to = spec->target;
        work->turn_at = (Sint32)&parent->work->ang.y;
        motion->mover = spec->mover;
        task->destructor = laser_beam_destroy;
        return task;
    }
#ifdef NONMATCHING
    /* for a null parent the image returns R0 as the caller left it, which C
     * can't give. this side returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void laser_beam_destroy(struct task *task)
{
}

static void laser_beam_step(struct task *task)
{
    struct laser_beam_work *work = BEAM(task);
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    /* @hack redundant: a local nothing reads, which sizes the frame */
    NJS_VECTOR unused[2]; /* NOLINT(clang-diagnostic-unused-variable) */

    if (game_state.time_stopped) {
        return;
    }
    if (work->timer++ == work->interval) {
        to = work->to;
        from = work->from;
        work->timer = 0;
        laser_pulse_create(task, from, to,
                           laser_beam_params.spread * work->next);
        /* @hack spelling: dir, spd and the unused vectors declared in a
         * block of their own, where the function's top would do */
        {
            /* @hack redundant: two vectors nothing reads, each initialized */
            /* NOLINTBEGIN(clang-diagnostic-unused-variable) */
            NJS_VECTOR unused_a = {-4.0F, 0.0F, 0.0F};
            NJS_VECTOR unused_b = {-1.0F, 0.0F, 0.0F};
            /* NOLINTEND(clang-diagnostic-unused-variable) */
            NJS_VECTOR dir;
            NJS_VECTOR spd;
            /* @hack redundant: a local nothing reads, which sizes the frame */
            /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
            NJS_VECTOR unused_c[2];

            dir.x = to->x - from->x;
            dir.y = to->y - from->y;
            dir.z = to->z - from->z;
            njUnitVector(&dir);
            if (ROLL(SPARK_ROLL) < SPARK_UNDER) {
                /* @hack spelling: motion, a local for the record, where its
                 * mover read through task->motion in place would do */
                struct laser_beam_motion *motion = task->motion.p;
                struct mover *mover = motion->mover;

                spd = dir;
                spd.x *= SPARK_PART;
                spd.y -= SPARK_DROP;
                /* @hack spelling: spd.z multiplied by 0, where spd.z = 0.0F
                 * would do */
                spd.z *= 0.0F;
                spd.x += mover->spd.x;
                spd.z += mover->spd.z;
                spark_throw(SPARKS, from, &spd);
            }
            if (ROLL(FIRE_ROLL) < FIRE_UNDER) {
                NJS_POINT3 zero = {0.0F, 0.0F, 0.0F};
                NJS_POINT3 offset;
                NJS_VECTOR front = {0.0F, 0.0F, -1.0F};
                Angle *ang;

                ang = (Angle *)work->turn_at;
                njPushMatrix(&_nj_unit_matrix_);
                njTranslate(NULL, FIRE_X, FIRE_Y, 0.0F);
                njRotateY(NULL, *ang);
                njCalcVector(NULL, &front, &dir);
                njCalcPoint(NULL, &zero, &offset);
                njPopMatrix(1);
                /* @hack spelling: from read again from the work before each
                 * call, where from would do */
                from = work->from;
                boss_gunfire_create(from, &dir, &offset);
                from = work->from;
                sound_play_at(MECH_BOSS_SOUND_LASER_FIRE, NULL, 0, FIRE_VOLUME,
                              from);
            }
        }
        if (work->next++ == PULSES - 1) {
            work->next = 0;
        }
    }
    /* @bug memory: turns the address in turn_at, not the angle it points to.
     * so the fire aims by an angle read a further 0x1000 bytes past the
     * parent's work at each step (0x7a6 and 0x73e of the object's code) */
#ifdef NONMATCHING
    /* this side repairs the tag above: turn_at stays where
     * mech_boss_laser_beam_create points it, so the fire aims by the parent's
     * ang.y */
#else
    work->turn_at += laser_beam_params.turn;
#endif
    to = work->to;
    sound_play_repeating_at(MECH_BOSS_SOUND_LASER_BEAM, task,
                            MECH_BOSS_SOUND_PRIORITY, 0, BEAM_SOUND_STEPS, to);
    sound_pan_by_pos(MECH_BOSS_SOUND_LASER_BEAM, task, NULL);
}
