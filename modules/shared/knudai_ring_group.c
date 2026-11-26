/* the ring groups a flute stand calls up, KDDRNGL and KDDRNGC in a stage's
 * object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/knudai.h"
#include "shared/knudai_ring_group.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "shared/stage_texture_groups.h"
#include "system/angle.h"
#include "system/task.h"

#include "shared/knudai_ring_group/text.inc"

NJS_TEXLIST *ring_effect_texlists[4] = {
#include "shared/knudai_ring_group/ring_effect_texlists.inc"
};

/* the most rings in a group */
#define RING_GROUP_RINGS 8
/* the level a group's rings run in */
#define RING_LEVEL 1

/* a line's spacing and a circle's radius are scl.x plus SPACING_BASE */
#define SPACING_BASE 10.0F
#define LINEAR_SPACING(work) ((work)->scl.x + SPACING_BASE)
#define LINEAR_HEIGHT(work) ((work)->scl.y)
#define CIRCLE_RADIUS(work) ((work)->scl.x + SPACING_BASE)
#define CIRCLE_SWEEP(work) ((Angle)(((work)->scl.y + 1.0F) * FULL_TURN))

/* a line's rings rise along the first quarter of a sine wave */
#define LINEAR_SWEEP QUARTER_TURN

#define KNUDAI_ID(work, mask) (((work)->ang.z & (mask)) % KNUDAI_IDS)

/* a group's modes, in its work's mode. in MODE_STAGGER, a ring appears once
 * the work's timer, which counts steps, reaches the ring's delay */
enum knudai_ring_mode { MODE_WAIT, MODE_STAGGER, MODE_ALL };
/* the timer stops at TIMER_MAX, past the longest delay */
#define TIMER_MAX 20000

/* a ring's delay is DELAY_PER_DISTANCE steps for each unit of its distance
 * from the group's target. a line's delays stop at LINEAR_DELAY_MAX, and a
 * circle's at CIRCLE_DELAY_MAX */
#define DELAY_PER_DISTANCE 0.8F
#define LINEAR_DELAY_MAX 160.0F
#define CIRCLE_DELAY_MAX 10000.0F

/* a circle plays APPEAR_SOUND on each of the SOUND_LEAD steps that end with
 * a ring's appearance. the sound's key is the group's work, so a call while
 * it plays gives it its time again */
#define APPEAR_SOUND 0x100f
#define SOUND_LEAD 3
#define SOUND_PRIORITY 1
#define SOUND_VOLUME SOUND_LOUDEST
#define SOUND_DURATION 80 /* in sound updates */

/* the sparkle over each live ring lies SPARKLE_RISE above the ring. a sparkle
 * more than SPARKLE_NEAR from the camera draws SPARKLE_PULL nearer the
 * camera */
#define SPARKLE_TEXTURE 4 /* ring05 of ring_sparkle_texlist */
#define SPARKLE_HALF_SIZE 3.0F
#define SPARKLE_RISE 2.0F
#define SPARKLE_NEAR 15.0F
#define SPARKLE_PULL 12.0F
#define OPAQUE_WHITE 0xffffffff

/* a group's record, in task->motion. the work's tick counts its rings */
struct knudai_ring_group {
    struct ring_link links[RING_GROUP_RINGS];
    NJS_POINT3 points[RING_GROUP_RINGS]; /* where each ring appears */
    NJS_POINT3 target; /* where each ring's delay is measured from */
};

/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define GROUP(task) ((struct knudai_ring_group *)*(void **)&(task)->motion)
#define CAMERA_POS (camera_views[camera_current_index]->pos)

static void knudai_ring_group_restore(struct task *task);
static void knudai_ring_group_destroy(struct task *task);
static void knudai_ring_linear_step(struct task *task);
static void knudai_ring_group_draw(struct task *task);
static void knudai_ring_circle_step(struct task *task);

static int knudai_ring_group_count(struct task_work *work)
{
    int count = (int)work->scl.z;

    if (count < 1) {
        count = 1;
    }
    if (count > RING_GROUP_RINGS) {
        count = RING_GROUP_RINGS;
    }
    return count;
}

/* set points[i] to origin plus an offset, for each i below count. the three
 * angles turn the offset, which is (0, height * njSin(i * (sweep / count)), -i
 * * spacing) */
