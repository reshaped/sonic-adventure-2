/* the circle of flowers that grows around a Chao calling a mate. the Chao
 * sets calling_mate on each step it calls, and the circle clears it. on a step
 * that finds it clear, the flowers shrink away, and the circle ends after
 * them */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/egg.h"
#include "chao/flower_models.h"
#include "chao/mating.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "collision/surface.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a circle's ang.x: whether its Chao called on the circle's last step */
#define CALLED(work) ((work)->ang.x)
/* a flower's submode: 0 for mating_flower_a, else mating_flower_b */
#define FLOWER_MODEL(work) ((work)->submode)

/* a flower's scl.x grows GROW_STEP a step, up to 1, while its Chao calls. it
 * shrinks SHRINK_STEP a step after. it draws at FLOWER_SCALE times scl.x */
#define GROW_STEP 0.1F
#define SHRINK_STEP 0.03F
#define FLOWER_SCALE 0.5F

enum circle_mode { PLANTING, PLANTED };
/* a flower stands from INNER_SHARE to INNER_SHARE + SHARE_SPAN of
 * flower_radius from the circle's center. the floor under it is probed from
 * PROBE_HEIGHT above the center's height */
/* the planting turns by a whole turn, DEGREES_PER_TURN, over flower_count
 * flowers */
#define DEGREES_PER_TURN 360.0F
#define INNER_SHARE 0.9F
#define SHARE_SPAN 0.1F
#define PROBE_HEIGHT 2.0F

static void flower_step(struct task *flower)
{
    struct task_work *work = flower->work;
    struct task *circle = flower->parent;
    if (CALLED(circle->work)) {
        work->scl.x += GROW_STEP;
        if (work->scl.x > 1.0F) {
            work->scl.x = 1.0F;
        }
    } else {
        work->scl.x -= SHRINK_STEP;
        if (work->scl.x < 0.0F) {
            work->scl.x = 0.0F;
            task_kill(flower);
        }
    }
}

static void flower_draw(struct task *flower)
{
    struct task_work *work = flower->work;
    Float scale = work->scl.x * FLOWER_SCALE;
    njSetTexture(&egg_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, scale, scale, scale);
    switch (FLOWER_MODEL(work)) {
    case 0:
        njCnkEasyDrawObject(&mating_flower_a);
        break;
    default:
        njCnkEasyDrawObject(&mating_flower_b);
        break;
    }
    njPopMatrixEx();
}

static void flower_destroy(struct task *flower)
{
}

static struct task *plant_flower(struct task *circle,
                                 const NJS_POINT3 *position)
{
    struct task_work *work;
    struct task *flower;
    flower = task_create_child(TASK_WORK, flower_step, circle);
    flower->draw = flower_draw;
    flower->destructor = flower_destroy;
    work = flower->work;
    work->pos = *position;
    work->ang.y = (Angle)RANDOM_TURN();
    if (rand() * RAND_SCALE < 0.5F) {
        FLOWER_MODEL(work) = 0;
    } else {
        FLOWER_MODEL(work) = 1;
    }
    return flower;
}

static void circle_step(struct task *circle)
{
    struct task_work *work = circle->work;
    NJS_POINT3 position;
    struct angles floor;
    Angle yaw;
    Float radius;
    NJS_POINT3 *center;
    CALLED(work) = 0;
    switch (work->mode) {
    case PLANTING:
        if (work->timer++ > chao_tuning.flower_interval) {
            center = &work->pos;
            yaw = work->ang.y;
            radius = chao_tuning.flower_radius *
                     ((rand() * (SHARE_SPAN * RAND_SCALE)) + INNER_SHARE);
            work->ang.y -=
                (Angle)((DEGREES_PER_TURN / chao_tuning.flower_count) *
                        ANGLE_PER_DEGREE);
            position.x = center->x + (radius * njSin(yaw));
            position.y = center->y + PROBE_HEIGHT;
            position.z = center->z + (radius * njCos(yaw));
            position.y =
                surface_probe_floor(position.x, position.y, position.z, &floor);
            plant_flower(circle, &position);
            work->tick++;
            if (work->tick >= chao_tuning.flower_count) {
                work->mode = PLANTED;
                work->submode = 0;
            }
            work->timer = 0;
        }
        break;
    case PLANTED:
        break;
    }
    if (((struct chao_work *)circle->parent->work)->calling_mate) {
        ((struct chao_work *)circle->parent->work)->calling_mate = 0;
        CALLED(work) = 1;
    } else {
        circle->step = task_destroy_children;
    }
}
static void circle_destroy(struct task *circle)
{
}

struct task *chao_create_mating_flowers(struct task *chao)
{
    struct task *circle = task_create_child(TASK_WORK, circle_step, chao);
    if (circle) {
        circle->destructor = circle_destroy;
        circle->work->pos = chao->work->pos;
        ((struct chao_work *)chao->work)->calling_mate = 1;
    }
    return circle;
}
