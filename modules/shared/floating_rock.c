/* the floating rock, ROCK in a stage's object list: a rock that turns and
 * bobs in the air. a hunter's strike breaks it into eight pieces */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "object/mover.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/floating_rock.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "shared/floating_rock/text.inc"

static const char floating_rock_edit_rotation[16] = "<- ROTATE SPEED";
static const char floating_rock_edit_emerald_disable[20] = "<- NO EMELALD(   )";
static const char floating_rock_edit_emerald_id[20] = "<- EMELALD ID(   )";
static const char floating_rock_edit_scale[16] = "<- ROCK_SCALE";
static const char floating_rock_edit_bob_height[16] = "<- UPDOWN_SCALE";
static const char floating_rock_edit_bob_speed[16] = "<- UPDOWN_SPEED";

static NJS_TEXNAME wind_stone_texlist_0_names[1] = {
#include "shared/floating_rock/wind_stone_texlist_0_names.inc"
};

static NJS_TEXLIST wind_stone_texlist_0 = {
#include "shared/floating_rock/wind_stone_texlist_0.inc"
};

static Sint16 wind_stone_0_polygons[125] = {
#include "shared/floating_rock/wind_stone_0_polygons.inc"
};

static Sint32 wind_stone_0_vertices[39] = {
#include "shared/floating_rock/wind_stone_0_vertices.inc"
};

static NJS_CNK_MODEL wind_stone_0_model = {
#include "shared/floating_rock/wind_stone_0_model.inc"
};

static NJS_CNK_OBJECT wind_stone_0_object = {
#include "shared/floating_rock/wind_stone_0_object.inc"
};

static NJS_TEXNAME wind_stone_texlist_1_names[1] = {
#include "shared/floating_rock/wind_stone_texlist_1_names.inc"
};

static NJS_TEXLIST wind_stone_texlist_1 = {
#include "shared/floating_rock/wind_stone_texlist_1.inc"
};

static Sint16 wind_stone_1_polygons[71] = {
#include "shared/floating_rock/wind_stone_1_polygons.inc"
};

static Sint32 wind_stone_1_vertices[31] = {
#include "shared/floating_rock/wind_stone_1_vertices.inc"
};

static NJS_CNK_MODEL wind_stone_1_model = {
#include "shared/floating_rock/wind_stone_1_model.inc"
};

static NJS_CNK_OBJECT wind_stone_1_object = {
#include "shared/floating_rock/wind_stone_1_object.inc"
};

static NJS_TEXNAME wind_stone_texlist_2_names[1] = {
#include "shared/floating_rock/wind_stone_texlist_2_names.inc"
};

static NJS_TEXLIST wind_stone_texlist_2 = {
#include "shared/floating_rock/wind_stone_texlist_2.inc"
};

static Sint16 wind_stone_2_polygons[59] = {
#include "shared/floating_rock/wind_stone_2_polygons.inc"
};

static Sint32 wind_stone_2_vertices[23] = {
#include "shared/floating_rock/wind_stone_2_vertices.inc"
};

static NJS_CNK_MODEL wind_stone_2_model = {
#include "shared/floating_rock/wind_stone_2_model.inc"
};

static NJS_CNK_OBJECT wind_stone_2_object = {
#include "shared/floating_rock/wind_stone_2_object.inc"
};

static NJS_TEXNAME wind_stone_texlist_3_names[1] = {
#include "shared/floating_rock/wind_stone_texlist_3_names.inc"
};

static NJS_TEXLIST wind_stone_texlist_3 = {
#include "shared/floating_rock/wind_stone_texlist_3.inc"
};

static Sint16 wind_stone_3_polygons[51] = {
#include "shared/floating_rock/wind_stone_3_polygons.inc"
};

static Sint32 wind_stone_3_vertices[23] = {
#include "shared/floating_rock/wind_stone_3_vertices.inc"
};

static NJS_CNK_MODEL wind_stone_3_model = {
#include "shared/floating_rock/wind_stone_3_model.inc"
};

static NJS_CNK_OBJECT wind_stone_3_object = {
#include "shared/floating_rock/wind_stone_3_object.inc"
};

static NJS_TEXNAME wind_stone_texlist_4_names[1] = {
#include "shared/floating_rock/wind_stone_texlist_4_names.inc"
};

static NJS_TEXLIST wind_stone_texlist_4 = {
#include "shared/floating_rock/wind_stone_texlist_4.inc"
};

static Sint16 wind_stone_4_polygons[45] = {
#include "shared/floating_rock/wind_stone_4_polygons.inc"
};

static Sint32 wind_stone_4_vertices[27] = {
#include "shared/floating_rock/wind_stone_4_vertices.inc"
};

static NJS_CNK_MODEL wind_stone_4_model = {
#include "shared/floating_rock/wind_stone_4_model.inc"
};

