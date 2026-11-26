/* the dragonfly, DFLY in the stage's object list. it flies a course of two
 * quarter turns, joined by a straight */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "object/set.h"
#include "stg03/dragonfly.h"
#include "stg03/dragonfly/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#define RADIUS(work) ((work)->scl.x)
#define STRAIGHT(work) ((work)->scl.y)
#define SPEED(work) ((work)->scl.z)
/* how far round its turn's center the dragonfly has flown, an Angle in the
 * work's timer. the first turn runs to QUARTER_TURN, and the second on to
 * HALF_TURN */
#define TURNED(work) ((work)->timer)

/* a circle's length over its radius, as the course rounds it */
#define CIRCLE 6.28F
/* the turn a step gives a dragonfly that flies SPEED along a circle of
 * RADIUS */
#define TURN_STEP(work)                                                        \
    (int)(FULL_TURN * (SPEED(work) / (RADIUS(work) * CIRCLE)))

/* the body pitches and rolls by up to PITCH and ROLL. the wings beat by up to
 * FLAP either way. each sways on a sine of game_state.steps times its _RATE.
 * the body bobs by up to BOB, a quarter turn ahead of the pitch */
#define BOB 15.0F
#define PITCH (12.0F * ANGLE_PER_DEGREE)
#define ROLL (15.0F * ANGLE_PER_DEGREE)
#define FLAP (30.0F * ANGLE_PER_DEGREE)
#define PITCH_RATE 0x100
#define ROLL_RATE 0xe0
#define FLAP_RATE 0x1e00

/* a dragonfly's record, at task->motion */
struct dragonfly {
    /* the first turn's center: the work's pos as the init found it. each step
     * then sets pos to where the dragonfly flies */
    NJS_POINT3 center;
    /* how far down the straight the center of the dragonfly's turn has moved.
     * it is 0 through the first turn, and STRAIGHT through the second */
    Float along;
};

#define DRAGONFLY(task) ((struct dragonfly *)(task)->motion.p)

/* The dragonfly body and its two articulated wings. */
static NJS_TEXNAME dragonfly_textures[1] = {
#include "stg03/dragonfly/dragonfly_textures.inc"
};

NJS_TEXLIST dragonfly_texlist = {
#include "stg03/dragonfly/dragonfly_texlist.inc"
};

static Sint16 dragonfly_object_node_2_polygons[51] = {
#include "stg03/dragonfly/dragonfly_object_node_2_polygons.inc"
};

static Sint32 dragonfly_object_node_2_vertices[51] = {
#include "stg03/dragonfly/dragonfly_object_node_2_vertices.inc"
};

static NJS_CNK_MODEL dragonfly_object_node_2_model = {
#include "stg03/dragonfly/dragonfly_object_node_2_model.inc"
};

static NJS_CNK_OBJECT dragonfly_object_node_2 = {
#include "stg03/dragonfly/dragonfly_object_node_2.inc"
};

static Sint16 dragonfly_object_node_1_polygons[51] = {
#include "stg03/dragonfly/dragonfly_object_node_1_polygons.inc"
};

static Sint32 dragonfly_object_node_1_vertices[51] = {
#include "stg03/dragonfly/dragonfly_object_node_1_vertices.inc"
};

static NJS_CNK_MODEL dragonfly_object_node_1_model = {
#include "stg03/dragonfly/dragonfly_object_node_1_model.inc"
};

static NJS_CNK_OBJECT dragonfly_object_node_1 = {
#include "stg03/dragonfly/dragonfly_object_node_1.inc"
};

static Sint16 dragonfly_polygons[279] = {
#include "stg03/dragonfly/dragonfly_polygons.inc"
};

static Sint32 dragonfly_vertices[211] = {
#include "stg03/dragonfly/dragonfly_vertices.inc"
};

static NJS_CNK_MODEL dragonfly_model = {
#include "stg03/dragonfly/dragonfly_model.inc"
};

NJS_CNK_OBJECT dragonfly_object = {
#include "stg03/dragonfly/dragonfly_object.inc"
};

