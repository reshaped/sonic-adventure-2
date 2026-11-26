/* a stage's paths: lines of points, places along them by distance, and the
 * task that starts their tasks */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/polygon.h"
#include "stage/path.h"
#include "stage/path_task.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the tension path_spline_pos gives njCardinalSpline */
#define CARDINAL_TENSION 0.2F

/* what path_find_nearest carries from segment to segment */
struct nearest_search {
    Float start; /* how far along the path the segment starts */
    Float length; /* of the segment */
    Float best; /* how far pos lies from the nearest place so far */
    /* the segment's ends */
    NJS_POINT3 *from;
    NJS_POINT3 *to;
    NJS_POINT3 *pos; /* the point measured */
    /* path_find_nearest's along and nearest, either of which can be null */
    Float *along;
    NJS_POINT3 *nearest;
};

/* the four points path_spline_pos draws its curve through */
/* @hack type: the points in a struct, where an array of four would do */
struct spline_points {
    NJS_POINT3 points[4];
};

static void path_manager_step(struct task *task);
static void path_manager_destroy(struct task *task);
static void path_create_tasks(struct path **paths);

/* the "ManagePath" task, or null while none lives */
static struct task *path_manager_task = NULL;

Bool path_create_manager(struct path **paths)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, 0, path_manager_step, "ManagePath")) !=
        NULL) {
        path_manager_task = task;
        task->destructor = path_manager_destroy;
        path_create_tasks(paths);
        return TRUE;
    }
    return FALSE;
}

static void path_manager_step(struct task *task)
{
}

static void path_manager_destroy(struct task *task)
{
    path_manager_task = NULL;
}

static void path_create_tasks(struct path **paths)
{
    struct path *path;
    struct task *task;
    struct path_task *record;

    for (path = *paths++; path != NULL; path = *paths++) {
        if (path->step != NULL) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((task = path_task_create(path->step)) != NULL) {
                record = task->extra.p;
                record->path = path;
            }
        }
    }
}

void path_point_pos(struct path *path, int index, NJS_POINT3 *pos)
{
    NJS_POINT3 *plain;
    struct path_point *point;
    struct path_up_point *up_point;

    /* @hack spelling: each layout's point named in a local, where a copy from
     * the points indexed in place would do */
    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_POS:
        plain = (NJS_POINT3 *)path->points + index;
        *pos = *plain;
        break;
    case PATH_LAYOUT_SLOPES:
        point = (struct path_point *)path->points + index;
        *pos = point->pos;
        break;
    case PATH_LAYOUT_UPS:
        up_point = (struct path_up_point *)path->points + index;
        *pos = up_point->pos;
        break;
    }
}

/* fill out with the two ends of point index's segment, as two points in
 * path's layout. a PATH_LAYOUT_SLOPES end takes the slopes halfway between
 * its own point's and the point before's. past its ends, an open path repeats
 * its first and last points, and a closed one wraps round. path must have 2
 * points or more, unchecked. on a path of one, the segment's end is read past
 * the point */
static void segment_ends(struct path *path, int index, void *out)
{
    int before = index - 1;
    int after = index + 1;
    struct path_point *ends;
    struct path_point *prev;
    struct path_point *point;
    struct path_point *next;
    Sint16 slope_x;
    Sint16 slope_z;
    NJS_POINT3 *plain_ends;
    NJS_POINT3 *plains;
    NJS_POINT3 *plain;
    NJS_POINT3 *plain_next;
    struct path_up_point *up_ends;
    struct path_up_point *ups;
    struct path_up_point *up_point;
    struct path_up_point *up_next;

    switch (path->flags & PATH_CLOSED) {
    case 0:
        if (before < 0) {
            before = 0;
        } else if (after >= path->count) {
            after = path->count - 1;
        }
        break;
    case PATH_CLOSED:
        if (before < 0) {
            before = path->count - 1;
        } else if (after >= path->count) {
            after = 0;
        }
        break;
    }
    /* @hack spelling: in the PATH_LAYOUT_POS and PATH_LAYOUT_UPS cases, each
     * end's point named in a local, where copies from the points indexed in
     * place would do */
    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_POS:
        plain_ends = out;
        plains = path->points;
        plain = &plains[index];
        plain_next = &plains[after];
        plain_ends[0] = *plain;
        plain_ends[1] = *plain_next;
        break;
    case PATH_LAYOUT_SLOPES:
        ends = out;
        prev = (struct path_point *)path->points + before;
        point = (struct path_point *)path->points + index;
        next = (struct path_point *)path->points + after;
        /* @hack spelling: slope_x and slope_z, locals for point's slopes,
         * where each use would read them in place */
        slope_x = point->slope_x;
        slope_z = point->slope_z;
        ends[0].slope_x =
            prev->slope_x + (angle_difference(prev->slope_x, slope_x) >> 1);
        ends[0].slope_z =
            prev->slope_z + (angle_difference(prev->slope_z, slope_z) >> 1);
        ends[1].slope_x =
            slope_x + (angle_difference(slope_x, next->slope_x) >> 1);
        ends[1].slope_z =
            slope_z + (angle_difference(slope_z, next->slope_z) >> 1);
        ends[0].pos = point->pos;
        ends[1].pos = next->pos;
        ends[0].length = point->length;
        ends[1].length = next->length;
        break;
    case PATH_LAYOUT_UPS:
        up_ends = out;
        ups = path->points;
        up_point = &ups[index];
        up_next = &ups[after];
        up_ends[0].pos = up_point->pos;
        up_ends[1].pos = up_next->pos;
        up_ends[0].up = up_point->up;
        up_ends[1].up = up_next->up;
        up_ends[0].length = up_point->length;
        up_ends[1].length = up_next->length;
        break;
    }
}

