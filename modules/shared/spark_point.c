/* point sparks: sparks that fly, fall and bounce, each drawn as a point. one
 * task, LnSparkExec, moves and draws them all */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "shared/spark_point.h"
#include "shared/spark_xyz.h"
#include "system/task.h"
#include "system/util.h"

/* the ring buffer's slots. it holds at most RING_SLOTS - 1 sparks */
#define RING_SLOTS 255
#define SPARK_LIFE 80 /* a spark's life, in steps */
#define SPARK_LEVEL 2 /* the task level of LnSparkExec */
/* struct spark_point's pos[1] guesses a spark's next pos. the guess slows
 * the speed by the drag times AHEAD_DRAG, where the spark slows by the drag
 * alone */
#define AHEAD_DRAG 0.97F

/* a spark lies at pos[0]. pos[1] is a guess at its next pos, which
 * spark_point_step makes and no draw reads */
struct spark_point {
    NJS_POINT3 pos[2];
    NJS_VECTOR spd;
    Float floor_y;
    Sint32 life;
};

/* the ring buffer: count sparks, oldest first, from sparks[first] on and
 * wrapping round to sparks[0] */
struct spark_point_ring {
    struct spark_point sparks[RING_SLOTS];
    Sint32 first;
    Sint32 count;
};

static void spark_point_step(struct task *task);
static void spark_point_draw(struct task *task);
static void spark_point_destroy(struct task *task);

/* a spark's colors for line_queue, which reads only the first for a point.
 * SHC refuses braces around each color, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR spark_point_colors[2] = {0xff902000, 0xffffa030};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Float spark_point_drag = 0.98F;
static Float spark_point_bounce = 0.7F;
static Float spark_point_gravity = 0.08F;
static Float dat_spark_point_0 = 0.08F;
/* the ring buffer, or null while no spark needs one */
static struct spark_point_ring *spark_point_ring = NULL;
static struct task *spark_point_task = NULL; /* LnSparkExec, or null */

void spark_point_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float spread,
                        Float floor_y)
{
    struct task *task;
    struct spark_point *spark;
    int index;
    Float vx;
    Float vy;
    Float vz;
    Float x;
    Float y;
    Float z;

    task = spark_point_task;
    if (task == NULL) {
        task = task_create(0, SPARK_LEVEL, spark_point_step, "LnSparkExec");
        if (task != NULL) {
            task->draw = spark_point_draw;
            /* @hack redundant: the step stored again, though task_create
             * gave it */
            task->step = spark_point_step;
            task->destructor = spark_point_destroy;
            spark_point_task = task;
        }
    }
    if (spark_point_task != NULL && spark_point_ring == NULL) {
        spark_point_ring = syCalloc(1, sizeof(struct spark_point_ring));
    }
    if (spark_point_task != NULL && spark_point_ring != NULL) {
        index = spark_point_ring->count;
        if (index >= RING_SLOTS - 1) {
            spark_point_ring->first =
                (spark_point_ring->first + 1) % RING_SLOTS;
        } else {
            index++;
        }
        spark_point_ring->count = index;
        index += spark_point_ring->first;
        index--;
        if (index < 0) {
            index = RING_SLOTS - 1;
        }
        index = index % RING_SLOTS;
        spark = &spark_point_ring->sparks[index];
        spread *= 2.0F;
        /* @hack spelling: the speed's parts summed in vx, vy and vz, where
         * stores to spark->spd would do */
        vx = (((Float)rand() * RAND_SCALE) - 0.5F) * spread;
        vy = (((Float)rand() * RAND_SCALE) - 0.5F) * spread;
        vz = (((Float)rand() * RAND_SCALE) - 0.5F) * spread;
        vx += spd->x;
        vy += spd->y;
        vz += spd->z;
        spark->spd.x = vx;
        spark->spd.y = vy;
        spark->spd.z = vz;
        /* @hack spelling: pos's parts in x, y and z, where pos copied to
         * each point would do */
        x = pos->x;
        y = pos->y;
        z = pos->z;
        spark->pos[0].x = x;
        spark->pos[0].y = y;
        spark->pos[0].z = z;
        spark->pos[1].x = x;
        spark->pos[1].y = y;
        spark->pos[1].z = z;
        spark->floor_y = floor_y;
        spark->life = SPARK_LIFE;
    } else {
        if (spark_point_ring != NULL) {
            syFree(spark_point_ring);
            spark_point_ring = NULL;
        }
        if (spark_point_task != NULL) {
            task_destroy(spark_point_task);
        }
    }
}

