/* paper: pieces of a model that tumble, bounce on their floor and fade */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/paper.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

/* stg08/paper.c compiles this source again for Security Hall's banknotes,
 * and defines these two first. it also defines PAPER_DIRECT, under which the
 * call is security_paper_launch. that call takes a direct build that each
 * piece may draw in its model's place */
#ifndef PAPER_GRAVITY
#define PAPER_GRAVITY 0.02F
#endif
#ifndef PAPER_TASK_NAME
#define PAPER_TASK_NAME "EfMsPaper3DExec"
#endif

/* task_create's level for the pieces' task */
#define LEVEL 3

/* a random Float from -0.5 up to 0.5 */
#define RANDOM_HALF() (RANDOM_UNIT() - 0.5F)

/* a new piece's random spin about x, y and z spans these widths, in degrees a
 * step. each is written as a share of FULL_TURN, but paper_step reads it as
 * degrees. SPIN_X is about 18.2 degrees, SPIN_Y 10.9 and SPIN_Z 9.1 */
#define SPIN_X (FULL_TURN / 3600.0F)
#define SPIN_Y (FULL_TURN / 6000.0F)
#define SPIN_Z (FULL_TURN / 7200.0F)
/* SPIN(width) is a random whole number within width / 2 of 0 */
#define SPIN(width) ((int)(RANDOM_HALF() * (width)))

/* a piece that meets its floor at a falling speed under REST_SPEED comes to
 * rest there. REST_SPEED is REST_GRAVITIES times the speed gravity adds in a
 * step */
#define REST_GRAVITIES 5.0F
#define REST_SPEED (paper_gravity * REST_GRAVITIES)
/* on each step a piece rests on its floor, its alpha falls by FADE_STEP. the
 * alpha falls by as much again on each step past the piece's life */
#define FADE_STEP (1.0F / 15.0F)
/* a PAPER_TRAIL piece leaves a puff on a random TRAIL_CHANCE of its steps
 * above its floor. the puff moves at TRAIL_SPEED times the piece's speed */
#define TRAIL_CHANCE 0.1F
#define TRAIL_SPEED 0.7F
/* the scale of a piece's puffs and bursts, in radii of its model. their
 * depth_bias is the model's radius plus PARTICLE_BIAS */
#define PARTICLE_SIZE 0.3F
#define BIG_PARTICLE_SIZE 0.9F
#define PARTICLE_BIAS 1.0F
#define PARTICLE_SCALE(piece)                                                  \
    ((piece)->model->r * (((piece)->flags & PAPER_BIG_PARTICLES)               \
                              ? BIG_PARTICLE_SIZE                              \
                              : PARTICLE_SIZE))
/* a PAPER_FADE_COLORS piece's tint is WHITE under an alpha from 0 to
 * ALPHA_MAX, at ALPHA_SHIFT */
#define ALPHA_MAX 255
#define WHITE 0xffffff

/* one piece, from its making until it fades away */
struct paper {
    NJS_POINT3 pos; /* where its model's center is */
    NJS_VECTOR spd;
    struct angles ang;
    NJS_VECTOR spin; /* degrees a step */
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
#ifdef PAPER_DIRECT
    /* drawn in model's place, or null. a piece fading by PAPER_FADE_COLORS
     * draws model */
    NJS_DIRECT_HEAD *built;
#endif
    Float floor;
    Sint16 life; /* steps left */
    Sint16 flags;
    Float alpha; /* 1 is opaque. paper_step frees the piece at 0 or below */
    struct paper *next;
};

static Float paper_friction = 0.99F;
/* a bounce turns a piece's falling speed upward at paper_bounce of its size.
 * it scales the spin by paper_bounce too. on each step at rest, a piece keeps
 * paper_bounce of its speed along its floor after friction */
static Float paper_bounce = 0.6F;
static Float paper_gravity = PAPER_GRAVITY;
static Sint32 paper_life = 80;
static Sint32 paper_life_spread = 5;
static Float paper_gust_chance = 0.05F;
static Float paper_gust = 0.6F;
/* the task that steps and draws the pieces, or null */
static struct task *paper_task = NULL;
/* the pieces, the newest first */
static struct paper *paper_list = NULL;

static void paper_destroy(struct task *task);
static void paper_step(struct task *task);
static void paper_draw(struct task *task);

