/* the pose walk's scratch, which each Chao's walk takes over in turn. the
 * spring pass transforms one mesh's resting points at a time into
 * chao_spring_points */
#include <ninja.h>

#include "chao/body_pose.h"

NJS_POINT3 chao_spring_points[CHAO_SPRING_POINTS];
struct chao_pose_blend chao_pose;
NJS_MDATA2 *chao_pose_cursor;
NJS_MATRIX chao_spring_delta;