static NJS_CNK_OBJECT wind_stone_4_object = {
#include "shared/floating_rock/wind_stone_4_object.inc"
};

static NJS_TEXNAME wind_stone_texlist_5_names[1] = {
#include "shared/floating_rock/wind_stone_texlist_5_names.inc"
};

static NJS_TEXLIST wind_stone_texlist_5 = {
#include "shared/floating_rock/wind_stone_texlist_5.inc"
};

static Sint16 wind_stone_5_polygons[53] = {
#include "shared/floating_rock/wind_stone_5_polygons.inc"
};

static Sint32 wind_stone_5_vertices[31] = {
#include "shared/floating_rock/wind_stone_5_vertices.inc"
};

static NJS_CNK_MODEL wind_stone_5_model = {
#include "shared/floating_rock/wind_stone_5_model.inc"
};

static NJS_CNK_OBJECT wind_stone_5_object = {
#include "shared/floating_rock/wind_stone_5_object.inc"
};

NJS_CNK_MODEL *wind_stone_models[6] = {
#include "shared/floating_rock/wind_stone_models.inc"
};

NJS_TEXLIST *wind_stone_texlists[6] = {
#include "shared/floating_rock/wind_stone_texlists.inc"
};

/* a stone's or a piece's mass goes as the cube of its size, and its area as
 * the square */
#define MASS(size) ((size) * (size) * (size) * 1.5F)
#define AREA(size) ((size) * (size))

/* no code reads kind 0's mass or area: no stone is of kind 0 */
Float wind_stone_masses[6] = {0.3F,       MASS(0.4F),  MASS(0.3F),
                              MASS(0.2F), MASS(0.15F), MASS(0.12F)};
Float wind_stone_areas[6] = {0.4F,       AREA(0.4F),  AREA(0.3F),
                             AREA(0.2F), AREA(0.15F), AREA(0.12F)};

static NJS_TEXNAME floating_rock_texlist_names[2] = {
#include "shared/floating_rock/floating_rock_texlist_names.inc"
};

static NJS_TEXLIST floating_rock_texlist = {
#include "shared/floating_rock/floating_rock_texlist.inc"
};

static Sint16 floating_rock_polygons[163] = {
#include "shared/floating_rock/floating_rock_polygons.inc"
};

static Sint32 floating_rock_vertices[75] = {
#include "shared/floating_rock/floating_rock_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_model = {
#include "shared/floating_rock/floating_rock_model.inc"
};

static NJS_CNK_OBJECT floating_rock_object = {
#include "shared/floating_rock/floating_rock_object.inc"
};

static NJS_TEXNAME floating_rock_piece_texlist_names[2] = {
#include "shared/floating_rock/floating_rock_piece_texlist_names.inc"
};

static NJS_TEXLIST floating_rock_piece_texlist = {
#include "shared/floating_rock/floating_rock_piece_texlist.inc"
};

static Sint16 floating_rock_piece_8_polygons[63] = {
#include "shared/floating_rock/floating_rock_piece_8_polygons.inc"
};

static Sint32 floating_rock_piece_8_vertices[51] = {
#include "shared/floating_rock/floating_rock_piece_8_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_8_model = {
#include "shared/floating_rock/floating_rock_piece_8_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_8_object = {
#include "shared/floating_rock/floating_rock_piece_8_object.inc"
};

static Sint16 floating_rock_piece_7_polygons[69] = {
#include "shared/floating_rock/floating_rock_piece_7_polygons.inc"
};

static Sint32 floating_rock_piece_7_vertices[51] = {
#include "shared/floating_rock/floating_rock_piece_7_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_7_model = {
#include "shared/floating_rock/floating_rock_piece_7_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_7_object = {
#include "shared/floating_rock/floating_rock_piece_7_object.inc"
};

static Sint16 floating_rock_piece_6_polygons[63] = {
#include "shared/floating_rock/floating_rock_piece_6_polygons.inc"
};

static Sint32 floating_rock_piece_6_vertices[51] = {
#include "shared/floating_rock/floating_rock_piece_6_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_6_model = {
#include "shared/floating_rock/floating_rock_piece_6_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_6_object = {
#include "shared/floating_rock/floating_rock_piece_6_object.inc"
};

static Sint16 floating_rock_piece_5_polygons[63] = {
#include "shared/floating_rock/floating_rock_piece_5_polygons.inc"
};

static Sint32 floating_rock_piece_5_vertices[51] = {
#include "shared/floating_rock/floating_rock_piece_5_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_5_model = {
#include "shared/floating_rock/floating_rock_piece_5_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_5_object = {
#include "shared/floating_rock/floating_rock_piece_5_object.inc"
};

static Sint16 floating_rock_piece_4_polygons[117] = {
#include "shared/floating_rock/floating_rock_piece_4_polygons.inc"
};

