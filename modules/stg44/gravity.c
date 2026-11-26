/* the gravity field, GRAV_SW in Mad Space's object list: a sphere or a
 * capsule. within its reach, it adds a way down to field_sample_at's sample */
#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/field.h"
#include "sound/sound.h"
#include "stg44/gravity.h"
#include "system/task.h"
#include "system/util.h"

/* the field's shapes. a GRAVITY_DOWN sphere sets a way down of -y over any
 * other field's */
enum gravity_shape {
    GRAVITY_SPHERE,
    GRAVITY_CAPSULE,
    GRAVITY_DOWN,
    GRAVITY_SHAPES
};
/* the bits of ang.x that, taken modulo GRAVITY_SHAPES, pick the shape */
#define SHAPE_MASK 0xf
#define GRAVITY_SHAPE(work) (((work)->ang.x & SHAPE_MASK) % GRAVITY_SHAPES)
/* a field reaches GRAVITY_MARGIN past its radius, scl.x */
#define GRAVITY_MARGIN 10.0F
/* while player 0 lies within a capsule's cylinder, the capsule plays
 * SURFACE_SOUND each step. the sound's point is the point of the cylinder's
 * side nearest player 0. the capsule plays AMBIENT_SOUND too, at a chance of
 * AMBIENT_CHANCE a step. the cylinder has a radius of scl.x, and runs scl.y +
 * scl.x each way along the segment */
#define SURFACE_SOUND 10
#define SURFACE_STEPS 30
#define AMBIENT_SOUND 11
#define AMBIENT_STEPS 60
#define AMBIENT_CHANCE 0.005F
/* AMBIENT_SOUND's point lies up to scl.y + scl.x * AMBIENT_OVERHANG either way
 * along the segment, at random */
#define AMBIENT_OVERHANG 0.5F
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0

static void gravity_sample(struct task *task, NJS_POINT3 *pos,
                           struct field_sample *sample)
{
    struct task_work *work = task->work;
    NJS_VECTOR delta;
    Float distance;

    /* @hack spelling: the difference as a walk of three Floats in a block of
     * its own, where three statements on the fields would do */
    {
        Float *target = &work->pos.x;
        Float *out = &delta.x;
        Float *origin = &pos->x;
        *out++ = *target++ - *origin++;
        *out++ = *target++ - *origin++;
        *out = *target - *origin;
    }
    if (GRAVITY_SHAPE(work) == GRAVITY_DOWN) {
        distance = njScalor(&delta);
        if (distance < work->scl.x + GRAVITY_MARGIN) {
            sample->flags |= FIELD_GRAVITY;
            sample->gravity.x = 0.0F;
            sample->gravity.y = -1.0F;
            sample->gravity.z = 0.0F;
        }
    } else if (!(sample->flags & FIELD_GRAVITY)) {
        if (GRAVITY_SHAPE(work)) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateZ(NULL, -work->ang.z);
            njRotateY(NULL, -work->ang.y);
            njCalcVector(NULL, &delta, &delta);
            if (njAbs(delta.y) < work->scl.y) {
                delta.y = 0.0F;
            } else if (delta.y > 0.0F) {
                delta.y -= work->scl.y;
            } else {
                delta.y += work->scl.y;
            }
            distance = njScalor(&delta);
            if (distance < work->scl.x + GRAVITY_MARGIN) {
                njUnitMatrix(NULL);
                njRotateY(NULL, work->ang.y);
                njRotateZ(NULL, work->ang.z);
                njCalcVector(NULL, &delta, &delta);
                /* @bug arithmetic: divides by a distance of 0 where pos lies on
                 * the segment, which leaves NaNs in the sample's way down (FDIV
                 * at 0x8c500f12) */
                distance = 1.0F / distance;
                /* @hack spelling: the scale as a walk of three Floats in a
                 * block of its own, where three statements on the fields would
                 * do */
                {
                    Float *at = &delta.x;
                    *at++ *= distance;
                    *at++ *= distance;
                    *at *= distance;
                }
                sample->flags |= FIELD_GRAVITY;
                /* @hack spelling: the sum as a walk of three Floats in a block
                 * of its own, where three statements on the fields would do */
                {
                    Float *out = &sample->gravity.x;
                    Float *in = &delta.x;
                    *out++ += *in++;
                    *out++ += *in++;
                    *out += *in;
                }
            }
            njPopMatrixEx();
        } else {
            distance = njScalor(&delta);
            if (distance < work->scl.x + GRAVITY_MARGIN) {
                /* @bug arithmetic: divides by a distance of 0 where pos is the
                 * center, which leaves NaNs in the sample's way down (FDIV at
                 * 0x8c500f72) */
                distance = 1.0F / distance;
                /* @hack spelling: the scale as a walk of three Floats in a
                 * block of its own, where three statements on the fields would
                 * do */
                {
                    Float *at = &delta.x;
                    *at++ *= distance;
                    *at++ *= distance;
                    *at *= distance;
                }
                sample->flags |= FIELD_GRAVITY;
                /* @hack spelling: the sum as a walk of three Floats in a block
                 * of its own, where three statements on the fields would do */
                {
                    Float *out = &sample->gravity.x;
                    Float *in = &delta.x;
                    *out++ += *in++;
                    *out++ += *in++;
                    *out += *in;
                }
            }
        }
    }
}

