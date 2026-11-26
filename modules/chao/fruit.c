/* a garden's fruit. a Chao eats it in WHOLE_FRUIT bites, and each bite feeds
 * the Chao. the last bite also gives the effect of the fruit's kind */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/bounce.h"
#include "chao/carry.h"
#include "chao/egg.h"
#include "chao/emotion.h"
#include "chao/fruit.h"
#include "chao/garden_scene.h"
#include "chao/life.h"
#include "chao/movement.h"
#include "chao/orchard_art.h"
#include "chao/pickup.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a fruit's ang.x: its kind, a garden_fruit. its ang.z: the bites left */
#define FRUIT_KIND(work) ((work)->ang.x)
#define FRUIT_BITES(work) ((work)->ang.z)
#define WHOLE_FRUIT 4

/* a bite comes after more than BITE_STEPS steps of PARTNER_USE. the fruit's
 * contact and carry radius are BITE_RADIUS for each bite left */
#define BITE_STEPS 180
#define BITE_RADIUS 0.5F

/* a rolling fruit rests after more than REST_STEPS steps in a row on the
 * floor slower than REST_SPEED. a dropped fruit rolls after more than
 * DROP_STEPS */
#define REST_SPEED 0.01F
#define REST_STEPS 30
#define DROP_STEPS 20
/* a fruit floats FLOAT_DEPTH under a flat surface, as water's */
#define FLOAT_DEPTH 2.5F
/* what a fruit's last bite gives, by its kind. a Chao fruit adds
 * CHAO_FRUIT_SKILL to five skills. a hero or dark fruit moves alignment by
 * ALIGNMENT_FRUIT. a heart fruit adds HEART_BREEDING to breeding, and a smart
 * fruit SMART_SKILL to intelligence. an active or calm fruit moves four
 * traits by TEMPER_STEP */
#define CHAO_FRUIT_SKILL 30
#define ALIGNMENT_FRUIT 0.15F
#define HEART_BREEDING 200
#define SMART_SKILL 100
#define TEMPER_STEP 10

static const struct collision_shape fruit_shapes[PICKUP_SHAPES] =
    GARDEN_PICKUP_SHAPES(PUSH_ENEMIES | PUSH_OBJECTS, WHOLE_FRUIT *BITE_RADIUS,
                         (Sint8)GARDEN_FRUIT_HIT, -4, 8);

void garden_fruit_land(struct task *fruit)
{
    garden_bounce_on_floor(fruit);
}

static void feed_chao(struct task *chao, int kind)
{
    if (chao) {
        struct chao_parameters *parameters =
            &CHAO_WORK(chao)->profile.parameters;
        if (parameters->type == CHAO_CHILD) {
            chao_add_evolution(chao, chao_tuning.child_fruit_evolution);
            chao_add_skill(chao, CHAO_STAMINA, chao_tuning.child_fruit_stamina);
            chao_add_life(chao, chao_tuning.fruit_life);
            chao_add_lifespan(chao, chao_tuning.fruit_lifespan);
        } else {
            chao_add_evolution(chao, chao_tuning.adult_fruit_evolution);
            chao_add_skill(chao, CHAO_STAMINA, chao_tuning.adult_fruit_stamina);
            chao_add_life(chao, chao_tuning.fruit_life);
        }
    }
}

static void fruit_rest(struct task *fruit)
{
    struct task_work *work = fruit->work;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(fruit);
        work->submode++;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = PICKUP_HELD;
        work->submode = 0;
        return;
    }
    if (collision_pushed(fruit)) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
    if (garden_object_locked(fruit)) {
        work->mode = PICKUP_CARRIED;
        work->submode = 0;
    }
}