Bool path_get_sample(struct path *path, struct path_sample *sample)
{
    NJS_VECTOR delta;
    Float distance;
    Float sum;
    Float length;
    Float part;
    Float rest;
    int i;

    distance = sample->distance;
    switch (path->flags & PATH_CLOSED) {
    case 0:
        if (distance < 0.0F || distance > path->length) {
            return FALSE;
        }
        break;
    case PATH_CLOSED:
        sample->distance = float_wrap(distance, path->length);
        break;
    }
#ifdef NONMATCHING
    /* this side sets length first, since GCC can't see that each loop below
     * sets it. path.h requires a point, so each loop runs */
    length = 1.0F;
#endif
    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_SLOPES: {
        struct path_point ends[2];
        struct path_point *point;

        point = path->points;
        distance = sample->distance;
        for (i = 0, sum = 0.0F; i < path->count; i++, point++, sum += length) {
            length = point->length;
            if (length == 0.0F) {
                length = 1.0F;
            }
            if (distance <= sum + length) {
                break;
            }
        }
        segment_ends(path, i, ends);
        part = (distance - sum) / length;
        sample->slope_x = ends[0].slope_x;
        sample->slope_z = ends[0].slope_z;
        sample->ang_x =
            ends[0].slope_x +
            (Angle)(angle_difference(ends[0].slope_x, ends[1].slope_x) * part);
        sample->ang_z =
            ends[0].slope_z +
            (Angle)(angle_difference(ends[0].slope_z, ends[1].slope_z) * part);
        delta.x = ends[1].pos.x - ends[0].pos.x;
        delta.y = ends[1].pos.y - ends[0].pos.y;
        delta.z = ends[1].pos.z - ends[0].pos.z;
        sample->pos.x = ends[0].pos.x + (part * delta.x);
        sample->pos.y = ends[0].pos.y + (part * delta.y);
        sample->pos.z = ends[0].pos.z + (part * delta.z);
        sample->point_up.x = -(njSin(sample->slope_z) * njCos(sample->slope_x));
        sample->point_up.y = njCos(sample->slope_z) * njCos(sample->slope_x);
        sample->point_up.z = njSin(sample->slope_x);
        sample->up.x = -(njSin(sample->ang_z) * njCos(sample->ang_x));
        sample->up.y = njCos(sample->ang_z) * njCos(sample->ang_x);
        sample->up.z = njSin(sample->ang_x);
        njUnitVector(&delta);
        sample->ahead = delta;
        break;
    }
    case PATH_LAYOUT_UPS: {
        struct path_up_point ends[2];
        struct path_up_point *point;

        point = path->points;
        distance = sample->distance;
        for (i = 0, sum = 0.0F; i < path->count; i++, point++, sum += length) {
            length = point->length;
            if (length == 0.0F) {
                length = 1.0F;
            }
            if (distance <= sum + length) {
                break;
            }
        }
        segment_ends(path, i, ends);
        part = (distance - sum) / length;
        /* @hack spelling: rest, a local for 1 - part, where the sums below
         * would subtract in place */
        rest = 1.0F - part;
        delta.x = ends[1].pos.x - ends[0].pos.x;
        delta.y = ends[1].pos.y - ends[0].pos.y;
        delta.z = ends[1].pos.z - ends[0].pos.z;
        sample->pos.x = ends[0].pos.x + (part * delta.x);
        sample->pos.y = ends[0].pos.y + (part * delta.y);
        sample->pos.z = ends[0].pos.z + (part * delta.z);
        sample->point_up = ends[0].up;
        /* @bug logic: weighs the segment's first up by part and its last by
         * 1 - part, the other way round from pos: up starts at the last up
         * and ends at the first (FMUL FR5,FR3 at 0x8c051bdc, FMAC
         * FR0,FR2,FR3 at 0x8c051bde) */
        sample->up.x = (ends[0].up.x * part) + (ends[1].up.x * rest);
        sample->up.y = (ends[0].up.y * part) + (ends[1].up.y * rest);
        sample->up.z = (ends[0].up.z * part) + (ends[1].up.z * rest);
        njUnitVector(&sample->up);
        njUnitVector(&delta);
        sample->ahead = delta;
        sample->slope_x = (Angle)(asinf(sample->point_up.z) * ANGLE_PER_RADIAN);
        sample->slope_z =
            -(Angle)(atan2f(sample->point_up.x, sample->point_up.y) *
                     ANGLE_PER_RADIAN);
        sample->ang_x = (Angle)(asinf(sample->up.z) * ANGLE_PER_RADIAN);
        sample->ang_z =
            -(Angle)(atan2f(sample->up.x, sample->up.y) * ANGLE_PER_RADIAN);
        break;
    }
    /* @hack redundant: the PATH_LAYOUT_POS label beside default */
    case PATH_LAYOUT_POS:
    default:
        return FALSE;
    }
    return TRUE;
}

