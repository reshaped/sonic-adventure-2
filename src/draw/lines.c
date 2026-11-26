/* lines and points, drawn at once or queued in batches that tasks draw, and
 * the constant material */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/lines.h"
#include "player/player.h"
#include "system/task.h"

/* the batches, and the most points one holds */
#define BATCHES 8
#define BATCH_POINTS_MAX 512

/* a batch's words in line_batches: the count of its points, its kind and its
 * color, then its points. a point takes POINT_WORDS, its x, y and z */
enum { BATCH_COUNT, BATCH_KIND, BATCH_COLOR, BATCH_POINTS };
#define POINT_WORDS 3
#define BATCH_WORDS (BATCH_POINTS + (BATCH_POINTS_MAX * POINT_WORDS))
/* a point's half side on the screen, as njPtclDrawPolygon takes it */
#define POINT_HALF_SIDE 1.0F
/* a line's half width in pixels, as njDrawLineExStartOpaque and
 * njDrawLineExStartTrans take it */
#define LINE_HALF_WIDTH 1.0F
/* a line's points, which a queued line adds to its batch's count */
#define LINE_POINTS 2
/* the task level of a batch's task */
#define BATCH_LEVEL 5
/* line_queue's free batch where none is free */
#define NO_BATCH (-1)
/* the colors line_draw and line_draw_points draw from and to: white, and
 * pale blue, 0x8888bb, each opaque */
#define CHANNEL_MAX 0xff
#define PALE_RED 0x88
#define PALE_GREEN 0x88
#define PALE_BLUE 0xbb
/* the gray constant_material_shade gives each of red, green and blue near a
 * player */
#define SHADE_GRAY 0.5F
/* the range constant_material_shade gives player_near */
#define SHADE_RANGE 20.0F

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set the constant material where mask holds the bit of player 0's
 * character, 1 << player_character(0). its alpha is 0. its red, green and
 * blue are 0.5 where player_near finds a player within SHADE_RANGE of work's
 * pos. they are 0 elsewhere */
void constant_material_shade(struct task_work *work, Sint16 mask);
/* NOLINTEND(misc-use-internal-linkage) */

/* each batch's task, or null for a free batch. a task keeps its batch's
 * index in unk_3c. the initializer keeps the table in D, where the image
 * holds it */
static struct task *line_batch_tasks[BATCHES] = {NULL};
/* the batches, BATCH_WORDS each, or null until line_batches_alloc. the
 * initializer keeps line_batches in D, where the image holds it */
/* @hack type: the batches as one array of words, indexed by a batch's first
 * word plus a word's offset, where an array of structs, one per batch, would
 * do */
static Uint32 *line_batches = NULL;

void constant_material_set(Float alpha, Float red, Float green, Float blue)
{
    NJS_ARGB material;

    material.a = alpha;
    material.r = red;
    material.g = green;
    material.b = blue;
    njSetConstantMaterial(&material);
}

void line_draw(Float x1, Float y1, Float z1, Float x2, Float y2, Float z2)
{
    NJS_POINT3COL line;
    NJS_POINT3 points[2];
    NJS_COLOR colors[2];

    points[0].x = x1;
    points[0].y = y1;
    points[0].z = z1;
    points[1].x = x2;
    points[1].y = y2;
    points[1].z = z2;
    colors[0].argb.a = CHANNEL_MAX;
    colors[0].argb.r = CHANNEL_MAX;
    colors[0].argb.g = CHANNEL_MAX;
    colors[0].argb.b = CHANNEL_MAX;
    colors[1].argb.a = CHANNEL_MAX;
    colors[1].argb.r = PALE_RED;
    colors[1].argb.g = PALE_GREEN;
    colors[1].argb.b = PALE_BLUE;
    line.p = points;
    line.col = colors;
    line.tex = NULL;
    line.num = 2;
    njDrawLine3D(&line, 1, 0);
}

void line_draw_colored(Float x1, Float y1, Float z1, NJS_COLOR *color1,
                       Float x2, Float y2, Float z2, NJS_COLOR *color2)
{
    NJS_POINT3COL line;
    NJS_POINT3 points[2];
    NJS_COLOR colors[2];

    points[0].x = x1;
    points[0].y = y1;
    points[0].z = z1;
    points[1].x = x2;
    points[1].y = y2;
    points[1].z = z2;
    colors[0] = *color1;
    colors[1] = *color2;
    line.p = points;
    line.col = colors;
    line.tex = NULL;
    line.num = 2;
    njDrawLine3D(&line, 1, 0);
}

