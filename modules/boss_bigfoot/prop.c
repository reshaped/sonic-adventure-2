/* the props, ROBJ in the Big Foot stage's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_bigfoot/prop.h"
#include "draw/chunk.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/color.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/paper.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/task.h"
#include "system/util.h"

/* what a breaking kind leaves: a model in the prop's place, and the pieces
 * the prop breaks into. texlist and object are null for a kind that leaves
 * no model */
struct prop_wreck {
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    struct debris_built_part *pieces;
    Sint32 piece_count;
    NJS_DIRECT_HEAD *build; /* object's build, or null */
};

struct prop_kind {
    Uint32 flags; /* PROP_BUILD and the rest */
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    struct collision_shape *shapes; /* null for none */
    Sint32 shape_count;
    void (*draw)(struct task *task);
    /* the kind's own step, or null for a prop whose step only registers its
     * collision */
    void (*step)(struct task *task, struct task_work *work,
                 struct prop_kind *kind);
    struct prop_wreck *wreck; /* null for a kind that doesn't break */
    Sint32 unk_20;
    NJS_DIRECT_HEAD *build; /* object's build, once PROP_BUILT is set */
};

/* a kind's flags */
#define PROP_BUILD 0x1 /* prop_build compiles object */
/* the prop keeps its place and turn in a matrix. no kind has this flag */
#define PROP_MATRIX 0x2
/* prop_build and prop_draw_near take object's whole tree, not its root's
 * model alone */
#define PROP_TREE 0x4
#define PROP_PAPERS 0x8 /* the prop throws papers as it breaks */
#define PROP_BUILT 0x8000 /* prop_build has compiled object */

struct prop_table {
    struct prop_kind *kinds;
    Sint32 count;
};

/* what a prop keeps in its task's fields. motion points to its table of
 * kinds. unk_3c points to its matrix, or is null. a wobbling prop keeps its
 * tilt and the tilt's speed as Floats in force and extra. task_destroy would
 * free motion, force and extra as pointers, so the destructor nulls them */
