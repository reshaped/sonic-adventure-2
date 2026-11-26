/* Death Chamber's lit floors: a task named ObjectFloorExec draws two floors of
 * tiles, 3 rows of 8 each. the tiles flash white and dim again, in three groups
 * by turns */
#include "draw/color.h"
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "stg25/death_chamber.h"
#include "stg25/floor_tiles.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "stg25/floor_tiles/text.inc"

#define FLOOR_TILES_LEVEL 1

#define ROWS 3
#define COLUMNS 8
#define TILE_KINDS 3

/* the hall's floor runs its columns COLUMN_PITCH apart along x, and its rows
 * ROW_PITCH apart along z. the far east's is the same floor turned a quarter
 * turn about y, tiles and all */
#define COLUMN_PITCH 31.25F
#define ROW_PITCH 55.0F
#define HALL_FLOOR_X (-125.0F)
#define HALL_FLOOR_Y (-130.0F)
#define HALL_FLOOR_Z 730.0F
#define FAR_EAST_FLOOR_X 1345.0F
#define FAR_EAST_FLOOR_Y (-30.0F)
#define FAR_EAST_FLOOR_Z 125.0F

/* a tile's light is how far its color lies toward white, from 0 to 1. it
 * repeats every CYCLE_STEPS of game_state.steps. a tile of phase p runs
 * p * PHASE_STEPS ahead of a tile of phase 0. a tile of phase 3 runs a whole
 * cycle ahead, so it lights with phase 0's */
#define CYCLE_STEPS 180
#define PHASE_STEPS 60
/* a tile's light rises from 0 by RISE a step, to 1 at step RISE_STEPS - 1. it
 * then falls by FALL a step, to 0 at step LIT_STEPS - 1. it is 0 for the rest
 * of the cycle. a tile shows a light of DIMMEST at least */
#define LIT_STEPS 120
#define RISE_STEPS 5
#define RISE (1.0F / (RISE_STEPS - 1))
#define FALL (1.0F / ((LIT_STEPS - 1) - RISE_STEPS))
#define DIMMEST 0.2F

/* the colors whiten_color mixes and the table's colors become. each holds 8
 * bits a channel: alpha in the top byte, then red, green and blue */
#define CHANNEL_MAX 255.0F
#define CHANNEL_MASK 0xff
#define OPAQUE 0xff000000
/* chamber_floor_tiles_table's colors hold 5 bits a channel: red in the
 * lowest, then green, then blue */
#define TABLE_CHANNEL_MAX 0x1f
#define TABLE_GREEN_SHIFT 5
#define TABLE_BLUE_SHIFT 10
#define TABLE_SCALE_SHIFT 2

static NJS_TEXNAME chamber_floor_tiles_texture_names_0[1] = {
#include "stg25/floor_tiles/chamber_floor_tiles_texture_names_0.inc"
};

static NJS_TEXLIST chamber_floor_tiles_texlist_0 = {
#include "stg25/floor_tiles/chamber_floor_tiles_texlist_0.inc"
};

static Sint16 chamber_floor_tiles_model_0_polygons[25] = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_0_polygons.inc"
};

static Sint32 chamber_floor_tiles_model_0_vertices[19] = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_0_vertices.inc"
};

static NJS_CNK_MODEL chamber_floor_tiles_model_0 = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_0.inc"
};

static NJS_CNK_OBJECT chamber_floor_tiles_0_object = {
#include "stg25/floor_tiles/chamber_floor_tiles_0_object.inc"
};

static NJS_TEXNAME chamber_floor_tiles_texture_names_1[1] = {
#include "stg25/floor_tiles/chamber_floor_tiles_texture_names_1.inc"
};

static NJS_TEXLIST chamber_floor_tiles_texlist_1 = {
#include "stg25/floor_tiles/chamber_floor_tiles_texlist_1.inc"
};

static Sint16 chamber_floor_tiles_model_1_polygons[25] = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_1_polygons.inc"
};