static void fruit_free(struct task *fruit)
{
    struct task_work *work = fruit->work;
    struct mover *mover = fruit->motion.p;
    NJS_VECTOR horizontal;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_FREE_SHAPES(fruit);
        work->submode++;
        work->timer = 0;
        break;
    }
    garden_apply_buoyancy(fruit, FLOAT_DEPTH);
    if (work->flags & MOVER_WORK_ON_FLOOR) {
        horizontal.x = mover->spd.x;
        horizontal.y = 0;
        horizontal.z = mover->spd.z;
        if (njScalor(&horizontal) < REST_SPEED) {
            if (work->timer++ > REST_STEPS) {
                mover_clear_spd(fruit);
                mover_clear_acc(fruit);
                work->mode = PICKUP_REST;
                work->submode = 0;
            }
        } else {
            work->timer = 0;
        }
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = PICKUP_HELD;
        work->submode = 0;
    }
    if (garden_object_locked(fruit)) {
        work->mode = PICKUP_CARRIED;
        work->submode = 0;
    }
    mover_step(fruit);
    mover_meet_stage(fruit);
}

static void fruit_drop(struct task *fruit)
{
    struct task_work *work = fruit->work;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HIDE_SHAPES(fruit);
        work->submode++;
        break;
    }
    mover_step(fruit);
    if (work->timer++ > DROP_STEPS) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
}

static void fruit_held(struct task *fruit)
{
    struct task_work *work = fruit->work;
    struct mover *mover = fruit->motion.p;
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_HELD_SHAPES(fruit);
        mover->aim_y = work->ang.y + player_work[0]->ang.y;
        garden_separate_partners(fruit);
        work->submode++;
        break;
    }
    /* @hack spelling: the negated heading added, where aim_y less the
     * heading would do */
    work->ang.y = -player_work[0]->ang.y + mover->aim_y;
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        if (garden_object_locked(fruit)) {
            if (FRUIT_BITES(work) == WHOLE_FRUIT) {
                struct garden_entry *entry =
                    garden_partner_in_category(fruit, GARDEN_OBJECT_CHAO);
                if (entry) {
                    Uint8 type =
                        CHAO_WORK(entry->task)->profile.parameters.type;
                    if (type == CHAO_CHILD) {
                        chao_player_affection(
                            entry->task, chao_tuning.fruit_gift_affection,
                            chao_tuning.child_fruit_gift_alignment);
                    } else {
                        chao_player_affection(
                            entry->task, chao_tuning.fruit_gift_affection,
                            chao_tuning.adult_fruit_gift_alignment);
                    }
                }
            }
            work->mode = PICKUP_CARRIED;
            work->submode = 0;
        } else {
            work->mode = PICKUP_ROLL;
            work->submode = 0;
        }
    }
}