/* every spark lasts SPARK_LIFE steps from its throw, so the sparks run out
 * oldest first. the step drops every spark up to the last that ran out */
static void spark_point_step(struct task *task)
{
    struct spark_point *spark;
    NJS_POINT3 *ahead;
    int left;
    int i;
    int last_out;
    int end;
    int first;
    Float x;
    Float y;
    Float z;
    Float vx;
    Float vy;
    Float vz;
    Float k;

    if (spark_point_ring == NULL) {
        task_destroy(task);
        return;
    }
    left = spark_point_ring->count;
    i = spark_point_ring->first;
    last_out = -1;
    while (left--) {
        spark = &spark_point_ring->sparks[i];
        GET_XYZ(&spark->spd, vx, vy, vz);
        GET_XYZ(&spark->pos[0], x, y, z);
        vx *= spark_point_drag;
        vy = (vy * spark_point_drag) - spark_point_gravity;
        vz *= spark_point_drag;
        x += vx;
        y += vy;
        z += vz;
        if (y < spark->floor_y) {
            vy = njAbs(vy) * spark_point_bounce;
            y = spark->floor_y;
        }
        SET_XYZ(&spark->spd, vx, vy, vz);
        SET_XYZ(&spark->pos[0], x, y, z);
        /* @hack spelling: a loop over the one point ahead, where its body
         * alone would do */
        for (ahead = &spark->pos[1]; ahead < &spark->pos[2]; ahead++) {
            k = spark_point_drag * AHEAD_DRAG;
            vx *= k;
            /* @hack spelling: y's drag worked out again, where k would do */
            vy = (vy * spark_point_drag * AHEAD_DRAG) - spark_point_gravity;
            /* @hack spelling: vz = vz * k, where *= would do */
            vz = vz * k;
            x += vx;
            y += vy;
            z += vz;
            if (y < spark->floor_y) {
                vy = njAbs(vy) * spark_point_bounce;
                y = spark->floor_y;
            }
            SET_XYZ(ahead, x, y, z);
        }
        spark->life--;
        if (spark->life <= 0) {
            last_out = i;
        }
        if (++i >= RING_SLOTS) {
            i -= RING_SLOTS;
        }
    }
    if (last_out >= 0) {
        first = spark_point_ring->first;
        end = spark_point_ring->count + first;
        i = (last_out + 1) % RING_SLOTS;
        /* @hack spelling: first stored in both arms, where one store before
         * the test would do */
        if (i < first) {
            spark_point_ring->first = i;
            i += RING_SLOTS;
        } else {
            spark_point_ring->first = i;
        }
        /* @hack spelling: i reused for the count, where count = end - i
         * would do */
        i = end - i;
        spark_point_ring->count = i;
    }
    if (spark_point_ring->count == 0) {
        task_destroy(task);
    }
}

static void spark_point_draw(struct task *task)
{
    struct spark_point *spark;
    int left;
    int i;

    if (spark_point_ring != NULL) {
        left = spark_point_ring->count;
        i = spark_point_ring->first;
        while (left--) {
            spark = &spark_point_ring->sparks[i];
            i++;
            if (i >= RING_SLOTS) {
                i -= RING_SLOTS;
            }
            line_queue(spark->pos, spark_point_colors, POINT_OPAQUE);
        }
    }
}

static void spark_point_destroy(struct task *task)
{
    if (spark_point_ring != NULL) {
        syFree(spark_point_ring);
        spark_point_ring = NULL;
    }
    if (spark_point_task == task) {
        spark_point_task = NULL;
    }
}
