/* the Chao box, CONTCHAO in a stage's object list: a box a player's attack
 * breaks. it leaves the stage's Chao key, and it falls when a box under it
 * breaks */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/animal.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/chao_box.h"
#include "shared/chao_key.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/stage_texture_groups.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the box is 2 * HALF_SIZE on a side. its work's pos is the center of its
 * bottom face */
#define HALF_SIZE 10.0F

/* a box's modes, in its work's submode. a MODE_BROKEN box's pieces have yet
 * to fly */
enum chao_box_mode { MODE_WHOLE, MODE_BROKEN, MODE_GONE };

/* a broken box's timer counts its steps from 0. the tasks stacked on it fall
 * on the step its timer is DROP_STEP. its pieces fly once the timer passes
 * PIECES_STEP */
#define DROP_STEP 1
#define PIECES_STEP 2

/* the box keeps its floor's height in its task's extra, as a Float's bits.
 * task_destroy frees a non-null extra, so the destructor nulls extra first */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FLOOR(task) (*(Float *)&(task)->extra)
/* FLOOR(task) is NO_FLOOR until a probe finds a floor. a whole box probes
 * every FLOOR_STEPS steps, a power of 2. each probe looks down from
 * FLOOR_LOOK_HEIGHT above the box's pos */
#define FLOOR_STEPS 32
#define FLOOR_LOOK_HEIGHT 5.0F

/* FALL_HEIGHT(work) is the height the box has left to fall, which
 * stack_drop adds to. FALL_SPEED(work) is its speed along y, 0 while it
 * rests. each step the speed keeps FALL_DRAG of itself and loses
 * FALL_GRAVITY */
#define FALL_HEIGHT(work) ((work)->scl.y)
#define FALL_SPEED(work) ((work)->scl.z)
#define FALL_DRAG 0.99F
#define FALL_GRAVITY 0.08F
/* a box that lands less than DUST_HEIGHT above or below its floor raises
 * dust */
#define DUST_HEIGHT 5.0F
#define DUST_SCALE 8.0F
#define DUST_DENSITY 1.2F
#define DUST_SPEED 0.5F

#define BREAK_SOUND 0x1011
#define SOUND_PRIORITY 1
#define BREAK_SOUND_DURATION 80
#define BREAK_POINTS 20
/* a box's break bursts BREAK_PUFFS puffs of dust about its center. each puff
 * starts on a sphere about the center, of radius PUFF_RADIUS plus up to
 * PUFF_RADIUS_RANGE. its speed is PUFF_SPEED plus up to PUFF_SPEED_RANGE,
 * and its size PUFF_SCALE plus up to PUFF_SCALE_RANGE */
#define BREAK_PUFFS 14
#define PUFF_RADIUS 7.0F
#define PUFF_RADIUS_RANGE 3.0F
#define PUFF_SPEED 0.5F
#define PUFF_SPEED_RANGE 1.0F
#define PUFF_SCALE 2.5F
#define PUFF_SCALE_RANGE 4.0F

/* while player 0 has any of these upgrades, the box's shape has
 * SHAPE_HURT_NO_PUSH. a broken box then registers no collision record */
#define NO_PUSH_UPGRADES                                                       \
    (UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT) |                                   \
     UPGRADE_BIT(UPGRADE_KNUCKLES_CLAWS) | UPGRADE_BIT(UPGRADE_ROUGE_NAILS))

/* the pieces fly straight away from the box's pos at PIECE_SPEED, plus up
 * to PIECE_SPREAD more at random. each also takes PIECE_RISE along y */
#define PIECE_SPEED 0.3F
#define PIECE_SPREAD 1.2F
#define PIECE_RISE 0.4F

/* the Chao key a box leaves starts up at KEY_SPEED */
#define KEY_SPEED 2.5F

/* where chao_key_taken returns TRUE, a box releases animals in the key's
 * place. the first such box releases BOX_SIDES animals. they start HALF_SIZE
 * out from its center along +z, +x, -z and -x. the second such box releases one
 * animal at its center */
