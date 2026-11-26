/* the stones a wind throws up. a stone spins as it rises, and can break into
 * pieces. it fades out once its y passes its wind's scl.y */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "shared/canyon_util.h"
#include "shared/floating_rock.h"
#include "shared/wind_stone.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"

/* the most stones an owner keeps */
#define STONES_MAX 100
/* the tables by kind run from 0, but no stone is of kind 0. every whole stone
 * draws model 0 of wind_stone_models, whatever its kind */
#define KINDS 6

/* a stone's acc.y is LIFT times its kind's area over its mass, less GRAVITY.
 * each step its speed gains acc, then keeps DRAG of itself. a whole stone
 * starts at a speed up of START_RISE, and a piece at an acc.y of PIECE_LIFT */
#define LIFT 0.06F
#define GRAVITY 0.08F
#define DRAG 0.995F
#define START_RISE 0.02F
#define PIECE_LIFT 0.1F

/* a stone of spin rate 1 turns SPIN_X Angle units a step about x, SPIN_Y about
 * y and SPIN_Z about z. each turn falls short by up to SPIN_SHORTFALL of
 * itself, at random. SPIN_BASE cancels out of a whole stone's spin */
#define SPIN_BASE 0.06F
#define SPIN_X 640.0F
#define SPIN_Y 128.0F
#define SPIN_Z 96.0F
#define SPIN_SHORTFALL 0.1F

/* a stone fades out over FADE_STEPS steps */
#define FADE_STEPS 15

/* each step, a whole stone above a y of BREAK_HEIGHT can break. one of kind
 * SPLIT_KIND breaks into pieces at the odds SPLIT_ODDS. one of kind SPLIT_KIND
 * or CHIP_KIND sheds a piece, of kind CHIP_PIECE_KIND, at the odds CHIP_ODDS */
#define SPLIT_KIND 1
#define CHIP_KIND 2
#define CHIP_PIECE_KIND 5
#define BREAK_HEIGHT 35.0F
#define SPLIT_ODDS 0.025F
#define CHIP_ODDS 0.0125F
#define SPLIT_PIECES 5
#define SPLIT_PIECES_TWO_PLAYERS 3
/* a split throws SPLIT_DUSTS dusts, and a chip one */
#define SPLIT_DUSTS 2
/* a piece's speed is its point's motion over the stone's last step. it falls
 * short of that by up to PIECE_SHORTFALL of it, at random */
#define PIECE_SHORTFALL 0.2F
/* a stone's size is the largest axis of its scl. a break's dust starts that
 * size times DUST_REACH from the stone's pos. a random factor from
 * DUST_REACH_LEAST up to DUST_REACH_RANGE more scales that distance. the dust's
 * size is split_dust_size or chip_dust_size times the stone's, less up to
 * DUST_SHORTFALL of it at random. the dust moves at DUST_SPEED times the motion
 * of the stone's pos over its last step */
#define DUST_REACH 7.5F
#define DUST_REACH_LEAST 1.25F
#define DUST_REACH_RANGE 0.5F
#define DUST_SHORTFALL 0.2F
#define DUST_SPEED 2.0F
/* a whole stone's roll picks its kind and a scale. each axis of its scl is
 * SCL_MOST times that scale, less up to SCL_SHORTFALL of it at random */
#define SCL_MOST 2.0F
#define SCL_SHORTFALL 0.5F

/* a fading stone's tint: white, at an alpha of CHANNEL_MAX times its alpha.
 * ALPHA_SHIFT puts the alpha's byte at the top of the tint */
#define CHANNEL_MAX 0xff
#define WHITE 0xffffff

/* a fading stone draws in a white constant material of its alpha. each strip's
 * own flags give way to FADE_ATTR_OR's */
#define FADE_ATTR_AND CHUNK_TYPE_BITS
#define FADE_ATTR_OR (NJD_FST_UA | NJD_FST_IS)

/* a piece that breaks off a stone. pos is its place in the stone's own axes,
 * before the stone's scale */
