/* the light switch, LIGHT SW in a stage's object list. its volume gives a
 * player inside it the light the switch names */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/light_switch.h"
#include "system/game_state.h"
#include "system/task.h"

/* the low byte of ang.y holds the type, and of ang.z the shape, in its low
 * two bits. the rest of each turns the volume */
#define SETTING_BYTE 0xff
#define SHAPE_BITS 0x3
#define SHAPES (SHAPE_BITS + 1)

/* where light_switch_init keeps the switch's light, type and shape in the
 * work */
#define LIGHT(work) ((work)->tick)
#define TYPE(work) ((work)->submode)
#define SHAPE(work) ((work)->timer)

/* a switch's record, in task->extra. only LIGHT_SWITCH_AREA and
 * LIGHT_SWITCH_AREADEF use the record */
struct light_switch_state {
    Sint8 inside[GAME_PLAYERS]; /* TRUE from a player's entering to leaving */
    Sint8 light_before[GAME_PLAYERS]; /* each player's light before entering */
};

static void light_switch_draw(struct task *task);
static void light_switch_destroy(struct task *task);
static Bool light_switch_contains(struct task_work *work, int player);
static void light_switch_step(struct task *task);

void light_switch_init(struct task *task)
{
    struct task_work *work = task->work;
    struct light_switch_state *state;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((state = syCalloc(1, sizeof(struct light_switch_state))) == NULL) {
        return;
    }
    task->extra.p = state;
    LIGHT(work) = work->ang.x;
    TYPE(work) = work->ang.y;
    if (TYPE(work) >= LIGHT_SWITCH_TYPES) {
        TYPE(work) = LIGHT_SWITCH_AREA;
    }
    if (TYPE(work) < 0) {
        TYPE(work) = LIGHT_SWITCH_AREA;
    }
    SHAPE(work) = work->ang.z & SHAPE_BITS;
    work->ang.x = 0;
    /* @hack spelling: the two masks written out, where &= would do */
    work->ang.y = work->ang.y & ~SETTING_BYTE;
    work->ang.z = work->ang.z & ~SETTING_BYTE;
    /* @hack redundant: both players' flags cleared in one assignment, though
     * syCalloc cleared the record */
    state->inside[0] = state->inside[1] = FALSE;
    task->step = light_switch_step;
    task->draw = light_switch_draw;
    task->destructor = light_switch_destroy;
}

static void light_switch_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 corner[2];

    /* @hack redundant: a return before a body no path reaches, where an empty
     * function would do. SHC writes none of the body's code, but keeps its
     * strings, and the body's locals size the stack frame. the statements
     * after the strings stand in for a body the image doesn't show, and make
     * the same frame */
    if (1) {
        return;
    }
    (void)"<- LIGHT:";
    (void)"<- ANG Y & TYPE:";
    (void)"<- ANG Z & SHAPE:";
    (void)"<- SCL X";
    (void)"<- SCL Y";
    (void)"<- SCL Z";
    (void)"<- TYPE:";
    (void)"<- SHAPE:";
    (void)"<- ANG X & LIGHT:";
    corner[0].x = work->pos.x - work->scl.x;
    corner[0].y = work->pos.y - work->scl.y;
    corner[0].z = work->pos.z - work->scl.z;
    corner[1].x = work->pos.x + work->scl.x;
    corner[1].y = work->pos.y + work->scl.y;
    corner[1].z = work->pos.z + work->scl.z;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&corner[0]);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&corner[1]);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    njPopMatrixEx();
}

static void light_switch_destroy(struct task *task)
{
}

/* whether player's first collision shape is in the switch's volume. without
 * the player, its collision record or the shape, the result is FALSE. the
 * shape is a ball of its radius about its center. a sphere holds a ball that
 * touches it. a cube holds a ball whose center lies within its half size plus
 * the radius on each axis. a cylinder holds a ball that lies wholly within
 * scl.x of its axis. along the axis, the center must lie within scl.y plus
 * the shape's size_y of pos */
