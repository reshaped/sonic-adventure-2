/* sprite sparks: sparks that fly, fall and bounce, each drawn as a trail of
 * sprites. one task, SpSparkExec, moves and draws them all */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "shared/spark_sprite.h"
#include "shared/spark_xyz.h"
#include "system/task.h"
#include "system/util.h"

/* the ring buffer's slots. it holds at most RING_SLOTS - 1 sparks */
#define RING_SLOTS 255
#define SPARK_LIFE 80 /* a spark's life, in steps */
#define SPARK_LEVEL 2 /* the task level of SpSparkExec */
/* a spark's points: pos[0], where it lies, then guesses at its next five
 * positions */
#define SPARK_POINTS 6
/* each guess slows the speed by the drag times AHEAD_DRAG, where the spark
 * slows by the drag alone */
#define AHEAD_DRAG 0.97F
/* point i's sprite has the size (i + 2) / SPARK_POINTS times spark_sprite_size.
 * no sprite is smaller than SPRITE_LEAST. the sprites shade from black at point
 * 0 to white at the last. each is SHADE_STEP lighter than the one before in
 * red, green and blue */
#define SPRITE_LEAST 0.023F
#define SHADE_STEP 51
#define SPRITE_OPAQUE 0xff000000

struct spark_sprite {
    NJS_POINT3 pos[SPARK_POINTS];
    NJS_VECTOR spd;
    Float floor_y;
    Sint32 life;
};

/* the ring buffer: count sparks, oldest first, from sparks[first] on and
 * wrapping round to sparks[0] */
struct spark_sprite_ring {
    struct spark_sprite sparks[RING_SLOTS];
    Sint32 first;
    Sint32 count;
};

static void spark_sprite_step(struct task *task);
static void spark_sprite_draw(struct task *task);
static void spark_sprite_destroy(struct task *task);

#include "shared/spark_sprite/text.inc"

static NJS_TEXNAME spark_sprite_texname[1] = {
#include "shared/spark_sprite/spark_sprite_texname.inc"
};
NJS_TEXLIST spark_sprite_texlist = {spark_sprite_texname, 1};
static Float spark_sprite_drag = 0.98F;
static Float spark_sprite_bounce = 0.7F;
static Float spark_sprite_gravity = 0.08F;
static Float spark_sprite_size = 0.06F;
static Float dat_spark_sprite_0 = 2.0F;
/* the ring buffer, or null while no spark needs one */
static struct spark_sprite_ring *spark_sprite_ring = NULL;
static struct task *spark_sprite_task = NULL; /* SpSparkExec, or null */
/* the tasks made, less those destroyed */
static Sint32 spark_sprite_tasks = 0;

void spark_sprite_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float spread,
                         Float floor_y)
{
    struct task *task;
    struct spark_sprite *spark;
    int index;
    int i;
    Float vx;
    Float vy;
    Float vz;
    Float x;
    Float y;
    Float z;

    task = spark_sprite_task;
    if (task == NULL) {
        task = task_create(0, SPARK_LEVEL, spark_sprite_step, "SpSparkExec");
        if (task != NULL) {
            task->draw = spark_sprite_draw;
            /* @hack redundant: the step stored again, though task_create
             * gave it */
            task->step = spark_sprite_step;
            task->destructor = spark_sprite_destroy;
            spark_sprite_task = task;
            spark_sprite_tasks++;
        }
    }
    if (spark_sprite_task != NULL && spark_sprite_ring == NULL) {
        spark_sprite_ring = syCalloc(1, sizeof(struct spark_sprite_ring));
    }
    if (spark_sprite_task != NULL && spark_sprite_ring != NULL) {
        index = spark_sprite_ring->count;
        if (index >= RING_SLOTS - 1) {
            spark_sprite_ring->first =
                (spark_sprite_ring->first + 1) % RING_SLOTS;
        } else {
            index++;
        }
        spark_sprite_ring->count = index;
        index += spark_sprite_ring->first;
        index--;
        if (index < 0) {
            index = RING_SLOTS - 1;
        }
        index = index % RING_SLOTS;
        spark = &spark_sprite_ring->sparks[index];
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
        for (i = 0; i < SPARK_POINTS; i++) {
            spark->pos[i].x = x;
            spark->pos[i].y = y;
            spark->pos[i].z = z;
        }
        spark->floor_y = floor_y;
        spark->life = SPARK_LIFE;
    } else {
        if (spark_sprite_ring != NULL) {
            syFree(spark_sprite_ring);
            spark_sprite_ring = NULL;
        }
        if (spark_sprite_task != NULL) {
            task_destroy(spark_sprite_task);
        }
    }
}