#define TABLE(task) ((struct prop_table *)(task)->motion.p)
#define MATRIX(task) ((task)->unk_3c.p)
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define TILT(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define TILT_SPEED(task) (*(Float *)&(task)->extra)
/* the prop's kind, which its work's scl.x picks from its table */
#define KIND(task, work) (&TABLE(task)->kinds[(int)(work)->scl.x])

/* the placement keeps a prop's submode from one task to the next. a
 * breaking prop's submode holds its state */
#define BROKEN_HIT 0x1 /* hit: the prop breaks at its next step */
#define BROKEN_PLAYER_1 0x2 /* player 1 dealt the hit, not player 0 */
#define BROKEN_DONE 0x40 /* the break has thrown its pieces */
#define BROKEN_KEPT 0x80 /* the prop stands as its wreck */

/* the SET editor's labels, which no code reads. each size rounds its label
 * up to whole words */
static const char editor_model[16] = "<- MODEL(0-  )";
static const char editor_angle_y[12] = "<- ANGLE Y";

#include "boss_bigfoot/prop/text.inc"

/* clang-format off */
#include "shared/street_fixtures/trash_can.h"
#include "shared/street_fixtures/loose_paper.h"
#include "shared/street_fixtures/roadside_signs.h"
#include "shared/street_fixtures/street_bench.h"
#include "shared/street_fixtures/street_bench_wreck.h"
#include "shared/street_fixtures/street_bench_piece.h"
#include "shared/street_fixtures/tall_newspaper_box.h"
#include "shared/street_fixtures/tall_newspaper_box_piece.h"
#include "shared/street_fixtures/tall_newspaper_box_wreck.h"
#include "shared/street_fixtures/short_newspaper_box.h"
#include "shared/street_fixtures/short_newspaper_box_piece.h"
#include "shared/street_fixtures/short_newspaper_box_wreck.h"
#include "shared/street_fixtures/street_lamp.h"
#include "shared/street_fixtures/palm_planter.h"
#include "shared/street_fixtures/roadside_dome.h"
#include "shared/street_fixtures/stop_sign.h"
#include "shared/street_fixtures/ground_panel.h"
/* clang-format on */

static void prop_setup(struct task *task);
static void prop_step_unchecked(struct task *task);
/* prop_setup, but with a step that never tests the prop's range. nothing
 * calls it, but the image holds its code. it is external because SHC leaves
 * out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void prop_setup_unchecked(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */
static void prop_step(struct task *task);
static void prop_destroy(struct task *task);
static void prop_wobble(struct task *task, struct task_work *work,
                        struct prop_kind *kind);
static void prop_break(struct task *task, struct task_work *work,
                       struct prop_kind *kind);
static void prop_break_away(struct task *task, struct task_work *work,
                            struct prop_kind *kind);
static void prop_wobble_draw(struct task *task);
static void prop_draw(struct task *task);
static void prop_draw_scaled(struct task *task);
static void prop_draw_near(struct task *task);

#define PROP_KINDS 10
/* once a breaking prop's submode is set, the prop breaks at its next step
 * where BROKEN_HIT is set. after the break, or after BREAK_STEPS steps without
 * one, it stands as its wreck. where its kind leaves no model, it ends
 * instead */
#define BREAK_STEPS 8
#define HIT_SCORE 20 /* the score a hit on a prop adds */
/* a break throws its pieces under PIECE_FLAGS. a piece flies away from the
 * prop at PIECE_SPEED, plus up to PIECE_SPREAD at random. it also rises at
 * PIECE_LIFT. a hit through the weak point also moves each piece at
 * PIECE_PULL toward the player who dealt it. a PIECE_RADIUS below 0.1 spins
 * the pieces as debris_break does */
#define PIECE_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)
#define PIECE_SPEED 0.7F
#define PIECE_SPREAD 1.0F
#define PIECE_LIFT 0.8F
#define PIECE_PULL 0.6F
#define PIECE_RADIUS 0.0F
/* a break probes for the floor the pieces land on from FLOOR_LOOK_HEIGHT
 * above the prop's pos */
#define FLOOR_LOOK_HEIGHT 1.0F
/* a PROP_PAPERS kind's break throws PAPERS papers under PAPER_FLAGS. a paper
 * starts PAPER_HEIGHT of the way up to the model's center. it also starts
 * PAPER_LEAD steps' travel out along x and z */
#define PAPERS 8
#define PAPER_FLAGS (PAPER_TRAIL | PAPER_FADE_COLORS | PAPER_GUSTS)
#define PAPER_HEIGHT 0.38F
#define PAPER_LEAD 3.0F
/* a hurt to the weak point sets BROKEN_HIT, bit WEAK_POINT_INDEX * 2 of the
 * work's submode. it also sets BROKEN_PLAYER_1 where the weak point's
 * enemy_work's shot_by isn't 0 */
#define WEAK_POINT_INDEX 0
/* a wobbling prop's submode counts its steps from the hit, from 1 up to
 * DUST_STEPS. each step of the count puffs dust with a chance of
 * DUST_CHANCE, DUST_HEIGHT above the prop. a puff rises at DUST_RISE. its
 * scale is DUST_SCALE plus up to DUST_SCALE_SPREAD, and its depth bias
 * DUST_DEPTH_BIAS */
#define DUST_STEPS 60
#define DUST_CHANCE 0.3F
#define DUST_HEIGHT 26.0F
#define DUST_RISE 1.0F
#define DUST_SCALE 2.0F
#define DUST_SCALE_SPREAD 6.0F
#define DUST_DEPTH_BIAS 3.0F
/* the blink of the wobbling draw until the hit, as blink_level's period, rest
 * and fade. the model's child draws with the blink's level as its alpha. the
 * child's sibling takes 1 minus that level */
#define BLINK_PERIOD 50
#define BLINK_REST 25
#define BLINK_FADE 5
/* the tint takes the alpha, 0 to FULL_ALPHA, in its top byte over WHITE */
#define FULL_ALPHA 255
#define WHITE 0xffffff
/* prop_draw_near draws a prop within its model's radius plus NEAR_MARGIN of
 * the camera see-through, in NEAR_COLOR. it draws nothing of a prop farther
 * than that behind the camera */
#define NEAR_MARGIN 10.0F
#define NEAR_COLOR 0x60ffffff
NJS_TEXLIST *prop_texlists[18] = {
#include "boss_bigfoot/prop/prop_texlists.inc"
};
/* clang-format off */
static struct debris_built_part prop_0_pieces[1] = {
#include "boss_bigfoot/prop/prop_0_pieces.inc"
};
static struct prop_wreck prop_0_wreck = {
#include "boss_bigfoot/prop/prop_0_wreck.inc"
};
static struct debris_built_part prop_3_pieces[5] = {
#include "boss_bigfoot/prop/prop_3_pieces.inc"
};
static struct prop_wreck prop_3_wreck = {
#include "boss_bigfoot/prop/prop_3_wreck.inc"
};
static struct debris_built_part prop_4_pieces[4] = {
#include "boss_bigfoot/prop/prop_4_pieces.inc"
};
static struct prop_wreck prop_4_wreck = {
#include "boss_bigfoot/prop/prop_4_wreck.inc"
};
static struct debris_built_part prop_5_pieces[4] = {
#include "boss_bigfoot/prop/prop_5_pieces.inc"
};
static struct prop_wreck prop_5_wreck = {
#include "boss_bigfoot/prop/prop_5_wreck.inc"
};
/* no kind has shapes, so no prop has a collision record. kinds 0 and 3 break,
 * and kinds 6 and 7 wobble, only on a hit to that record. so these four never
 * break or wobble. kinds 4 and 5 break when their weak point is hurt */
static struct prop_kind prop_kinds[PROP_KINDS] = {
#include "boss_bigfoot/prop/prop_kinds.inc"
};
/* clang-format on */
static struct prop_table prop_table = {
#include "boss_bigfoot/prop/prop_table.inc"
};
/* the one wreck whose model and pieces prop_build compiles. the other
 * wrecks draw without a build */
static struct prop_wreck *prop_built_wreck = {
#include "boss_bigfoot/prop/prop_built_wreck.inc"
};
void prop_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &prop_table;
        prop_setup(task);
    }
}

