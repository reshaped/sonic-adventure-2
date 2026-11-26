/* the crash stars, which burst from a player, and the confu stars, which
 * circle a confused one */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "effect/stars.h"
#include "object/set.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

#define STAR_LEVEL 3 /* a star's task level */

#define CRASH_STARS 6 /* the stars of a burst */
#define CRASH_STAR_STEPS 10 /* the steps a crash star flies */
#define CRASH_STAR_SPEED 2.0F /* how far a crash star flies a step */
#define CRASH_STAR_SPIN 0x1000 /* how far a crash star spins about z a step */

#define CONFU_STARS 3 /* the stars that circle a player */
/* how far out from its player's unk_218 a confu star circles, and how far
 * above it */
#define CONFU_STAR_RADIUS 3.0F
#define CONFU_STAR_HEIGHT 3.0F
#define CONFU_STAR_TURN 0x800 /* how far round a confu star goes a step */

/* one random number from 0 up to 1 picks a star's heading, tilt and color.
 * the number times STAR_HEADINGS, rounded down, is the heading's number, and
 * the number times STAR_TILT is the tilt. the star is yellow where the number
 * times STAR_COLOR_BANDS, rounded down, is odd */
#define STAR_HEADINGS 32.0F /* the headings, evenly round a turn */
/* the shift that makes a heading's number, from 0 to 31, its Angle */
#define STAR_HEADING_SHIFT 11
#define STAR_TILT 12288.0F /* a tilt's bound: 67.5 degrees, as an Angle */
/* a mask that keeps a tilt under a quarter turn, which every tilt under
 * STAR_TILT already is */
#define STAR_TILT_MASK (QUARTER_TURN - 1)
#define STAR_COLOR_BANDS 64.0F
#define STAR_YELLOW 0x1 /* the bit of a star's tick that is set for yellow */

/* a star's work:
 * - tick holds STAR_YELLOW
 * - ang.z is the turn about z that star_draw gives it
 * - a crash star's scl is its velocity, and timer the steps it has flown
 * - a confu star's ang.y is its place round its circle
 * - a confu star's submode is its player's slot */

static void star_destroy(struct task *task);
static void crash_star_step(struct task *task);
static void star_draw(struct task *task);
static void confu_star_step(struct task *task);

/* The two colored models have distinct writable chunk streams and nodes. */

static Sint16 yellow_star_polygons[31] = {
#include "effect/stars/yellow_star_polygons.inc"
};

static Sint32 yellow_star_vertices[69] = {
#include "effect/stars/yellow_star_vertices.inc"
};

static NJS_CNK_MODEL yellow_star_model = {
#include "effect/stars/yellow_star_model.inc"
};

static NJS_CNK_OBJECT yellow_star_object = {
#include "effect/stars/yellow_star_object.inc"
};

static Sint16 orange_star_polygons[31] = {
#include "effect/stars/orange_star_polygons.inc"
};

static Sint32 orange_star_vertices[69] = {
#include "effect/stars/orange_star_vertices.inc"
};

static NJS_CNK_MODEL orange_star_model = {
#include "effect/stars/orange_star_model.inc"
};

static NJS_CNK_OBJECT orange_star_object = {
#include "effect/stars/orange_star_object.inc"
};

#include "effect/stars/text.inc"

void crash_stars_burst(int player)
{
    struct task_work *from;
    struct task *task;
    struct task_work *work;
    NJS_POINT3 pos;
    NJS_VECTOR ahead;
    Float roll;
    int i;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((from = player_work[player]) == NULL) {
        return;
    }
    pos = from->collision->shapes->center;
    njPushMatrix(&_nj_unit_matrix_);
    ahead.x = CRASH_STAR_SPEED;
    /* @hack spelling: z set ahead of y, not in their order */
    ahead.z = 0.0F;
    ahead.y = 0.0F;
    for (i = 0; i < CRASH_STARS; i++) {
        task = task_create(TASK_WORK, STAR_LEVEL, crash_star_step,
                           CRASH_STAR_TASK_NAME);
        if (task == NULL) {
            break;
        }
        roll = rand() * RAND_SCALE;
        work = task->work;
        work->ang.y = (Uint32)(roll * STAR_HEADINGS) << STAR_HEADING_SHIFT;
        /* @hack spelling: the tilt stored, then negated in place, not in one
         * statement */
        work->ang.x = (Uint32)(roll * STAR_TILT) & STAR_TILT_MASK;
        work->ang.x = FULL_TURN - work->ang.x;
        work->tick = (Uint8)(roll * STAR_COLOR_BANDS) & STAR_YELLOW;
        work->pos = pos;
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y);
        njCalcVector(NULL, &ahead, &work->scl);
        /* @bug logic: each star after the first turns from the caller's
         * matrix, where the first turns from the unit matrix the push loaded.
         * matrix_load_parent (JSR at 0x8c0a11c0) loads XMTRX with the matrix
         * below the current one, the caller's, which the push at 0x8c0a10fc
         * saved. while the players step, the caller's matrix is camera 0's
         * view. the camera's task steps at level 0 and ends in njSetCamera
         * (JSR at 0x8c0aae9a). the players' tasks step after it, at level 1,
         * as Tails' does (MOV #1,R5 at 0x8c0878b8) */
        matrix_load_parent();
        task->draw = star_draw;
        task->destructor = star_destroy;
    }
    njPopMatrixEx();
}

static void star_destroy(struct task *task)
{
}

static void crash_star_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        if (work->timer++ >= CRASH_STAR_STEPS) {
            task_kill(task);
            return;
        }
        work->pos.x += work->scl.x;
        work->pos.y += work->scl.y;
        work->pos.z += work->scl.z;
        work->ang.z += CRASH_STAR_SPIN;
    }
}

static void star_draw(struct task *task)
{
    struct task_work *work = task->work;

    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njRotateZ(NULL, work->ang.z);
    if (work->tick & STAR_YELLOW) {
        njCnkSimpleDrawObject(&yellow_star_object);
    } else {
        njCnkSimpleDrawObject(&orange_star_object);
    }
    njPopMatrixEx();
    njFogEnable();
}

static void confu_star_step(struct task *task)
{
    struct task_work *work;
    struct player *player;
    NJS_POINT3 offset;
    int slot;

    work = task->work;
    /* @hack spelling: slot, a local for the work's submode, where the
     * submode read in place would do */
    slot = work->submode;
    player = player_records[slot];
    if (player == NULL || player->confused_steps <= 0) {
        task_kill(task);
        return;
    }
    offset.x = CONFU_STAR_RADIUS;
    offset.y = CONFU_STAR_HEIGHT;
    offset.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&player->unk_218);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &work->pos);
    njPopMatrixEx();
    work->ang.y += CONFU_STAR_TURN;
    /* a star draws once its first step has placed it */
    if (!task->draw) {
        task->draw = star_draw;
    }
}

void confu_stars_start(int player)
{
    struct task *task;
    struct task_work *work;
    Float roll;
    int i;

    for (i = 0; i < CONFU_STARS; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((task = task_create(TASK_WORK, STAR_LEVEL, confu_star_step,
                                CONFU_STAR_TASK_NAME)) == NULL) {
            return;
        }
        roll = rand() * RAND_SCALE;
        work = task->work;
        work->ang.y = (Uint32)(roll * STAR_HEADINGS) << STAR_HEADING_SHIFT;
        work->ang.z = (Uint32)(roll * STAR_TILT) & STAR_TILT_MASK;
        work->tick = (Uint8)(roll * STAR_COLOR_BANDS) & STAR_YELLOW;
        work->submode = player;
        task->destructor = star_destroy;
    }
}
