/* the switch ring groups, SWDRNGL and SWDRNGC in a stage's object list. each
 * is a line or a circle of up to eight rings. the rings appear once a switch
 * id is on */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ring.h"
#include "shared/ring_sparkle.h"
#include "shared/switch.h"
#include "shared/switch_ring_group.h"
#include "system/angle.h"
#include "system/task.h"

NJS_TEXLIST *switch_ring_texlists[] = {
    &ring_texlist,
    &ring_shadow_texlist,
    &ring_sparkle_texlist,
    NULL,
};

/* the most rings in a group */
#define RING_GROUP_RINGS 8
/* the level a group's rings run in */
#define RING_LEVEL 1

/* SPACING_BASE is a line's spacing, or a circle's radius, at a scl.x of 0 */
#define SPACING_BASE 10.0F
#define LINEAR_SPACING(work) ((work)->scl.x + SPACING_BASE)
#define LINEAR_HEIGHT(work) ((work)->scl.y)
#define CIRCLE_RADIUS(work) ((work)->scl.x + SPACING_BASE)
#define CIRCLE_SWEEP(work) ((Angle)(((work)->scl.y + 1.0F) * FULL_TURN))

/* a line's rings rise along the first quarter of a sine wave */
#define LINEAR_SWEEP QUARTER_TURN

#define SWITCH_ID_MASK 0xf
/* @hack redundant: the id taken modulo SWITCH_IDS, which is 32, though the
 * mask keeps it below 16 */
#define SWITCH_ID(work) (((work)->ang.z & SWITCH_ID_MASK) % SWITCH_IDS)

/* a group's modes, in its work's mode. MODE_WAIT waits for the switch id.
 * in MODE_STAGGER, the work's timer counts steps, and each ring appears once
 * the timer reaches its delay. in MODE_ALL, each ring appears at once */
enum switch_ring_mode { MODE_WAIT, MODE_STAGGER, MODE_ALL };
/* the timer stops at TIMER_MAX, above the longest delay, DELAY_MAX */
#define TIMER_MAX 20000

/* a ring's delay is DELAY_PER_DISTANCE steps for each unit of its distance
 * from the group's target. no delay passes DELAY_MAX */
#define DELAY_PER_DISTANCE 0.2F
#define DELAY_MAX 10000.0F

/* the sparkle over each live ring lies SPARKLE_RISE above the ring. a
 * sparkle more than SPARKLE_NEAR from the camera draws SPARKLE_PULL nearer
 * the camera */
#define SPARKLE_TEXTURE 4 /* ring05 of ring_sparkle_texlist */
#define SPARKLE_HALF_SIZE 3.0F
#define SPARKLE_RISE 2.0F
#define SPARKLE_NEAR 15.0F
#define SPARKLE_PULL 12.0F
#define OPAQUE_WHITE 0xffffffff

/* a group's record, in task->motion. the work's tick counts the group's
 * rings. ring i appears at points[i] */
struct switch_ring_group {
    struct ring_link links[RING_GROUP_RINGS];
    NJS_POINT3 points[RING_GROUP_RINGS];
    NJS_POINT3 target; /* where each ring's delay is measured from */
};

/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define GROUP(task) ((struct switch_ring_group *)*(void **)&(task)->motion)
#define CAMERA_POS (camera_views[camera_current_index]->pos)
#define NEAREST_PLAYER_POS(work)                                               \
    (player_work[player_nearest(&(work)->pos)]->pos)

static void switch_ring_group_alloc(struct task *task);
static void switch_ring_group_destroy(struct task *task);
static void switch_ring_linear_step(struct task *task);
static void switch_ring_group_draw(struct task *task);
static void switch_ring_circle_step(struct task *task);

static int switch_ring_group_count(struct task_work *work)
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

/* set points[0] to points[count - 1] along a line from origin toward
 * negative z, spacing apart. points[i] rises by height times the sine of
 * i * (sweep / count). ang_x, ang_y and ang_z turn the line about origin */
static void switch_ring_linear_points(NJS_POINT3 *origin, Angle ang_x,
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
    njPopMatrix(1);
}

/* set points[0] to points[count - 1] round a circle of radius about origin.
 * points[i] lies i * (sweep / count) round from positive z toward positive
 * y. ang_x, ang_y and ang_z turn the circle about origin */
