#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/blast.h"
#include "shared/common_object.h"
#include "shared/dynamite_assets.h"
#include "shared/item_box.h"
#include "sound/sound.h"
#include "stg23/charge.h"
#include "system/task.h"

#define OFFSET(task) ((NJS_POINT3 *)(task)->extra.p)
/* the number of the bit of the parent's submode that the blast sets */
#define BIT_NUMBER(work) ((work)->ang.x)
/* the steps since the charge blew up, counting that step as 1. it is 0 while
 * the charge is whole */
#define BLAST_AGE(work) ((work)->tick)
/* a blown charge is killed on the first step its BLAST_AGE passes
 * BLOWN_STEPS */
#define BLOWN_STEPS 50
#define BLAST_SOUND 0x100c
#define EXPLOSION_LIMIT 100.0F
#define DYNAMITE_BUILDS 1

static void charge_destroy(struct task *task);
static void charge_step(struct task *task);
static void dynamite_draw(struct task *task);

/* clang-format off */
static struct collision_shape shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0.0F, 0.0F, 0.0F},
    7.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Bool dynamite_built = FALSE;
static struct model_build dynamite_builds[DYNAMITE_BUILDS] = {
    {MODEL_BUILD_ALONE, &dynamite_texlist, &dynamite_object, NULL},
};
/* the flare's center, in the charge's frame */
static NJS_POINT3 flare_pos = {0.0F, 10.0F, 5.0F};
/* in pixels */
static Float flare_half_side = 15.0F;
static Float flare_distance = 400.0F;

Uint8 *base_charge_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, dynamite_builds, DYNAMITE_BUILDS) ==
        DYNAMITE_BUILDS) {
        dynamite_built = TRUE;
    }
    return buffer;
}

void base_charge_attach(struct task *parent, NJS_POINT3 *offset, Angle yaw,
                        int index)
{
    struct task *charge;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((charge = task_create_child(TASK_WORK | TASK_EXTRA, charge_step,
                                    parent)) != NULL) {
        charge->draw = dynamite_draw;
        charge->destructor = charge_destroy;
        OFFSET(charge)->x = offset->x;
        OFFSET(charge)->y = offset->y;
        OFFSET(charge)->z = offset->z;
        charge->work->pos.x = parent->work->pos.x + OFFSET(charge)->x;
        charge->work->pos.y = parent->work->pos.y + OFFSET(charge)->y;
        charge->work->pos.z = parent->work->pos.z + OFFSET(charge)->z;
        charge->work->ang.y = yaw;
        collision_init(charge, &shape, 1, COLLISION_ITEM);
        enemy_work_alloc(charge);
        BIT_NUMBER(charge->work) = index;
    }
}

static void charge_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void charge_step(struct task *task)
{
    /* @hack spelling: task->work read again below, where work would do */
    struct task_work *work = task->work;
    struct task_work *parent = task->parent->work;
    struct enemy_work *enemy;

    work->pos.x = parent->pos.x + OFFSET(task)->x;
    work->pos.y = parent->pos.y + OFFSET(task)->y;
    work->pos.z = parent->pos.z + OFFSET(task)->z;
    if (BLAST_AGE(task->work)) {
        if (BLAST_AGE(task->work) > BLOWN_STEPS) {
            task_kill(task);
        }
        BLAST_AGE(task->work)++;
    } else {
        enemy = task->motion.p;
        if (((work->flags & WORK_HURT) &&
             (enemy->shot_by == 0 || enemy->shot_by == 1)) ||
            ((work->collision->flags & COLLISION_TOUCHED) &&
             work->collision->hit->owner->work->id == WORK_ID_LOCKON_BLAST)) {
            BLAST_AGE(task->work) = 1;
            explosion_create_sized(&work->pos, EXPLOSION_LIMIT);
            blast_create(BLAST_MEDIUM_RAISED, &work->pos);
            sound_play(BLAST_SOUND, NULL, 0, 0);
            {
                /* @hack spelling: charge, a copy of task, where task
                 * would do */
                struct task *charge = task;
                charge->parent->work->submode |= 1 << BIT_NUMBER(charge->work);
            }
        } else {
            collision_register(task);
        }
    }
}

#define HIDDEN_BASE_CHARGE
#include "shared/dynamite_draw.h"