static void fruit_carried(struct task *fruit)
{
    struct task_work *work = fruit->work;
    int command = garden_take_command(fruit);
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        PICKUP_CARRIED_SHAPES(fruit);
        work->submode++;
        work->timer = 0;
        break;
    }
    /* @hack spelling: the command and the bite's steps in nested ifs, where
     * one && would do */
    if (command == PARTNER_USE) {
        if (work->timer++ > BITE_STEPS) {
            struct garden_entry *entry =
                garden_partner_in_category(fruit, GARDEN_OBJECT_CHAO);
            if (entry) {
                struct task *chao = entry->task;
                int kind = FRUIT_KIND(work);
                feed_chao(chao, kind);
            }
            work->timer = 0;
            FRUIT_BITES(work)--;
            collision_get_shape(fruit, PICKUP_CONTACT)->radius =
                FRUIT_BITES(work) * BITE_RADIUS;
            garden_set_carry_radius(fruit, FRUIT_BITES(work) * BITE_RADIUS);
            if (player_character(0) != PLAYER_EGGMAN) {
                ((struct mover *)fruit->motion.p)->hold_turn =
                    FRUIT_BITES(work) * 832;
            } else {
                ((struct mover *)fruit->motion.p)->hold_turn =
                    (FRUIT_BITES(work) * 320) - 4864;
            }
            if (FRUIT_BITES(work) == 0) {
                if (entry) {
                    struct task *chao = entry->task;
                    switch (FRUIT_KIND(work)) {
                    case GARDEN_FRUIT_CHAO:
                        chao_add_skill(chao, CHAO_SWIM, CHAO_FRUIT_SKILL);
                        chao_add_skill(chao, CHAO_FLY, CHAO_FRUIT_SKILL);
                        chao_add_skill(chao, CHAO_RUN, CHAO_FRUIT_SKILL);
                        chao_add_skill(chao, CHAO_POWER, CHAO_FRUIT_SKILL);
                        chao_add_skill(chao, CHAO_STAMINA, CHAO_FRUIT_SKILL);
                        break;
                    case GARDEN_FRUIT_HERO:
                        chao_add_alignment(chao, ALIGNMENT_FRUIT);
                        break;
                    case GARDEN_FRUIT_DARK:
                        chao_add_alignment(chao, -ALIGNMENT_FRUIT);
                        break;
                    case GARDEN_FRUIT_HEART:
                        chao_emotion_add(chao, CHAO_BREEDING, HEART_BREEDING);
                        break;
                    case GARDEN_FRUIT_ACTIVE:
                        chao_emotion_add(chao, CHAO_CURIOSITY, TEMPER_STEP);
                        chao_emotion_add(chao, CHAO_AGGRESSION, TEMPER_STEP);
                        chao_emotion_add(chao, CHAO_CALM, -TEMPER_STEP);
                        chao_emotion_add(chao, CHAO_SLEEPY_HEAD, -TEMPER_STEP);
                        break;
                    case GARDEN_FRUIT_SMART:
                        chao_add_skill(chao, CHAO_INTELLIGENCE, SMART_SKILL);
                        break;
                    case GARDEN_FRUIT_CALM:
                        chao_emotion_add(chao, CHAO_CURIOSITY, -TEMPER_STEP);
                        chao_emotion_add(chao, CHAO_AGGRESSION, -TEMPER_STEP);
                        chao_emotion_add(chao, CHAO_CALM, TEMPER_STEP);
                        chao_emotion_add(chao, CHAO_SLEEPY_HEAD, TEMPER_STEP);
                        break;
                    case GARDEN_FRUIT_ROUND:
                    case GARDEN_FRUIT_TRIANGLE:
                    case GARDEN_FRUIT_SQUARE:
                        break;
                    }
                }
                garden_command_partner(fruit, PARTNER_USED_UP);
                task_kill(fruit);
            }
        }
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        garden_command_partner(fruit, PARTNER_STOLEN);
        garden_separate_partners(fruit);
        work->mode = PICKUP_HELD;
        work->submode = 0;
    }
    if (!garden_follow_chao(fruit)) {
        work->mode = PICKUP_ROLL;
        work->submode = 0;
    }
}

static void fruit_step(struct task *fruit)
{
    struct task_work *work = fruit->work;
    switch (work->mode) {
    case PICKUP_REST:
        fruit_rest(fruit);
        break;
    case PICKUP_DROP:
        fruit_drop(fruit);
        break;
    case PICKUP_ROLL:
        fruit_free(fruit);
        break;
    case PICKUP_HELD:
        fruit_held(fruit);
        break;
    case PICKUP_CARRIED:
        fruit_carried(fruit);
        break;
    }
    mover_save_pos(fruit);
    collision_register(fruit);
}