static void knudai_ring_linear_points(NJS_POINT3 *origin, Angle ang_x,
                                      Angle ang_y, Angle ang_z, Angle sweep,
                                      Float height, Float spacing,
                                      NJS_POINT3 *points, int count)
{
    NJS_POINT3 point;
    int i;
    Angle phase;

    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, origin);
    njRotateZ(NULL, ang_z);
    njRotateY(NULL, ang_y);
    njRotateX(NULL, ang_x);
    point.x = 0.0F;
    /* @hack redundant: point.y cleared, though the loop sets it before each
     * use */
    point.y = 0.0F;
    point.z = 0.0F;
    /* @hack spelling: sweep divided in place into each ring's share, where
     * the share in a local of its own would do */
    sweep /= count;
    phase = 0;
    for (i = 0; i < count; i++) {
        point.y = njSin(phase) * height;
        njCalcPoint(NULL, &point, &points[i]);
        point.z -= spacing;
        phase += sweep;
    }
    njPopMatrix(1);
}

/* set points[i] to origin plus an offset, for each i below count. the three
 * angles turn the offset, which is radius * (0, njSin(i * (sweep / count)),
 * njCos(i * (sweep / count))) */
static void knudai_ring_circle_points(NJS_POINT3 *origin, Angle ang_x,
                                      Angle ang_y, Angle ang_z, Angle sweep,
                                      int count, NJS_POINT3 *points,
                                      Float radius)
{
    NJS_POINT3 point;
    int i;
    Angle phase;

    /* @hack spelling: sweep divided in place into each ring's share, where
     * the share in a local of its own would do */
    sweep /= count;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, origin);
    njRotateZ(NULL, ang_z);
    njRotateY(NULL, ang_y);
    njRotateX(NULL, ang_x);
    point.x = 0.0F;
    /* @hack redundant: point.y and point.z cleared, though the loop sets each
     * before its use */
    point.y = 0.0F;
    point.z = 0.0F;
    phase = 0;
    for (i = 0; i < count; i++) {
        point.y = njSin(phase) * radius;
        point.z = njCos(phase) * radius;
        njCalcPoint(NULL, &point, &points[i]);
        phase += sweep;
    }
    njPopMatrix(1);
}

void knudai_ring_linear_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 target;
    NJS_POINT3 knudai_pos;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct knudai_ring_group));
    if (task->motion.p == NULL) {
        return;
    }
    /* @bug logic: the test of placement is the wrong way round. a group with
     * a placement starts in MODE_WAIT whatever its state. a group without one
     * reads placement_get_state's undefined result. knudai_ring_group_restore
     * reads the state only with a placement. the branch at 0x1d4 of the
     * object's code skips this block where there is a placement */
    if (task->placement == NULL) {
#ifdef NONMATCHING
        /* this side neither keeps nor repairs the bug tagged above. it reads
         * the undefined state as 0, so a group without a placement never starts
         * in MODE_ALL. the console reads the low byte of task->motion, which
         * syCalloc leaves in R0. where that byte isn't 0, the console starts
         * the group in MODE_ALL */
        if (FALSE) {
#else
        if (placement_get_state(task)) {
#endif
            work->mode = MODE_ALL;
        } else if (knudai_played(KNUDAI_ID(work, KNUDAI_ID_MASK)) == 1 &&
                   knudai_get_pos(KNUDAI_ID(work, KNUDAI_ID_MASK),
                                  &knudai_pos)) {
            work->mode = MODE_STAGGER;
        }
    }
    task->draw = knudai_ring_group_draw;
    task->step = knudai_ring_linear_step;
    task->destructor = knudai_ring_group_destroy;
    knudai_ring_group_restore(task);
    /* @hack redundant: a second test of the record, which nothing has
     * touched since the first */
    if (task->motion.p != NULL) {
        knudai_ring_linear_points(&work->pos, (Uint16)work->ang.x,
                                  (Uint16)work->ang.y, 0, LINEAR_SWEEP,
                                  LINEAR_HEIGHT(work), LINEAR_SPACING(work),
                                  GROUP(task)->points, work->tick);
    }
    /* only a group without a placement starts in MODE_STAGGER. its target
     * should be knudai_pos, which knudai_get_pos set, but the console's code
     * stores target, which no path sets. the timer starts at TIMER_MAX, so
     * every ring is due at once whatever target holds */
    if (work->mode == MODE_STAGGER) {
#ifdef NONMATCHING
        /* this side stores knudai_pos, where the matching side stores the unset
         * target */
        GROUP(task)->target = knudai_pos;
#else
        /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
        GROUP(task)->target = target;
#endif
        work->timer = TIMER_MAX;
    }
}

/* set the work's tick to the count of rings. mark each link spent whose bit
 * the placement's state holds. bit 7 stands for links[0], and bit 0 for
 * links[7] */
