#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg27/colony_spark/text.inc"
#include "effect/particle.h"
#include "object/set.h"
#include "stg27/colony_spark.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for the spark's ang.z, scl.x and scl.y */
static const char editor_rotation[8] = "<- ROT";
static const char editor_animation_speed[12] = "<- ANIMSPD";
static const char editor_scale[8] = "<- SCL";

#define SPARK_FRAMES 12
#define WHITE 0xffffffff
#define HALF_SIZE_LEAST 1.0F /* SPARK's half size where scl.y is 0 */
/* SPARK's frame, a Float in the bits of task->unk_3c. its whole part numbers
 * the texture it shows */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FRAME(task) (*(Float *)&(task)->unk_3c)

static int spark_move(struct particle_group *group, struct particle *spark);
static void spark_destroy(struct task *task);
static void spark_step(struct task *task);
static void spark_draw(struct task *task);

static NJS_TEXNAME textures[SPARK_FRAMES] = {
#include "stg27/colony_spark/textures.inc"
};
NJS_TEXLIST colony_spark_texlist = {
#include "stg27/colony_spark/colony_spark_texlist.inc"
};
static struct particle_group spark_group = {
#include "stg27/colony_spark/spark_group.inc"
};

struct particle *colony_spark_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                     Float scale)
{
    struct particle *spark = particle_create(&spark_group);

    if (spark != NULL) {
        spark->pos = *pos;
        spark->spd = *spd;
        spark->scale = scale;
        spark->frame += rand() * RAND_SCALE;
    }
    return spark;
}

/* the stage's own copy of particle_move */
static int spark_move(struct particle_group *group, struct particle *spark)
{
    spark->ang += spark->ang_spd;
    spark->scale += group->scale_step;
    spark->pos.x += spark->spd.x;
    spark->pos.y += spark->spd.y;
    spark->pos.z += spark->spd.z;
    spark->spd.x *= group->friction;
    spark->spd.y = (spark->spd.y * group->friction) + group->gravity;
    spark->spd.z *= group->friction;
    spark->frame += group->frame_step;
    /* @hack type: the frame compared through a Sint16, where
     * spark->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)spark->frame >= group->frames) {
        spark->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

void colony_spark_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = spark_draw;
    task->step = spark_step;
    task->destructor = spark_destroy;
}

static void spark_destroy(struct task *task)
{
}

static void spark_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, a stand-in that gives the
     * step the image's registers */
    if (0) {
        task_destroy(NULL);
        task_destroy(NULL);
        task_destroy(NULL);
    }
    FRAME(task) += work->scl.x;
    if (FRAME(task) >= SPARK_FRAMES || FRAME(task) < 0.0F) {
        FRAME(task) = 0.0F;
    }
}

static void spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    int frame;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    frame = (int)FRAME(task);
    if (frame >= SPARK_FRAMES) {
        frame = SPARK_FRAMES - 1;
    }
    njSetTexture(&colony_spark_texlist);
    njFogDisable();
    particle_draw_quad(&work->pos, frame, QUARTER_TURN, work->ang.z,
                       work->scl.y + HALF_SIZE_LEAST,
                       work->scl.y + HALF_SIZE_LEAST, WHITE, 0, 0.0F, 0.0F);
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}
