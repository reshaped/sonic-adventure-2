#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/deck_figure.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the figures, in kinds' order */
enum figure_id { FIGURE_ALLY, FIGURE_ENEMY, FIGURE_ALLY_2, FIGURE_COUNT };
/* the work's tick holds the figure's id */
#define FIGURE(work) ((work)->tick)

/* a figure's models and shapes. where far_distance is above 0, far_object
 * draws in object's place from that distance of the camera on. collision is
 * the solid's node, or null for a figure with no solid. built and far_built
 * are the models' builds, or null */
struct figure_kind {
    Uint32 flags;
    Float far_distance;
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *far_texlist;
    NJS_CNK_OBJECT *far_object;
    struct collision_shape *shapes;
    int shape_count;
    NJS_OBJECT *collision;
    NJS_DIRECT_HEAD *built;
    NJS_DIRECT_HEAD *far_built;
};

/* the record at task->extra: the solid's node, which the world holds, and
 * its model's center in the world */
struct figure_solid {
    NJS_OBJECT object;
    NJS_POINT3 center;
};
#define SOLID(task) ((struct figure_solid *)(task)->extra.p)
#define KIND(work) (kinds[FIGURE(work)])
/* a kind's flag: the figure draws with DRAW_CONTROL_SHADOW set */
#define AS_LAND 1
/* the solid's size class is that of its model's radius plus REACH_MARGIN.
 * the solid is in the world's collision only while a player is within that
 * sum of its center */
#define REACH_MARGIN 15.0F
/* base plus a random amount within half JITTER_SPAN either way */
#define JITTER(base) (((RANDOM_UNIT() - 0.5F) * JITTER_SPAN) + (base))
#define JITTER_SPAN 0.5F
/* a piece whose center lies within NEAR_ZERO of its node's origin flies
 * straight up. its speed is NEAR_ZERO before the jitter */
#define NEAR_ZERO 0.001F
/* each piece but the shadow's flies off at about a unit speed, plus
 * PIECE_LIFT upward. every piece's floor lies FLOOR_DROP below pos. every
 * node makes a piece while fewer than FEW_PIECES fly. past that, a node makes
 * one at a chance of CROWDED_CHANCE */
#define PIECE_LIFT 1.8F
#define FLOOR_DROP 0.5F
#define FEW_PIECES 10
#define CROWDED_CHANCE 0.4F
#define BREAK_POINTS 20

#include "shared/deck_figure/text.inc"

/* clang-format off */
#include "shared/deck_figure_art/ally.h"
#include "shared/deck_figure_art/ally_2.h"
#include "shared/deck_figure_art/enemy_far.h"
#include "shared/deck_figure_art/enemy.h"
#include "shared/deck_figure_art/solid.h"
/* clang-format on */

NJS_TEXLIST *deck_figure_texlists[5] = {
#include "shared/deck_figure/deck_figure_texlists.inc"
};
static struct collision_shape ally_target = {
#include "shared/deck_figure/ally_target.inc"
};
static struct collision_shape enemy_target = {
#include "shared/deck_figure/enemy_target.inc"
};

static struct figure_kind kinds[FIGURE_COUNT] = {
#include "shared/deck_figure/kinds.inc"
};

Uint8 *deck_figures_build(Uint8 *buffer)
{
    unsigned int i;
    for (i = 0; i < FIGURE_COUNT; ++i) {
        if (kinds[i].object) {
            njSetTexture(kinds[i].texlist);
            kinds[i].built = object_tree_build(&buffer, kinds[i].object);
        }
        if (kinds[i].far_object) {
            njSetTexture(kinds[i].far_texlist);
            kinds[i].far_built =
                object_tree_build(&buffer, kinds[i].far_object);
        }
    }
    return buffer;
}

static void figure_kill(struct task *task)
{
    task_kill(task);
}

