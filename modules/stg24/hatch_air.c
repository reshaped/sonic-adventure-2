#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "stg24/hatch_air.h"

#include "stg24/hatch_air/text.inc"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the air's record, in task->motion. points holds each speck's place, in the
 * column's axes. timers holds the steps each has left before it starts again.
 * the column's length and the sprites' size are values in task->force and
 * task->extra, which point at nothing */
struct hatch_air {
    NJS_POINT3 *points;
    Uint16 *timers;
    int count;
};

/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define AIR(task) ((struct hatch_air *)*(void **)&(task)->motion)
/* @hack type: the length read as a Float through force's address, where
 * (task)->force.f would do */
#define COLUMN_LENGTH(task) (*(Float *)&(task)->force)
/* @hack type: the size read as a Float through extra's address, where
 * (task)->extra.f would do */
#define SPRITE_SIZE(task) (*(Float *)&(task)->extra)
/* hatch_air_place's flags. PLACE_AT_BASE puts the speck at the column's foot.
 * PLACE_ANYWHERE starts it anywhere along the column, not near its far end.
 * the work's mode holds the flags each later start takes */
#define PLACE_AT_BASE 1
#define PLACE_ANYWHERE 2
/* a speck's timer is a Uint16, and holds no more steps than this */
#define MAX_SPECK_STEPS 65000.0F
/* the draw passes over the air where sphere_in_front_within(pos, DRAW_RADIUS,
 * DRAW_REACH) is FALSE. the step stops while the draw has passed it over more
 * than SKIPPED_DRAWS times in a row */
#define SKIPPED_DRAWS 3
#define DRAW_RADIUS 50.0F
#define DRAW_REACH 300.0F
/* a pass's sprites are an opaque gray, GRAY_STEP darker than the next pass's.
 * the last pass's are LAST_GRAY, white */
#define LAST_GRAY 255
#define GRAY_STEP 32
#define OPAQUE_BLACK 0xff000000
/* the texture of engine_hatch_air_texlist the sprites take, and
 * njPtclSpriteStart's flag for the translucent list */
#define SPRITE_TEXTURE 0
#define TRANSLUCENT 1
/* a speck starts tilted from the column's axis. the tilt is up to TILT_X
 * degrees either way about x, then up to TILT_Y about y */
#define TILT_X 40.0F
#define TILT_Y 45.0F
#define AIR_LEVEL 3

static NJS_TEXNAME textures[1] = {
#include "stg24/hatch_air/textures.inc"
};
NJS_TEXLIST engine_hatch_air_texlist = {textures, 1};
/* each step scales the column by width_shrink across it, and by
 * length_shrink along it */
static Float length_shrink = 0.96F;
static Float width_shrink = 0.985F;
/* the column's turn each step, in degrees about its own axes. only turn_z
 * turns it */
static Float turn_x = -0.5F;
static Float turn_y = 0.25F;
static Float turn_z = 0.95F;
/* the draw draws the specks pass_count times. each pass's sprites are
 * pass_size_ratio the size of the next's. each pass after the first draws the
 * column a step further on */
static Float pass_size_ratio = 0.75F;
/* the column's foot lies base_fraction of its length from pos. a speck that
 * starts again lies up to edge_spread of the length short of the far end */
static Float base_fraction = 0.07F;
static Float edge_spread = 0.2F;
static Uint32 pass_count = 2;

static void hatch_air_step(struct task *task);
static void hatch_air_draw(struct task *task);
static void hatch_air_destroy(struct task *task);

static void hatch_air_place(NJS_POINT3 *point, Uint16 *timer, Float length,
                            Uint8 flags)
{
    Float base;
    Float distance;
    Float fraction;
    Float steps;
    Float ratio;

    base = base_fraction * length;
    if (flags & PLACE_ANYWHERE) {
        distance = base + ((Float)rand() * length * RAND_SCALE);
    } else {
        fraction =
            (edge_spread * (Float)rand() * RAND_SCALE) - edge_spread + 1.0F;
        distance = base + (fraction * length);
    }
    point->x = 0.0F;
    point->y = 0.0F;
    point->z = distance;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, RANDOM_ANGLE(TILT_X));
    njRotateY(NULL, RANDOM_ANGLE(TILT_Y));
    njCalcPoint(NULL, point, point);
    njPopMatrix(1);
    steps = logf(base / distance) / logf(length_shrink);
    if (steps > MAX_SPECK_STEPS) {
        steps = MAX_SPECK_STEPS;
    }
    if (steps < 1.0F) {
        steps = 1.0F;
    }
    *timer = (Uint16)steps;
    if (flags & PLACE_AT_BASE) {
        ratio = base / distance;
        point->x *= ratio;
        point->y *= ratio;
        point->z *= ratio;
    }
}

