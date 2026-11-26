/* the easy chunk drawer's motion links: njCnkMotLinkS.c's text, under the
 * names and parameters below. the links interpolate in a straight line,
 * whatever the spline counts. the Old draws take each angle the short way
 * round */
/* NOLINTBEGIN(readability-identifier-naming) */
#define njCnkSimplePushPopMotionLink njCnkEasyPushPopMotionLink
#define njCnkSimplePushPopShapeLink njCnkEasyPushPopShapeLink
#define njCnkSimpleDrawMotionLink njCnkEasyDrawMotionLink
#define njCnkSimpleDrawShapeMotionLink njCnkEasyDrawShapeMotionLink
#define njCnkSimpleDrawMotionLinkOld njCnkEasyDrawMotionLinkOld
#define njCnkSimpleDrawShapeMotionLinkOld njCnkEasyDrawShapeMotionLinkOld
/* NOLINTEND(readability-identifier-naming) */
#define LINK_DRAW_MODEL _njCnkEasyDrawModel
#define LINK_MAKE_SHAPE_LINK njCnkMakeEasyShapeLink
#define LINK_MOTION_INT_F njCnkLinearMotLinkF
#define LINK_MOTION_INT_A njCnkLinearMotLinkA
#define LINK_INT_F njCnkLinearIntF
#define LINK_INT_A njCnkLinearIntA
#define LINK_SHAPE_POINT njCnkEasyShapeLinkLinearPoint
#define LINK_MOTION_INT_A_OLD njCnkLinearMotLinkAOld
#define LINK_INT_A_OLD njCnkLinearIntAOld

#include "../ninja/njCnkMotLinkS.c" /* NOLINT(bugprone-suspicious-include) */