static void gravity_destroy(struct task *task);
static void gravity_step(struct task *task);
static void gravity_draw(struct task *task);

void mad_space_gravity_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = gravity_draw;
        task->step = gravity_step;
        task->destructor = gravity_destroy;
        field_add(task, gravity_sample);
    }
}

static void gravity_destroy(struct task *task)
{
    field_remove(task);
}

static void gravity_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_POINT3 ambient;
    Bool outside;
    Float scale;

    if (placement_try_leave(task)) {
        return;
    }
    if (GRAVITY_SHAPE(work) != GRAVITY_CAPSULE) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, -work->ang.z);
    njRotateY(NULL, -work->ang.y);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[0]->pos, &pos);
    /* @hack redundant: a block no path reaches, which tests GRAVITY_SHAPE
     * twice */
    if (0) {
        if (GRAVITY_SHAPE(work)) {
        }
        if (GRAVITY_SHAPE(work)) {
        }
    }
    outside = FALSE;
    if (pos.y > work->scl.y + work->scl.x) {
        outside = TRUE;
        pos.y = work->scl.y + work->scl.x;
    }
    if (pos.y < -work->scl.y - work->scl.x) {
        outside = TRUE;
        pos.y = -work->scl.y - work->scl.x;
    }
    if ((pos.x * pos.x) + (pos.z * pos.z) > (work->scl.x * work->scl.x)) {
        outside = TRUE;
    }
    if (!outside) {
        /* @bug arithmetic: divides by 0 where player 0 lies on the segment's
         * line, which leaves NaNs in both sounds' points (FDIV at
         * 0x8c50115a) */
        scale = work->scl.x / njSqrt((pos.x * pos.x) + (pos.z * pos.z));
        pos.x *= scale;
        pos.z *= scale;
        ambient.y = (work->scl.y + (work->scl.x * AMBIENT_OVERHANG)) *
                    (((Float)rand() * RAND_SCALE) - 0.5F) * 2.0F;
        rand();
        ambient.x = pos.x;
        ambient.z = pos.z;
        njInvertMatrix(NULL);
        njCalcPoint(NULL, &pos, &pos);
        sound_play_at_for(SURFACE_SOUND, task, SOUND_PRIORITY, SOUND_VOLUME,
                          SURFACE_STEPS, &pos);
        if ((Float)rand() * RAND_SCALE < AMBIENT_CHANCE) {
            /* the key, the step's address, is the same for every capsule. all
             * capsules share one slot of AMBIENT_SOUND */
            /* @bug logic: plays AMBIENT_SOUND at ambient, a point in the
             * capsule's frame, as if it were in the world's (JSR at
             * 0x8c5011f4). njCalcPoint takes only pos back into the world (JSR
             * at 0x8c5011b8) */
            sound_play_at_for(AMBIENT_SOUND, (const void *)(Uint32)task->step,
                              SOUND_PRIORITY, SOUND_VOLUME, AMBIENT_STEPS,
                              &ambient);
        }
    }
    njPopMatrixEx();
}

static void gravity_draw(struct task *task)
{
    struct task_work *work = task->work;
    static const char *shape_names[GRAVITY_SHAPES] = {"<- SPHERE", "<- CAPSULE",
                                                      "<- SPHERE(IGONORE)"};
    NJS_POINT3 first;
    NJS_POINT3 second;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and its three calls */
    if (0) {
        (void)"<- ROT Y";
        (void)"<- ROT Z";
        (void)"<- DIST";
        (void)"<- RANGE";
        (void)shape_names[GRAVITY_SHAPE(work)];
        njCalcPoint(NULL, &work->pos, &first);
        njCalcPoint(NULL, &work->scl, &second);
        njCalcPoint(NULL, &work->pos, &second);
    }
    /* @hack redundant: a loop no path enters, which calls trapa */
    while (0) {
        trapa(0);
    }
}
