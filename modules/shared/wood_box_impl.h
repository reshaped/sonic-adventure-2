/* CONTWOOD's build, init and callbacks. shared/wood_box.c and
 * shared/wood_box_e.c compile them over models of their own. the includer
 * defines the box's and its pieces' art, which shared/wood_box.h declares */
#ifndef SHARED_WOOD_BOX_IMPL_H
#define SHARED_WOOD_BOX_IMPL_H

#include "shared/wood_box_internal.h"
#include "system/port.h"

static void wood_box_destroy(struct task *task);
static void wood_box_step(struct task *task);
static void wood_box_draw(struct task *task);

KEPT static struct wood_box_piece wood_box_pieces[WOOD_BOX_PIECES] = {
#include "shared/wood_box_impl/wood_box_pieces.inc"
};
/* TRUE once one call of wood_box_build has built all WOOD_BOX_BUILDS models */
static Bool wood_box_built = FALSE;
static struct model_build wood_box_builds[WOOD_BOX_BUILDS] = {
#include "shared/wood_box_impl/wood_box_builds.inc"
};
/* every box's collision record uses this shape in place. a step's change to
 * its flags reaches every box */
static struct collision_shape wood_box_shapes[WOOD_BOX_SHAPES] = {
#include "shared/wood_box_impl/wood_box_shapes.inc"
};

Uint8 *wood_box_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, wood_box_builds, WOOD_BOX_BUILDS) ==
        WOOD_BOX_BUILDS) {
        wood_box_built = TRUE;
    }
    return buf;
}

void wood_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's order
     * would set the step first */
    task->draw = wood_box_draw;
    task->step = wood_box_step;
    task->destructor = wood_box_destroy;
    work->submode = MODE_WHOLE;
    FLOOR(task) = NO_FLOOR;
    FALL_LEFT(work) = 0.0F;
    FALL_SPEED(work) = 0.0F;
    collision_init_in_place(task, wood_box_shapes, WOOD_BOX_SHAPES,
                            COLLISION_OBJECT);
    ANIMAL(work) = work->ang.z;
    stack_add(task);
}

static void wood_box_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    stack_remove(owner);
    owner->extra.p = NULL;
}