static void knudai_ring_group_restore(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    /* @hack type: state, an int the loop tests and shifts as unsigned, where
     * a Uint32 would do */
    int state;

    work->tick = knudai_ring_group_count(work);
    /* @hack redundant: a test of the record, which each caller has made */
    if (task->motion.p == NULL) {
        return;
    }
    state = 0;
    if (task->placement != NULL) {
        state = placement_get_state(task);
    }
    for (i = RING_GROUP_RINGS - 1; i >= 0; i--) {
        if (state & 1U) {
            GROUP(task)->links[i].flags = RING_SPENT;
        }
        state = (Uint32)state >> 1;
    }
}

/* kill each live ring, and keep the spent links in the placement's state
 * for knudai_ring_group_restore */
static void knudai_ring_group_destroy(struct task *task)
{
    struct task *ring;
    int state;
    int i;

    if (task->motion.p != NULL) {
        state = 0;
        for (i = 0; i < RING_GROUP_RINGS; i++) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((ring = GROUP(task)->links[i].ring) != NULL) {
                ring_detach(ring);
                task_kill(ring);
            }
            state = (state << 1) |
                    ((GROUP(task)->links[i].flags & RING_SPENT) ? 1 : 0);
        }
        if (task->placement != NULL) {
            placement_set_state(task, state);
        }
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void knudai_ring_linear_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ring_link *link;
    NJS_POINT3 *point;
    struct task *ring;
    Float delay;
    int mask;
    int live;
    int waiting;
    int left;
    int i;

    /* @hack spelling: mask, a local for KNUDAI_ID_MASK, where the constant in
     * place would do */
    mask = KNUDAI_ID_MASK;
    /* @hack redundant: a block no path reaches, kept for its strings and its
     * loop, which SHC still places in the object. the image doesn't show
     * which functions the loop called: the calls here are ones that build
     * the same code */
    if (0) {
        (void)KNUDAI_RING_GROUP_DEBUG_PITCH;
        (void)KNUDAI_RING_GROUP_DEBUG_YAW;
        (void)KNUDAI_RING_GROUP_DEBUG_ID;
        (void)KNUDAI_RING_GROUP_DEBUG_SPACING;
        (void)KNUDAI_RING_GROUP_DEBUG_WAVE_SIZE;
        (void)KNUDAI_RING_GROUP_DEBUG_RING_COUNT;
        for (; i < work->tick; i++) {
            /* @hack redundant: an if (0) block inside the loop, with the
             * stand-ins called again and an empty test of i */
            if (0) {
                njPushMatrixEx();
                njFogDisable();
                njPopMatrix(1);
                njTranslateV(NULL, NULL);
                if (i) {
                }
            }
            njPushMatrixEx();
            njTranslateV(NULL, &GROUP(task)->points[i]);
            njFogDisable();
            njPopMatrix(1);
        }
    } else if (task->motion.p != NULL) {
        live = 0;
        link = GROUP(task)->links;
        for (i = 0; i < work->tick; i++, link++) {
            if (link->ring != NULL) {
                /* @hack spelling: live counted, though it stops at 1, where
                 * live = 1 would do */
                live++;
                break;
            }
        }
        /* @hack spelling: one test inside the other, where live == 0 &&
         * placement_try_leave(task) would do */
        if (live == 0) {
            if (placement_try_leave(task)) {
                return;
            }
        }
        switch (work->mode) {
        case MODE_WAIT:
            if (knudai_played(KNUDAI_ID(work, mask)) == 1 &&
                knudai_get_pos(KNUDAI_ID(work, mask), &GROUP(task)->target)) {
                work->mode = MODE_STAGGER;
            }
            break;
        case MODE_STAGGER:
            if (work->timer < TIMER_MAX) {
                work->timer++;
            }
            break;
        }
        link = GROUP(task)->links;
        point = GROUP(task)->points;
        waiting = 0;
        left = 0;
        for (i = 0; i < work->tick; i++, link++, point++) {
            if (!(link->flags & RING_SPENT)) {
                if (link->ring == NULL) {
                    switch (work->mode) {
                    case MODE_STAGGER:
                        if (stage_current_id() != STAGE_RELAY_SONIC) {
                            delay = njDistanceP2P(point, &GROUP(task)->target) *
                                    DELAY_PER_DISTANCE;
                            if (delay > LINEAR_DELAY_MAX) {
                                delay = LINEAR_DELAY_MAX;
                            }
                            /* @hack type: delay converted to a Sint16,
                             * where an int would do for a delay of 0 to
                             * LINEAR_DELAY_MAX */
                            if (work->timer < (Sint16)delay) {
                                waiting++;
                                break;
                            }
                        }
                        /* fall through */
                    case MODE_ALL:
                        ring = task_create(TASK_WORK, RING_LEVEL, ring_step,
                                           KNUDAI_RING_GROUP_RING_TASK_NAME);
                        if (ring != NULL) {
                            link->flags = 0;
                            ring->work->pos = *point;
                            /* @hack redundant: the angles cleared, though
                             * task_create zeroed the work */
                            ring->work->ang.x = 0;
                            ring->work->ang.y = 0;
                            ring->work->ang.z = 0;
                            ring_attach(ring, link);
                        }
                    }
                }
                /* @hack redundant: a second test of the link's RING_SPENT,
                 * which nothing has set since the first */
                if (!(link->flags & RING_SPENT)) {
                    left++;
                }
            }
        }
        if (work->mode == MODE_STAGGER && waiting == 0) {
            work->mode = MODE_ALL;
        }
        if (left == 0) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
    }
}

