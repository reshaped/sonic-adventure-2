#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg27/spark_emitter/text.inc"
#include "object/set.h"
#include "stg27/colony_spark.h"
#include "system/task.h"
#include "system/util.h"

/* the effect in the low byte of ang.x. EFFECT_SPARK alone throws anything */
#define KIND(work) ((Uint8)(work)->ang.x)
#define EFFECT_SPARK 0
/* the chance of a throw on each step, out of CHANCE_MAX */
#define CHANCE(work) ((Uint8)((work)->ang.x >> 8))
#define CHANCE_MAX 255

#define SCALE_BIAS 0.05F
#define SCALE_MAX 20.0F
/* VARIED's factor runs from FACTOR_LEAST up to FACTOR_LEAST + FACTOR_RANDOM */
#define FACTOR_LEAST 0.9F
#define FACTOR_RANDOM 0.2F
#define VARIED(scale)                                                          \
    ((scale) * (FACTOR_LEAST + (rand() * (FACTOR_RANDOM * RAND_SCALE))))

/* the set editor's labels for the emitter's ang.x to ang.z and scl.x to
 * scl.z */
static const char editor_effect[20] = "<- EFF[  ]/RND[   ]";
static const char editor_angle_y[12] = "<- ANGLE Y";
static const char editor_angle_z[12] = "<- ANGLE Z";
static const char editor_speed[8] = "<- SPD";
static const char editor_spread[12] = "<- SPDRND";
static const char editor_scale[8] = "<- SCL";
/* the effects' names in the set editor, by KIND */
static const char *emitter_effects[1] = {
#include "stg27/spark_emitter/emitter_effects.inc"
};

NJS_TEXLIST *colony_spark_texlists[2] = {
#include "stg27/spark_emitter/colony_spark_texlists.inc"
};

static void emitter_destroy(struct task *task);
static void emitter_step(struct task *task);

void colony_spark_emitter_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->step = emitter_step;
    task->destructor = emitter_destroy;
}

static void emitter_destroy(struct task *task)
{
}

static void emitter_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd;
    Float scale;
    /* @hack spelling: chance, a local for the throw's chance, where
     * CHANCE(work) worked out in the test would do */
    Float chance;

    if (placement_try_leave(task)) {
        return;
    }
    chance = CHANCE(work) * (1.0F / CHANCE_MAX);
    if (chance > RANDOM_UNIT()) {
        spd.y = work->scl.x + ((RANDOM_UNIT() - 0.5F) * work->scl.y);
        spd.x = (RANDOM_UNIT() - 0.5F) * work->scl.y;
        spd.z = (RANDOM_UNIT() - 0.5F) * work->scl.y;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njCalcVector(NULL, &spd, &spd);
        njPopMatrix(1);
        scale = work->scl.z + SCALE_BIAS;
        if (scale > SCALE_MAX) {
            scale = SCALE_MAX;
        }
        switch (KIND(work)) {
        case EFFECT_SPARK:
            colony_spark_create(&work->pos, &spd, VARIED(scale));
            /* @hack redundant: a block no path reaches, a stand-in that gives
             * the step the image's registers */
            if (0) {
                task_destroy(NULL);
                task_destroy(NULL);
                task_destroy(NULL);
                work->scl.z = (Float)rand();
            }
            break;
        }
    }
}