static Sint32 floating_rock_piece_4_vertices[57] = {
#include "shared/floating_rock/floating_rock_piece_4_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_4_model = {
#include "shared/floating_rock/floating_rock_piece_4_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_4_object = {
#include "shared/floating_rock/floating_rock_piece_4_object.inc"
};

static Sint16 floating_rock_piece_3_polygons[109] = {
#include "shared/floating_rock/floating_rock_piece_3_polygons.inc"
};

static Sint32 floating_rock_piece_3_vertices[51] = {
#include "shared/floating_rock/floating_rock_piece_3_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_3_model = {
#include "shared/floating_rock/floating_rock_piece_3_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_3_object = {
#include "shared/floating_rock/floating_rock_piece_3_object.inc"
};

static Sint16 floating_rock_piece_2_polygons[129] = {
#include "shared/floating_rock/floating_rock_piece_2_polygons.inc"
};

static Sint32 floating_rock_piece_2_vertices[63] = {
#include "shared/floating_rock/floating_rock_piece_2_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_2_model = {
#include "shared/floating_rock/floating_rock_piece_2_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_2_object = {
#include "shared/floating_rock/floating_rock_piece_2_object.inc"
};

static Sint16 floating_rock_piece_1_polygons[117] = {
#include "shared/floating_rock/floating_rock_piece_1_polygons.inc"
};

static Sint32 floating_rock_piece_1_vertices[57] = {
#include "shared/floating_rock/floating_rock_piece_1_vertices.inc"
};

static NJS_CNK_MODEL floating_rock_piece_1_model = {
#include "shared/floating_rock/floating_rock_piece_1_model.inc"
};

static NJS_CNK_OBJECT floating_rock_piece_1_object = {
#include "shared/floating_rock/floating_rock_piece_1_object.inc"
};

static NJS_CNK_OBJECT floating_rock_shards = {
#include "shared/floating_rock/floating_rock_shards.inc"
};

/* nothing calls the first three of these four, and no header declares them.
 * they are external only because SHC leaves out a static function that
 * nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void floating_rock_set_base(struct task *task);
/* save the work's pos, ang and scl in the record, and restore them. each
 * does nothing without a record */
void floating_rock_save(struct task *task);
void floating_rock_restore(struct task *task);
/* make the rock take mode at its next step */
void floating_rock_set_mode(struct task *task, int mode);
/* NOLINTEND(misc-use-internal-linkage) */

enum floating_rock_mode { MODE_WHOLE, MODE_BROKEN };
/* the mode the rock's next step takes, or NO_NEXT_MODE, in the work's
 * submode */
#define NEXT_MODE(work) ((work)->submode)
#define NO_NEXT_MODE (-1)

#define PIECES 8
/* the low byte of ang.y for a rock in no enemy group. such a rock takes no
 * enemy_work */
#define NO_GROUP 0xff

/* each step a piece's upward speed gains LIFT times its area over its mass,
 * less GRAVITY. DRAG then scales its speed and its spin. the break sets each
 * piece's acc.y to PIECE_LIFT, which the piece's first step replaces
 * unread */
#define LIFT 0.06F
#define GRAVITY 0.08F
#define DRAG 0.995F
#define PIECE_LIFT 0.1F
/* the work's tick counts the broken steps from 0. the step that finds it at
 * BROKEN_STEPS stops the placement. so the pieces fly for BROKEN_STEPS + 1
 * steps. they fade from BROKEN_STEPS - FADE_STEPS on, and are gone at
 * BROKEN_STEPS */
#define BROKEN_STEPS 90
#define FADE_STEPS 45
/* a fading piece draws in white, its alpha ALPHA_MAX at full */
#define ALPHA_FULL 255.0F
#define ALPHA_MAX 0xff
#define WHITE_RGB 0xffffff

/* the spin's speed swings by SPIN_SWING about SPIN_BASE. a speed of 1 turns
 * the rock by SPIN_X, SPIN_Y and SPIN_Z a step */
#define SPIN_BASE 0.06F
#define SPIN_SWING 0.03F
#define SPIN_X 6400.0005F
#define SPIN_Y 533.3334F
#define SPIN_Z 1066.6667F
/* the swing's angle gains SWING_TURN * swing_rate * SWING_SCALE a step.
 * swing_rate is scl.z + 1, times a random SWING_LEAST up to SWING_LEAST +
 * SWING_SPREAD. phase is rand() times PHASE_SCALE */
#define SWING_TURN 16
#define SWING_SCALE 0.1F
#define SWING_LEAST 0.9F
#define SWING_SPREAD 0.2F
#define PHASE_SCALE 2.0F

/* the radius of the rock's shape, before its size */
#define RADIUS 30.0F

