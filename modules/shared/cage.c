/* the cage, ORI in a stage's object list: a box that a missile of its id
 * breaks. a CAGE_BREAK_SHAPE_ID shape's touch breaks it too. it falls when a
 * box under it breaks */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/cage.h"
#include "shared/cage_models.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/missile.h"
#include "shared/object_builds.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/cage/text.inc"

/* the cage is 2 * HALF_SIZE on a side, and stands on its pos */
#define HALF_SIZE 10.0F

/* a cage's modes, in its work's submode. a MODE_BROKEN cage's pieces have
 * yet to fly */
enum cage_mode { MODE_WHOLE, MODE_BROKEN, MODE_GONE };

/* the cage's record, at task->motion */
struct cage {
    /* where a missile of the cage's id landed, or null. missile_target_add
     * takes its address */
    NJS_POINT3 *hit;
};

#define CAGE(task) ((struct cage *)(task)->motion.p)

/* the steps a broken cage waits, in task->unk_3c, before its work's timer
 * counts. a touch sets the wait to 1. a missile sets it to the whole units by
 * which the cage lies past BLAST_REACH from the blast */
#define BREAK_DELAY(task) ((task)->unk_3c.l)
#define BLAST_REACH 18.0F
/* once the wait is over, the boxes on the cage fall on the step its timer is
 * DROP_STEP. its pieces fly once the timer passes PIECES_STEP */
#define DROP_STEP 1
#define PIECES_STEP 2

/* the height of the floor under the cage, kept as a Float in its task's extra
 * word. the destructor clears extra, which task_destroy would free */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FLOOR(task) (*(Float *)&(task)->extra)
/* FLOOR is NO_FLOOR until the cage finds a floor. a whole cage looks for one
 * every FLOOR_STEPS steps, down from FLOOR_LOOK_HEIGHT above its pos */
#define FLOOR_STEPS 32
#define FLOOR_LOOK_HEIGHT 5.0F

/* a falling cage keeps the height it has left to fall in scl.y, which
 * stack_drop raises. it keeps its speed in y in scl.z. each step scales the
 * speed by FALL_DRAG and takes FALL_GRAVITY off it. a cage that lands within
 * DUST_HEIGHT of its floor raises dust */
#define FALL_DRAG 0.99F
#define FALL_GRAVITY 0.08F
#define DUST_HEIGHT 5.0F

/* breaking a cage plays BREAK_SOUND by sound_play_at_for, at FULL_VOLUME and
 * for a duration of BREAK_SOUND_DURATION. it scores BREAK_POINTS */
#define BREAK_SOUND 0x1011
#define FULL_VOLUME 127
#define BREAK_SOUND_DURATION 80
#define BREAK_POINTS 20
/* the puffs a landing cage raises along its sides: their size, how densely
 * they lie, and their speed */
#define LANDING_DUST_SCALE 8.0F
#define LANDING_DUST_DENSITY 1.2F
#define LANDING_DUST_SPEED 0.5F
/* as the pieces fly, BREAK_PUFFS puffs of dust burst from the cage's center.
 * a puff's radius runs from PUFF_RADIUS up to PUFF_RADIUS + PUFF_RADIUS_RANGE.
 * its speed runs from PUFF_SPEED up to PUFF_SPEED + 1. its size runs from
 * PUFF_SCALE up to PUFF_SCALE + PUFF_SCALE_RANGE */
#define BREAK_PUFFS 14
#define PUFF_RADIUS 7.0F
#define PUFF_RADIUS_RANGE 3.0F
#define PUFF_SPEED 0.5F
#define PUFF_SCALE 2.5F
#define PUFF_SCALE_RANGE 4.0F

/* the crash mode, ang.x's CRASH_MODE_BITS modulo CRASH_MODES, says what a
 * broken cage's pieces do */
#define CRASH_MODE_BITS 0xf
#define CRASH_MODES 3
#define CRASH_BOUNCE 0 /* they bounce on the cage's floor */
#define CRASH_THROUGH 1 /* they fall through the cage's floor */
#define CRASH_NO_PIECES 2 /* the cage throws none */
/* the bits of ang.z that hold the cage's missile target id */
#define TARGET_ID_BITS 0x1f

