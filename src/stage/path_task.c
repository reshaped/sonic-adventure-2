/* the tasks of a stage's paths: a path's draw, and the steps that put
 * players on a path */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/polygon.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "player/player.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"

/* the modes of a path task's record */
enum path_task_mode {
    PATH_SET_UP = 0,
    PATH_RUN = 1,
    /* a rail's: it takes players from above, or from beneath where gravity
     * pulls along its up */
    PATH_RAIL_ABOVE = 1,
    PATH_RAIL_BENEATH = 2
};

/* path_draw draws while the path's first point lies within 8000 of the
 * camera. the test compares squared distances */
#define DRAW_RANGE (8000.0F * 8000.0F)
/* a triangle's line out along up grows from UP_LENGTH by UP_GROWTH a notch.
 * UP_NOTCHES gives the notches at a count of frames. a notch lasts
 * 1 << UP_NOTCH_SHIFT frames. the notches count from 0 again every UP_CYCLE
 * frames, a power of 2 */
#define UP_LENGTH 2.0F
#define UP_GROWTH 0.3F
#define UP_NOTCH_SHIFT 2
#define UP_CYCLE 64
#define UP_NOTCHES(frames) (((frames) & (UP_CYCLE - 1)) >> UP_NOTCH_SHIFT)

/* a channel phase / BLEND_ONE of the way from from to to, in 256ths.
 * DRAW_COLOR shifts each into its byte of an ARGB color */
#define BLEND_ONE 256
#define BLEND(from, to, phase, rest) (((to) * (phase)) + ((from) * (rest)))
/* the opaque color phase / BLEND_ONE of the way from (r0, g0, b0) to
 * (r1, g1, b1). rest must be BLEND_ONE - phase */
#define DRAW_OPAQUE 0xff000000
#define DRAW_COLOR(r0, g0, b0, r1, g1, b1, phase, rest)                        \
    (DRAW_OPAQUE | ((BLEND(r0, r1, phase, rest) << 8) & 0x00ff0000) |          \
     (BLEND(g0, g1, phase, rest) & 0x0000ff00) |                               \
     ((BLEND(b0, b1, phase, rest) >> 8) & 0x000000ff))

/* each step's wait, margin and reach. a step can take a player again once
 * the player's wait reaches the step's. the margin grows the step's box about
 * the path's points. the reach is how far from a point a step takes a player. a
 * rail's or a current's is how far from its path */
#define JOIN_WAIT 60
#define JOIN_MARGIN 40.0F
#define JOIN_REACH 40.0F
#define HANG_WAIT 30
/* path_hang_step's set-up starts each wait past HANG_WAIT, so either player
 * can be taken at once */
#define HANG_START_WAIT 60
#define HANG_MARGIN 20.0F
#define HANG_REACH 10.0F
#define LEDGE_WAIT 60
#define LEDGE_REACH 5.0F
#define RAIL_WAIT 12
#define RAIL_MARGIN 50.0F
#define RAIL_REACH 4.0F
#define RAIL_LOCKED_REACH 6.5F /* while the player's stick_lock isn't 0 */
/* for a player whose stick_lock is 0, on STAGE_FINAL_RUSH, STAGE_FINAL_CHASE
 * and STAGE_BOSS_LAST1, and on STAGE_SKY_RAIL over no floor */
#define RAIL_WIDE_REACH 8.0F
#define RUN_WAIT 60
#define RUN_MARGIN 40.0F
#define RUN_REACH 40.0F
#define CURRENT_WAIT 10
#define CURRENT_MARGIN 100.0F
#define CURRENT_REACH 100.0F

/* a rail takes players from beneath where the inner product of
 * world_gravity and its up passes RAIL_FLIP. it then measures from RAIL_HEAD
 * above each player, against gravity */
#define RAIL_FLIP 0.1F
#define RAIL_HEAD 9.0F
/* how far before its end a rail takes a player whose nearest place lies at
 * or past it */
#define RAIL_END_ROOM 0.01F

/* a current pushes a player along the path and toward the path's place
 * nearest the player. where that place lies within CURRENT_NEAR, the second
 * push runs along the path too. the push falls as the player speeds up. its
 * scale is 1 / sqrt(CURRENT_SPEED_SHARE * speed + CURRENT_SPEED_FLOOR), at
 * most CURRENT_PUSH_MAX, times current_push */