static Sint32 chamber_floor_tiles_model_1_vertices[19] = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_floor_tiles_model_1 = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_1.inc"
};

static NJS_CNK_OBJECT chamber_floor_tiles_1_object = {
#include "stg25/floor_tiles/chamber_floor_tiles_1_object.inc"
};

static NJS_TEXNAME chamber_floor_tiles_texture_names_2[1] = {
#include "stg25/floor_tiles/chamber_floor_tiles_texture_names_2.inc"
};

static NJS_TEXLIST chamber_floor_tiles_texlist_2 = {
#include "stg25/floor_tiles/chamber_floor_tiles_texlist_2.inc"
};

static Sint16 chamber_floor_tiles_model_2_polygons[25] = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_2_polygons.inc"
};

static Sint32 chamber_floor_tiles_model_2_vertices[19] = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_floor_tiles_model_2 = {
#include "stg25/floor_tiles/chamber_floor_tiles_model_2.inc"
};

static NJS_CNK_OBJECT chamber_floor_tiles_2_object = {
#include "stg25/floor_tiles/chamber_floor_tiles_2_object.inc"
};

/* chamber_floor_tiles_table's color at index, as an opaque color of 8 bits a
 * channel. each 5-bit channel moves up TABLE_SCALE_SHIFT bits, so a channel
 * tops out at 0x7c. nothing calls chamber_floor_tiles_table_color, and no
 * header declares it. it is external only because SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint32 chamber_floor_tiles_table_color(int index);
/* NOLINTEND(misc-use-internal-linkage) */

static void chamber_floor_tiles_destroy(struct task *task);
static void chamber_floor_tiles_step(struct task *task);
static void chamber_floor_tiles_draw(struct task *task);
static Uint32 whiten_color(Uint32 color, Float part);

static NJS_CNK_MODEL *chamber_floor_tiles_models[TILE_KINDS] = {
#include "stg25/floor_tiles/chamber_floor_tiles_models.inc"
};
NJS_TEXLIST *chamber_floor_tiles_texlists[TILE_KINDS + 1] = {
#include "stg25/floor_tiles/chamber_floor_tiles_texlists.inc"
};
/* 90 colors, which only chamber_floor_tiles_table_color reads */
static Uint16 chamber_floor_tiles_table[90] = {
#include "stg25/floor_tiles/chamber_floor_tiles_table.inc"
};
/* each tile's phase, by row and column */
static Uint8 chamber_floor_tiles_phases[ROWS][COLUMNS] = {
    {0, 1, 2, 3, 3, 2, 1, 0},
    {1, 2, 3, 0, 0, 3, 2, 1},
    {0, 1, 2, 3, 3, 2, 1, 0},
};
/* each tile's kind on the hall's floor, and on the far east's */
static Uint8 chamber_floor_tiles_hall_kinds[ROWS][COLUMNS] = {
    {2, 2, 2, 2, 2, 2, 2, 2},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
};
static Uint8 chamber_floor_tiles_far_east_kinds[ROWS][COLUMNS] = {
    {2, 2, 2, 2, 2, 2, 2, 2},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
};
/* the tiles' own color, which whiten_color moves toward white */
static Uint32 chamber_floor_tiles_color = 0xff503c32;

struct task *chamber_floor_tiles_create(void)
{
    struct task *task;

    task = task_create(TASK_WORK, FLOOR_TILES_LEVEL, chamber_floor_tiles_step,
                       "ObjectFloorExec");
    task->draw = chamber_floor_tiles_draw;
    task->destructor = chamber_floor_tiles_destroy;
    return task;
}

static void chamber_floor_tiles_destroy(struct task *task)
{
}

static void chamber_floor_tiles_step(struct task *task)
{
}

