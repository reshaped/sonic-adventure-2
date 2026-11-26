/* the street's props. a prop's work's scl.x picks its kind from its table.
 * ROBJ's table is street_prop_kinds, and POLEOBJ's street_pole_kinds. a
 * pole's parts are children with a table of their own,
 * street_pole_part_kinds */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/color.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/paper.h"
#include "shared/street_prop.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* what a breaking kind leaves: a model in the prop's place, and the pieces
 * the prop breaks into. texlist and object are null for a kind that leaves
 * no model */
struct street_prop_wreck {
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    struct debris_built_part *pieces;
    Sint32 piece_count;
    NJS_DIRECT_HEAD *build; /* object's build, or null */
};

/* a part a pole makes as its child. pos places the part about the pole,
 * turned by the pole's ang.y. scl becomes the part's scl, whose x picks its
 * kind. the part takes the pole's ang */
struct street_prop_part {
    NJS_POINT3 pos;
    NJS_POINT3 scl;
    void (*init)(struct task *task); /* the part's first step */
};

struct street_prop_kind {
    Uint32 flags; /* PROP_BUILD and the rest */
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    struct collision_shape *shapes; /* null for none */
    Sint32 shape_count;
    void (*draw)(struct task *task);
    /* the kind's own step, or null for a prop whose step only registers its
     * collision */
    void (*step)(struct task *task, struct task_work *work,
                 struct street_prop_kind *kind);
    /* a breaking kind's struct street_prop_wreck. a pole's is an array of the
     * parts it makes, part_count long. any other kind's is null. no step but
     * street_prop_make_parts reads part_count */
    void *more;
    Sint32 part_count;
    NJS_DIRECT_HEAD *build; /* object's build, once PROP_BUILT is set */
};

/* a kind's flags */
#define PROP_BUILD 0x1 /* street_prop_build compiles object */
/* the prop keeps its place and turn in a matrix. no kind has this flag */
#define PROP_MATRIX 0x2
/* street_prop_build and the lights and wreck draws take object's whole
 * tree, not its root's model alone */
#define PROP_TREE 0x4
#define PROP_PAPERS 0x8 /* the prop throws papers as it breaks */
/* the lights draw a glow, and street_prop_break plays GLASS_SOUND too */
#define PROP_GLOWS 0x10
#define PROP_BUILT 0x8000 /* street_prop_build has compiled object */

struct street_prop_table {
    struct street_prop_kind *kinds;
    Sint32 count;
};

/* what a prop keeps in its task's fields. motion points to its table of
 * kinds. unk_3c points to its matrix, or is null. a wobbling prop keeps its
 * tilt and the tilt's speed as Floats in force and extra. task_destroy would
 * free motion, force and extra as pointers, so the destructor nulls them */
