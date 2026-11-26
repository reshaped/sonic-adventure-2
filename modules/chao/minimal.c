/* the garden's small animals. garden_animals_start's task empties player 0's
 * item stock into the garden, an item at a time. the items are animals and
 * chaos drives. a Chao may take an animal from the player's hands and carry it.
 * once the Chao lets it go, the animal leaves the garden after
 * MINIMAL_HIDDEN_FRAMES out of view */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "chao/bounce.h"
#include "chao/carry.h"
#include "chao/drive.h"
#include "chao/garden_scene.h"
#include "chao/minimal.h"
#include "chao/model.h"
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "hud/item_stock.h"
#include "object/animal.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define MINIMAL_HIDDEN_FRAMES 200
/* the task lets an item out on the frame its timer passes RELEASE_WAIT. the
 * timer then starts again from 0, so an item comes out every RELEASE_WAIT + 2
 * frames */
#define RELEASE_WAIT 15
/* each item leaves from LAUNCH_HEIGHT above player 0. a random turn of up to
 * half of LAUNCH_DEGREES either way sets its heading */
#define LAUNCH_HEIGHT 4.5F
#define LAUNCH_DEGREES 60.0F
/* the id of the lower shape, by which a Chao finds the animal as a training
 * item */
#define TRAINING_ITEM_ID (-121)

/* clang-format off */
static const struct collision_shape minimal_shapes[MINIMAL_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_HURTABLE | SHAPE_UNK_2000, {0.0F, 1.5F, 0.0F}, 15.0F, 3.0F, 0.0F,
     0.0F, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_OBJECTS, SHAPE_GUARD_LEVEL(3),
     SHAPE_UNTURNED, {0.0F, 1.2F, 0.0F}, 2.0F, 0.0F, 0.0F, 0.0F, {0, 0, 0}},
    {TRAINING_ITEM_ID, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3), SHAPE_UNTURNED, {0.0F, -4.0F, 0.0F}, 4.2F, 8.0F,
     0.0F, 0.0F, {0, 0, 0}}};
/* clang-format on */

static void (*minimal_actions[ANIMAL_KINDS])(struct task *) = {
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam,
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam,
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam,
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam,
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam,
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam,
    garden_minimal_roam, garden_minimal_roam, garden_minimal_roam};

static struct motion_table_entry minimal_clips[MINIMAL_CLIPS] = {
#include "chao/minimal/minimal_clips.inc"
};

void garden_minimal_land(struct task *animal)
{
    garden_bounce_on_floor(animal);
}

static void minimal_held(struct task *animal)
{
    struct garden_minimal_work *work = MINIMAL_WORK(animal);
    struct mover *mover = animal->motion.p;

    /* @hack spelling: a switch of one case, where an if would do */
    switch (work->base.submode) {
    case 0:
        collision_shape_off(animal, MINIMAL_PICKUP);
        collision_shape_off(animal, MINIMAL_CONTACT);
        collision_shape_on(animal, MINIMAL_LOWER);
        mover->aim_y = work->base.ang.y + player_work[0]->ang.y;
        garden_separate_partners(animal);
        motion_link_frames(&work->motion, MINIMAL_CARRY, 15);
        work->base.submode++;
        break;
    }
    /* @hack spelling: the player's yaw negated and added, where aim_y less
     * the yaw would do */
    work->base.ang.y = -player_work[0]->ang.y + mover->aim_y;
    mover->aim_y = angle_approach(mover->aim_y, QUARTER_TURN, 0xc00);
    if (!(work->base.flags & WORK_HELD_BY_PLAYER)) {
        if (garden_object_locked(animal)) {
            work->base.mode = MINIMAL_CARRIED;
            work->base.submode = 0;
        } else {
            /* @hack spelling: thrown, the mover read again, where mover
             * would do */
            struct mover *thrown = animal->motion.p;
            work->base.mode = MINIMAL_ROAM;
            work->base.submode = 0;
            thrown->spd.x *= 0.4F;
            thrown->spd.y *= 0.5F;
            thrown->spd.z *= 0.4F;
            mover_clear_acc(animal);
        }
    }
}