#ifdef PAPER_DIRECT
void security_paper_launch(NJS_POINT3 *pos, struct angles *ang, Float floor,
                           NJS_VECTOR *spd, struct paper_spin *spin,
                           NJS_CNK_MODEL *model, NJS_DIRECT_HEAD *built,
                           NJS_TEXLIST *texlist, Sint16 flags)
#else
void paper_launch(NJS_POINT3 *pos, struct angles *ang, Float floor,
                  NJS_VECTOR *spd, struct paper_spin *spin,
                  NJS_CNK_MODEL *model, NJS_TEXLIST *texlist, Sint16 flags)
#endif
{
    struct task *task;
    struct paper *piece;
    /* @hack spelling: center, a local for the piece's pos, where njCalcPoint
     * into piece->pos would do */
    NJS_POINT3 center;

    if (paper_task == NULL) {
        task = task_create(TASK_WORK, LEVEL, paper_step, PAPER_TASK_NAME);
        if (task != NULL) {
            task->draw = paper_draw;
            task->destructor = paper_destroy;
            paper_task = task;
        }
    }
    if (paper_task == NULL) {
        return;
    }
    piece = syCalloc(1, sizeof(struct paper));
    if (piece == NULL) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, pos);
    njRotateZ(NULL, ang->z);
    njRotateY(NULL, ang->y);
    /* @bug logic: turns the center about z by ang.x, where paper_draw turns
     * the piece about x by it (JSR @R2 at 0x7e of the object's code) */
    njRotateZ(NULL, ang->x);
    njCalcPoint(NULL, &model->center, &center);
    njPopMatrix(1);
    piece->flags = flags;
    piece->alpha = 1.0F;
    piece->life =
        (int)(rand() * (Float)paper_life_spread * RAND_SCALE) + paper_life;
    if (spin != NULL) {
        piece->spin.x = spin->x;
        piece->spin.y = spin->y;
        piece->spin.z = spin->z;
    } else {
        piece->spin.x = (flags & PAPER_NO_SPIN_X) ? 0 : SPIN(SPIN_X);
        piece->spin.y = (flags & PAPER_NO_SPIN_Y) ? 0 : SPIN(SPIN_Y);
        piece->spin.z = (flags & PAPER_NO_SPIN_Z) ? 0 : SPIN(SPIN_Z);
    }
    piece->model = model;
#ifdef PAPER_DIRECT
    piece->built = built;
#endif
    piece->texlist = texlist;
    piece->floor = floor;
    piece->pos = center;
    piece->ang = *ang;
    piece->spd = *spd;
    piece->next = paper_list;
    paper_list = piece;
}

static void paper_destroy(struct task *task)
{
    struct paper *piece;

    if (task == paper_task) {
        paper_task = NULL;
    }
    while (paper_list != NULL) {
        piece = paper_list;
        /* @hack spelling: the next piece read through paper_list, where
         * piece->next would do */
        paper_list = paper_list->next;
        syFree(piece);
    }
}

