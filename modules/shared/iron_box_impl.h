/* the iron box's task. shared/iron_box.c and shared/iron_box_e.c each compile
 * it with art of their own. iron_box_e.c's macros give its globals names of
 * their own */
#ifndef SHARED_IRON_BOX_IMPL_H
#define SHARED_IRON_BOX_IMPL_H

#include "shared/iron_box_internal.h"
#include "system/port.h"

static void iron_box_destroy(struct task *task);
static void iron_box_step(struct task *task);
static void iron_box_draw(struct task *task);
static void iron_box_draw_built(struct task *task);

KEPT static struct iron_box_piece iron_box_pieces[IRON_BOX_PIECES] = {
#include "shared/iron_box_impl/iron_box_pieces.inc"
};
/* TRUE once a single call of iron_box_build has built all four models */
static Bool iron_box_built = FALSE;
static struct model_build iron_box_builds[IRON_BOX_BUILDS] = {
#include "shared/iron_box_impl/iron_box_builds.inc"
};
/* every box's collision record uses this shape in place. so a change one
 * box's step makes to the shape's flags reaches every box */
static struct collision_shape iron_box_shapes[IRON_BOX_SHAPES] = {
#include "shared/iron_box_impl/iron_box_shapes.inc"
};

Uint8 *iron_box_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, iron_box_builds, IRON_BOX_BUILDS) ==
        IRON_BOX_BUILDS) {
        iron_box_built = TRUE;
    }
    return buf;
}

void iron_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * member order would set the step first */
    if (iron_box_built) {
        task->draw = iron_box_draw_built;
    } else {
        task->draw = iron_box_draw;
    }
    task->step = iron_box_step;
    task->destructor = iron_box_destroy;
    work->submode = MODE_WHOLE;
    FLOOR(task) = NO_FLOOR;
    ANIMAL(work) = work->ang.z;
    collision_init_in_place(task, iron_box_shapes, IRON_BOX_SHAPES,
                            COLLISION_OBJECT);
    FALL_LEFT(work) = 0.0F;
    FALL_SPEED(work) = 0.0F;
    stack_add(task);
}

static void iron_box_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    stack_remove(owner);
    owner->extra.p = NULL;
}

static void iron_box_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int j;
    int hit;
    Bool broken;
    int chance;
    /* @hack spelling: nibble_mask, a local for ANIMAL_KIND_BITS, where the
     * name in place would do */
    int nibble_mask;
    struct task *other;
    Uint32 upgrades;

    if (placement_try_leave(task)) {
        return;
    }
    nibble_mask = ANIMAL_KIND_BITS;
    /* @hack spelling: the fall's test as a while that jumps to resting, and
     * the jump from the fall to landing, where an if and its else would do */
    while (!(FALL_LEFT(work) > 0.0F)) {
        goto resting;
    }
    {
        /* @hack spelling: height, a local for FALL_LEFT(work), where
         * FALL_LEFT(work) read in place would do */
        Float height = FALL_LEFT(work);
        Float step = FALL_SPEED(work);

        /* @hack redundant: a block no path reaches, with a store to
         * iron_box_built and one to camera_views[0]'s pos.x */
        if (0) {
            iron_box_built = TRUE;
            camera_views[0]->pos.x = 1.0F;
        }
        if (-step > height) {
            step = -FALL_LEFT(work);
        }
        work->pos.y += step;
        FALL_LEFT(work) += step;
        /* @hack redundant: a block no path reaches, with three reads of step
         * and a store to scl.x */
        if (0) {
            work->scl.x = step + step + step;
        }
        FALL_SPEED(work) = (FALL_SPEED(work) * FALL_DRAG) - FALL_GRAVITY;
        /* while the box falls, a child stands where it will land. the child
         * collides as the box does, from a copy of its shape, and draws
         * nothing */
        if (task->child == NULL) {
            model_object_create(task, NULL, NULL, iron_box_shapes,
                                IRON_BOX_SHAPES, 0);
        }
        if (task->child != NULL) {
            struct task_work *child = task->child->work;

            child->pos.x = work->pos.x;
            child->pos.z = work->pos.z;
            child->pos.y = work->pos.y - FALL_LEFT(work);
        }
        goto landing;
    }
resting:
    if (task->child != NULL) {
        task_destroy_children(task);
    }
