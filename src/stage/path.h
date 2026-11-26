/* a stage's paths: lines of points, places along them by distance, and the
 * task that starts their tasks */
#ifndef STAGE_PATH_H
#define STAGE_PATH_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a path's flags. PATH_LAYOUT_BITS hold its points' enum path_layout. a
 * PATH_CLOSED path is a loop, and any other path stops at its ends */
#define PATH_LAYOUT_BITS 0x7fff
#define PATH_CLOSED 0x8000

/* what each of a path's points holds. a layout without lengths, below, is
 * PATH_LAYOUT_POS or a value not named here */
enum path_layout {
    PATH_LAYOUT_POS = 0, /* an NJS_POINT3 alone */
    PATH_LAYOUT_SLOPES = 1, /* a struct path_point */
    PATH_LAYOUT_UPS = 2 /* a struct path_up_point */
};

/* a point of a PATH_LAYOUT_SLOPES path. up at the point is the y axis turned
 * by slope_x about x, then by slope_z about z */
struct path_point {
    Sint16 slope_x;
    Sint16 slope_z;
    Float length; /* of the point's segment */
    NJS_POINT3 pos;
};

/* a point of a PATH_LAYOUT_UPS path */
struct path_up_point {
    NJS_VECTOR up; /* a unit vector */
    Float length; /* of the point's segment */
    NJS_POINT3 pos;
};

/* a line of count points. a point's segment runs from it to the next point.
 * a closed path's last segment runs back to its first point */
struct path {
    Sint16 flags; /* PATH_CLOSED and the layout */
    Sint16 count;
    Float length; /* of the whole path */
    void *points; /* count points of the layout */
    /* the step path_create_manager starts the path's task with, or null for
     * no task */
    void (*step)(struct task *task);
};

/* a place along a path. the caller sets distance, and path_get_sample sets
 * the rest. slope_x and slope_z are the tilt at the start of the place's
 * segment, as in a struct path_point. ang_x and ang_z are the tilt at the
 * place. they blend the tilts at the segment's two ends by how far through
 * it the place lies.
 *
 * the ends of a PATH_LAYOUT_SLOPES segment take the slopes halfway between
 * their own point's and the point before's. on a PATH_LAYOUT_UPS path, up
 * blends the ends' ups the other way round from pos. ang_x and ang_z follow
 * it. so up is the end point's at the segment's start, and the start
 * point's at its end */
struct path_sample {
    Angle slope_x;
    Angle slope_z;
    Angle ang_x;
    Angle ang_z;
    Float distance;
    NJS_POINT3 pos;
    NJS_VECTOR point_up; /* the way up slope_x and slope_z give */
    NJS_VECTOR up; /* the way up ang_x and ang_z give */
    NJS_VECTOR ahead; /* a unit vector along the segment */
};

/* start the task named "ManagePath", which does nothing. each of paths with a
 * step then gets a task from path_task_create, whose record holds the path.
 * paths ends with a null. it returns TRUE, or FALSE with no task started
 * where task_create fails. a path whose task can't be created has none */
Bool path_create_manager(struct path **paths);

/* set *pos to point index's pos, in any of the three layouts. any other
 * layout leaves *pos. index is unchecked */
void path_point_pos(struct path *path, int index, NJS_POINT3 *pos);
/* fill sample at sample->distance along path, and return TRUE. a closed
 * path's distance first wraps into its length, in place. a point whose length
 * is 0 counts as 1 long. it returns FALSE, leaving the sample, for an open
 * path's distance below 0 or past its length. it returns FALSE after the wrap
 * for a layout without lengths. path must hold a point, unchecked. the distance
 * must lie within the points' lengths summed, unchecked. past them, the
 * segment starts at the point past the last */
Bool path_get_sample(struct path *path, struct path_sample *sample);
/* set *index to the point whose segment holds distance. that is the first
 * whose length and the lengths before it sum to distance or more. it returns
 * TRUE, or FALSE with *index set to count where distance lies past the lengths
 * summed. a layout without lengths returns FALSE and leaves *index */
Bool path_segment_at(struct path *path, Float distance, unsigned int *index);
/* set *distance to how far along path point index lies, and return TRUE.
 * that is the lengths of the points before it summed. it returns FALSE, leaving
 * *distance, for an index past count, and for a layout without lengths */
Bool path_point_distance(struct path *path, unsigned int index,
                         Float *distance);
/* distance, wrapped into 0 up to the path's length as float_wrap wraps it */
Float path_wrap(struct path *path, Float distance);
/* how far pos lies from the nearest place on path's segments. *nearest takes
 * that place, and *along how far along the path it lies, each where not
 * null. a layout without lengths returns 0 and sets neither. path must have
 * a segment, unchecked: without one, it returns a value nothing set.
 *
 * a segment that starts 0 along the path sets the nearest place, however far
 * it lies. that is the first segment, and each after segments of length 0
 * alone. a later segment sets the place only where it lies nearer than the
 * place so far.
 *
 * a closed PATH_LAYOUT_UPS path's last segment doesn't end at its first
 * point. it ends at a place read from that point's other fields */
Float path_find_nearest(struct path *path, NJS_POINT3 *pos, NJS_POINT3 *nearest,
                        Float *along);
/* the curves path_spline_pos draws through four points of a path */
enum path_spline {
    PATH_SPLINE_CARDINAL = 0, /* by njCardinalSpline, at a tension of 0.2 */
    PATH_SPLINE_BEZIER = 1, /* by njCubicBezierSpline */
    PATH_SPLINE_OVERHAUSER = 2 /* by njOverhauserSpline */
};
/* nothing in the game's images calls it. set sample->pos to a place on the
 * curve of kind, an enum path_spline. the curve's four points are the first of
 * sample->distance's segment and the three after it. a third or fourth point
 * past the path's end repeats the one before it. a second point past the end is
 * read there, unchecked. the curve runs from the second point to the third.
 * under PATH_SPLINE_BEZIER it runs from the first to the fourth. any other kind
 * leaves sample->pos. path must have lengths, and the distance must lie on it,
 * unchecked */
void path_spline_pos(struct path *path, struct path_sample *sample, int kind);

#endif