static void fruit_draw(struct task *fruit)
{
    struct task_work *work = fruit->work;
    Float scale;
    if (garden_visible_at_height(fruit, 2.5F, 2.0F)) {
        scale = 0.3F + (FRUIT_BITES(work) * 0.3F);
        if (FRUIT_KIND(work) == GARDEN_FRUIT_SQUARE) {
            scale *= 0.8F;
        }
        switch (FRUIT_KIND(work)) {
        case GARDEN_FRUIT_NEUTRAL:
            njSetTexture(&neutral_garden_texlist);
            break;
        case GARDEN_FRUIT_HERO_GARDEN:
            njSetTexture(&hero_garden_texlist);
            break;
        case GARDEN_FRUIT_DARK_GARDEN:
            njSetTexture(&dark_garden_texlist);
            break;
        default:
            njSetTexture(&egg_texlist);
            break;
        }
        njPushMatrix(NULL);
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        if (work->flags & WORK_HELD_BY_PLAYER) {
            njTranslate(NULL, 0, 0.4F, 0);
        } else {
            njTranslate(NULL, 0, 0.8F + (scale * 0.55F), 0);
        }
        if (FRUIT_BITES(work) > 0) {
            njRotateY(NULL, work->ang.y);
            njPushMatrixEx();
            njScale(NULL, scale, scale, scale);
            if (garden_hit_kind(fruit, PICKUP_CONTACT, GARDEN_SHADE_HIT)) {
                draw_control_set(DRAW_CONTROL_SHADOW);
            }
            if (FRUIT_KIND(work) == 1) {
                njRotateX(NULL, HALF_TURN);
            }
            njCnkEasyDrawObject(garden_fruit_models[FRUIT_KIND(work)]);
            draw_control_clear(DRAW_CONTROL_SHADOW);
            njPopMatrixEx();
            if (((struct garden_entry *)fruit->force.p)->distance <
                chao_tuning.object_shadow_distance) {
                if (garden_hit_kind(fruit, PICKUP_CONTACT, GARDEN_SHADE_HIT)) {
                    njTranslate(NULL, 0, -1.85F, 0);
                } else {
                    njTranslate(NULL, 0, -1.0F, 0);
                }
                njScale(NULL, scale, 0.7F, scale);
                njCnkModDrawModel(&garden_shadow_model);
            }
        }
        njPopMatrix(1);
    }
}

static void fruit_destroy(struct task *fruit)
{
    garden_unregister_object(fruit);
}

static void fruit_init(struct task *fruit)
{
    struct task_work *work = fruit->work;
#ifdef NONMATCHING
    /* this side takes the garden slot that garden_prepare_pickup reserved. it
     * destroys a fruit whose reservation clear_entries has emptied since. its
     * shapes were made with the reservation */
    if (!garden_take_pickup_reservation(fruit)) {
        task_destroy(fruit);
        return;
    }
#else
    collision_init(fruit, fruit_shapes, PICKUP_SHAPES, COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(work, fruit->motion.p, FALL_KIND_NORMAL);
    fruit->step = fruit_step;
    fruit->destructor = fruit_destroy;
    fruit->draw = fruit_draw;
    work->mode = PICKUP_DROP;
    work->submode = 0;
    garden_register_object(GARDEN_OBJECT_FRUIT, fruit, 0);
    garden_set_carry_height(fruit, 1.0F);
    garden_set_carry_radius(fruit, WHOLE_FRUIT * BITE_RADIUS);
    FRUIT_BITES(work) = WHOLE_FRUIT;
}

#ifdef NONMATCHING
/* this side returns the fruit, as fruit.h says */
struct task *garden_fruit_create(int kind, NJS_POINT3 *position, Angle yaw,
                                 NJS_VECTOR *velocity)
#else
void garden_fruit_create(int kind, NJS_POINT3 *position, Angle yaw,
                         NJS_VECTOR *velocity)
#endif
{
#ifdef NONMATCHING
    /* this side makes the task, its mover and its shapes by
     * garden_prepare_pickup, which also reserves a garden slot. it makes no
     * fruit where any of them fails */
    struct task *fruit = garden_prepare_pickup(
        GARDEN_OBJECT_FRUIT, 4, fruit_init, "ALO_Fruit", fruit_shapes);
    struct task_work *work;
    struct mover *mover;
    if (fruit == NULL) {
        return NULL;
    }
    work = fruit->work;
    mover = fruit->motion.p;
#else
    struct task *fruit = task_create(TASK_WORK, 4, fruit_init, "ALO_Fruit");
    struct task_work *work = fruit->work;
    struct mover *mover = mover_alloc(fruit);
#endif
    work->pos = *position;
    work->ang.y = yaw;
    if (velocity) {
        mover->spd = *velocity;
    }
    FRUIT_KIND(work) = kind;
    mover_set_gravity(fruit, -0.05F);
    mover->probe_height = 3.0F;
    mover->floor_bounce = 0.65F;
    mover->friction = 0.7F;
#ifdef NONMATCHING
    /* this side returns the fruit it made */
    return fruit;
#endif
}

int garden_fruit_kind(struct task *fruit)
{
    return FRUIT_KIND(fruit->work);
}

void fun_0c5250ae(void) /* NOLINT(misc-use-internal-linkage) */
{
}