#define BOX_SIDES 4

#define CHAO_BOX_PARTS 9
#define CHAO_BOX_BUILDS 3
#define CHAO_BOX_SHAPES 1

#include "shared/chao_box/text.inc"

/* The stage box and the three debris models it scatters. */
static NJS_TEXNAME chao_box_textures[2] = {
#include "shared/chao_box/chao_box_textures.inc"
};

NJS_TEXLIST chao_box_texlist = {
#include "shared/chao_box/chao_box_texlist.inc"
};

static Sint16 chao_box_polygons[83] = {
#include "shared/chao_box/chao_box_polygons.inc"
};

static Sint32 chao_box_vertices[51] = {
#include "shared/chao_box/chao_box_vertices.inc"
};

NJS_CNK_MODEL chao_box_model = {
#include "shared/chao_box/chao_box_model.inc"
};

NJS_CNK_OBJECT chao_box_object = {
#include "shared/chao_box/chao_box_object.inc"
};

/* Debris model 1. */

static NJS_TEXNAME chao_box_piece1_textures[2] = {
#include "shared/chao_box/chao_box_piece1_textures.inc"
};

NJS_TEXLIST chao_box_piece1_texlist = {
#include "shared/chao_box/chao_box_piece1_texlist.inc"
};

static Sint16 chao_box_piece1_polygons[75] = {
#include "shared/chao_box/chao_box_piece1_polygons.inc"
};

static Sint32 chao_box_piece1_vertices[51] = {
#include "shared/chao_box/chao_box_piece1_vertices.inc"
};

NJS_CNK_MODEL chao_box_piece1_model = {
#include "shared/chao_box/chao_box_piece1_model.inc"
};

NJS_CNK_OBJECT chao_box_piece1_object = {
#include "shared/chao_box/chao_box_piece1_object.inc"
};

/* Debris model 2. */

static NJS_TEXNAME chao_box_piece2_textures[1] = {
#include "shared/chao_box/chao_box_piece2_textures.inc"
};

NJS_TEXLIST chao_box_piece2_texlist = {
#include "shared/chao_box/chao_box_piece2_texlist.inc"
};

static Sint16 chao_box_piece2_polygons[25] = {
#include "shared/chao_box/chao_box_piece2_polygons.inc"
};

static Sint32 chao_box_piece2_vertices[27] = {
#include "shared/chao_box/chao_box_piece2_vertices.inc"
};

NJS_CNK_MODEL chao_box_piece2_model = {
#include "shared/chao_box/chao_box_piece2_model.inc"
};

NJS_CNK_OBJECT chao_box_piece2_object = {
#include "shared/chao_box/chao_box_piece2_object.inc"
};

/* Debris model 3. */

static NJS_TEXNAME chao_box_piece3_textures[1] = {
#include "shared/chao_box/chao_box_piece3_textures.inc"
};

NJS_TEXLIST chao_box_piece3_texlist = {
#include "shared/chao_box/chao_box_piece3_texlist.inc"
};

static Sint16 chao_box_piece3_polygons[25] = {
#include "shared/chao_box/chao_box_piece3_polygons.inc"
};

static Sint32 chao_box_piece3_vertices[27] = {
#include "shared/chao_box/chao_box_piece3_vertices.inc"
};

NJS_CNK_MODEL chao_box_piece3_model = {
#include "shared/chao_box/chao_box_piece3_model.inc"
};

NJS_CNK_OBJECT chao_box_piece3_object = {
#include "shared/chao_box/chao_box_piece3_object.inc"
};

NJS_TEXLIST *chao_box_texlists[6] = {
#include "shared/chao_box/chao_box_texlists.inc"
};

static void chao_box_destroy(struct task *task);
static void chao_box_step(struct task *task);
static void chao_box_draw(struct task *task);
static void chao_box_draw_built(struct task *task);