static void paper_step(struct task *task)
{
    struct paper *piece;
    struct paper *next;
    struct paper *prev;
    NJS_VECTOR spd;
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct task_work *work;

    work = task->work;
    prev = NULL;
    piece = paper_list;
    while (piece != NULL) {
        piece->pos.x += piece->spd.x;
        piece->pos.y += piece->spd.y;
        piece->pos.z += piece->spd.z;
        if (piece->floor >= piece->pos.y) {
            piece->spd.x *= paper_friction;
            piece->spd.z *= paper_friction;
            if (piece->flags & PAPER_THROUGH_FLOOR) {
                piece->spd.y *= paper_friction;
            } else {
                piece->pos.y = piece->floor;
                if (njAbs(piece->spd.y) < REST_SPEED) {
                    piece->spd.y = 0.0F;
                    piece->spd.x *= paper_bounce;
                    piece->spd.z *= paper_bounce;
                    piece->spin.x = 0.0F;
                    piece->spin.y = 0.0F;
                    piece->spin.z = 0.0F;
                    if (piece->alpha < FADE_STEP) {
                        piece->alpha = 0.0F;
                    } else {
                        piece->alpha -= FADE_STEP;
                    }
                } else {
                    piece->spd.y *= -paper_bounce;
                    piece->spin.x *= paper_bounce;
                    piece->spin.y *= paper_bounce;
                    piece->spin.z *= paper_bounce;
                }
            }
            if (piece->flags & PAPER_LANDING_BURST) {
                spd.x = 0.0F;
                spd.y = 0.0F;
                spd.z = 0.0F;
                piece->flags &= ~PAPER_LANDING_BURST;
                burst_create(&piece->pos, &spd, PARTICLE_SCALE(piece),
                             piece->model->r + PARTICLE_BIAS);
            }
            if (piece->flags & PAPER_VANISH) {
                piece->alpha = 0.0F;
            }
        } else {
            if ((piece->flags & PAPER_TRAIL) && RANDOM_UNIT() < TRAIL_CHANCE) {
                spd.x = piece->spd.x * TRAIL_SPEED;
                spd.y = piece->spd.y * TRAIL_SPEED;
                spd.z = piece->spd.z * TRAIL_SPEED;
                dust_steady_create(&piece->pos, &spd, PARTICLE_SCALE(piece),
                                   piece->model->r + PARTICLE_BIAS);
            }
            if ((piece->flags & PAPER_GUSTS) &&
                RANDOM_UNIT() < paper_gust_chance) {
                piece->spd.x += RANDOM_HALF() * paper_gust;
                piece->spd.z += RANDOM_HALF() * paper_gust;
            }
            piece->spd.x *= paper_friction;
            piece->spd.y = (piece->spd.y * paper_friction) - paper_gravity;
            piece->spd.z *= paper_friction;
        }
        piece->ang.x += (Angle)(piece->spin.x * ANGLE_PER_DEGREE);
        piece->ang.y += (Angle)(piece->spin.y * ANGLE_PER_DEGREE);
        piece->ang.z += (Angle)(piece->spin.z * ANGLE_PER_DEGREE);
        /* @hack spelling: x and y written x = x * friction, and z with *=,
         * where *= for all three would do */
        piece->spin.x = piece->spin.x * paper_friction;
        piece->spin.y = piece->spin.y * paper_friction;
        piece->spin.z *= paper_friction;
        if (piece->life) {
            piece->life--;
        } else {
            piece->alpha -= FADE_STEP;
        }
        if (piece->alpha <= 0.0F) {
            next = piece->next;
            if (prev != NULL) {
                prev->next = next;
            } else {
                paper_list = next;
            }
            syFree(piece);
            piece = next;
        } else {
            prev = piece;
            piece = piece->next;
        }
    }
    if (paper_list == NULL) {
        task_destroy(task);
    }
}

static void paper_draw(struct task *task)
{
    struct paper *piece;
    int alpha;
    Uint32 color;
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct task_work *work;

    work = task->work;
    piece = paper_list;
    njPushMatrixEx();
    while (piece != NULL) {
        njSetTexture(piece->texlist);
        njTranslateV(NULL, &piece->pos);
        njRotateZ(NULL, piece->ang.z);
        njRotateY(NULL, piece->ang.y);
        njRotateX(NULL, piece->ang.x);
        /* below full alpha, a piece draws in Ninja's constant material,
         * white at its alpha. each strip of its model draws with alpha and
         * without specular, whatever flags the strip holds */
        if (piece->alpha < 1.0F) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
            constant_material_set(piece->alpha, 1.0F, 1.0F, 1.0F);
        }
        njTranslate(NULL, -piece->model->center.x, -piece->model->center.y,
                    -piece->model->center.z);
        if ((piece->flags & PAPER_FADE_COLORS) && piece->alpha < 1.0F) {
            alpha = (int)(piece->alpha * ALPHA_MAX);
            if (alpha > ALPHA_MAX) {
                alpha = ALPHA_MAX;
            }
            if (alpha < 0) {
                alpha = 0;
            }
            color = (alpha << ALPHA_SHIFT) + WHITE;
            model_draw_tinted(piece->model, color);
#ifdef PAPER_DIRECT
        } else if (piece->built != NULL) {
            njDirectDrawObject(piece->built);
#endif
        } else {
            njCnkEasyDrawModel(piece->model);
        }
        if (piece->alpha < 1.0F) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
        matrix_load_parent();
        piece = piece->next;
    }
    njPopMatrixEx();
}
