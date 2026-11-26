/* the easy chunk drawer's motion draws: njCnkMotionS.c's text, under the
 * names and parameters below. the draws interpolate in a straight line,
 * whatever the spline counts */
/* NOLINTBEGIN(readability-identifier-naming) */
#define njCnkSimpleDrawMotion njCnkEasyDrawMotion
#define njCnkSimpleDrawShapeMotion njCnkEasyDrawShapeMotion
#define njCnkSimpleDrawMotionOld njCnkEasyDrawMotionOld
#define njCnkSimpleDrawShapeMotionOld njCnkEasyDrawShapeMotionOld
#define _njCnkSimpleDrawModel _njCnkEasyDrawModel
/* NOLINTEND(readability-identifier-naming) */
#define MOTION_INT_F(spline) njCnkLinearMotionF
#define MOTION_INT_A(spline) njCnkLinearMotionA
#define MOTION_INT_A_OLD(spline) njCnkLinearMotionAOld
#define SHAPE_INT(spline) njCnkEasyShapeLinearPoint

#include "../ninja/njCnkMotionS.c" /* NOLINT(bugprone-suspicious-include) */