static void figure_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *piece;
    NJS_POINT3 position;
    struct angles angles;
    NJS_VECTOR velocity;
    Float length;

    if (placement_try_leave(task)) {
        return;
    }
    if (task->extra.p && SOLID(task)->object.model) {
        if (player_near(&SOLID(task)->center,
                        SOLID(task)->object.model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    if (enemy_take_hit(work, task->motion.p)) {
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        piece = KIND(work).object;
        while (piece) {
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            angles.x = piece->ang[0];
            angles.y = piece->ang[1];
            angles.z = piece->ang[2];
#else
            /* @hack type: the three angles copied as one struct angles,
             * where assigning x, y and z would do */
            angles = *(struct angles *)piece->ang;
#endif
            angles.y += work->ang.y;
            njCalcPoint(NULL, (NJS_POINT3 *)piece->pos, &position);
            njPushMatrixEx();
            njTranslateEx((NJS_VECTOR *)piece->pos);
            njRotateZ(NULL, piece->ang[2]);
            njRotateY(NULL, piece->ang[1]);
            njRotateX(NULL, piece->ang[0]);
            njCalcVector(NULL, &piece->model->center, &velocity);
            njPopMatrixEx();
            length = njScalor(&velocity);
            if (length > NEAR_ZERO) {
                length = 1.0F / length;
                velocity.x *= length;
                velocity.y *= length;
                velocity.z *= length;
            } else {
                velocity.x = 0.0F;
                velocity.y = NEAR_ZERO;
                velocity.z = 0.0F;
            }
            velocity.x += JITTER(0.0F);
            velocity.y += JITTER(PIECE_LIFT);
            velocity.z += JITTER(0.0F);
            if (debris_count() < FEW_PIECES || RANDOM_UNIT() < CROWDED_CHANCE) {
                if (piece == &deck_enemy_shadow_node ||
                    piece == &deck_ally_shadow_node) {
                    velocity.x = 0.0F;
                    velocity.y = 0.0F;
                    velocity.z = 0.0F;
                    debris_launch_flags(&position, &angles,
                                        work->pos.y - FLOOR_DROP, &velocity,
                                        piece->model, KIND(work).texlist,
                                        DEBRIS_NO_SPIN_X | DEBRIS_NO_SPIN_Y |
                                            DEBRIS_NO_SPIN_Z | DEBRIS_VANISH);
                } else {
                    debris_launch_flags(&position, &angles,
                                        work->pos.y - FLOOR_DROP, &velocity,
                                        piece->model, KIND(work).texlist,
                                        DEBRIS_LANDING_BURST | DEBRIS_VANISH);
                }
            }
            if (piece->sibling) {
                piece = piece->sibling;
            } else {
                piece = piece->child;
            }
        }
        njPopMatrixEx();
        score_add(BREAK_POINTS);
        work->collision->shapes->flags |= SHAPE_OFF;
        enemy_defeat(work, task->motion.p);
        if (task->placement) {
            placement_stop(task);
        }
        task->step = figure_kill;
    } else {
        collision_register(task);
    }
}

static void figure_destroy(struct task *task)
{
    if (task->extra.p && SOLID(task)->object.model) {
        world_remove(task, &SOLID(task)->object);
    }
    syFree(task->extra.p);
    task->extra.p = NULL;
    enemy_destroy(task);
}

/* draw work's figure from the kind's fields named texlist, object and built.
 * they are the near model's or the far one's */
#define DRAW_FIGURE(work, texlist, object, built)                              \
    do {                                                                       \
        njSetTexture(KIND(work).texlist);                                      \
        njPushMatrixEx();                                                      \
        njTranslateEx(&(work)->pos);                                           \
        njRotateY(NULL, (work)->ang.y);                                        \
        if (KIND(work).built) {                                                \
            njDirectDrawObject(KIND(work).built);                              \
        } else {                                                               \
            njCnkEasyDrawObject(KIND(work).object);                            \
        }                                                                      \
        njPopMatrixEx();                                                       \
    } while (0)

static void figure_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (KIND(work).flags & AS_LAND) {
        draw_control_set(DRAW_CONTROL_SHADOW);
    }
    if (KIND(work).far_distance > 0.0F) {
        if (njDistanceP2P(&camera_views[camera_current_index]->pos,
                          &work->pos) < KIND(work).far_distance) {
            DRAW_FIGURE(work, texlist, object, built);
        } else {
            DRAW_FIGURE(work, far_texlist, far_object, far_built);
        }
    } else {
        DRAW_FIGURE(work, texlist, object, built);
    }
    if (KIND(work).flags & AS_LAND) {
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
}

static void figure_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *node;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the goto past the solid's setup, where an if around it
     * would do */
    if (!KIND(work).collision) {
        goto callbacks;
    }
    task->extra.p = syCalloc(1, sizeof(struct figure_solid));
    if (!task->extra.p) {
        return;
    }
    flags = WORLD_SOLID;
    node = &SOLID(task)->object;
    *node = *KIND(work).collision;
    node->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    node->ang[1] = work->ang.y;
    node->ang[2] = 0;
    node->ang[0] = 0;
    node->pos[0] = work->pos.x;
    node->pos[1] = work->pos.y;
    node->pos[2] = work->pos.z;
    radius = node->model->r + REACH_MARGIN;
    if (!(radius > WORLD_SIZE_CLASS_1_MAX)) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(2);
        } else {
            flags = WORLD_SOLID | WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, node);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx((NJS_VECTOR *)SOLID(task)->object.pos);
    njRotateY(NULL, SOLID(task)->object.ang[1]);
    njCalcPoint(NULL, &SOLID(task)->object.model->center, &SOLID(task)->center);
    njPopMatrixEx();
callbacks:
    task->step = figure_step;
    task->destructor = figure_destroy;
    task->draw = figure_draw;
    collision_init(task, KIND(work).shapes, KIND(work).shape_count,
                   COLLISION_ITEM);
    enemy_work_alloc(task);
    if (task->motion.p) {
        struct enemy_work *enemy = task->motion.p;
        enemy->flags |= ENEMY_BURST;
    }
}

void deck_figure_ally_init(struct task *task)
{
    struct task_work *work = task->work;
    FIGURE(work) = FIGURE_ALLY;
    figure_init(task);
}

void deck_figure_ally_2_init(struct task *task)
{
    struct task_work *work = task->work;
    FIGURE(work) = FIGURE_ALLY_2;
    figure_init(task);
}

void deck_figure_enemy_init(struct task *task)
{
    struct task_work *work = task->work;
    FIGURE(work) = FIGURE_ENEMY;
    figure_init(task);
}