/* an additive sparkle over each live ring. the draw leaves fog on, the blend
 * PARTICLE_BLEND_ALPHA names, and ring_sparkle_texlist as the current texture
 * list */
static void knudai_ring_group_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct ring_link *link;
    NJS_POINT3 *point;
    Float scale;
    int i;

    njSetTexture(&ring_sparkle_texlist);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    if (task->motion.p != NULL) {
        link = GROUP(task)->links;
        point = GROUP(task)->points;
        for (i = 0; i < work->tick; i++, link++, point++) {
            if (link->ring != NULL) {
                pos.x = point->x - CAMERA_POS.x;
                pos.y = point->y - CAMERA_POS.y + SPARKLE_RISE;
                pos.z = point->z - CAMERA_POS.z;
                /* @hack spelling: scale holds the camera's distance first,
                 * where a local of its own would do */
                scale = njScalor(&pos);
                if (scale > SPARKLE_NEAR) {
                    scale = (scale - SPARKLE_PULL) / scale;
                } else {
                    scale = 1.0F;
                }
                pos.x = CAMERA_POS.x + (pos.x * scale);
                pos.y = CAMERA_POS.y + (pos.y * scale);
                pos.z = CAMERA_POS.z + (pos.z * scale);
                /* particle_draw_quad reads no blend from its flags.
                 * PARTICLE_BLEND_ADD names the blend the draw set at its
                 * start */
                particle_draw_quad(&pos, SPARKLE_TEXTURE, QUARTER_TURN, 0,
                                   SPARKLE_HALF_SIZE, SPARKLE_HALF_SIZE,
                                   OPAQUE_WHITE, PARTICLE_BLEND_ADD, 0.0F,
                                   0.0F);
            }
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

void knudai_ring_circle_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 target;
    NJS_POINT3 knudai_pos;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct knudai_ring_group));
    if (task->motion.p == NULL) {
        return;
    }
    /* @bug logic: the test of placement is the wrong way round, as in
     * knudai_ring_linear_init. the branch at 0x926 of the object's code skips
     * this block where there is a placement */
    if (task->placement == NULL) {
#ifdef NONMATCHING
        /* this side neither keeps nor repairs the bug tagged above. it reads
         * the undefined state as 0, so a group without a placement never starts
         * in MODE_ALL. the console reads the low byte of task->motion, which
         * syCalloc leaves in R0. where that byte isn't 0, the console starts
         * the group in MODE_ALL */
        if (FALSE) {
#else
        if (placement_get_state(task)) {
#endif
            work->mode = MODE_ALL;
        } else if (knudai_played(KNUDAI_ID(work, KNUDAI_ID_MASK)) == 1 &&
                   knudai_get_pos(KNUDAI_ID(work, KNUDAI_ID_MASK),
                                  &knudai_pos)) {
            work->mode = MODE_STAGGER;
        }
    }
    task->draw = knudai_ring_group_draw;
    task->step = knudai_ring_circle_step;
    task->destructor = knudai_ring_group_destroy;
    knudai_ring_group_restore(task);
    /* @hack redundant: a second test of the record, which nothing has
     * touched since the first */
    if (task->motion.p != NULL) {
        knudai_ring_circle_points(&work->pos, (Uint16)work->ang.x,
                                  (Uint16)work->ang.y, 0, CIRCLE_SWEEP(work),
                                  work->tick, GROUP(task)->points,
                                  CIRCLE_RADIUS(work));
    }
    /* only a group without a placement starts in MODE_STAGGER */
    if (work->mode == MODE_STAGGER) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: it stores
         * knudai_pos */
        GROUP(task)->target = knudai_pos;
#else
        /* @bug uninitialized: the rings' delays measure from target, which no
         * path sets, where knudai_pos was meant, as in knudai_ring_linear_init.
         * knudai_pos lies at R15 and target at R15 + 12. MOV R15,R5 passes the
         * first to knudai_get_pos, and MOV.L @R3,R1 reads the second, at 0x962
         * and 0x9e6 of the object's code */
        /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
        GROUP(task)->target = target;
#endif
    }
}