#define CURRENT_NEAR 1.0F
#define CURRENT_SPEED_SHARE 0.5F
#define CURRENT_SPEED_FLOOR 0.01F
#define CURRENT_PUSH_MAX 4.0F
/* the least stick_lock a current holds a player at, off STAGE_BOSS_LAST1 and
 * STAGE_RELAY_SONIC */
#define CURRENT_STICK_LOCK 2
/* a current takes no player in PLAYER_MODE_JUMP whose jump_steps is below
 * this */
#define CURRENT_JUMP_STEPS_MIN 5

/* the triangle path_draw draws at each point of a path */
static NJS_POLYGON_VTX path_draw_triangle[3] = {
#include "stage/path_task/path_draw_triangle.inc"
};
/* the push of a current, before the player's speed scales it */
static Float current_push = 3.0F;
/* the acc a current gives a player along the path on STAGE_RELAY_SONIC */
static Float current_relay_acc = 0.05F;

struct task *path_task_create(void (*step)(struct task *task))
{
    struct task *task;
    void *record;

    task = task_create(0, 0, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((record = syCalloc(1, sizeof(struct path_task))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = record;
    }
    return task;
}

void path_draw(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path;
    struct path_point *point;
    Uint16 phase;
    Uint16 rest;
    Uint16 i;
    Uint32 path_color;
    Uint32 tip_color;
    NJS_VECTOR up;
    Float length;

    if (polygon_point_distance2(
            &((struct path_point *)ptask->path->points)->pos,
            &camera_task_work->pos) > DRAW_RANGE) {
        return;
    }
    ptask->frames++;
    phase = (Uint8)ptask->frames;
    rest = BLEND_ONE - phase;
    path_color = DRAW_COLOR(0xff, 0xff, 0xff, 0x00, 0x00, 0xff, phase, rest);
    /* @hack spelling: tip_color, a local for the color its one store could
     * compute in place */
    tip_color = DRAW_COLOR(0x00, 0xff, 0x00, 0xff, 0x00, 0xff, phase, rest);
    path_draw_triangle[0].col = path_color;
    path_draw_triangle[1].col = path_color;
    path_draw_triangle[2].col = tip_color;
    path = ptask->path;
    njPushMatrix(NULL);
    njDrawPolygon3DExStart(0);
    for (i = 0, point = path->points; i < path->count - 1; i++, point++) {
        up.x = -njSin(point->slope_z) * njCos(point->slope_x);
        up.y = njCos(point->slope_z) * njCos(point->slope_x);
        up.z = njSin(point->slope_x);
        length = UP_LENGTH + (UP_GROWTH * (Float)UP_NOTCHES(ptask->frames));
        path_draw_triangle[0].x = point->pos.x;
        path_draw_triangle[0].y = point->pos.y;
        path_draw_triangle[0].z = point->pos.z;
        path_draw_triangle[1].x = point[1].pos.x;
        path_draw_triangle[1].y = point[1].pos.y;
        path_draw_triangle[1].z = point[1].pos.z;
        path_draw_triangle[2].x = point->pos.x + (length * up.x);
        path_draw_triangle[2].y = point->pos.y + (length * up.y);
        path_draw_triangle[2].z = point->pos.z + (length * up.z);
        njDrawPolygon3DExSetData(path_draw_triangle, 3);
    }
    njDrawPolygon3DExEnd();
    njPopMatrix(1);
}

static void path_task_box(struct path_task *ptask)
{
    struct path *path = ptask->path;
    struct path_point *point = path->points;
    int i;

    ptask->low.x = ptask->high.x = point->pos.x;
    ptask->low.y = ptask->high.y = point->pos.y;
    ptask->low.z = ptask->high.z = point->pos.z;
    for (i = 0; i < path->count; point++, i++) {
        if (point->pos.x > ptask->high.x) {
            ptask->high.x = point->pos.x;
        }
        if (point->pos.y > ptask->high.y) {
            ptask->high.y = point->pos.y;
        }
        if (point->pos.z > ptask->high.z) {
            ptask->high.z = point->pos.z;
        }
        if (point->pos.x < ptask->low.x) {
            ptask->low.x = point->pos.x;
        }
        if (point->pos.y < ptask->low.y) {
            ptask->low.y = point->pos.y;
        }
        if (point->pos.z < ptask->low.z) {
            ptask->low.z = point->pos.z;
        }
    }
}

/* TRUE where the player in slot stands in ptask's box, within reach of a
 * point of its path but the last. *index takes the first such point, where
 * index isn't null. FALSE for an empty slot */
/* @hack spelling: an old-style definition, whose list names reach ahead of
 * index though each call passes index first, where a prototype in the
 * calls' order would do. SHC passes a Float in a floating-point register and
 * the ints and pointers in general ones, so either order takes the same
 * arguments */
#ifdef NONMATCHING
/* this side takes the prototype, in the calls' order */
static Bool path_task_near_point(struct path_task *ptask, int slot, int *index,
                                 Float reach)
#else
#ifdef DREAMCAST
static Bool path_task_near_point(ptask, slot, reach, index)
struct path_task *ptask;
int slot;
Float reach;
int *index;
#else
/* a build off the console, as clang-tidy's, takes the prototype too, which
 * its checks of the calls read */
static Bool path_task_near_point(struct path_task *ptask, int slot, int *index,
                                 Float reach)
#endif
#endif
{
    struct task_work *player = player_work[slot];
    struct path *path = ptask->path;
    struct path_point *point = path->points;
    NJS_VECTOR to;
    int i;

    if (player == NULL) {
        return FALSE;
    }
    if (player->pos.x > ptask->high.x || player->pos.y > ptask->high.y ||
        player->pos.z > ptask->high.z || player->pos.x < ptask->low.x ||
        player->pos.y < ptask->low.y || player->pos.z < ptask->low.z) {
        return FALSE;
    }
    for (i = 0; i < path->count - 1; i++, point++) {
        to = point->pos;
        njSubVector(&to, &player->pos);
        if (reach > njScalor(&to)) {
            break;
        }
    }
    if (i >= path->count - 1) {
        return FALSE;
    }
    if (index != NULL) {
        *index = i;
    }
    return TRUE;
}

/* TRUE where the player in slot stands in ptask's box, within reach of its
 * path. *index takes the point path_segment_at gives for the place nearest
 * the player, where index isn't null. FALSE for an empty slot */
static Bool path_task_near_path(struct path_task *ptask, int slot,
                                unsigned int *index, Float reach)
{
    struct task_work *player = player_work[slot];
    struct path *path = ptask->path;
    NJS_POINT3 nearest;
    Float along;

    if (player == NULL) {
        return FALSE;
    }
    if (player->pos.x > ptask->high.x || player->pos.y > ptask->high.y ||
        player->pos.z > ptask->high.z || player->pos.x < ptask->low.x ||
        player->pos.y < ptask->low.y || player->pos.z < ptask->low.z) {
        return FALSE;
    }
    if (path_find_nearest(path, &player->pos, &nearest, &along) > reach) {
        return FALSE;
    }
    if (index != NULL) {
        path_segment_at(path, along, index);
    }
    return TRUE;
}

void path_join_step(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path = ptask->path;
    struct task_work *player;
    /* @hack type: slot a Uint8, where the int of the other steps would do */
    Uint8 slot;

    switch (ptask->mode) {
    case PATH_SET_UP:
        ptask->held = 0;
        ptask->wait[0] = ptask->wait[1] = JOIN_WAIT;
        path_task_box(ptask);
        ptask->high.x += JOIN_MARGIN;
        ptask->high.y += JOIN_MARGIN;
        ptask->high.z += JOIN_MARGIN;
        ptask->low.x -= JOIN_MARGIN;
        ptask->low.y -= JOIN_MARGIN;
        ptask->low.z -= JOIN_MARGIN;
        ptask->mode = PATH_RUN;
        break;
    case PATH_RUN:
        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((player = player_work[slot]) == NULL) {
                continue;
            }
            if (!(ptask->held & (1 << slot))) {
                if (ptask->wait[slot] < JOIN_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                if (player_read_stick(player, player_records[slot], NULL,
                                      NULL) ||
                    player_records[slot]->stick_lock) {
                    if (path_task_near_point(ptask, slot, NULL, JOIN_REACH)) {
                        player_join_path(slot, path);
                        ptask->held |= 1 << slot;
                    }
                }
            } else if (!(player->flags & PLAYER_WORK_ON_PATH)) {
                ptask->wait[slot] = 0;
                ptask->held &= ~(1 << slot);
            }
        }
        break;
    }
}

void path_hang_step(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path = ptask->path;
    struct task_work *player;
    int point;
    int slot;

    switch (ptask->mode) {
    case PATH_SET_UP:
        ptask->held = 0;
        ptask->wait[0] = ptask->wait[1] = HANG_START_WAIT;
        path_task_box(ptask);
        ptask->high.x += HANG_MARGIN;
        ptask->high.y += HANG_MARGIN;
        ptask->high.z += HANG_MARGIN;
        ptask->low.x -= HANG_MARGIN;
        ptask->low.y -= HANG_MARGIN;
        ptask->low.z -= HANG_MARGIN;
        ptask->mode = PATH_RUN;
        break;
    case PATH_RUN:
        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            player = player_work[slot];
            if (player == NULL) {
                continue;
            }
            if (!(ptask->held & (1 << slot))) {
                if (ptask->wait[slot] < HANG_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                if (path_task_near_point(ptask, slot, &point, HANG_REACH)) {
                    player_hang_path(slot, path, point);
                    ptask->held |= 1 << slot;
                }
            } else if (!(player->flags & PLAYER_WORK_ON_PATH)) {
                ptask->wait[slot] = 0;
                ptask->held &= ~(1 << slot);
            }
        }
        break;
    }
}

void path_ledge_step(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path = ptask->path;
    struct task_work *player;
    int point;
    int slot;
    int count; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    /* @hack redundant: count, a read of the path's count that nothing
     * uses */
    count = path->count;
    /* @bug logic: never takes a player. the step has no set-up, and nothing
     * sets the record's mode to PATH_RUN, which path_task_create leaves 0
     * (CMP/EQ #1,R0 at 0x8c037e2e) */
    switch (ptask->mode) {
    case PATH_RUN:
        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            player = player_work[slot];
            if (player == NULL) {
                continue;
            }
            if (!(ptask->held & (1 << slot))) {
                if (ptask->wait[slot] < LEDGE_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                if (path_task_near_point(ptask, slot, &point, LEDGE_REACH)) {
                    player_ledge_path(slot, path, point, &ptask->ang);
                    ptask->held |= 1 << slot;
                }
            } else if (!(player->flags & PLAYER_WORK_ON_PATH)) {
                ptask->wait[slot] = 0;
                ptask->held &= ~(1 << slot);
            }
        }
        break;
    }
}

static void path_rail_step(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path = ptask->path;
    struct task_work *player;
    struct player *record;
    /* @hack redundant: unused, a point of frame that no code reads */
    NJS_POINT3 unused;
    Float distance;
    int slot;

    if (ptask->mode != PATH_SET_UP) {
        if (njInnerProduct(&ptask->up, &world_gravity) > RAIL_FLIP) {
            ptask->mode = PATH_RAIL_BENEATH;
        } else {
            ptask->mode = PATH_RAIL_ABOVE;
        }
    }
    switch (ptask->mode) {
    case PATH_RAIL_ABOVE: {
        NJS_POINT3 nearest;
        Float along;

        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            record = player_records[slot];
            player = player_work[slot];
            if (player == NULL) {
                continue;
            }
            if (record->stick_lock <= 0 && record->spd.y > 0.0F) {
                continue;
            }
            if (!(ptask->held & (1 << slot))) {
                if (ptask->wait[slot] < RAIL_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                ptask->wait[slot] = RAIL_WAIT;
                /* @hack spelling: player_work[slot] read again, where
                 * player, which holds it, would do */
                switch (player_work[slot]->submode) {
                case PLAYER_REQUEST_RESET:
                case PLAYER_REQUEST_MISS:
                    continue;
                }
                if (player->pos.x > ptask->high.x) {
                    continue;
                }
                if (player->pos.y > ptask->high.y) {
                    continue;
                }
                if (player->pos.z > ptask->high.z) {
                    continue;
                }
                if (player->pos.x < ptask->low.x) {
                    continue;
                }
                if (player->pos.y < ptask->low.y) {
                    continue;
                }
                if (player->pos.z < ptask->low.z) {
                    continue;
                }
                distance =
                    path_find_nearest(path, &player->pos, &nearest, &along);
                /* @hack spelling: player_records[slot] read again for
                 * stick_lock and ground.below_y, where record, which holds it,
                 * would do */
                if (player_records[slot]->stick_lock) {
                    if (distance > RAIL_LOCKED_REACH) {
                        continue;
                    }
                } else if (stage_current_id() == STAGE_FINAL_RUSH ||
                           stage_current_id() == STAGE_FINAL_CHASE ||
                           stage_current_id() == STAGE_BOSS_LAST1 ||
                           (stage_current_id() == STAGE_SKY_RAIL &&
                            !(player_records[slot]->ground.below_y >
                              NO_FLOOR))) {
                    if (distance > RAIL_WIDE_REACH) {
                        continue;
                    }
                } else if (distance > RAIL_REACH) {
                    continue;
                }
                if (along >= path->length) {
                    along = path->length - RAIL_END_ROOM;
                }
                player_rail_path(slot, path, along);
                ptask->held |= 1 << slot;
            } else if (!(player->flags & PLAYER_WORK_ON_PATH)) {
                ptask->wait[slot] = 0;
                ptask->held &= ~(1 << slot);
            }
        }
        break;
    }
    case PATH_RAIL_BENEATH: {
        NJS_POINT3 nearest;
        NJS_POINT3 head;
        Float along;
        int bit;

        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((player = player_work[slot]) == NULL) {
                continue;
            }
            bit = 1 << slot;
            if (!(ptask->held & bit)) {
                if (ptask->wait[slot] < RAIL_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                ptask->wait[slot] = RAIL_WAIT;
                /* @hack spelling: player_work[slot] read again, where
                 * player, which holds it, would do */
                switch (player_work[slot]->submode) {
                case PLAYER_REQUEST_RESET:
                case PLAYER_REQUEST_MISS:
                    continue;
                }
                if (player->pos.x > ptask->high.x) {
                    continue;
                }
                if (player->pos.y > ptask->high.y) {
                    continue;
                }
                if (player->pos.z > ptask->high.z) {
                    continue;
                }
                if (player->pos.x < ptask->low.x) {
                    continue;
                }
                if (player->pos.y < ptask->low.y) {
                    continue;
                }
                if (player->pos.z < ptask->low.z) {
                    continue;
                }
                head = player->pos;
                head.x -= world_gravity.x * RAIL_HEAD;
                head.y -= world_gravity.y * RAIL_HEAD;
                head.z -= world_gravity.z * RAIL_HEAD;
                distance = path_find_nearest(path, &head, &nearest, &along);
                if (distance > RAIL_REACH) {
                    continue;
                }
                player_rail_path_beneath(slot, path, along);
                ptask->held |= bit;
            } else if (!(player->flags & PLAYER_WORK_ON_PATH)) {
                ptask->wait[slot] = 0;
                ptask->held &= ~bit;
            }
        }
        break;
    }
    }
}

void path_rail_init(struct task *task)
{
    struct path_task *ptask = task->extra.p;

    ptask->held = 0;
    ptask->wait[0] = ptask->wait[1] = RAIL_WAIT;
    path_task_box(ptask);
    ptask->high.x += RAIL_MARGIN;
    ptask->high.y += RAIL_MARGIN;
    ptask->high.z += RAIL_MARGIN;
    ptask->low.x -= RAIL_MARGIN;
    ptask->low.y -= RAIL_MARGIN;
    ptask->low.z -= RAIL_MARGIN;
    ptask->mode = PATH_RAIL_ABOVE;
    ptask->up.x = 0.0F;
    ptask->up.y = 1.0F;
    ptask->up.z = 0.0F;
    task->step = path_rail_step;
}

void path_rail_inverted_init(struct task *task)
{
    struct path_task *ptask = task->extra.p;

    ptask->held = 0;
    ptask->wait[0] = ptask->wait[1] = RAIL_WAIT;
    path_task_box(ptask);
    ptask->high.x += RAIL_MARGIN;
    ptask->high.y += RAIL_MARGIN;
    ptask->high.z += RAIL_MARGIN;
    ptask->low.x -= RAIL_MARGIN;
    ptask->low.y -= RAIL_MARGIN;
    ptask->low.z -= RAIL_MARGIN;
    ptask->mode = PATH_RAIL_BENEATH;
    ptask->up.x = 0.0F;
    ptask->up.y = -1.0F;
    ptask->up.z = 0.0F;
    task->step = path_rail_step;
}

void path_run_step(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path = ptask->path;
    struct task_work *player;
    /* @hack type: slot a Uint8, where the int of the other steps would do */
    Uint8 slot;

    switch (ptask->mode) {
    case PATH_SET_UP:
        ptask->held = 0;
        ptask->wait[0] = ptask->wait[1] = RUN_WAIT;
        path_task_box(ptask);
        ptask->high.x += RUN_MARGIN;
        ptask->high.y += RUN_MARGIN;
        ptask->high.z += RUN_MARGIN;
        ptask->low.x -= RUN_MARGIN;
        ptask->low.y -= RUN_MARGIN;
        ptask->low.z -= RUN_MARGIN;
        ptask->mode = PATH_RUN;
        break;
    case PATH_RUN:
        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            player = player_work[slot];
            if (player == NULL) {
                continue;
            }
            if (!(ptask->held & (1 << slot))) {
                if (ptask->wait[slot] < RUN_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                /* @hack spelling: player_work[slot] read again, where
                 * player, which holds it, would do */
                switch (player_work[slot]->submode) {
                case PLAYER_REQUEST_RESET:
                case PLAYER_REQUEST_MISS:
                    continue;
                }
                if (player_read_stick(player, player_records[slot], NULL,
                                      NULL) ||
                    player_records[slot]->stick_lock) {
                    if (path_task_near_point(ptask, slot, NULL, RUN_REACH)) {
                        player_run_path(slot, path);
                        ptask->held |= 1 << slot;
                    }
                }
            } else if (!(player->flags & PLAYER_WORK_ON_PATH)) {
                ptask->wait[slot] = 0;
                ptask->held &= ~(1 << slot);
            }
        }
        break;
    }
}

void path_current_step(struct task *task)
{
    struct path_task *ptask = task->extra.p;
    struct path *path = ptask->path;
    struct task_work *player;
    NJS_VECTOR push;
    struct path_sample sample;
    Float distance;
    Float scale;
    int slot;

    switch (ptask->mode) {
    case PATH_SET_UP:
        ptask->held = 0;
        ptask->wait[0] = ptask->wait[1] = CURRENT_WAIT;
        path_task_box(ptask);
        ptask->high.x += CURRENT_MARGIN;
        ptask->high.y += CURRENT_MARGIN;
        ptask->high.z += CURRENT_MARGIN;
        ptask->low.x -= CURRENT_MARGIN;
        ptask->low.y -= CURRENT_MARGIN;
        ptask->low.z -= CURRENT_MARGIN;
        ptask->mode = PATH_RUN;
        /* @hack redundant: path set again to the path it holds */
        path = ptask->path;
        break;
    case PATH_RUN:
        for (slot = 0; slot < GAME_PLAYERS; slot++) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((player = player_work[slot]) == NULL) {
                continue;
            }
            if (!(ptask->held & (1 << slot))) {
                if (ptask->wait[slot] < CURRENT_WAIT) {
                    ptask->wait[slot]++;
                    continue;
                }
                /* @hack spelling: player_work[slot] read again for submode
                 * and mode, where player, which holds it, would do */
                switch (player_work[slot]->submode) {
                case PLAYER_REQUEST_RESET:
                case PLAYER_REQUEST_MISS:
                    continue;
                }
                if (game_state.time_stopped) {
                    continue;
                }
                if ((player->flags & PLAYER_WORK_ON_PATH) &&
                    player_records[slot]->path->step == path_current_step) {
                    continue;
                }
                if (!(player_records[slot]->ground.last_above_flags &
                      WORLD_FLAT)) {
                    continue;
                }
                if (player_records[slot]->jump_steps < CURRENT_JUMP_STEPS_MIN &&
                    player_work[slot]->mode == PLAYER_MODE_JUMP) {
                    continue;
                }
                if (player_records[slot]->character == PLAYER_SONIC) {
                    if (player_work[slot]->mode == PLAYER_MODE_SONIC_BOUNCE) {
                        continue;
                    }
                    if (player_work[slot]->mode ==
                        PLAYER_MODE_SONIC_BOUNCE_UP) {
                        continue;
                    }
                }
                if (path_task_near_path(ptask, slot, NULL, CURRENT_REACH)) {
                    player_current_path(slot, path);
                    ptask->held |= 1 << slot;
                }
            } else if (!(player->flags & PLAYER_WORK_ON_PATH) ||
                       player_records[slot]->path != path) {
                ptask->wait[slot] = 0;
                ptask->held &= ~(1 << slot);
            } else if (player_records[slot]->ground.last_above_flags &
                       WORLD_FLAT) {
                if (players_on_path(path)) {
                    path_find_nearest(path, &player->pos, &sample.pos,
                                      &sample.distance);
                    /* @hack spelling: the assignment inside the test, where
                     * a statement of its own would do */
                    if (!((distance = sample.distance) > 0.0F &&
                          distance < path->length &&
                          path_get_sample(path, &sample))) {
                        ptask->wait[slot] = 0;
                        ptask->held &= ~(1 << slot);
                        player->flags &= ~PLAYER_WORK_ON_PATH;
                        continue;
                    }
                    if (stage_current_id() == STAGE_BOSS_LAST1) {
                        push = sample.pos;
                        njSubVector(&push, &player->pos);
                        if (njScalor(&push) > CURRENT_NEAR) {
                            njUnitVector(&push);
                        } else {
                            push = sample.ahead;
                        }
                        scale = njInvertSqrt(
                            (CURRENT_SPEED_SHARE *
                             njScalor(&player_records[slot]->spd)) +
                            CURRENT_SPEED_FLOOR);
                        if (scale > CURRENT_PUSH_MAX) {
                            scale = CURRENT_PUSH_MAX;
                        }
                        scale *= current_push;
                        push.x = (push.x + sample.ahead.x) * scale;
                        push.y = (push.y + sample.ahead.y) * scale;
                        push.z = (push.z + sample.ahead.z) * scale;
                        player_set_speed(slot, push.x, push.y, push.z);
                    } else if (stage_current_id() == STAGE_RELAY_SONIC) {
                        scale = current_relay_acc;
                        push.x = sample.ahead.x * scale;
                        push.y = sample.ahead.y * scale;
                        push.z = sample.ahead.z * scale;
                        player_set_acc(slot, push.x, push.y, push.z);
                    } else {
                        push = sample.pos;
                        njSubVector(&push, &player->pos);
                        if (njScalor(&push) > CURRENT_NEAR) {
                            njUnitVector(&push);
                        } else {
                            push = sample.ahead;
                        }
                        scale = njInvertSqrt(
                            (CURRENT_SPEED_SHARE *
                             njScalor(&player_records[slot]->spd)) +
                            CURRENT_SPEED_FLOOR);
                        if (scale > CURRENT_PUSH_MAX) {
                            scale = CURRENT_PUSH_MAX;
                        }
                        scale *= current_push;
                        push.x = (push.x + sample.ahead.x) * scale;
                        push.y = (push.y + sample.ahead.y) * scale;
                        push.z = (push.z + sample.ahead.z) * scale;
                        njAddVector(&player_records[slot]->push, &push);
                        /* @hack spelling: the bound written <=
                         * CURRENT_STICK_LOCK - 1, where < CURRENT_STICK_LOCK
                         * would do */
                        if (player_records[slot]->stick_lock <=
                            CURRENT_STICK_LOCK - 1) {
                            player_records[slot]->stick_lock =
                                CURRENT_STICK_LOCK;
                        }
                    }
                }
            }
        }
        break;
    }
}