static void switch_ring_circle_points(NJS_POINT3 *origin, Angle ang_x,
                                      Angle ang_y, Angle ang_z, Angle sweep,
                                      int count, NJS_POINT3 *points,
                                      Float radius)
{
    NJS_POINT3 point;
    int i;
    Angle phase;

    /* @hack spelling: sweep reused for each place's share of it, where the
     * share in a local of its own would do */
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

void switch_ring_linear_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 target;
    /* @hack redundant: unused, a local that only sizes the frame */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    NJS_POINT3 unused;

    if (placement_try_leave(task)) {
        return;
    }
    /* @bug logic: the init never sets MODE_ALL. the test is for no
     * placement, while switch_ring_group_alloc reads the state only where
     * there is one, so the test reads as turned round. a group with a
     * placement skips the block, and waits for its switch id even where its
     * state holds spent rings. without a placement, placement_get_state
     * returns what R0 holds, placement_try_leave's FALSE. the branch is at
     * 0x1c0 of the object's code, and the call of placement_get_state at
     * 0x1c4 */
    if (task->placement == NULL) {
#ifdef NONMATCHING
        /* this side keeps the tag's result: placement_get_state without a
         * placement returns the FALSE R0 still holds */
        if (FALSE) {
#else
        if (placement_get_state(task)) {
#endif
            work->mode = MODE_ALL;
        } else if (switch_is_on(SWITCH_ID(work))) {
            /* @bug memory: writes the nearest player's pos through
             * task->motion before switch_ring_group_alloc gives the record.
             * the record's target is then copied from target, a local no
             * path sets. the write is at 0x25a of the object's code, the
             * call of switch_ring_group_alloc at 0x266, and the copy at
             * 0x2c2 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above. it writes target, which
             * the copy below reads, in place of the write through a null
             * task->motion */
            target = NEAREST_PLAYER_POS(work);
#else
            GROUP(task)->target = NEAREST_PLAYER_POS(work);
#endif
            work->mode = MODE_STAGGER;
        }
    }
    switch_ring_group_alloc(task);
    if (task->motion.p != NULL) {
        task->draw = switch_ring_group_draw;
        task->step = switch_ring_linear_step;
        task->destructor = switch_ring_group_destroy;
        switch_ring_linear_points(&work->pos, (Uint16)work->ang.x,
                                  (Uint16)work->ang.y, 0, LINEAR_SWEEP,
                                  LINEAR_HEIGHT(work), LINEAR_SPACING(work),
                                  GROUP(task)->points, work->tick);
        if (work->mode == MODE_STAGGER) {
            /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
            GROUP(task)->target = target;
        }
    }
}

/* give task its record, and set the work's tick to the count of rings.
 * where the heap has no room, task->motion is null and tick keeps its value.
 * each set bit of the placement's state is a spent ring, whose link takes
 * RING_SPENT. bit 7 is links[0]'s, and bit 0 links[7]'s */
static void switch_ring_group_alloc(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    /* @hack type: state, an int the loop tests and shifts as unsigned, where
     * a Uint32 would do */
    int state;

    task->motion.p = syCalloc(1, sizeof(struct switch_ring_group));
    if (task->motion.p == NULL) {
        return;
    }
    work->tick = switch_ring_group_count(work);
    /* @hack redundant: a second test of the record, which nothing has
     * touched since the first */
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

/* the destructor of both kinds of group. it kills each live ring. it saves
 * the spent rings in the placement's state, in the bits
 * switch_ring_group_alloc reads */
static void switch_ring_group_destroy(struct task *task)
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

static void switch_ring_linear_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ring_link *link;
    NJS_POINT3 *point;
    struct task *ring;
    Float delay;
    int live;
    int left;
    int waiting;
    int i;

    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object, with its loop's code. the loop's calls go through
     * registers no code loads, so the functions it calls are stand-ins that
     * build the same code */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ID";
        (void)"<- PITCH+10";
        (void)"<- WAVE SIZE";
        (void)"<- RING NUM+1";
        for (; i < work->tick; i++) {
            /* @hack redundant: a block no path reaches, inside the loop. the
             * block calls three of the stand-ins again, and tests i with an
             * empty body */
            if (0) {
                njTranslateV(NULL, NULL);
                njFogDisable();
                njPopMatrix(1);
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
            if (switch_is_on(SWITCH_ID(work))) {
                GROUP(task)->target = NEAREST_PLAYER_POS(work);
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
                        if (delay > DELAY_MAX) {
                            delay = DELAY_MAX;
                        }
                        /* @hack type: delay converted to a Sint16, where an
                         * int would do for a delay of 0 to DELAY_MAX */
                        if (work->timer < (Sint16)delay) {
                            waiting++;
                            break;
                        }
                        /* fall through */
                    case MODE_ALL:
                        ring = task_create(TASK_WORK, RING_LEVEL, ring_step,
                                           "Ring");
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

/* the draw of both kinds of group. it leaves fog enabled, the blend that
 * PARTICLE_BLEND_ALPHA names, and ring_sparkle_texlist as the current
 * texture list */
static void switch_ring_group_draw(struct task *task)
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
                /* @hack spelling: scale holds the sparkle's distance from
                 * the camera first, where a local of its own would do */
                scale = njScalor(&pos);
                if (scale > SPARKLE_NEAR) {
                    scale = (scale - SPARKLE_PULL) / scale;
                } else {
                    scale = 1.0F;
                }
                pos.x = CAMERA_POS.x + (pos.x * scale);
                pos.y = CAMERA_POS.y + (pos.y * scale);
                pos.z = CAMERA_POS.z + (pos.z * scale);
                /* particle_draw_quad ignores the blend bits of its flags.
                 * PARTICLE_BLEND_ADD names the blend the draw set at its
                 * start, which the quad keeps */
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

void switch_ring_circle_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 target;
    /* @hack redundant: unused, a local that only sizes the frame */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    NJS_POINT3 unused;

    if (placement_try_leave(task)) {
        return;
    }
    /* @bug logic: the init never sets MODE_ALL, as in
     * switch_ring_linear_init. the branch is at 0x8d6 of the object's code,
     * and the call of placement_get_state at 0x8da */
    if (task->placement == NULL) {
#ifdef NONMATCHING
        /* this side keeps the tag's result: placement_get_state without a
         * placement returns the FALSE R0 still holds */
        if (FALSE) {
#else
        if (placement_get_state(task)) {
#endif
            work->mode = MODE_ALL;
        } else if (switch_is_on(SWITCH_ID(work))) {
            /* @bug memory: writes through task->motion before the record
             * exists, as switch_ring_linear_init does. the write is at 0x924
             * of the object's code, the call of switch_ring_group_alloc at
             * 0x930, and the copy at 0x996 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above. it writes target, which
             * the copy below reads, in place of the write through a null
             * task->motion */
            target = NEAREST_PLAYER_POS(work);
#else
            GROUP(task)->target = NEAREST_PLAYER_POS(work);
#endif
            work->mode = MODE_STAGGER;
        }
    }
    switch_ring_group_alloc(task);
    if (task->motion.p != NULL) {
        task->draw = switch_ring_group_draw;
        task->step = switch_ring_circle_step;
        task->destructor = switch_ring_group_destroy;
        switch_ring_circle_points(&work->pos, (Uint16)work->ang.x,
                                  (Uint16)work->ang.y, 0, CIRCLE_SWEEP(work),
                                  work->tick, GROUP(task)->points,
                                  CIRCLE_RADIUS(work));
        if (work->mode == MODE_STAGGER) {
            /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
            GROUP(task)->target = target;
        }
    }
}

/* a circle's step: switch_ring_linear_step's code, but for the strings of
 * the block no path reaches */
static void switch_ring_circle_step(struct task *task)
{
    struct task_work *work = task->work;
    struct ring_link *link;
    NJS_POINT3 *point;
    struct task *ring;
    Float delay;
    int live;
    int left;
    int waiting;
    int i;

    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object, with its loop's code, as in
     * switch_ring_linear_step */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ID";
        (void)"<- RAD+10";
        (void)"<- ROT ANGLE(+1)";
        (void)"<- RING NUM+1";
        for (; i < work->tick; i++) {
            /* @hack redundant: a block no path reaches, inside the loop. the
             * block calls three of the stand-ins again, and tests i with an
             * empty body */
            if (0) {
                njTranslateV(NULL, NULL);
                njFogDisable();
                njPopMatrix(1);
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
            if (switch_is_on(SWITCH_ID(work))) {
                GROUP(task)->target = NEAREST_PLAYER_POS(work);
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
                        if (delay > DELAY_MAX) {
                            delay = DELAY_MAX;
                        }
                        /* @hack type: delay converted to a Sint16, where an
                         * int would do for a delay of 0 to DELAY_MAX */
                        if (work->timer < (Sint16)delay) {
                            waiting++;
                            break;
                        }
                        /* fall through */
                    case MODE_ALL:
                        ring = task_create(TASK_WORK, RING_LEVEL, ring_step,
                                           "Ring");
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
