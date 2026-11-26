/* a kart's invincibility: a ball about the kart that throws off sparks. it
 * lasts while the kart's invincible isn't 0 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "effect/barrier.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "cart/vehicle.h"
#include "effect/texlist.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the index in cart_racers of the racer whose kart the ball or a spark
 * follows. it is kept in the work's submode */
#define RACER(work) ((work)->submode)
#define BALL_SCALE 3
/* a spark counts its steps in its tick. the step that finds the tick at
 * SPARK_STEPS kills it. it cycles through SPARK_TEXTURES, one a step */
#define SPARK_STEPS 16
#define SPARK_TEXTURES 8

/* a texture list of the one texture at index in effect_texnames */
#define ONE_TEXTURE(index) {&effect_texnames[index], 1}
static NJS_TEXLIST spark_textures[SPARK_TEXTURES] = {
    ONE_TEXTURE(EFFECT_MUTEKI01 + 0), ONE_TEXTURE(EFFECT_MUTEKI01 + 1),
    ONE_TEXTURE(EFFECT_MUTEKI01 + 2), ONE_TEXTURE(EFFECT_MUTEKI01 + 3),
    ONE_TEXTURE(EFFECT_MUTEKI01 + 4), ONE_TEXTURE(EFFECT_MUTEKI01 + 5),
    ONE_TEXTURE(EFFECT_MUTEKI01 + 6), ONE_TEXTURE(EFFECT_MUTEKI01 + 7),
};
static NJS_TEXLIST ball_texture = ONE_TEXTURE(EFFECT_MUTEKIBALL);
static Float spark_scale = 1;
static void ball_draw(struct task *task);
static void ball_step(struct task *task);
static void spark_step(struct task *task);
static void spark_draw(struct task *task);

struct task *cart_create_invincibility(int player)
{
    struct task *task;
    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((task = task_create(TASK_WORK, 3, ball_step, "Muteki")) != NULL) {
        RACER(task->work) = player;
        task->draw = ball_draw;
    }
    return task;
}
static void ball_draw(struct task *task)
{
    struct task_work *work;
    Float scale;
    work = task->work;
    work->pos = cart_racers[RACER(work)]->work->pos;
    work->scl.x = BALL_SCALE;
    njFogDisable();
    njPushMatrixEx();
    njSetTexture(&ball_texture);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    scale = work->scl.x;
    njScale(NULL, scale, scale, scale);
    njCnkSimpleDrawModel(&invincibility_ball_model);
    njPopMatrixEx();
    njFogEnable();
}
static void ball_step(struct task *task)
{
    struct task_work *work;
    struct cart_vehicle *kart;
    struct task *child;
    struct task_work *spark;
    int count;
    work = task->work;
    kart = cart_racers[RACER(work)]->motion.p;
    if (!kart->invincible) {
        task_kill(task);
        return;
    }
    work->timer++;
    /* @hack spelling: count counted down from the sparks to make, where a
     * count up would do */
    for (count = (work->timer & 1) + 1; count > 0; count--) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = task_create_child(TASK_WORK, spark_step, task)) != NULL) {
            spark = child->work;
            spark->ang.x = rand() * (FULL_TURN * RAND_SCALE);
            spark->ang.y = rand() * (FULL_TURN * RAND_SCALE);
            RACER(spark) = RACER(work);
            child->draw = spark_draw;
        }
    }
}
static void spark_step(struct task *task)
{
    struct task_work *work = task->work;
    if (work->tick >= SPARK_STEPS) {
        task_kill(task);
        return;
    }
    work->tick++;
}
/* the first of the ball's sparks turns the fog off, and the last turns it on */
static void spark_draw(struct task *task)
{
    struct task_work *work;
    Float scale;
    /* @hack spelling: frame, a local for the texture's index, where the index
     * in place would do */
    int frame;
    work = task->work;
    work->pos = cart_racers[RACER(work)]->work->pos;
    work->scl.x = spark_scale;
    if (task->parent->child == task) {
        njFogDisable();
    }
    njPushMatrixEx();
    frame = work->tick & (SPARK_TEXTURES - 1);
    njSetTexture(&spark_textures[frame]);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    scale = work->scl.x;
    njScale(NULL, scale, scale, scale);
    njCnkEasyDrawModel(&invincibility_spark_model);
    njPopMatrixEx();
    if (task->parent->child->prev == task) {
        njFogEnable();
    }
}
