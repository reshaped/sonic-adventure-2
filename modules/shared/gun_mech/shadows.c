/* the modifier volumes of the mech bosses' shadows: the body's and a foot's.
 * Big Foot and Hot Shot draw the foot's under each foot, and Flying Dog draws
 * only the body's */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 body_shadow_polygons[281] = {
#include "shared/gun_mech/shadows/body_shadow_polygons.inc"
};

struct body_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 points[52];
    Sint32 end;
};
static struct body_shadow_vertex_list body_shadow_vertices = {
#include "shared/gun_mech/shadows/body_shadow_vertices.inc"
};

static NJS_CNK_MODEL body_shadow_mesh = {
#include "shared/gun_mech/shadows/body_shadow_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_body_shadow = {
#include "shared/gun_mech/shadows/mech_boss_body_shadow.inc"
};

static Sint16 foot_shadow_polygons[119] = {
#include "shared/gun_mech/shadows/foot_shadow_polygons.inc"
};

struct foot_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 points[21];
    Sint32 end;
};
static struct foot_shadow_vertex_list foot_shadow_vertices = {
#include "shared/gun_mech/shadows/foot_shadow_vertices.inc"
};

static NJS_CNK_MODEL foot_shadow_mesh = {
#include "shared/gun_mech/shadows/foot_shadow_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_foot_shadow = {
#include "shared/gun_mech/shadows/mech_boss_foot_shadow.inc"
};