static void chamber_floor_tiles_draw(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = task->work;
    int row;
    int column;
    int step;
    Float light;
    Uint32 color;

    draw_control_set(DRAW_CONTROL_SHADOW);
    if (game_state.land_block_mask[0] & HALL_BLOCKS) {
        for (row = 0; row < ROWS; row++) {
            for (column = 0; column < COLUMNS; column++) {
                step =
                    ((chamber_floor_tiles_phases[row][column] * PHASE_STEPS) +
                     game_state.steps) %
                    CYCLE_STEPS;
                if (step >= LIT_STEPS) {
                    step = 0;
                }
                if (step < RISE_STEPS) {
                    light = step * RISE;
                } else {
                    light = ((LIT_STEPS - 1) - step) * FALL;
                }
                if (light < DIMMEST) {
                    light = DIMMEST;
                }
                color = whiten_color(chamber_floor_tiles_color, light);
                njPushMatrixEx();
                njTranslate(NULL, (column * COLUMN_PITCH) + HALL_FLOOR_X,
                            HALL_FLOOR_Y, (row * ROW_PITCH) + HALL_FLOOR_Z);
                njSetTexture(chamber_floor_tiles_texlists
                                 [chamber_floor_tiles_hall_kinds[row][column]]);
                model_draw_tinted(
                    chamber_floor_tiles_models
                        [chamber_floor_tiles_hall_kinds[row][column]],
                    color);
                njPopMatrixEx();
            }
        }
    }
    if (game_state.land_block_mask[0] & FAR_EAST_BLOCKS) {
        for (row = 0; row < ROWS; row++) {
            for (column = 0; column < COLUMNS; column++) {
                step =
                    ((chamber_floor_tiles_phases[row][column] * PHASE_STEPS) +
                     game_state.steps) %
                    CYCLE_STEPS;
                if (step >= LIT_STEPS) {
                    step = 0;
                }
                if (step < RISE_STEPS) {
                    light = step * RISE;
                } else {
                    light = ((LIT_STEPS - 1) - step) * FALL;
                }
                if (light < DIMMEST) {
                    light = DIMMEST;
                }
                color = whiten_color(chamber_floor_tiles_color, light);
                njPushMatrixEx();
                njTranslate(NULL, (row * ROW_PITCH) + FAR_EAST_FLOOR_X,
                            FAR_EAST_FLOOR_Y,
                            FAR_EAST_FLOOR_Z - (column * COLUMN_PITCH));
                njRotateY(NULL, QUARTER_TURN);
                njSetTexture(
                    chamber_floor_tiles_texlists
                        [chamber_floor_tiles_far_east_kinds[row][column]]);
                model_draw_tinted(
                    chamber_floor_tiles_models
                        [chamber_floor_tiles_far_east_kinds[row][column]],
                    color);
                njPopMatrixEx();
            }
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

Uint32 chamber_floor_tiles_table_color(int index)
{
    int red;
    int green;
    int blue;

    red = (chamber_floor_tiles_table[index] & TABLE_CHANNEL_MAX)
          << TABLE_SCALE_SHIFT;
    green = ((chamber_floor_tiles_table[index] >> TABLE_GREEN_SHIFT) &
             TABLE_CHANNEL_MAX)
            << TABLE_SCALE_SHIFT;
    blue = ((chamber_floor_tiles_table[index] >> TABLE_BLUE_SHIFT) &
            TABLE_CHANNEL_MAX)
           << TABLE_SCALE_SHIFT;
    return (red << RED_SHIFT) | (green << GREEN_SHIFT) | blue | OPAQUE;
}

/* color moved part of the way to white, and made opaque. a part of 0 moves
 * it none of the way, and 1 all of it */
static Uint32 whiten_color(Uint32 color, Float part)
{
    int red;
    int green;
    int blue;

    red = (((color >> RED_SHIFT) & CHANNEL_MASK) * (1.0F - part)) +
          (part * CHANNEL_MAX);
    green = (((color >> GREEN_SHIFT) & CHANNEL_MASK) * (1.0F - part)) +
            (part * CHANNEL_MAX);
    blue = ((color & CHANNEL_MASK) * (1.0F - part)) + (part * CHANNEL_MAX);
    return (red << RED_SHIFT) | (green << GREEN_SHIFT) | blue | OPAQUE;
}