static void minimal_carried(struct task *animal)
{
    struct task_work *base = animal->work;
    struct garden_minimal_work *work;
    struct garden_entry *partner;

    garden_take_command(animal);
    work = MINIMAL_WORK(animal);
    /* @hack spelling: a switch of one case, where an if would do */
    switch (base->submode) {
    case 0:
        collision_shape_off(animal, MINIMAL_PICKUP);
        collision_shape_off(animal, MINIMAL_CONTACT);
        collision_shape_off(animal, MINIMAL_LOWER);
        base->submode++;
        base->timer = 0;
        motion_link_frames(&work->motion, MINIMAL_CARRY, 15);
        partner = garden_partner_in_category(animal, GARDEN_OBJECT_CHAO);
        if (partner && partner->task) {
            garden_set_target(animal, partner->task);
        }
        break;
    }
    garden_turn_to_target(animal, 0x200);
    if (!garden_follow_chao(animal)) {
        work->released = TRUE;
        base->flags &= ~(FALL_UNK_200 | FALL_OFFERABLE);
        collision_shape_off(animal, MINIMAL_PICKUP);
        collision_shape_off(animal, MINIMAL_LOWER);
        base->mode = MINIMAL_ROAM;
        base->submode = 0;
    }
}

static void minimal_draw(struct task *animal)
{
    struct garden_minimal_work *work = MINIMAL_WORK(animal);
    /* @hack spelling: scale, a local that holds 1.0F, where the constant
     * would do */
    Float scale = 1.0F;
    Float lift;
    /* @hack redundant: unused, a word set to -1 that nothing reads */
    Sint32 unused[1];

    if (!garden_visible_at_height(animal, 3.0F, 2.0F)) {
        if (work->released) {
            work->hidden_frames--;
            if (work->hidden_frames <= 0) {
                work->base.mode = MINIMAL_EXIT;
                work->base.submode = 0;
            }
        }
        return;
    }
    work->hidden_frames = MINIMAL_HIDDEN_FRAMES;
    if (work->base.flags & WORK_HELD_BY_PLAYER) {
        lift = -1.2F;
    } else {
        lift = -1.0F;
    }
    unused[0] = -1;
    (void)unused;
    njSetTexture(&garden_animal_texlist);
    njPushMatrix(NULL);
    njTranslate(NULL, work->base.pos.x, work->base.pos.y, work->base.pos.z);
    njTranslate(NULL, 0.0F, lift, 0.0F);
    njRotateY(NULL, work->base.ang.y);
    njPushMatrixEx();
    njScale(NULL, scale, scale, scale);
    if (garden_hit_kind(animal, MINIMAL_CONTACT, GARDEN_SHADE_HIT)) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    motion_draw(garden_minimal_models[MINIMAL_KIND(work)], &work->motion);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
    if (((struct garden_entry *)animal->force.p)->distance <
        chao_tuning.object_shadow_distance) {
        if (garden_hit_kind(animal, MINIMAL_CONTACT, GARDEN_SHADE_HIT)) {
            njTranslate(NULL, 0.0F, 0.01F, 0.0F);
        } else {
            njTranslate(NULL, 0.0F, 0.5F, 0.0F);
        }
        njCnkModDrawModel(&garden_shadow_model);
    }
    njPopMatrix(1);
}

static void minimal_destroy(struct task *animal)
{
    animal->extra.p = NULL;
    garden_unregister_object(animal);
}