/* a bob's quarter lasts scl.z + 1 steps. the bob's height is (scl.y + 1) *
 * CLIMB_STEP for each of those steps. each of its first two quarters climbs
 * QUARTER_RISE of that height. a quarter of 0 steps takes ZERO_QUARTER in its
 * place, so that no division is by 0 */
#define CLIMB_STEP 0.1F
#define QUARTER_RISE 0.5F
#define ZERO_QUARTER 0.1F

/* the stage's sound the break plays, the shake it gives the hunter's pad,
 * and its puffs of dust */
#define BREAK_SOUND 7
#define BREAK_POWER 6
#define BREAK_FREQ 15
#define BREAK_DUSTS 6
/* a puff of dust starts DUST_LEAST to DUST_LEAST + DUST_SPREAD times the
 * rock's radius from its pos. its size is dust_size, less up to DUST_SHRINK
 * of it */
#define DUST_LEAST 1.25F
#define DUST_SPREAD 0.5F
#define DUST_SHRINK 0.2F
/* each axis of a piece's speed takes a random factor of
 * PIECE_SPEED_MOST - PIECE_SPEED_RANGE up to PIECE_SPEED_MOST. each axis of
 * its spin takes one of PIECE_SPIN_MOST - PIECE_SPIN_RANGE up to
 * PIECE_SPIN_MOST */
#define PIECE_SPEED_MOST 1.1F
#define PIECE_SPEED_RANGE 0.2F
#define PIECE_SPIN_MOST 1.2F
#define PIECE_SPIN_RANGE 0.4F

#define FADE_ATTR_AND CHUNK_TYPE_BITS
#define FADE_ATTR_OR (NJD_FST_UA | NJD_FST_IS)

/* the rock's size, as a scale of its model: 1, and SIZE_STEP more for each
 * unit of scl.x */
#define SIZE_STEP 0.1F
#define SCALE(task) (1.0F + ((task)->work->scl.x * SIZE_STEP))
/* @hack type: extra read as a void * through its address, where
 * (task)->extra.p would do */
#define ROCK(task) ((struct floating_rock *)*(void **)&(task)->extra)

/* the rock's record, in task->extra */
struct floating_rock {
    NJS_POINT3 saved_pos;
    struct angles saved_ang;
    NJS_POINT3 saved_scl;
    NJS_VECTOR unk_24;
    NJS_VECTOR unk_30;
    /* what the work's ang gains a step, which floating_rock_whole_step sets
     * anew */
    struct angles spin;
    NJS_POINT3 base; /* the place the bob rises from */
    Float swing_rate; /* how fast the spin's speed swings */
    /* a random count of steps that the bob and the swing add to
     * game_state.steps */
    Uint32 phase;
    NJS_POINT3 piece_pos[PIECES];
    struct angles piece_ang[PIECES];
    NJS_VECTOR piece_spd[PIECES];
    NJS_VECTOR piece_acc[PIECES];
    struct angles piece_spin[PIECES];
    Bool has_last; /* TRUE once last_matrix holds a matrix */
    NJS_MATRIX *matrix; /* the matrix of the work's pos and ang */
    NJS_MATRIX *last_matrix; /* matrix as the step before left it */
};

/* a piece of the rock, and its index in models and texlists. its place is
 * in the rock's own axes, before the rock's size */
struct floating_rock_piece {
    NJS_POINT3 pos;
    Sint32 id;
};

static void floating_rock_whole_step(struct task *task);
static void floating_rock_broken_step(struct task *task);

/* by id: the whole rock, then its pieces */
KEPT static NJS_CNK_MODEL *models[PIECES + 1] = {
#include "shared/floating_rock/models.inc"
};
KEPT static NJS_TEXLIST *texlists[PIECES + 1] = {
#include "shared/floating_rock/texlists.inc"
};
/* each piece's mass and area, by id, as models: the whole rock, then its
 * pieces */
KEPT static Float masses[PIECES + 1] = {
    0.3F,       MASS(0.37F), MASS(0.37F), MASS(0.37F), MASS(0.37F),
    MASS(0.3F), MASS(0.3F),  MASS(0.2F),  MASS(0.2F),
};
KEPT static Float areas[PIECES + 1] = {
    0.4F,       AREA(0.37F), AREA(0.37F), AREA(0.37F), AREA(0.37F),
    AREA(0.3F), AREA(0.3F),  AREA(0.2F),  AREA(0.2F),
};
static struct floating_rock_piece pieces[PIECES] = {
#include "shared/floating_rock/pieces.inc"
};
/* clang-format off */
NJS_TEXLIST *floating_rock_stone_texlists[7] = {
#include "shared/floating_rock/floating_rock_stone_texlists.inc"
};
NJS_TEXLIST *floating_rock_texlists[3] = {
#include "shared/floating_rock/floating_rock_texlists.inc"
};
/* clang-format on */
/* a sphere about the rock's pos, which floating_rock_init sizes with the
 * rock */