/* every spark lasts SPARK_LIFE steps from its throw, so the sparks run out
 * oldest first. the step drops every spark up to the last that ran out */
static void spark_sprite_step(struct task *task)
{
    struct spark_sprite *spark;
    NJS_POINT3 *ahead;
    int left;
    int i;
    int last_out;
    int end;
    Float x;
    Float y;
    Float z;
    Float vx;
    Float vy;
    Float vz;

    if (spark_sprite_ring == NULL) {
        task_destroy(task);
        return;
    }
    left = spark_sprite_ring->count;
    i = spark_sprite_ring->first;
    last_out = -1;
    while (left--) {
        spark = &spark_sprite_ring->sparks[i];
        GET_XYZ(&spark->spd, vx, vy, vz);
        GET_XYZ(&spark->pos[0], x, y, z);
        vx *= spark_sprite_drag;
        vy = (vy * spark_sprite_drag) - spark_sprite_gravity;
        vz *= spark_sprite_drag;
        x += vx;
        y += vy;
        z += vz;
        if (y < spark->floor_y) {
            vy = njAbs(vy) * spark_sprite_bounce;
            y = spark->floor_y;
        }
        SET_XYZ(&spark->spd, vx, vy, vz);
        SET_XYZ(&spark->pos[0], x, y, z);
        /* @hack spelling: ahead stepped by an assignment, where ahead++
         * would do */
        for (ahead = &spark->pos[1]; ahead < &spark->pos[SPARK_POINTS];
             ahead = ahead + 1) {
            vx *= spark_sprite_drag * AHEAD_DRAG;
            vy = (vy * spark_sprite_drag * AHEAD_DRAG) - spark_sprite_gravity;
            vz *= spark_sprite_drag * AHEAD_DRAG;
            x += vx;
            y += vy;
            z += vz;
            if (y < spark->floor_y) {
                vy = njAbs(vy) * spark_sprite_bounce;
                y = spark->floor_y;
            }
            SET_XYZ(ahead, x, y, z);
        }
        spark->life--;
        if (spark->life <= 0) {
            last_out = i;
        }
        i = (i + 1) % RING_SLOTS;
    }
    if (last_out >= 0) {
        end = spark_sprite_ring->first + spark_sprite_ring->count;
        i = (last_out + 1) % RING_SLOTS;
        /* @hack spelling: first stored in both arms, where one store before
         * the test would do */
        if (i < spark_sprite_ring->first) {
            spark_sprite_ring->first = i;
            i += RING_SLOTS;
        } else {
            spark_sprite_ring->first = i;
        }
        /* @hack spelling: i reused for the count, where count = end - i
         * would do */
        i = end - i;
        spark_sprite_ring->count = i;
    }
    if (spark_sprite_ring->count == 0) {
        task_destroy(task);
    }
}

static void spark_sprite_draw(struct task *task)
{
    struct spark_sprite *spark;
    int left;
    int index;
    int i;
    int shade;
    Float size;

    if (spark_sprite_ring == NULL) {
        return;
    }
    left = spark_sprite_ring->count;
    index = spark_sprite_ring->first;
    while (left--) {
        spark = &spark_sprite_ring->sparks[index];
        index = (index + 1) % RING_SLOTS;
        size = spark_sprite_size * (1.0F / SPARK_POINTS);
        shade = 0;
        for (i = 0; i < SPARK_POINTS; i++) {
            size += spark_sprite_size * (1.0F / SPARK_POINTS);
            njSetTexture(&spark_sprite_texlist);
            njPtclSpriteStart(
                0, SPRITE_OPAQUE + (shade << 16) + (shade << 8) + shade, 0);
            if (size < SPRITE_LEAST) {
                njPtclDrawSprite(&spark->pos[i], 1, SPRITE_LEAST, SPRITE_LEAST);
            } else {
                njPtclDrawSprite(&spark->pos[i], 1, size, size);
            }
            njPtclSpriteEnd();
            shade += SHADE_STEP;
        }
    }
}

static void spark_sprite_destroy(struct task *task)
{
    if (spark_sprite_ring != NULL) {
        syFree(spark_sprite_ring);
        spark_sprite_ring = NULL;
    }
    if (spark_sprite_task == task) {
        spark_sprite_task = NULL;
    }
    spark_sprite_tasks--;
}