landing:
    if (FALL_LEFT(work) <= 0.0F && FALL_SPEED(work) != 0.0F) {
        FALL_LEFT(work) = 0.0F;
        FALL_SPEED(work) = 0.0F;
        if (njAbs(work->pos.y - FLOOR(task)) < DUST_HEIGHT) {
            dust_short_rect(&work->pos, HALF_SIZE, HALF_SIZE, &work->ang,
                            LAND_PUFF_SCALE, LAND_PUFF_DENSITY,
                            LAND_PUFF_SPEED);
        }
    }
    switch (work->submode) {
    case MODE_WHOLE:
        hit = collision_get_hit_count(task);
        /* @hack redundant: a block no path reaches, with an increment of
         * hit */
        if (0) {
            hit++;
        }
        broken = FALSE;
        while (hit--) {
            /* @hack spelling: the assignment to other inside the call, where
             * a statement of its own would do */
            if (player_breaks_task(
                    task, other = collision_get_hit(task, hit)->task)) {
                broken = TRUE;
                break;
            }
        }
        if (broken) {
            NJS_POINT3 center;
            int player;
            struct task_work *breaker_work;
            struct player *breaker;

            center = work->pos;
            center.y += HALF_SIZE;
            sound_play_at_for(BREAK_SOUND, work, BREAK_PRIORITY, SOUND_LOUDEST,
                              BREAK_SOUND_DURATION, &work->pos);
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &center,
                    ((Float)rand() * RAND_SCALE * BREAK_PUFF_RADIUS_RANGE) +
                        BREAK_PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE) + BREAK_PUFF_SPEED,
                    ((Float)rand() * RAND_SCALE * BREAK_PUFF_SCALE_RANGE) +
                        BREAK_PUFF_SCALE);
            }
            work->timer = 0;
            work->submode = MODE_BROKEN;
            score_add(BREAK_POINTS);
            if (other->work->id == WORK_ID_PLAYER) {
                player = player_slot_of(other);
                /* @hack spelling: the assignments to breaker_work and
                 * breaker inside the test, where statements of their own
                 * would do */
                if (player != -1 &&
                    (breaker_work = player_work[player]) != NULL &&
                    (breaker = player_records[player]) != NULL &&
                    (breaker->character != PLAYER_SONIC ||
                     breaker_work->mode == PLAYER_MODE_SONIC_BOUNCE) &&
                    (breaker->character != PLAYER_KNUCKLES ||
                     breaker_work->mode == PLAYER_MODE_HUNTER_DIVE) &&
                    (breaker->character != PLAYER_ROUGE ||
                     breaker_work->mode == PLAYER_MODE_HUNTER_DIVE)) {
                    player_set_speed(player, 0.0F, 0.0F, 0.0F);
                }
            }
        } else {
            work->timer++;
            if (FLOOR(task) == NO_FLOOR && !(work->timer & (FLOOR_STEPS - 1))) {
                struct angles ang;

                ang.x = 0;
                ang.y = work->ang.y;
                ang.z = 0;
                FLOOR(task) = surface_probe_floor(
                    work->pos.x, work->pos.y + FLOOR_LOOK_HEIGHT, work->pos.z,
                    &ang);
            }
            upgrades = player_records[0]->upgrades;
            if (NO_PUSH(upgrades)) {
                work->collision->shapes->flags |= SHAPE_HURT_NO_PUSH;
            } else {
                work->collision->shapes->flags &= ~SHAPE_HURT_NO_PUSH;
            }
            collision_register(task);
        }
        break;
    case MODE_BROKEN:
        if (work->timer == DROP_STEP) {
            /* @hack spelling: at, a copy of the work's pos, where &work->pos
             * would do: stack_drop doesn't write through pos */
            NJS_POINT3 at;

            at = work->pos;
            stack_drop(&at);
        }
        if (work->timer++ > PIECES_STEP) {
            NJS_POINT3 center;
            NJS_VECTOR scale;

            center = work->pos;
            center.y += HALF_SIZE;
            scale.x = HALF_SIZE;
            scale.y = HALF_SIZE;
            scale.z = HALF_SIZE;
            if (iron_box_built) {
                for (i = 0; i < IRON_BOX_PIECES; i++) {
                    for (j = 0; j < iron_box_pieces[i].count; j++) {
                        debris_throw_built(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            PIECE_SPEED + ((Float)rand() *
                                           (PIECE_SPEED_RANGE * RAND_SCALE)),
                            (work->ang.x & PIECES_FALL_THROUGH)
                                ? NO_FLOOR
                                : (FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                           : work->pos.y),
                            iron_box_pieces[i].model,
                            iron_box_builds[i + PIECE_BUILD].built,
                            iron_box_pieces[i].texlist);
                    }
                }
            } else {
                for (i = 0; i < IRON_BOX_PIECES; i++) {
                    for (j = 0; j < iron_box_pieces[i].count; j++) {
                        debris_throw(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            PIECE_SPEED + ((Float)rand() *
                                           (PIECE_SPEED_RANGE * RAND_SCALE)),
                            (work->ang.x & PIECES_FALL_THROUGH)
                                ? NO_FLOOR
                                : (FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                           : work->pos.y),
                            iron_box_pieces[i].model,
                            iron_box_pieces[i].texlist);
                    }
                }
            }
            work->submode = MODE_GONE;
        }
        upgrades = player_records[0]->upgrades;
        if (NO_PUSH(upgrades)) {
            work->collision->shapes->flags |= SHAPE_HURT_NO_PUSH;
        } else {
            work->collision->shapes->flags &= ~SHAPE_HURT_NO_PUSH;
            collision_register(task);
        }
        break;
    case MODE_GONE:
        /* @hack redundant: the mask with nibble_mask after the shift, where
         * the shift alone would do */
        chance = ((ANIMAL(work) & ANIMAL_CHANCE_BITS) >> ANIMAL_CHANCE_SHIFT) &
                 nibble_mask;
        if (chance && task->placement != NULL && !placement_unk_20(task)) {
            /* @hack spelling: the second multiply by TENTH in a statement of
             * its own, where (Float)chance * TENTH * TENTH would do */
            Float odds = (Float)chance * TENTH;

            odds *= TENTH;
            if (odds > (Float)rand() * RAND_SCALE || chance >= ANIMAL_CERTAIN) {
                animal_release(
                    work->pos.x, work->pos.y + HALF_SIZE, work->pos.z,
                    (ANIMAL(work) & nibble_mask) % ANIMAL_RELEASE_KINDS);
            }
            placement_set_unk_20(task);
        }
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

static void iron_box_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its four strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<- CRASHMODE(THROUGH)";
        (void)"<- CRASHMODE(REFRECT)";
        (void)"<- ANGLE Y";
        (void)"<- MINIMAL AP(  ) LEV( )";
    }

    njSetTexture(&iron_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&iron_box_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}

static void iron_box_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(iron_box_builds[BOX_BUILD].texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njDirectDrawObject(iron_box_builds[BOX_BUILD].built);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}

#endif
