#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "boss_last1/rails.h"
#include "boss_last1/path_lights.h"
#include "collision/collision.h"
#include "player/player.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"

/* @hack spelling: rail_draw_enabled, a symbol of its own at
 * game_state.unk_04[1], where the field would do */
#ifdef NONMATCHING
/* the field by its own spelling: this side's objects don't define
 * rail_draw_enabled, which only symbols.txt places */
#define rail_draw_enabled (game_state.unk_04[1])
#else
extern Sint8 rail_draw_enabled;
#endif

/* the point of the set's first rail where its hazard stands */
#define HAZARD_POINT 3

/* the record a rail's task keeps in its motion */
struct rail_link {
    Uint32 unused;
    /* the set's next rail, which this one kills as it dies */
    struct task *next;
};

static struct path_point rail_1_points[19] = {
#include "boss_last1/rails/rail_1_points.inc"
};
static struct path rail_1 = {
#include "boss_last1/rails/rail_1.inc"
};
static struct path_point rail_2_points[19] = {
#include "boss_last1/rails/rail_2_points.inc"
};
static struct path rail_2 = {
#include "boss_last1/rails/rail_2.inc"
};
static struct path_point rail_3_points[4] = {
#include "boss_last1/rails/rail_3_points.inc"
};
static struct path rail_3 = {
#include "boss_last1/rails/rail_3.inc"
};
static struct path_point rail_4_points[4] = {
#include "boss_last1/rails/rail_4_points.inc"
};
static struct path rail_4 = {
#include "boss_last1/rails/rail_4.inc"
};
static struct path_point rail_5_points[4] = {
#include "boss_last1/rails/rail_5_points.inc"
};
static struct path rail_5 = {
#include "boss_last1/rails/rail_5.inc"
};
static struct path_point rail_6_points[4] = {
#include "boss_last1/rails/rail_6_points.inc"
};
static struct path rail_6 = {
#include "boss_last1/rails/rail_6.inc"
};
static struct path *rail_templates[7] = {
#include "boss_last1/rails/rail_templates.inc"
};
static struct path *active_path = NULL;
struct path **biolizard_rail_paths = {
#include "boss_last1/rails/biolizard_rail_paths.inc"
};
static struct task *active_task = NULL;
static struct task *rail_hazard(struct task *parent, const NJS_POINT3 *pos);

static struct path *copy_path(struct path *source)
{
    struct path *copy;
    struct path **input;
    int bytes = 0;
    Uint16 count;
    /* @hack spelling: source read through input, its address, where source
     * itself would do */
    input = &source;
    copy = syCalloc(1, sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = **input;
    count = (*input)->count;
    switch ((*input)->flags) {
    case PATH_LAYOUT_POS:
        bytes = count * sizeof(NJS_POINT3);
        break;
    case PATH_LAYOUT_SLOPES:
        bytes = count * sizeof(struct path_point);
        break;
    case PATH_LAYOUT_UPS:
        bytes = count * sizeof(struct path_up_point);
        break;
    }
    copy->points = syCalloc(1, bytes);
    if (!copy->points) {
        syFree(copy);
        return NULL;
    }
    memcpy(copy->points, (*input)->points, bytes);
    return copy;
}

static void rotate_path(struct path *path, Angle angle)
{
    Sint8 count = path->count;
    struct path_point *point = path->points;
    Sint8 i;
    Angle x;
    Angle z;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, angle);
    njPushMatrixEx();
    for (i = 0; i < count; i++, point++) {
        {
            NJS_POINT3 from;
            NJS_POINT3 to;
            from = point->pos;
            njCalcPoint(NULL, &from, &to);
            point->pos = to;
        }
        {
            NJS_VECTOR up = {0, 1, 0};
            NJS_VECTOR turned;
            njRotateZ(NULL, point->slope_z);
            njRotateX(NULL, point->slope_x);
            njCalcVector(NULL, &up, &turned);
            direction_angles_xz(turned.x, turned.y, turned.z, &x, &z);
            point->slope_x = x;
            point->slope_z = z;
            matrix_load_parent();
        }
    }
    njPopMatrixEx();
    njPopMatrix(1);
}

static void rail_destroy(struct task *task)
{
    struct rail_link *link = task->motion.p;
    struct path_task *rail = task->extra.p;
    struct task *next = link->next;
    if (next) {
        /* @hack redundant: link->next takes next->next, the task after next
         * in its ring, and is freed below unread */
        link->next = next->next;
        task_kill(next);
    } else {
        active_path = NULL;
        active_task = NULL;
        player_work[0]->flags &= ~PLAYER_WORK_ON_PATH;
    }
    if (rail->path->points) {
        syFree(rail->path->points);
        rail->path->points = NULL;
    }
    syFree(rail->path);
    rail->path = NULL;
    syFree(link);
    task->motion.p = NULL;
}