#define TABLE(task) ((struct street_prop_table *)(task)->motion.p)
#define MATRIX(task) ((task)->unk_3c.p)
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define TILT(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define TILT_SPEED(task) (*(Float *)&(task)->extra)
/* the prop's kind, which its work's scl.x picks from its table */
#define KIND(task, work) (&TABLE(task)->kinds[(int)(work)->scl.x])
#define WRECK(kind) ((struct street_prop_wreck *)(kind)->more)

/* the placement keeps a prop's submode from one task to the next. a
 * breaking prop's submode holds its state */
#define BROKEN_HIT 0x1 /* hit: the prop breaks at its next step */
#define BROKEN_PLAYER_1 0x2 /* player 1 dealt the hit, not player 0 */
#define BROKEN_DONE 0x40 /* the break has thrown its pieces */
#define BROKEN_KEPT 0x80 /* the prop stands as its wreck */

/* a part's index among its pole's parts is its work's tick. a part that
 * breaks sets PART_BIT in its pole's submode. a part made while that bit is
 * set starts broken */
#define PART_INDEX(work) ((work)->tick)
#define PART_BIT(work) (1 << PART_INDEX(work))

#include "shared/street_prop/text.inc"

/* clang-format off */
#include "shared/road_furniture/chevron_curve_2.h"
#include "shared/road_furniture/chevron_curve_1.h"
#include "shared/road_furniture/chevron_curve_4.h"
#include "shared/road_furniture/chevron_curve_3.h"
#include "shared/street_fixtures/palm_planter.h"
#include "shared/street_fixtures/warning_sign.h"
#include "shared/road_furniture/diamond_sign.h"
#include "shared/road_furniture/horizontal_arrow_panel.h"
#include "shared/road_furniture/vertical_arrow_panel.h"
#include "shared/street_fixtures/horizontal_arrow_panel_wreck.h"
#include "shared/street_fixtures/vertical_arrow_panel_wreck.h"
#include "shared/street_fixtures/horizontal_arrow_panel_piece.h"
#include "shared/street_fixtures/vertical_arrow_panel_piece.h"
#include "shared/street_fixtures/trash_can.h"
#include "shared/street_fixtures/street_lamp.h"
#include "shared/street_fixtures/street_bench.h"
#include "shared/street_fixtures/street_bench_wreck.h"
#include "shared/street_fixtures/street_bench_piece.h"
#include "shared/street_fixtures/loose_paper.h"
#include "shared/street_fixtures/tall_newspaper_box.h"
#include "shared/street_fixtures/tall_newspaper_box_piece.h"
#include "shared/street_fixtures/tall_newspaper_box_wreck.h"
#include "shared/street_fixtures/short_newspaper_box.h"
#include "shared/street_fixtures/short_newspaper_box_piece.h"
#include "shared/street_fixtures/short_newspaper_box_wreck.h"
#include "shared/street_fixtures/road_barricade.h"
#include "shared/street_fixtures/road_barricade_piece_0.h"
#include "shared/street_fixtures/road_barricade_piece_1.h"
#include "shared/street_fixtures/road_barricade_piece_2.h"
#include "shared/road_furniture/single_sign_gantry.h"
#include "shared/road_furniture/double_sign_gantry.h"
#include "shared/street_fixtures/sign_gantry.h"
#include "shared/street_fixtures/gantry_sign.h"
#include "shared/street_fixtures/gantry_sign_piece.h"
#include "shared/street_fixtures/gantry_sign_wreck.h"
#include "shared/street_fixtures/stacked_road_signs.h"
#include "shared/street_fixtures/stop_sign.h"
#include "shared/street_fixtures/ground_panel.h"
#include "shared/street_fixtures/roadside_signs.h"
#include "shared/street_fixtures/roadside_dome.h"
#include "shared/street_fixtures/rectangular_sign.h"
#include "shared/street_fixtures/street_light_glow.h"
/* clang-format on */

static void street_pole_part_init(struct task *task);
static void street_prop_setup(struct task *task);
static void street_pole_part_setup(struct task *task);
static void street_prop_destroy(struct task *task);
static void street_pole_part_step(struct task *task);
static void street_prop_step(struct task *task);
static void street_prop_wobble(struct task *task, struct task_work *work,
                               struct street_prop_kind *kind);
static void street_prop_stand(struct task *task, struct task_work *work,
                              struct street_prop_kind *kind);
static void street_prop_break_away(struct task *task, struct task_work *work,
                                   struct street_prop_kind *kind);
static void street_prop_break(struct task *task, struct task_work *work,
                              struct street_prop_kind *kind);
static void street_pole_part_break(struct task *task, struct task_work *work,
                                   struct street_prop_kind *kind);
static void street_prop_make_parts(struct task *task, struct task_work *work,
                                   struct street_prop_kind *kind);
static void street_prop_wobble_draw(struct task *task);
static void street_prop_draw(struct task *task);
static void street_prop_draw_scaled(struct task *task);
static void street_prop_blink_draw(struct task *task);
static void street_prop_lights_draw(struct task *task);
static void street_prop_wreck_draw(struct task *task);

#define STREET_PROP_KINDS 21
#define STREET_POLE_KINDS 3
#define STREET_POLE_PART_KINDS 1
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
/* the props' sounds, by index in the stage's sound list */
#define BREAK_SOUND 8
#define WOBBLE_SOUND 9
#define GLASS_SOUND 10
#define PART_SOUND 11
/* a wobbling prop's submode counts its steps from the hit, from 1 up to
 * DUST_STEPS. each step of the count puffs dust with a chance of
 * DUST_CHANCE, DUST_HEIGHT above the prop. a puff rises at DUST_RISE. its
 * scale is DUST_SCALE plus up to DUST_SCALE_SPREAD */
#define DUST_STEPS 60
#define DUST_CHANCE 0.3F
#define DUST_HEIGHT 26.0F
#define DUST_RISE 1.0F
#define DUST_SCALE 2.0F
#define DUST_SCALE_SPREAD 6.0F
#define DUST_DEPTH_BIAS 3.0F
/* the blink of the wobbling and blinking draws, as blink_level's period,
 * rest and fade. the model's child draws with the blink's level as its
 * alpha. in the wobbling draw, the child's sibling takes 1 minus that
 * level */
#define BLINK_PERIOD 50
#define BLINK_REST 25
#define BLINK_FADE 5
/* the tint takes the alpha, 0 to FULL_ALPHA, in its top byte over WHITE */
#define FULL_ALPHA 255
#define WHITE 0xffffff

#define MATRICES_PUSHED 2
/* street_prop_lights_draw's lights are the model's child and the child's next
 * two siblings. each is on for LIGHT_STEPS steps in turn: the second sibling,
 * then the first, then the child. over the first LIGHT_FADE_STEPS steps a
 * light is on, its level rises from 0 to 1. over the same steps, the light
 * that was on falls from 1 to 0 */
#define LIGHTS 3
#define LIGHT_STEPS 30
#define LIGHT_FADE_STEPS 8
/* a light at LIGHT_MIN_LEVEL or below isn't drawn */
#define LIGHT_MIN_LEVEL 0.001F
/* a light's color blends from LIGHT_DARK toward LIGHT_LIT by its level */
#define LIGHT_DARK 0xff000000
#define LIGHT_LIT 0xffffffff
/* a PROP_GLOWS light draws a glow, and its model only above
 * GLOW_MODEL_LEVEL. the glow's color blends from LIGHT_DARK toward GLOW_LIT
 * by the light's level */
#define GLOW_MODEL_LEVEL 0.5F
#define GLOW_TEXTURE 0 /* in street_light_glow_texlist */
#define GLOW_HALF_SIZE 6.0F /* from the glow's center to each edge */
#define GLOW_DEPTH 8.0F /* how much nearer than its light the glow draws */
#define GLOW_LIT 0x90ffffff
/* every prop's and pole's shape pushes players, enemies and objects, and
 * none of them pushes it */
/* clang-format off */
static struct collision_shape street_prop_9_shapes[1] = {
#include "shared/street_prop/street_prop_9_shapes.inc"
};
static struct collision_shape street_prop_1_shapes[7] = {
#include "shared/street_prop/street_prop_1_shapes.inc"
};
static struct collision_shape street_prop_0_shapes[7] = {
#include "shared/street_prop/street_prop_0_shapes.inc"
};
static struct collision_shape street_prop_5_shapes[1] = {
#include "shared/street_prop/street_prop_5_shapes.inc"
};
static struct collision_shape street_prop_7_shapes[1] = {
#include "shared/street_prop/street_prop_7_shapes.inc"
};
static struct collision_shape street_prop_8_shapes[1] = {
#include "shared/street_prop/street_prop_8_shapes.inc"
};
static struct collision_shape street_prop_11_shapes[1] = {
#include "shared/street_prop/street_prop_11_shapes.inc"
};
static struct collision_shape street_prop_12_shapes[1] = {
#include "shared/street_prop/street_prop_12_shapes.inc"
};
static struct collision_shape street_prop_14_shapes[1] = {
#include "shared/street_prop/street_prop_14_shapes.inc"
};
static struct collision_shape street_pole_0_shapes[4] = {
#include "shared/street_prop/street_pole_0_shapes.inc"
};
static struct collision_shape street_pole_1_shapes[5] = {
#include "shared/street_prop/street_pole_1_shapes.inc"
};
static struct collision_shape street_pole_2_shapes[3] = {
#include "shared/street_prop/street_pole_2_shapes.inc"
};
static struct collision_shape street_pole_part_shapes[1] = {
#include "shared/street_prop/street_pole_part_shapes.inc"
};
static struct collision_shape street_prop_15_shapes[1] = {
#include "shared/street_prop/street_prop_15_shapes.inc"
};
static struct collision_shape street_prop_20_shapes[1] = {
#include "shared/street_prop/street_prop_20_shapes.inc"
};
static struct collision_shape street_prop_10_shapes[1] = {
#include "shared/street_prop/street_prop_10_shapes.inc"
};
/* clang-format on */
NJS_TEXLIST *street_prop_texlists[43] = {
#include "shared/street_prop/street_prop_texlists.inc"
};
/* clang-format off */
static struct debris_built_part street_prop_7_pieces[12] = {
#include "shared/street_prop/street_prop_7_pieces.inc"
};
static struct street_prop_wreck street_prop_7_wreck = {
#include "shared/street_prop/street_prop_7_wreck.inc"
};
static struct debris_built_part street_prop_8_pieces[6] = {
#include "shared/street_prop/street_prop_8_pieces.inc"
};
static struct street_prop_wreck street_prop_8_wreck = {
#include "shared/street_prop/street_prop_8_wreck.inc"
};
static struct debris_built_part street_prop_9_pieces[1] = {
#include "shared/street_prop/street_prop_9_pieces.inc"
};
static struct street_prop_wreck street_prop_9_wreck = {
#include "shared/street_prop/street_prop_9_wreck.inc"
};
static struct debris_built_part street_prop_11_pieces[5] = {
#include "shared/street_prop/street_prop_11_pieces.inc"
};
static struct street_prop_wreck street_prop_11_wreck = {
#include "shared/street_prop/street_prop_11_wreck.inc"
};
static struct debris_built_part street_prop_12_pieces[4] = {
#include "shared/street_prop/street_prop_12_pieces.inc"
};
static struct street_prop_wreck street_prop_12_wreck = {
#include "shared/street_prop/street_prop_12_wreck.inc"
};
static struct debris_built_part street_prop_13_pieces[4] = {
#include "shared/street_prop/street_prop_13_pieces.inc"
};
static struct street_prop_wreck street_prop_13_wreck = {
#include "shared/street_prop/street_prop_13_wreck.inc"
};
static struct debris_built_part street_prop_14_pieces[12] = {
#include "shared/street_prop/street_prop_14_pieces.inc"
};
static struct street_prop_wreck street_prop_14_wreck = {
#include "shared/street_prop/street_prop_14_wreck.inc"
};
static struct debris_built_part street_pole_part_pieces[8] = {
#include "shared/street_prop/street_pole_part_pieces.inc"
};
static struct street_prop_wreck street_pole_part_wreck = {
#include "shared/street_prop/street_pole_part_wreck.inc"
};
static struct street_prop_part street_pole_parts[1] = {
#include "shared/street_prop/street_pole_parts.inc"
};
static struct street_prop_kind street_prop_kinds[STREET_PROP_KINDS] = {
#include "shared/street_prop/street_prop_kinds.inc"
};
static struct street_prop_kind street_pole_kinds[STREET_POLE_KINDS] = {
#include "shared/street_prop/street_pole_kinds.inc"
};
static struct street_prop_kind street_pole_part_kinds[STREET_POLE_PART_KINDS] = {
#include "shared/street_prop/street_pole_part_kinds.inc"
};
/* clang-format on */

static struct street_prop_table street_prop_tables[3] = {
#include "shared/street_prop/street_prop_tables.inc"
};
#define TABLES ARRAY_COUNT(street_prop_tables)
/* the wrecks whose models and pieces street_prop_build compiles */
static struct street_prop_wreck *street_prop_wrecks[8] = {
#include "shared/street_prop/street_prop_wrecks.inc"
};
#define WRECKS ARRAY_COUNT(street_prop_wrecks)
/* a wobbling prop's tilt runs from 0 to 1. each step, the tilt's speed keeps
 * TILT_KEEP of itself and gains tilt_pull. past 0 or 1, the tilt stops there
 * and turns back at TILT_BOUNCE of its speed. a tilt of 1 leans the prop
 * tilt_degrees about x, and half that about z */
#define TILT_KEEP 0.99F
#define TILT_BOUNCE 0.7F
static Float tilt_pull = 0.008F;
static Float tilt_degrees = 25.0F;
/* the wobbling and standing steps count the work's mode from 0 to
 * wobble_count. each time the mode wraps, they count its timer up, modulo
 * 256. nothing reads either count */
static Sint32 wobble_count = 5;
/* a paper's speed is paper_rise up, plus up to paper_spread / 2 either way
 * along each axis. it spins about each axis at up to paper_spin / 2 degrees
 * a step either way */
static Float paper_rise = 1.2F;
static Float paper_spread = 0.38F;
static Float paper_spin = 0.17F;
/* the glow's center, about its light */
static NJS_POINT3 glow_pos = {0.0F, 0.0F, 0.0F};

void street_prop_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &street_prop_tables[0];
        street_prop_setup(task);
    }
}

