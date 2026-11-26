/* the headgear a garden drops: a pumpkin, a skull or an egg shell's top. it
 * rolls, the player can hold it, and a Chao can carry it and wear it */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/body.h"
#include "chao/carry.h"
#include "chao/egg.h"
#include "chao/garden.h"
#include "chao/model.h"
#include "chao/movement.h"
#include "chao/pickup.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/task.h"
#include "system/util.h"

/* the garden's shadow under headgear is scaled SHADOW_WIDTH across and
 * SHADOW_HEIGHT up */
#define SHADOW_WIDTH 1.5F
#define SHADOW_HEIGHT 0.7F
/* a rolling item rests after more than REST_STEPS steps in a row on the floor
 * slower than REST_SPEED. a dropped item rolls after more than DROP_STEPS. it
 * floats FLOAT_DEPTH under a flat surface, as water's */
#define REST_SPEED 0.01F
#define REST_STEPS 10
#define DROP_STEPS 10
#define FLOAT_DEPTH 1.5F
extern NJS_CNK_OBJECT garden_pumpkin_model;
static const struct collision_shape headgear_shapes[PICKUP_SHAPES] =
    GARDEN_PICKUP_SHAPES(PUSH_ENEMIES | PUSH_OBJECTS, 2.0F,
                         (Sint8)GARDEN_HEADGEAR_HIT, -2, 5);