/* knudai_ring_linear_step, but with APPEAR_SOUND, CIRCLE_DELAY_MAX and a
 * delay on every stage */
static void knudai_ring_circle_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ring_link *link;
    NJS_POINT3 *point;
    struct task *ring;
    Float delay;
    int mask;
    int live;
    int waiting;
    int left;
    int i;

    /* @hack spelling: mask, a local for KNUDAI_ID_MASK, where the constant in
     * place would do */
    mask = KNUDAI_ID_MASK;
    /* @hack redundant: an if (0) block, as in knudai_ring_linear_step, with a
     * circle's strings */
    if (0) {
        (void)KNUDAI_RING_GROUP_DEBUG_PITCH;
        (void)KNUDAI_RING_GROUP_DEBUG_YAW;
        (void)KNUDAI_RING_GROUP_DEBUG_ID;
        (void)KNUDAI_RING_GROUP_DEBUG_RADIUS;
        (void)KNUDAI_RING_GROUP_DEBUG_ROTATION;
        (void)KNUDAI_RING_GROUP_DEBUG_RING_COUNT;
        for (; i < work->tick; i++) {
            njPushMatrixEx();
            njTranslateV(NULL, &GROUP(task)->points[i]);
            njFogDisable();
            njPopMatrix(1);
        }
    } else if (task->motion.p != NULL) {
        live = 0;
        link = GROUP(task)->links;
        for (i = 0; i < work->tick; i++, link++) {
            if (link->ring != NULL) {
                /* @hack spelling: live counted, though it stops at 1, where
                 * live = 1 would do */
                live++;
                break;
            }
        }
        /* @hack spelling: one test inside the other, where live == 0 &&
         * placement_try_leave(task) would do */
        if (live == 0) {
            if (placement_try_leave(task)) {
                return;
            }
        }
        switch (work->mode) {
        case MODE_WAIT:
            if (knudai_played(KNUDAI_ID(work, mask)) == 1 &&
                knudai_get_pos(KNUDAI_ID(work, mask), &GROUP(task)->target)) {
                work->mode = MODE_STAGGER;
            }
            break;
        case MODE_STAGGER:
            if (work->timer < TIMER_MAX) {
                work->timer++;
            }
            break;
        }
        link = GROUP(task)->links;
        point = GROUP(task)->points;
        waiting = 0;
        left = 0;
        for (i = 0; i < work->tick; i++, link++, point++) {
            if (!(link->flags & RING_SPENT)) {
                if (link->ring == NULL) {
                    switch (work->mode) {
                    case MODE_STAGGER:
                        delay = njDistanceP2P(point, &GROUP(task)->target) *
                                DELAY_PER_DISTANCE;
                        if (delay > CIRCLE_DELAY_MAX) {
                            delay = CIRCLE_DELAY_MAX;
                        }
                        /* @hack type: delay converted to a Sint16 in the two
                         * tests that follow, where an int would do for a
                         * delay of 0 to CIRCLE_DELAY_MAX */
                        if (work->timer + SOUND_LEAD > (Sint16)delay) {
                            sound_play_at_for(APPEAR_SOUND, work,
                                              SOUND_PRIORITY, SOUND_VOLUME,
                                              SOUND_DURATION, &work->pos);
                        }
                        if (work->timer < (Sint16)delay) {
                            waiting++;
                            break;
                        }
                        /* fall through */
                    case MODE_ALL:
                        ring = task_create(TASK_WORK, RING_LEVEL, ring_step,
                                           KNUDAI_RING_GROUP_RING_TASK_NAME);
                        if (ring != NULL) {
                            link->flags = 0;
                            ring->work->pos = *point;
                            /* @hack redundant: the angles cleared, though
                             * task_create zeroed the work */
                            ring->work->ang.x = 0;
                            ring->work->ang.y = 0;
                            ring->work->ang.z = 0;
                            ring_attach(ring, link);
                        }
                    }
                }
                /* @hack redundant: a second test of the link's RING_SPENT,
                 * which nothing has set since the first */
                if (!(link->flags & RING_SPENT)) {
                    left++;
                }
            }
        }
        if (work->mode == MODE_STAGGER && waiting == 0) {
            work->mode = MODE_ALL;
        }
        if (left == 0) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
    }
}