/* the pieces the box breaks into, round its sides and corners from +z, then
 * its top. the +z side's piece takes piece1's model, which the corners take,
 * where the other sides take piece2's */
/* @bug logic: the -x side's piece starts 10 below the box's center, where
 * the other sides' pieces start 10 above it. its y is the -10 at 0x694 of
 * the object's data */
static struct debris_part chao_box_parts[CHAO_BOX_PARTS] = {
#include "shared/chao_box/chao_box_parts.inc"
};
/* TRUE once one call of chao_box_build has compiled all three builds */
static Bool chao_box_built = FALSE;
/* the box's model, then its first two pieces'. only the box's compiled model
 * is drawn. the pieces fly with their models uncompiled */
static struct model_build chao_box_builds[CHAO_BOX_BUILDS] = {
#include "shared/chao_box/chao_box_builds.inc"
};
/* the box's shape, which its record and its landing child copy */
/* clang-format off */
static struct collision_shape chao_box_shapes[CHAO_BOX_SHAPES] = {
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(1), SHAPE_HURT_NO_PUSH | SHAPE_HURTABLE,
     {0.0F, HALF_SIZE, 0.0F}, HALF_SIZE, HALF_SIZE, HALF_SIZE, 0, {0, 0, 0}},
};
/* clang-format on */

Uint8 *chao_box_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, chao_box_builds, CHAO_BOX_BUILDS) ==
        CHAO_BOX_BUILDS) {
        chao_box_built = TRUE;
    }
    return buf;
}

void chao_box_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    if (chao_box_built) {
        task->draw = chao_box_draw_built;
    } else {
        task->draw = chao_box_draw;
    }
    task->step = chao_box_step;
    task->destructor = chao_box_destroy;
    work->submode = MODE_WHOLE;
    FLOOR(task) = NO_FLOOR;
    work->ang.x = 0;
    work->ang.z = 0;
    collision_init(task, chao_box_shapes, CHAO_BOX_SHAPES, COLLISION_OBJECT);
    FALL_HEIGHT(work) = 0.0F;
    FALL_SPEED(work) = 0.0F;
    stack_add(task);
}

static void chao_box_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    stack_remove(owner);
    owner->extra.p = NULL;
}