/* clang-format off */
static struct collision_shape floating_rock_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), 0, {0.0F, 0.0F, 0.0F}, RADIUS, 0.0F, 0.0F, 0,
    {0, 0, 0},
};
/* clang-format on */
static Sint32 break_volume = SOUND_LOUDEST;
typedef void (*floating_rock_step_fn)(struct task *task);

KEPT static floating_rock_step_fn mode_steps[2] = {
#include "shared/floating_rock/mode_steps.inc"
};
/* the size of the break's dust, which the rock's size scales */
static Float dust_size = 4.0F;
/* each piece flies out at spread times its offset from the rock's pos. it
 * also takes push_back along the heading from the rock to the hunter. push_back
 * is below 0, so that push is away from the hunter */
static Float spread = 0.04F;
static Float push_back = -0.25F;
/* each piece keeps carry times the rock's motion at its place, and
 * player_carry times the hunter's speed */
static Float carry = 0.9F;
static Float player_carry = 0.95F;
/* a piece spins piece_spin_scale times as fast as the rock, before its
 * random factor */
static Float piece_spin_scale = 1.4F;

static void floating_rock_step(struct task *task);
static void floating_rock_destroy(struct task *task);
static void floating_rock_draw(struct task *task);
static struct floating_rock *floating_rock_alloc(struct task *task);
static void floating_rock_free(struct task *task);
static void floating_rock_take_mode(struct task *task);
static void floating_rock_break(struct task *task, struct task *hunter);

void floating_rock_init(struct task *task)
{
    /* @hack spelling: task->work read again for timer, ang.y, mode and
     * submode, where work would do */
    struct task_work *work = task->work;
    struct collision_shape shape;
    /* @hack spelling: at, a pointer to shape, where shape written in place
     * would do */
    struct collision_shape *at;

    if (task->extra.p != NULL && placement_try_leave(task)) {
        return;
    }
    if (floating_rock_alloc(task) == NULL) {
        task_destroy(task);
        return;
    }
    task->draw = floating_rock_draw;
    task->step = floating_rock_step;
    task->destructor = floating_rock_destroy;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateXYZ(NULL, work->ang.x, work->ang.y, work->ang.z);
    matrix_get(ROCK(task)->matrix);
    njPopMatrixEx();
    if (ROCK(task)->has_last == FALSE) {
        njSetMatrix(ROCK(task)->last_matrix, ROCK(task)->matrix);
        ROCK(task)->has_last = TRUE;
    }
    at = &shape;
    *at = floating_rock_shape;
    at->radius *= SCALE(task);
    collision_init(task, &shape, 1, COLLISION_OBJECT);
    task->work->timer = (Uint8)task->work->ang.y;
    if ((Uint8)work->ang.y == NO_GROUP) {
        task->motion.p = NULL;
    } else {
        task->motion.p = enemy_work_alloc_grouped(task);
    }
    task->work->mode = MODE_WHOLE;
    NEXT_MODE(task->work) = NO_NEXT_MODE;
}

static void floating_rock_destroy(struct task *task)
{
    floating_rock_free(task);
    if (task->motion.p != NULL) {
        enemy_destroy(task);
    }
}

static void floating_rock_step(struct task *task)
{
    /* @hack spelling: task->work read again for mode, where work would do */
    struct task_work *work = task->work;

    /* @hack redundant: the else, where the code after the test would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        floating_rock_take_mode(task);
        mode_steps[task->work->mode](task);
        if (ROCK(task)->has_last) {
            njSetMatrix(ROCK(task)->last_matrix, ROCK(task)->matrix);
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateXYZ(NULL, work->ang.x, work->ang.y, work->ang.z);
        matrix_get(ROCK(task)->matrix);
        njPopMatrixEx();
        if (ROCK(task)->has_last == FALSE) {
            njSetMatrix(ROCK(task)->last_matrix, ROCK(task)->matrix);
            ROCK(task)->has_last = TRUE;
        }
        /* @hack redundant: a block no path reaches */
        if (0) {
            work = task->work;
        }
        if (task->motion.p != NULL) {
            enemy_set_home(work, task->motion.p);
        }
    }
}

