/* sparks: tasks that fly from a point, fall and end */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "draw/lines.h"
#include "shared/spark.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define SPARK_LEVEL 3 /* the task level of a spark */

/* a spark starts within SCATTER / 2 of pos on each axis */
#define SCATTER 0.5F
/* a spark thrown along spd adds a random part to it. the part's x and z each
 * lie within THROWN_SPREAD / 2 either way. the part's y lies from 0 up to
 * THROWN_RISE */
#define THROWN_SPREAD 1.5F
#define THROWN_RISE 0.5F
/* a spark thrown without spd takes a random speed. the speed's x and z each
 * lie within FREE_SPREAD / 2 either way. the speed's y lies from 0 up to
 * FREE_RISE. the spark lasts FREE_STEPS steps longer */
#define FREE_SPREAD 2.6F
#define FREE_RISE 1.7F
#define FREE_STEPS 30

/* the share of world_gravity that each step adds to a spark's speed */
#define GRAVITY_SHARE 0.02F
/* a spark draws LEAD from its pos, along its speed */
#define LEAD 0.6F

/* the sparks of a throw take ORANGE_RED, ORANGE, ORANGE_RED and DULL_RED in
 * turn. no spark takes BLUE */
#define ORANGE_RED 0xffe03000
#define ORANGE 0xffff6010
#define DULL_RED 0xffa04030
#define BLUE 0xff1050ff
/* the steps each color adds to its spark's life */
#define ORANGE_RED_STEPS 12
#define ORANGE_STEPS 15
#define DULL_RED_STEPS 18
#define BLUE_STEPS 18

/* a spark's record, in its task's extra */
struct spark {
    Sint16 unk_00;
    Sint16 timer; /* the steps the spark moves before it ends */
    NJS_COLOR color;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
};

static void spark_step(struct task *task)
{
    struct spark *spark = task->extra.p;
    NJS_VECTOR gravity;

    if (game_state.time_stopped) {
        return;
    }
    if (spark->timer-- <= 0) {
        task_kill(task);
        return;
    }
    njAddVector(&spark->pos, &spark->spd);
    gravity = world_gravity;
    gravity.x *= GRAVITY_SHARE;
    gravity.y *= GRAVITY_SHARE;
    gravity.z *= GRAVITY_SHARE;
    njAddVector(&spark->spd, &gravity);
}

/* draw the spark as a point. its spd must not be zero. njUnitVector divides a
 * copy of it by its length, which it doesn't test for 0 */
static void spark_draw(struct task *task)
{
    struct spark *spark = task->extra.p;
    /* two points, though a spark is one. where line_queue can't queue the
     * point, it draws it now, and that draw reads two points */
    NJS_POINT3 points[2];
    /* @hack redundant: a second color, which line_queue reads only for a
     * line */
    NJS_COLOR colors[2];

    /* @hack spelling: one assignment to colors[0] and colors[1], where a
     * statement for each would do */
    colors[0] = colors[1] = spark->color;
    points[0] = spark->spd;
    /* @hack redundant: points[1] set, though only a line's draw uses its
     * value */
    points[1] = spark->pos;
    njUnitVector(points);
    /* @hack spelling: points->x and the rest, where points[0].x would do */
    points->x *= LEAD;
    points->y *= LEAD;
    points->z *= LEAD;
    njAddVector(points, &spark->pos);
    line_queue(points, colors, POINT_ADDITIVE);
}

void spark_throw(int count, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    struct task *task;
    struct spark *spark;
    int i;

    for (i = 0; i < count; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((task = task_create(0, SPARK_LEVEL, spark_step, "SparkExecute")) ==
            NULL) {
            break;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((spark = syCalloc(1, sizeof(struct spark))) == NULL) {
            task_kill(task);
            break;
        }
        spark->pos = *pos;
        spark->pos.x += SCATTER * (0.5F - RANDOM_UNIT());
        spark->pos.y += SCATTER * (0.5F - RANDOM_UNIT());
        spark->pos.z += SCATTER * (0.5F - RANDOM_UNIT());
        if (spd != NULL) {
            spark->spd = *spd;
            spark->spd.x += THROWN_SPREAD * (0.5F - RANDOM_UNIT());
            spark->spd.y += rand() * (THROWN_RISE * RAND_SCALE);
            spark->spd.z += THROWN_SPREAD * (0.5F - RANDOM_UNIT());
            /* @hack redundant: the timer cleared, though syCalloc zeroed the
             * record */
            spark->timer = 0;
        } else {
            spark->spd.x = (0.5F - RANDOM_UNIT()) * FREE_SPREAD;
            spark->spd.y = rand() * (FREE_RISE * RAND_SCALE);
            spark->spd.z = (0.5F - RANDOM_UNIT()) * FREE_SPREAD;
            spark->timer = FREE_STEPS;
        }
        switch (i & 3) {
        case 0:
        case 2:
            spark->color.color = ORANGE_RED;
            spark->timer += ORANGE_RED_STEPS;
            break;
        case 1:
            spark->color.color = ORANGE;
            spark->timer += ORANGE_STEPS;
            break;
        case 3:
            spark->color.color = DULL_RED;
            spark->timer += DULL_RED_STEPS;
            break;
        /* @hack redundant: a default no i & 3 reaches */
        default:
            spark->color.color = BLUE;
            spark->timer += BLUE_STEPS;
            break;
        }
        task->extra.p = spark;
        task->draw = spark_draw;
    }
}
