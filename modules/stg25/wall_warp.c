/* Death Chamber's wall warp, WARP. warp_task_impl.h also implements the
 * ground warp. */
#include "stg25/warp_task.h"
#include "stg25/wall_warp/text.inc"

static NJS_TEXNAME chamber_warp_texture_names[1] = {
#include "stg25/wall_warp/chamber_warp_texture_names.inc"
};

NJS_TEXLIST chamber_warp_texlist = {
#include "stg25/wall_warp/chamber_warp_texlist.inc"
};

static Sint16 chamber_warp_polygons[77] = {
#include "stg25/wall_warp/chamber_warp_polygons.inc"
};

static Sint32 chamber_warp_vertices[35] = {
#include "stg25/wall_warp/chamber_warp_vertices.inc"
};

NJS_CNK_MODEL chamber_warp_model = {
#include "stg25/wall_warp/chamber_warp_model.inc"
};

static NJS_CNK_OBJECT chamber_warp_object = {
#include "stg25/wall_warp/chamber_warp_object.inc"
};
NJS_TEXLIST *chamber_wall_warp_texlists[2] = {
#include "stg25/wall_warp/chamber_wall_warp_texlists.inc"
};
/* the box at a size of SIZE_UNIT */
/* clang-format off */
static struct collision_shape chamber_warp_shapes[WARP_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000, {0.0F, -10.0F, 0.0F},
     10.0F, 10.0F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static Float chamber_warp_sizes[WARP_SIZES] = {60.0F, 50.0F, 40.0F};

#include "stg25/warp_task_impl.h" /* IWYU pragma: keep -- the code it compiles here */