Uint8 *prop_build(Uint8 *buf)
{
    /* @hack spelling: table, a local for prop_table's address, where
     * prop_table read in place would do */
    struct prop_table *table = &prop_table;
    int i;
    struct prop_kind *kind;
    Uint8 *end;
    NJS_CNK_OBJECT object;
    struct debris_built_part *pieces;
    /* @hack spelling: the wreck reached through its pointer's address, where
     * a copy of the pointer would do */
    struct prop_wreck **wreck;

    /* @hack spelling: a do-while under a test of the count, where a for would
     * do */
    i = 0;
    if (table->count > 0) {
        do {
            kind = &table->kinds[i];
            if ((kind->flags & PROP_BUILD) && !(kind->flags & PROP_BUILT)) {
                object = *kind->object;
                if (!(kind->flags & PROP_TREE)) {
                    object.sibling = NULL;
                    object.child = NULL;
                    object.evalflags |= NJD_EVAL_BREAK;
                }
                njSetTexture(kind->texlist);
                end = buf + njCnkDirectObjectCompileSize(&object);
                if (end < DEMO_AREA) {
                    njCnkDirectObjectCompile(&object, buf);
                    kind->build = (NJS_DIRECT_HEAD *)buf;
                    buf = end;
                    kind->flags |= PROP_BUILT;
                }
            }
        } while (++i < table->count);
    }
    wreck = &prop_built_wreck;
    pieces = (*wreck)->pieces;
    if ((*wreck)->texlist != NULL) {
        njSetTexture((*wreck)->texlist);
        (*wreck)->build = object_build(&buf, (*wreck)->object);
    } else {
        (*wreck)->build = NULL;
    }
    debris_build(&buf, pieces, (*wreck)->piece_count);
    return buf;
}

static void prop_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;

    task->step = prop_step;
    task->destructor = prop_destroy;
    if (work->scl.x < 0.0F) {
        work->scl.x = 0.0F;
    }
    if ((int)work->scl.x > TABLE(task)->count - 1) {
        work->scl.x = TABLE(task)->count - 1;
    }
    kind = KIND(task, work);
    if (kind->shapes != NULL) {
        collision_init(task, kind->shapes, kind->shape_count, COLLISION_OBJECT);
    }
    task->draw = kind->draw;
    MATRIX(task) = NULL;
    if (kind->flags & PROP_MATRIX) {
        MATRIX(task) = syMalloc(sizeof(NJS_MATRIX));
        if (MATRIX(task) != NULL) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njGetMatrix(MATRIX(task));
            njPopMatrixEx();
        }
    }
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
}