static void minimal_step(struct task *animal)
{
    struct garden_minimal_work *work = MINIMAL_WORK(animal);

    switch (work->base.mode) {
    case MINIMAL_ROAM:
        minimal_actions[MINIMAL_KIND(work)](animal);
        break;
    case MINIMAL_HELD:
        minimal_held(animal);
        break;
    case MINIMAL_CARRIED:
        minimal_carried(animal);
        break;
    }
    mover_save_pos(animal);
    motion_step(&work->motion);
    if (work->base.mode == MINIMAL_EXIT) {
        task_kill(animal);
    } else {
        switch (work->base.mode) {
        case MINIMAL_ROAM:
        case MINIMAL_HELD:
            collision_register(animal);
            break;
        default:
            collision_reset(animal);
            break;
        }
    }
}

void garden_minimal_motion_table(struct motion_table_entry *clips, Uint32 kind)
{
#ifdef NONMATCHING
    /* the clip this side's loop below steps */
    int clip;
#endif
    njMemCopy(clips, minimal_clips, sizeof(minimal_clips));
#ifdef NONMATCHING
    /* the image's clips give link timing, and a build without the disc's
     * data has clips that give none. each of those takes MOTION_LINK_FRAMES
     * and no next entry */
    if (clips[MINIMAL_IDLE].link_frames == 0) {
        for (clip = MINIMAL_IDLE; clip <= MINIMAL_FLY; clip++) {
            clips[clip].link_frames = MOTION_LINK_FRAMES;
            clips[clip].next = MOTION_NONE;
        }
    }
#endif
    clips[MINIMAL_IDLE].motion = garden_minimal_idle_motions[kind];
    clips[MINIMAL_WALK].motion = garden_minimal_walk_motions[kind];
    clips[MINIMAL_CARRY].motion = garden_minimal_held_motions[kind];
    clips[MINIMAL_FLY].motion = garden_minimal_fly_motions[kind];
}

#ifdef NONMATCHING
/* this side returns whether it made the animal, as minimal.h says */
Bool garden_minimal_create(Sint8 kind, NJS_POINT3 *position, Angle yaw,
                           NJS_VECTOR *velocity)
#else
void garden_minimal_create(Sint8 kind, NJS_POINT3 *position, Angle yaw,
                           NJS_VECTOR *velocity)
#endif
{
    struct task *animal = task_create(0, 2, minimal_step, "AL_MinimalExecutor");
    struct garden_minimal_work *work;
    struct mover *mover;
#ifdef NONMATCHING
    /* this side returns FALSE where task_create fails. the matching side writes
     * through the null */
    if (animal == NULL) {
        return FALSE;
    }
#endif

    animal->work = syCalloc(1, sizeof(*work));
    work = MINIMAL_WORK(animal);
#ifdef NONMATCHING
    /* this side destroys the task and returns FALSE where the heap has no work.
     * the matching side writes through the null */
    if (work == NULL) {
        task_destroy(animal);
        return FALSE;
    }
#endif
    garden_minimal_motion_table(work->clips, kind);
    if (kind == ANIMAL_BATMAN) {
        work->clips[MINIMAL_IDLE] = work->clips[MINIMAL_WALK];
    }
    motion_init(&work->motion, work->clips);
    motion_link(&work->motion, MINIMAL_WALK);
    mover = mover_alloc(animal);
#ifdef NONMATCHING
    /* this side destroys the animal and returns FALSE where mover_alloc fails.
     * the matching side writes through the null */
    if (mover == NULL) {
        task_destroy(animal);
        return FALSE;
    }
#endif
    work->base.pos = *position;
    work->base.ang.y = yaw;
    mover->spd = *velocity;
    MINIMAL_KIND(work) = kind;
    work->hidden_frames = MINIMAL_HIDDEN_FRAMES;
#ifdef NONMATCHING
    /* this side destroys the animal and returns FALSE where collision_init
     * fails */
    if (!collision_init(animal, minimal_shapes, MINIMAL_SHAPES,
                        COLLISION_OBJECT_MUTUAL)) {
        task_destroy(animal);
        return FALSE;
    }
#else
    collision_init(animal, minimal_shapes, MINIMAL_SHAPES,
                   COLLISION_OBJECT_MUTUAL);
#endif
    fall_set_kind(&work->base, animal->motion.p, FALL_KIND_NORMAL);
    animal->destructor = minimal_destroy;
    animal->draw = minimal_draw;
    work->base.mode = MINIMAL_ROAM;
    work->base.submode = 0;
    mover->probe_height = 0.5F;
    mover_set_gravity(animal, -0.05F);
    mover->unk_b4 = 1.5F;
    mover->bottom = -1.5F;
    mover->floor_bounce = 0.65F;
    mover->friction = 0.85F;
    if (player_character(0) != PLAYER_EGGMAN) {
        mover->hold_turn = 0xd00;
    } else {
        mover->hold_turn = -0x1300;
    }
    mover_save_pos(animal);
#ifdef NONMATCHING
    /* this side destroys the animal and returns FALSE where the garden has no
     * slot for it */
    if (!garden_register_object(GARDEN_OBJECT_MINIMAL, animal, 0)) {
        task_destroy(animal);
        return FALSE;
    }
#else
    garden_register_object(GARDEN_OBJECT_MINIMAL, animal, 0);
#endif
    garden_set_carry_height(animal, -0.5F);
    garden_set_carry_radius(animal, 1.3F);
#ifdef NONMATCHING
    /* this side returns that it made the animal */
    return TRUE;
#endif
}