static Bool light_switch_contains(struct task_work *work, int player)
{
    /* @hack spelling: a local for the player's work, where
     * player_work[player] read in place would do */
    struct task_work *player_task_work = player_work[player];
    struct collision_work *collision;
    struct collision_shape *shape;
    Float reach; /* from pos to the volume's farthest point */
    Float off_axis;
    NJS_POINT3 point;

    if (player_task_work == NULL) {
        return FALSE;
    }
    /* @hack spelling: the assignments inside one test, where a statement and
     * a test for each would do */
    if ((collision = player_task_work->collision) == NULL ||
        (shape = collision->shapes) == NULL) {
        return FALSE;
    }
    switch (SHAPE(work)) {
    case LIGHT_SWITCH_CUBE:
        reach = njScalor(&work->scl);
        break;
    case LIGHT_SWITCH_SPHERE:
        reach = work->scl.x;
        break;
    case LIGHT_SWITCH_CYLINDER:
        reach =
            njSqrt((work->scl.x * work->scl.x) + (work->scl.y * work->scl.y));
        break;
    default:
        return FALSE;
    }
    if (njDistanceP2P(&shape->center, &work->pos) > reach + shape->radius) {
        return FALSE;
    }
    switch (SHAPE(work)) {
    case LIGHT_SWITCH_CUBE:
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njInvertMatrix(NULL);
        njCalcPoint(NULL, &shape->center, &point);
        njPopMatrixEx();
        if (point.x > work->scl.x + shape->radius ||
            point.x < -work->scl.x - shape->radius ||
            point.y > work->scl.y + shape->radius ||
            point.y < -work->scl.y - shape->radius ||
            point.z > work->scl.z + shape->radius ||
            point.z < -work->scl.z - shape->radius) {
            return FALSE;
        }
        break;
    case LIGHT_SWITCH_SPHERE:
        break;
    case LIGHT_SWITCH_CYLINDER:
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njRotateZ(NULL, work->ang.z);
        njInvertMatrix(NULL);
        njCalcPoint(NULL, &shape->center, &point);
        njPopMatrixEx();
        if (point.y > work->scl.y + shape->size_y ||
            point.y < -work->scl.y - shape->size_y) {
            return FALSE;
        }
        off_axis = njSqrt((point.x * point.x) + (point.z * point.z));
        if (off_axis + shape->radius > work->scl.x) {
            return FALSE;
        }
        break;
    /* @hack redundant: a default no path reaches: the first switch has
     * returned for every other shape */
    default:
        return FALSE;
    }
    return TRUE;
}

static void light_switch_step(struct task *task)
{
    struct task_work *work = task->work;
    struct light_switch_state *state = task->extra.p;
    int player;
    /* @hack spelling: left, a local the test of the call sets and the next
     * test compares with TRUE, where the call tested in place would do */
    Bool left;

    if (placement_try_leave(task)) {
        return;
    }
    switch (TYPE(work)) {
    case LIGHT_SWITCH_AREA:
    case LIGHT_SWITCH_AREADEF:
        for (player = 0; player < GAME_PLAYERS; player++) {
            /* @hack spelling: the flag compared with TRUE, where
             * state->inside[player] alone would do */
            if (state->inside[player] == TRUE) {
                left = FALSE;
                if (!light_switch_contains(work, player)) {
                    left = TRUE;
                }
                if (left == TRUE) {
                    state->inside[player] = FALSE;
                    if (TYPE(work) == LIGHT_SWITCH_AREADEF) {
                        light_select(light_camera, player);
                    } else {
                        light_select(state->light_before[player], player);
                    }
                }
            } else if (light_switch_contains(work, player)) {
                state->light_before[player] = light_selected[player];
                light_select(LIGHT(work), player);
                state->inside[player] = TRUE;
            }
        }
        break;
    case LIGHT_SWITCH_SWITCH:
        /* @hack spelling: the two players written out, where a loop would
         * do */
        if (light_switch_contains(work, 0)) {
            light_select(LIGHT(work), 0);
        }
        if (light_switch_contains(work, 1)) {
            light_select(LIGHT(work), 1);
        }
        break;
    }
}

/* the types' names, by light_switch_type */
static const char *light_switch_type_names[LIGHT_SWITCH_TYPES] = {
    "AREA",
    "SWITCH",
    "AREADEF",
};

/* the shapes' names, by the low two bits of ang.z */
static const char *light_switch_shape_names[SHAPES] = {
    "CUBE",
    "SPHERE",
    "CYLINDER",
    "OUT OF ORDER",
};

static Sint8 dat_light_switch_0[SHAPES] = {1, 4, 2, 0};