static void chao_box_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int player;
    int kind;

    if (work->submode == MODE_WHOLE && placement_try_leave(task)) {
        return;
    }
    if (FALL_HEIGHT(work) > 0.0F) {
        /* @hack spelling: height, a local for FALL_HEIGHT(work), where the
         * field read in place would do */
        Float height = FALL_HEIGHT(work);
        Float step = FALL_SPEED(work);

        if (height < -step) {
            step = -FALL_HEIGHT(work);
        }
        work->pos.y += step;
        FALL_HEIGHT(work) += step;
        FALL_SPEED(work) = (FALL_SPEED(work) * FALL_DRAG) - FALL_GRAVITY;
        /* a falling box keeps a child task with the box's shape, where the
         * box will land */
        if (task->child == NULL) {
            model_object_create(task, NULL, NULL, chao_box_shapes,
                                CHAO_BOX_SHAPES, 0);
        }
        if (task->child != NULL) {
            struct task_work *child = task->child->work;

            child->pos.x = work->pos.x;
            child->pos.z = work->pos.z;
            child->pos.y = work->pos.y - FALL_HEIGHT(work);
        }
    } else if (task->child != NULL) {
        task_destroy_children(task);
    }
    if (FALL_HEIGHT(work) <= 0.0F && FALL_SPEED(work) != 0.0F) {
        FALL_HEIGHT(work) = 0.0F;
        FALL_SPEED(work) = 0.0F;
        if (njAbs(work->pos.y - FLOOR(task)) < DUST_HEIGHT) {
            dust_short_rect(&work->pos, HALF_SIZE, HALF_SIZE, &work->ang,
                            DUST_SCALE, DUST_DENSITY, DUST_SPEED);
        }
    }
    switch (work->submode) {
    case MODE_WHOLE:
        if ((work->collision->flags & COLLISION_TOUCHED) &&
            player_attacks_task(task, work->collision->hit->owner)) {
            NJS_POINT3 center;

            center = work->pos;
            center.y += HALF_SIZE;
            sound_play_at_for(BREAK_SOUND, work, SOUND_PRIORITY, SOUND_LOUDEST,
                              BREAK_SOUND_DURATION, &work->pos);
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &center,
                    ((Float)rand() * RAND_SCALE * PUFF_RADIUS_RANGE) +
                        PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE * PUFF_SPEED_RANGE) +
                        PUFF_SPEED,
                    ((Float)rand() * RAND_SCALE * PUFF_SCALE_RANGE) +
                        PUFF_SCALE);
            }
            work->timer = 0;
            work->submode = MODE_BROKEN;
            score_add(BREAK_POINTS);
            /* @hack spelling: the assignment to player inside the test, where
             * a statement of its own would do */
            if ((player = player_slot_of(work->collision->hit->owner)) != -1) {
                player_set_speed(player, 0.0F, 0.0F, 0.0F);
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
            /* @hack spelling: at, a copy of the work's pos, where &work->pos
             * would do: debris_break doesn't write through pos */
            NJS_POINT3 at;
            NJS_VECTOR spd;

            at = work->pos;
            spd.x = 0.0F;
            spd.y = PIECE_RISE;
            spd.z = 0.0F;
            debris_break(&at, work->ang.y, chao_box_parts, CHAO_BOX_PARTS,
                         PIECE_SPEED, PIECE_SPREAD,
                         FLOOR(task) != NO_FLOOR ? FLOOR(task) : work->pos.y,
                         &spd);
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
        if (task->placement != NULL && !placement_unk_20(task) &&
            !placement_used(task)) {
            if (!chao_key_taken()) {
                chao_key_create(&work->pos,
                                FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                        : work->pos.y,
                                KEY_SPEED);
            } else {
                /* @hack spelling: i set to 0 before the test of
                 * CHAO_BOX_FOUR_ANIMALS, where the loop's head would do */
                i = 0;
                if (kept_object_bits & CHAO_BOX_FOUR_ANIMALS) {
                    if (!(kept_object_bits & CHAO_BOX_LAST_ANIMAL)) {
                        kept_object_bits |= CHAO_BOX_LAST_ANIMAL;
                        animal_release_uncounted(work->pos.x,
                                                 work->pos.y + HALF_SIZE,
                                                 work->pos.z, ANIMAL_FIFTH);
                    }
                } else {
                    kept_object_bits |= CHAO_BOX_FOUR_ANIMALS;
                    for (; i < BOX_SIDES; i++) {
                        if (i & 1) {
                            kind = ANIMAL_THIRD_OR_FOURTH;
                        } else {
                            kind = ANIMAL_FIRST_OR_SECOND;
                        }
                        animal_release_uncounted(
                            work->pos.x + (njSin(i * QUARTER_TURN) * HALF_SIZE),
                            work->pos.y + HALF_SIZE,
                            work->pos.z + (njCos(i * QUARTER_TURN) * HALF_SIZE),
                            kind);
                        /* @hack redundant: two continues that end the loop's
                         * body */
                        continue;
                        /* NOLINTNEXTLINE(readability-redundant-control-flow) */
                        continue;
                    }
                }
            }
        }
        if (task->placement != NULL) {
            placement_set_used(task);
            placement_set_unk_20(task);
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

static void chao_box_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, which leaves its string in
     * the object */
    if (0) {
        (void)CHAO_BOX_EDITOR_ANGLE_Y;
    }

    njSetTexture(&chao_box_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&chao_box_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
}

static void chao_box_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(chao_box_builds[0].texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njDirectDrawObject(chao_box_builds[0].built);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