void line_draw_points(NJS_POINT3 *from, NJS_POINT3 *to)
{
    NJS_POINT3COL line;
    NJS_POINT3 points[2];
    NJS_COLOR colors[2];

    /* @hack spelling: the points copied member by member, where points[0] =
     * *from and points[1] = *to would do */
    points[0].x = from->x;
    points[0].y = from->y;
    points[0].z = from->z;
    points[1].x = to->x;
    points[1].y = to->y;
    points[1].z = to->z;
    colors[0].argb.a = CHANNEL_MAX;
    colors[0].argb.r = CHANNEL_MAX;
    colors[0].argb.g = CHANNEL_MAX;
    colors[0].argb.b = CHANNEL_MAX;
    colors[1].argb.a = CHANNEL_MAX;
    colors[1].argb.r = PALE_RED;
    colors[1].argb.g = PALE_GREEN;
    colors[1].argb.b = PALE_BLUE;
    line.p = points;
    line.col = colors;
    line.tex = NULL;
    line.num = 2;
    njDrawLine3D(&line, 1, 0);
}

void constant_material_shade(struct task_work *work, Sint16 mask)
{
    NJS_ARGB material;
    /* @hack spelling: bit, a local for the shift, where the shift inside the
     * test would do while player_character(0) is below 16 */
    Sint16 bit = 1 << player_character(0);

    if (mask & bit) {
        if (player_near(&work->pos, SHADE_RANGE)) {
            material.a = 0.0F;
            material.r = SHADE_GRAY;
            material.g = SHADE_GRAY;
            material.b = SHADE_GRAY;
        } else {
            material.a = 0.0F;
            material.r = 0.0F;
            material.g = 0.0F;
            material.b = 0.0F;
        }
        njSetConstantMaterial(&material);
    }
}

void line_batches_alloc(void)
{
    if (line_batches == NULL) {
        line_batches = syMalloc(sizeof(*line_batches) * BATCH_WORDS * BATCHES);
    }
}

void line_batches_free(void)
{
    if (line_batches != NULL) {
        syFree(line_batches);
        line_batches = NULL;
    }
}

/* draw the lines of task's batch. the draw for the last of the
 * camera_active_count cameras empties it */
static void line_batch_draw_lines(struct task *task)
{
    Uint32 first;
    Sint32 kind;
    Uint32 count;
    Uint32 color;

    /* @hack spelling: first, set inside the subscript, where a statement of
     * its own would do */
    kind = line_batches[(first = task->unk_3c.l * BATCH_WORDS) + BATCH_KIND];
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((count = line_batches[first + BATCH_COUNT]) == 0) {
        return;
    }
    if (count <= BATCH_POINTS_MAX) {
        njFogDisable();
        /* @hack spelling: the batch's first word computed again from
         * task->unk_3c, here and at each use below, where first would do */
        color = line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_COLOR];
        if (kind == LINE_TRANSLUCENT) {
            njDrawLineExStartTrans(LINE_HALF_WIDTH, color);
        } else if (kind == LINE_ADDITIVE) {
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            njDrawLineExStartTrans(LINE_HALF_WIDTH, color);
        } else {
            njDrawLineExStartOpaque(LINE_HALF_WIDTH, color);
        }
        njDrawLine3DExSetList(
            (NJS_POINT3
                 *)&line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_POINTS],
            count);
        njDrawLineExEnd();
        if (kind == LINE_ADDITIVE) {
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
        }
        njFogEnable();
    }
    if (camera_current_index >= camera_active_count - 1) {
        line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_COUNT] = 0;
    }
}

/* draw the points of task's batch. the draw for the last of the
 * camera_active_count cameras empties it */
static void line_batch_draw_points(struct task *task)
{
    Uint32 count;
    NJS_POINT3 *points;
    Uint32 first;
    Sint32 kind;
    Uint32 color;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    /* @hack spelling: the batch's first word computed from task->unk_3c at
     * each use but kind's and color's, where first set at the top would do */
    if ((count = line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_COUNT]) ==
        0) {
        return;
    }
    if (count <= BATCH_POINTS_MAX) {
        points = (NJS_POINT3 *)&line_batches[(task->unk_3c.l * BATCH_WORDS) +
                                             BATCH_POINTS];
        njFogDisable();
        /* @hack spelling: first, set inside the subscript, where a statement
         * of its own would do */
        kind =
            line_batches[(first = task->unk_3c.l * BATCH_WORDS) + BATCH_KIND];
        color = line_batches[first + BATCH_COLOR];
        if (kind == POINT_TRANSLUCENT) {
            njPtclPolygonStartTrans(color);
        } else if (kind == POINT_ADDITIVE) {
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            njPtclPolygonStartTrans(color);
        } else {
            njPtclPolygonStart(color);
        }
        njPtclDrawPolygon(points, count, POINT_HALF_SIDE);
        njPtclPolygonEnd();
        /* @hack redundant: the kind read again, not tested as kind */
        if (line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_KIND] ==
            POINT_ADDITIVE) {
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
        }
        njFogEnable();
    }
    if (camera_current_index >= camera_active_count - 1) {
        line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_COUNT] = 0;
    }
}