struct task *biolizard_rails_create(struct path **paths, Sint8 count,
                                    Angle angle)
{
    struct path *path;
    struct path *copy;
    struct path **list;
    struct task *task;
#ifdef NONMATCHING
    /* starts previous at null, and tests it where the matching side tests
     * active_task. both are null until the set's first rail */
    struct task *previous = NULL;
#else
    struct task *previous;
#endif
    struct path_task *rail;
    struct rail_link *link;
    struct path_point *point;
    Bool has_hazard = FALSE;
    Sint8 i;
    if (active_task || !paths) {
        return NULL;
    }
    list = paths;
    path = *list++;
    if (path == rail_templates[0] || path == rail_templates[1]) {
        has_hazard = TRUE;
    }
    for (i = 0; i < count; i++) {
        copy = copy_path(path);
        if (!copy) {
            /* @bug memory: rotate_path reads through null where copy_path
             * fails. the null from 0x8c50b91e goes on to rotate_path at
             * 0x8c50b926 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above and the one tagged in the
             * matching side. it skips to the list's next path, as the loop's
             * end does */
            path = *list++;
            continue;
#else
            /* @bug memory: steps path to the bytes after its record at
             * 0x8c50b922, where it should take the list's next entry. the
             * next copy reads those bytes only where copy->step reads 0
             * through null */
            path++;
#endif
        }
        rotate_path(copy, angle);
        if (copy->step) {
            if ((task = path_task_create(copy->step)) != NULL) {
                rail = task->extra.p;
                rail->path = copy;
                link = syCalloc(1, sizeof(*link));
                if (!link) {
                    /* @bug memory: leaks the path's copy where the link's
                     * allocation fails. the copy is made at 0x8c50b918, and
                     * the task that holds it dies at 0x8c50b950, before it
                     * takes rail_destroy at 0x8c50b9b6 */
#ifdef NONMATCHING
                    /* this side repairs the bug tagged above: it frees the
                     * path's copy */
                    syFree(copy->points);
                    syFree(copy);
                    rail->path = NULL;
#endif
                    task_kill(task);
#ifdef NONMATCHING
                    /* this side repairs the bug tagged in the matching side. it
                     * takes the list's next path, as the loop's end does */
                    path = *list++;
#else
                    /* @bug memory: steps path to the bytes after its record
                     * at 0x8c50b956, where it should take the list's next
                     * entry. the next copy reads those bytes as a path */
                    path++;
#endif
                    continue;
                }
                task->motion.p = link;
#ifdef NONMATCHING
                /* this side tests previous, which starts null, where the
                 * matching side tests active_task. both are null until the
                 * set's first rail, so the two tests agree. this test shows
                 * that the else reads previous only once it is set */
                if (!previous) {
#else
                if (!active_task) {
#endif
                    active_task = task;
                    active_path = copy;
                } else {
                    link = previous->motion.p;
                    link->next = task;
                }
                previous = task;
                if (rail_draw_enabled) {
                    task->draw = path_draw;
                }
                task->destructor = rail_destroy;
                biolizard_path_lights_create(task, rail->path);
            }
            /* @bug memory: leaks the path's copy where path_task_create
             * fails. the copy is made at 0x8c50b918, the creation fails at
             * 0x8c50b936, and the copy is dropped at 0x8c50b9be */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it frees the path's
             * copy */
            else {
                syFree(copy->points);
                syFree(copy);
            }
#endif
            path = *list++;
        }
    }
    /* @hack redundant: a test of previous that never runs */
    if (0) {
        if (previous) {
        }
    }
    if (has_hazard) {
        /* @bug memory: reads through a null active_path at 0x8c50b9dc where
         * the set has no rail, as where path_task_create fails at 0x8c50b936 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: without a rail, it returns
         * null */
        if (!active_path) {
            return NULL;
        }
#endif
        point = active_path->points;
        point += HAZARD_POINT;
        rail_hazard(active_task, &point->pos);
    }
    return active_task;
}

struct path *biolizard_active_rail(void)
{
    return active_path;
}

static void hazard_step(struct task *task)
{
    if (!game_state.time_stopped && !game_state.unk_09) {
        collision_register(task);
    }
}

static void hazard_destroy(struct task *task)
{
}

static const struct collision_shape contact_shape = {
#include "boss_last1/rails/contact_shape.inc"
};
static struct task *rail_hazard(struct task *parent, const NJS_POINT3 *pos)
{
    struct task *task;
    struct task_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, hazard_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    work->pos = *pos;
    if (!collision_init(task, &contact_shape, 1, COLLISION_ENEMY)) {
        task_kill(task);
        return NULL;
    }
    task->destructor = hazard_destroy;
    return task;
}