/* a cage's pieces fly from its center at speeds from PIECE_SPEED up to
 * PIECE_SPEED + PIECE_SPEED_RANGE */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F

static void cage_destroy(struct task *task);
static void cage_step(struct task *task);
static void cage_draw(struct task *task);
static void cage_draw_built(struct task *task);

/* The cage, its three debris models and its basic box mesh. */
/* clang-format off */
static NJS_TEXNAME cage_textures[1] = {
#include "shared/cage/cage_textures.inc"
};

NJS_TEXLIST cage_texlist = {
#include "shared/cage/cage_texlist.inc"
};

static Sint16 cage_polygons[895] = {
#include "shared/cage/cage_polygons.inc"
};

static Sint32 cage_vertices[915] = {
#include "shared/cage/cage_vertices.inc"
};

NJS_CNK_MODEL cage_model = {
#include "shared/cage/cage_model.inc"
};

NJS_CNK_OBJECT cage_object = {
#include "shared/cage/cage_object.inc"
};

static NJS_TEXNAME cage_piece1_textures[1] = {
#include "shared/cage/cage_piece1_textures.inc"
};

NJS_TEXLIST cage_piece1_texlist = {
#include "shared/cage/cage_piece1_texlist.inc"
};

static Sint16 cage_piece1_polygons[111] = {
#include "shared/cage/cage_piece1_polygons.inc"
};

static Sint32 cage_piece1_vertices[93] = {
#include "shared/cage/cage_piece1_vertices.inc"
};

NJS_CNK_MODEL cage_piece1_model = {
#include "shared/cage/cage_piece1_model.inc"
};

NJS_CNK_OBJECT cage_piece1_object = {
#include "shared/cage/cage_piece1_object.inc"
};

static NJS_TEXNAME cage_piece2_textures[1] = {
#include "shared/cage/cage_piece2_textures.inc"
};

NJS_TEXLIST cage_piece2_texlist = {
#include "shared/cage/cage_piece2_texlist.inc"
};

static Sint16 cage_piece2_polygons[143] = {
#include "shared/cage/cage_piece2_polygons.inc"
};

static Sint32 cage_piece2_vertices[129] = {
#include "shared/cage/cage_piece2_vertices.inc"
};

NJS_CNK_MODEL cage_piece2_model = {
#include "shared/cage/cage_piece2_model.inc"
};

NJS_CNK_OBJECT cage_piece2_object = {
#include "shared/cage/cage_piece2_object.inc"
};

static NJS_TEXNAME cage_piece3_textures[1] = {
#include "shared/cage/cage_piece3_textures.inc"
};

NJS_TEXLIST cage_piece3_texlist = {
#include "shared/cage/cage_piece3_texlist.inc"
};

static Sint16 cage_piece3_polygons[281] = {
#include "shared/cage/cage_piece3_polygons.inc"
};

static Sint32 cage_piece3_vertices[225] = {
#include "shared/cage/cage_piece3_vertices.inc"
};

NJS_CNK_MODEL cage_piece3_model = {
#include "shared/cage/cage_piece3_model.inc"
};

NJS_CNK_OBJECT cage_piece3_object = {
#include "shared/cage/cage_piece3_object.inc"
};

/* SHC accepts the two packed material colors as scalars. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL cage_bounds_material = {
#include "shared/cage/cage_bounds_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 cage_bounds_indices[19] = {
#include "shared/cage/cage_bounds_indices.inc"
};

static NJS_MESHSET cage_bounds_meshset = {
#include "shared/cage/cage_bounds_meshset.inc"
};

static NJS_POINT3 cage_bounds_points[8] = {
#include "shared/cage/cage_bounds_points.inc"
};

static NJS_VECTOR cage_bounds_normals[8] = {
#include "shared/cage/cage_bounds_normals.inc"
};

static NJS_MODEL cage_bounds_model = {
#include "shared/cage/cage_bounds_model.inc"
};

NJS_OBJECT cage_bounds_object = {
#include "shared/cage/cage_bounds_object.inc"
};

NJS_TEXLIST *cage_texlists[5] = {
#include "shared/cage/cage_texlists.inc"
};

/* clang-format on */
KEPT static struct cage_piece cage_pieces[CAGE_PIECES] = {
#include "shared/cage/cage_pieces.inc"
};
static Bool cage_built = FALSE;
/* the cage's model, then its pieces' in cage_pieces' order */
static struct model_build cage_builds[CAGE_BUILDS] = {
#include "shared/cage/cage_builds.inc"
};

