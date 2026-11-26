/* the menus' projections: where a point on a circle about the y axis lands
 * on the screen */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_projection.h"
#include "system/angle.h"

#define PANEL_RISE 2.0F
#define ARC_RISE 0.8F
#define QUARTER_DEGREES 90
#define HALF_DEGREES 180

void menu_panel_point(const struct menu_panel *panel, NJS_POINT3 *out,
                      Int angle)
{
    NJS_CAMERA camera;
    NJS_POINT3 point;
    NJS_POINT2 screen;

    point.z =
        njSin((QUARTER_DEGREES - angle) * ANGLE_PER_DEGREE) * panel->radius;
    point.y = 0.0F;
    point.x =
        njCos((QUARTER_DEGREES - angle) * ANGLE_PER_DEGREE) * panel->radius;
    njUnitMatrix(NULL);
    njRotateZ(NULL, panel->roll);
    njCalcVector(NULL, &point, &point);
    point.z += panel->depth;
    njInitCamera(&camera);
    camera.py += PANEL_RISE;
    njSetCamera(&camera);
    njProjectScreen(NULL, &point, &screen);
    out->x =
        ((panel->pos.x - screen.x) * angle * (1.0F / HALF_DEGREES)) + screen.x;
    out->y =
        ((panel->pos.y - screen.y) * angle * (1.0F / HALF_DEGREES)) + screen.y;
}

void menu_arc_point(const struct menu_arc *arc, NJS_POINT3 *out, Int angle)
{
    NJS_CAMERA camera;
    NJS_POINT3 point;
    NJS_POINT2 screen;

    point.x = njSin(angle * ANGLE_PER_DEGREE) * arc->radius;
    point.y = 0.0F;
    point.z = njCos(angle * ANGLE_PER_DEGREE) * arc->radius;
    point.z += arc->depth;
    njUnitMatrix(NULL);
    njInitCamera(&camera);
    camera.py += ARC_RISE;
    njSetCamera(&camera);
    njProjectScreen(NULL, &point, &screen);
    out->x = screen.x;
    out->y = screen.y;
}
