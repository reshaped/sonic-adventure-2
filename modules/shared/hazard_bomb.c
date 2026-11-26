#include <ninja.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/enemy_bomb.h"
#include "shared/hazard_bomb.h"
#include "system/task.h"

enum hazard_bomb_mode { BOMB_WAITING, BOMB_RISING };
/* a rising bomb keeps its upward speed, in units a step, in the work's scl.z */
#define RISE_SPEED(work) ((work)->scl.z)
#define INITIAL_SPEED 2.5F
#define GRAVITY 0.06F

/* clang-format off */
static struct collision_shape trigger[] = {
    {0, SHAPE_SPHERE, 0, 0, SHAPE_UNTURNED, {0.0F, 0.0F, 0.0F}, 5.0F, 0.0F, 0.0F,
     0, {0, 0, 0}},
};
/* clang-format on */

static void hazard_bomb_destroy(struct task *task)
{
}

static void hazard_bomb_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->mode == BOMB_RISING) {
        work->pos.y += RISE_SPEED(work);
        RISE_SPEED(work) -= GRAVITY;
        if (RISE_SPEED(work) < 0.0F) {
            enemy_bomb_create(NULL, BOMB_NORMAL, BOMB_DROPPED, &work->pos);
            if (task->placement) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            return;
        }
    } else if (collision_next_player(task)) {
        work->mode = BOMB_RISING;
        RISE_SPEED(work) = INITIAL_SPEED;
    }
    collision_register(task);
}

static void hazard_bomb_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&enemy_bomb_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njCnkEasyDrawModel(&enemy_bomb_normal_model);
    njPopMatrixEx();
}

void hazard_bomb_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = hazard_bomb_draw;
        task->step = hazard_bomb_step;
        task->destructor = hazard_bomb_destroy;
        collision_init(task, trigger, 1, COLLISION_OBJECT);
    }
}