static void headgear_rest(struct task *item)
{
    struct task_work *work = item->work;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(item);
        work->submode++;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = PICKUP_HELD;
        work->submode = 0;
        return;
    }
    if (collision_pushed(item)) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
        work->timer = 0;
    }
    if (garden_object_locked(item)) {
        work->mode = PICKUP_CARRIED;
        work->submode = 0;
    }
}
static void headgear_roll(struct task *item)
{
    struct task_work *work = item->work;
    struct mover *mover = item->motion.p;
    NJS_VECTOR horizontal;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(item);
        /* @hack spelling: submode + 1 assigned, where ++ would do */
        work->submode = work->submode + 1;
        break;
    }
    garden_apply_buoyancy(item, FLOAT_DEPTH);
    if (work->flags & MOVER_WORK_ON_FLOOR) {
        horizontal.x = mover->spd.x;
        horizontal.y = 0;
        horizontal.z = mover->spd.z;
        if (njScalor(&horizontal) < REST_SPEED) {
            if (work->timer++ > REST_STEPS) {
                work->timer = 0;
                work->mode = PICKUP_REST;
                work->submode = 0;
            }
        }
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = PICKUP_HELD;
        work->submode = 0;
    }
    if (garden_object_locked(item)) {
        work->mode = PICKUP_CARRIED;
        work->submode = 0;
    }
    mover_step(item);
    mover_meet_stage(item);
}
static void headgear_drop(struct task *item)
{
    struct task_work *work = item->work;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HIDE_SHAPES(item);
        work->submode++;
        break;
    }
    mover_step(item);
    if (work->timer++ > DROP_STEPS) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
}
static void headgear_held(struct task *item)
{
    struct task_work *work = item->work;
    struct mover *mover = item->motion.p;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HELD_SHAPES(item);
        mover->aim_y = work->ang.y + player_work[0]->ang.y;
        garden_separate_partners(item);
        work->submode++;
        break;
    }
    /* @hack spelling: the negated heading added, where aim_y less the
     * heading would do */
    work->ang.y = -player_work[0]->ang.y + mover->aim_y;
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        if (garden_object_locked(item)) {
            work->mode = PICKUP_CARRIED;
            work->submode = 0;
        } else {
            work->mode = PICKUP_ROLL;
            work->submode = 0;
            work->timer = 0;
        }
    }
}
static void headgear_carried(struct task *item)
{
    struct task_work *work = item->work;
    garden_take_command(item);
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_CARRIED_SHAPES(item);
        work->submode++;
        work->timer = 0;
        break;
    }
    if (!garden_follow_chao(item)) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
        work->timer = 0;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        garden_separate_partners(item);
        work->mode = PICKUP_HELD;
        work->submode = 0;
    }
}
static void headgear_step(struct task *item)
{
    struct task_work *work = item->work;
    switch (work->mode) {
    case PICKUP_REST:
        headgear_rest(item);
        break;
    case PICKUP_DROP:
        headgear_drop(item);
        break;
    case PICKUP_ROLL:
        headgear_roll(item);
        break;
    case PICKUP_HELD:
        headgear_held(item);
        break;
    case PICKUP_CARRIED:
        headgear_carried(item);
        break;
    }
    mover_save_pos(item);
    collision_register(item);
}
static void headgear_draw(struct task *item)
{
    struct task_work *work = item->work;
    if (garden_visible_at_height(item, 2.5F, 2.0F)) {
        njSetTexture(&chao_body_texlist);
        njPushMatrix(NULL);
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        if (work->flags & WORK_HELD_BY_PLAYER) {
            njTranslate(NULL, 0, -0.35F, 0);
        }
        njRotateY(NULL, work->ang.y);
        njPushMatrixEx();
        if (garden_hit_kind(item, PICKUP_CONTACT, GARDEN_SHADE_HIT)) {
            draw_control_set(DRAW_CONTROL_SHADOW);
        }
        switch (HEADGEAR_KIND(work)) {
        case GARDEN_HEADGEAR_PUMPKIN:
            njCnkEasyDrawObject(&garden_pumpkin_model);
            break;
        case GARDEN_HEADGEAR_SKULL:
            njCnkEasyDrawObject(&chao_hat_2);
            break;
        case GARDEN_HEADGEAR_SHELL:
            njSetTexture(&egg_texlist);
            njTranslate(NULL, 0, 0.4F, 0);
            njCnkEasyDrawModel(egg_shell_top->model);
            break;
        }
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
        if (((struct garden_entry *)item->force.p)->distance <
            chao_tuning.object_shadow_distance) {
            if (garden_hit_kind(item, PICKUP_CONTACT, GARDEN_SHADE_HIT)) {
                njTranslate(NULL, 0, 0.1F, 0);
            } else {
                njTranslate(NULL, 0, 0.4F, 0);
            }
            njScale(NULL, SHADOW_WIDTH, SHADOW_HEIGHT, SHADOW_WIDTH);
            njCnkModDrawModel(&garden_shadow_model);
        }
        njPopMatrix(1);
    }
}
static void headgear_destroy(struct task *item)
{
    garden_unregister_object(item);
}
static void headgear_init(struct task *item)
{
    struct task_work *work = item->work;
#ifdef NONMATCHING
    /* this side takes the garden slot that garden_prepare_pickup reserved. it
     * destroys headgear whose reservation clear_entries has emptied since. its
     * shapes were made with the reservation */
    if (!garden_take_pickup_reservation(item)) {
        task_destroy(item);
        return;
    }
#else
    collision_init(item, headgear_shapes, PICKUP_SHAPES,
                   COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(work, item->motion.p, FALL_KIND_NORMAL);
    item->step = headgear_step;
    item->destructor = headgear_destroy;
    item->draw = headgear_draw;
    work->mode = PICKUP_DROP;
    work->submode = 0;
    garden_register_object(GARDEN_OBJECT_HEADGEAR, item, 0);
    garden_set_carry_height(item, 0);
    garden_set_carry_radius(item, 1.4F);
}
#ifdef NONMATCHING
/* this side returns the headgear, as garden.h says */
struct task *garden_drop_headgear(int kind, NJS_POINT3 *position, Angle yaw,
                                  NJS_VECTOR *velocity)
#else
void garden_drop_headgear(int kind, NJS_POINT3 *position, Angle yaw,
                          NJS_VECTOR *velocity)
#endif
{
#ifdef NONMATCHING
    /* this side makes the task, its mover and its shapes by
     * garden_prepare_pickup, which also reserves a garden slot. it makes no
     * headgear where any of them fails */
    struct task *item =
        garden_prepare_pickup(GARDEN_OBJECT_HEADGEAR, 4, headgear_init,
                              "ALO_ObakeHead", headgear_shapes);
    struct task_work *work;
    struct mover *mover;
    if (item == NULL) {
        return NULL;
    }
    work = item->work;
    mover = item->motion.p;
#else
    struct task *item =
        task_create(TASK_WORK, 4, headgear_init, "ALO_ObakeHead");
    struct task_work *work = item->work;
    struct mover *mover = mover_alloc(item);
#endif
    work->pos = *position;
    work->ang.y = yaw;
    if (velocity) {
        mover->spd = *velocity;
    }
    HEADGEAR_KIND(work) = kind;
    mover_set_gravity(item, -0.05F);
    mover->probe_height = 3.0F;
    mover->floor_bounce = 0.8F;
    mover->friction = 0.7F;
    if (player_character(0) != PLAYER_EGGMAN) {
        mover->hold_turn = 3328;
    } else {
        mover->hold_turn = -4864;
    }
#ifdef NONMATCHING
    /* this side returns the headgear it made */
    return item;
#endif
}
void fun_0c5275dc(void) /* NOLINT(misc-use-internal-linkage) */
{
}