static void floating_rock_draw(struct task *task)
{
    /* @hack spelling: piece, a pointer that walks pieces, where pieces[i]
     * would do */
    struct floating_rock_piece *piece = pieces;
    Float alpha;
    int alpha_byte;
    /* @hack spelling: color, the alpha's byte held, then reused for the
     * packed color, where the packing in the call would do */
    int color;
    int i;

    if (ROCK(task)->has_last == FALSE) {
        return;
    }
    njPushMatrixEx();
    if (task->work->mode == MODE_BROKEN) {
        alpha = 1.0F;
        if (task->work->tick >= BROKEN_STEPS - FADE_STEPS) {
            alpha =
                (Float)(BROKEN_STEPS - task->work->tick) * (1.0F / FADE_STEPS);
        }
        if (alpha < 1.0F) {
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(FADE_ATTR_AND, FADE_ATTR_OR);
            constant_material_set(alpha, 1.0F, 1.0F, 1.0F);
        }
        alpha_byte = (int)(alpha * ALPHA_FULL);
        for (i = 0; i < PIECES; i++, piece++) {
            njPushMatrixEx();
            njTranslateEx(&ROCK(task)->piece_pos[i]);
            njRotateXYZ(NULL, ROCK(task)->piece_ang[i].x,
                        ROCK(task)->piece_ang[i].y, ROCK(task)->piece_ang[i].z);
            njSetTexture(texlists[piece->id]);
            if (alpha < 1.0F) {
                color = alpha_byte;
                if (color > ALPHA_MAX) {
                    color = ALPHA_MAX;
                }
                if (color < 0) {
                    color = 0;
                }
                color = (color << ALPHA_SHIFT) + WHITE_RGB;
                canyon_model_draw_tinted_scaled(models[piece->id], color,
                                                SCALE(task));
            } else {
                canyon_model_draw_scaled(models[piece->id], SCALE(task));
            }
            njPopMatrixEx();
        }
        if (alpha < 1.0F) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
        }
    } else {
        matrix_multiply_before(ROCK(task)->matrix);
        njSetTexture(&floating_rock_texlist);
        canyon_model_draw_scaled(&floating_rock_model, SCALE(task));
    }
    njPopMatrixEx();
}

/* give task its record in extra, with the record's two matrices. it returns
 * the record, or null where the heap can't give all three. a failure frees
 * what the heap gave */