struct wind_stone_piece {
    NJS_POINT3 pos;
    Sint32 kind;
};

static struct wind_stone_piece split_pieces[SPLIT_PIECES] = {
    {{-1.0187F, 3.7468F, 0.668F}, 1},   {{2.874F, -0.0612F, -2.0064F}, 2},
    {{-2.8207F, 0.1629F, -2.1638F}, 3}, {{-2.2171F, -0.7693F, 2.0971F}, 4},
    {{-0.5516F, 3.1947F, -0.4371F}, 5},
};
/* where the piece a stone sheds lies, as split_pieces' piece of kind
 * CHIP_PIECE_KIND does */
static NJS_POINT3 chip_piece_pos = {-0.5516F, 3.1947F, -0.4371F};
/* the most of each axis of a stone's speed, by kind */
static Float speed_limits[KINDS] = {2.5F, 3.0F, 3.5F, 4.0F, 4.5F, 5.0F};
/* how fast a stone spins, by kind */
static Float spin_rates[KINDS] = {1.0F, 1.25F, 1.5F, 2.0F, 2.25F, 2.5F};
/* clang-format off */
static struct collision_shape wind_stone_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_GUARD_LEVEL(3), 0,
    {0.0F, 0.0F, 0.0F}, 5.0F, 5.0F, 5.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Float split_dust_size = 8.0F;
static Float chip_dust_size = 4.0F;

static void wind_stone_set_matrix(struct wind_stone *stone);
static void wind_stone_split(struct wind_stone *stone);
static void wind_stone_chip(struct wind_stone *stone);

/* make a whole stone and its two matrices at the head of owner's list, and
 * count it. return the stone, or null where the heap has no room for all
 * three */
static struct wind_stone *wind_stone_alloc(struct task *owner)
{
    NJS_MATRIX *matrix;
    NJS_MATRIX *last_matrix;
    struct wind_stone *stone;

    /* @hack spelling: the assignment inside the test, here and in the two
     * tests below, where a statement of its own would do */
    if ((matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        return NULL;
    }
    if ((last_matrix = syMalloc(sizeof(NJS_MATRIX))) == NULL) {
        syFree(matrix);
        return NULL;
    }
    if ((stone = syMalloc(sizeof(struct wind_stone))) == NULL) {
        syFree(matrix);
        syFree(last_matrix);
        return NULL;
    }
    stone->has_last = FALSE;
    stone->matrix = matrix;
    stone->last_matrix = last_matrix;
    stone->prev = NULL;
    stone->next = WIND_STONES(owner);
    if (stone->next != NULL) {
        stone->next->prev = stone;
    }
    stone->owner = owner;
    owner->force.p = stone;
    WIND_STONE_COUNT(owner)++;
    stone->is_piece = FALSE;
    stone->alpha = 1.0F;
    return stone;
}

struct wind_stone *wind_stone_create(struct task *owner, NJS_POINT3 *pos)
{
    struct wind_stone *stone;
    Float roll;
    Float scale;
    /* @hack spelling: spin_base, a local for SPIN_BASE, where SPIN_BASE in
     * place would do */
    Float spin_base;