void street_pole_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.p = &street_prop_tables[1];
        street_prop_setup(task);
    }
}

static void street_pole_part_init(struct task *task)
{
    task->motion.p = &street_prop_tables[2];
    street_pole_part_setup(task);
}

Uint8 *street_prop_build(Uint8 *buf)
{
    int i;
    int j;
    struct street_prop_table *table;
    struct street_prop_kind *kind;
    Uint8 *end;
    NJS_CNK_OBJECT object;
    struct debris_built_part *pieces;
    struct street_prop_wreck **wreck;

    for (i = 0; (unsigned int)i < TABLES; i++) {
        table = &street_prop_tables[i];
        for (j = 0; j < table->count; j++) {
            kind = &table->kinds[j];
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
        }
    }
    /* @hack spelling: each wreck read through its entry's address at each
     * use, where a copy of the entry would do */
    wreck = street_prop_wrecks;
    for (i = 0; (unsigned int)i < WRECKS; i++) {
        pieces = (*wreck)->pieces;
        if ((*wreck)->texlist != NULL) {
            njSetTexture((*wreck)->texlist);
            (*wreck)->build = object_build(&buf, (*wreck)->object);
        } else {
            (*wreck)->build = NULL;
        }
        debris_build(&buf, pieces, (*wreck++)->piece_count);
    }
    return buf;
}