void prop_setup_unchecked(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;

    task->step = prop_step_unchecked;
    task->destructor = prop_destroy;
    if (work->scl.x < 0.0F) {
        work->scl.x = 0.0F;
    }
    if ((int)work->scl.x > TABLE(task)->count - 1) {
        work->scl.x = TABLE(task)->count - 1;
    }
    kind = KIND(task, work);
    if (kind->shapes != NULL) {
        collision_init(task, kind->shapes, kind->shape_count, COLLISION_OBJECT);
    }
    task->draw = kind->draw;
    MATRIX(task) = NULL;
    if (kind->flags & PROP_MATRIX) {
        MATRIX(task) = syMalloc(sizeof(NJS_MATRIX));
        if (MATRIX(task) != NULL) {
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njGetMatrix(MATRIX(task));
            njPopMatrixEx();
        }
    }
    if (task->placement != NULL) {
        work->submode = placement_get_state(task);
    }
}

static void prop_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (MATRIX(task) != NULL) {
        syFree(MATRIX(task));
        MATRIX(task) = NULL;
    }
    task->motion.p = NULL;
    task->extra.p = NULL;
    task->force.p = NULL;
    if (task->placement != NULL) {
        placement_set_state(task, work->submode);
    }
}

static void prop_step_unchecked(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;

    /* @hack redundant: a test of placement_try_leave that no path reaches */
    if (1) {
    } else if (placement_try_leave(task)) {
        return;
    }
    kind = KIND(task, work);
    if (kind->step != NULL) {
        kind->step(task, work, kind);
    } else if (work->collision != NULL) {
        collision_register(task);
    }
}

static void prop_step(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;

    if (placement_try_leave(task)) {
        return;
    }
    kind = KIND(task, work);
    if (kind->step != NULL) {
        kind->step(task, work, kind);
    } else if (work->collision != NULL) {
        collision_register(task);
    }
}

/* a wobbling prop's tilt runs from 0 to 1. each step, the tilt's speed keeps
 * TILT_KEEP of itself and gains tilt_pull. past 0 or 1, the tilt stops there
 * and turns back at TILT_BOUNCE of its speed. a tilt of 1 leans the prop
 * tilt_degrees about x, and half that about z */
#define TILT_KEEP 0.99F
#define TILT_BOUNCE 0.7F
static Float tilt_pull = 0.008F;
static Float tilt_degrees = 25.0F;
/* the wobbling step counts the work's mode from 0 to wobble_count. each time
 * the mode wraps, it counts the timer up, modulo 256. no other code reads the
 * mode or the timer */
static Sint32 wobble_count = 5;
static void prop_wobble(struct task *task, struct task_work *work,
                        struct prop_kind *kind)
{
    NJS_VECTOR spd;
    NJS_POINT3 pos;