Bool path_segment_at(struct path *path, Float distance, unsigned int *index)
{
    int count = path->count;
    struct path_point *point;
    struct path_up_point *up_point;
    Float sum;
    Float length;
    int i;

    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_SLOPES:
        point = path->points;
        for (i = 0, sum = 0.0F; i < count; i++, point++, sum += length) {
            length = point->length;
            if (distance <= sum + length) {
                break;
            }
        }
        break;
    case PATH_LAYOUT_UPS:
        up_point = path->points;
        for (i = 0, sum = 0.0F; i < count; i++, up_point++, sum += length) {
            length = up_point->length;
            if (distance <= sum + length) {
                break;
            }
        }
        break;
    default:
        return FALSE;
    }
    *index = i;
    /* @hack spelling: the result as two returns, where return i < count
     * would do */
    if (i < count) {
        return TRUE;
    }
    return FALSE;
}

Bool path_point_distance(struct path *path, unsigned int index, Float *distance)
{
    struct path_point *point;
    struct path_up_point *up_point;
    Float sum;
    unsigned int i;

    if (index > (unsigned int)path->count) {
        return FALSE;
    }
    /* @hack spelling: each loop counts i before it adds the point's length,
     * where a for loop would count it after */
    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_SLOPES:
        point = path->points;
        i = 0;
        sum = 0.0F;
        while (i < index) {
            i++;
            sum += point->length;
            point++;
        }
        break;
    case PATH_LAYOUT_UPS:
        up_point = path->points;
        i = 0;
        sum = 0.0F;
        while (i < index) {
            i++;
            sum += up_point->length;
            up_point++;
        }
        break;
    default:
        return FALSE;
    }
    *distance = sum;
    return TRUE;
}

Float path_wrap(struct path *path, Float distance)
{
    return float_wrap(distance, path->length);
}

/* measure search's segment, and move start past it. a segment that starts 0
 * along the path sets the nearest place without a comparison. the first
 * does, so it sets best */
static void nearest_measure(struct nearest_search *search)
{
    NJS_POINT3 on;
    Float away;

    /* @hack spelling: the else arm's assignment inside its test, where a
     * statement of its own would do */
    if (search->start == 0.0F) {
        search->best = collision_segment_distance(search->pos, search->from,
                                                  search->to, &on);
        if (search->nearest != NULL) {
            *search->nearest = on;
        }
        if (search->along != NULL) {
            *search->along = njDistanceP2P(search->from, &on) + search->start;
        }
    } else if (search->best >
               (away = collision_segment_distance(search->pos, search->from,
                                                  search->to, &on))) {
        search->best = away;
        if (search->nearest != NULL) {
            *search->nearest = on;
        }
        if (search->along != NULL) {
            *search->along = njDistanceP2P(search->from, &on) + search->start;
        }
    }
    search->start += search->length;
}

/* an open path has a segment fewer than it has points, and a closed path as
 * many */