static void street_prop_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;

    task->step = street_prop_step;
    task->destructor = street_prop_destroy;
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

/* street_prop_setup for a pole's part, whose step never tests its range */
static void street_pole_part_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;

    task->step = street_pole_part_step;
    task->destructor = street_prop_destroy;
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

static void street_prop_destroy(struct task *task)
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

static void street_pole_part_step(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;

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

static void street_prop_step(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;

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

static void street_prop_wobble(struct task *task, struct task_work *work,
                               struct street_prop_kind *kind)
{
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
                NJS_VECTOR spd;
                NJS_POINT3 pos;

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
    } else if (work->collision != NULL && collision_next_attack(task) != NULL) {
        work->submode = 1;
        score_add(HIT_SCORE);
        sound_play(WOBBLE_SOUND, NULL, 0, 0);
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
}

static void street_prop_stand(struct task *task, struct task_work *work,
                              struct street_prop_kind *kind)
{
    work->mode++;
    if (work->mode > wobble_count) {
        work->mode = 0;
        work->timer = (Uint8)(work->timer + 1);
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
}

/* a breaking kind's step: the prop breaks once its weak point is hurt */
static void street_prop_break_away(struct task *task, struct task_work *work,
                                   struct street_prop_kind *kind)
{
    if (work->submode != 0) {
        if (work->timer < BREAK_STEPS && !(work->submode & BROKEN_DONE)) {
            if (work->submode & BROKEN_HIT) {
                score_add(HIT_SCORE);
                sound_play(BREAK_SOUND, NULL, 0, 0);
                if (WRECK(kind) != NULL) {
                    struct street_prop_wreck *wreck = WRECK(kind);
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
        } else if (WRECK(kind) != NULL && WRECK(kind)->object != NULL) {
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

/* a breaking kind's step: the prop breaks once an attack touches it */
static void street_prop_break(struct task *task, struct task_work *work,
                              struct street_prop_kind *kind)
{
    if (work->submode != 0) {
        if (work->timer < BREAK_STEPS && !(work->submode & BROKEN_DONE)) {
            if (work->submode & BROKEN_HIT) {
                score_add(HIT_SCORE);
                sound_play(BREAK_SOUND, NULL, 0, 0);
                if (kind->flags & PROP_GLOWS) {
                    sound_play(GLASS_SOUND, NULL, 0, 0);
                }
                if (WRECK(kind) != NULL) {
                    struct street_prop_wreck *wreck = WRECK(kind);
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
        } else if (WRECK(kind) != NULL && WRECK(kind)->object != NULL) {
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
        if (collision_next_attack(task) != NULL) {
            work->submode |= BROKEN_HIT;
        }
        if (work->collision != NULL) {
            collision_register(task);
        }
    }
}

/* street_prop_break_away for a pole's part, which also marks its break in
 * its pole's submode */
static void street_pole_part_break(struct task *task, struct task_work *work,
                                   struct street_prop_kind *kind)
{
    if (work->submode != 0) {
        if (work->timer < BREAK_STEPS && !(work->submode & BROKEN_DONE)) {
            if (work->submode & BROKEN_HIT) {
                score_add(HIT_SCORE);
                sound_play(BREAK_SOUND, NULL, 0, 0);
                if (kind->object == &gantry_sign_object) {
                    sound_play(PART_SOUND, NULL, 0, 0);
                }
                if (WRECK(kind) != NULL) {
                    struct street_prop_wreck *wreck = WRECK(kind);
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
                    *axis *= scale;
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
                task->parent->work->submode |= PART_BIT(work);
            }
            work->timer++;
        } else if (WRECK(kind) != NULL && WRECK(kind)->object != NULL) {
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
        if (task->parent->work->submode & PART_BIT(work)) {
            work->submode |= BROKEN_HIT;
            work->submode |= BROKEN_DONE;
        } else if (task->child == NULL) {
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

static void street_prop_make_parts(struct task *task, struct task_work *work,
                                   struct street_prop_kind *kind)
{
    if (task->child == NULL) {
        struct street_prop_part *part = kind->more;
        struct task *child;
        int i;

        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        for (i = 0; i < kind->part_count; i++, part++) {
            child = task_create_child(TASK_WORK, part->init, task);
            if (child != NULL) {
                njCalcPoint(NULL, &part->pos, &child->work->pos);
                child->work->ang = work->ang;
                child->work->scl = part->scl;
                PART_INDEX(child->work) = i;
            }
        }
        njPopMatrixEx();
    }
    if (work->collision != NULL) {
        collision_register(task);
    }
}

static void street_prop_wobble_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;
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

static void street_prop_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;

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

static void street_prop_draw_scaled(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind;

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

static void street_prop_blink_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind = KIND(task, work);
    struct street_prop_wreck *wreck = WRECK(kind);
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *child;
    Float level;
    int alpha;

    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (work->submode && wreck && wreck->object) {
        njSetTexture(wreck->texlist);
        if (wreck->build != NULL) {
            njDirectDrawObject(wreck->build);
        } else {
            njCnkEasyDrawModel(wreck->object->model);
        }
    } else {
        njSetTexture(kind->texlist);
        object = kind->object;
        if (kind->flags & PROP_BUILT) {
            njDirectDrawObject(kind->build);
        } else {
            njCnkEasyDrawModel(object->model);
        }
        /* @hack spelling: the assignment inside the test, where a
         * statement of its own would do */
        if ((child = object->child) != NULL) {
            /* @hack spelling: level, a local for blink_level's result,
             * where the result scaled in place would do */
            level = blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE);
            alpha = level * FULL_ALPHA;
            if (alpha > FULL_ALPHA) {
                alpha = FULL_ALPHA;
            }
            if (alpha < 0) {
                alpha = 0;
            }
            /* @hack spelling: alpha turned into the color in place, where a
             * local of its own would do */
            alpha = (alpha << ALPHA_SHIFT) + WHITE;
            model_draw_tinted(child->model, alpha);
        }
    }
    njPopMatrixEx();
}

static void street_prop_lights_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind = KIND(task, work);
    struct street_prop_wreck *wreck = WRECK(kind);
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *child;
    int lit;
    int step;
    int i;
    Float level;

    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (work->submode && wreck && wreck->object) {
        njSetTexture(wreck->texlist);
        njPushMatrixEx();
        if (wreck->build != NULL) {
            njDirectDrawObject(wreck->build);
        } else {
            njCnkEasyDrawModel(wreck->object->model);
        }
    } else {
        njSetTexture(kind->texlist);
        object = kind->object;
        /* @hack spelling: the model read through kind, where object would
         * do */
        if (kind->flags & PROP_BUILT) {
            njDirectDrawObject(kind->build);
        } else if (kind->flags & PROP_TREE) {
            njCnkEasyDrawObject(kind->object);
        } else {
            njCnkEasyDrawModel(kind->object->model);
        }
        njPushMatrixEx();
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = object->child) != NULL) {
            step = game_state.steps % (LIGHTS * LIGHT_STEPS);
            lit = LIGHTS - 1 - (step / LIGHT_STEPS);
            /* @hack spelling: the remainder assigned in full, where %= would
             * do */
            step = step % LIGHT_STEPS;
            for (i = 0; i < LIGHTS; i++, child = child->sibling) {
                if (i == lit) {
                    if (step < LIGHT_FADE_STEPS) {
                        level = step * (1.0F / LIGHT_FADE_STEPS);
                    } else {
                        level = 1.0F;
                    }
                } else if (i == (lit + 1) % LIGHTS) {
                    if (step < LIGHT_FADE_STEPS) {
                        level = (LIGHT_FADE_STEPS - step) *
                                (1.0F / LIGHT_FADE_STEPS);
                    } else {
                        level = 0.0F;
                    }
                } else {
                    level = 0.0F;
                }
                if (level > LIGHT_MIN_LEVEL) {
                    njTranslateEx((NJS_VECTOR *)child->pos);
                    if (kind->flags & PROP_GLOWS) {
                        if (level > GLOW_MODEL_LEVEL) {
                            njSetTexture(kind->texlist);
                            njCnkEasyDrawModel(child->model);
                        }
                        njFogDisable();
                        njColorBlendingMode(NJD_SOURCE_COLOR,
                                            NJD_COLOR_BLENDING_SRCALPHA);
                        njColorBlendingMode(NJD_DESTINATION_COLOR,
                                            NJD_COLOR_BLENDING_ONE);
                        njSetTexture(&street_light_glow_texlist);
                        particle_draw_quad(
                            &glow_pos, GLOW_TEXTURE, QUARTER_TURN, 0,
                            GLOW_HALF_SIZE, GLOW_HALF_SIZE,
                            color_blend(LIGHT_DARK, GLOW_LIT, level),
                            PARTICLE_BLEND_ADD, GLOW_DEPTH, 0.0F);
                        njColorBlendingMode(NJD_SOURCE_COLOR,
                                            NJD_COLOR_BLENDING_SRCALPHA);
                        njColorBlendingMode(NJD_DESTINATION_COLOR,
                                            NJD_COLOR_BLENDING_INVSRCALPHA);
                        njFogEnable();
                    } else {
                        model_draw_tinted(
                            child->model,
                            color_blend(LIGHT_DARK, LIGHT_LIT, level));
                    }
                    matrix_load_parent();
                }
            }
        }
    }
    njPopMatrix(MATRICES_PUSHED);
}

static void street_prop_wreck_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct street_prop_kind *kind = KIND(task, work);
    struct street_prop_wreck *wreck = WRECK(kind);

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODEL(0-  )";
        (void)"<- ANGLE Y";
    }
    njPushMatrixEx();
    if (MATRIX(task) != NULL) {
        njMultiMatrix(NULL, MATRIX(task));
    } else {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
    }
    if (work->submode && wreck && wreck->object) {
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
