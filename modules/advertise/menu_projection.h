/* the menus' projections: where a point on a circle about the y axis lands
 * on the screen */
#ifndef ADVERTISE_MENU_PROJECTION_H
#define ADVERTISE_MENU_PROJECTION_H
#include <ninja.h>
#include <sg_xpt.h>

/* a panel that swings on to the screen from a circle about the y axis */
struct menu_panel {
    NJS_POINT3 pos; /* where the panel lands. only x and y are read */
    Angle roll; /* the circle's turn about the z axis */
    Float depth; /* the circle's move along z, after roll */
    Float radius;
};
/* a circle about the y axis, moved depth along z */
struct menu_arc {
    Float depth;
    Float radius;
};

/* set out's x and y to where panel stands at angle, in degrees from 0 to
 * 180. the panel's point on the circle lies angle degrees from z toward x.
 * the point projects to the screen through njInitCamera's camera, with the
 * camera's py raised by 2. out lies angle / 180 of the way from that
 * projection to pos */
void menu_panel_point(const struct menu_panel *panel, NJS_POINT3 *out,
                      Int angle);
/* set out's x and y to where arc's point at angle lands on the screen. the
 * point lies angle degrees from z toward x. it projects through
 * njInitCamera's camera, with the camera's py raised by 0.8 */
void menu_arc_point(const struct menu_arc *arc, NJS_POINT3 *out, Int angle);

#endif