    work->mode++;
    if (work->mode > wobble_count) {
        work->mode = 0;
        work->timer = (Uint8)(work->timer + 1);
    }
    if (work->submode) {
        TILT_SPEED(task) = (TILT_SPEED(task) * TILT_KEEP) + tilt_pull;
        TILT(task) += TILT_SPEED(task);
        if (TILT(task) > 1.0F) {
            TILT(task) = 1.0F;
            TILT_SPEED(task) = -njAbs(TILT_SPEED(task)) * TILT_BOUNCE;
        }
        if (TILT(task) < 0.0F) {
            TILT(task) = 0.0F;
            TILT_SPEED(task) = njAbs(TILT_SPEED(task)) * TILT_BOUNCE;
        }
        if (work->submode < DUST_STEPS) {
            if (rand() * RAND_SCALE < DUST_CHANCE) {
                pos.x = 0.0F;
                pos.y = DUST_HEIGHT;
                pos.z = 0.0F;
                spd.x = 0.0F;
                spd.y = DUST_RISE;
                spd.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateV(NULL, &work->pos);
                njRotateY(NULL, work->ang.y);
                njCalcPoint(NULL, &pos, &pos);
                njPopMatrixEx();
                dust_steady_create(&pos, &spd,
                                   (rand() * (DUST_SCALE_SPREAD * RAND_SCALE)) +
                                       DUST_SCALE,
                                   DUST_DEPTH_BIAS);
            }
            work->submode++;
        }
    } else {
        if (work->collision == NULL) {
            return;
        }
        if (collision_next_attack(task) != NULL || (work->flags & WORK_HURT)) {
            work->submode = 1;
            score_add(HIT_SCORE);
        }
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
}

/* a paper's speed is paper_rise up, plus up to paper_spread / 2 either way
 * along each axis. it spins about each axis at up to paper_spin / 2 degrees
 * a step either way */
static Float paper_rise = 1.2F;
static Float paper_spread = 0.38F;
static Float paper_spin = 0.17F;

/* a breaking kind's step: the prop breaks once an attack touches it, or once
 * it is hurt */
static void prop_break(struct task *task, struct task_work *work,
                       struct prop_kind *kind)
{
    if (work->submode != 0) {
        if (work->timer < BREAK_STEPS && !(work->submode & BROKEN_DONE)) {
            if (work->submode & BROKEN_HIT) {
                score_add(HIT_SCORE);
                if (kind->wreck != NULL) {
                    struct prop_wreck *wreck = kind->wreck;
                    NJS_POINT3 pos;
                    struct angles floor_ang;
                    NJS_VECTOR spd;
                    Float floor;
                    int i;

                    pos = work->pos;
                    floor = surface_probe_floor(
                        pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &floor_ang);
                    spd.x = 0.0F;
                    spd.y = PIECE_LIFT;
                    spd.z = 0.0F;
                    debris_break_built(&pos, work->ang.y, wreck->pieces,
                                       wreck->piece_count, PIECE_SPEED,
                                       PIECE_SPREAD, floor, &spd, PIECE_FLAGS,
                                       PIECE_RADIUS);
                    if (kind->flags & PROP_PAPERS) {
                        for (i = 0; i < PAPERS; i++) {
                            NJS_VECTOR paper_spd;
                            struct paper_spin spin;
                            NJS_POINT3 paper_pos;

                            paper_pos = work->pos;
                            paper_pos.y +=
                                kind->object->model->center.y * PAPER_HEIGHT;
                            spin.x = ((rand() * RAND_SCALE) - 0.5F) *
                                     paper_spin * ANGLE_PER_DEGREE;
                            spin.y = ((rand() * RAND_SCALE) - 0.5F) *
                                     paper_spin * ANGLE_PER_DEGREE;
                            spin.z = ((rand() * RAND_SCALE) - 0.5F) *
                                     paper_spin * ANGLE_PER_DEGREE;
                            paper_spd.x =
                                ((rand() * RAND_SCALE) - 0.5F) * paper_spread;
                            paper_spd.y = (((rand() * RAND_SCALE) - 0.5F) *
                                           paper_spread) +
                                          paper_rise;
                            paper_spd.z =
                                ((rand() * RAND_SCALE) - 0.5F) * paper_spread;
                            paper_pos.x += paper_spd.x * PAPER_LEAD;
                            paper_pos.z += paper_spd.z * PAPER_LEAD;
                            paper_launch(&paper_pos, &work->ang, work->pos.y,
                                         &paper_spd, &spin, &loose_paper_model,
                                         &loose_paper_texlist, PAPER_FLAGS);
                        }
                    }
                }
                work->submode |= BROKEN_DONE;
            }
            work->timer++;
        } else if (kind->wreck != NULL && kind->wreck->object != NULL) {
            if (work->collision != NULL) {
                collision_register(task);
            }
            work->submode |= BROKEN_KEPT;
        } else if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else if (work->collision != NULL) {
        if (collision_next_attack(task) != NULL || (work->flags & WORK_HURT)) {
            work->submode |= BROKEN_HIT;
        }
        if (work->collision != NULL) {
            collision_register(task);
        }
    }
}

/* a breaking kind's step: the prop breaks once its weak point is hurt */
static void prop_break_away(struct task *task, struct task_work *work,
                            struct prop_kind *kind)
{
    if (work->submode != 0) {
        if (work->timer < BREAK_STEPS && !(work->submode & BROKEN_DONE)) {
            if (work->submode & BROKEN_HIT) {
                score_add(HIT_SCORE);
                if (kind->wreck != NULL) {
                    struct prop_wreck *wreck = kind->wreck;
                    int player;
                    NJS_POINT3 pos;
                    struct angles floor_ang;
                    NJS_VECTOR spd;
                    Float floor;
                    Float scale;
                    Float *to;
                    Float *from;
                    Float *out;
                    Float *axis;
                    int i;

                    if (work->submode & BROKEN_PLAYER_1) {
                        player = 1;
                    } else {
                        player = 0;
                    }
                    pos = work->pos;
                    floor = surface_probe_floor(
                        pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &floor_ang);
                    /* @hack spelling: the difference walked by pointers, where
                     * a statement for each axis would do */
                    from = &work->pos.x;
                    to = &player_work[player]->pos.x;
                    out = &spd.x;
                    *out++ = *to++ - *from++;
                    *out++ = *to++ - *from++;
                    *out = *to - *from;
                    /* @hack spelling: the length in a statement of its own,
                     * where the division would take it in place */
                    scale = njScalor(&spd);
                    scale = PIECE_PULL / scale;
                    /* @hack spelling: the scaling walked by a pointer, where a
                     * statement for each axis would do */
                    axis = &spd.x;
                    *axis++ *= scale;
                    *axis++ *= scale;
                    /* @hack spelling: the last product assigned in full, where
                     * *= would do */
                    *axis = *axis * scale;
                    spd.x += 0.0F;
                    spd.y += PIECE_LIFT;
                    spd.z += 0.0F;
                    debris_break_built(&pos, work->ang.y, wreck->pieces,
                                       wreck->piece_count, PIECE_SPEED,
                                       PIECE_SPREAD, floor, &spd, PIECE_FLAGS,
                                       PIECE_RADIUS);
                    if (kind->flags & PROP_PAPERS) {
                        for (i = 0; i < PAPERS; i++) {
                            NJS_VECTOR paper_spd;
                            struct paper_spin spin;
                            NJS_POINT3 paper_pos;

                            paper_pos = work->pos;
                            paper_pos.y +=
                                kind->object->model->center.y * PAPER_HEIGHT;
                            spin.x = ((rand() * RAND_SCALE) - 0.5F) *
                                     paper_spin * ANGLE_PER_DEGREE;
                            spin.y = ((rand() * RAND_SCALE) - 0.5F) *
                                     paper_spin * ANGLE_PER_DEGREE;
                            spin.z = ((rand() * RAND_SCALE) - 0.5F) *
                                     paper_spin * ANGLE_PER_DEGREE;
                            paper_spd.x =
                                ((rand() * RAND_SCALE) - 0.5F) * paper_spread;
                            paper_spd.y = (((rand() * RAND_SCALE) - 0.5F) *
                                           paper_spread) +
                                          paper_rise;
                            paper_spd.z =
                                ((rand() * RAND_SCALE) - 0.5F) * paper_spread;
                            paper_pos.x += paper_spd.x * PAPER_LEAD;
                            paper_pos.z += paper_spd.z * PAPER_LEAD;
                            paper_launch(&paper_pos, &work->ang, work->pos.y,
                                         &paper_spd, &spin, &loose_paper_model,
                                         &loose_paper_texlist, PAPER_FLAGS);
                        }
                    }
                }
                work->submode |= BROKEN_DONE;
            }
            work->timer++;
        } else if (kind->wreck != NULL && kind->wreck->object != NULL) {
            if (work->collision != NULL) {
                collision_register(task);
            }
            work->submode |= BROKEN_KEPT;
        } else if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        if (task->child == NULL) {
            NJS_POINT3 pos;

            pos = work->pos;
            pos.y += kind->object->model->center.y;
            weak_point_create_paired(task, WEAK_POINT_INDEX, &pos);
        }
        if (work->collision != NULL) {
            collision_register(task);
        }
    }
}

static void prop_wobble_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *child;
    NJS_CNK_OBJECT *sibling;
    Float tilt;
    Float level;
    int alpha;
    Uint32 color;
    Uint32 fade;