static void wood_box_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int j;
    int hit;
    Bool broken;
    int chance;
    /* @hack spelling: nibble_mask, a local that masks a nibble, where
     * ANIMAL_KIND_BITS in place would do */
    int nibble_mask;
    struct task *other;

    if (placement_try_leave(task)) {
        return;
    }
    nibble_mask = ANIMAL_KIND_BITS;
    /* @hack spelling: the test as a loop that jumps to resting, and the fall's
     * block as one that ends in a jump to landing, where an if with an else
     * would do */
    while (!(FALL_LEFT(work) > 0.0F)) {
        goto resting;
    }
    {
        /* @hack spelling: height, a local for FALL_LEFT(work), where
         * FALL_LEFT(work) read in place would do */
        Float height = FALL_LEFT(work);
        Float step = FALL_SPEED(work);

        /* @hack redundant: a block no path reaches, which stores to
         * wood_box_built and to camera_views[0]'s pos.x */
        if (0) {
            wood_box_built = TRUE;
            camera_views[0]->pos.x = 1.0F;
        }
        if (-step > height) {
            step = -FALL_LEFT(work);
        }
        work->pos.y += step;
        FALL_LEFT(work) += step;
        FALL_SPEED(work) = (FALL_SPEED(work) * FALL_DRAG) - FALL_GRAVITY;
        /* a falling box's child stands where the box will land. the child
         * has a copy of the box's shape, and no model */
        if (task->child == NULL) {
            model_object_create(task, NULL, NULL, wood_box_shapes,
                                WOOD_BOX_SHAPES, 0);
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
        broken = FALSE;
        while (hit--) {
            /* @hack spelling: the assignment to other inside the call, where
             * a statement of its own would do */
            if (player_attacks_task(
                    task, other = collision_get_hit(task, hit)->task)) {
                broken = TRUE;
                break;
            }
        }
        if (broken) {
            NJS_POINT3 center;
            int player;

            center = work->pos;
            center.y += HALF_SIZE;
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
            sound_play_at_for(BREAK_SOUND, work, BREAK_PRIORITY, SOUND_LOUDEST,
                              BREAK_SOUND_DURATION, &work->pos);
            if (other->work->id == WORK_ID_PLAYER) {
                player = player_slot_of(other);
                if (player != -1 &&
                    player_records[player]->character != PLAYER_SONIC &&
                    player_work[player]->mode != PLAYER_MODE_SONIC_BOUNCE &&
                    player_records[player]->character != PLAYER_KNUCKLES &&
                    player_records[player]->character != PLAYER_ROUGE &&
                    player_work[player]->mode != PLAYER_MODE_HUNTER_DIVE) {
                    player_set_speed(player, 0.0F, 0.0F, 0.0F);
                }
            }
            score_add(BREAK_POINTS);
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
            if (player_records[0]->upgrades & NO_PUSH_UPGRADES) {
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
            if (wood_box_built) {
                for (i = 0; i < WOOD_BOX_PIECES; i++) {
                    for (j = 0; j < wood_box_pieces[i].count; j++) {
                        debris_throw_built(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            PIECE_SPEED + ((Float)rand() *
                                           (PIECE_SPEED_RANGE * RAND_SCALE)),
                            (work->ang.x & PIECES_FALL_THROUGH)
                                ? NO_FLOOR
                                : (FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                           : work->pos.y),
                            wood_box_pieces[i].model,
                            wood_box_builds[i + PIECE_BUILD].built,
                            wood_box_pieces[i].texlist);
                    }
                }
            } else {
                for (i = 0; i < WOOD_BOX_PIECES; i++) {
                    for (j = 0; j < wood_box_pieces[i].count; j++) {
                        debris_throw(
                            &center, DEBRIS_THROW_SQUARE, &scale, work->ang.y,
                            PIECE_SPEED + ((Float)rand() *
                                           (PIECE_SPEED_RANGE * RAND_SCALE)),
                            (work->ang.x & PIECES_FALL_THROUGH)
                                ? NO_FLOOR
                                : (FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                           : work->pos.y),
                            wood_box_pieces[i].model,
                            wood_box_pieces[i].texlist);
                    }
                }
            }
            work->submode = MODE_GONE;
        }
        if (player_records[0]->upgrades & NO_PUSH_UPGRADES) {
            work->collision->shapes->flags |= SHAPE_HURT_NO_PUSH;
        } else {
            work->collision->shapes->flags &= ~SHAPE_HURT_NO_PUSH;
            collision_register(task);
        }
        break;
    case MODE_GONE:
        /* @hack redundant: the chance masked by nibble_mask after the shift,
         * where the shift alone would do */
        chance = ((ANIMAL(work) & ANIMAL_CHANCE_BITS) >> ANIMAL_CHANCE_SHIFT) &
                 nibble_mask;
        if (chance && task->placement != NULL && !placement_unk_20(task)) {
            /* @hack spelling: odds *= TENTH in a statement of its own, where
             * (Float)chance * TENTH * TENTH would do */
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

static void wood_box_draw(struct task *task)
{
    /* @hack spelling: see_through set before work, where work first would
     * do */
    Bool see_through = FALSE;
    struct task_work *work = task->work;

    /* @hack spelling: the switch's last case without its break, where one
     * would do */
    switch (stage_current_id()) {
    case STAGE_BOSS_BIGFOOT:
    case STAGE_BOSS_HOTSHOT:
    case STAGE_BOSS_FDOG: {
        Float best = TARGET_RANGE;
        struct task *nearest = NULL;
        NJS_VECTOR offset;
        Float distance;
        int i;

        vector_diff(&work->pos, &camera_views[camera_current_index]->pos,
                    &offset);
        if (njScalor2(&offset) < SEE_THROUGH_RANGE2) {
            see_through = TRUE;
            for (i = 0; i < STACK_TASKS; i++) {
                /* @bug memory: the draw reads stack_tasks[i] without testing
                 * stack_tasks for null (0x7e2 of the object's code).
                 * stack_remove and stack_drop test it. the list is null where
                 * stack_add couldn't allocate it, and the box still draws
                 * then */
#ifdef NONMATCHING
                /* this side repairs the tag above: without the list, the scan
                 * finds no box, as for a list this box isn't in */
                if (stack_tasks == NULL) {
                    break;
                }
#endif
                if (stack_tasks[i] == NULL) {
                    continue;
                }
                if (stack_tasks[i]->step != wood_box_step) {
                    continue;
                }
                vector_diff(&stack_tasks[i]->work->pos,
                            &camera_views[camera_current_index]->pos, &offset);
                if (njScalor2(&offset) > SEE_THROUGH_RANGE2) {
                    continue;
                }
                vector_diff(&camera_views[camera_current_index]->target,
                            &stack_tasks[i]->work->pos, &offset);
                /* @hack spelling: the assignment to distance inside the
                 * test, where a statement of its own would do */
                if ((distance = njScalor(&offset)) < best) {
                    best = distance;
                    nearest = stack_tasks[i];
                }
            }
            if (nearest != task) {
                return;
            }
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(SEE_THROUGH_ATTR_AND, SEE_THROUGH_ATTR_OR);
            constant_material_set(SEE_THROUGH_ALPHA, 1.0F, 1.0F, 1.0F);
        }
    }
    }
    njSetTexture(&wood_box_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    if (wood_box_built && !see_through) {
        njDirectDrawObject(wood_box_builds[BOX_BUILD].built);
    } else {
        njCnkEasyDrawModel(&wood_box_model);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    if (see_through) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njPopMatrixEx();
    /* @hack redundant: a block no path reaches, kept for the SET editor's
     * captions, which SHC still places in this source's constants */
    if (0) {
        (void)"<- CRASHMODE(THROUGH)";
        (void)"<- CRASHMODE(REFRECT)";
        (void)"<- ANGLE Y";
        (void)"<- MINIMAL AP(  ) LEV( )";
    }
}

#endif
