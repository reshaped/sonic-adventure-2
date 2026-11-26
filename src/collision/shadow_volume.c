/* the shadow's modifier volume, an eight-sided funnel. its top, at y = 0, is
 * an octagon of radius 1. its bottom, at y = -50, is one of radius 0.1 */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/surface.h"

static Sint16 faces[89] = {
#include "collision/shadow_volume/faces.inc"
};

struct vertex_list {
    Sint32 head[2];
    NJS_POINT3 points[16];
    Sint32 end;
};
static struct vertex_list vertices = {
#include "collision/shadow_volume/vertices.inc"
};

NJS_CNK_MODEL shadow_model[1] = {
#include "collision/shadow_volume/shadow_model.inc"
};

NJS_CNK_OBJECT shadow_object = {
#include "collision/shadow_volume/shadow_object.inc"
};