    kind = KIND(task, work);
    njSetTexture(kind->texlist);
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (work->submode) {
        tilt = TILT(task) * tilt_degrees;
        njRotateX(NULL, -(tilt * ANGLE_PER_DEGREE));
        njRotateZ(NULL, -(tilt * (ANGLE_PER_DEGREE / 2)));
    }
    object = kind->object;
    if (kind->flags & PROP_BUILT) {
        njDirectDrawObject(kind->build);
    } else {
        njCnkEasyDrawModel(object->model);
    }
    child = object->child;
    if (child && !work->submode) {
        sibling = child->sibling;
        /* @hack spelling: level, a local for blink_level's result, where the
         * result scaled in place would do */
        level = blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE);
        alpha = level * FULL_ALPHA;
        if (alpha > FULL_ALPHA) {
            alpha = FULL_ALPHA;
        }
        if (alpha < 0) {
            alpha = 0;
        }
        color = (alpha << ALPHA_SHIFT) + WHITE;
        fade = ((FULL_ALPHA - alpha) << ALPHA_SHIFT) + WHITE;
        njPushMatrixEx();
        njTranslateEx((NJS_VECTOR *)child->pos);
        model_draw_tinted(child->model, color);
        njPopMatrixEx();
        if (sibling != NULL) {
            njTranslateEx((NJS_VECTOR *)sibling->pos);
            model_draw_tinted(sibling->model, fade);
        }
    }
    njPopMatrixEx();
}