static struct floating_rock *floating_rock_alloc(struct task *task)
{
    NJS_MATRIX *matrix;
    NJS_MATRIX *last_matrix;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((last_matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        syFree(matrix);
        return NULL;
    }
    task->extra.p = syMalloc(sizeof(struct floating_rock));
    if (task->extra.p == NULL) {
        syFree(matrix);
        syFree(last_matrix);
        return NULL;
    }
    /* @hack spelling: one assignment to the three, where a statement for
     * each would do */
    ROCK(task)->unk_24.x = ROCK(task)->unk_24.y = ROCK(task)->unk_24.z = 0.0F;
    /* @hack spelling: one assignment to the three, where a statement for
     * each would do */
    ROCK(task)->unk_30.x = ROCK(task)->unk_30.y = ROCK(task)->unk_30.z = 0.0F;
    ROCK(task)->spin.x = task->work->ang.x;
    /* @hack spelling: one assignment to the two, where a statement for each
     * would do */
    ROCK(task)->spin.y = ROCK(task)->spin.z = 0;
    ROCK(task)->base = task->work->pos;
    ROCK(task)->has_last = FALSE;
    ROCK(task)->matrix = matrix;
    ROCK(task)->last_matrix = last_matrix;
    ROCK(task)->swing_rate =
        (task->work->scl.z + 1.0F) *
        (SWING_LEAST + ((Float)rand() * (SWING_SPREAD * RAND_SCALE)));
    ROCK(task)->phase = (int)((Float)rand() * PHASE_SCALE);
    return ROCK(task);
}

static void floating_rock_free(struct task *task)
{
    if (task->extra.p != NULL) {
        if (ROCK(task)->matrix != NULL) {
            syFree(ROCK(task)->matrix);
        }
        if (ROCK(task)->last_matrix != NULL) {
            syFree(ROCK(task)->last_matrix);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

void floating_rock_set_base(struct task *task)
{
    ROCK(task)->base = task->work->pos;
    ROCK(task)->spin.x = task->work->ang.x;
}

void floating_rock_save(struct task *task)
{
    if (task->extra.p != NULL) {
        ROCK(task)->saved_pos = task->work->pos;
        ROCK(task)->saved_ang = task->work->ang;
        ROCK(task)->saved_scl = task->work->scl;
    }
}

void floating_rock_restore(struct task *task)
{
    if (task->extra.p != NULL) {
        task->work->pos = ROCK(task)->saved_pos;
        task->work->ang = ROCK(task)->saved_ang;
        task->work->scl = ROCK(task)->saved_scl;
    }
}

static void floating_rock_take_mode(struct task *task)
{
    if (NEXT_MODE(task->work) != NO_NEXT_MODE) {
        task->work->mode = NEXT_MODE(task->work);
        NEXT_MODE(task->work) = NO_NEXT_MODE;
    }
}

void floating_rock_set_mode(struct task *task, int mode)
{
    NEXT_MODE(task->work) = mode;
}

/* turn and bob the rock, and break it where a hunter strikes it */
static void floating_rock_whole_step(struct task *task)
{
    /* @hack spelling: task->work read again for scl and tick, where work
     * would do */
    struct task_work *work = task->work;
    struct task *hunter;
    Float spin_speed;
    Float quarter_steps;
    Float climb;
    Float rise;
    Float step;
    Float part;
    Float y;

    spin_speed = SPIN_BASE +
                 (SPIN_SWING *
                  njCos((Angle)((Float)((game_state.steps + ROCK(task)->phase) *
                                        SWING_TURN) *
                                ROCK(task)->swing_rate * SWING_SCALE)));
    ROCK(task)->spin.x = (Angle)(spin_speed * SPIN_X);
    ROCK(task)->spin.y = (Angle)(spin_speed * SPIN_Y);
    ROCK(task)->spin.z = (Angle)(spin_speed * SPIN_Z);
    work->ang.x += ROCK(task)->spin.x;
    work->ang.y += ROCK(task)->spin.y;
    work->ang.z += ROCK(task)->spin.z;
    /* the bob climbs rise in each of its first two quarters, easing in, then
     * out. it falls back the same way in the last two */
    /* @hack spelling: quarter_steps set, then raised by 1 in a statement of
     * its own, where scl.z + 1 in one would do */
    quarter_steps = task->work->scl.z;
    quarter_steps += 1.0F;
    climb = (task->work->scl.y + 1.0F) * CLIMB_STEP;
    if (quarter_steps == 0.0F) {
        quarter_steps = ZERO_QUARTER;
    }
    step = (Float)((game_state.steps + ROCK(task)->phase) %
                   (int)(quarter_steps * 4.0F));
    rise = quarter_steps * climb * QUARTER_RISE;
    if (step >= quarter_steps) {
        y = rise;
        if (step >= quarter_steps * 2.0F) {
            y += rise;
            if (step >= quarter_steps * 3.0F) {
                y -= rise;
                part = ((quarter_steps * 4.0F) - step) / quarter_steps;
                y -= rise - (rise * part * part);
            } else {
                part = (step - (quarter_steps * 2.0F)) / quarter_steps;
                y -= rise * part * part;
            }
        } else {
            part = ((quarter_steps * 2.0F) - step) / quarter_steps;
            y += rise - (rise * part * part);
        }
    } else {
        part = step / quarter_steps;
        y = rise * part * part;
    }
    work->pos.y = ROCK(task)->base.y + y;
    if ((work->collision->flags & COLLISION_TOUCHED) &&
        (hunter = collision_next_player(task)) != NULL) {
        if (hunter_attacking(hunter)) {
            if (task->motion.p != NULL && !placement_get_state(task)) {
                enemy_defeat(work, task->motion.p);
            }
            placement_set_state(task, 1);
            floating_rock_set_mode(task, MODE_BROKEN);
            task->work->tick = 0;
            floating_rock_break(task, hunter);
            sound_play(BREAK_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, break_volume);
            vibration_start(player_slot_of(hunter), BREAK_POWER, BREAK_FREQ,
                            VIBRATION_UNTIMED);
            return;
        }
    }
    collision_register(task);
}

/* break the rock under hunter: throw its dust, and send each piece flying at
 * the rock's angles */
static void floating_rock_break(struct task *task, struct task *hunter)
{
    /* @hack spelling: task->work read again for top, the dust and the
     * pieces' ang, where work would do */
    struct task_work *work = task->work;
    /* @hack spelling: piece, a local for pieces, where pieces indexed in
     * place would do */
    struct floating_rock_piece *piece = pieces;
    struct player *player;
    NJS_POINT3 last_pos[PIECES];
    NJS_VECTOR spd;
    /* @hack redundant: top, a point above the rock that nothing reads */
    NJS_POINT3 top;
    Angle ang;
    Float push_x;
    Float push_z;
    int i;

    player = player_records[player_slot_of(hunter)];
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, hunter->work->ang.z);
    njRotateX(NULL, hunter->work->ang.x);
    njRotateY(NULL, -hunter->work->ang.y);
    njCalcPoint(NULL, &player->spd, &spd);
    njPopMatrixEx();
    ang = mover_player_heading(task, player_slot_of(hunter));
    top = task->work->pos;
    top.y += RADIUS * SCALE(task);
    for (i = 0; i < BREAK_DUSTS; i++) {
        canyon_dust_behind(
            &task->work->pos, &spd,
            SCALE(task) *
                (DUST_LEAST + ((Float)rand() * (DUST_SPREAD * RAND_SCALE))) *
                RADIUS,
            dust_size * (1.0F - ((Float)rand() * (DUST_SHRINK * RAND_SCALE))) *
                SCALE(task));
    }
    canyon_polar_xz(ang, push_back, &push_x, &push_z);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_multiply_before(ROCK(task)->matrix);
    njScale(NULL, SCALE(task), SCALE(task), SCALE(task));
    for (i = 0; i < PIECES; i++) {
        njCalcPoint(NULL, &piece[i].pos, &ROCK(task)->piece_pos[i]);
    }
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_multiply_before(ROCK(task)->last_matrix);
    njScale(NULL, SCALE(task), SCALE(task), SCALE(task));
    for (i = 0; i < PIECES; i++) {
        njCalcPoint(NULL, &piece[i].pos, &last_pos[i]);
    }
    njPopMatrixEx();
    for (i = 0; i < PIECES; i++) {
        ROCK(task)->piece_spd[i].x =
            (ROCK(task)->piece_pos[i].x - work->pos.x) * spread;
        ROCK(task)->piece_spd[i].y =
            (ROCK(task)->piece_pos[i].y - work->pos.y) * spread;
        ROCK(task)->piece_spd[i].z =
            (ROCK(task)->piece_pos[i].z - work->pos.z) * spread;
        ROCK(task)->piece_spd[i].x +=
            (ROCK(task)->piece_pos[i].x - last_pos[i].x) * carry;
        ROCK(task)->piece_spd[i].y +=
            (ROCK(task)->piece_pos[i].y - last_pos[i].y) * carry;
        ROCK(task)->piece_spd[i].z +=
            (ROCK(task)->piece_pos[i].z - last_pos[i].z) * carry;
        ROCK(task)->piece_spd[i].x += push_x + (spd.x * player_carry);
        ROCK(task)->piece_spd[i].y += spd.y * player_carry;
        ROCK(task)->piece_spd[i].z += push_z + (spd.z * player_carry);
        ROCK(task)->piece_spd[i].x *=
            PIECE_SPEED_MOST -
            ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE));
        ROCK(task)->piece_spd[i].y *=
            PIECE_SPEED_MOST -
            ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE));
        ROCK(task)->piece_spd[i].z *=
            PIECE_SPEED_MOST -
            ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE));
        ROCK(task)->piece_acc[i].x = 0.0F;
        ROCK(task)->piece_acc[i].y = PIECE_LIFT;
        ROCK(task)->piece_acc[i].z = 0.0F;
        ROCK(task)->piece_spin[i].x =
            ROCK(task)->spin.x * piece_spin_scale *
            (PIECE_SPIN_MOST -
             ((Float)rand() * (PIECE_SPIN_RANGE * RAND_SCALE)));
        ROCK(task)->piece_spin[i].y =
            ROCK(task)->spin.y * piece_spin_scale *
            (PIECE_SPIN_MOST -
             ((Float)rand() * (PIECE_SPIN_RANGE * RAND_SCALE)));
        ROCK(task)->piece_spin[i].z =
            ROCK(task)->spin.z * piece_spin_scale *
            (PIECE_SPIN_MOST -
             ((Float)rand() * (PIECE_SPIN_RANGE * RAND_SCALE)));
        ROCK(task)->piece_ang[i] = task->work->ang;
    }
}