/* the cage's shape, which every cage's collision record uses in place */
/* clang-format off */
static struct collision_shape cage_shapes[CAGE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0,
     {0.0F, HALF_SIZE, 0.0F}, HALF_SIZE, HALF_SIZE, HALF_SIZE, 0, {0, 0, 0}},
};
/* clang-format on */

/* the crash modes' labels, by mode, which no code reads */
static const char *cage_crash_labels[CRASH_MODES] = {
#include "shared/cage/cage_crash_labels.inc"
};

Uint8 *cage_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, cage_builds, CAGE_BUILDS) == CAGE_BUILDS) {
        cage_built = TRUE;
    }
    return buf;
}

void cage_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct cage));
    if (task->motion.p == NULL) {
        return;
    }
    if (cage_built) {
        task->draw = cage_draw_built;
    } else {
        task->draw = cage_draw;
    }
    task->step = cage_step;
    task->destructor = cage_destroy;
    work->submode = MODE_WHOLE;
    FLOOR(task) = NO_FLOOR;
    /* @hack redundant: a store of null to hit, which syCalloc has zeroed */
    CAGE(task)->hit = NULL;
    work->ang.x &= CRASH_MODE_BITS;
    work->ang.z &= TARGET_ID_BITS;
    collision_init_in_place(task, cage_shapes, CAGE_SHAPES, COLLISION_OBJECT);
    /* @hack redundant: a second mask of ang.z with TARGET_ID_BITS, after the
     * statement that masked it */
    missile_target_add(work->ang.z & TARGET_ID_BITS, &CAGE(task)->hit);
    stack_add(task);
    work->scl.y = 0.0F;
    work->scl.z = 0.0F;
}

