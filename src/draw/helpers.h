/* a sphere's test against the current camera's view */
#ifndef DRAW_HELPERS_H
#define DRAW_HELPERS_H
#include <ninja.h>
#include <sg_xpt.h>

/* TRUE when a sphere of radius at center is out of the current camera's
 * view, and FALSE otherwise. a sphere is in view where center lies less
 * than 50 degrees off the camera's direction. it is in view too where
 * center's squared distance from the camera plus radius squared is below
 * 2500 */
Bool sphere_out_of_view(const NJS_POINT3 *center, Float radius);

#endif