NJS_TEXLIST *dragonfly_texlists[2] = {
#include "stg03/dragonfly/dragonfly_texlists.inc"
};

static void dragonfly_destroy(struct task *task);
static void dragonfly_step(struct task *task);
static void dragonfly_draw(struct task *task);

void dragonfly_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        task->motion.p = syCalloc(1, sizeof(struct dragonfly));
        if (task->motion.p != NULL) {
            task->draw = dragonfly_draw;
            task->step = dragonfly_step;
            task->destructor = dragonfly_destroy;
            DRAGONFLY(task)->center = work->pos;
        }
    }
}

static void dragonfly_destroy(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void dragonfly_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 arm;
    int turn;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: the else after the return */
    if (game_state.time_stopped) {
        return;
    } else if (DRAGONFLY(task)->along == 0.0F) {
        turn = TURN_STEP(work);
        TURNED(work) += turn;
        if (TURNED(work) > QUARTER_TURN) {
            TURNED(work) = QUARTER_TURN;
            DRAGONFLY(task)->along = SPEED(work);
        }
    } else {
        DRAGONFLY(task)->along += SPEED(work);
        if (DRAGONFLY(task)->along >= STRAIGHT(work)) {
            turn = TURN_STEP(work);
            DRAGONFLY(task)->along = STRAIGHT(work);
            /* @hack redundant: the test of TURNED below 0, which never holds
             * for the unsigned timer */
            if (TURNED(work) < 0 || TURNED(work) > HALF_TURN - turn) {
                TURNED(work) = 0;
                DRAGONFLY(task)->along = 0.0F;
            } else {
                TURNED(work) += turn;
            }
        }
    }
    arm.x = RADIUS(work);
    arm.y = 0.0F;
    arm.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&DRAGONFLY(task)->center);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F, 0.0F, DRAGONFLY(task)->along);
    njRotateY(NULL, TURNED(work) + QUARTER_TURN);
    njCalcPoint(NULL, &arm, &work->pos);
    njPopMatrixEx();
}

static void dragonfly_draw(struct task *task)
{
    struct task_work *work;
    NJS_CNK_OBJECT *object;
    Angle phase;
    Angle pitch;
    Angle roll;
    Angle flap;

    work = task->work;
    /* @hack spelling: the object and the pitch's phase set in one statement,
     * where the object's own statement and the phase in njSin's call would
     * do */
    object = &dragonfly_object, phase = game_state.steps * PITCH_RATE;
    pitch = (Angle)(njSin(phase) * PITCH);
    /* @hack redundant: a block no path reaches, kept for its strings and for
     * the frame the draw keeps. SHC still places the strings in this
     * source's constants, for the SET editor labels. the
     * image holds none of the block's code: what follows the strings only
     * makes the same frame */
    if (0) {
        NJS_POINT3 at;

        (void)DRAGONFLY_EDITOR_ROT_Y;
        (void)DRAGONFLY_EDITOR_LEN_R;
        (void)DRAGONFLY_EDITOR_LEN_L;
        (void)DRAGONFLY_EDITOR_SPD;
        at = task->work->pos;
        object = (NJS_CNK_OBJECT *)&at;
    }
    roll = (Angle)(njSin(game_state.steps * ROLL_RATE) * ROLL);
    njSetTexture(&dragonfly_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + TURNED(work) - QUARTER_TURN);
    njTranslate(
        NULL, object->pos[0],
        object->pos[1] +
            (njSin((game_state.steps * PITCH_RATE) + QUARTER_TURN) * BOB),
        object->pos[2]);
    njRotateX(NULL, pitch);
    njRotateZ(NULL, roll);
    njTranslate(NULL, -object->pos[0], -object->pos[1], -object->pos[2]);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    flap = (Angle)(njSin(game_state.steps * FLAP_RATE) * FLAP);
    njPushMatrixEx();
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateZ(NULL, flap);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
    object = object->sibling;
    njTranslateEx((NJS_VECTOR *)object->pos);
    njRotateZ(NULL, -flap);
    njRotateY(NULL, HALF_TURN);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}