static void cage_destroy(struct task *task)
{
    missile_target_remove(&CAGE(task)->hit);
    stack_remove(task);
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void cage_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    int j;
    int crash;
    /* @hack spelling: crash_bits and crash_modes, locals for CRASH_MODE_BITS
     * and CRASH_MODES, where the constants in place would do */
    int crash_bits;
    int crash_modes;

    /* the cage keeps its modes in its work's submode, and never sets the mode
     * this tests */
    if (work->mode == 0 && placement_try_leave(task)) {
        return;
    }
    crash_bits = CRASH_MODE_BITS;
    crash_modes = CRASH_MODES;
    if (work->scl.y > 0.0F) {
        /* @hack spelling: height, a local for scl.y, where scl.y read in
         * place would do */
        Float height = work->scl.y;
        Float step = work->scl.z;

        if (-step > height) {
            step = -work->scl.y;
        }
        work->pos.y += step;
        work->scl.y += step;
        work->scl.z = (work->scl.z * FALL_DRAG) - FALL_GRAVITY;
        /* while the cage falls, a child with a copy of its shape stands where
         * the cage will land */
        if (task->child == NULL) {
            model_object_create(task, NULL, NULL, cage_shapes, CAGE_SHAPES, 0);
        }
        if (task->child != NULL) {
            struct task_work *child = task->child->work;

            child->pos.x = work->pos.x;
            child->pos.z = work->pos.z;
            child->pos.y = work->pos.y - work->scl.y;
        }
    } else if (task->child != NULL) {
        task_destroy_children(task);
    }
    if (work->scl.y <= 0.0F && work->scl.z != 0.0F) {
        work->scl.y = 0.0F;
        work->scl.z = 0.0F;
        if (njAbs(work->pos.y - FLOOR(task)) < DUST_HEIGHT) {
            dust_short_rect(&work->pos, HALF_SIZE, HALF_SIZE, &work->ang,
                            LANDING_DUST_SCALE, LANDING_DUST_DENSITY,
                            LANDING_DUST_SPEED);
        }
    }
    switch (work->submode) {
    case MODE_WHOLE:
        if (CAGE(task)->hit != NULL ||
            collision_first_with_id(task, CAGE_BREAK_SHAPE_ID) != NULL) {
            Float delay;

            if (CAGE(task)->hit != NULL) {
                delay =
                    njDistanceP2P(&work->pos, CAGE(task)->hit) - BLAST_REACH;
                if (delay < 0.0F) {
                    delay = 0.0F;
                }
                BREAK_DELAY(task) = delay;
            } else {
                BREAK_DELAY(task) = 1;
            }
            /* @hack redundant: a block no path reaches, which adds delay and
             * four fields of the work into a fifth */
            if (0) {
                work->pos.y = work->scl.y + work->scl.z + work->pos.x +
                              work->pos.z + delay;
            }
            missile_target_remove(&CAGE(task)->hit);
            work->timer = 0;
            work->submode = MODE_BROKEN;
            sound_play_at_for(BREAK_SOUND, work, 1, FULL_VOLUME,
                              BREAK_SOUND_DURATION, &work->pos);
            score_add(BREAK_POINTS);
            /* @hack spelling: collision_register called in both arms, where
             * one call after them would do */
            collision_register(task);
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
            collision_register(task);
        }
        break;
    case MODE_BROKEN:
        if (work->timer == 0 && BREAK_DELAY(task) != 0) {
            BREAK_DELAY(task)--;
            collision_register(task);
            return;
        }
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
            /* @hack spelling: the crash mode worked out inside the test in each
             * arm, where a statement of its own before the test of cage_built
             * would do */
            if (cage_built) {
                if ((crash = (work->ang.x & crash_bits) % crash_modes) !=
                    CRASH_NO_PIECES) {
                    for (i = 0; i < CAGE_PIECES; i++) {
                        for (j = 0; j < cage_pieces[i].count; j++) {
                            debris_throw_built(
                                &center, DEBRIS_THROW_SQUARE, &scale,
                                work->ang.y,
                                PIECE_SPEED +
                                    ((Float)rand() *
                                     (PIECE_SPEED_RANGE * RAND_SCALE)),
                                crash == CRASH_THROUGH
                                    ? NO_FLOOR
                                    : (FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                               : work->pos.y),
                                cage_pieces[i].model, cage_builds[i + 1].built,
                                cage_pieces[i].texlist);
                        }
                    }
                }
            } else {
                if ((crash = (work->ang.x & crash_bits) % crash_modes) !=
                    CRASH_NO_PIECES) {
                    for (i = 0; i < CAGE_PIECES; i++) {
                        for (j = 0; j < cage_pieces[i].count; j++) {
                            debris_throw(
                                &center, DEBRIS_THROW_SQUARE, &scale,
                                work->ang.y,
                                PIECE_SPEED +
                                    ((Float)rand() *
                                     (PIECE_SPEED_RANGE * RAND_SCALE)),
                                crash == CRASH_THROUGH
                                    ? NO_FLOOR
                                    : (FLOOR(task) != NO_FLOOR ? FLOOR(task)
                                                               : work->pos.y),
                                cage_pieces[i].model, cage_pieces[i].texlist);
                        }
                    }
                }
            }
            for (i = 0; i < BREAK_PUFFS; i++) {
                dust_short_sphere(
                    &center,
                    ((Float)rand() * RAND_SCALE * PUFF_RADIUS_RANGE) +
                        PUFF_RADIUS,
                    ((Float)rand() * RAND_SCALE) + PUFF_SPEED,
                    ((Float)rand() * RAND_SCALE * PUFF_SCALE_RANGE) +
                        PUFF_SCALE);
            }
            work->submode = MODE_GONE;
        }
        collision_register(task);
        break;
    case MODE_GONE:
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        break;
    }
}

static void cage_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: the retail compiler retains the SET editor text. */
    if (0) {
        (void)CAGE_Y_ANGLE_LABEL;
        (void)CAGE_ID_LABEL;
    }

    njSetTexture(&cage_texlist);
    if (objects_fog_off) {
        njFogDisable();
    }
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njCnkEasyDrawModel(&cage_model);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
    if (objects_fog_off) {
        njFogEnable();
    }
}

static void cage_draw_built(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&cage_texlist);
    if (objects_fog_off) {
        njFogDisable();
    }
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njDirectDrawObject(cage_builds[0].built);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(1);
    if (objects_fog_off) {
        njFogEnable();
    }
}