/* move the pieces, and stop the placement at the last broken step */
static void floating_rock_broken_step(struct task *task)
{
    int id;
    int i;

    for (i = 0; i < PIECES; i++) {
        id = pieces[i].id;
        ROCK(task)->piece_acc[i].y =
            ((areas[id] * LIFT) / masses[id]) - GRAVITY;
        ROCK(task)->piece_spd[i].x += ROCK(task)->piece_acc[i].x;
        ROCK(task)->piece_spd[i].y += ROCK(task)->piece_acc[i].y;
        ROCK(task)->piece_spd[i].z += ROCK(task)->piece_acc[i].z;
        ROCK(task)->piece_spd[i].x *= DRAG;
        ROCK(task)->piece_spd[i].y *= DRAG;
        ROCK(task)->piece_spd[i].z *= DRAG;
        ROCK(task)->piece_pos[i].x += ROCK(task)->piece_spd[i].x;
        ROCK(task)->piece_pos[i].y += ROCK(task)->piece_spd[i].y;
        ROCK(task)->piece_pos[i].z += ROCK(task)->piece_spd[i].z;
        ROCK(task)->piece_spin[i].x *= DRAG;
        ROCK(task)->piece_spin[i].y *= DRAG;
        ROCK(task)->piece_spin[i].z *= DRAG;
        ROCK(task)->piece_ang[i].x += ROCK(task)->piece_spin[i].x;
        ROCK(task)->piece_ang[i].y += ROCK(task)->piece_spin[i].y;
        ROCK(task)->piece_ang[i].z += ROCK(task)->piece_spin[i].z;
    }
    if (task->work->tick++ == BROKEN_STEPS) {
        placement_kill(task);
    }
}
