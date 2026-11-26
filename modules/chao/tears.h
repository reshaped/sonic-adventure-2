/* CHAO_SHED_TEAR sheds a tear from eye, a violet splash 0.2 below the eye.
 * it moves at 0.1 along the eye's direction, plus 0.1 up. point and
 * velocity are the caller's */
#ifndef CHAO_TEARS_H
#define CHAO_TEARS_H
#include "chao/work.h"
#include "shared/splash.h"

#define CHAO_SHED_TEAR(shape, eye, point, velocity)                            \
    {                                                                          \
        (point) = (shape)->eyes[eye].position;                                 \
        (point).y -= 0.2F;                                                     \
        (velocity).x = (shape)->eyes[eye].direction.x * 0.1F;                  \
        (velocity).y = ((shape)->eyes[eye].direction.y * 0.1F) + 0.1F;         \
        (velocity).z = (shape)->eyes[eye].direction.z * 0.1F;                  \
        splash_violet_create(&(point), &(velocity), 0.1F);                     \
    }

#endif