    if (WIND_STONE_COUNT(owner) - WIND_STONE_PIECE_COUNT(owner) >=
        WIND_STONE_WHOLE_MAX) {
        return NULL;
    }
    if (WIND_STONE_COUNT(owner) >= STONES_MAX) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((stone = wind_stone_alloc(owner)) == NULL) {
        return NULL;
    }
    roll = (Float)rand() * RAND_SCALE;
    if (roll < 0.5F) {
        scale = 0.25F;
        stone->kind = 5;
    } else if (roll < 0.75F) {
        scale = 0.675F;
        stone->kind = 4;
    } else if (roll < 0.9F) {
        scale = 0.875F;
        stone->kind = 3;
    } else if (roll < 0.95F) {
        scale = 0.925F;
        stone->kind = 2;
    } else if (roll < 0.98F) {
        scale = 0.965F;
        stone->kind = 1;
    } else {
        scale = 1.2F;
        stone->kind = 1;
    }
    stone->scl.x = scale *
                   (1.0F - ((Float)rand() * (SCL_SHORTFALL * RAND_SCALE))) *
                   SCL_MOST;
    stone->scl.y = scale *
                   (1.0F - ((Float)rand() * (SCL_SHORTFALL * RAND_SCALE))) *
                   SCL_MOST;
    stone->scl.z = scale *
                   (1.0F - ((Float)rand() * (SCL_SHORTFALL * RAND_SCALE))) *
                   SCL_MOST;
    stone->pos = *pos;
    stone->spd.x = 0.0F;
    stone->spd.y = START_RISE;
    stone->spd.z = 0.0F;
    /* @hack spelling: one assignment to the three, where a statement for
     * each would do */
    stone->acc.x = stone->acc.y = stone->acc.z = 0.0F;
    stone->ang.x = RANDOM_TURN();
    stone->ang.y = RANDOM_TURN();
    stone->ang.z = RANDOM_TURN();
    spin_base = SPIN_BASE;
    stone->spin.x = spin_rates[stone->kind] * spin_base *
                    (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                    (SPIN_X * (1.0F / SPIN_BASE));
    stone->spin.y = spin_rates[stone->kind] * spin_base *
                    (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                    (SPIN_Y * (1.0F / SPIN_BASE));
    stone->spin.z = spin_rates[stone->kind] * spin_base *
                    (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                    (SPIN_Z * (1.0F / SPIN_BASE));
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&stone->pos);
    njRotateXYZ(NULL, stone->ang.x, stone->ang.y, stone->ang.z);
    matrix_get(stone->matrix);
    njPopMatrixEx();
    if (!stone->is_piece && !stone->has_last) {
        njSetMatrix(stone->last_matrix, stone->matrix);
        stone->has_last = TRUE;
    }
    return stone;
}

/* make a piece at the head of owner's list, for the caller to place, and return
 * it. return null where owner has WIND_STONE_PIECE_MAX pieces or STONES_MAX
 * stones already. return null too where the heap has no room for the piece */
static struct wind_stone *wind_stone_create_piece(struct task *owner)
{
    struct wind_stone *stone;

    if (WIND_STONE_PIECE_COUNT(owner) >= WIND_STONE_PIECE_MAX) {
        return NULL;
    }
    if (WIND_STONE_COUNT(owner) >= STONES_MAX) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((stone = wind_stone_alloc(owner)) == NULL) {
        return NULL;
    }
    stone->is_piece = TRUE;
    WIND_STONE_PIECE_COUNT(owner)++;
    return stone;
}

void wind_stone_free(struct wind_stone *stone)
{
    if (stone != NULL) {
        if (stone->matrix != NULL) {
            syFree(stone->matrix);
        }
        if (stone->last_matrix != NULL) {
            syFree(stone->last_matrix);
        }
        if (stone->next != NULL) {
            stone->next->prev = stone->prev;
        }
        if (stone->prev != NULL) {
            stone->prev->next = stone->next;
        } else if (stone->owner != NULL) {
            stone->owner->force.p = stone->next;
        }
        if (stone->owner != NULL) {
            WIND_STONE_COUNT(stone->owner)--;
            if (stone->is_piece) {
                WIND_STONE_PIECE_COUNT(stone->owner)--;
            }
        }
        syFree(stone);
    }
}

/* set stone's matrix from its pos and ang. a whole stone's last_matrix takes
 * the matrix before, or the new one where it has none */
static void wind_stone_set_matrix(struct wind_stone *stone)
{
    if (!stone->is_piece && stone->has_last) {
        njSetMatrix(stone->last_matrix, stone->matrix);
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&stone->pos);
    njRotateXYZ(NULL, stone->ang.x, stone->ang.y, stone->ang.z);
    matrix_get(stone->matrix);
    njPopMatrixEx();
    if (!stone->is_piece && !stone->has_last) {
        njSetMatrix(stone->last_matrix, stone->matrix);
        stone->has_last = TRUE;
    }
}

void wind_stone_step(struct wind_stone *stone)
{
    /* @hack redundant: sx and sz, which only a compare that never runs
     * reads */
    Float sx;
    Float sz;

    if (stone->pos.y > stone->owner->work->scl.y) {
        stone->alpha -= 1.0F / FADE_STEPS;
        if (stone->alpha < 0.0F) {
            wind_stone_free(stone);
            return;
        }
    }
    stone->spd.x += stone->acc.x;
    stone->spd.y += stone->acc.y;
    stone->spd.z += stone->acc.z;
    stone->spd.x *= DRAG;
    stone->spd.y *= DRAG;
    stone->spd.z *= DRAG;
    if (stone->spd.x > speed_limits[stone->kind]) {
        stone->spd.x = speed_limits[stone->kind];
    }
    if (stone->spd.y > speed_limits[stone->kind]) {
        stone->spd.y = speed_limits[stone->kind];
    }
    if (stone->spd.z > speed_limits[stone->kind]) {
        stone->spd.z = speed_limits[stone->kind];
    }
    stone->pos.x += stone->spd.x;
    stone->pos.y += stone->spd.y;
    stone->pos.z += stone->spd.z;
    stone->ang.x += stone->spin.x;
    stone->ang.y += stone->spin.y;
    stone->ang.z += stone->spin.z;
    sx = stone->scl.x;
    sz = stone->scl.z;
    /* @hack redundant: a compare of sx and sz, in a block no path reaches */
    if (0) {
        if (sx > sz) {
        }
    }
    stone->acc.x = 0.0F;
    stone->acc.y = ((wind_stone_areas[stone->kind] * LIFT) /
                    wind_stone_masses[stone->kind]) -
                   GRAVITY;
    stone->acc.z = 0.0F;
    wind_stone_set_matrix(stone);
    if (!stone->is_piece && stone->kind == SPLIT_KIND &&
        stone->pos.y > BREAK_HEIGHT &&
        (Float)rand() * RAND_SCALE < SPLIT_ODDS &&
        WIND_STONE_PIECE_COUNT(stone->owner) + SPLIT_PIECES <=
            WIND_STONE_PIECE_MAX &&
        in_view_box(&stone->pos)) {
        wind_stone_split(stone);
        wind_stone_free(stone);
        return;
    }
    if (game_state.two_players == 0 && !stone->is_piece &&
        (stone->kind == SPLIT_KIND || stone->kind == CHIP_KIND) &&
        stone->pos.y > BREAK_HEIGHT && (Float)rand() * RAND_SCALE < CHIP_ODDS &&
        WIND_STONE_PIECE_COUNT(stone->owner) + 1 <= WIND_STONE_PIECE_MAX &&
        in_view_box(&stone->pos)) {
        wind_stone_chip(stone);
    }
}

void wind_stone_draw(struct wind_stone *stone)
{
    int index;
    /* @hack spelling: color, the alpha's byte, then reused for the packed
     * color, where the packing in the call would do */
    int color;

    index = stone->is_piece ? stone->kind : 0;
    if (stone->alpha < 1.0F) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(FADE_ATTR_AND, FADE_ATTR_OR);
        constant_material_set(stone->alpha, 1.0F, 1.0F, 1.0F);
    }
    njPushMatrixEx();
    matrix_multiply_before(stone->matrix);
    njSetTexture(wind_stone_texlists[index]);
    if (stone->alpha < 1.0F) {
        color = stone->alpha * CHANNEL_MAX;
        if (color > CHANNEL_MAX) {
            color = CHANNEL_MAX;
        }
        if (color < 0) {
            color = 0;
        }
        color = (color << ALPHA_SHIFT) + WHITE;
        canyon_model_draw_tinted_scaled_v(wind_stone_models[index], color,
                                          &stone->scl);
    } else {
        canyon_model_draw_scaled_v(wind_stone_models[index], &stone->scl);
    }
    njPopMatrixEx();
    if (stone->alpha < 1.0F) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
}

/* break stone into the first pieces of split_pieces, five or three in a
 * two-player game, and throw dust. where the first piece can't be made, it
 * does nothing. each piece takes stone's ang and scl. the caller frees
 * stone */
static void wind_stone_split(struct wind_stone *stone)
{
    NJS_POINT3 center;
    NJS_VECTOR spd;
    /* @hack redundant: top, a point above the stone that nothing reads */
    NJS_POINT3 top;
    struct wind_stone *pieces[SPLIT_PIECES];
    NJS_POINT3 point;
    /* @hack spelling: piece, a local for split_pieces, where split_pieces
     * indexed in place would do */
    struct wind_stone_piece *piece = split_pieces;
    int count;
    int i;
    Float size;

    count = game_state.two_players ? SPLIT_PIECES_TWO_PLAYERS : SPLIT_PIECES;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_multiply_before(stone->matrix);
    njScale(NULL, stone->scl.x, stone->scl.y, stone->scl.z);
    for (i = 0; i < count; i++) {
        njCalcPoint(NULL, &piece[i].pos, &point);
        pieces[i] = wind_stone_create_piece(stone->owner);
        if (pieces[i] == NULL) {
            if (i == 0) {
                break;
            }
        } else {
            pieces[i]->kind = piece[i].kind;
            pieces[i]->pos = point;
        }
    }
    njPopMatrixEx();
    if (i == 0) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_multiply_before(stone->last_matrix);
    njGetTranslation(NULL, &center);
    spd.x = (stone->pos.x - center.x) * DUST_SPEED;
    spd.y = (stone->pos.y - center.y) * DUST_SPEED;
    spd.z = (stone->pos.z - center.z) * DUST_SPEED;
    njScale(NULL, stone->scl.x, stone->scl.y, stone->scl.z);
    for (i = 0; i < count; i++) {
        if (pieces[i] != NULL) {
            njCalcPoint(NULL, &piece[i].pos, &center);
            pieces[i]->spd.x = pieces[i]->pos.x - center.x;
            pieces[i]->spd.y = pieces[i]->pos.y - center.y;
            pieces[i]->spd.z = pieces[i]->pos.z - center.z;
        }
    }
    njPopMatrixEx();
    size = stone->scl.x > stone->scl.y ? stone->scl.x : stone->scl.y;
    size = size > stone->scl.z ? size : stone->scl.z;
    top = stone->pos;
    top.y += DUST_REACH * size;
    for (i = 0; i < SPLIT_DUSTS; i++) {
        canyon_dust_behind(
            &stone->pos, &spd,
            size *
                (DUST_REACH_LEAST +
                 ((Float)rand() * (DUST_REACH_RANGE * RAND_SCALE))) *
                DUST_REACH,
            split_dust_size *
                (1.0F - ((Float)rand() * (DUST_SHORTFALL * RAND_SCALE))) *
                size);
    }
    for (i = 0; i < count; i++) {
        if (pieces[i] != NULL) {
            pieces[i]->spd.x *=
                1.0F - ((Float)rand() * (PIECE_SHORTFALL * RAND_SCALE));
            pieces[i]->spd.y *=
                1.0F - ((Float)rand() * (PIECE_SHORTFALL * RAND_SCALE));
            pieces[i]->spd.z *=
                1.0F - ((Float)rand() * (PIECE_SHORTFALL * RAND_SCALE));
            pieces[i]->acc.x = 0.0F;
            pieces[i]->acc.y = PIECE_LIFT;
            pieces[i]->acc.z = 0.0F;
            pieces[i]->spin.x =
                spin_rates[pieces[i]->kind] *
                (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                SPIN_X;
            pieces[i]->spin.y =
                spin_rates[pieces[i]->kind] *
                (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                SPIN_Y;
            pieces[i]->spin.z =
                spin_rates[pieces[i]->kind] *
                (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                SPIN_Z;
            pieces[i]->ang = stone->ang;
            pieces[i]->scl = stone->scl;
            wind_stone_set_matrix(pieces[i]);
        }
    }
}

/* break the piece at chip_piece_pos off stone, as wind_stone_split breaks off
 * each of its pieces. the piece is of kind CHIP_PIECE_KIND, and stone stays
 * whole */
static void wind_stone_chip(struct wind_stone *stone)
{
    NJS_POINT3 center;
    NJS_VECTOR spd;
    /* @hack redundant: top, a point above the stone that nothing reads */
    NJS_POINT3 top;
    /* @hack spelling: pieces, an array of one, and loops over it, where a
     * pointer would do */
    struct wind_stone *pieces[1];
    NJS_POINT3 point;
    int i;
    Float size;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_multiply_before(stone->matrix);
    njScale(NULL, stone->scl.x, stone->scl.y, stone->scl.z);
    for (i = 0; i < 1; i++) {
        njCalcPoint(NULL, &chip_piece_pos, &point);
        pieces[i] = wind_stone_create_piece(stone->owner);
        if (pieces[i] == NULL) {
            if (i == 0) {
                break;
            }
        } else {
            pieces[i]->kind = CHIP_PIECE_KIND;
            pieces[i]->pos = point;
        }
    }
    njPopMatrixEx();
    if (i == 0) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_multiply_before(stone->last_matrix);
    njGetTranslation(NULL, &center);
    spd.x = (stone->pos.x - center.x) * DUST_SPEED;
    spd.y = (stone->pos.y - center.y) * DUST_SPEED;
    spd.z = (stone->pos.z - center.z) * DUST_SPEED;
    njScale(NULL, stone->scl.x, stone->scl.y, stone->scl.z);
    for (i = 0; i < 1; i++) {
        if (pieces[i] != NULL) {
            njCalcPoint(NULL, &chip_piece_pos, &center);
            pieces[i]->spd.x = pieces[i]->pos.x - center.x;
            pieces[i]->spd.y = pieces[i]->pos.y - center.y;
            pieces[i]->spd.z = pieces[i]->pos.z - center.z;
        }
    }
    njPopMatrixEx();
    size = stone->scl.x > stone->scl.y ? stone->scl.x : stone->scl.y;
    size = size > stone->scl.z ? size : stone->scl.z;
    top = stone->pos;
    top.y += DUST_REACH * size;
    canyon_dust_behind(
        &stone->pos, &spd,
        size *
            (DUST_REACH_LEAST +
             ((Float)rand() * (DUST_REACH_RANGE * RAND_SCALE))) *
            DUST_REACH,
        chip_dust_size *
            (1.0F - ((Float)rand() * (DUST_SHORTFALL * RAND_SCALE))) * size);
    for (i = 0; i < 1; i++) {
        if (pieces[i] != NULL) {
            pieces[i]->spd.x *=
                1.0F - ((Float)rand() * (PIECE_SHORTFALL * RAND_SCALE));
            pieces[i]->spd.y *=
                1.0F - ((Float)rand() * (PIECE_SHORTFALL * RAND_SCALE));
            pieces[i]->spd.z *=
                1.0F - ((Float)rand() * (PIECE_SHORTFALL * RAND_SCALE));
            pieces[i]->acc.x = 0.0F;
            pieces[i]->acc.y = PIECE_LIFT;
            pieces[i]->acc.z = 0.0F;
            pieces[i]->spin.x =
                spin_rates[pieces[i]->kind] *
                (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                SPIN_X;
            pieces[i]->spin.y =
                spin_rates[pieces[i]->kind] *
                (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                SPIN_Y;
            pieces[i]->spin.z =
                spin_rates[pieces[i]->kind] *
                (1.0F - ((Float)rand() * (SPIN_SHORTFALL * RAND_SCALE))) *
                SPIN_Z;
            pieces[i]->ang = stone->ang;
            pieces[i]->scl = stone->scl;
            wind_stone_set_matrix(pieces[i]);
        }
    }
}