struct task *hatch_air_create(NJS_POINT3 *pos, struct angles *ang, Float length,
                              int count, Uint8 at_base, Float size,
                              struct task *parent)
{
    struct task *task;
    int i;

    if (parent) {
        task = task_create_child(TASK_WORK, hatch_air_step, parent);
    } else {
        task =
            task_create(TASK_WORK, AIR_LEVEL, hatch_air_step, "HatchAirExec");
    }
    if (task) {
        task->work->pos = *pos;
        task->work->ang = *ang;
        if (!parent && placement_try_leave(task)) {
            return NULL;
        }
        task->motion.p = syCalloc(1, sizeof(struct hatch_air));
        if (!task->motion.p) {
            task_destroy(task);
            return NULL;
        }
        AIR(task)->points = syCalloc(count, sizeof(NJS_POINT3));
        AIR(task)->timers = syCalloc(count, sizeof(Uint16));
        if (!AIR(task)->points || !AIR(task)->timers) {
            syFree(AIR(task)->points);
            syFree(AIR(task)->timers);
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            return NULL;
        }
        task->draw = hatch_air_draw;
        task->step = hatch_air_step;
        task->destructor = hatch_air_destroy;
        COLUMN_LENGTH(task) = length;
        SPRITE_SIZE(task) = size;
        AIR(task)->count = count;
        if (at_base) {
            at_base = PLACE_AT_BASE;
        }
        task->work->mode = at_base;
        for (i = 0; i < count; i++) {
            hatch_air_place(&AIR(task)->points[i], &AIR(task)->timers[i],
                            length, at_base | PLACE_ANYWHERE);
        }
        return task;
    }
    return NULL;
}

#pragma inline(advance_column)
static void advance_column(void)
{
    (void)turn_x;
    (void)turn_y;
    njRotateZ(NULL, (Angle)(turn_z * ANGLE_PER_DEGREE));
    njScale(NULL, width_shrink, width_shrink, length_shrink);
}

static void hatch_air_step(struct task *task)
{
    struct task_work *work = task->work;
    Uint16 *timer;
    int count;
    int i;

    if (work->tick > SKIPPED_DRAWS) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    advance_column();
    njCalcPoints(NULL, AIR(task)->points, AIR(task)->points, AIR(task)->count);
    njPopMatrix(1);
    count = AIR(task)->count;
    timer = AIR(task)->timers;
    for (i = 0; i < count; i++, timer++) {
        if ((*timer)-- == 0) {
            hatch_air_place(AIR(task)->points + i, timer, COLUMN_LENGTH(task),
                            task->work->mode);
        }
    }
}

static void hatch_air_draw(struct task *task)
{
    struct task_work *work = task->work;
    int i;
    Float size;
    Uint32 gray;
    Uint32 color;

    if (!sphere_in_front_within(&work->pos, DRAW_RADIUS, DRAW_REACH)) {
        work->tick++;
        return;
    }
    work->tick = 0;
    njFogDisable();
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateEx(&work->ang.x, 0);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&engine_hatch_air_texlist);
    for (i = 0; (Uint32)i < pass_count; i++) {
        if (i > 0) {
            advance_column();
        }
        size = SPRITE_SIZE(task) * powf(pass_size_ratio, pass_count - i - 1);
        gray = LAST_GRAY - ((pass_count - i - 1) * GRAY_STEP);
        color = OPAQUE_BLACK + (gray << 16) + (gray << 8) + gray;
        njPtclSpriteStart(SPRITE_TEXTURE, color, TRANSLUCENT);
        njPtclDrawSprite(AIR(task)->points, AIR(task)->count, size, size);
        njPtclSpriteEnd();
    }
    njPopMatrix(1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

/* task_destroy frees task->motion, task->force and task->extra once the
 * destructor returns. force and extra hold values, not blocks of the heap, so
 * the destructor clears them */
static void hatch_air_destroy(struct task *task)
{
    syFree(AIR(task)->points);
    syFree(AIR(task)->timers);
    task->force.p = NULL;
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}
