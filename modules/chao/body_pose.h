/* the pose walk's state, which each Chao's walk takes over in turn, and its
 * springs' matrix steps */
#ifndef CHAO_BODY_POSE_H
#define CHAO_BODY_POSE_H
#include <ninja.h>
#include <sg_xpt.h>

/* a motion as the walk reads it. node is the key lists of the node the walk
 * is at, and frames the motion's frame count */
struct chao_pose_track {
    NJS_MDATA2 *node;
    Float frame;
    Uint32 frames;
};

/* the playing motion, and while it links, the next one, whose weight share
 * runs from 0 to 1. next.node is null while the motion doesn't link */
struct chao_pose_blend {
    struct chao_pose_track current, next;
    Float share;
};

/* the most vertices a mesh's springs move. chao_spring_points holds how far
 * each of one mesh's resting points has moved since the last step */
#define CHAO_SPRING_POINTS 512
extern NJS_POINT3 chao_spring_points[CHAO_SPRING_POINTS];
extern struct chao_pose_blend chao_pose;
/* chao_pose.current.node, as chao_transform_node last stepped it */
extern NJS_MDATA2 *chao_pose_cursor;
extern NJS_MATRIX chao_spring_delta;

/* subtract previous from matrix, element by element. a null matrix is
 * Ninja's current one. a matrix in memory keeps its last element */
void garden_subtract_matrix(NJS_MATRIX *matrix, NJS_MATRIX *previous);
/* set matrix's translation to 0 and its last element to 1. a null matrix is
 * Ninja's current one */
void garden_clear_translation(NJS_MATRIX *matrix);

#endif
