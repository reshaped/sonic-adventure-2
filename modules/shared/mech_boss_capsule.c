/* the mech boss's capsule: a child task whose collision capsule the players can
 * hurt. it follows a point and angles the boss keeps. Big Foot, Hot Shot and
 * Flying Dog each make one */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_capsule.h"
#include "system/angle.h"
#include "system/task.h"

/* the record's radius, which collision_set_radius gives it */
#define RADIUS 80.0F
/* in the capsule's work flags: its record is a COLLISION_TARGET */
#define TARGETED 0x100

/* what the capsule follows, in its task's extra */
struct capsule_follow {
    NJS_POINT3 *pos;
    struct angles *ang;
};

/* the capsule's work keeps its boss, an enum mech_boss, in tick. it keeps the
 * range in scl.x */
#define BOSS(work) ((work)->tick)
#define RANGE(work) ((work)->scl.x)

/* each boss's capsule, which attacks at level 1, guards at level 0, and hurts
 * players and objects */
#define LEVELS                                                                 \
    (SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS | SHAPE_HURTS_OBJECTS)
/* clang-format off */
static const struct collision_shape mech_boss_capsule_big_foot[] = {
    {0, SHAPE_CAPSULE, PUSH_PLAYERS, LEVELS, MECH_BOSS_SHAPE_FLAGS,
     {0.0F, 0.0F, 7.0F}, 6.5F, 2.5F, 0.0F, 0, {0x5800, 0, 0}},
};
static const struct collision_shape mech_boss_capsule_hot_shot[] = {
    {0, SHAPE_CAPSULE, PUSH_PLAYERS, LEVELS, MECH_BOSS_SHAPE_FLAGS,
     {0.0F, 0.0F, 8.0F}, 5.5F, 2.0F, 0.0F, 0, {0x6000, 0, 0x6000}},
};
static const struct collision_shape mech_boss_capsule_flying_dog[] = {
    {0, SHAPE_CAPSULE, PUSH_PLAYERS, LEVELS, MECH_BOSS_SHAPE_FLAGS,
     {0.0F, 0.0F, 10.0F}, 5.5F, 4.0F, 0.0F, 0, {0x6000, 0, 0}},
};
/* clang-format on */

static void mech_boss_capsule_destroy(struct task *task);
static void mech_boss_capsule_step(struct task *task);

struct task *mech_boss_capsule_create(struct task *parent,
                                      struct mech_boss_capsule_spec *spec)
{
    struct task *task;
    struct task_work *work;
    struct capsule_follow *follow;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, mech_boss_capsule_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = task->work;
        task->destructor = mech_boss_capsule_destroy;
        follow = syCalloc(1, sizeof(struct capsule_follow));
        if (follow == NULL) {
            task_kill(task);
            return NULL;
        }
        task->extra.p = follow;
        follow->pos = spec->pos;
        follow->ang = spec->ang;
        switch (spec->boss) {
        case MECH_BOSS_BIG_FOOT:
            collision_init(task, mech_boss_capsule_big_foot, 1, COLLISION_ITEM);
            break;
        case MECH_BOSS_HOT_SHOT:
            collision_init(task, mech_boss_capsule_hot_shot, 1, COLLISION_ITEM);
            break;
        case MECH_BOSS_FLYING_DOG:
            collision_init(task, mech_boss_capsule_flying_dog, 1,
                           COLLISION_ITEM);
            break;
        }
        BOSS(work) = spec->boss;
        RANGE(work) = spec->range;
        work->flags |= TARGETED;
        /* @hack spelling: the work reached again through task, where work
         * would do */
        task->work->collision->flags |= COLLISION_TARGET;
        collision_set_radius(task, RADIUS);
        return task;
    }
#ifdef NONMATCHING
    /* for a null parent the image returns R0 as the caller left it, which C
     * can't give. this side returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void mech_boss_capsule_destroy(struct task *task)
{
}

void mech_boss_capsule_reset(struct task *task)
{
    struct task_work *work = task->work;
    const struct collision_shape *shape;
    /* @hack type: levels, an array of one entry, where one record would do */
    struct mech_boss_levels levels[1];

#ifdef NONMATCHING
    /* GCC can't see that every path sets shape. the capsule's boss is one of
     * enum mech_boss, as mech_boss_capsule.h requires */
    shape = mech_boss_capsule_big_foot;
#endif
    switch (BOSS(work)) {
    case MECH_BOSS_BIG_FOOT:
        shape = mech_boss_capsule_big_foot;
        break;
    case MECH_BOSS_HOT_SHOT:
        shape = mech_boss_capsule_hot_shot;
        break;
    case MECH_BOSS_FLYING_DOG:
        shape = mech_boss_capsule_flying_dog;
        break;
    }
    levels[0].attack = shape->levels & SHAPE_ATTACK;
    levels[0].guard = (shape->levels & SHAPE_GUARD) >> SHAPE_GUARD_SHIFT;
    mech_boss_set_levels(task, levels);
    /* @hack spelling: the work reached again through task, where work would
     * do */
    task->work->flags &= ~WORK_HURT;
}

static void mech_boss_capsule_step(struct task *task)
{
    struct capsule_follow *follow = task->extra.p;
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local initialized from a constant, which
     * nothing reads */
    NJS_VECTOR unused = {0.0F, 0.0F,
                         0.0F}; /* NOLINT(clang-diagnostic-unused-variable) */
    Float x;
    Float z;
    Float distance;

    work->pos = *follow->pos;
    work->ang = *follow->ang;
    x = work->pos.x;
    z = work->pos.z;
    distance = njSqrt((x * x) + (z * z));
    if ((work->flags & TARGETED) && distance > RANGE(work)) {
        work->flags &= ~TARGETED;
        /* @hack spelling: the work reached again through task, where work
         * would do */
        task->work->collision->flags &= ~COLLISION_TARGET;
    } else if (!(work->flags & TARGETED) && distance < RANGE(work)) {
        work->flags |= TARGETED;
        /* @hack spelling: the work reached again through task, where work
         * would do */
        task->work->collision->flags |= COLLISION_TARGET;
    }
    collision_register(task);
}
