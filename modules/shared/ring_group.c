/* ring groups: up to eight rings placed as one object, along a line or round
 * a circle. an object list names the two kinds RING_LINEAR and RING_CIRCLE,
 * or, in some images, RINGLINE and RINGCIRC */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "object/set.h"
#include "shared/ring.h"
#include "shared/ring_group.h"
#include "system/angle.h"
#include "system/task.h"

NJS_TEXLIST *ring_group_texlists[] = {
    &ring_texlist,
    &ring_shadow_texlist,
    NULL,
};

/* the most rings in a group */
#define RING_GROUP_RINGS 8
/* the level a group's rings run in */
#define RING_LEVEL 1

/* what a group's scl gives, as ring_group.h says. SPACING_BASE is a line's
 * spacing, or a circle's radius, at a scl.x of 0 */
#define SPACING_BASE 10.0F
#define LINEAR_SPACING(work) ((work)->scl.x + SPACING_BASE)
#define LINEAR_HEIGHT(work) ((work)->scl.y)
#define CIRCLE_RADIUS(work) ((work)->scl.x + SPACING_BASE)
#define CIRCLE_SWEEP(work) ((Angle)(((work)->scl.y + 1.0F) * FULL_TURN))

/* a line's rings rise along the first quarter of a sine wave */
#define LINEAR_SWEEP QUARTER_TURN

/* a group's record, in task->motion. links[i] joins ring i to the group.
 * points[i] is where ring i is made. the work's tick counts the rings, 1
 * through RING_GROUP_RINGS */
struct ring_group {
    struct ring_link links[RING_GROUP_RINGS];
    NJS_POINT3 points[RING_GROUP_RINGS];
};

/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define GROUP(task) ((struct ring_group *)*(void **)&(task)->motion)

static void ring_group_alloc(struct task *task);
static void ring_group_destroy(struct task *task);
static void ring_linear_step(struct task *task);
static void ring_circle_step(struct task *task);

static int ring_group_count(struct task_work *work)
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

/* set points[0] to points[count - 1] on a line from origin toward negative
 * z, spacing apart. points[i] is raised by height times the sine of
 * i * (sweep / count). the three angles turn the line about origin */
static void ring_linear_points(NJS_POINT3 *origin, Angle ang_x, Angle ang_y,
                               Angle ang_z, Angle sweep, NJS_POINT3 *points,
                               int count, Float height, Float spacing)
{
    NJS_POINT3 point;
    int i;
    Angle phase;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(origin);
    njRotateZ(NULL, ang_z);
    njRotateY(NULL, ang_y);
    njRotateX(NULL, ang_x);
    point.x = 0.0F;
    /* @hack redundant: point.y cleared, though the loop sets it before each
     * use */
    point.y = 0.0F;
    point.z = 0.0F;
    /* @hack spelling: sweep reused for each place's share of it, where the
     * share in a local of its own would do */
    sweep /= count;
    phase = 0;
    for (i = 0; i < count; i++) {
        point.y = njSin(phase) * height;
        njCalcPoint(NULL, &point, &points[i]);
        point.z -= spacing;
        phase += sweep;
    }
    njPopMatrixEx();
}

/* set points[0] to points[count - 1] on a circle of radius about origin. the
 * circle lies in the plane of y and z. points[i] lies i * (sweep / count) round
 * from positive z toward positive y. the three angles turn the circle about
 * origin */
static void ring_circle_points(NJS_POINT3 *origin, Angle ang_x, Angle ang_y,
                               Angle ang_z, Angle sweep, int count,
                               NJS_POINT3 *points, Float radius)
{
    NJS_POINT3 point;
    int i;
    Angle phase;

    /* @hack spelling: sweep reused for each place's share of it, where the
     * share in a local of its own would do */
    sweep /= count;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(origin);
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
    njPopMatrixEx();
}

void ring_linear_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = ring_linear_step;
    task->destructor = ring_group_destroy;
    ring_group_alloc(task);
    if (task->motion.p != NULL) {
        /* @hack type: the angles cast to Uint16, where the work's angles
         * passed as they are would do */
        ring_linear_points(&work->pos, (Uint16)work->ang.x, (Uint16)work->ang.y,
                           (Uint16)work->ang.z, LINEAR_SWEEP,
                           GROUP(task)->points, work->tick, LINEAR_HEIGHT(work),
                           LINEAR_SPACING(work));
    }
}

/* give task its record, and set the work's tick to the count of rings. the
 * placement's state holds a bit for each RING_SPENT link. bit 7 is
 * links[0]'s, and bit 0 links[7]'s. where the heap can't give the record,
 * task->motion is null and tick keeps its value */
