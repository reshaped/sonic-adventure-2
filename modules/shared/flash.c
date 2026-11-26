/* the flash: a particle that shows eight textures in turn, then ends. a
 * FlashManager task makes one and throws sparks from it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/burst.h"
#include "effect/particle.h"
#include "shared/flash.h"
#include "shared/flash/text.inc"
#include "shared/spark_sprite.h"
#include "system/task.h"
#include "system/util.h"

#define FLASH_FRAMES 8
#define FRAME_STEP 0.4F
/* a FlashManager counts frames as its flash does. it throws sparks until its
 * count passes SPRAY_FRAMES */
#define SPRAY_FRAMES 7.0F

#define MANAGER_LEVEL 3
/* the sparks flash_manager_create and flash_manager_create_on throw a step */
#define FLASH_SPARKS 3

/* SPARK_SPEED(least) is random, from least up to least + SPARK_SPREAD. a
 * spark takes SPARK_LEAST along x and z, and SPARK_LEAST_Y along y. so
 * spark_sprite_create adds NO_SPREAD of its own */
#define SPARK_SPREAD 5.5F
#define NO_SPREAD 0.0F
#define SPARK_LEAST (-2.75F)
#define SPARK_LEAST_Y (-2.0F)
#define SPARK_SCALE 0.05F
#define SPARK_SPEED(least)                                                     \
    (((Float)rand() * (SPARK_SPREAD * RAND_SCALE)) + (least))
#define SPARK_FLOOR (-100000.0F)

enum flash_manager_mode { MODE_START, MODE_SPRAY, MODE_END };

/* a FlashManager's work keeps its flash's scale and depth_bias, and the
 * sparks it throws a step */
#define SCALE(work) ((work)->scl.x)
#define DEPTH_BIAS(work) ((work)->scl.y)
#define SPARKS(work) ((work)->tick)
/* the frames a FlashManager has counted, a Float kept in the bits of
 * task->motion. the destructor clears motion, so that task_destroy doesn't
 * free the Float as a pointer */
/* @hack type: motion read as a Float through its address, where
 * (task)->motion.f would do */
#define FRAMES(task) (*(Float *)&(task)->motion)

static int flash_move(struct particle_group *group, struct particle *particle);
static void flash_manager_step(struct task *task);

static NJS_TEXNAME flash_texnames[FLASH_FRAMES] = {
#include "shared/flash/flash_texnames.inc"
};
NJS_TEXLIST flash_texlist = {
#include "shared/flash/flash_texlist.inc"
};

/* clang-format off */
static struct particle_group flash_group = {
    PARTICLE_BLEND_ADD | PARTICLE_DEPTH, &flash_texlist, 0, FLASH_FRAMES,
    FRAME_STEP, 1.0F, 0.0F, 0.0F, flash_move, 75000.0F, NULL, 0, NULL, NULL,
};
/* clang-format on */

struct particle *flash_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale,
                              Float depth_bias)
{
    struct particle *flash;

    flash = particle_create(&flash_group);
    if (flash != NULL) {
        flash->pos = *pos;
        flash->spd = *spd;
        flash->scale = scale;
        /* @hack type: the random angle worked out as a Float, where
         * rand() * 2 would do */
        flash->ang = (Angle)RANDOM_TURN();
        flash->depth_bias = depth_bias;
    }
    return flash;
}

/* particle_move, but a frame that reaches the group's frames stays there.
 * particle_move sets it back to frames - 1 */
static int flash_move(struct particle_group *group, struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        return 0;
    }
    return 1;
}

static void flash_manager_begin(struct task_work *work)
{
    NJS_VECTOR spd;

    spd.x = 0.0F;
    spd.y = 0.0F;
    spd.z = 0.0F;
    flash_create(&work->pos, &spd, SCALE(work), DEPTH_BIAS(work));
    work->mode = MODE_SPRAY;
}