Float path_find_nearest(struct path *path, NJS_POINT3 *pos, NJS_POINT3 *nearest,
                        Float *along)
{
    struct nearest_search search;
    struct path_point *point;
    struct path_up_point *up_point;
    int last;
    int i;

    search.start = 0.0F;
    search.pos = pos;
    search.nearest = nearest;
    search.along = along;
    /* @hack spelling: the bit compared with PATH_CLOSED, where a test of the
     * bit alone would do */
    if ((path->flags & PATH_CLOSED) == PATH_CLOSED) {
        last = 0;
    } else {
        last = 1;
    }
    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_SLOPES:
        for (i = path->count, point = path->points; i > last; i--) {
            search.length = point->length;
            search.from = &point->pos;
            point++;
            if (i != 1) {
                search.to = &point->pos;
            } else {
                search.to = &((struct path_point *)path->points)->pos;
            }
            nearest_measure(&search);
        }
        break;
    case PATH_LAYOUT_UPS:
        for (i = path->count, up_point = path->points; i > last; i--) {
            search.length = up_point->length;
            search.from = &up_point->pos;
            up_point++;
            if (i != 1) {
                search.to = &up_point->pos;
            } else {
                /* @bug logic: ends the closing segment at the place a struct
                 * path_point keeps its pos, 8 bytes into the first point. it
                 * reads the first point's up.z, length and pos.x as a point
                 * (ADD #8,R3 at 0x8c051f22) */
                search.to = &((struct path_point *)path->points)->pos;
            }
            nearest_measure(&search);
        }
        break;
    default:
        return 0.0F;
    }
    return search.best;
}

/* set *part to distance's share of the way through its segment, and return
 * TRUE. a distance below 0 gives a share below 0. it returns FALSE, leaving
 * *part, where part is null or path_segment_at returns FALSE */
static Bool path_segment_part(struct path *path, Float distance, Float *part)
{
    struct path_point *point;
    struct path_up_point *up_point;
    Float length;
    unsigned int index;
    Float start;

    if (part == NULL || path_segment_at(path, distance, &index) == FALSE) {
        return FALSE;
    }
    path_point_distance(path, index, &start);
    switch (path->flags & PATH_LAYOUT_BITS) {
    case PATH_LAYOUT_POS:
        path_point_distance(path, index + 1, &length);
        length -= start;
        break;
    case PATH_LAYOUT_SLOPES:
        point = (struct path_point *)path->points + index;
        length = point->length;
        break;
    case PATH_LAYOUT_UPS:
        up_point = (struct path_up_point *)path->points + index;
        length = up_point->length;
        break;
    }
    *part = (distance - start) / length;
    return TRUE;
}

void path_spline_pos(struct path *path, struct path_sample *sample, int kind)
{
    struct spline_points around;
    NJS_SPLINE spline;
#ifdef NONMATCHING
    /* the tension this side points spline's fparam at, below */
    Float tension;
#endif
    NJS_POINT3 *point;
    Float part;
    unsigned int index;

    path_segment_at(path, sample->distance, &index);
    path_point_pos(path, index, &around.points[0]);
    path_point_pos(path, index + 1, &around.points[1]);
    /* @hack spelling: point, a local set to each later point ahead of its
     * test, where each arm would take the point's address itself */
    point = &around.points[2];
    if (index + 2 >= (unsigned int)path->count) {
        *point = around.points[1];
    } else {
        path_point_pos(path, index + 2, point);
    }
    point = &around.points[3];
    if (index + 3 >= (unsigned int)path->count) {
        *point = around.points[2];
    } else {
        path_point_pos(path, index + 3, point);
    }
    path_point_distance(path, index, &part);
    path_segment_part(path, sample->distance, &part);
    switch (kind) {
    case PATH_SPLINE_BEZIER:
        njCubicBezierSpline((Float *)&around, &sample->pos.x, NULL, part);
        break;
    case PATH_SPLINE_CARDINAL:
        /* @bug memory: stores the tension through spline's fparam, which
         * nothing sets. the store, and njCardinalSpline's read of the
         * tension, go wherever the stack's word points (MOV.L @(20,R15),R3
         * at 0x8c0520a6, FMOV.S FR3,@R3 at 0x8c0520b0) */
#ifdef NONMATCHING
        /* this side repairs the tag above, whose target C can't give: its
         * fparam points at a tension of its own */
        spline.fparam = &tension;
#endif
        spline.fparam[0] = CARDINAL_TENSION;
        njCardinalSpline((Float *)&around, &sample->pos.x, &spline, part);
        break;
    case PATH_SPLINE_OVERHAUSER:
        njOverhauserSpline((Float *)&around, &sample->pos.x, NULL, part);
        break;
    }
}
