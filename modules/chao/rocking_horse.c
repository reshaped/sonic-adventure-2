/* the Hero Garden's rocking horse, which a Chao rides */
#include <ninja.h>
#include <shinobi.h>

#include "chao/garden_scene.h"
#include "chao/garden_toys.h"
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "system/task.h"
#include "system/util.h"

enum horse_mode { HORSE_EMPTY, HORSE_RIDDEN, HORSE_WAIT };
enum horse_clip { HORSE_ROCK = 3, HORSE_REST = 4 };
#define RIDE_FRAMES 1800

/* clang-format off */
static const struct collision_shape horse_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3), 0,
    {0.0F, 1.2F, 0.0F}, 2.0F, 0.0F, 0.0F, 0.0F, {0, 0, 0}};
/* clang-format on */

static void horse_step(struct task *horse)
{
    struct task_work *work = horse->work;
    struct motion_control *motion = horse->extra.p;

    switch (work->mode) {
    case HORSE_EMPTY:
        if (collision_first_with_id(horse, GARDEN_TOY_ACTIVATE)) {
            GARDEN_TOY_MODE(work, HORSE_RIDDEN);
            motion_set(motion, HORSE_ROCK);
            work->timer = RIDE_FRAMES;
        }
        break;
    case HORSE_RIDDEN:
        /* @hack spelling: the test of the unsigned timer against zero with
         * <=, where equality would do */
        if (--work->timer <= 0) {
            motion_set(motion, HORSE_REST);
            GARDEN_TOY_MODE(work, HORSE_EMPTY);
        }
        if (!garden_mutual_partner(horse)) {
            motion_set(motion, HORSE_REST);
            garden_separate_partners(horse);
            GARDEN_TOY_MODE(work, HORSE_EMPTY);
        }
        break;
    case HORSE_WAIT:
        break;
    }
    motion_step(motion);
    collision_register(horse);
}

static void horse_draw(struct task *horse)
{
    struct task_work *work = horse->work;
    struct motion_control *motion = horse->extra.p;

    if (garden_visible_box(horse, 3.4F, 3.2F, 0.0F)) {
        njSetTexture(&garden_toy_texlist);
        njPushMatrixEx();
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        njTranslate(NULL, 3.0F, 0.0F, 0.0F);
        draw_control_set(DRAW_CONTROL_SHADOW);
        motion_draw_simple(&garden_horse_model, motion);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
        if (((struct garden_entry *)horse->force.p)->distance <
            chao_tuning.large_object_shadow_distance) {
            njTranslate(NULL, 0.0F, 0.8F, 0.0F);
            njScale(NULL, 0.7F, 0.7F, 1.8F);
            njCnkModDrawModel(&garden_shadow_model);
        }
        njPopMatrixEx();
    }
}

static void horse_destroy(struct task *horse)
{
    garden_unregister_object(horse);
}

static void horse_init(struct task *horse)
{
    struct task_work *work = horse->work;
    struct motion_control *motion;

    motion = syCalloc(1, sizeof(*motion));
#ifdef NONMATCHING
    /* this side destroys the horse where the heap has no motion. the matching
     * side writes through the null */
    if (motion == NULL) {
        task_destroy(horse);
        return;
    }
#endif
    motion_init(motion, garden_horse_clips);
    motion_set(motion, HORSE_REST);
    horse->extra.p = motion;
#ifdef NONMATCHING
    /* this side destroys the horse where mover_alloc fails. the matching side
     * writes through the null */
    if (mover_alloc(horse) == NULL) {
        task_destroy(horse);
        return;
    }
#else
    mover_alloc(horse);
#endif
    mover_set_gravity(horse, -0.05F);
    mover_set_hold_turn(horse, 0xd00);
#ifdef NONMATCHING
    /* this side destroys the horse where collision_init fails */
    if (!collision_init(horse, &horse_shape, 1, COLLISION_OBJECT_MUTUAL)) {
        task_destroy(horse);
        return;
    }
#else
    collision_init(horse, &horse_shape, 1, COLLISION_OBJECT_MUTUAL);
#endif
    horse->step = horse_step;
    horse->destructor = horse_destroy;
    horse->draw = horse_draw;
    work->mode = HORSE_EMPTY;
    work->submode = 0;
#ifdef NONMATCHING
    /* this side destroys the horse where the garden has no slot for it */
    if (!garden_register_object(GARDEN_OBJECT_TOY, horse, GARDEN_TOY_HORSE)) {
        task_destroy(horse);
    }
#else
    garden_register_object(GARDEN_OBJECT_TOY, horse, GARDEN_TOY_HORSE);
#endif
}

void garden_rocking_horse_create(NJS_POINT3 *position, Angle yaw)
{
    struct task *horse = task_create(TASK_WORK, 3, horse_init, "ALO_Horse");
#ifdef NONMATCHING
    struct task_work *work;

    /* this side makes no horse where task_create fails. the matching side
     * writes through the null */
    if (horse == NULL) {
        return;
    }
    work = horse->work;
#else
    struct task_work *work = horse->work;
#endif

    work->pos = *position;
    work->ang.y = yaw;
}
