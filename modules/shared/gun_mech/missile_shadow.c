/* the modifier volume that mech_boss_missile.c draws as a missile's shadow */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 missile_shadow_polygons[167] = {
#include "shared/gun_mech/missile_shadow/missile_shadow_polygons.inc"
};

struct missile_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 points[29];
    Sint32 end;
};
static struct missile_shadow_vertex_list missile_shadow_vertices = {
#include "shared/gun_mech/missile_shadow/missile_shadow_vertices.inc"
};

static NJS_CNK_MODEL missile_shadow_mesh = {
#include "shared/gun_mech/missile_shadow/missile_shadow_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_missile_shadow = {
#include "shared/gun_mech/missile_shadow/mech_boss_missile_shadow.inc"
};