static void prop_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;

    kind = KIND(task, work);
    njSetTexture(kind->texlist);
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (kind->flags & PROP_BUILT) {
        njDirectDrawObject(kind->build);
    } else {
        njCnkEasyDrawModel(kind->object->model);
    }
    njPopMatrixEx();
}

static void prop_draw_scaled(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;

    kind = KIND(task, work);
    njSetTexture(kind->texlist);
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njScale(NULL, work->scl.y, work->scl.z, 1.0F);
    }
    if (kind->flags & PROP_BUILT) {
        njDirectDrawObject(kind->build);
    } else {
        njCnkEasyDrawModel(kind->object->model);
    }
    njPopMatrixEx();
}

static void prop_draw_near(struct task *task)
{
    struct task_work *work = task->work;
    struct prop_kind *kind;
    struct prop_wreck *wreck;
    NJS_CNK_OBJECT *object;
    NJS_POINT3 view;
    Float radius;
    Float depth;
    Bool near;

    /* @hack spelling: the kind found in two statements, where one would do */
    kind = TABLE(task)->kinds;
    kind += (int)work->scl.x;
    wreck = kind->wreck;
    njCalcPoint(NULL, &work->pos, &view);
    /* @hack spelling: depth, a local for view.z, where view.z read at its
     * test would do */
    depth = view.z;
    radius = kind->object->model->r + NEAR_MARGIN;
    if (depth < -radius) {
        return;
    }
    /* @hack spelling: near set in two arms, where the comparison assigned
     * would do */
    if (njScalor2(&view) < radius * radius) {
        near = TRUE;
    } else {
        near = FALSE;
    }
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (near) {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        if (work->submode && wreck && wreck->object) {
            njSetTexture(wreck->texlist);
            model_draw_tinted(wreck->object->model, NEAR_COLOR);
        } else {
            njSetTexture(kind->texlist);
            object = kind->object;
            model_draw_tinted(object->model, NEAR_COLOR);
            object = object->child;
            if (object != NULL) {
                model_draw_tinted(object->model, NEAR_COLOR);
            }
        }
        draw_attr_restore();
        draw_control_restore();
    } else if (work->submode && wreck && wreck->object) {
        njSetTexture(wreck->texlist);
        if (wreck->build != NULL) {
            njDirectDrawObject(wreck->build);
        } else {
            njCnkEasyDrawModel(wreck->object->model);
        }
    } else {
        njSetTexture(kind->texlist);
        if (kind->flags & PROP_BUILT) {
            njDirectDrawObject(kind->build);
        } else if (kind->flags & PROP_TREE) {
            njCnkEasyDrawObject(kind->object);
        } else {
            njCnkEasyDrawModel(kind->object->model);
        }
    }
    njPopMatrixEx();
}