static void ring_group_alloc(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    /* @hack type: state, an int the loop tests and shifts as unsigned, where
     * a Uint32 would do */
    int state;

    task->motion.p = syCalloc(1, sizeof(struct ring_group));
    if (task->motion.p == NULL) {
        return;
    }
    work->tick = ring_group_count(work);
    /* @hack redundant: a second test of the record, which nothing has
     * touched since the first */
    if (task->motion.p == NULL) {
        return;
    }
    state = 0;
    if (task->placement != NULL) {
        state = (int)placement_get_state(task);
    }
    for (i = RING_GROUP_RINGS - 1; i >= 0; i--) {
        if (state & 1U) {
            GROUP(task)->links[i].flags = RING_SPENT;
        }
        state = (Uint32)state >> 1;
    }
}

/* the destructor of both kinds of group. it detaches and kills each ring
 * that lives. the RING_SPENT links go to the placement's state, in the bits
 * ring_group_alloc reads */
static void ring_group_destroy(struct task *task)
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
    /* @hack redundant: a second test of the record, which nothing has
     * touched since the first */
    if (task->motion.p != NULL) {
        syFree(task->motion.p);
    }
    task->motion.p = NULL;
}

/* a line's step. the group tests whether it has left range only while none
 * of its rings lives. the step makes each ring that neither lives nor is
 * spent. it kills the group once every ring is spent */
static void ring_linear_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ring_link *link;
    NJS_POINT3 *point;
    struct task *ring;
    int live;
    int left;
    int i;

    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object. of its code, the image holds only the loop's body, without
     * the names of the functions that body calls. the calls written here stand
     * in for those, and build the same body */
    if (0) {
        ring_linear_points(&work->pos, (Uint16)work->ang.x, (Uint16)work->ang.y,
                           (Uint16)work->ang.z, LINEAR_SWEEP,
                           GROUP(task)->points, work->tick, LINEAR_HEIGHT(work),
                           LINEAR_SPACING(work));
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- PITCH+10";
        (void)"<- WAVE SIZE";
        (void)"<- RING NUM+1";
        njTranslateEx(&work->pos);
        for (i = 0; i < work->tick; i++) {
            njPushMatrixEx();
            njTranslateEx(&GROUP(task)->points[i]);
            njFogDisable();
            njPopMatrixEx();
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
        link = GROUP(task)->links;
        point = GROUP(task)->points;
        left = 0;
        for (i = 0; i < work->tick; i++, link++, point++) {
            if (!(link->flags & RING_SPENT)) {
                if (link->ring == NULL) {
                    ring =
                        task_create(TASK_WORK, RING_LEVEL, ring_step, "Ring");
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
                /* @hack redundant: a second test of the link's RING_SPENT,
                 * which nothing has set since the first */
                if (!(link->flags & RING_SPENT)) {
                    left++;
                }
            }
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

void ring_circle_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->step = ring_circle_step;
    task->destructor = ring_group_destroy;
    ring_group_alloc(task);
    if (task->motion.p != NULL) {
        /* @hack type: the angles cast to Uint16, where the work's angles
         * passed as they are would do */
        ring_circle_points(&work->pos, (Uint16)work->ang.x, (Uint16)work->ang.y,
                           (Uint16)work->ang.z, CIRCLE_SWEEP(work), work->tick,
                           GROUP(task)->points, CIRCLE_RADIUS(work));
    }
}

/* a circle's step, the same as a line's */
static void ring_circle_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ring_link *link;
    NJS_POINT3 *point;
    struct task *ring;
    int live;
    int left;
    int i;

    /* @hack redundant: a block no path reaches, as in ring_linear_step, here
     * with a circle's strings */
    if (0) {
        ring_circle_points(&work->pos, (Uint16)work->ang.x, (Uint16)work->ang.y,
                           (Uint16)work->ang.z, CIRCLE_SWEEP(work), work->tick,
                           GROUP(task)->points, CIRCLE_RADIUS(work));
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- RAD+10";
        (void)"<- ROT ANGLE(+1)";
        (void)"<- RING NUM+1";
        njTranslateEx(&work->pos);
        for (i = 0; i < work->tick; i++) {
            njPushMatrixEx();
            njTranslateEx(&GROUP(task)->points[i]);
            njFogDisable();
            njPopMatrixEx();
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
        link = GROUP(task)->links;
        point = GROUP(task)->points;
        left = 0;
        for (i = 0; i < work->tick; i++, link++, point++) {
            if (!(link->flags & RING_SPENT)) {
                if (link->ring == NULL) {
                    ring =
                        task_create(TASK_WORK, RING_LEVEL, ring_step, "Ring");
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
                /* @hack redundant: a second test of the link's RING_SPENT,
                 * which nothing has set since the first */
                if (!(link->flags & RING_SPENT)) {
                    left++;
                }
            }
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