static void release_from_stock(struct task *manager)
{
    struct task_work *work = manager->work;
    NJS_POINT3 position;
    int kind;
    int animals;

    item_stock_hold(0);
    if (work->timer++ > RELEASE_WAIT) {
        struct task_work *player = player_work[0];
        NJS_VECTOR launch = {0.0F, 1.6F, 1.6F};
        NJS_VECTOR velocity;

        work->timer = 0;
        position.x = player->pos.x;
        position.y = player->pos.y + LAUNCH_HEIGHT;
        position.z = player->pos.z;
        launch.y *= ((0.5F - (rand() * RAND_SCALE)) * 0.2F) + 0.6F;
        launch.z *= ((0.5F - (rand() * RAND_SCALE)) * 0.25F) + 0.35F;
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, -player->ang.y +
                            (Angle)((0.5F - (rand() * RAND_SCALE)) *
                                    (LAUNCH_DEGREES * ANGLE_PER_DEGREE)) +
                            QUARTER_TURN);
        njCalcVector(NULL, &launch, &velocity);
        njPopMatrixEx();
#ifdef NONMATCHING
        /* this side looks at the stock's first item. it pops the item below
         * only once the animal or the drive is made */
        kind = item_stock_peek(0);
#else
        kind = item_stock_pop(0);
#endif
        /* @hack spelling: animals, a local set to ANIMAL_KINDS after the pop,
         * where the constant would do */
        animals = ANIMAL_KINDS;
        if (kind < 0) {
            task_kill(manager);
        } else if (kind < animals) {
#ifdef NONMATCHING
            /* this side keeps the item in the stock, and returns where the
             * animal can't be made */
            if (!garden_minimal_create(kind, &position, player->ang.y,
                                       &velocity)) {
                return;
            }
#else
            garden_minimal_create(kind, &position, player->ang.y, &velocity);
#endif
        } else if (kind < ITEMS) {
#ifdef NONMATCHING
            /* this side keeps the item in the stock, and returns where the
             * drive can't be made */
            if (!garden_drive_create(kind - ITEM_CHAOS_DRIVE, &position,
                                     &velocity)) {
                return;
            }
#else
            garden_drive_create(kind - ITEM_CHAOS_DRIVE, &position, &velocity);
#endif
        }
#ifdef NONMATCHING
        /* this side pops the item it released */
        if (kind >= 0) {
            item_stock_pop(0);
        }
#endif
    }
}

void garden_animals_start(void)
{
    task_create(TASK_WORK, 2, release_from_stock,
                "AL_MinimalCreateManagerExecutor");
}