static void flash_manager_destroy(struct task *task)
{
    task->motion.p = NULL;
}

static void flash_manager_spray(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd;
    int i;

    FRAMES(task) += FRAME_STEP;
    if (FRAMES(task) > SPRAY_FRAMES) {
        work->mode = MODE_END;
    } else {
        for (i = 0; i < SPARKS(work); i++) {
            spd.x = SPARK_SPEED(SPARK_LEAST) * SCALE(work) * SPARK_SCALE;
            spd.y = SPARK_SPEED(SPARK_LEAST_Y) * SCALE(work) * SPARK_SCALE;
            spd.z = SPARK_SPEED(SPARK_LEAST) * SCALE(work) * SPARK_SCALE;
            spark_sprite_create(&work->pos, &spd, NO_SPREAD, SPARK_FLOOR);
        }
    }
}

static void flash_manager_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_START:
        FRAMES(task) = 0.0F;
        task->destructor = flash_manager_destroy;
        flash_manager_begin(work);
        break;
    case MODE_SPRAY:
        flash_manager_spray(task);
        break;
    case MODE_END:
        task_kill(task);
        break;
    }
}

/* make a FlashManager at parent's pos, as flash_manager_create does. nothing
 * calls flash_manager_create_on, and no header declares it. it is external
 * only because SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void flash_manager_create_on(struct task_work *parent, Float scale)
{
    static NJS_VECTOR still = {0.0F, 0.0F, 0.0F};
    struct task *task;
    struct task_work *work;

    if (burst_hook != NULL) {
        burst_hook(&parent->pos, &still, scale, 0.0F);
    }
    task = task_create(TASK_WORK, MANAGER_LEVEL, flash_manager_step,
                       "FlashManager");
    if (task != NULL) {
        work = task->work;
        work->pos = parent->pos;
        SCALE(work) = scale;
        SPARKS(work) = FLASH_SPARKS;
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

void flash_manager_create(Float x, Float y, Float z, Float scale)
{
    static NJS_VECTOR still = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 pos;
    struct task *task;
    struct task_work *work;

    if (burst_hook != NULL) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        burst_hook(&pos, &still, scale, 0.0F);
    }
    task = task_create(TASK_WORK, MANAGER_LEVEL, flash_manager_step,
                       "FlashManager");
    if (task != NULL) {
        work = task->work;
        work->pos.x = x;
        work->pos.y = y;
        work->pos.z = z;
        SCALE(work) = scale;
        SPARKS(work) = FLASH_SPARKS;
    }
}

void flash_manager_create_sparks(Float x, Float y, Float z, Float scale,
                                 Uint8 sparks)
{
    static NJS_VECTOR still = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 pos;
    struct task *task;
    struct task_work *work;

    if (burst_hook != NULL) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        burst_hook(&pos, &still, scale, 0.0F);
    }
    task = task_create(TASK_WORK, MANAGER_LEVEL, flash_manager_step,
                       "FlashManager");
    if (task != NULL) {
        work = task->work;
        work->pos.x = x;
        work->pos.y = y;
        work->pos.z = z;
        SCALE(work) = scale;
        SPARKS(work) = sparks;
    }
}

void flash_manager_create_sparks_depth(Float x, Float y, Float z, Float scale,
                                       Uint8 sparks, Float depth_bias)
{
    static NJS_VECTOR still = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 pos;
    struct task *task;
    struct task_work *work;

    if (burst_hook != NULL) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        burst_hook(&pos, &still, scale, 0.0F);
    }
    task = task_create(TASK_WORK, MANAGER_LEVEL, flash_manager_step,
                       "FlashManager");
    if (task != NULL) {
        work = task->work;
        work->pos.x = x;
        work->pos.y = y;
        work->pos.z = z;
        SCALE(work) = scale;
        SPARKS(work) = sparks;
        DEPTH_BIAS(work) = depth_bias;
    }
}
