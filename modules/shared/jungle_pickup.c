/* the jungle pickup, PIC in a stage's object list: an object a player picks
 * up and holds */
#include "shared/jungle_pickup/text.inc"
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "shared/jungle_pickup.h"

static NJS_TEXNAME jungle_pickup_textures[2] = {
#include "shared/jungle_pickup/jungle_pickup_textures.inc"
};

static NJS_TEXLIST jungle_pickup_texlist = {
#include "shared/jungle_pickup/jungle_pickup_texlist.inc"
};

static Sint16 jungle_pickup_polygons[231] = {
#include "shared/jungle_pickup/jungle_pickup_polygons.inc"
};

static Sint32 jungle_pickup_vertices[131] = {
#include "shared/jungle_pickup/jungle_pickup_vertices.inc"
};

static NJS_CNK_MODEL jungle_pickup_model = {
#include "shared/jungle_pickup/jungle_pickup_model.inc"
};

static NJS_CNK_OBJECT jungle_pickup_object = {
#include "shared/jungle_pickup/jungle_pickup_object.inc"
};

NJS_TEXLIST *jungle_pickup_texlists[2] = {
#include "shared/jungle_pickup/jungle_pickup_texlists.inc"
};
/* how far a holder's draw turns the nodes it holds the pickup with, in Angle
 * units. holdable_init keeps it in the motion record's hold_turn */
static Sint32 jungle_pickup_hold_angle = 0x1800;

void jungle_pickup_init(struct task *task)
{
    holdable_init(task, &jungle_pickup_texlist, &jungle_pickup_model,
                  jungle_pickup_hold_angle, HOLDABLE_PUSHES | OBJECT_MAY_LEAVE);
}
