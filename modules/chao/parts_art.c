/* CHAO.PRS's chunk models, chunk object pointers and chunk object tables,
 * from the disc. its D lies between parts.c and course.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/parts_art.h"
#include "object/animal.h"

static NJS_CNK_OBJECT adult_raccoon_arm_3_child = {
#include "chao/parts_art/adult_raccoon_arm_3_child.inc"
};
static Sint16 adult_raccoon_arm_3_polygons[179] = {
#include "chao/parts_art/adult_raccoon_arm_3_polygons.inc"
};
static Sint32 adult_raccoon_arm_3_vertices[147] = {
#include "chao/parts_art/adult_raccoon_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_arm_3_model = {
#include "chao/parts_art/adult_raccoon_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_arm_3 = {
#include "chao/parts_art/adult_raccoon_arm_3.inc"
};
static NJS_CNK_OBJECT adult_raccoon_arm_3_mount = {
#include "chao/parts_art/adult_raccoon_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_raccoon_arm_10_child = {
#include "chao/parts_art/adult_raccoon_arm_10_child.inc"
};
static Sint16 adult_raccoon_arm_10_polygons[179] = {
#include "chao/parts_art/adult_raccoon_arm_10_polygons.inc"
};
static Sint32 adult_raccoon_arm_10_vertices[147] = {
#include "chao/parts_art/adult_raccoon_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_arm_10_model = {
#include "chao/parts_art/adult_raccoon_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_arm_10 = {
#include "chao/parts_art/adult_raccoon_arm_10.inc"
};
static NJS_CNK_OBJECT adult_raccoon_arm_10_mount = {
#include "chao/parts_art/adult_raccoon_arm_10_mount.inc"
};
static Sint16 adult_raccoon_ear_24_polygons[171] = {
#include "chao/parts_art/adult_raccoon_ear_24_polygons.inc"
};
static Sint32 adult_raccoon_ear_24_vertices[123] = {
#include "chao/parts_art/adult_raccoon_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_ear_24_model = {
#include "chao/parts_art/adult_raccoon_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_ear_24 = {
#include "chao/parts_art/adult_raccoon_ear_24.inc"
};
static NJS_CNK_OBJECT adult_raccoon_ear_24_mount = {
#include "chao/parts_art/adult_raccoon_ear_24_mount.inc"
};
static Sint16 adult_raccoon_ear_26_polygons[171] = {
#include "chao/parts_art/adult_raccoon_ear_26_polygons.inc"
};
static Sint32 adult_raccoon_ear_26_vertices[123] = {
#include "chao/parts_art/adult_raccoon_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_ear_26_model = {
#include "chao/parts_art/adult_raccoon_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_ear_26 = {
#include "chao/parts_art/adult_raccoon_ear_26.inc"
};
static NJS_CNK_OBJECT adult_raccoon_ear_26_mount = {
#include "chao/parts_art/adult_raccoon_ear_26_mount.inc"
};
static Sint16 adult_raccoon_leg_6_polygons[161] = {
#include "chao/parts_art/adult_raccoon_leg_6_polygons.inc"
};
static Sint32 adult_raccoon_leg_6_vertices[129] = {
#include "chao/parts_art/adult_raccoon_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_leg_6_model = {
#include "chao/parts_art/adult_raccoon_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_leg_6 = {
#include "chao/parts_art/adult_raccoon_leg_6.inc"
};
static NJS_CNK_OBJECT adult_raccoon_leg_6_mount = {
#include "chao/parts_art/adult_raccoon_leg_6_mount.inc"
};
static Sint16 adult_raccoon_leg_13_polygons[161] = {
#include "chao/parts_art/adult_raccoon_leg_13_polygons.inc"
};
static Sint32 adult_raccoon_leg_13_vertices[129] = {
#include "chao/parts_art/adult_raccoon_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_leg_13_model = {
#include "chao/parts_art/adult_raccoon_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_leg_13 = {
#include "chao/parts_art/adult_raccoon_leg_13.inc"
};
static NJS_CNK_OBJECT adult_raccoon_leg_13_mount = {
#include "chao/parts_art/adult_raccoon_leg_13_mount.inc"
};
static Sint16 adult_raccoon_unused_part_polygons[183] = {
#include "chao/parts_art/adult_raccoon_unused_part_polygons.inc"
};
static Sint32 adult_raccoon_unused_part_vertices[129] = {
#include "chao/parts_art/adult_raccoon_unused_part_vertices.inc"
};
static NJS_CNK_MODEL adult_raccoon_unused_part_model = {
#include "chao/parts_art/adult_raccoon_unused_part_model.inc"
};
static NJS_CNK_OBJECT adult_raccoon_unused_part = {
#include "chao/parts_art/adult_raccoon_unused_part.inc"
};
static NJS_CNK_OBJECT adult_raccoon_unused_part_mount = {
#include "chao/parts_art/adult_raccoon_unused_part_mount.inc"
};
static NJS_CNK_OBJECT adult_cheetah_arm_3_child = {
#include "chao/parts_art/adult_cheetah_arm_3_child.inc"
};
static Sint16 adult_cheetah_arm_3_polygons[255] = {
#include "chao/parts_art/adult_cheetah_arm_3_polygons.inc"
};
static Sint32 adult_cheetah_arm_3_vertices[207] = {
#include "chao/parts_art/adult_cheetah_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_arm_3_model = {
#include "chao/parts_art/adult_cheetah_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_arm_3 = {
#include "chao/parts_art/adult_cheetah_arm_3.inc"
};
static NJS_CNK_OBJECT adult_cheetah_arm_3_mount = {
#include "chao/parts_art/adult_cheetah_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_cheetah_arm_10_child = {
#include "chao/parts_art/adult_cheetah_arm_10_child.inc"
};
static Sint16 adult_cheetah_arm_10_polygons[255] = {
#include "chao/parts_art/adult_cheetah_arm_10_polygons.inc"
};
static Sint32 adult_cheetah_arm_10_vertices[207] = {
#include "chao/parts_art/adult_cheetah_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_arm_10_model = {
#include "chao/parts_art/adult_cheetah_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_arm_10 = {
#include "chao/parts_art/adult_cheetah_arm_10.inc"
};
static NJS_CNK_OBJECT adult_cheetah_arm_10_mount = {
#include "chao/parts_art/adult_cheetah_arm_10_mount.inc"
};
static Sint16 adult_cheetah_ear_24_polygons[149] = {
#include "chao/parts_art/adult_cheetah_ear_24_polygons.inc"
};
static Sint32 adult_cheetah_ear_24_vertices[105] = {
#include "chao/parts_art/adult_cheetah_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_ear_24_model = {
#include "chao/parts_art/adult_cheetah_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_ear_24 = {
#include "chao/parts_art/adult_cheetah_ear_24.inc"
};
static NJS_CNK_OBJECT adult_cheetah_ear_24_mount = {
#include "chao/parts_art/adult_cheetah_ear_24_mount.inc"
};
static Sint16 adult_cheetah_ear_26_polygons[149] = {
#include "chao/parts_art/adult_cheetah_ear_26_polygons.inc"
};
static Sint32 adult_cheetah_ear_26_vertices[105] = {
#include "chao/parts_art/adult_cheetah_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_ear_26_model = {
#include "chao/parts_art/adult_cheetah_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_ear_26 = {
#include "chao/parts_art/adult_cheetah_ear_26.inc"
};
static NJS_CNK_OBJECT adult_cheetah_ear_26_mount = {
#include "chao/parts_art/adult_cheetah_ear_26_mount.inc"
};
static Sint16 adult_cheetah_leg_6_polygons[213] = {
#include "chao/parts_art/adult_cheetah_leg_6_polygons.inc"
};
static Sint32 adult_cheetah_leg_6_vertices[189] = {
#include "chao/parts_art/adult_cheetah_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_leg_6_model = {
#include "chao/parts_art/adult_cheetah_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_leg_6 = {
#include "chao/parts_art/adult_cheetah_leg_6.inc"
};
static NJS_CNK_OBJECT adult_cheetah_leg_6_mount = {
#include "chao/parts_art/adult_cheetah_leg_6_mount.inc"
};
static Sint16 adult_cheetah_leg_13_polygons[213] = {
#include "chao/parts_art/adult_cheetah_leg_13_polygons.inc"
};
static Sint32 adult_cheetah_leg_13_vertices[189] = {
#include "chao/parts_art/adult_cheetah_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_leg_13_model = {
#include "chao/parts_art/adult_cheetah_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_leg_13 = {
#include "chao/parts_art/adult_cheetah_leg_13.inc"
};
static NJS_CNK_OBJECT adult_cheetah_leg_13_mount = {
#include "chao/parts_art/adult_cheetah_leg_13_mount.inc"
};
static Sint16 adult_cheetah_tail_polygons[269] = {
#include "chao/parts_art/adult_cheetah_tail_polygons.inc"
};
static Sint32 adult_cheetah_tail_vertices[153] = {
#include "chao/parts_art/adult_cheetah_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_cheetah_tail_model = {
#include "chao/parts_art/adult_cheetah_tail_model.inc"
};
static NJS_CNK_OBJECT adult_cheetah_tail = {
#include "chao/parts_art/adult_cheetah_tail.inc"
};
static NJS_CNK_OBJECT adult_cheetah_tail_mount = {
#include "chao/parts_art/adult_cheetah_tail_mount.inc"
};
static NJS_CNK_OBJECT adult_dragon_arm_3_child = {
#include "chao/parts_art/adult_dragon_arm_3_child.inc"
};
static Sint16 adult_dragon_arm_3_polygons[211] = {
#include "chao/parts_art/adult_dragon_arm_3_polygons.inc"
};
static Sint32 adult_dragon_arm_3_vertices[141] = {
#include "chao/parts_art/adult_dragon_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_arm_3_model = {
#include "chao/parts_art/adult_dragon_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_arm_3 = {
#include "chao/parts_art/adult_dragon_arm_3.inc"
};
static NJS_CNK_OBJECT adult_dragon_arm_3_mount = {
#include "chao/parts_art/adult_dragon_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_dragon_arm_10_child = {
#include "chao/parts_art/adult_dragon_arm_10_child.inc"
};
static Sint16 adult_dragon_arm_10_polygons[211] = {
#include "chao/parts_art/adult_dragon_arm_10_polygons.inc"
};
static Sint32 adult_dragon_arm_10_vertices[141] = {
#include "chao/parts_art/adult_dragon_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_arm_10_model = {
#include "chao/parts_art/adult_dragon_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_arm_10 = {
#include "chao/parts_art/adult_dragon_arm_10.inc"
};
static NJS_CNK_OBJECT adult_dragon_arm_10_mount = {
#include "chao/parts_art/adult_dragon_arm_10_mount.inc"
};
static Sint16 adult_dragon_ear_24_polygons[143] = {
#include "chao/parts_art/adult_dragon_ear_24_polygons.inc"
};
static Sint32 adult_dragon_ear_24_vertices[99] = {
#include "chao/parts_art/adult_dragon_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_ear_24_model = {
#include "chao/parts_art/adult_dragon_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_ear_24 = {
#include "chao/parts_art/adult_dragon_ear_24.inc"
};
static NJS_CNK_OBJECT adult_dragon_ear_24_mount = {
#include "chao/parts_art/adult_dragon_ear_24_mount.inc"
};
static Sint16 adult_dragon_ear_26_polygons[143] = {
#include "chao/parts_art/adult_dragon_ear_26_polygons.inc"
};
static Sint32 adult_dragon_ear_26_vertices[99] = {
#include "chao/parts_art/adult_dragon_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_ear_26_model = {
#include "chao/parts_art/adult_dragon_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_ear_26 = {
#include "chao/parts_art/adult_dragon_ear_26.inc"
};
static NJS_CNK_OBJECT adult_dragon_ear_26_mount = {
#include "chao/parts_art/adult_dragon_ear_26_mount.inc"
};
static Sint16 adult_dragon_horn_30_polygons[137] = {
#include "chao/parts_art/adult_dragon_horn_30_polygons.inc"
};
static Sint32 adult_dragon_horn_30_vertices[93] = {
#include "chao/parts_art/adult_dragon_horn_30_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_horn_30_model = {
#include "chao/parts_art/adult_dragon_horn_30_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_horn_30 = {
#include "chao/parts_art/adult_dragon_horn_30.inc"
};
static NJS_CNK_OBJECT adult_dragon_horn_30_mount = {
#include "chao/parts_art/adult_dragon_horn_30_mount.inc"
};
static Sint16 adult_dragon_horn_31_polygons[137] = {
#include "chao/parts_art/adult_dragon_horn_31_polygons.inc"
};
static Sint32 adult_dragon_horn_31_vertices[93] = {
#include "chao/parts_art/adult_dragon_horn_31_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_horn_31_model = {
#include "chao/parts_art/adult_dragon_horn_31_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_horn_31 = {
#include "chao/parts_art/adult_dragon_horn_31.inc"
};
static NJS_CNK_OBJECT adult_dragon_horn_31_mount = {
#include "chao/parts_art/adult_dragon_horn_31_mount.inc"
};
static Sint16 adult_dragon_leg_6_polygons[191] = {
#include "chao/parts_art/adult_dragon_leg_6_polygons.inc"
};
static Sint32 adult_dragon_leg_6_vertices[135] = {
#include "chao/parts_art/adult_dragon_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_leg_6_model = {
#include "chao/parts_art/adult_dragon_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_leg_6 = {
#include "chao/parts_art/adult_dragon_leg_6.inc"
};
static NJS_CNK_OBJECT adult_dragon_leg_6_mount = {
#include "chao/parts_art/adult_dragon_leg_6_mount.inc"
};
static Sint16 adult_dragon_leg_13_polygons[181] = {
#include "chao/parts_art/adult_dragon_leg_13_polygons.inc"
};
static Sint32 adult_dragon_leg_13_vertices[135] = {
#include "chao/parts_art/adult_dragon_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_leg_13_model = {
#include "chao/parts_art/adult_dragon_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_leg_13 = {
#include "chao/parts_art/adult_dragon_leg_13.inc"
};
static NJS_CNK_OBJECT adult_dragon_leg_13_mount = {
#include "chao/parts_art/adult_dragon_leg_13_mount.inc"
};
static Sint16 adult_dragon_tail_polygons[241] = {
#include "chao/parts_art/adult_dragon_tail_polygons.inc"
};
static Sint32 adult_dragon_tail_vertices[177] = {
#include "chao/parts_art/adult_dragon_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_tail_model = {
#include "chao/parts_art/adult_dragon_tail_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_tail = {
#include "chao/parts_art/adult_dragon_tail.inc"
};
static NJS_CNK_OBJECT adult_dragon_tail_mount = {
#include "chao/parts_art/adult_dragon_tail_mount.inc"
};
static Sint16 adult_dragon_wing_37_polygons[177] = {
#include "chao/parts_art/adult_dragon_wing_37_polygons.inc"
};
static Sint32 adult_dragon_wing_37_vertices[165] = {
#include "chao/parts_art/adult_dragon_wing_37_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_wing_37_model = {
#include "chao/parts_art/adult_dragon_wing_37_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_wing_37 = {
#include "chao/parts_art/adult_dragon_wing_37.inc"
};
static NJS_CNK_OBJECT adult_dragon_wing_37_mount = {
#include "chao/parts_art/adult_dragon_wing_37_mount.inc"
};
static Sint16 adult_dragon_wing_39_polygons[169] = {
#include "chao/parts_art/adult_dragon_wing_39_polygons.inc"
};
static Sint32 adult_dragon_wing_39_vertices[165] = {
#include "chao/parts_art/adult_dragon_wing_39_vertices.inc"
};
static NJS_CNK_MODEL adult_dragon_wing_39_model = {
#include "chao/parts_art/adult_dragon_wing_39_model.inc"
};
static NJS_CNK_OBJECT adult_dragon_wing_39 = {
#include "chao/parts_art/adult_dragon_wing_39.inc"
};
static NJS_CNK_OBJECT adult_dragon_wing_39_mount = {
#include "chao/parts_art/adult_dragon_wing_39_mount.inc"
};
static NJS_CNK_OBJECT adult_seal_arm_3_child = {
#include "chao/parts_art/adult_seal_arm_3_child.inc"
};
static Sint16 adult_seal_arm_3_polygons[169] = {
#include "chao/parts_art/adult_seal_arm_3_polygons.inc"
};
static Sint32 adult_seal_arm_3_vertices[123] = {
#include "chao/parts_art/adult_seal_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_seal_arm_3_model = {
#include "chao/parts_art/adult_seal_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_seal_arm_3 = {
#include "chao/parts_art/adult_seal_arm_3.inc"
};
static NJS_CNK_OBJECT adult_seal_arm_3_mount = {
#include "chao/parts_art/adult_seal_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_seal_arm_10_child = {
#include "chao/parts_art/adult_seal_arm_10_child.inc"
};
static Sint16 adult_seal_arm_10_polygons[169] = {
#include "chao/parts_art/adult_seal_arm_10_polygons.inc"
};
static Sint32 adult_seal_arm_10_vertices[123] = {
#include "chao/parts_art/adult_seal_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_seal_arm_10_model = {
#include "chao/parts_art/adult_seal_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_seal_arm_10 = {
#include "chao/parts_art/adult_seal_arm_10.inc"
};
static NJS_CNK_OBJECT adult_seal_arm_10_mount = {
#include "chao/parts_art/adult_seal_arm_10_mount.inc"
};
static Sint16 adult_seal_tail_polygons[181] = {
#include "chao/parts_art/adult_seal_tail_polygons.inc"
};
static Sint32 adult_seal_tail_vertices[135] = {
#include "chao/parts_art/adult_seal_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_seal_tail_model = {
#include "chao/parts_art/adult_seal_tail_model.inc"
};
static NJS_CNK_OBJECT adult_seal_tail = {
#include "chao/parts_art/adult_seal_tail.inc"
};
static NJS_CNK_OBJECT adult_seal_tail_mount = {
#include "chao/parts_art/adult_seal_tail_mount.inc"
};
static NJS_CNK_OBJECT adult_gorilla_arm_3_child = {
#include "chao/parts_art/adult_gorilla_arm_3_child.inc"
};
static Sint16 adult_gorilla_arm_3_polygons[215] = {
#include "chao/parts_art/adult_gorilla_arm_3_polygons.inc"
};
static Sint32 adult_gorilla_arm_3_vertices[141] = {
#include "chao/parts_art/adult_gorilla_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_arm_3_model = {
#include "chao/parts_art/adult_gorilla_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_arm_3 = {
#include "chao/parts_art/adult_gorilla_arm_3.inc"
};
static NJS_CNK_OBJECT adult_gorilla_arm_3_mount = {
#include "chao/parts_art/adult_gorilla_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_gorilla_arm_10_child = {
#include "chao/parts_art/adult_gorilla_arm_10_child.inc"
};
static Sint16 adult_gorilla_arm_10_polygons[215] = {
#include "chao/parts_art/adult_gorilla_arm_10_polygons.inc"
};
static Sint32 adult_gorilla_arm_10_vertices[141] = {
#include "chao/parts_art/adult_gorilla_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_arm_10_model = {
#include "chao/parts_art/adult_gorilla_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_arm_10 = {
#include "chao/parts_art/adult_gorilla_arm_10.inc"
};
static NJS_CNK_OBJECT adult_gorilla_arm_10_mount = {
#include "chao/parts_art/adult_gorilla_arm_10_mount.inc"
};
static Sint16 adult_gorilla_ear_24_polygons[157] = {
#include "chao/parts_art/adult_gorilla_ear_24_polygons.inc"
};
static Sint32 adult_gorilla_ear_24_vertices[105] = {
#include "chao/parts_art/adult_gorilla_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_ear_24_model = {
#include "chao/parts_art/adult_gorilla_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_ear_24 = {
#include "chao/parts_art/adult_gorilla_ear_24.inc"
};
static NJS_CNK_OBJECT adult_gorilla_ear_24_mount = {
#include "chao/parts_art/adult_gorilla_ear_24_mount.inc"
};
static Sint16 adult_gorilla_ear_26_polygons[157] = {
#include "chao/parts_art/adult_gorilla_ear_26_polygons.inc"
};
static Sint32 adult_gorilla_ear_26_vertices[105] = {
#include "chao/parts_art/adult_gorilla_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_ear_26_model = {
#include "chao/parts_art/adult_gorilla_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_ear_26 = {
#include "chao/parts_art/adult_gorilla_ear_26.inc"
};
static NJS_CNK_OBJECT adult_gorilla_ear_26_mount = {
#include "chao/parts_art/adult_gorilla_ear_26_mount.inc"
};
static Sint16 adult_gorilla_forehead_polygons[161] = {
#include "chao/parts_art/adult_gorilla_forehead_polygons.inc"
};
static Sint32 adult_gorilla_forehead_vertices[165] = {
#include "chao/parts_art/adult_gorilla_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_forehead_model = {
#include "chao/parts_art/adult_gorilla_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_forehead = {
#include "chao/parts_art/adult_gorilla_forehead.inc"
};
static NJS_CNK_OBJECT adult_gorilla_forehead_mount = {
#include "chao/parts_art/adult_gorilla_forehead_mount.inc"
};
static Sint16 adult_gorilla_leg_6_polygons[181] = {
#include "chao/parts_art/adult_gorilla_leg_6_polygons.inc"
};
static Sint32 adult_gorilla_leg_6_vertices[129] = {
#include "chao/parts_art/adult_gorilla_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_leg_6_model = {
#include "chao/parts_art/adult_gorilla_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_leg_6 = {
#include "chao/parts_art/adult_gorilla_leg_6.inc"
};
static NJS_CNK_OBJECT adult_gorilla_leg_6_mount = {
#include "chao/parts_art/adult_gorilla_leg_6_mount.inc"
};
static Sint16 adult_gorilla_leg_13_polygons[181] = {
#include "chao/parts_art/adult_gorilla_leg_13_polygons.inc"
};
static Sint32 adult_gorilla_leg_13_vertices[129] = {
#include "chao/parts_art/adult_gorilla_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_gorilla_leg_13_model = {
#include "chao/parts_art/adult_gorilla_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_gorilla_leg_13 = {
#include "chao/parts_art/adult_gorilla_leg_13.inc"
};
static NJS_CNK_OBJECT adult_gorilla_leg_13_mount = {
#include "chao/parts_art/adult_gorilla_leg_13_mount.inc"
};
static NJS_CNK_OBJECT adult_boar_arm_3_child = {
#include "chao/parts_art/adult_boar_arm_3_child.inc"
};
static Sint16 adult_boar_arm_3_polygons[317] = {
#include "chao/parts_art/adult_boar_arm_3_polygons.inc"
};
static Sint32 adult_boar_arm_3_vertices[159] = {
#include "chao/parts_art/adult_boar_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_arm_3_model = {
#include "chao/parts_art/adult_boar_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_boar_arm_3 = {
#include "chao/parts_art/adult_boar_arm_3.inc"
};
static NJS_CNK_OBJECT adult_boar_arm_3_mount = {
#include "chao/parts_art/adult_boar_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_boar_arm_10_child = {
#include "chao/parts_art/adult_boar_arm_10_child.inc"
};
static Sint16 adult_boar_arm_10_polygons[309] = {
#include "chao/parts_art/adult_boar_arm_10_polygons.inc"
};
static Sint32 adult_boar_arm_10_vertices[159] = {
#include "chao/parts_art/adult_boar_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_arm_10_model = {
#include "chao/parts_art/adult_boar_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_boar_arm_10 = {
#include "chao/parts_art/adult_boar_arm_10.inc"
};
static NJS_CNK_OBJECT adult_boar_arm_10_mount = {
#include "chao/parts_art/adult_boar_arm_10_mount.inc"
};
static Sint16 adult_boar_ear_24_polygons[121] = {
#include "chao/parts_art/adult_boar_ear_24_polygons.inc"
};
static Sint32 adult_boar_ear_24_vertices[81] = {
#include "chao/parts_art/adult_boar_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_ear_24_model = {
#include "chao/parts_art/adult_boar_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_boar_ear_24 = {
#include "chao/parts_art/adult_boar_ear_24.inc"
};
static NJS_CNK_OBJECT adult_boar_ear_24_mount = {
#include "chao/parts_art/adult_boar_ear_24_mount.inc"
};
static Sint16 adult_boar_ear_26_polygons[135] = {
#include "chao/parts_art/adult_boar_ear_26_polygons.inc"
};
static Sint32 adult_boar_ear_26_vertices[81] = {
#include "chao/parts_art/adult_boar_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_ear_26_model = {
#include "chao/parts_art/adult_boar_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_boar_ear_26 = {
#include "chao/parts_art/adult_boar_ear_26.inc"
};
static NJS_CNK_OBJECT adult_boar_ear_26_mount = {
#include "chao/parts_art/adult_boar_ear_26_mount.inc"
};
static Sint16 adult_boar_leg_6_polygons[269] = {
#include "chao/parts_art/adult_boar_leg_6_polygons.inc"
};
static Sint32 adult_boar_leg_6_vertices[165] = {
#include "chao/parts_art/adult_boar_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_leg_6_model = {
#include "chao/parts_art/adult_boar_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_boar_leg_6 = {
#include "chao/parts_art/adult_boar_leg_6.inc"
};
static NJS_CNK_OBJECT adult_boar_leg_6_mount = {
#include "chao/parts_art/adult_boar_leg_6_mount.inc"
};
static Sint16 adult_boar_leg_13_polygons[269] = {
#include "chao/parts_art/adult_boar_leg_13_polygons.inc"
};
static Sint32 adult_boar_leg_13_vertices[165] = {
#include "chao/parts_art/adult_boar_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_leg_13_model = {
#include "chao/parts_art/adult_boar_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_boar_leg_13 = {
#include "chao/parts_art/adult_boar_leg_13.inc"
};
static NJS_CNK_OBJECT adult_boar_leg_13_mount = {
#include "chao/parts_art/adult_boar_leg_13_mount.inc"
};
static Sint16 adult_boar_tail_polygons[207] = {
#include "chao/parts_art/adult_boar_tail_polygons.inc"
};
static Sint32 adult_boar_tail_vertices[117] = {
#include "chao/parts_art/adult_boar_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_tail_model = {
#include "chao/parts_art/adult_boar_tail_model.inc"
};
static NJS_CNK_OBJECT adult_boar_tail = {
#include "chao/parts_art/adult_boar_tail.inc"
};
static NJS_CNK_OBJECT adult_boar_tail_mount = {
#include "chao/parts_art/adult_boar_tail_mount.inc"
};
static Sint16 adult_boar_face_polygons[181] = {
#include "chao/parts_art/adult_boar_face_polygons.inc"
};
static Sint32 adult_boar_face_vertices[171] = {
#include "chao/parts_art/adult_boar_face_vertices.inc"
};
static NJS_CNK_MODEL adult_boar_face_model = {
#include "chao/parts_art/adult_boar_face_model.inc"
};
static NJS_CNK_OBJECT adult_boar_face = {
#include "chao/parts_art/adult_boar_face.inc"
};
static NJS_CNK_OBJECT adult_boar_face_mount = {
#include "chao/parts_art/adult_boar_face_mount.inc"
};
static Sint16 adult_condor_ear_24_polygons[171] = {
#include "chao/parts_art/adult_condor_ear_24_polygons.inc"
};
static Sint32 adult_condor_ear_24_vertices[129] = {
#include "chao/parts_art/adult_condor_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_ear_24_model = {
#include "chao/parts_art/adult_condor_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_condor_ear_24 = {
#include "chao/parts_art/adult_condor_ear_24.inc"
};
static NJS_CNK_OBJECT adult_condor_ear_24_mount = {
#include "chao/parts_art/adult_condor_ear_24_mount.inc"
};
static Sint16 adult_condor_ear_26_polygons[171] = {
#include "chao/parts_art/adult_condor_ear_26_polygons.inc"
};
static Sint32 adult_condor_ear_26_vertices[129] = {
#include "chao/parts_art/adult_condor_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_ear_26_model = {
#include "chao/parts_art/adult_condor_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_condor_ear_26 = {
#include "chao/parts_art/adult_condor_ear_26.inc"
};
static NJS_CNK_OBJECT adult_condor_ear_26_mount = {
#include "chao/parts_art/adult_condor_ear_26_mount.inc"
};
static Sint16 adult_condor_leg_6_polygons[187] = {
#include "chao/parts_art/adult_condor_leg_6_polygons.inc"
};
static Sint32 adult_condor_leg_6_vertices[129] = {
#include "chao/parts_art/adult_condor_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_leg_6_model = {
#include "chao/parts_art/adult_condor_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_condor_leg_6 = {
#include "chao/parts_art/adult_condor_leg_6.inc"
};
static NJS_CNK_OBJECT adult_condor_leg_6_mount = {
#include "chao/parts_art/adult_condor_leg_6_mount.inc"
};
static Sint16 adult_condor_leg_13_polygons[187] = {
#include "chao/parts_art/adult_condor_leg_13_polygons.inc"
};
static Sint32 adult_condor_leg_13_vertices[129] = {
#include "chao/parts_art/adult_condor_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_leg_13_model = {
#include "chao/parts_art/adult_condor_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_condor_leg_13 = {
#include "chao/parts_art/adult_condor_leg_13.inc"
};
static NJS_CNK_OBJECT adult_condor_leg_13_mount = {
#include "chao/parts_art/adult_condor_leg_13_mount.inc"
};
static Sint16 adult_condor_tail_polygons[203] = {
#include "chao/parts_art/adult_condor_tail_polygons.inc"
};
static Sint32 adult_condor_tail_vertices[93] = {
#include "chao/parts_art/adult_condor_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_tail_model = {
#include "chao/parts_art/adult_condor_tail_model.inc"
};
static NJS_CNK_OBJECT adult_condor_tail = {
#include "chao/parts_art/adult_condor_tail.inc"
};
static NJS_CNK_OBJECT adult_condor_tail_mount = {
#include "chao/parts_art/adult_condor_tail_mount.inc"
};
static Sint16 adult_condor_wing_37_polygons[127] = {
#include "chao/parts_art/adult_condor_wing_37_polygons.inc"
};
static Sint32 adult_condor_wing_37_vertices[87] = {
#include "chao/parts_art/adult_condor_wing_37_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_wing_37_model = {
#include "chao/parts_art/adult_condor_wing_37_model.inc"
};
static NJS_CNK_OBJECT adult_condor_wing_37 = {
#include "chao/parts_art/adult_condor_wing_37.inc"
};
static NJS_CNK_OBJECT adult_condor_wing_37_mount = {
#include "chao/parts_art/adult_condor_wing_37_mount.inc"
};
static Sint16 adult_condor_wing_39_polygons[127] = {
#include "chao/parts_art/adult_condor_wing_39_polygons.inc"
};
static Sint32 adult_condor_wing_39_vertices[87] = {
#include "chao/parts_art/adult_condor_wing_39_vertices.inc"
};
static NJS_CNK_MODEL adult_condor_wing_39_model = {
#include "chao/parts_art/adult_condor_wing_39_model.inc"
};
static NJS_CNK_OBJECT adult_condor_wing_39 = {
#include "chao/parts_art/adult_condor_wing_39.inc"
};
static NJS_CNK_OBJECT adult_condor_wing_39_mount = {
#include "chao/parts_art/adult_condor_wing_39_mount.inc"
};
static Sint16 adult_peacock_forehead_polygons[183] = {
#include "chao/parts_art/adult_peacock_forehead_polygons.inc"
};
static Sint32 adult_peacock_forehead_vertices[159] = {
#include "chao/parts_art/adult_peacock_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_peacock_forehead_model = {
#include "chao/parts_art/adult_peacock_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_peacock_forehead = {
#include "chao/parts_art/adult_peacock_forehead.inc"
};
static NJS_CNK_OBJECT adult_peacock_forehead_mount = {
#include "chao/parts_art/adult_peacock_forehead_mount.inc"
};
static Sint16 adult_peacock_leg_6_polygons[195] = {
#include "chao/parts_art/adult_peacock_leg_6_polygons.inc"
};
static Sint32 adult_peacock_leg_6_vertices[135] = {
#include "chao/parts_art/adult_peacock_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_peacock_leg_6_model = {
#include "chao/parts_art/adult_peacock_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_peacock_leg_6 = {
#include "chao/parts_art/adult_peacock_leg_6.inc"
};
static NJS_CNK_OBJECT adult_peacock_leg_6_mount = {
#include "chao/parts_art/adult_peacock_leg_6_mount.inc"
};
static Sint16 adult_peacock_leg_13_polygons[195] = {
#include "chao/parts_art/adult_peacock_leg_13_polygons.inc"
};
static Sint32 adult_peacock_leg_13_vertices[135] = {
#include "chao/parts_art/adult_peacock_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_peacock_leg_13_model = {
#include "chao/parts_art/adult_peacock_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_peacock_leg_13 = {
#include "chao/parts_art/adult_peacock_leg_13.inc"
};
static NJS_CNK_OBJECT adult_peacock_leg_13_mount = {
#include "chao/parts_art/adult_peacock_leg_13_mount.inc"
};
static Sint16 adult_peacock_tail_polygons[275] = {
#include "chao/parts_art/adult_peacock_tail_polygons.inc"
};
static Sint32 adult_peacock_tail_vertices[387] = {
#include "chao/parts_art/adult_peacock_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_peacock_tail_model = {
#include "chao/parts_art/adult_peacock_tail_model.inc"
};
static NJS_CNK_OBJECT adult_peacock_tail = {
#include "chao/parts_art/adult_peacock_tail.inc"
};
static NJS_CNK_OBJECT adult_peacock_tail_mount = {
#include "chao/parts_art/adult_peacock_tail_mount.inc"
};
static Sint16 adult_peacock_wing_37_polygons[119] = {
#include "chao/parts_art/adult_peacock_wing_37_polygons.inc"
};
static Sint32 adult_peacock_wing_37_vertices[93] = {
#include "chao/parts_art/adult_peacock_wing_37_vertices.inc"
};
static NJS_CNK_MODEL adult_peacock_wing_37_model = {
#include "chao/parts_art/adult_peacock_wing_37_model.inc"
};
static NJS_CNK_OBJECT adult_peacock_wing_37 = {
#include "chao/parts_art/adult_peacock_wing_37.inc"
};
static NJS_CNK_OBJECT adult_peacock_wing_37_mount = {
#include "chao/parts_art/adult_peacock_wing_37_mount.inc"
};
static Sint16 adult_peacock_wing_39_polygons[119] = {
#include "chao/parts_art/adult_peacock_wing_39_polygons.inc"
};
static Sint32 adult_peacock_wing_39_vertices[93] = {
#include "chao/parts_art/adult_peacock_wing_39_vertices.inc"
};
static NJS_CNK_MODEL adult_peacock_wing_39_model = {
#include "chao/parts_art/adult_peacock_wing_39_model.inc"
};
static NJS_CNK_OBJECT adult_peacock_wing_39 = {
#include "chao/parts_art/adult_peacock_wing_39.inc"
};
static NJS_CNK_OBJECT adult_peacock_wing_39_mount = {
#include "chao/parts_art/adult_peacock_wing_39_mount.inc"
};
static NJS_CNK_OBJECT adult_bear_arm_3_child = {
#include "chao/parts_art/adult_bear_arm_3_child.inc"
};
static Sint16 adult_bear_arm_3_polygons[237] = {
#include "chao/parts_art/adult_bear_arm_3_polygons.inc"
};
static Sint32 adult_bear_arm_3_vertices[183] = {
#include "chao/parts_art/adult_bear_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_bear_arm_3_model = {
#include "chao/parts_art/adult_bear_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_bear_arm_3 = {
#include "chao/parts_art/adult_bear_arm_3.inc"
};
static NJS_CNK_OBJECT adult_bear_arm_3_mount = {
#include "chao/parts_art/adult_bear_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_bear_arm_10_child = {
#include "chao/parts_art/adult_bear_arm_10_child.inc"
};
static Sint16 adult_bear_arm_10_polygons[237] = {
#include "chao/parts_art/adult_bear_arm_10_polygons.inc"
};
static Sint32 adult_bear_arm_10_vertices[183] = {
#include "chao/parts_art/adult_bear_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_bear_arm_10_model = {
#include "chao/parts_art/adult_bear_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_bear_arm_10 = {
#include "chao/parts_art/adult_bear_arm_10.inc"
};
static NJS_CNK_OBJECT adult_bear_arm_10_mount = {
#include "chao/parts_art/adult_bear_arm_10_mount.inc"
};
static Sint16 adult_bear_ear_24_polygons[157] = {
#include "chao/parts_art/adult_bear_ear_24_polygons.inc"
};
static Sint32 adult_bear_ear_24_vertices[111] = {
#include "chao/parts_art/adult_bear_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_bear_ear_24_model = {
#include "chao/parts_art/adult_bear_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_bear_ear_24 = {
#include "chao/parts_art/adult_bear_ear_24.inc"
};
static NJS_CNK_OBJECT adult_bear_ear_24_mount = {
#include "chao/parts_art/adult_bear_ear_24_mount.inc"
};
static Sint16 adult_bear_ear_26_polygons[157] = {
#include "chao/parts_art/adult_bear_ear_26_polygons.inc"
};
static Sint32 adult_bear_ear_26_vertices[111] = {
#include "chao/parts_art/adult_bear_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_bear_ear_26_model = {
#include "chao/parts_art/adult_bear_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_bear_ear_26 = {
#include "chao/parts_art/adult_bear_ear_26.inc"
};
static NJS_CNK_OBJECT adult_bear_ear_26_mount = {
#include "chao/parts_art/adult_bear_ear_26_mount.inc"
};
static Sint16 adult_bear_leg_6_polygons[237] = {
#include "chao/parts_art/adult_bear_leg_6_polygons.inc"
};
static Sint32 adult_bear_leg_6_vertices[189] = {
#include "chao/parts_art/adult_bear_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_bear_leg_6_model = {
#include "chao/parts_art/adult_bear_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_bear_leg_6 = {
#include "chao/parts_art/adult_bear_leg_6.inc"
};
static NJS_CNK_OBJECT adult_bear_leg_6_mount = {
#include "chao/parts_art/adult_bear_leg_6_mount.inc"
};
static Sint16 adult_bear_leg_13_polygons[237] = {
#include "chao/parts_art/adult_bear_leg_13_polygons.inc"
};
static Sint32 adult_bear_leg_13_vertices[189] = {
#include "chao/parts_art/adult_bear_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_bear_leg_13_model = {
#include "chao/parts_art/adult_bear_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_bear_leg_13 = {
#include "chao/parts_art/adult_bear_leg_13.inc"
};
static NJS_CNK_OBJECT adult_bear_leg_13_mount = {
#include "chao/parts_art/adult_bear_leg_13_mount.inc"
};
static Sint16 adult_parrot_forehead_polygons[201] = {
#include "chao/parts_art/adult_parrot_forehead_polygons.inc"
};
static Sint32 adult_parrot_forehead_vertices[135] = {
#include "chao/parts_art/adult_parrot_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_parrot_forehead_model = {
#include "chao/parts_art/adult_parrot_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_parrot_forehead = {
#include "chao/parts_art/adult_parrot_forehead.inc"
};
static NJS_CNK_OBJECT adult_parrot_forehead_mount = {
#include "chao/parts_art/adult_parrot_forehead_mount.inc"
};
static Sint16 adult_parrot_leg_6_polygons[213] = {
#include "chao/parts_art/adult_parrot_leg_6_polygons.inc"
};
static Sint32 adult_parrot_leg_6_vertices[129] = {
#include "chao/parts_art/adult_parrot_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_parrot_leg_6_model = {
#include "chao/parts_art/adult_parrot_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_parrot_leg_6 = {
#include "chao/parts_art/adult_parrot_leg_6.inc"
};
static NJS_CNK_OBJECT adult_parrot_leg_6_mount = {
#include "chao/parts_art/adult_parrot_leg_6_mount.inc"
};
static Sint16 adult_parrot_leg_13_polygons[213] = {
#include "chao/parts_art/adult_parrot_leg_13_polygons.inc"
};
static Sint32 adult_parrot_leg_13_vertices[129] = {
#include "chao/parts_art/adult_parrot_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_parrot_leg_13_model = {
#include "chao/parts_art/adult_parrot_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_parrot_leg_13 = {
#include "chao/parts_art/adult_parrot_leg_13.inc"
};
static NJS_CNK_OBJECT adult_parrot_leg_13_mount = {
#include "chao/parts_art/adult_parrot_leg_13_mount.inc"
};
static Sint16 adult_parrot_tail_polygons[175] = {
#include "chao/parts_art/adult_parrot_tail_polygons.inc"
};
static Sint32 adult_parrot_tail_vertices[135] = {
#include "chao/parts_art/adult_parrot_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_parrot_tail_model = {
#include "chao/parts_art/adult_parrot_tail_model.inc"
};
static NJS_CNK_OBJECT adult_parrot_tail = {
#include "chao/parts_art/adult_parrot_tail.inc"
};
static NJS_CNK_OBJECT adult_parrot_tail_mount = {
#include "chao/parts_art/adult_parrot_tail_mount.inc"
};
static Sint16 adult_parrot_wing_37_polygons[119] = {
#include "chao/parts_art/adult_parrot_wing_37_polygons.inc"
};
static Sint32 adult_parrot_wing_37_vertices[87] = {
#include "chao/parts_art/adult_parrot_wing_37_vertices.inc"
};
static NJS_CNK_MODEL adult_parrot_wing_37_model = {
#include "chao/parts_art/adult_parrot_wing_37_model.inc"
};
static NJS_CNK_OBJECT adult_parrot_wing_37 = {
#include "chao/parts_art/adult_parrot_wing_37.inc"
};
static NJS_CNK_OBJECT adult_parrot_wing_37_mount = {
#include "chao/parts_art/adult_parrot_wing_37_mount.inc"
};
static Sint16 adult_parrot_wing_39_polygons[119] = {
#include "chao/parts_art/adult_parrot_wing_39_polygons.inc"
};
static Sint32 adult_parrot_wing_39_vertices[87] = {
#include "chao/parts_art/adult_parrot_wing_39_vertices.inc"
};
static NJS_CNK_MODEL adult_parrot_wing_39_model = {
#include "chao/parts_art/adult_parrot_wing_39_model.inc"
};
static NJS_CNK_OBJECT adult_parrot_wing_39 = {
#include "chao/parts_art/adult_parrot_wing_39.inc"
};
static NJS_CNK_OBJECT adult_parrot_wing_39_mount = {
#include "chao/parts_art/adult_parrot_wing_39_mount.inc"
};
static NJS_CNK_OBJECT adult_penguin_arm_3_child = {
#include "chao/parts_art/adult_penguin_arm_3_child.inc"
};
static Sint16 adult_penguin_arm_3_polygons[153] = {
#include "chao/parts_art/adult_penguin_arm_3_polygons.inc"
};
static Sint32 adult_penguin_arm_3_vertices[105] = {
#include "chao/parts_art/adult_penguin_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_penguin_arm_3_model = {
#include "chao/parts_art/adult_penguin_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_penguin_arm_3 = {
#include "chao/parts_art/adult_penguin_arm_3.inc"
};
static NJS_CNK_OBJECT adult_penguin_arm_3_mount = {
#include "chao/parts_art/adult_penguin_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_penguin_arm_10_child = {
#include "chao/parts_art/adult_penguin_arm_10_child.inc"
};
static Sint16 adult_penguin_arm_10_polygons[153] = {
#include "chao/parts_art/adult_penguin_arm_10_polygons.inc"
};
static Sint32 adult_penguin_arm_10_vertices[105] = {
#include "chao/parts_art/adult_penguin_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_penguin_arm_10_model = {
#include "chao/parts_art/adult_penguin_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_penguin_arm_10 = {
#include "chao/parts_art/adult_penguin_arm_10.inc"
};
static NJS_CNK_OBJECT adult_penguin_arm_10_mount = {
#include "chao/parts_art/adult_penguin_arm_10_mount.inc"
};
static Sint16 adult_penguin_forehead_polygons[169] = {
#include "chao/parts_art/adult_penguin_forehead_polygons.inc"
};
static Sint32 adult_penguin_forehead_vertices[147] = {
#include "chao/parts_art/adult_penguin_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_penguin_forehead_model = {
#include "chao/parts_art/adult_penguin_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_penguin_forehead = {
#include "chao/parts_art/adult_penguin_forehead.inc"
};
static NJS_CNK_OBJECT adult_penguin_forehead_mount = {
#include "chao/parts_art/adult_penguin_forehead_mount.inc"
};
static Sint16 adult_penguin_leg_6_polygons[225] = {
#include "chao/parts_art/adult_penguin_leg_6_polygons.inc"
};
static Sint32 adult_penguin_leg_6_vertices[123] = {
#include "chao/parts_art/adult_penguin_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_penguin_leg_6_model = {
#include "chao/parts_art/adult_penguin_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_penguin_leg_6 = {
#include "chao/parts_art/adult_penguin_leg_6.inc"
};
static NJS_CNK_OBJECT adult_penguin_leg_6_mount = {
#include "chao/parts_art/adult_penguin_leg_6_mount.inc"
};
static Sint16 adult_penguin_leg_13_polygons[217] = {
#include "chao/parts_art/adult_penguin_leg_13_polygons.inc"
};
static Sint32 adult_penguin_leg_13_vertices[123] = {
#include "chao/parts_art/adult_penguin_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_penguin_leg_13_model = {
#include "chao/parts_art/adult_penguin_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_penguin_leg_13 = {
#include "chao/parts_art/adult_penguin_leg_13.inc"
};
static NJS_CNK_OBJECT adult_penguin_leg_13_mount = {
#include "chao/parts_art/adult_penguin_leg_13_mount.inc"
};
static Sint16 adult_phoenix_forehead_polygons[293] = {
#include "chao/parts_art/adult_phoenix_forehead_polygons.inc"
};
static Sint32 adult_phoenix_forehead_vertices[243] = {
#include "chao/parts_art/adult_phoenix_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_phoenix_forehead_model = {
#include "chao/parts_art/adult_phoenix_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_phoenix_forehead = {
#include "chao/parts_art/adult_phoenix_forehead.inc"
};
static NJS_CNK_OBJECT adult_phoenix_forehead_mount = {
#include "chao/parts_art/adult_phoenix_forehead_mount.inc"
};
static Sint16 adult_phoenix_leg_6_polygons[239] = {
#include "chao/parts_art/adult_phoenix_leg_6_polygons.inc"
};
static Sint32 adult_phoenix_leg_6_vertices[135] = {
#include "chao/parts_art/adult_phoenix_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_phoenix_leg_6_model = {
#include "chao/parts_art/adult_phoenix_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_phoenix_leg_6 = {
#include "chao/parts_art/adult_phoenix_leg_6.inc"
};
static NJS_CNK_OBJECT adult_phoenix_leg_6_mount = {
#include "chao/parts_art/adult_phoenix_leg_6_mount.inc"
};
static Sint16 adult_phoenix_leg_13_polygons[233] = {
#include "chao/parts_art/adult_phoenix_leg_13_polygons.inc"
};
static Sint32 adult_phoenix_leg_13_vertices[135] = {
#include "chao/parts_art/adult_phoenix_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_phoenix_leg_13_model = {
#include "chao/parts_art/adult_phoenix_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_phoenix_leg_13 = {
#include "chao/parts_art/adult_phoenix_leg_13.inc"
};
static NJS_CNK_OBJECT adult_phoenix_leg_13_mount = {
#include "chao/parts_art/adult_phoenix_leg_13_mount.inc"
};
static Sint16 adult_phoenix_tail_polygons[223] = {
#include "chao/parts_art/adult_phoenix_tail_polygons.inc"
};
static Sint32 adult_phoenix_tail_vertices[255] = {
#include "chao/parts_art/adult_phoenix_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_phoenix_tail_model = {
#include "chao/parts_art/adult_phoenix_tail_model.inc"
};
static NJS_CNK_OBJECT adult_phoenix_tail = {
#include "chao/parts_art/adult_phoenix_tail.inc"
};
static NJS_CNK_OBJECT adult_phoenix_tail_mount = {
#include "chao/parts_art/adult_phoenix_tail_mount.inc"
};
static Sint16 adult_phoenix_wing_37_polygons[219] = {
#include "chao/parts_art/adult_phoenix_wing_37_polygons.inc"
};
static Sint32 adult_phoenix_wing_37_vertices[147] = {
#include "chao/parts_art/adult_phoenix_wing_37_vertices.inc"
};
static NJS_CNK_MODEL adult_phoenix_wing_37_model = {
#include "chao/parts_art/adult_phoenix_wing_37_model.inc"
};
static NJS_CNK_OBJECT adult_phoenix_wing_37 = {
#include "chao/parts_art/adult_phoenix_wing_37.inc"
};
static NJS_CNK_OBJECT adult_phoenix_wing_37_mount = {
#include "chao/parts_art/adult_phoenix_wing_37_mount.inc"
};
static Sint16 adult_phoenix_wing_39_polygons[219] = {
#include "chao/parts_art/adult_phoenix_wing_39_polygons.inc"
};
static Sint32 adult_phoenix_wing_39_vertices[147] = {
#include "chao/parts_art/adult_phoenix_wing_39_vertices.inc"
};
static NJS_CNK_MODEL adult_phoenix_wing_39_model = {
#include "chao/parts_art/adult_phoenix_wing_39_model.inc"
};
static NJS_CNK_OBJECT adult_phoenix_wing_39 = {
#include "chao/parts_art/adult_phoenix_wing_39.inc"
};
static NJS_CNK_OBJECT adult_phoenix_wing_39_mount = {
#include "chao/parts_art/adult_phoenix_wing_39_mount.inc"
};
static NJS_CNK_OBJECT adult_otter_arm_3_child = {
#include "chao/parts_art/adult_otter_arm_3_child.inc"
};
static Sint16 adult_otter_arm_3_polygons[211] = {
#include "chao/parts_art/adult_otter_arm_3_polygons.inc"
};
static Sint32 adult_otter_arm_3_vertices[171] = {
#include "chao/parts_art/adult_otter_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_arm_3_model = {
#include "chao/parts_art/adult_otter_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_otter_arm_3 = {
#include "chao/parts_art/adult_otter_arm_3.inc"
};
static NJS_CNK_OBJECT adult_otter_arm_3_mount = {
#include "chao/parts_art/adult_otter_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_otter_arm_10_child = {
#include "chao/parts_art/adult_otter_arm_10_child.inc"
};
static Sint16 adult_otter_arm_10_polygons[217] = {
#include "chao/parts_art/adult_otter_arm_10_polygons.inc"
};
static Sint32 adult_otter_arm_10_vertices[171] = {
#include "chao/parts_art/adult_otter_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_arm_10_model = {
#include "chao/parts_art/adult_otter_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_otter_arm_10 = {
#include "chao/parts_art/adult_otter_arm_10.inc"
};
static NJS_CNK_OBJECT adult_otter_arm_10_mount = {
#include "chao/parts_art/adult_otter_arm_10_mount.inc"
};
static Sint16 adult_otter_ear_24_polygons[181] = {
#include "chao/parts_art/adult_otter_ear_24_polygons.inc"
};
static Sint32 adult_otter_ear_24_vertices[111] = {
#include "chao/parts_art/adult_otter_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_ear_24_model = {
#include "chao/parts_art/adult_otter_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_otter_ear_24 = {
#include "chao/parts_art/adult_otter_ear_24.inc"
};
static NJS_CNK_OBJECT adult_otter_ear_24_mount = {
#include "chao/parts_art/adult_otter_ear_24_mount.inc"
};
static Sint16 adult_otter_ear_26_polygons[181] = {
#include "chao/parts_art/adult_otter_ear_26_polygons.inc"
};
static Sint32 adult_otter_ear_26_vertices[111] = {
#include "chao/parts_art/adult_otter_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_ear_26_model = {
#include "chao/parts_art/adult_otter_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_otter_ear_26 = {
#include "chao/parts_art/adult_otter_ear_26.inc"
};
static NJS_CNK_OBJECT adult_otter_ear_26_mount = {
#include "chao/parts_art/adult_otter_ear_26_mount.inc"
};
static Sint16 adult_otter_leg_6_polygons[189] = {
#include "chao/parts_art/adult_otter_leg_6_polygons.inc"
};
static Sint32 adult_otter_leg_6_vertices[153] = {
#include "chao/parts_art/adult_otter_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_leg_6_model = {
#include "chao/parts_art/adult_otter_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_otter_leg_6 = {
#include "chao/parts_art/adult_otter_leg_6.inc"
};
static NJS_CNK_OBJECT adult_otter_leg_6_mount = {
#include "chao/parts_art/adult_otter_leg_6_mount.inc"
};
static Sint16 adult_otter_leg_13_polygons[189] = {
#include "chao/parts_art/adult_otter_leg_13_polygons.inc"
};
static Sint32 adult_otter_leg_13_vertices[153] = {
#include "chao/parts_art/adult_otter_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_leg_13_model = {
#include "chao/parts_art/adult_otter_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_otter_leg_13 = {
#include "chao/parts_art/adult_otter_leg_13.inc"
};
static NJS_CNK_OBJECT adult_otter_leg_13_mount = {
#include "chao/parts_art/adult_otter_leg_13_mount.inc"
};
static Sint16 adult_otter_tail_polygons[145] = {
#include "chao/parts_art/adult_otter_tail_polygons.inc"
};
static Sint32 adult_otter_tail_vertices[105] = {
#include "chao/parts_art/adult_otter_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_tail_model = {
#include "chao/parts_art/adult_otter_tail_model.inc"
};
static NJS_CNK_OBJECT adult_otter_tail = {
#include "chao/parts_art/adult_otter_tail.inc"
};
static NJS_CNK_OBJECT adult_otter_tail_mount = {
#include "chao/parts_art/adult_otter_tail_mount.inc"
};
static Sint16 adult_otter_face_polygons[109] = {
#include "chao/parts_art/adult_otter_face_polygons.inc"
};
static Sint32 adult_otter_face_vertices[147] = {
#include "chao/parts_art/adult_otter_face_vertices.inc"
};
static NJS_CNK_MODEL adult_otter_face_model = {
#include "chao/parts_art/adult_otter_face_model.inc"
};
static NJS_CNK_OBJECT adult_otter_face = {
#include "chao/parts_art/adult_otter_face.inc"
};
static NJS_CNK_OBJECT adult_otter_face_mount = {
#include "chao/parts_art/adult_otter_face_mount.inc"
};
static NJS_CNK_OBJECT adult_sheep_arm_3_child = {
#include "chao/parts_art/adult_sheep_arm_3_child.inc"
};
static Sint16 adult_sheep_arm_3_polygons[187] = {
#include "chao/parts_art/adult_sheep_arm_3_polygons.inc"
};
static Sint32 adult_sheep_arm_3_vertices[147] = {
#include "chao/parts_art/adult_sheep_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_arm_3_model = {
#include "chao/parts_art/adult_sheep_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_arm_3 = {
#include "chao/parts_art/adult_sheep_arm_3.inc"
};
static NJS_CNK_OBJECT adult_sheep_arm_3_mount = {
#include "chao/parts_art/adult_sheep_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_sheep_arm_10_child = {
#include "chao/parts_art/adult_sheep_arm_10_child.inc"
};
static Sint16 adult_sheep_arm_10_polygons[187] = {
#include "chao/parts_art/adult_sheep_arm_10_polygons.inc"
};
static Sint32 adult_sheep_arm_10_vertices[147] = {
#include "chao/parts_art/adult_sheep_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_arm_10_model = {
#include "chao/parts_art/adult_sheep_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_arm_10 = {
#include "chao/parts_art/adult_sheep_arm_10.inc"
};
static NJS_CNK_OBJECT adult_sheep_arm_10_mount = {
#include "chao/parts_art/adult_sheep_arm_10_mount.inc"
};
static Sint16 adult_sheep_ear_24_polygons[125] = {
#include "chao/parts_art/adult_sheep_ear_24_polygons.inc"
};
static Sint32 adult_sheep_ear_24_vertices[93] = {
#include "chao/parts_art/adult_sheep_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_ear_24_model = {
#include "chao/parts_art/adult_sheep_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_ear_24 = {
#include "chao/parts_art/adult_sheep_ear_24.inc"
};
static NJS_CNK_OBJECT adult_sheep_ear_24_mount = {
#include "chao/parts_art/adult_sheep_ear_24_mount.inc"
};
static Sint16 adult_sheep_ear_26_polygons[125] = {
#include "chao/parts_art/adult_sheep_ear_26_polygons.inc"
};
static Sint32 adult_sheep_ear_26_vertices[93] = {
#include "chao/parts_art/adult_sheep_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_ear_26_model = {
#include "chao/parts_art/adult_sheep_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_ear_26 = {
#include "chao/parts_art/adult_sheep_ear_26.inc"
};
static NJS_CNK_OBJECT adult_sheep_ear_26_mount = {
#include "chao/parts_art/adult_sheep_ear_26_mount.inc"
};
static Sint16 adult_sheep_horn_30_polygons[151] = {
#include "chao/parts_art/adult_sheep_horn_30_polygons.inc"
};
static Sint32 adult_sheep_horn_30_vertices[117] = {
#include "chao/parts_art/adult_sheep_horn_30_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_horn_30_model = {
#include "chao/parts_art/adult_sheep_horn_30_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_horn_30 = {
#include "chao/parts_art/adult_sheep_horn_30.inc"
};
static NJS_CNK_OBJECT adult_sheep_horn_30_mount = {
#include "chao/parts_art/adult_sheep_horn_30_mount.inc"
};
static Sint16 adult_sheep_horn_31_polygons[151] = {
#include "chao/parts_art/adult_sheep_horn_31_polygons.inc"
};
static Sint32 adult_sheep_horn_31_vertices[117] = {
#include "chao/parts_art/adult_sheep_horn_31_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_horn_31_model = {
#include "chao/parts_art/adult_sheep_horn_31_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_horn_31 = {
#include "chao/parts_art/adult_sheep_horn_31.inc"
};
static NJS_CNK_OBJECT adult_sheep_horn_31_mount = {
#include "chao/parts_art/adult_sheep_horn_31_mount.inc"
};
static Sint16 adult_sheep_leg_6_polygons[163] = {
#include "chao/parts_art/adult_sheep_leg_6_polygons.inc"
};
static Sint32 adult_sheep_leg_6_vertices[117] = {
#include "chao/parts_art/adult_sheep_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_leg_6_model = {
#include "chao/parts_art/adult_sheep_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_leg_6 = {
#include "chao/parts_art/adult_sheep_leg_6.inc"
};
static NJS_CNK_OBJECT adult_sheep_leg_6_mount = {
#include "chao/parts_art/adult_sheep_leg_6_mount.inc"
};
static Sint16 adult_sheep_leg_13_polygons[163] = {
#include "chao/parts_art/adult_sheep_leg_13_polygons.inc"
};
static Sint32 adult_sheep_leg_13_vertices[117] = {
#include "chao/parts_art/adult_sheep_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_leg_13_model = {
#include "chao/parts_art/adult_sheep_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_leg_13 = {
#include "chao/parts_art/adult_sheep_leg_13.inc"
};
static NJS_CNK_OBJECT adult_sheep_leg_13_mount = {
#include "chao/parts_art/adult_sheep_leg_13_mount.inc"
};
static Sint16 adult_sheep_tail_polygons[107] = {
#include "chao/parts_art/adult_sheep_tail_polygons.inc"
};
static Sint32 adult_sheep_tail_vertices[75] = {
#include "chao/parts_art/adult_sheep_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_sheep_tail_model = {
#include "chao/parts_art/adult_sheep_tail_model.inc"
};
static NJS_CNK_OBJECT adult_sheep_tail = {
#include "chao/parts_art/adult_sheep_tail.inc"
};
static NJS_CNK_OBJECT adult_sheep_tail_mount = {
#include "chao/parts_art/adult_sheep_tail_mount.inc"
};
static NJS_CNK_OBJECT adult_skunk_arm_3_child = {
#include "chao/parts_art/adult_skunk_arm_3_child.inc"
};
static Sint16 adult_skunk_arm_3_polygons[213] = {
#include "chao/parts_art/adult_skunk_arm_3_polygons.inc"
};
static Sint32 adult_skunk_arm_3_vertices[141] = {
#include "chao/parts_art/adult_skunk_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_skunk_arm_3_model = {
#include "chao/parts_art/adult_skunk_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_skunk_arm_3 = {
#include "chao/parts_art/adult_skunk_arm_3.inc"
};
static NJS_CNK_OBJECT adult_skunk_arm_3_mount = {
#include "chao/parts_art/adult_skunk_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_skunk_arm_10_child = {
#include "chao/parts_art/adult_skunk_arm_10_child.inc"
};
static Sint16 adult_skunk_arm_10_polygons[213] = {
#include "chao/parts_art/adult_skunk_arm_10_polygons.inc"
};
static Sint32 adult_skunk_arm_10_vertices[141] = {
#include "chao/parts_art/adult_skunk_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_skunk_arm_10_model = {
#include "chao/parts_art/adult_skunk_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_skunk_arm_10 = {
#include "chao/parts_art/adult_skunk_arm_10.inc"
};
static NJS_CNK_OBJECT adult_skunk_arm_10_mount = {
#include "chao/parts_art/adult_skunk_arm_10_mount.inc"
};
static Sint16 adult_skunk_forehead_polygons[213] = {
#include "chao/parts_art/adult_skunk_forehead_polygons.inc"
};
static Sint32 adult_skunk_forehead_vertices[171] = {
#include "chao/parts_art/adult_skunk_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_skunk_forehead_model = {
#include "chao/parts_art/adult_skunk_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_skunk_forehead = {
#include "chao/parts_art/adult_skunk_forehead.inc"
};
static NJS_CNK_OBJECT adult_skunk_forehead_mount = {
#include "chao/parts_art/adult_skunk_forehead_mount.inc"
};
static Sint16 adult_skunk_leg_6_polygons[181] = {
#include "chao/parts_art/adult_skunk_leg_6_polygons.inc"
};
static Sint32 adult_skunk_leg_6_vertices[135] = {
#include "chao/parts_art/adult_skunk_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_skunk_leg_6_model = {
#include "chao/parts_art/adult_skunk_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_skunk_leg_6 = {
#include "chao/parts_art/adult_skunk_leg_6.inc"
};
static NJS_CNK_OBJECT adult_skunk_leg_6_mount = {
#include "chao/parts_art/adult_skunk_leg_6_mount.inc"
};
static Sint16 adult_skunk_leg_13_polygons[181] = {
#include "chao/parts_art/adult_skunk_leg_13_polygons.inc"
};
static Sint32 adult_skunk_leg_13_vertices[135] = {
#include "chao/parts_art/adult_skunk_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_skunk_leg_13_model = {
#include "chao/parts_art/adult_skunk_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_skunk_leg_13 = {
#include "chao/parts_art/adult_skunk_leg_13.inc"
};
static NJS_CNK_OBJECT adult_skunk_leg_13_mount = {
#include "chao/parts_art/adult_skunk_leg_13_mount.inc"
};
static Sint16 adult_skunk_tail_polygons[207] = {
#include "chao/parts_art/adult_skunk_tail_polygons.inc"
};
static Sint32 adult_skunk_tail_vertices[153] = {
#include "chao/parts_art/adult_skunk_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_skunk_tail_model = {
#include "chao/parts_art/adult_skunk_tail_model.inc"
};
static NJS_CNK_OBJECT adult_skunk_tail = {
#include "chao/parts_art/adult_skunk_tail.inc"
};
static NJS_CNK_OBJECT adult_skunk_tail_mount = {
#include "chao/parts_art/adult_skunk_tail_mount.inc"
};
static NJS_CNK_OBJECT adult_tiger_arm_3_child = {
#include "chao/parts_art/adult_tiger_arm_3_child.inc"
};
static Sint16 adult_tiger_arm_3_polygons[259] = {
#include "chao/parts_art/adult_tiger_arm_3_polygons.inc"
};
static Sint32 adult_tiger_arm_3_vertices[147] = {
#include "chao/parts_art/adult_tiger_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_arm_3_model = {
#include "chao/parts_art/adult_tiger_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_arm_3 = {
#include "chao/parts_art/adult_tiger_arm_3.inc"
};
static NJS_CNK_OBJECT adult_tiger_arm_3_mount = {
#include "chao/parts_art/adult_tiger_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_tiger_arm_10_child = {
#include "chao/parts_art/adult_tiger_arm_10_child.inc"
};
static Sint16 adult_tiger_arm_10_polygons[259] = {
#include "chao/parts_art/adult_tiger_arm_10_polygons.inc"
};
static Sint32 adult_tiger_arm_10_vertices[147] = {
#include "chao/parts_art/adult_tiger_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_arm_10_model = {
#include "chao/parts_art/adult_tiger_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_arm_10 = {
#include "chao/parts_art/adult_tiger_arm_10.inc"
};
static NJS_CNK_OBJECT adult_tiger_arm_10_mount = {
#include "chao/parts_art/adult_tiger_arm_10_mount.inc"
};
static Sint16 adult_tiger_ear_24_polygons[147] = {
#include "chao/parts_art/adult_tiger_ear_24_polygons.inc"
};
static Sint32 adult_tiger_ear_24_vertices[87] = {
#include "chao/parts_art/adult_tiger_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_ear_24_model = {
#include "chao/parts_art/adult_tiger_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_ear_24 = {
#include "chao/parts_art/adult_tiger_ear_24.inc"
};
static NJS_CNK_OBJECT adult_tiger_ear_24_mount = {
#include "chao/parts_art/adult_tiger_ear_24_mount.inc"
};
static Sint16 adult_tiger_ear_26_polygons[139] = {
#include "chao/parts_art/adult_tiger_ear_26_polygons.inc"
};
static Sint32 adult_tiger_ear_26_vertices[87] = {
#include "chao/parts_art/adult_tiger_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_ear_26_model = {
#include "chao/parts_art/adult_tiger_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_ear_26 = {
#include "chao/parts_art/adult_tiger_ear_26.inc"
};
static NJS_CNK_OBJECT adult_tiger_ear_26_mount = {
#include "chao/parts_art/adult_tiger_ear_26_mount.inc"
};
static Sint16 adult_tiger_leg_6_polygons[237] = {
#include "chao/parts_art/adult_tiger_leg_6_polygons.inc"
};
static Sint32 adult_tiger_leg_6_vertices[135] = {
#include "chao/parts_art/adult_tiger_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_leg_6_model = {
#include "chao/parts_art/adult_tiger_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_leg_6 = {
#include "chao/parts_art/adult_tiger_leg_6.inc"
};
static NJS_CNK_OBJECT adult_tiger_leg_6_mount = {
#include "chao/parts_art/adult_tiger_leg_6_mount.inc"
};
static Sint16 adult_tiger_leg_13_polygons[237] = {
#include "chao/parts_art/adult_tiger_leg_13_polygons.inc"
};
static Sint32 adult_tiger_leg_13_vertices[135] = {
#include "chao/parts_art/adult_tiger_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_leg_13_model = {
#include "chao/parts_art/adult_tiger_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_leg_13 = {
#include "chao/parts_art/adult_tiger_leg_13.inc"
};
static NJS_CNK_OBJECT adult_tiger_leg_13_mount = {
#include "chao/parts_art/adult_tiger_leg_13_mount.inc"
};
static Sint16 adult_tiger_tail_polygons[189] = {
#include "chao/parts_art/adult_tiger_tail_polygons.inc"
};
static Sint32 adult_tiger_tail_vertices[135] = {
#include "chao/parts_art/adult_tiger_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_tiger_tail_model = {
#include "chao/parts_art/adult_tiger_tail_model.inc"
};
static NJS_CNK_OBJECT adult_tiger_tail = {
#include "chao/parts_art/adult_tiger_tail.inc"
};
static NJS_CNK_OBJECT adult_tiger_tail_mount = {
#include "chao/parts_art/adult_tiger_tail_mount.inc"
};
static NJS_CNK_OBJECT adult_unicorn_arm_3_child = {
#include "chao/parts_art/adult_unicorn_arm_3_child.inc"
};
static Sint16 adult_unicorn_arm_3_polygons[255] = {
#include "chao/parts_art/adult_unicorn_arm_3_polygons.inc"
};
static Sint32 adult_unicorn_arm_3_vertices[171] = {
#include "chao/parts_art/adult_unicorn_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_arm_3_model = {
#include "chao/parts_art/adult_unicorn_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_arm_3 = {
#include "chao/parts_art/adult_unicorn_arm_3.inc"
};
static NJS_CNK_OBJECT adult_unicorn_arm_3_mount = {
#include "chao/parts_art/adult_unicorn_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_unicorn_arm_10_child = {
#include "chao/parts_art/adult_unicorn_arm_10_child.inc"
};
static Sint16 adult_unicorn_arm_10_polygons[255] = {
#include "chao/parts_art/adult_unicorn_arm_10_polygons.inc"
};
static Sint32 adult_unicorn_arm_10_vertices[171] = {
#include "chao/parts_art/adult_unicorn_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_arm_10_model = {
#include "chao/parts_art/adult_unicorn_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_arm_10 = {
#include "chao/parts_art/adult_unicorn_arm_10.inc"
};
static NJS_CNK_OBJECT adult_unicorn_arm_10_mount = {
#include "chao/parts_art/adult_unicorn_arm_10_mount.inc"
};
static Sint16 adult_unicorn_ear_24_polygons[169] = {
#include "chao/parts_art/adult_unicorn_ear_24_polygons.inc"
};
static Sint32 adult_unicorn_ear_24_vertices[111] = {
#include "chao/parts_art/adult_unicorn_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_ear_24_model = {
#include "chao/parts_art/adult_unicorn_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_ear_24 = {
#include "chao/parts_art/adult_unicorn_ear_24.inc"
};
static NJS_CNK_OBJECT adult_unicorn_ear_24_mount = {
#include "chao/parts_art/adult_unicorn_ear_24_mount.inc"
};
static Sint16 adult_unicorn_ear_26_polygons[169] = {
#include "chao/parts_art/adult_unicorn_ear_26_polygons.inc"
};
static Sint32 adult_unicorn_ear_26_vertices[111] = {
#include "chao/parts_art/adult_unicorn_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_ear_26_model = {
#include "chao/parts_art/adult_unicorn_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_ear_26 = {
#include "chao/parts_art/adult_unicorn_ear_26.inc"
};
static NJS_CNK_OBJECT adult_unicorn_ear_26_mount = {
#include "chao/parts_art/adult_unicorn_ear_26_mount.inc"
};
static Sint16 adult_unicorn_forehead_polygons[243] = {
#include "chao/parts_art/adult_unicorn_forehead_polygons.inc"
};
static Sint32 adult_unicorn_forehead_vertices[189] = {
#include "chao/parts_art/adult_unicorn_forehead_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_forehead_model = {
#include "chao/parts_art/adult_unicorn_forehead_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_forehead = {
#include "chao/parts_art/adult_unicorn_forehead.inc"
};
static NJS_CNK_OBJECT adult_unicorn_forehead_mount = {
#include "chao/parts_art/adult_unicorn_forehead_mount.inc"
};
static Sint16 adult_unicorn_leg_6_polygons[183] = {
#include "chao/parts_art/adult_unicorn_leg_6_polygons.inc"
};
static Sint32 adult_unicorn_leg_6_vertices[117] = {
#include "chao/parts_art/adult_unicorn_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_leg_6_model = {
#include "chao/parts_art/adult_unicorn_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_leg_6 = {
#include "chao/parts_art/adult_unicorn_leg_6.inc"
};
static NJS_CNK_OBJECT adult_unicorn_leg_6_mount = {
#include "chao/parts_art/adult_unicorn_leg_6_mount.inc"
};
static Sint16 adult_unicorn_leg_13_polygons[183] = {
#include "chao/parts_art/adult_unicorn_leg_13_polygons.inc"
};
static Sint32 adult_unicorn_leg_13_vertices[117] = {
#include "chao/parts_art/adult_unicorn_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_leg_13_model = {
#include "chao/parts_art/adult_unicorn_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_leg_13 = {
#include "chao/parts_art/adult_unicorn_leg_13.inc"
};
static NJS_CNK_OBJECT adult_unicorn_leg_13_mount = {
#include "chao/parts_art/adult_unicorn_leg_13_mount.inc"
};
static Sint16 adult_unicorn_tail_polygons[179] = {
#include "chao/parts_art/adult_unicorn_tail_polygons.inc"
};
static Sint32 adult_unicorn_tail_vertices[111] = {
#include "chao/parts_art/adult_unicorn_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_unicorn_tail_model = {
#include "chao/parts_art/adult_unicorn_tail_model.inc"
};
static NJS_CNK_OBJECT adult_unicorn_tail = {
#include "chao/parts_art/adult_unicorn_tail.inc"
};
static NJS_CNK_OBJECT adult_unicorn_tail_mount = {
#include "chao/parts_art/adult_unicorn_tail_mount.inc"
};
static NJS_CNK_OBJECT adult_rabbit_arm_3_child = {
#include "chao/parts_art/adult_rabbit_arm_3_child.inc"
};
static Sint16 adult_rabbit_arm_3_polygons[201] = {
#include "chao/parts_art/adult_rabbit_arm_3_polygons.inc"
};
static Sint32 adult_rabbit_arm_3_vertices[141] = {
#include "chao/parts_art/adult_rabbit_arm_3_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_arm_3_model = {
#include "chao/parts_art/adult_rabbit_arm_3_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_arm_3 = {
#include "chao/parts_art/adult_rabbit_arm_3.inc"
};
static NJS_CNK_OBJECT adult_rabbit_arm_3_mount = {
#include "chao/parts_art/adult_rabbit_arm_3_mount.inc"
};
static NJS_CNK_OBJECT adult_rabbit_arm_10_child = {
#include "chao/parts_art/adult_rabbit_arm_10_child.inc"
};
static Sint16 adult_rabbit_arm_10_polygons[201] = {
#include "chao/parts_art/adult_rabbit_arm_10_polygons.inc"
};
static Sint32 adult_rabbit_arm_10_vertices[141] = {
#include "chao/parts_art/adult_rabbit_arm_10_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_arm_10_model = {
#include "chao/parts_art/adult_rabbit_arm_10_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_arm_10 = {
#include "chao/parts_art/adult_rabbit_arm_10.inc"
};
static NJS_CNK_OBJECT adult_rabbit_arm_10_mount = {
#include "chao/parts_art/adult_rabbit_arm_10_mount.inc"
};
static Sint16 adult_rabbit_ear_24_polygons[139] = {
#include "chao/parts_art/adult_rabbit_ear_24_polygons.inc"
};
static Sint32 adult_rabbit_ear_24_vertices[93] = {
#include "chao/parts_art/adult_rabbit_ear_24_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_ear_24_model = {
#include "chao/parts_art/adult_rabbit_ear_24_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_ear_24 = {
#include "chao/parts_art/adult_rabbit_ear_24.inc"
};
static NJS_CNK_OBJECT adult_rabbit_ear_24_mount = {
#include "chao/parts_art/adult_rabbit_ear_24_mount.inc"
};
static Sint16 adult_rabbit_ear_26_polygons[147] = {
#include "chao/parts_art/adult_rabbit_ear_26_polygons.inc"
};
static Sint32 adult_rabbit_ear_26_vertices[93] = {
#include "chao/parts_art/adult_rabbit_ear_26_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_ear_26_model = {
#include "chao/parts_art/adult_rabbit_ear_26_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_ear_26 = {
#include "chao/parts_art/adult_rabbit_ear_26.inc"
};
static NJS_CNK_OBJECT adult_rabbit_ear_26_mount = {
#include "chao/parts_art/adult_rabbit_ear_26_mount.inc"
};
static Sint16 adult_rabbit_leg_6_polygons[157] = {
#include "chao/parts_art/adult_rabbit_leg_6_polygons.inc"
};
static Sint32 adult_rabbit_leg_6_vertices[117] = {
#include "chao/parts_art/adult_rabbit_leg_6_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_leg_6_model = {
#include "chao/parts_art/adult_rabbit_leg_6_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_leg_6 = {
#include "chao/parts_art/adult_rabbit_leg_6.inc"
};
static NJS_CNK_OBJECT adult_rabbit_leg_6_mount = {
#include "chao/parts_art/adult_rabbit_leg_6_mount.inc"
};
static Sint16 adult_rabbit_leg_13_polygons[157] = {
#include "chao/parts_art/adult_rabbit_leg_13_polygons.inc"
};
static Sint32 adult_rabbit_leg_13_vertices[117] = {
#include "chao/parts_art/adult_rabbit_leg_13_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_leg_13_model = {
#include "chao/parts_art/adult_rabbit_leg_13_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_leg_13 = {
#include "chao/parts_art/adult_rabbit_leg_13.inc"
};
static NJS_CNK_OBJECT adult_rabbit_leg_13_mount = {
#include "chao/parts_art/adult_rabbit_leg_13_mount.inc"
};
static Sint16 adult_rabbit_tail_polygons[165] = {
#include "chao/parts_art/adult_rabbit_tail_polygons.inc"
};
static Sint32 adult_rabbit_tail_vertices[105] = {
#include "chao/parts_art/adult_rabbit_tail_vertices.inc"
};
static NJS_CNK_MODEL adult_rabbit_tail_model = {
#include "chao/parts_art/adult_rabbit_tail_model.inc"
};
static NJS_CNK_OBJECT adult_rabbit_tail = {
#include "chao/parts_art/adult_rabbit_tail.inc"
};
static NJS_CNK_OBJECT adult_rabbit_tail_mount = {
#include "chao/parts_art/adult_rabbit_tail_mount.inc"
};
static NJS_CNK_OBJECT child_raccoon_arm_3_child = {
#include "chao/parts_art/child_raccoon_arm_3_child.inc"
};
static Sint16 child_raccoon_arm_3_polygons[187] = {
#include "chao/parts_art/child_raccoon_arm_3_polygons.inc"
};
static Sint32 child_raccoon_arm_3_vertices[147] = {
#include "chao/parts_art/child_raccoon_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_arm_3_model = {
#include "chao/parts_art/child_raccoon_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_arm_3 = {
#include "chao/parts_art/child_raccoon_arm_3.inc"
};
static NJS_CNK_OBJECT child_raccoon_arm_3_mount = {
#include "chao/parts_art/child_raccoon_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_raccoon_arm_10_child = {
#include "chao/parts_art/child_raccoon_arm_10_child.inc"
};
static Sint16 child_raccoon_arm_10_polygons[187] = {
#include "chao/parts_art/child_raccoon_arm_10_polygons.inc"
};
static Sint32 child_raccoon_arm_10_vertices[147] = {
#include "chao/parts_art/child_raccoon_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_arm_10_model = {
#include "chao/parts_art/child_raccoon_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_arm_10 = {
#include "chao/parts_art/child_raccoon_arm_10.inc"
};
static NJS_CNK_OBJECT child_raccoon_arm_10_mount = {
#include "chao/parts_art/child_raccoon_arm_10_mount.inc"
};
static Sint16 child_raccoon_ear_24_polygons[157] = {
#include "chao/parts_art/child_raccoon_ear_24_polygons.inc"
};
static Sint32 child_raccoon_ear_24_vertices[123] = {
#include "chao/parts_art/child_raccoon_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_ear_24_model = {
#include "chao/parts_art/child_raccoon_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_ear_24 = {
#include "chao/parts_art/child_raccoon_ear_24.inc"
};
static NJS_CNK_OBJECT child_raccoon_ear_24_mount = {
#include "chao/parts_art/child_raccoon_ear_24_mount.inc"
};
static Sint16 child_raccoon_ear_26_polygons[157] = {
#include "chao/parts_art/child_raccoon_ear_26_polygons.inc"
};
static Sint32 child_raccoon_ear_26_vertices[123] = {
#include "chao/parts_art/child_raccoon_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_ear_26_model = {
#include "chao/parts_art/child_raccoon_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_ear_26 = {
#include "chao/parts_art/child_raccoon_ear_26.inc"
};
static NJS_CNK_OBJECT child_raccoon_ear_26_mount = {
#include "chao/parts_art/child_raccoon_ear_26_mount.inc"
};
static Sint16 child_raccoon_leg_6_polygons[161] = {
#include "chao/parts_art/child_raccoon_leg_6_polygons.inc"
};
static Sint32 child_raccoon_leg_6_vertices[129] = {
#include "chao/parts_art/child_raccoon_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_leg_6_model = {
#include "chao/parts_art/child_raccoon_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_leg_6 = {
#include "chao/parts_art/child_raccoon_leg_6.inc"
};
static NJS_CNK_OBJECT child_raccoon_leg_6_mount = {
#include "chao/parts_art/child_raccoon_leg_6_mount.inc"
};
static Sint16 child_raccoon_leg_13_polygons[161] = {
#include "chao/parts_art/child_raccoon_leg_13_polygons.inc"
};
static Sint32 child_raccoon_leg_13_vertices[129] = {
#include "chao/parts_art/child_raccoon_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_leg_13_model = {
#include "chao/parts_art/child_raccoon_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_leg_13 = {
#include "chao/parts_art/child_raccoon_leg_13.inc"
};
static NJS_CNK_OBJECT child_raccoon_leg_13_mount = {
#include "chao/parts_art/child_raccoon_leg_13_mount.inc"
};
static Sint16 child_raccoon_unused_part_polygons[183] = {
#include "chao/parts_art/child_raccoon_unused_part_polygons.inc"
};
static Sint32 child_raccoon_unused_part_vertices[129] = {
#include "chao/parts_art/child_raccoon_unused_part_vertices.inc"
};
static NJS_CNK_MODEL child_raccoon_unused_part_model = {
#include "chao/parts_art/child_raccoon_unused_part_model.inc"
};
static NJS_CNK_OBJECT child_raccoon_unused_part = {
#include "chao/parts_art/child_raccoon_unused_part.inc"
};
static NJS_CNK_OBJECT child_raccoon_unused_part_mount = {
#include "chao/parts_art/child_raccoon_unused_part_mount.inc"
};
static NJS_CNK_OBJECT child_cheetah_arm_3_child = {
#include "chao/parts_art/child_cheetah_arm_3_child.inc"
};
static Sint16 child_cheetah_arm_3_polygons[227] = {
#include "chao/parts_art/child_cheetah_arm_3_polygons.inc"
};
static Sint32 child_cheetah_arm_3_vertices[147] = {
#include "chao/parts_art/child_cheetah_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_arm_3_model = {
#include "chao/parts_art/child_cheetah_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_arm_3 = {
#include "chao/parts_art/child_cheetah_arm_3.inc"
};
static NJS_CNK_OBJECT child_cheetah_arm_3_mount = {
#include "chao/parts_art/child_cheetah_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_cheetah_arm_10_child = {
#include "chao/parts_art/child_cheetah_arm_10_child.inc"
};
static Sint16 child_cheetah_arm_10_polygons[227] = {
#include "chao/parts_art/child_cheetah_arm_10_polygons.inc"
};
static Sint32 child_cheetah_arm_10_vertices[147] = {
#include "chao/parts_art/child_cheetah_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_arm_10_model = {
#include "chao/parts_art/child_cheetah_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_arm_10 = {
#include "chao/parts_art/child_cheetah_arm_10.inc"
};
static NJS_CNK_OBJECT child_cheetah_arm_10_mount = {
#include "chao/parts_art/child_cheetah_arm_10_mount.inc"
};
static Sint16 child_cheetah_ear_24_polygons[169] = {
#include "chao/parts_art/child_cheetah_ear_24_polygons.inc"
};
static Sint32 child_cheetah_ear_24_vertices[123] = {
#include "chao/parts_art/child_cheetah_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_ear_24_model = {
#include "chao/parts_art/child_cheetah_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_ear_24 = {
#include "chao/parts_art/child_cheetah_ear_24.inc"
};
static NJS_CNK_OBJECT child_cheetah_ear_24_mount = {
#include "chao/parts_art/child_cheetah_ear_24_mount.inc"
};
static Sint16 child_cheetah_ear_26_polygons[169] = {
#include "chao/parts_art/child_cheetah_ear_26_polygons.inc"
};
static Sint32 child_cheetah_ear_26_vertices[123] = {
#include "chao/parts_art/child_cheetah_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_ear_26_model = {
#include "chao/parts_art/child_cheetah_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_ear_26 = {
#include "chao/parts_art/child_cheetah_ear_26.inc"
};
static NJS_CNK_OBJECT child_cheetah_ear_26_mount = {
#include "chao/parts_art/child_cheetah_ear_26_mount.inc"
};
static Sint16 child_cheetah_leg_6_polygons[219] = {
#include "chao/parts_art/child_cheetah_leg_6_polygons.inc"
};
static Sint32 child_cheetah_leg_6_vertices[189] = {
#include "chao/parts_art/child_cheetah_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_leg_6_model = {
#include "chao/parts_art/child_cheetah_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_leg_6 = {
#include "chao/parts_art/child_cheetah_leg_6.inc"
};
static NJS_CNK_OBJECT child_cheetah_leg_6_mount = {
#include "chao/parts_art/child_cheetah_leg_6_mount.inc"
};
static Sint16 child_cheetah_leg_13_polygons[219] = {
#include "chao/parts_art/child_cheetah_leg_13_polygons.inc"
};
static Sint32 child_cheetah_leg_13_vertices[189] = {
#include "chao/parts_art/child_cheetah_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_leg_13_model = {
#include "chao/parts_art/child_cheetah_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_leg_13 = {
#include "chao/parts_art/child_cheetah_leg_13.inc"
};
static NJS_CNK_OBJECT child_cheetah_leg_13_mount = {
#include "chao/parts_art/child_cheetah_leg_13_mount.inc"
};
static Sint16 child_cheetah_tail_polygons[277] = {
#include "chao/parts_art/child_cheetah_tail_polygons.inc"
};
static Sint32 child_cheetah_tail_vertices[153] = {
#include "chao/parts_art/child_cheetah_tail_vertices.inc"
};
static NJS_CNK_MODEL child_cheetah_tail_model = {
#include "chao/parts_art/child_cheetah_tail_model.inc"
};
static NJS_CNK_OBJECT child_cheetah_tail = {
#include "chao/parts_art/child_cheetah_tail.inc"
};
static NJS_CNK_OBJECT child_cheetah_tail_mount = {
#include "chao/parts_art/child_cheetah_tail_mount.inc"
};
static NJS_CNK_OBJECT child_dragon_arm_3_child = {
#include "chao/parts_art/child_dragon_arm_3_child.inc"
};
static Sint16 child_dragon_arm_3_polygons[195] = {
#include "chao/parts_art/child_dragon_arm_3_polygons.inc"
};
static Sint32 child_dragon_arm_3_vertices[141] = {
#include "chao/parts_art/child_dragon_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_arm_3_model = {
#include "chao/parts_art/child_dragon_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_dragon_arm_3 = {
#include "chao/parts_art/child_dragon_arm_3.inc"
};
static NJS_CNK_OBJECT child_dragon_arm_3_mount = {
#include "chao/parts_art/child_dragon_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_dragon_arm_10_child = {
#include "chao/parts_art/child_dragon_arm_10_child.inc"
};
static Sint16 child_dragon_arm_10_polygons[195] = {
#include "chao/parts_art/child_dragon_arm_10_polygons.inc"
};
static Sint32 child_dragon_arm_10_vertices[141] = {
#include "chao/parts_art/child_dragon_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_arm_10_model = {
#include "chao/parts_art/child_dragon_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_dragon_arm_10 = {
#include "chao/parts_art/child_dragon_arm_10.inc"
};
static NJS_CNK_OBJECT child_dragon_arm_10_mount = {
#include "chao/parts_art/child_dragon_arm_10_mount.inc"
};
static Sint16 child_dragon_ear_24_polygons[143] = {
#include "chao/parts_art/child_dragon_ear_24_polygons.inc"
};
static Sint32 child_dragon_ear_24_vertices[99] = {
#include "chao/parts_art/child_dragon_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_ear_24_model = {
#include "chao/parts_art/child_dragon_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_dragon_ear_24 = {
#include "chao/parts_art/child_dragon_ear_24.inc"
};
static NJS_CNK_OBJECT child_dragon_ear_24_mount = {
#include "chao/parts_art/child_dragon_ear_24_mount.inc"
};
static Sint16 child_dragon_ear_26_polygons[143] = {
#include "chao/parts_art/child_dragon_ear_26_polygons.inc"
};
static Sint32 child_dragon_ear_26_vertices[99] = {
#include "chao/parts_art/child_dragon_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_ear_26_model = {
#include "chao/parts_art/child_dragon_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_dragon_ear_26 = {
#include "chao/parts_art/child_dragon_ear_26.inc"
};
static NJS_CNK_OBJECT child_dragon_ear_26_mount = {
#include "chao/parts_art/child_dragon_ear_26_mount.inc"
};
static Sint16 child_dragon_horn_30_polygons[117] = {
#include "chao/parts_art/child_dragon_horn_30_polygons.inc"
};
static Sint32 child_dragon_horn_30_vertices[93] = {
#include "chao/parts_art/child_dragon_horn_30_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_horn_30_model = {
#include "chao/parts_art/child_dragon_horn_30_model.inc"
};
static NJS_CNK_OBJECT child_dragon_horn_30 = {
#include "chao/parts_art/child_dragon_horn_30.inc"
};
static NJS_CNK_OBJECT child_dragon_horn_30_mount = {
#include "chao/parts_art/child_dragon_horn_30_mount.inc"
};
static Sint16 child_dragon_horn_31_polygons[117] = {
#include "chao/parts_art/child_dragon_horn_31_polygons.inc"
};
static Sint32 child_dragon_horn_31_vertices[93] = {
#include "chao/parts_art/child_dragon_horn_31_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_horn_31_model = {
#include "chao/parts_art/child_dragon_horn_31_model.inc"
};
static NJS_CNK_OBJECT child_dragon_horn_31 = {
#include "chao/parts_art/child_dragon_horn_31.inc"
};
static NJS_CNK_OBJECT child_dragon_horn_31_mount = {
#include "chao/parts_art/child_dragon_horn_31_mount.inc"
};
static Sint16 child_dragon_leg_6_polygons[175] = {
#include "chao/parts_art/child_dragon_leg_6_polygons.inc"
};
static Sint32 child_dragon_leg_6_vertices[135] = {
#include "chao/parts_art/child_dragon_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_leg_6_model = {
#include "chao/parts_art/child_dragon_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_dragon_leg_6 = {
#include "chao/parts_art/child_dragon_leg_6.inc"
};
static NJS_CNK_OBJECT child_dragon_leg_6_mount = {
#include "chao/parts_art/child_dragon_leg_6_mount.inc"
};
static Sint16 child_dragon_leg_13_polygons[175] = {
#include "chao/parts_art/child_dragon_leg_13_polygons.inc"
};
static Sint32 child_dragon_leg_13_vertices[135] = {
#include "chao/parts_art/child_dragon_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_leg_13_model = {
#include "chao/parts_art/child_dragon_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_dragon_leg_13 = {
#include "chao/parts_art/child_dragon_leg_13.inc"
};
static NJS_CNK_OBJECT child_dragon_leg_13_mount = {
#include "chao/parts_art/child_dragon_leg_13_mount.inc"
};
static Sint16 child_dragon_tail_polygons[233] = {
#include "chao/parts_art/child_dragon_tail_polygons.inc"
};
static Sint32 child_dragon_tail_vertices[177] = {
#include "chao/parts_art/child_dragon_tail_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_tail_model = {
#include "chao/parts_art/child_dragon_tail_model.inc"
};
static NJS_CNK_OBJECT child_dragon_tail = {
#include "chao/parts_art/child_dragon_tail.inc"
};
static NJS_CNK_OBJECT child_dragon_tail_mount = {
#include "chao/parts_art/child_dragon_tail_mount.inc"
};
static Sint16 child_dragon_wing_37_polygons[177] = {
#include "chao/parts_art/child_dragon_wing_37_polygons.inc"
};
static Sint32 child_dragon_wing_37_vertices[165] = {
#include "chao/parts_art/child_dragon_wing_37_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_wing_37_model = {
#include "chao/parts_art/child_dragon_wing_37_model.inc"
};
static NJS_CNK_OBJECT child_dragon_wing_37 = {
#include "chao/parts_art/child_dragon_wing_37.inc"
};
static NJS_CNK_OBJECT child_dragon_wing_37_mount = {
#include "chao/parts_art/child_dragon_wing_37_mount.inc"
};
static Sint16 child_dragon_wing_39_polygons[169] = {
#include "chao/parts_art/child_dragon_wing_39_polygons.inc"
};
static Sint32 child_dragon_wing_39_vertices[165] = {
#include "chao/parts_art/child_dragon_wing_39_vertices.inc"
};
static NJS_CNK_MODEL child_dragon_wing_39_model = {
#include "chao/parts_art/child_dragon_wing_39_model.inc"
};
static NJS_CNK_OBJECT child_dragon_wing_39 = {
#include "chao/parts_art/child_dragon_wing_39.inc"
};
static NJS_CNK_OBJECT child_dragon_wing_39_mount = {
#include "chao/parts_art/child_dragon_wing_39_mount.inc"
};
static NJS_CNK_OBJECT child_seal_arm_3_child = {
#include "chao/parts_art/child_seal_arm_3_child.inc"
};
static Sint16 child_seal_arm_3_polygons[169] = {
#include "chao/parts_art/child_seal_arm_3_polygons.inc"
};
static Sint32 child_seal_arm_3_vertices[123] = {
#include "chao/parts_art/child_seal_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_seal_arm_3_model = {
#include "chao/parts_art/child_seal_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_seal_arm_3 = {
#include "chao/parts_art/child_seal_arm_3.inc"
};
static NJS_CNK_OBJECT child_seal_arm_3_mount = {
#include "chao/parts_art/child_seal_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_seal_arm_10_child = {
#include "chao/parts_art/child_seal_arm_10_child.inc"
};
static Sint16 child_seal_arm_10_polygons[169] = {
#include "chao/parts_art/child_seal_arm_10_polygons.inc"
};
static Sint32 child_seal_arm_10_vertices[123] = {
#include "chao/parts_art/child_seal_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_seal_arm_10_model = {
#include "chao/parts_art/child_seal_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_seal_arm_10 = {
#include "chao/parts_art/child_seal_arm_10.inc"
};
static NJS_CNK_OBJECT child_seal_arm_10_mount = {
#include "chao/parts_art/child_seal_arm_10_mount.inc"
};
static Sint16 child_seal_tail_polygons[209] = {
#include "chao/parts_art/child_seal_tail_polygons.inc"
};
static Sint32 child_seal_tail_vertices[135] = {
#include "chao/parts_art/child_seal_tail_vertices.inc"
};
static NJS_CNK_MODEL child_seal_tail_model = {
#include "chao/parts_art/child_seal_tail_model.inc"
};
static NJS_CNK_OBJECT child_seal_tail = {
#include "chao/parts_art/child_seal_tail.inc"
};
static NJS_CNK_OBJECT child_seal_tail_mount = {
#include "chao/parts_art/child_seal_tail_mount.inc"
};
static NJS_CNK_OBJECT child_gorilla_arm_3_child = {
#include "chao/parts_art/child_gorilla_arm_3_child.inc"
};
static Sint16 child_gorilla_arm_3_polygons[215] = {
#include "chao/parts_art/child_gorilla_arm_3_polygons.inc"
};
static Sint32 child_gorilla_arm_3_vertices[141] = {
#include "chao/parts_art/child_gorilla_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_arm_3_model = {
#include "chao/parts_art/child_gorilla_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_arm_3 = {
#include "chao/parts_art/child_gorilla_arm_3.inc"
};
static NJS_CNK_OBJECT child_gorilla_arm_3_mount = {
#include "chao/parts_art/child_gorilla_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_gorilla_arm_10_child = {
#include "chao/parts_art/child_gorilla_arm_10_child.inc"
};
static Sint16 child_gorilla_arm_10_polygons[215] = {
#include "chao/parts_art/child_gorilla_arm_10_polygons.inc"
};
static Sint32 child_gorilla_arm_10_vertices[141] = {
#include "chao/parts_art/child_gorilla_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_arm_10_model = {
#include "chao/parts_art/child_gorilla_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_arm_10 = {
#include "chao/parts_art/child_gorilla_arm_10.inc"
};
static NJS_CNK_OBJECT child_gorilla_arm_10_mount = {
#include "chao/parts_art/child_gorilla_arm_10_mount.inc"
};
static Sint16 child_gorilla_ear_24_polygons[157] = {
#include "chao/parts_art/child_gorilla_ear_24_polygons.inc"
};
static Sint32 child_gorilla_ear_24_vertices[105] = {
#include "chao/parts_art/child_gorilla_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_ear_24_model = {
#include "chao/parts_art/child_gorilla_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_ear_24 = {
#include "chao/parts_art/child_gorilla_ear_24.inc"
};
static NJS_CNK_OBJECT child_gorilla_ear_24_mount = {
#include "chao/parts_art/child_gorilla_ear_24_mount.inc"
};
static Sint16 child_gorilla_ear_26_polygons[157] = {
#include "chao/parts_art/child_gorilla_ear_26_polygons.inc"
};
static Sint32 child_gorilla_ear_26_vertices[105] = {
#include "chao/parts_art/child_gorilla_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_ear_26_model = {
#include "chao/parts_art/child_gorilla_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_ear_26 = {
#include "chao/parts_art/child_gorilla_ear_26.inc"
};
static NJS_CNK_OBJECT child_gorilla_ear_26_mount = {
#include "chao/parts_art/child_gorilla_ear_26_mount.inc"
};
static Sint16 child_gorilla_forehead_polygons[131] = {
#include "chao/parts_art/child_gorilla_forehead_polygons.inc"
};
static Sint32 child_gorilla_forehead_vertices[147] = {
#include "chao/parts_art/child_gorilla_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_forehead_model = {
#include "chao/parts_art/child_gorilla_forehead_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_forehead = {
#include "chao/parts_art/child_gorilla_forehead.inc"
};
static NJS_CNK_OBJECT child_gorilla_forehead_mount = {
#include "chao/parts_art/child_gorilla_forehead_mount.inc"
};
static Sint16 child_gorilla_leg_6_polygons[181] = {
#include "chao/parts_art/child_gorilla_leg_6_polygons.inc"
};
static Sint32 child_gorilla_leg_6_vertices[129] = {
#include "chao/parts_art/child_gorilla_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_leg_6_model = {
#include "chao/parts_art/child_gorilla_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_leg_6 = {
#include "chao/parts_art/child_gorilla_leg_6.inc"
};
static NJS_CNK_OBJECT child_gorilla_leg_6_mount = {
#include "chao/parts_art/child_gorilla_leg_6_mount.inc"
};
static Sint16 child_gorilla_leg_13_polygons[181] = {
#include "chao/parts_art/child_gorilla_leg_13_polygons.inc"
};
static Sint32 child_gorilla_leg_13_vertices[129] = {
#include "chao/parts_art/child_gorilla_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_gorilla_leg_13_model = {
#include "chao/parts_art/child_gorilla_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_gorilla_leg_13 = {
#include "chao/parts_art/child_gorilla_leg_13.inc"
};
static NJS_CNK_OBJECT child_gorilla_leg_13_mount = {
#include "chao/parts_art/child_gorilla_leg_13_mount.inc"
};
static NJS_CNK_OBJECT child_boar_arm_3_child = {
#include "chao/parts_art/child_boar_arm_3_child.inc"
};
static Sint16 child_boar_arm_3_polygons[289] = {
#include "chao/parts_art/child_boar_arm_3_polygons.inc"
};
static Sint32 child_boar_arm_3_vertices[159] = {
#include "chao/parts_art/child_boar_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_boar_arm_3_model = {
#include "chao/parts_art/child_boar_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_boar_arm_3 = {
#include "chao/parts_art/child_boar_arm_3.inc"
};
static NJS_CNK_OBJECT child_boar_arm_3_mount = {
#include "chao/parts_art/child_boar_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_boar_arm_10_child = {
#include "chao/parts_art/child_boar_arm_10_child.inc"
};
static Sint16 child_boar_arm_10_polygons[281] = {
#include "chao/parts_art/child_boar_arm_10_polygons.inc"
};
static Sint32 child_boar_arm_10_vertices[159] = {
#include "chao/parts_art/child_boar_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_boar_arm_10_model = {
#include "chao/parts_art/child_boar_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_boar_arm_10 = {
#include "chao/parts_art/child_boar_arm_10.inc"
};
static NJS_CNK_OBJECT child_boar_arm_10_mount = {
#include "chao/parts_art/child_boar_arm_10_mount.inc"
};
static Sint16 child_boar_ear_24_polygons[135] = {
#include "chao/parts_art/child_boar_ear_24_polygons.inc"
};
static Sint32 child_boar_ear_24_vertices[81] = {
#include "chao/parts_art/child_boar_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_boar_ear_24_model = {
#include "chao/parts_art/child_boar_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_boar_ear_24 = {
#include "chao/parts_art/child_boar_ear_24.inc"
};
static NJS_CNK_OBJECT child_boar_ear_24_mount = {
#include "chao/parts_art/child_boar_ear_24_mount.inc"
};
static Sint16 child_boar_ear_26_polygons[135] = {
#include "chao/parts_art/child_boar_ear_26_polygons.inc"
};
static Sint32 child_boar_ear_26_vertices[81] = {
#include "chao/parts_art/child_boar_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_boar_ear_26_model = {
#include "chao/parts_art/child_boar_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_boar_ear_26 = {
#include "chao/parts_art/child_boar_ear_26.inc"
};
static NJS_CNK_OBJECT child_boar_ear_26_mount = {
#include "chao/parts_art/child_boar_ear_26_mount.inc"
};
static Sint16 child_boar_leg_6_polygons[265] = {
#include "chao/parts_art/child_boar_leg_6_polygons.inc"
};
static Sint32 child_boar_leg_6_vertices[159] = {
#include "chao/parts_art/child_boar_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_boar_leg_6_model = {
#include "chao/parts_art/child_boar_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_boar_leg_6 = {
#include "chao/parts_art/child_boar_leg_6.inc"
};
static NJS_CNK_OBJECT child_boar_leg_6_mount = {
#include "chao/parts_art/child_boar_leg_6_mount.inc"
};
static Sint16 child_boar_leg_13_polygons[261] = {
#include "chao/parts_art/child_boar_leg_13_polygons.inc"
};
static Sint32 child_boar_leg_13_vertices[159] = {
#include "chao/parts_art/child_boar_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_boar_leg_13_model = {
#include "chao/parts_art/child_boar_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_boar_leg_13 = {
#include "chao/parts_art/child_boar_leg_13.inc"
};
static NJS_CNK_OBJECT child_boar_leg_13_mount = {
#include "chao/parts_art/child_boar_leg_13_mount.inc"
};
static Sint16 child_boar_tail_polygons[131] = {
#include "chao/parts_art/child_boar_tail_polygons.inc"
};
static Sint32 child_boar_tail_vertices[99] = {
#include "chao/parts_art/child_boar_tail_vertices.inc"
};
static NJS_CNK_MODEL child_boar_tail_model = {
#include "chao/parts_art/child_boar_tail_model.inc"
};
static NJS_CNK_OBJECT child_boar_tail = {
#include "chao/parts_art/child_boar_tail.inc"
};
static NJS_CNK_OBJECT child_boar_tail_mount = {
#include "chao/parts_art/child_boar_tail_mount.inc"
};
static Sint16 child_boar_face_polygons[201] = {
#include "chao/parts_art/child_boar_face_polygons.inc"
};
static Sint32 child_boar_face_vertices[171] = {
#include "chao/parts_art/child_boar_face_vertices.inc"
};
static NJS_CNK_MODEL child_boar_face_model = {
#include "chao/parts_art/child_boar_face_model.inc"
};
static NJS_CNK_OBJECT child_boar_face = {
#include "chao/parts_art/child_boar_face.inc"
};
static NJS_CNK_OBJECT child_boar_face_mount = {
#include "chao/parts_art/child_boar_face_mount.inc"
};
static Sint16 child_condor_ear_24_polygons[107] = {
#include "chao/parts_art/child_condor_ear_24_polygons.inc"
};
static Sint32 child_condor_ear_24_vertices[87] = {
#include "chao/parts_art/child_condor_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_condor_ear_24_model = {
#include "chao/parts_art/child_condor_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_condor_ear_24 = {
#include "chao/parts_art/child_condor_ear_24.inc"
};
static NJS_CNK_OBJECT child_condor_ear_24_mount = {
#include "chao/parts_art/child_condor_ear_24_mount.inc"
};
static Sint16 child_condor_ear_26_polygons[107] = {
#include "chao/parts_art/child_condor_ear_26_polygons.inc"
};
static Sint32 child_condor_ear_26_vertices[87] = {
#include "chao/parts_art/child_condor_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_condor_ear_26_model = {
#include "chao/parts_art/child_condor_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_condor_ear_26 = {
#include "chao/parts_art/child_condor_ear_26.inc"
};
static NJS_CNK_OBJECT child_condor_ear_26_mount = {
#include "chao/parts_art/child_condor_ear_26_mount.inc"
};
static Sint16 child_condor_leg_6_polygons[187] = {
#include "chao/parts_art/child_condor_leg_6_polygons.inc"
};
static Sint32 child_condor_leg_6_vertices[129] = {
#include "chao/parts_art/child_condor_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_condor_leg_6_model = {
#include "chao/parts_art/child_condor_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_condor_leg_6 = {
#include "chao/parts_art/child_condor_leg_6.inc"
};
static NJS_CNK_OBJECT child_condor_leg_6_mount = {
#include "chao/parts_art/child_condor_leg_6_mount.inc"
};
static Sint16 child_condor_leg_13_polygons[187] = {
#include "chao/parts_art/child_condor_leg_13_polygons.inc"
};
static Sint32 child_condor_leg_13_vertices[129] = {
#include "chao/parts_art/child_condor_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_condor_leg_13_model = {
#include "chao/parts_art/child_condor_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_condor_leg_13 = {
#include "chao/parts_art/child_condor_leg_13.inc"
};
static NJS_CNK_OBJECT child_condor_leg_13_mount = {
#include "chao/parts_art/child_condor_leg_13_mount.inc"
};
static Sint16 child_condor_tail_polygons[143] = {
#include "chao/parts_art/child_condor_tail_polygons.inc"
};
static Sint32 child_condor_tail_vertices[111] = {
#include "chao/parts_art/child_condor_tail_vertices.inc"
};
static NJS_CNK_MODEL child_condor_tail_model = {
#include "chao/parts_art/child_condor_tail_model.inc"
};
static NJS_CNK_OBJECT child_condor_tail = {
#include "chao/parts_art/child_condor_tail.inc"
};
static NJS_CNK_OBJECT child_condor_tail_mount = {
#include "chao/parts_art/child_condor_tail_mount.inc"
};
static Sint16 child_condor_wing_37_polygons[107] = {
#include "chao/parts_art/child_condor_wing_37_polygons.inc"
};
static Sint32 child_condor_wing_37_vertices[81] = {
#include "chao/parts_art/child_condor_wing_37_vertices.inc"
};
static NJS_CNK_MODEL child_condor_wing_37_model = {
#include "chao/parts_art/child_condor_wing_37_model.inc"
};
static NJS_CNK_OBJECT child_condor_wing_37 = {
#include "chao/parts_art/child_condor_wing_37.inc"
};
static NJS_CNK_OBJECT child_condor_wing_37_mount = {
#include "chao/parts_art/child_condor_wing_37_mount.inc"
};
static Sint16 child_condor_wing_39_polygons[107] = {
#include "chao/parts_art/child_condor_wing_39_polygons.inc"
};
static Sint32 child_condor_wing_39_vertices[81] = {
#include "chao/parts_art/child_condor_wing_39_vertices.inc"
};
static NJS_CNK_MODEL child_condor_wing_39_model = {
#include "chao/parts_art/child_condor_wing_39_model.inc"
};
static NJS_CNK_OBJECT child_condor_wing_39 = {
#include "chao/parts_art/child_condor_wing_39.inc"
};
static NJS_CNK_OBJECT child_condor_wing_39_mount = {
#include "chao/parts_art/child_condor_wing_39_mount.inc"
};
static Sint16 child_peacock_forehead_polygons[195] = {
#include "chao/parts_art/child_peacock_forehead_polygons.inc"
};
static Sint32 child_peacock_forehead_vertices[177] = {
#include "chao/parts_art/child_peacock_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_peacock_forehead_model = {
#include "chao/parts_art/child_peacock_forehead_model.inc"
};
static NJS_CNK_OBJECT child_peacock_forehead = {
#include "chao/parts_art/child_peacock_forehead.inc"
};
static NJS_CNK_OBJECT child_peacock_forehead_mount = {
#include "chao/parts_art/child_peacock_forehead_mount.inc"
};
static Sint16 child_peacock_leg_6_polygons[197] = {
#include "chao/parts_art/child_peacock_leg_6_polygons.inc"
};
static Sint32 child_peacock_leg_6_vertices[123] = {
#include "chao/parts_art/child_peacock_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_peacock_leg_6_model = {
#include "chao/parts_art/child_peacock_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_peacock_leg_6 = {
#include "chao/parts_art/child_peacock_leg_6.inc"
};
static NJS_CNK_OBJECT child_peacock_leg_6_mount = {
#include "chao/parts_art/child_peacock_leg_6_mount.inc"
};
static Sint16 child_peacock_leg_13_polygons[197] = {
#include "chao/parts_art/child_peacock_leg_13_polygons.inc"
};
static Sint32 child_peacock_leg_13_vertices[123] = {
#include "chao/parts_art/child_peacock_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_peacock_leg_13_model = {
#include "chao/parts_art/child_peacock_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_peacock_leg_13 = {
#include "chao/parts_art/child_peacock_leg_13.inc"
};
static NJS_CNK_OBJECT child_peacock_leg_13_mount = {
#include "chao/parts_art/child_peacock_leg_13_mount.inc"
};
static Sint16 child_peacock_tail_polygons[275] = {
#include "chao/parts_art/child_peacock_tail_polygons.inc"
};
static Sint32 child_peacock_tail_vertices[387] = {
#include "chao/parts_art/child_peacock_tail_vertices.inc"
};
static NJS_CNK_MODEL child_peacock_tail_model = {
#include "chao/parts_art/child_peacock_tail_model.inc"
};
static NJS_CNK_OBJECT child_peacock_tail = {
#include "chao/parts_art/child_peacock_tail.inc"
};
static NJS_CNK_OBJECT child_peacock_tail_mount = {
#include "chao/parts_art/child_peacock_tail_mount.inc"
};
static Sint16 child_peacock_wing_37_polygons[101] = {
#include "chao/parts_art/child_peacock_wing_37_polygons.inc"
};
static Sint32 child_peacock_wing_37_vertices[69] = {
#include "chao/parts_art/child_peacock_wing_37_vertices.inc"
};
static NJS_CNK_MODEL child_peacock_wing_37_model = {
#include "chao/parts_art/child_peacock_wing_37_model.inc"
};
static NJS_CNK_OBJECT child_peacock_wing_37 = {
#include "chao/parts_art/child_peacock_wing_37.inc"
};
static NJS_CNK_OBJECT child_peacock_wing_37_mount = {
#include "chao/parts_art/child_peacock_wing_37_mount.inc"
};
static Sint16 child_peacock_wing_39_polygons[101] = {
#include "chao/parts_art/child_peacock_wing_39_polygons.inc"
};
static Sint32 child_peacock_wing_39_vertices[69] = {
#include "chao/parts_art/child_peacock_wing_39_vertices.inc"
};
static NJS_CNK_MODEL child_peacock_wing_39_model = {
#include "chao/parts_art/child_peacock_wing_39_model.inc"
};
static NJS_CNK_OBJECT child_peacock_wing_39 = {
#include "chao/parts_art/child_peacock_wing_39.inc"
};
static NJS_CNK_OBJECT child_peacock_wing_39_mount = {
#include "chao/parts_art/child_peacock_wing_39_mount.inc"
};
static NJS_CNK_OBJECT child_bear_arm_3_child = {
#include "chao/parts_art/child_bear_arm_3_child.inc"
};
static Sint16 child_bear_arm_3_polygons[237] = {
#include "chao/parts_art/child_bear_arm_3_polygons.inc"
};
static Sint32 child_bear_arm_3_vertices[183] = {
#include "chao/parts_art/child_bear_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_bear_arm_3_model = {
#include "chao/parts_art/child_bear_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_bear_arm_3 = {
#include "chao/parts_art/child_bear_arm_3.inc"
};
static NJS_CNK_OBJECT child_bear_arm_3_mount = {
#include "chao/parts_art/child_bear_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_bear_arm_10_child = {
#include "chao/parts_art/child_bear_arm_10_child.inc"
};
static Sint16 child_bear_arm_10_polygons[237] = {
#include "chao/parts_art/child_bear_arm_10_polygons.inc"
};
static Sint32 child_bear_arm_10_vertices[183] = {
#include "chao/parts_art/child_bear_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_bear_arm_10_model = {
#include "chao/parts_art/child_bear_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_bear_arm_10 = {
#include "chao/parts_art/child_bear_arm_10.inc"
};
static NJS_CNK_OBJECT child_bear_arm_10_mount = {
#include "chao/parts_art/child_bear_arm_10_mount.inc"
};
static Sint16 child_bear_ear_24_polygons[157] = {
#include "chao/parts_art/child_bear_ear_24_polygons.inc"
};
static Sint32 child_bear_ear_24_vertices[111] = {
#include "chao/parts_art/child_bear_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_bear_ear_24_model = {
#include "chao/parts_art/child_bear_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_bear_ear_24 = {
#include "chao/parts_art/child_bear_ear_24.inc"
};
static NJS_CNK_OBJECT child_bear_ear_24_mount = {
#include "chao/parts_art/child_bear_ear_24_mount.inc"
};
static Sint16 child_bear_ear_26_polygons[157] = {
#include "chao/parts_art/child_bear_ear_26_polygons.inc"
};
static Sint32 child_bear_ear_26_vertices[111] = {
#include "chao/parts_art/child_bear_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_bear_ear_26_model = {
#include "chao/parts_art/child_bear_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_bear_ear_26 = {
#include "chao/parts_art/child_bear_ear_26.inc"
};
static NJS_CNK_OBJECT child_bear_ear_26_mount = {
#include "chao/parts_art/child_bear_ear_26_mount.inc"
};
static Sint16 child_bear_leg_6_polygons[237] = {
#include "chao/parts_art/child_bear_leg_6_polygons.inc"
};
static Sint32 child_bear_leg_6_vertices[189] = {
#include "chao/parts_art/child_bear_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_bear_leg_6_model = {
#include "chao/parts_art/child_bear_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_bear_leg_6 = {
#include "chao/parts_art/child_bear_leg_6.inc"
};
static NJS_CNK_OBJECT child_bear_leg_6_mount = {
#include "chao/parts_art/child_bear_leg_6_mount.inc"
};
static Sint16 child_bear_leg_13_polygons[237] = {
#include "chao/parts_art/child_bear_leg_13_polygons.inc"
};
static Sint32 child_bear_leg_13_vertices[189] = {
#include "chao/parts_art/child_bear_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_bear_leg_13_model = {
#include "chao/parts_art/child_bear_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_bear_leg_13 = {
#include "chao/parts_art/child_bear_leg_13.inc"
};
static NJS_CNK_OBJECT child_bear_leg_13_mount = {
#include "chao/parts_art/child_bear_leg_13_mount.inc"
};
static Sint16 child_parrot_forehead_polygons[165] = {
#include "chao/parts_art/child_parrot_forehead_polygons.inc"
};
static Sint32 child_parrot_forehead_vertices[123] = {
#include "chao/parts_art/child_parrot_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_parrot_forehead_model = {
#include "chao/parts_art/child_parrot_forehead_model.inc"
};
static NJS_CNK_OBJECT child_parrot_forehead = {
#include "chao/parts_art/child_parrot_forehead.inc"
};
static NJS_CNK_OBJECT child_parrot_forehead_mount = {
#include "chao/parts_art/child_parrot_forehead_mount.inc"
};
static Sint16 child_parrot_leg_6_polygons[205] = {
#include "chao/parts_art/child_parrot_leg_6_polygons.inc"
};
static Sint32 child_parrot_leg_6_vertices[129] = {
#include "chao/parts_art/child_parrot_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_parrot_leg_6_model = {
#include "chao/parts_art/child_parrot_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_parrot_leg_6 = {
#include "chao/parts_art/child_parrot_leg_6.inc"
};
static NJS_CNK_OBJECT child_parrot_leg_6_mount = {
#include "chao/parts_art/child_parrot_leg_6_mount.inc"
};
static Sint16 child_parrot_leg_13_polygons[199] = {
#include "chao/parts_art/child_parrot_leg_13_polygons.inc"
};
static Sint32 child_parrot_leg_13_vertices[129] = {
#include "chao/parts_art/child_parrot_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_parrot_leg_13_model = {
#include "chao/parts_art/child_parrot_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_parrot_leg_13 = {
#include "chao/parts_art/child_parrot_leg_13.inc"
};
static NJS_CNK_OBJECT child_parrot_leg_13_mount = {
#include "chao/parts_art/child_parrot_leg_13_mount.inc"
};
static Sint16 child_parrot_tail_polygons[175] = {
#include "chao/parts_art/child_parrot_tail_polygons.inc"
};
static Sint32 child_parrot_tail_vertices[135] = {
#include "chao/parts_art/child_parrot_tail_vertices.inc"
};
static NJS_CNK_MODEL child_parrot_tail_model = {
#include "chao/parts_art/child_parrot_tail_model.inc"
};
static NJS_CNK_OBJECT child_parrot_tail = {
#include "chao/parts_art/child_parrot_tail.inc"
};
static NJS_CNK_OBJECT child_parrot_tail_mount = {
#include "chao/parts_art/child_parrot_tail_mount.inc"
};
static Sint16 child_parrot_wing_37_polygons[119] = {
#include "chao/parts_art/child_parrot_wing_37_polygons.inc"
};
static Sint32 child_parrot_wing_37_vertices[87] = {
#include "chao/parts_art/child_parrot_wing_37_vertices.inc"
};
static NJS_CNK_MODEL child_parrot_wing_37_model = {
#include "chao/parts_art/child_parrot_wing_37_model.inc"
};
static NJS_CNK_OBJECT child_parrot_wing_37 = {
#include "chao/parts_art/child_parrot_wing_37.inc"
};
static NJS_CNK_OBJECT child_parrot_wing_37_mount = {
#include "chao/parts_art/child_parrot_wing_37_mount.inc"
};
static Sint16 child_parrot_wing_39_polygons[119] = {
#include "chao/parts_art/child_parrot_wing_39_polygons.inc"
};
static Sint32 child_parrot_wing_39_vertices[87] = {
#include "chao/parts_art/child_parrot_wing_39_vertices.inc"
};
static NJS_CNK_MODEL child_parrot_wing_39_model = {
#include "chao/parts_art/child_parrot_wing_39_model.inc"
};
static NJS_CNK_OBJECT child_parrot_wing_39 = {
#include "chao/parts_art/child_parrot_wing_39.inc"
};
static NJS_CNK_OBJECT child_parrot_wing_39_mount = {
#include "chao/parts_art/child_parrot_wing_39_mount.inc"
};
static NJS_CNK_OBJECT child_penguin_arm_3_child = {
#include "chao/parts_art/child_penguin_arm_3_child.inc"
};
static Sint16 child_penguin_arm_3_polygons[119] = {
#include "chao/parts_art/child_penguin_arm_3_polygons.inc"
};
static Sint32 child_penguin_arm_3_vertices[93] = {
#include "chao/parts_art/child_penguin_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_penguin_arm_3_model = {
#include "chao/parts_art/child_penguin_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_penguin_arm_3 = {
#include "chao/parts_art/child_penguin_arm_3.inc"
};
static NJS_CNK_OBJECT child_penguin_arm_3_mount = {
#include "chao/parts_art/child_penguin_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_penguin_arm_10_child = {
#include "chao/parts_art/child_penguin_arm_10_child.inc"
};
static Sint16 child_penguin_arm_10_polygons[119] = {
#include "chao/parts_art/child_penguin_arm_10_polygons.inc"
};
static Sint32 child_penguin_arm_10_vertices[93] = {
#include "chao/parts_art/child_penguin_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_penguin_arm_10_model = {
#include "chao/parts_art/child_penguin_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_penguin_arm_10 = {
#include "chao/parts_art/child_penguin_arm_10.inc"
};
static NJS_CNK_OBJECT child_penguin_arm_10_mount = {
#include "chao/parts_art/child_penguin_arm_10_mount.inc"
};
static Sint16 child_penguin_forehead_polygons[157] = {
#include "chao/parts_art/child_penguin_forehead_polygons.inc"
};
static Sint32 child_penguin_forehead_vertices[129] = {
#include "chao/parts_art/child_penguin_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_penguin_forehead_model = {
#include "chao/parts_art/child_penguin_forehead_model.inc"
};
static NJS_CNK_OBJECT child_penguin_forehead = {
#include "chao/parts_art/child_penguin_forehead.inc"
};
static NJS_CNK_OBJECT child_penguin_forehead_mount = {
#include "chao/parts_art/child_penguin_forehead_mount.inc"
};
static Sint16 child_penguin_leg_6_polygons[223] = {
#include "chao/parts_art/child_penguin_leg_6_polygons.inc"
};
static Sint32 child_penguin_leg_6_vertices[135] = {
#include "chao/parts_art/child_penguin_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_penguin_leg_6_model = {
#include "chao/parts_art/child_penguin_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_penguin_leg_6 = {
#include "chao/parts_art/child_penguin_leg_6.inc"
};
static NJS_CNK_OBJECT child_penguin_leg_6_mount = {
#include "chao/parts_art/child_penguin_leg_6_mount.inc"
};
static Sint16 child_penguin_leg_13_polygons[223] = {
#include "chao/parts_art/child_penguin_leg_13_polygons.inc"
};
static Sint32 child_penguin_leg_13_vertices[135] = {
#include "chao/parts_art/child_penguin_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_penguin_leg_13_model = {
#include "chao/parts_art/child_penguin_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_penguin_leg_13 = {
#include "chao/parts_art/child_penguin_leg_13.inc"
};
static NJS_CNK_OBJECT child_penguin_leg_13_mount = {
#include "chao/parts_art/child_penguin_leg_13_mount.inc"
};
static Sint16 child_phoenix_forehead_polygons[215] = {
#include "chao/parts_art/child_phoenix_forehead_polygons.inc"
};
static Sint32 child_phoenix_forehead_vertices[171] = {
#include "chao/parts_art/child_phoenix_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_phoenix_forehead_model = {
#include "chao/parts_art/child_phoenix_forehead_model.inc"
};
static NJS_CNK_OBJECT child_phoenix_forehead = {
#include "chao/parts_art/child_phoenix_forehead.inc"
};
static NJS_CNK_OBJECT child_phoenix_forehead_mount = {
#include "chao/parts_art/child_phoenix_forehead_mount.inc"
};
static Sint16 child_phoenix_leg_6_polygons[239] = {
#include "chao/parts_art/child_phoenix_leg_6_polygons.inc"
};
static Sint32 child_phoenix_leg_6_vertices[135] = {
#include "chao/parts_art/child_phoenix_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_phoenix_leg_6_model = {
#include "chao/parts_art/child_phoenix_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_phoenix_leg_6 = {
#include "chao/parts_art/child_phoenix_leg_6.inc"
};
static NJS_CNK_OBJECT child_phoenix_leg_6_mount = {
#include "chao/parts_art/child_phoenix_leg_6_mount.inc"
};
static Sint16 child_phoenix_leg_13_polygons[233] = {
#include "chao/parts_art/child_phoenix_leg_13_polygons.inc"
};
static Sint32 child_phoenix_leg_13_vertices[135] = {
#include "chao/parts_art/child_phoenix_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_phoenix_leg_13_model = {
#include "chao/parts_art/child_phoenix_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_phoenix_leg_13 = {
#include "chao/parts_art/child_phoenix_leg_13.inc"
};
static NJS_CNK_OBJECT child_phoenix_leg_13_mount = {
#include "chao/parts_art/child_phoenix_leg_13_mount.inc"
};
static Sint16 child_phoenix_tail_polygons[153] = {
#include "chao/parts_art/child_phoenix_tail_polygons.inc"
};
static Sint32 child_phoenix_tail_vertices[171] = {
#include "chao/parts_art/child_phoenix_tail_vertices.inc"
};
static NJS_CNK_MODEL child_phoenix_tail_model = {
#include "chao/parts_art/child_phoenix_tail_model.inc"
};
static NJS_CNK_OBJECT child_phoenix_tail = {
#include "chao/parts_art/child_phoenix_tail.inc"
};
static NJS_CNK_OBJECT child_phoenix_tail_mount = {
#include "chao/parts_art/child_phoenix_tail_mount.inc"
};
static Sint16 child_phoenix_wing_37_polygons[193] = {
#include "chao/parts_art/child_phoenix_wing_37_polygons.inc"
};
static Sint32 child_phoenix_wing_37_vertices[135] = {
#include "chao/parts_art/child_phoenix_wing_37_vertices.inc"
};
static NJS_CNK_MODEL child_phoenix_wing_37_model = {
#include "chao/parts_art/child_phoenix_wing_37_model.inc"
};
static NJS_CNK_OBJECT child_phoenix_wing_37 = {
#include "chao/parts_art/child_phoenix_wing_37.inc"
};
static NJS_CNK_OBJECT child_phoenix_wing_37_mount = {
#include "chao/parts_art/child_phoenix_wing_37_mount.inc"
};
static Sint16 child_phoenix_wing_39_polygons[193] = {
#include "chao/parts_art/child_phoenix_wing_39_polygons.inc"
};
static Sint32 child_phoenix_wing_39_vertices[135] = {
#include "chao/parts_art/child_phoenix_wing_39_vertices.inc"
};
static NJS_CNK_MODEL child_phoenix_wing_39_model = {
#include "chao/parts_art/child_phoenix_wing_39_model.inc"
};
static NJS_CNK_OBJECT child_phoenix_wing_39 = {
#include "chao/parts_art/child_phoenix_wing_39.inc"
};
static NJS_CNK_OBJECT child_phoenix_wing_39_mount = {
#include "chao/parts_art/child_phoenix_wing_39_mount.inc"
};
static NJS_CNK_OBJECT child_otter_arm_3_child = {
#include "chao/parts_art/child_otter_arm_3_child.inc"
};
static Sint16 child_otter_arm_3_polygons[211] = {
#include "chao/parts_art/child_otter_arm_3_polygons.inc"
};
static Sint32 child_otter_arm_3_vertices[171] = {
#include "chao/parts_art/child_otter_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_otter_arm_3_model = {
#include "chao/parts_art/child_otter_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_otter_arm_3 = {
#include "chao/parts_art/child_otter_arm_3.inc"
};
static NJS_CNK_OBJECT child_otter_arm_3_mount = {
#include "chao/parts_art/child_otter_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_otter_arm_10_child = {
#include "chao/parts_art/child_otter_arm_10_child.inc"
};
static Sint16 child_otter_arm_10_polygons[211] = {
#include "chao/parts_art/child_otter_arm_10_polygons.inc"
};
static Sint32 child_otter_arm_10_vertices[171] = {
#include "chao/parts_art/child_otter_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_otter_arm_10_model = {
#include "chao/parts_art/child_otter_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_otter_arm_10 = {
#include "chao/parts_art/child_otter_arm_10.inc"
};
static NJS_CNK_OBJECT child_otter_arm_10_mount = {
#include "chao/parts_art/child_otter_arm_10_mount.inc"
};
static Sint16 child_otter_ear_24_polygons[149] = {
#include "chao/parts_art/child_otter_ear_24_polygons.inc"
};
static Sint32 child_otter_ear_24_vertices[75] = {
#include "chao/parts_art/child_otter_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_otter_ear_24_model = {
#include "chao/parts_art/child_otter_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_otter_ear_24 = {
#include "chao/parts_art/child_otter_ear_24.inc"
};
static NJS_CNK_OBJECT child_otter_ear_24_mount = {
#include "chao/parts_art/child_otter_ear_24_mount.inc"
};
static Sint16 child_otter_ear_26_polygons[149] = {
#include "chao/parts_art/child_otter_ear_26_polygons.inc"
};
static Sint32 child_otter_ear_26_vertices[75] = {
#include "chao/parts_art/child_otter_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_otter_ear_26_model = {
#include "chao/parts_art/child_otter_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_otter_ear_26 = {
#include "chao/parts_art/child_otter_ear_26.inc"
};
static NJS_CNK_OBJECT child_otter_ear_26_mount = {
#include "chao/parts_art/child_otter_ear_26_mount.inc"
};
static Sint16 child_otter_leg_6_polygons[201] = {
#include "chao/parts_art/child_otter_leg_6_polygons.inc"
};
static Sint32 child_otter_leg_6_vertices[153] = {
#include "chao/parts_art/child_otter_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_otter_leg_6_model = {
#include "chao/parts_art/child_otter_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_otter_leg_6 = {
#include "chao/parts_art/child_otter_leg_6.inc"
};
static NJS_CNK_OBJECT child_otter_leg_6_mount = {
#include "chao/parts_art/child_otter_leg_6_mount.inc"
};
static Sint16 child_otter_leg_13_polygons[201] = {
#include "chao/parts_art/child_otter_leg_13_polygons.inc"
};
static Sint32 child_otter_leg_13_vertices[153] = {
#include "chao/parts_art/child_otter_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_otter_leg_13_model = {
#include "chao/parts_art/child_otter_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_otter_leg_13 = {
#include "chao/parts_art/child_otter_leg_13.inc"
};
static NJS_CNK_OBJECT child_otter_leg_13_mount = {
#include "chao/parts_art/child_otter_leg_13_mount.inc"
};
static Sint16 child_otter_tail_polygons[145] = {
#include "chao/parts_art/child_otter_tail_polygons.inc"
};
static Sint32 child_otter_tail_vertices[105] = {
#include "chao/parts_art/child_otter_tail_vertices.inc"
};
static NJS_CNK_MODEL child_otter_tail_model = {
#include "chao/parts_art/child_otter_tail_model.inc"
};
static NJS_CNK_OBJECT child_otter_tail = {
#include "chao/parts_art/child_otter_tail.inc"
};
static NJS_CNK_OBJECT child_otter_tail_mount = {
#include "chao/parts_art/child_otter_tail_mount.inc"
};
static NJS_CNK_OBJECT child_sheep_arm_3_child = {
#include "chao/parts_art/child_sheep_arm_3_child.inc"
};
static Sint16 child_sheep_arm_3_polygons[207] = {
#include "chao/parts_art/child_sheep_arm_3_polygons.inc"
};
static Sint32 child_sheep_arm_3_vertices[147] = {
#include "chao/parts_art/child_sheep_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_arm_3_model = {
#include "chao/parts_art/child_sheep_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_sheep_arm_3 = {
#include "chao/parts_art/child_sheep_arm_3.inc"
};
static NJS_CNK_OBJECT child_sheep_arm_3_mount = {
#include "chao/parts_art/child_sheep_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_sheep_arm_10_child = {
#include "chao/parts_art/child_sheep_arm_10_child.inc"
};
static Sint16 child_sheep_arm_10_polygons[207] = {
#include "chao/parts_art/child_sheep_arm_10_polygons.inc"
};
static Sint32 child_sheep_arm_10_vertices[147] = {
#include "chao/parts_art/child_sheep_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_arm_10_model = {
#include "chao/parts_art/child_sheep_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_sheep_arm_10 = {
#include "chao/parts_art/child_sheep_arm_10.inc"
};
static NJS_CNK_OBJECT child_sheep_arm_10_mount = {
#include "chao/parts_art/child_sheep_arm_10_mount.inc"
};
static Sint16 child_sheep_ear_24_polygons[125] = {
#include "chao/parts_art/child_sheep_ear_24_polygons.inc"
};
static Sint32 child_sheep_ear_24_vertices[93] = {
#include "chao/parts_art/child_sheep_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_ear_24_model = {
#include "chao/parts_art/child_sheep_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_sheep_ear_24 = {
#include "chao/parts_art/child_sheep_ear_24.inc"
};
static NJS_CNK_OBJECT child_sheep_ear_24_mount = {
#include "chao/parts_art/child_sheep_ear_24_mount.inc"
};
static Sint16 child_sheep_ear_26_polygons[125] = {
#include "chao/parts_art/child_sheep_ear_26_polygons.inc"
};
static Sint32 child_sheep_ear_26_vertices[93] = {
#include "chao/parts_art/child_sheep_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_ear_26_model = {
#include "chao/parts_art/child_sheep_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_sheep_ear_26 = {
#include "chao/parts_art/child_sheep_ear_26.inc"
};
static NJS_CNK_OBJECT child_sheep_ear_26_mount = {
#include "chao/parts_art/child_sheep_ear_26_mount.inc"
};
static Sint16 child_sheep_horn_30_polygons[151] = {
#include "chao/parts_art/child_sheep_horn_30_polygons.inc"
};
static Sint32 child_sheep_horn_30_vertices[117] = {
#include "chao/parts_art/child_sheep_horn_30_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_horn_30_model = {
#include "chao/parts_art/child_sheep_horn_30_model.inc"
};
static NJS_CNK_OBJECT child_sheep_horn_30 = {
#include "chao/parts_art/child_sheep_horn_30.inc"
};
static NJS_CNK_OBJECT child_sheep_horn_30_mount = {
#include "chao/parts_art/child_sheep_horn_30_mount.inc"
};
static Sint16 child_sheep_horn_31_polygons[151] = {
#include "chao/parts_art/child_sheep_horn_31_polygons.inc"
};
static Sint32 child_sheep_horn_31_vertices[117] = {
#include "chao/parts_art/child_sheep_horn_31_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_horn_31_model = {
#include "chao/parts_art/child_sheep_horn_31_model.inc"
};
static NJS_CNK_OBJECT child_sheep_horn_31 = {
#include "chao/parts_art/child_sheep_horn_31.inc"
};
static NJS_CNK_OBJECT child_sheep_horn_31_mount = {
#include "chao/parts_art/child_sheep_horn_31_mount.inc"
};
static Sint16 child_sheep_leg_6_polygons[163] = {
#include "chao/parts_art/child_sheep_leg_6_polygons.inc"
};
static Sint32 child_sheep_leg_6_vertices[117] = {
#include "chao/parts_art/child_sheep_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_leg_6_model = {
#include "chao/parts_art/child_sheep_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_sheep_leg_6 = {
#include "chao/parts_art/child_sheep_leg_6.inc"
};
static NJS_CNK_OBJECT child_sheep_leg_6_mount = {
#include "chao/parts_art/child_sheep_leg_6_mount.inc"
};
static Sint16 child_sheep_leg_13_polygons[163] = {
#include "chao/parts_art/child_sheep_leg_13_polygons.inc"
};
static Sint32 child_sheep_leg_13_vertices[117] = {
#include "chao/parts_art/child_sheep_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_leg_13_model = {
#include "chao/parts_art/child_sheep_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_sheep_leg_13 = {
#include "chao/parts_art/child_sheep_leg_13.inc"
};
static NJS_CNK_OBJECT child_sheep_leg_13_mount = {
#include "chao/parts_art/child_sheep_leg_13_mount.inc"
};
static Sint16 child_sheep_tail_polygons[107] = {
#include "chao/parts_art/child_sheep_tail_polygons.inc"
};
static Sint32 child_sheep_tail_vertices[75] = {
#include "chao/parts_art/child_sheep_tail_vertices.inc"
};
static NJS_CNK_MODEL child_sheep_tail_model = {
#include "chao/parts_art/child_sheep_tail_model.inc"
};
static NJS_CNK_OBJECT child_sheep_tail = {
#include "chao/parts_art/child_sheep_tail.inc"
};
static NJS_CNK_OBJECT child_sheep_tail_mount = {
#include "chao/parts_art/child_sheep_tail_mount.inc"
};
static NJS_CNK_OBJECT child_skunk_arm_3_child = {
#include "chao/parts_art/child_skunk_arm_3_child.inc"
};
static Sint16 child_skunk_arm_3_polygons[221] = {
#include "chao/parts_art/child_skunk_arm_3_polygons.inc"
};
static Sint32 child_skunk_arm_3_vertices[141] = {
#include "chao/parts_art/child_skunk_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_skunk_arm_3_model = {
#include "chao/parts_art/child_skunk_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_skunk_arm_3 = {
#include "chao/parts_art/child_skunk_arm_3.inc"
};
static NJS_CNK_OBJECT child_skunk_arm_3_mount = {
#include "chao/parts_art/child_skunk_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_skunk_arm_10_child = {
#include "chao/parts_art/child_skunk_arm_10_child.inc"
};
static Sint16 child_skunk_arm_10_polygons[221] = {
#include "chao/parts_art/child_skunk_arm_10_polygons.inc"
};
static Sint32 child_skunk_arm_10_vertices[141] = {
#include "chao/parts_art/child_skunk_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_skunk_arm_10_model = {
#include "chao/parts_art/child_skunk_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_skunk_arm_10 = {
#include "chao/parts_art/child_skunk_arm_10.inc"
};
static NJS_CNK_OBJECT child_skunk_arm_10_mount = {
#include "chao/parts_art/child_skunk_arm_10_mount.inc"
};
static Sint16 child_skunk_forehead_polygons[171] = {
#include "chao/parts_art/child_skunk_forehead_polygons.inc"
};
static Sint32 child_skunk_forehead_vertices[135] = {
#include "chao/parts_art/child_skunk_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_skunk_forehead_model = {
#include "chao/parts_art/child_skunk_forehead_model.inc"
};
static NJS_CNK_OBJECT child_skunk_forehead = {
#include "chao/parts_art/child_skunk_forehead.inc"
};
static NJS_CNK_OBJECT child_skunk_forehead_mount = {
#include "chao/parts_art/child_skunk_forehead_mount.inc"
};
static Sint16 child_skunk_leg_6_polygons[187] = {
#include "chao/parts_art/child_skunk_leg_6_polygons.inc"
};
static Sint32 child_skunk_leg_6_vertices[135] = {
#include "chao/parts_art/child_skunk_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_skunk_leg_6_model = {
#include "chao/parts_art/child_skunk_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_skunk_leg_6 = {
#include "chao/parts_art/child_skunk_leg_6.inc"
};
static NJS_CNK_OBJECT child_skunk_leg_6_mount = {
#include "chao/parts_art/child_skunk_leg_6_mount.inc"
};
static Sint16 child_skunk_leg_13_polygons[187] = {
#include "chao/parts_art/child_skunk_leg_13_polygons.inc"
};
static Sint32 child_skunk_leg_13_vertices[135] = {
#include "chao/parts_art/child_skunk_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_skunk_leg_13_model = {
#include "chao/parts_art/child_skunk_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_skunk_leg_13 = {
#include "chao/parts_art/child_skunk_leg_13.inc"
};
static NJS_CNK_OBJECT child_skunk_leg_13_mount = {
#include "chao/parts_art/child_skunk_leg_13_mount.inc"
};
static Sint16 child_skunk_tail_polygons[189] = {
#include "chao/parts_art/child_skunk_tail_polygons.inc"
};
static Sint32 child_skunk_tail_vertices[129] = {
#include "chao/parts_art/child_skunk_tail_vertices.inc"
};
static NJS_CNK_MODEL child_skunk_tail_model = {
#include "chao/parts_art/child_skunk_tail_model.inc"
};
static NJS_CNK_OBJECT child_skunk_tail = {
#include "chao/parts_art/child_skunk_tail.inc"
};
static NJS_CNK_OBJECT child_skunk_tail_mount = {
#include "chao/parts_art/child_skunk_tail_mount.inc"
};
static NJS_CNK_OBJECT child_tiger_arm_3_child = {
#include "chao/parts_art/child_tiger_arm_3_child.inc"
};
static Sint16 child_tiger_arm_3_polygons[251] = {
#include "chao/parts_art/child_tiger_arm_3_polygons.inc"
};
static Sint32 child_tiger_arm_3_vertices[147] = {
#include "chao/parts_art/child_tiger_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_arm_3_model = {
#include "chao/parts_art/child_tiger_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_tiger_arm_3 = {
#include "chao/parts_art/child_tiger_arm_3.inc"
};
static NJS_CNK_OBJECT child_tiger_arm_3_mount = {
#include "chao/parts_art/child_tiger_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_tiger_arm_10_child = {
#include "chao/parts_art/child_tiger_arm_10_child.inc"
};
static Sint16 child_tiger_arm_10_polygons[259] = {
#include "chao/parts_art/child_tiger_arm_10_polygons.inc"
};
static Sint32 child_tiger_arm_10_vertices[147] = {
#include "chao/parts_art/child_tiger_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_arm_10_model = {
#include "chao/parts_art/child_tiger_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_tiger_arm_10 = {
#include "chao/parts_art/child_tiger_arm_10.inc"
};
static NJS_CNK_OBJECT child_tiger_arm_10_mount = {
#include "chao/parts_art/child_tiger_arm_10_mount.inc"
};
static Sint16 child_tiger_ear_24_polygons[131] = {
#include "chao/parts_art/child_tiger_ear_24_polygons.inc"
};
static Sint32 child_tiger_ear_24_vertices[87] = {
#include "chao/parts_art/child_tiger_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_ear_24_model = {
#include "chao/parts_art/child_tiger_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_tiger_ear_24 = {
#include "chao/parts_art/child_tiger_ear_24.inc"
};
static NJS_CNK_OBJECT child_tiger_ear_24_mount = {
#include "chao/parts_art/child_tiger_ear_24_mount.inc"
};
static Sint16 child_tiger_ear_26_polygons[131] = {
#include "chao/parts_art/child_tiger_ear_26_polygons.inc"
};
static Sint32 child_tiger_ear_26_vertices[87] = {
#include "chao/parts_art/child_tiger_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_ear_26_model = {
#include "chao/parts_art/child_tiger_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_tiger_ear_26 = {
#include "chao/parts_art/child_tiger_ear_26.inc"
};
static NJS_CNK_OBJECT child_tiger_ear_26_mount = {
#include "chao/parts_art/child_tiger_ear_26_mount.inc"
};
static Sint16 child_tiger_leg_6_polygons[259] = {
#include "chao/parts_art/child_tiger_leg_6_polygons.inc"
};
static Sint32 child_tiger_leg_6_vertices[135] = {
#include "chao/parts_art/child_tiger_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_leg_6_model = {
#include "chao/parts_art/child_tiger_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_tiger_leg_6 = {
#include "chao/parts_art/child_tiger_leg_6.inc"
};
static NJS_CNK_OBJECT child_tiger_leg_6_mount = {
#include "chao/parts_art/child_tiger_leg_6_mount.inc"
};
static Sint16 child_tiger_leg_13_polygons[259] = {
#include "chao/parts_art/child_tiger_leg_13_polygons.inc"
};
static Sint32 child_tiger_leg_13_vertices[135] = {
#include "chao/parts_art/child_tiger_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_leg_13_model = {
#include "chao/parts_art/child_tiger_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_tiger_leg_13 = {
#include "chao/parts_art/child_tiger_leg_13.inc"
};
static NJS_CNK_OBJECT child_tiger_leg_13_mount = {
#include "chao/parts_art/child_tiger_leg_13_mount.inc"
};
static Sint16 child_tiger_tail_polygons[189] = {
#include "chao/parts_art/child_tiger_tail_polygons.inc"
};
static Sint32 child_tiger_tail_vertices[135] = {
#include "chao/parts_art/child_tiger_tail_vertices.inc"
};
static NJS_CNK_MODEL child_tiger_tail_model = {
#include "chao/parts_art/child_tiger_tail_model.inc"
};
static NJS_CNK_OBJECT child_tiger_tail = {
#include "chao/parts_art/child_tiger_tail.inc"
};
static NJS_CNK_OBJECT child_tiger_tail_mount = {
#include "chao/parts_art/child_tiger_tail_mount.inc"
};
static NJS_CNK_OBJECT child_unicorn_arm_3_child = {
#include "chao/parts_art/child_unicorn_arm_3_child.inc"
};
static Sint16 child_unicorn_arm_3_polygons[249] = {
#include "chao/parts_art/child_unicorn_arm_3_polygons.inc"
};
static Sint32 child_unicorn_arm_3_vertices[171] = {
#include "chao/parts_art/child_unicorn_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_arm_3_model = {
#include "chao/parts_art/child_unicorn_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_arm_3 = {
#include "chao/parts_art/child_unicorn_arm_3.inc"
};
static NJS_CNK_OBJECT child_unicorn_arm_3_mount = {
#include "chao/parts_art/child_unicorn_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_unicorn_arm_10_child = {
#include "chao/parts_art/child_unicorn_arm_10_child.inc"
};
static Sint16 child_unicorn_arm_10_polygons[261] = {
#include "chao/parts_art/child_unicorn_arm_10_polygons.inc"
};
static Sint32 child_unicorn_arm_10_vertices[171] = {
#include "chao/parts_art/child_unicorn_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_arm_10_model = {
#include "chao/parts_art/child_unicorn_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_arm_10 = {
#include "chao/parts_art/child_unicorn_arm_10.inc"
};
static NJS_CNK_OBJECT child_unicorn_arm_10_mount = {
#include "chao/parts_art/child_unicorn_arm_10_mount.inc"
};
static Sint16 child_unicorn_ear_24_polygons[155] = {
#include "chao/parts_art/child_unicorn_ear_24_polygons.inc"
};
static Sint32 child_unicorn_ear_24_vertices[105] = {
#include "chao/parts_art/child_unicorn_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_ear_24_model = {
#include "chao/parts_art/child_unicorn_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_ear_24 = {
#include "chao/parts_art/child_unicorn_ear_24.inc"
};
static NJS_CNK_OBJECT child_unicorn_ear_24_mount = {
#include "chao/parts_art/child_unicorn_ear_24_mount.inc"
};
static Sint16 child_unicorn_ear_26_polygons[155] = {
#include "chao/parts_art/child_unicorn_ear_26_polygons.inc"
};
static Sint32 child_unicorn_ear_26_vertices[105] = {
#include "chao/parts_art/child_unicorn_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_ear_26_model = {
#include "chao/parts_art/child_unicorn_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_ear_26 = {
#include "chao/parts_art/child_unicorn_ear_26.inc"
};
static NJS_CNK_OBJECT child_unicorn_ear_26_mount = {
#include "chao/parts_art/child_unicorn_ear_26_mount.inc"
};
static Sint16 child_unicorn_forehead_polygons[243] = {
#include "chao/parts_art/child_unicorn_forehead_polygons.inc"
};
static Sint32 child_unicorn_forehead_vertices[183] = {
#include "chao/parts_art/child_unicorn_forehead_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_forehead_model = {
#include "chao/parts_art/child_unicorn_forehead_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_forehead = {
#include "chao/parts_art/child_unicorn_forehead.inc"
};
static NJS_CNK_OBJECT child_unicorn_forehead_mount = {
#include "chao/parts_art/child_unicorn_forehead_mount.inc"
};
static Sint16 child_unicorn_leg_6_polygons[177] = {
#include "chao/parts_art/child_unicorn_leg_6_polygons.inc"
};
static Sint32 child_unicorn_leg_6_vertices[111] = {
#include "chao/parts_art/child_unicorn_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_leg_6_model = {
#include "chao/parts_art/child_unicorn_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_leg_6 = {
#include "chao/parts_art/child_unicorn_leg_6.inc"
};
static NJS_CNK_OBJECT child_unicorn_leg_6_mount = {
#include "chao/parts_art/child_unicorn_leg_6_mount.inc"
};
static Sint16 child_unicorn_leg_13_polygons[183] = {
#include "chao/parts_art/child_unicorn_leg_13_polygons.inc"
};
static Sint32 child_unicorn_leg_13_vertices[111] = {
#include "chao/parts_art/child_unicorn_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_leg_13_model = {
#include "chao/parts_art/child_unicorn_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_leg_13 = {
#include "chao/parts_art/child_unicorn_leg_13.inc"
};
static NJS_CNK_OBJECT child_unicorn_leg_13_mount = {
#include "chao/parts_art/child_unicorn_leg_13_mount.inc"
};
static Sint16 child_unicorn_tail_polygons[185] = {
#include "chao/parts_art/child_unicorn_tail_polygons.inc"
};
static Sint32 child_unicorn_tail_vertices[111] = {
#include "chao/parts_art/child_unicorn_tail_vertices.inc"
};
static NJS_CNK_MODEL child_unicorn_tail_model = {
#include "chao/parts_art/child_unicorn_tail_model.inc"
};
static NJS_CNK_OBJECT child_unicorn_tail = {
#include "chao/parts_art/child_unicorn_tail.inc"
};
static NJS_CNK_OBJECT child_unicorn_tail_mount = {
#include "chao/parts_art/child_unicorn_tail_mount.inc"
};
static NJS_CNK_OBJECT child_rabbit_arm_3_child = {
#include "chao/parts_art/child_rabbit_arm_3_child.inc"
};
static Sint16 child_rabbit_arm_3_polygons[195] = {
#include "chao/parts_art/child_rabbit_arm_3_polygons.inc"
};
static Sint32 child_rabbit_arm_3_vertices[141] = {
#include "chao/parts_art/child_rabbit_arm_3_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_arm_3_model = {
#include "chao/parts_art/child_rabbit_arm_3_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_arm_3 = {
#include "chao/parts_art/child_rabbit_arm_3.inc"
};
static NJS_CNK_OBJECT child_rabbit_arm_3_mount = {
#include "chao/parts_art/child_rabbit_arm_3_mount.inc"
};
static NJS_CNK_OBJECT child_rabbit_arm_10_child = {
#include "chao/parts_art/child_rabbit_arm_10_child.inc"
};
static Sint16 child_rabbit_arm_10_polygons[195] = {
#include "chao/parts_art/child_rabbit_arm_10_polygons.inc"
};
static Sint32 child_rabbit_arm_10_vertices[141] = {
#include "chao/parts_art/child_rabbit_arm_10_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_arm_10_model = {
#include "chao/parts_art/child_rabbit_arm_10_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_arm_10 = {
#include "chao/parts_art/child_rabbit_arm_10.inc"
};
static NJS_CNK_OBJECT child_rabbit_arm_10_mount = {
#include "chao/parts_art/child_rabbit_arm_10_mount.inc"
};
static Sint16 child_rabbit_ear_24_polygons[139] = {
#include "chao/parts_art/child_rabbit_ear_24_polygons.inc"
};
static Sint32 child_rabbit_ear_24_vertices[93] = {
#include "chao/parts_art/child_rabbit_ear_24_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_ear_24_model = {
#include "chao/parts_art/child_rabbit_ear_24_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_ear_24 = {
#include "chao/parts_art/child_rabbit_ear_24.inc"
};
static NJS_CNK_OBJECT child_rabbit_ear_24_mount = {
#include "chao/parts_art/child_rabbit_ear_24_mount.inc"
};
static Sint16 child_rabbit_ear_26_polygons[147] = {
#include "chao/parts_art/child_rabbit_ear_26_polygons.inc"
};
static Sint32 child_rabbit_ear_26_vertices[93] = {
#include "chao/parts_art/child_rabbit_ear_26_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_ear_26_model = {
#include "chao/parts_art/child_rabbit_ear_26_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_ear_26 = {
#include "chao/parts_art/child_rabbit_ear_26.inc"
};
static NJS_CNK_OBJECT child_rabbit_ear_26_mount = {
#include "chao/parts_art/child_rabbit_ear_26_mount.inc"
};
static Sint16 child_rabbit_leg_6_polygons[157] = {
#include "chao/parts_art/child_rabbit_leg_6_polygons.inc"
};
static Sint32 child_rabbit_leg_6_vertices[117] = {
#include "chao/parts_art/child_rabbit_leg_6_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_leg_6_model = {
#include "chao/parts_art/child_rabbit_leg_6_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_leg_6 = {
#include "chao/parts_art/child_rabbit_leg_6.inc"
};
static NJS_CNK_OBJECT child_rabbit_leg_6_mount = {
#include "chao/parts_art/child_rabbit_leg_6_mount.inc"
};
static Sint16 child_rabbit_leg_13_polygons[157] = {
#include "chao/parts_art/child_rabbit_leg_13_polygons.inc"
};
static Sint32 child_rabbit_leg_13_vertices[117] = {
#include "chao/parts_art/child_rabbit_leg_13_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_leg_13_model = {
#include "chao/parts_art/child_rabbit_leg_13_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_leg_13 = {
#include "chao/parts_art/child_rabbit_leg_13.inc"
};
static NJS_CNK_OBJECT child_rabbit_leg_13_mount = {
#include "chao/parts_art/child_rabbit_leg_13_mount.inc"
};
static Sint16 child_rabbit_tail_polygons[151] = {
#include "chao/parts_art/child_rabbit_tail_polygons.inc"
};
static Sint32 child_rabbit_tail_vertices[105] = {
#include "chao/parts_art/child_rabbit_tail_vertices.inc"
};
static NJS_CNK_MODEL child_rabbit_tail_model = {
#include "chao/parts_art/child_rabbit_tail_model.inc"
};
static NJS_CNK_OBJECT child_rabbit_tail = {
#include "chao/parts_art/child_rabbit_tail.inc"
};
static NJS_CNK_OBJECT child_rabbit_tail_mount = {
#include "chao/parts_art/child_rabbit_tail_mount.inc"
};
static Sint16 medal_12_polygons[31] = {
#include "chao/parts_art/medal_12_polygons.inc"
};
static Sint32 medal_12_vertices[75] = {
#include "chao/parts_art/medal_12_vertices.inc"
};
static NJS_CNK_MODEL medal_12_model = {
#include "chao/parts_art/medal_12_model.inc"
};
static NJS_CNK_OBJECT medal_12 = {
#include "chao/parts_art/medal_12.inc"
};
static Sint16 medal_1_polygons[49] = {
#include "chao/parts_art/medal_1_polygons.inc"
};
static Sint32 medal_1_vertices[147] = {
#include "chao/parts_art/medal_1_vertices.inc"
};
static NJS_CNK_MODEL medal_1_model = {
#include "chao/parts_art/medal_1_model.inc"
};
static NJS_CNK_OBJECT medal_1 = {
#include "chao/parts_art/medal_1.inc"
};
static Sint16 medal_6_polygons[101] = {
#include "chao/parts_art/medal_6_polygons.inc"
};
static Sint32 medal_6_vertices[147] = {
#include "chao/parts_art/medal_6_vertices.inc"
};
static NJS_CNK_MODEL medal_6_model = {
#include "chao/parts_art/medal_6_model.inc"
};
static NJS_CNK_OBJECT medal_6 = {
#include "chao/parts_art/medal_6.inc"
};
static Sint16 medal_13_polygons[49] = {
#include "chao/parts_art/medal_13_polygons.inc"
};
static Sint32 medal_13_vertices[171] = {
#include "chao/parts_art/medal_13_vertices.inc"
};
static NJS_CNK_MODEL medal_13_model = {
#include "chao/parts_art/medal_13_model.inc"
};
static NJS_CNK_OBJECT medal_13 = {
#include "chao/parts_art/medal_13.inc"
};
static Sint16 medal_4_polygons[39] = {
#include "chao/parts_art/medal_4_polygons.inc"
};
static Sint32 medal_4_vertices[123] = {
#include "chao/parts_art/medal_4_vertices.inc"
};
static NJS_CNK_MODEL medal_4_model = {
#include "chao/parts_art/medal_4_model.inc"
};
static NJS_CNK_OBJECT medal_4 = {
#include "chao/parts_art/medal_4.inc"
};
static Sint16 medal_5_polygons[209] = {
#include "chao/parts_art/medal_5_polygons.inc"
};
static Sint32 medal_5_vertices[243] = {
#include "chao/parts_art/medal_5_vertices.inc"
};
static NJS_CNK_MODEL medal_5_model = {
#include "chao/parts_art/medal_5_model.inc"
};
static NJS_CNK_OBJECT medal_5 = {
#include "chao/parts_art/medal_5.inc"
};
static Sint16 medal_11_polygons[53] = {
#include "chao/parts_art/medal_11_polygons.inc"
};
static Sint32 medal_11_vertices[123] = {
#include "chao/parts_art/medal_11_vertices.inc"
};
static NJS_CNK_MODEL medal_11_model = {
#include "chao/parts_art/medal_11_model.inc"
};
static NJS_CNK_OBJECT medal_11 = {
#include "chao/parts_art/medal_11.inc"
};
static Sint16 medal_3_polygons[51] = {
#include "chao/parts_art/medal_3_polygons.inc"
};
static Sint32 medal_3_vertices[183] = {
#include "chao/parts_art/medal_3_vertices.inc"
};
static NJS_CNK_MODEL medal_3_model = {
#include "chao/parts_art/medal_3_model.inc"
};
static NJS_CNK_OBJECT medal_3 = {
#include "chao/parts_art/medal_3.inc"
};
static Sint16 medal_14_polygons[39] = {
#include "chao/parts_art/medal_14_polygons.inc"
};
static Sint32 medal_14_vertices[123] = {
#include "chao/parts_art/medal_14_vertices.inc"
};
static NJS_CNK_MODEL medal_14_model = {
#include "chao/parts_art/medal_14_model.inc"
};
static NJS_CNK_OBJECT medal_14 = {
#include "chao/parts_art/medal_14.inc"
};
static Sint16 medal_15_polygons[39] = {
#include "chao/parts_art/medal_15_polygons.inc"
};
static Sint32 medal_15_vertices[123] = {
#include "chao/parts_art/medal_15_vertices.inc"
};
static NJS_CNK_MODEL medal_15_model = {
#include "chao/parts_art/medal_15_model.inc"
};
static NJS_CNK_OBJECT medal_15 = {
#include "chao/parts_art/medal_15.inc"
};
static Sint16 medal_2_polygons[51] = {
#include "chao/parts_art/medal_2_polygons.inc"
};
static Sint32 medal_2_vertices[183] = {
#include "chao/parts_art/medal_2_vertices.inc"
};
static NJS_CNK_MODEL medal_2_model = {
#include "chao/parts_art/medal_2_model.inc"
};
static NJS_CNK_OBJECT medal_2 = {
#include "chao/parts_art/medal_2.inc"
};
static Sint16 medal_9_polygons[157] = {
#include "chao/parts_art/medal_9_polygons.inc"
};
static Sint32 medal_9_vertices[201] = {
#include "chao/parts_art/medal_9_vertices.inc"
};
static NJS_CNK_MODEL medal_9_model = {
#include "chao/parts_art/medal_9_model.inc"
};
static NJS_CNK_OBJECT medal_9 = {
#include "chao/parts_art/medal_9.inc"
};
static Sint16 medal_10_polygons[121] = {
#include "chao/parts_art/medal_10_polygons.inc"
};
static Sint32 medal_10_vertices[201] = {
#include "chao/parts_art/medal_10_vertices.inc"
};
static NJS_CNK_MODEL medal_10_model = {
#include "chao/parts_art/medal_10_model.inc"
};
static NJS_CNK_OBJECT medal_10 = {
#include "chao/parts_art/medal_10.inc"
};
static Sint16 medal_8_polygons[127] = {
#include "chao/parts_art/medal_8_polygons.inc"
};
static Sint32 medal_8_vertices[213] = {
#include "chao/parts_art/medal_8_vertices.inc"
};
static NJS_CNK_MODEL medal_8_model = {
#include "chao/parts_art/medal_8_model.inc"
};
static NJS_CNK_OBJECT medal_8 = {
#include "chao/parts_art/medal_8.inc"
};
static Sint16 medal_7_polygons[127] = {
#include "chao/parts_art/medal_7_polygons.inc"
};
static Sint32 medal_7_vertices[213] = {
#include "chao/parts_art/medal_7_vertices.inc"
};
static NJS_CNK_MODEL medal_7_model = {
#include "chao/parts_art/medal_7_model.inc"
};
static NJS_CNK_OBJECT medal_7 = {
#include "chao/parts_art/medal_7.inc"
};
static Sint16 medal_17_child_sibling_polygons[57] = {
#include "chao/parts_art/medal_17_child_sibling_polygons.inc"
};
static Sint32 medal_17_child_sibling_vertices[135] = {
#include "chao/parts_art/medal_17_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL medal_17_child_sibling_model = {
#include "chao/parts_art/medal_17_child_sibling_model.inc"
};
static NJS_CNK_OBJECT medal_17_child_sibling = {
#include "chao/parts_art/medal_17_child_sibling.inc"
};
static Sint16 medal_17_child_polygons[67] = {
#include "chao/parts_art/medal_17_child_polygons.inc"
};
static Sint32 medal_17_child_vertices[171] = {
#include "chao/parts_art/medal_17_child_vertices.inc"
};
static NJS_CNK_MODEL medal_17_child_model = {
#include "chao/parts_art/medal_17_child_model.inc"
};
static NJS_CNK_OBJECT medal_17_child = {
#include "chao/parts_art/medal_17_child.inc"
};
static Sint16 medal_17_polygons[77] = {
#include "chao/parts_art/medal_17_polygons.inc"
};
static Sint32 medal_17_vertices[171] = {
#include "chao/parts_art/medal_17_vertices.inc"
};
static NJS_CNK_MODEL medal_17_model = {
#include "chao/parts_art/medal_17_model.inc"
};
static NJS_CNK_OBJECT medal_17 = {
#include "chao/parts_art/medal_17.inc"
};
static Sint16 medal_22_child_sibling_polygons[79] = {
#include "chao/parts_art/medal_22_child_sibling_polygons.inc"
};
static Sint32 medal_22_child_sibling_vertices[147] = {
#include "chao/parts_art/medal_22_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL medal_22_child_sibling_model = {
#include "chao/parts_art/medal_22_child_sibling_model.inc"
};
static NJS_CNK_OBJECT medal_22_child_sibling = {
#include "chao/parts_art/medal_22_child_sibling.inc"
};
static Sint16 medal_22_child_polygons[51] = {
#include "chao/parts_art/medal_22_child_polygons.inc"
};
static Sint32 medal_22_child_vertices[105] = {
#include "chao/parts_art/medal_22_child_vertices.inc"
};
static NJS_CNK_MODEL medal_22_child_model = {
#include "chao/parts_art/medal_22_child_model.inc"
};
static NJS_CNK_OBJECT medal_22_child = {
#include "chao/parts_art/medal_22_child.inc"
};
static Sint16 medal_22_polygons[113] = {
#include "chao/parts_art/medal_22_polygons.inc"
};
static Sint32 medal_22_vertices[225] = {
#include "chao/parts_art/medal_22_vertices.inc"
};
static NJS_CNK_MODEL medal_22_model = {
#include "chao/parts_art/medal_22_model.inc"
};
static NJS_CNK_OBJECT medal_22 = {
#include "chao/parts_art/medal_22.inc"
};
static Sint16 medal_20_child_polygons[115] = {
#include "chao/parts_art/medal_20_child_polygons.inc"
};
static Sint32 medal_20_child_vertices[243] = {
#include "chao/parts_art/medal_20_child_vertices.inc"
};
static NJS_CNK_MODEL medal_20_child_model = {
#include "chao/parts_art/medal_20_child_model.inc"
};
static NJS_CNK_OBJECT medal_20_child = {
#include "chao/parts_art/medal_20_child.inc"
};
static Sint16 medal_20_polygons[135] = {
#include "chao/parts_art/medal_20_polygons.inc"
};
static Sint32 medal_20_vertices[303] = {
#include "chao/parts_art/medal_20_vertices.inc"
};
static NJS_CNK_MODEL medal_20_model = {
#include "chao/parts_art/medal_20_model.inc"
};
static NJS_CNK_OBJECT medal_20 = {
#include "chao/parts_art/medal_20.inc"
};
static Sint16 medal_21_polygons[429] = {
#include "chao/parts_art/medal_21_polygons.inc"
};
static Sint32 medal_21_vertices[495] = {
#include "chao/parts_art/medal_21_vertices.inc"
};
static NJS_CNK_MODEL medal_21_model = {
#include "chao/parts_art/medal_21_model.inc"
};
static NJS_CNK_OBJECT medal_21 = {
#include "chao/parts_art/medal_21.inc"
};
static Sint16 medal_19_child_sibling_polygons[55] = {
#include "chao/parts_art/medal_19_child_sibling_polygons.inc"
};
static Sint32 medal_19_child_sibling_vertices[159] = {
#include "chao/parts_art/medal_19_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL medal_19_child_sibling_model = {
#include "chao/parts_art/medal_19_child_sibling_model.inc"
};
static NJS_CNK_OBJECT medal_19_child_sibling = {
#include "chao/parts_art/medal_19_child_sibling.inc"
};
static Sint16 medal_19_child_polygons[93] = {
#include "chao/parts_art/medal_19_child_polygons.inc"
};
static Sint32 medal_19_child_vertices[231] = {
#include "chao/parts_art/medal_19_child_vertices.inc"
};
static NJS_CNK_MODEL medal_19_child_model = {
#include "chao/parts_art/medal_19_child_model.inc"
};
static NJS_CNK_OBJECT medal_19_child = {
#include "chao/parts_art/medal_19_child.inc"
};
static Sint16 medal_19_polygons[93] = {
#include "chao/parts_art/medal_19_polygons.inc"
};
static Sint32 medal_19_vertices[231] = {
#include "chao/parts_art/medal_19_vertices.inc"
};
static NJS_CNK_MODEL medal_19_model = {
#include "chao/parts_art/medal_19_model.inc"
};
static NJS_CNK_OBJECT medal_19 = {
#include "chao/parts_art/medal_19.inc"
};
static Sint16 medal_18_child_sibling_polygons[67] = {
#include "chao/parts_art/medal_18_child_sibling_polygons.inc"
};
static Sint32 medal_18_child_sibling_vertices[219] = {
#include "chao/parts_art/medal_18_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL medal_18_child_sibling_model = {
#include "chao/parts_art/medal_18_child_sibling_model.inc"
};
static NJS_CNK_OBJECT medal_18_child_sibling = {
#include "chao/parts_art/medal_18_child_sibling.inc"
};
static Sint16 medal_18_child_polygons[67] = {
#include "chao/parts_art/medal_18_child_polygons.inc"
};
static Sint32 medal_18_child_vertices[219] = {
#include "chao/parts_art/medal_18_child_vertices.inc"
};
static NJS_CNK_MODEL medal_18_child_model = {
#include "chao/parts_art/medal_18_child_model.inc"
};
static NJS_CNK_OBJECT medal_18_child = {
#include "chao/parts_art/medal_18_child.inc"
};
static Sint16 medal_18_polygons[75] = {
#include "chao/parts_art/medal_18_polygons.inc"
};
static Sint32 medal_18_vertices[219] = {
#include "chao/parts_art/medal_18_vertices.inc"
};
static NJS_CNK_MODEL medal_18_model = {
#include "chao/parts_art/medal_18_model.inc"
};
static NJS_CNK_OBJECT medal_18 = {
#include "chao/parts_art/medal_18.inc"
};
static Sint16 medal_25_polygons[309] = {
#include "chao/parts_art/medal_25_polygons.inc"
};
static Sint32 medal_25_vertices[375] = {
#include "chao/parts_art/medal_25_vertices.inc"
};
static NJS_CNK_MODEL medal_25_model = {
#include "chao/parts_art/medal_25_model.inc"
};
static NJS_CNK_OBJECT medal_25 = {
#include "chao/parts_art/medal_25.inc"
};
static Sint16 medal_26_polygons[277] = {
#include "chao/parts_art/medal_26_polygons.inc"
};
static Sint32 medal_26_vertices[351] = {
#include "chao/parts_art/medal_26_vertices.inc"
};
static NJS_CNK_MODEL medal_26_model = {
#include "chao/parts_art/medal_26_model.inc"
};
static NJS_CNK_OBJECT medal_26 = {
#include "chao/parts_art/medal_26.inc"
};
static Sint16 medal_24_polygons[207] = {
#include "chao/parts_art/medal_24_polygons.inc"
};
static Sint32 medal_24_vertices[369] = {
#include "chao/parts_art/medal_24_vertices.inc"
};
static NJS_CNK_MODEL medal_24_model = {
#include "chao/parts_art/medal_24_model.inc"
};
static NJS_CNK_OBJECT medal_24 = {
#include "chao/parts_art/medal_24.inc"
};
static Sint16 medal_23_polygons[195] = {
#include "chao/parts_art/medal_23_polygons.inc"
};
static Sint32 medal_23_vertices[315] = {
#include "chao/parts_art/medal_23_vertices.inc"
};
static NJS_CNK_MODEL medal_23_model = {
#include "chao/parts_art/medal_23_model.inc"
};
static NJS_CNK_OBJECT medal_23 = {
#include "chao/parts_art/medal_23.inc"
};
static Sint16 unused_medal_1_polygons[91] = {
#include "chao/parts_art/unused_medal_1_polygons.inc"
};
static Sint32 unused_medal_1_vertices[171] = {
#include "chao/parts_art/unused_medal_1_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_1_model = {
#include "chao/parts_art/unused_medal_1_model.inc"
};
static NJS_CNK_OBJECT unused_medal_1 = {
#include "chao/parts_art/unused_medal_1.inc"
};
static Sint16 unused_medal_2_polygons[49] = {
#include "chao/parts_art/unused_medal_2_polygons.inc"
};
static Sint32 unused_medal_2_vertices[117] = {
#include "chao/parts_art/unused_medal_2_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_2_model = {
#include "chao/parts_art/unused_medal_2_model.inc"
};
static NJS_CNK_OBJECT unused_medal_2 = {
#include "chao/parts_art/unused_medal_2.inc"
};
static Sint16 unused_medal_3_child_sibling_polygons[83] = {
#include "chao/parts_art/unused_medal_3_child_sibling_polygons.inc"
};
static Sint32 unused_medal_3_child_sibling_vertices[183] = {
#include "chao/parts_art/unused_medal_3_child_sibling_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_3_child_sibling_model = {
#include "chao/parts_art/unused_medal_3_child_sibling_model.inc"
};
static NJS_CNK_OBJECT unused_medal_3_child_sibling = {
#include "chao/parts_art/unused_medal_3_child_sibling.inc"
};
static Sint16 unused_medal_3_child_polygons[45] = {
#include "chao/parts_art/unused_medal_3_child_polygons.inc"
};
static Sint32 unused_medal_3_child_vertices[123] = {
#include "chao/parts_art/unused_medal_3_child_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_3_child_model = {
#include "chao/parts_art/unused_medal_3_child_model.inc"
};
static NJS_CNK_OBJECT unused_medal_3_child = {
#include "chao/parts_art/unused_medal_3_child.inc"
};
static Sint16 unused_medal_3_polygons[83] = {
#include "chao/parts_art/unused_medal_3_polygons.inc"
};
static Sint32 unused_medal_3_vertices[183] = {
#include "chao/parts_art/unused_medal_3_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_3_model = {
#include "chao/parts_art/unused_medal_3_model.inc"
};
static NJS_CNK_OBJECT unused_medal_3 = {
#include "chao/parts_art/unused_medal_3.inc"
};
static Sint16 unused_medal_4_child_polygons[115] = {
#include "chao/parts_art/unused_medal_4_child_polygons.inc"
};
static Sint32 unused_medal_4_child_vertices[243] = {
#include "chao/parts_art/unused_medal_4_child_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_4_child_model = {
#include "chao/parts_art/unused_medal_4_child_model.inc"
};
static NJS_CNK_OBJECT unused_medal_4_child = {
#include "chao/parts_art/unused_medal_4_child.inc"
};
static Sint16 unused_medal_4_polygons[117] = {
#include "chao/parts_art/unused_medal_4_polygons.inc"
};
static Sint32 unused_medal_4_vertices[243] = {
#include "chao/parts_art/unused_medal_4_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_4_model = {
#include "chao/parts_art/unused_medal_4_model.inc"
};
static NJS_CNK_OBJECT unused_medal_4 = {
#include "chao/parts_art/unused_medal_4.inc"
};
static Sint16 unused_medal_5_child_polygons[115] = {
#include "chao/parts_art/unused_medal_5_child_polygons.inc"
};
static Sint32 unused_medal_5_child_vertices[243] = {
#include "chao/parts_art/unused_medal_5_child_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_5_child_model = {
#include "chao/parts_art/unused_medal_5_child_model.inc"
};
static NJS_CNK_OBJECT unused_medal_5_child = {
#include "chao/parts_art/unused_medal_5_child.inc"
};
static Sint16 unused_medal_5_polygons[117] = {
#include "chao/parts_art/unused_medal_5_polygons.inc"
};
static Sint32 unused_medal_5_vertices[243] = {
#include "chao/parts_art/unused_medal_5_vertices.inc"
};
static NJS_CNK_MODEL unused_medal_5_model = {
#include "chao/parts_art/unused_medal_5_model.inc"
};
static NJS_CNK_OBJECT unused_medal_5 = {
#include "chao/parts_art/unused_medal_5.inc"
};
NJS_CNK_OBJECT *chao_medal_models[32] = {
#include "chao/parts_art/chao_medal_models.inc"
};
static NJS_CNK_OBJECT *adult_raccoon_parts[13] = {
#include "chao/parts_art/adult_raccoon_parts.inc"
};
static NJS_CNK_OBJECT *adult_cheetah_parts[13] = {
#include "chao/parts_art/adult_cheetah_parts.inc"
};
static NJS_CNK_OBJECT *adult_dragon_parts[13] = {
#include "chao/parts_art/adult_dragon_parts.inc"
};
static NJS_CNK_OBJECT *adult_seal_parts[13] = {
#include "chao/parts_art/adult_seal_parts.inc"
};
static NJS_CNK_OBJECT *adult_gorilla_parts[13] = {
#include "chao/parts_art/adult_gorilla_parts.inc"
};
static NJS_CNK_OBJECT *adult_boar_parts[13] = {
#include "chao/parts_art/adult_boar_parts.inc"
};
static NJS_CNK_OBJECT *adult_condor_parts[13] = {
#include "chao/parts_art/adult_condor_parts.inc"
};
static NJS_CNK_OBJECT *adult_peacock_parts[13] = {
#include "chao/parts_art/adult_peacock_parts.inc"
};
static NJS_CNK_OBJECT *adult_bear_parts[13] = {
#include "chao/parts_art/adult_bear_parts.inc"
};
static NJS_CNK_OBJECT *adult_parrot_parts[13] = {
#include "chao/parts_art/adult_parrot_parts.inc"
};
static NJS_CNK_OBJECT *adult_penguin_parts[13] = {
#include "chao/parts_art/adult_penguin_parts.inc"
};
static NJS_CNK_OBJECT *adult_phoenix_parts[13] = {
#include "chao/parts_art/adult_phoenix_parts.inc"
};
static NJS_CNK_OBJECT *adult_otter_parts[13] = {
#include "chao/parts_art/adult_otter_parts.inc"
};
static NJS_CNK_OBJECT *adult_sheep_parts[13] = {
#include "chao/parts_art/adult_sheep_parts.inc"
};
static NJS_CNK_OBJECT *adult_skunk_parts[13] = {
#include "chao/parts_art/adult_skunk_parts.inc"
};
static NJS_CNK_OBJECT *adult_tiger_parts[13] = {
#include "chao/parts_art/adult_tiger_parts.inc"
};
static NJS_CNK_OBJECT *adult_unicorn_parts[13] = {
#include "chao/parts_art/adult_unicorn_parts.inc"
};
static NJS_CNK_OBJECT *adult_rabbit_parts[13] = {
#include "chao/parts_art/adult_rabbit_parts.inc"
};
static NJS_CNK_OBJECT *child_raccoon_parts[13] = {
#include "chao/parts_art/child_raccoon_parts.inc"
};
static NJS_CNK_OBJECT *child_cheetah_parts[13] = {
#include "chao/parts_art/child_cheetah_parts.inc"
};
static NJS_CNK_OBJECT *child_dragon_parts[13] = {
#include "chao/parts_art/child_dragon_parts.inc"
};
static NJS_CNK_OBJECT *child_seal_parts[13] = {
#include "chao/parts_art/child_seal_parts.inc"
};
static NJS_CNK_OBJECT *child_gorilla_parts[13] = {
#include "chao/parts_art/child_gorilla_parts.inc"
};
static NJS_CNK_OBJECT *child_boar_parts[13] = {
#include "chao/parts_art/child_boar_parts.inc"
};
static NJS_CNK_OBJECT *child_condor_parts[13] = {
#include "chao/parts_art/child_condor_parts.inc"
};
static NJS_CNK_OBJECT *child_peacock_parts[13] = {
#include "chao/parts_art/child_peacock_parts.inc"
};
static NJS_CNK_OBJECT *child_bear_parts[13] = {
#include "chao/parts_art/child_bear_parts.inc"
};
static NJS_CNK_OBJECT *child_parrot_parts[13] = {
#include "chao/parts_art/child_parrot_parts.inc"
};
static NJS_CNK_OBJECT *child_penguin_parts[13] = {
#include "chao/parts_art/child_penguin_parts.inc"
};
static NJS_CNK_OBJECT *child_phoenix_parts[13] = {
#include "chao/parts_art/child_phoenix_parts.inc"
};
static NJS_CNK_OBJECT *child_otter_parts[13] = {
#include "chao/parts_art/child_otter_parts.inc"
};
static NJS_CNK_OBJECT *child_sheep_parts[13] = {
#include "chao/parts_art/child_sheep_parts.inc"
};
static NJS_CNK_OBJECT *child_skunk_parts[13] = {
#include "chao/parts_art/child_skunk_parts.inc"
};
static NJS_CNK_OBJECT *child_tiger_parts[13] = {
#include "chao/parts_art/child_tiger_parts.inc"
};
static NJS_CNK_OBJECT *child_unicorn_parts[13] = {
#include "chao/parts_art/child_unicorn_parts.inc"
};
static NJS_CNK_OBJECT *child_rabbit_parts[13] = {
#include "chao/parts_art/child_rabbit_parts.inc"
};
static NJS_CNK_OBJECT *empty_parts[13] = {
#include "chao/parts_art/empty_parts.inc"
};
NJS_CNK_OBJECT **chao_adult_part_models[ANIMAL_KINDS] = {
#include "chao/parts_art/chao_adult_part_models.inc"
};
NJS_CNK_OBJECT **chao_child_part_models[ANIMAL_KINDS] = {
#include "chao/parts_art/chao_child_part_models.inc"
};