static void point_draw_now(NJS_POINT3 *points, NJS_COLOR *colors,
                           enum line_kind kind)
{
    njFogDisable();
    if (kind == POINT_TRANSLUCENT) {
        njPtclPolygonStartTrans(colors->color);
    } else if (kind == POINT_ADDITIVE) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njPtclPolygonStartTrans(colors->color);
    } else {
        njPtclPolygonStart(colors->color);
    }
    njPtclDrawPolygon(points, 1, POINT_HALF_SIDE);
    njPtclPolygonEnd();
    if (kind == POINT_ADDITIVE) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njFogEnable();
}

static void line_batch_destroy(struct task *task)
{
    line_batch_tasks[task->unk_3c.l] = NULL;
}

static void line_batch_step(struct task *task)
{
    if (line_batches[(task->unk_3c.l * BATCH_WORDS) + BATCH_COUNT] == 0) {
        task_destroy(task);
    }
}

void line_draw_now(NJS_POINT3 *points, NJS_COLOR *colors, enum line_kind kind)
{
    NJS_POINT3COL line;
    Uint32 attr;

    line.p = points;
    line.col = colors;
    line.tex = NULL;
    line.num = 2;
    njFogDisable();
    if (kind == LINE_TRANSLUCENT) {
        attr = NJD_TRANSPARENT;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    } else if (kind == LINE_ADDITIVE) {
        attr = NJD_TRANSPARENT;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    } else {
        attr = 0;
    }
    njDrawLine3D(&line, 1, attr);
    if (kind == LINE_ADDITIVE) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njFogEnable();
}

void line_queue(NJS_POINT3 *points, NJS_COLOR *colors, enum line_kind kind)
{
    NJS_POINT3 *point;
    struct task *task;
    struct task **tasks;
    int batch;
    int free_batch;
    Uint32 first;

    /* @hack spelling: the tasks walked through a pointer, not indexed */
    for (batch = 0, free_batch = NO_BATCH, tasks = line_batch_tasks;
         batch < BATCHES; batch++) {
        if (*tasks++ == NULL) {
            if (free_batch == NO_BATCH) {
                free_batch = batch;
            }
            continue;
        }
        /* @hack type: BATCH_KIND, BATCH_COUNT and BATCH_COLOR cast to Uint32
         * in this subscript and those below, where the plain constants would
         * do */
        if (line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_KIND] !=
            (Uint32)kind) {
            continue;
        }
        /* @hack redundant: the test of kind, whose branches test the count
         * alike, where one test would do */
        if (kind >= POINT_OPAQUE) {
            if (line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COUNT] >=
                BATCH_POINTS_MAX) {
                continue;
            }
        } else {
            if (line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COUNT] >=
                BATCH_POINTS_MAX) {
                continue;
            }
        }
        if (line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COLOR] !=
            colors->color) {
            continue;
        }
        break;
    }
    if (batch == BATCHES && free_batch != NO_BATCH) {
        /* @hack spelling: the assignments inside the test, where the
         * statements task = task_create(...) and line_batch_tasks[free_batch] =
         * task would do */
        if ((task = line_batch_tasks[free_batch] = task_create(
                 0, BATCH_LEVEL, line_batch_step, "DrawLine3DExec")) != NULL) {
            if (kind >= POINT_OPAQUE) {
                task->draw = line_batch_draw_points;
            } else {
                task->draw = line_batch_draw_lines;
            }
            task->destructor = line_batch_destroy;
            batch = free_batch;
            first = batch * BATCH_WORDS;
            line_batches[first + BATCH_COUNT] = 0;
            line_batches[first + BATCH_KIND] = kind;
            line_batches[first + BATCH_COLOR] = colors->color;
            task->unk_3c.l = free_batch;
        }
    }
    if (batch != BATCHES) {
        if (kind >= POINT_OPAQUE) {
            point = (NJS_POINT3 *)&line_batches
                [(batch * BATCH_WORDS) + BATCH_POINTS +
                 (line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COUNT] *
                  POINT_WORDS)];
            *point = *points;
            line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COUNT] += 1;
        } else {
            point = (NJS_POINT3 *)&line_batches
                [(batch * BATCH_WORDS) + BATCH_POINTS +
                 (line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COUNT] *
                  POINT_WORDS)];
            *point++ = *points++;
            *point = *points;
            line_batches[(batch * BATCH_WORDS) + (Uint32)BATCH_COUNT] +=
                LINE_POINTS;
        }
    } else if (kind >= POINT_OPAQUE) {
        point_draw_now(points, colors, kind);
    } else {
        line_draw_now(points, colors, kind);
    }
}
