/* the room volumes, each placed by its object's pos, ang and scl */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/glow.h"

static NJS_MATERIAL room_volume_04_material = {
#include "stg22/fall_volumes/room_volume_04_material.inc"
};
static Sint16 room_volume_04_strips[5] = {
#include "stg22/fall_volumes/room_volume_04_strips.inc"
};
static NJS_MESHSET room_volume_04_meshset = {
#include "stg22/fall_volumes/room_volume_04_meshset.inc"
};
static NJS_POINT3 room_volume_04_points[4] = {
#include "stg22/fall_volumes/room_volume_04_points.inc"
};
static NJS_VECTOR room_volume_04_normals[4] = {
#include "stg22/fall_volumes/room_volume_04_normals.inc"
};
static NJS_MODEL room_volume_04_model = {
#include "stg22/fall_volumes/room_volume_04_model.inc"
};
static NJS_OBJECT room_volume_04 = {
#include "stg22/fall_volumes/room_volume_04.inc"
};

static NJS_MATERIAL room_volume_05_material = {
#include "stg22/fall_volumes/room_volume_05_material.inc"
};
static Sint16 room_volume_05_strips[5] = {
#include "stg22/fall_volumes/room_volume_05_strips.inc"
};
static NJS_MESHSET room_volume_05_meshset = {
#include "stg22/fall_volumes/room_volume_05_meshset.inc"
};
static NJS_POINT3 room_volume_05_points[4] = {
#include "stg22/fall_volumes/room_volume_05_points.inc"
};
static NJS_VECTOR room_volume_05_normals[4] = {
#include "stg22/fall_volumes/room_volume_05_normals.inc"
};
static NJS_MODEL room_volume_05_model = {
#include "stg22/fall_volumes/room_volume_05_model.inc"
};
static NJS_OBJECT room_volume_05 = {
#include "stg22/fall_volumes/room_volume_05.inc"
};

static NJS_MATERIAL room_volume_06_material = {
#include "stg22/fall_volumes/room_volume_06_material.inc"
};
static Sint16 room_volume_06_strips[5] = {
#include "stg22/fall_volumes/room_volume_06_strips.inc"
};
static NJS_MESHSET room_volume_06_meshset = {
#include "stg22/fall_volumes/room_volume_06_meshset.inc"
};
static NJS_POINT3 room_volume_06_points[4] = {
#include "stg22/fall_volumes/room_volume_06_points.inc"
};
static NJS_VECTOR room_volume_06_normals[4] = {
#include "stg22/fall_volumes/room_volume_06_normals.inc"
};
static NJS_MODEL room_volume_06_model = {
#include "stg22/fall_volumes/room_volume_06_model.inc"
};
static NJS_OBJECT room_volume_06 = {
#include "stg22/fall_volumes/room_volume_06.inc"
};

static NJS_MATERIAL room_volume_07_material = {
#include "stg22/fall_volumes/room_volume_07_material.inc"
};
static Sint16 room_volume_07_strips[5] = {
#include "stg22/fall_volumes/room_volume_07_strips.inc"
};
static NJS_MESHSET room_volume_07_meshset = {
#include "stg22/fall_volumes/room_volume_07_meshset.inc"
};
static NJS_POINT3 room_volume_07_points[4] = {
#include "stg22/fall_volumes/room_volume_07_points.inc"
};
static NJS_VECTOR room_volume_07_normals[4] = {
#include "stg22/fall_volumes/room_volume_07_normals.inc"
};
static NJS_MODEL room_volume_07_model = {
#include "stg22/fall_volumes/room_volume_07_model.inc"
};
static NJS_OBJECT room_volume_07 = {
#include "stg22/fall_volumes/room_volume_07.inc"
};

static NJS_MATERIAL room_volume_23_material = {
#include "stg22/fall_volumes/room_volume_23_material.inc"
};
static Sint16 room_volume_23_strips[5] = {
#include "stg22/fall_volumes/room_volume_23_strips.inc"
};
static NJS_MESHSET room_volume_23_meshset = {
#include "stg22/fall_volumes/room_volume_23_meshset.inc"
};
static NJS_POINT3 room_volume_23_points[4] = {
#include "stg22/fall_volumes/room_volume_23_points.inc"
};
static NJS_VECTOR room_volume_23_normals[4] = {
#include "stg22/fall_volumes/room_volume_23_normals.inc"
};
static NJS_MODEL room_volume_23_model = {
#include "stg22/fall_volumes/room_volume_23_model.inc"
};
static NJS_OBJECT room_volume_23 = {
#include "stg22/fall_volumes/room_volume_23.inc"
};

static NJS_MATERIAL room_volume_24_material = {
#include "stg22/fall_volumes/room_volume_24_material.inc"
};
static Sint16 room_volume_24_strips[5] = {
#include "stg22/fall_volumes/room_volume_24_strips.inc"
};
static NJS_MESHSET room_volume_24_meshset = {
#include "stg22/fall_volumes/room_volume_24_meshset.inc"
};
static NJS_POINT3 room_volume_24_points[4] = {
#include "stg22/fall_volumes/room_volume_24_points.inc"
};
static NJS_VECTOR room_volume_24_normals[4] = {
#include "stg22/fall_volumes/room_volume_24_normals.inc"
};
static NJS_MODEL room_volume_24_model = {
#include "stg22/fall_volumes/room_volume_24_model.inc"
};
static NJS_OBJECT room_volume_24 = {
#include "stg22/fall_volumes/room_volume_24.inc"
};

static NJS_MATERIAL room_volume_25_material = {
#include "stg22/fall_volumes/room_volume_25_material.inc"
};
static Sint16 room_volume_25_strips[5] = {
#include "stg22/fall_volumes/room_volume_25_strips.inc"
};
static NJS_MESHSET room_volume_25_meshset = {
#include "stg22/fall_volumes/room_volume_25_meshset.inc"
};
static NJS_POINT3 room_volume_25_points[4] = {
#include "stg22/fall_volumes/room_volume_25_points.inc"
};
static NJS_VECTOR room_volume_25_normals[4] = {
#include "stg22/fall_volumes/room_volume_25_normals.inc"
};
static NJS_MODEL room_volume_25_model = {
#include "stg22/fall_volumes/room_volume_25_model.inc"
};
static NJS_OBJECT room_volume_25 = {
#include "stg22/fall_volumes/room_volume_25.inc"
};

static NJS_MATERIAL room_volume_26_material = {
#include "stg22/fall_volumes/room_volume_26_material.inc"
};
static Sint16 room_volume_26_strips[5] = {
#include "stg22/fall_volumes/room_volume_26_strips.inc"
};
static NJS_MESHSET room_volume_26_meshset = {
#include "stg22/fall_volumes/room_volume_26_meshset.inc"
};
static NJS_POINT3 room_volume_26_points[4] = {
#include "stg22/fall_volumes/room_volume_26_points.inc"
};
static NJS_VECTOR room_volume_26_normals[4] = {
#include "stg22/fall_volumes/room_volume_26_normals.inc"
};
static NJS_MODEL room_volume_26_model = {
#include "stg22/fall_volumes/room_volume_26_model.inc"
};
static NJS_OBJECT room_volume_26 = {
#include "stg22/fall_volumes/room_volume_26.inc"
};

static NJS_MATERIAL room_volume_27_material = {
#include "stg22/fall_volumes/room_volume_27_material.inc"
};
static Sint16 room_volume_27_strips[5] = {
#include "stg22/fall_volumes/room_volume_27_strips.inc"
};
static NJS_MESHSET room_volume_27_meshset = {
#include "stg22/fall_volumes/room_volume_27_meshset.inc"
};
static NJS_POINT3 room_volume_27_points[4] = {
#include "stg22/fall_volumes/room_volume_27_points.inc"
};
static NJS_VECTOR room_volume_27_normals[4] = {
#include "stg22/fall_volumes/room_volume_27_normals.inc"
};
static NJS_MODEL room_volume_27_model = {
#include "stg22/fall_volumes/room_volume_27_model.inc"
};
static NJS_OBJECT room_volume_27 = {
#include "stg22/fall_volumes/room_volume_27.inc"
};

static NJS_MATERIAL room_volume_28_material = {
#include "stg22/fall_volumes/room_volume_28_material.inc"
};
static Sint16 room_volume_28_strips[5] = {
#include "stg22/fall_volumes/room_volume_28_strips.inc"
};
static NJS_MESHSET room_volume_28_meshset = {
#include "stg22/fall_volumes/room_volume_28_meshset.inc"
};
static NJS_POINT3 room_volume_28_points[4] = {
#include "stg22/fall_volumes/room_volume_28_points.inc"
};
static NJS_VECTOR room_volume_28_normals[4] = {
#include "stg22/fall_volumes/room_volume_28_normals.inc"
};
static NJS_MODEL room_volume_28_model = {
#include "stg22/fall_volumes/room_volume_28_model.inc"
};
static NJS_OBJECT room_volume_28 = {
#include "stg22/fall_volumes/room_volume_28.inc"
};

static NJS_MATERIAL room_volume_29_material = {
#include "stg22/fall_volumes/room_volume_29_material.inc"
};
static Sint16 room_volume_29_strips[5] = {
#include "stg22/fall_volumes/room_volume_29_strips.inc"
};
static NJS_MESHSET room_volume_29_meshset = {
#include "stg22/fall_volumes/room_volume_29_meshset.inc"
};
static NJS_POINT3 room_volume_29_points[4] = {
#include "stg22/fall_volumes/room_volume_29_points.inc"
};
static NJS_VECTOR room_volume_29_normals[4] = {
#include "stg22/fall_volumes/room_volume_29_normals.inc"
};
static NJS_MODEL room_volume_29_model = {
#include "stg22/fall_volumes/room_volume_29_model.inc"
};
static NJS_OBJECT room_volume_29 = {
#include "stg22/fall_volumes/room_volume_29.inc"
};

static NJS_MATERIAL room_volume_30_material = {
#include "stg22/fall_volumes/room_volume_30_material.inc"
};
static Sint16 room_volume_30_strips[5] = {
#include "stg22/fall_volumes/room_volume_30_strips.inc"
};
static NJS_MESHSET room_volume_30_meshset = {
#include "stg22/fall_volumes/room_volume_30_meshset.inc"
};
static NJS_POINT3 room_volume_30_points[4] = {
#include "stg22/fall_volumes/room_volume_30_points.inc"
};
static NJS_VECTOR room_volume_30_normals[4] = {
#include "stg22/fall_volumes/room_volume_30_normals.inc"
};
static NJS_MODEL room_volume_30_model = {
#include "stg22/fall_volumes/room_volume_30_model.inc"
};
static NJS_OBJECT room_volume_30 = {
#include "stg22/fall_volumes/room_volume_30.inc"
};

static NJS_MATERIAL room_volume_31_material = {
#include "stg22/fall_volumes/room_volume_31_material.inc"
};
static Sint16 room_volume_31_strips[5] = {
#include "stg22/fall_volumes/room_volume_31_strips.inc"
};
static NJS_MESHSET room_volume_31_meshset = {
#include "stg22/fall_volumes/room_volume_31_meshset.inc"
};
static NJS_POINT3 room_volume_31_points[4] = {
#include "stg22/fall_volumes/room_volume_31_points.inc"
};
static NJS_VECTOR room_volume_31_normals[4] = {
#include "stg22/fall_volumes/room_volume_31_normals.inc"
};
static NJS_MODEL room_volume_31_model = {
#include "stg22/fall_volumes/room_volume_31_model.inc"
};
static NJS_OBJECT room_volume_31 = {
#include "stg22/fall_volumes/room_volume_31.inc"
};

static NJS_MATERIAL room_volume_32_material = {
#include "stg22/fall_volumes/room_volume_32_material.inc"
};
static Sint16 room_volume_32_strips[5] = {
#include "stg22/fall_volumes/room_volume_32_strips.inc"
};
static NJS_MESHSET room_volume_32_meshset = {
#include "stg22/fall_volumes/room_volume_32_meshset.inc"
};
static NJS_POINT3 room_volume_32_points[4] = {
#include "stg22/fall_volumes/room_volume_32_points.inc"
};
static NJS_VECTOR room_volume_32_normals[4] = {
#include "stg22/fall_volumes/room_volume_32_normals.inc"
};
static NJS_MODEL room_volume_32_model = {
#include "stg22/fall_volumes/room_volume_32_model.inc"
};
static NJS_OBJECT room_volume_32 = {
#include "stg22/fall_volumes/room_volume_32.inc"
};

static NJS_MATERIAL room_volume_33_material = {
#include "stg22/fall_volumes/room_volume_33_material.inc"
};
static Sint16 room_volume_33_strips[5] = {
#include "stg22/fall_volumes/room_volume_33_strips.inc"
};
static NJS_MESHSET room_volume_33_meshset = {
#include "stg22/fall_volumes/room_volume_33_meshset.inc"
};
static NJS_POINT3 room_volume_33_points[4] = {
#include "stg22/fall_volumes/room_volume_33_points.inc"
};
static NJS_VECTOR room_volume_33_normals[4] = {
#include "stg22/fall_volumes/room_volume_33_normals.inc"
};
static NJS_MODEL room_volume_33_model = {
#include "stg22/fall_volumes/room_volume_33_model.inc"
};
static NJS_OBJECT room_volume_33 = {
#include "stg22/fall_volumes/room_volume_33.inc"
};

static NJS_MATERIAL room_volume_34_material = {
#include "stg22/fall_volumes/room_volume_34_material.inc"
};
static Sint16 room_volume_34_strips[5] = {
#include "stg22/fall_volumes/room_volume_34_strips.inc"
};
static NJS_MESHSET room_volume_34_meshset = {
#include "stg22/fall_volumes/room_volume_34_meshset.inc"
};
static NJS_POINT3 room_volume_34_points[4] = {
#include "stg22/fall_volumes/room_volume_34_points.inc"
};
static NJS_VECTOR room_volume_34_normals[4] = {
#include "stg22/fall_volumes/room_volume_34_normals.inc"
};
static NJS_MODEL room_volume_34_model = {
#include "stg22/fall_volumes/room_volume_34_model.inc"
};
static NJS_OBJECT room_volume_34 = {
#include "stg22/fall_volumes/room_volume_34.inc"
};

static NJS_MATERIAL room_volume_35_material = {
#include "stg22/fall_volumes/room_volume_35_material.inc"
};
static Sint16 room_volume_35_strips[16] = {
#include "stg22/fall_volumes/room_volume_35_strips.inc"
};
static NJS_MESHSET room_volume_35_meshset = {
#include "stg22/fall_volumes/room_volume_35_meshset.inc"
};
static NJS_POINT3 room_volume_35_points[8] = {
#include "stg22/fall_volumes/room_volume_35_points.inc"
};
static NJS_VECTOR room_volume_35_normals[8] = {
#include "stg22/fall_volumes/room_volume_35_normals.inc"
};
static NJS_MODEL room_volume_35_model = {
#include "stg22/fall_volumes/room_volume_35_model.inc"
};
static NJS_OBJECT room_volume_35 = {
#include "stg22/fall_volumes/room_volume_35.inc"
};

static NJS_MATERIAL room_volume_36_material = {
#include "stg22/fall_volumes/room_volume_36_material.inc"
};
static Sint16 room_volume_36_strips[5] = {
#include "stg22/fall_volumes/room_volume_36_strips.inc"
};
static NJS_MESHSET room_volume_36_meshset = {
#include "stg22/fall_volumes/room_volume_36_meshset.inc"
};
static NJS_POINT3 room_volume_36_points[4] = {
#include "stg22/fall_volumes/room_volume_36_points.inc"
};
static NJS_VECTOR room_volume_36_normals[4] = {
#include "stg22/fall_volumes/room_volume_36_normals.inc"
};
static NJS_MODEL room_volume_36_model = {
#include "stg22/fall_volumes/room_volume_36_model.inc"
};
static NJS_OBJECT room_volume_36 = {
#include "stg22/fall_volumes/room_volume_36.inc"
};

static NJS_MATERIAL room_volume_37_material = {
#include "stg22/fall_volumes/room_volume_37_material.inc"
};
static Sint16 room_volume_37_strips[5] = {
#include "stg22/fall_volumes/room_volume_37_strips.inc"
};
static NJS_MESHSET room_volume_37_meshset = {
#include "stg22/fall_volumes/room_volume_37_meshset.inc"
};
static NJS_POINT3 room_volume_37_points[4] = {
#include "stg22/fall_volumes/room_volume_37_points.inc"
};
static NJS_VECTOR room_volume_37_normals[4] = {
#include "stg22/fall_volumes/room_volume_37_normals.inc"
};
static NJS_MODEL room_volume_37_model = {
#include "stg22/fall_volumes/room_volume_37_model.inc"
};
static NJS_OBJECT room_volume_37 = {
#include "stg22/fall_volumes/room_volume_37.inc"
};

static NJS_MATERIAL room_volume_08_material = {
#include "stg22/fall_volumes/room_volume_08_material.inc"
};
static Sint16 room_volume_08_strips[5] = {
#include "stg22/fall_volumes/room_volume_08_strips.inc"
};
static NJS_MESHSET room_volume_08_meshset = {
#include "stg22/fall_volumes/room_volume_08_meshset.inc"
};
static NJS_POINT3 room_volume_08_points[4] = {
#include "stg22/fall_volumes/room_volume_08_points.inc"
};
static NJS_VECTOR room_volume_08_normals[4] = {
#include "stg22/fall_volumes/room_volume_08_normals.inc"
};
static NJS_MODEL room_volume_08_model = {
#include "stg22/fall_volumes/room_volume_08_model.inc"
};
static NJS_OBJECT room_volume_08 = {
#include "stg22/fall_volumes/room_volume_08.inc"
};

static NJS_MATERIAL room_volume_11_material = {
#include "stg22/fall_volumes/room_volume_11_material.inc"
};
static Sint16 room_volume_11_strips[5] = {
#include "stg22/fall_volumes/room_volume_11_strips.inc"
};
static NJS_MESHSET room_volume_11_meshset = {
#include "stg22/fall_volumes/room_volume_11_meshset.inc"
};
static NJS_POINT3 room_volume_11_points[4] = {
#include "stg22/fall_volumes/room_volume_11_points.inc"
};
static NJS_VECTOR room_volume_11_normals[4] = {
#include "stg22/fall_volumes/room_volume_11_normals.inc"
};
static NJS_MODEL room_volume_11_model = {
#include "stg22/fall_volumes/room_volume_11_model.inc"
};
static NJS_OBJECT room_volume_11 = {
#include "stg22/fall_volumes/room_volume_11.inc"
};

static NJS_MATERIAL room_volume_10_material = {
#include "stg22/fall_volumes/room_volume_10_material.inc"
};
static Sint16 room_volume_10_strips[5] = {
#include "stg22/fall_volumes/room_volume_10_strips.inc"
};
static NJS_MESHSET room_volume_10_meshset = {
#include "stg22/fall_volumes/room_volume_10_meshset.inc"
};
static NJS_POINT3 room_volume_10_points[4] = {
#include "stg22/fall_volumes/room_volume_10_points.inc"
};
static NJS_VECTOR room_volume_10_normals[4] = {
#include "stg22/fall_volumes/room_volume_10_normals.inc"
};
static NJS_MODEL room_volume_10_model = {
#include "stg22/fall_volumes/room_volume_10_model.inc"
};
static NJS_OBJECT room_volume_10 = {
#include "stg22/fall_volumes/room_volume_10.inc"
};

static NJS_MATERIAL room_volume_09_material = {
#include "stg22/fall_volumes/room_volume_09_material.inc"
};
static Sint16 room_volume_09_strips[5] = {
#include "stg22/fall_volumes/room_volume_09_strips.inc"
};
static NJS_MESHSET room_volume_09_meshset = {
#include "stg22/fall_volumes/room_volume_09_meshset.inc"
};
static NJS_POINT3 room_volume_09_points[4] = {
#include "stg22/fall_volumes/room_volume_09_points.inc"
};
static NJS_VECTOR room_volume_09_normals[4] = {
#include "stg22/fall_volumes/room_volume_09_normals.inc"
};
static NJS_MODEL room_volume_09_model = {
#include "stg22/fall_volumes/room_volume_09_model.inc"
};
static NJS_OBJECT room_volume_09 = {
#include "stg22/fall_volumes/room_volume_09.inc"
};

static NJS_MATERIAL room_volume_12_material = {
#include "stg22/fall_volumes/room_volume_12_material.inc"
};
static Sint16 room_volume_12_strips[5] = {
#include "stg22/fall_volumes/room_volume_12_strips.inc"
};
static NJS_MESHSET room_volume_12_meshset = {
#include "stg22/fall_volumes/room_volume_12_meshset.inc"
};
static NJS_POINT3 room_volume_12_points[4] = {
#include "stg22/fall_volumes/room_volume_12_points.inc"
};
static NJS_VECTOR room_volume_12_normals[4] = {
#include "stg22/fall_volumes/room_volume_12_normals.inc"
};
static NJS_MODEL room_volume_12_model = {
#include "stg22/fall_volumes/room_volume_12_model.inc"
};
static NJS_OBJECT room_volume_12 = {
#include "stg22/fall_volumes/room_volume_12.inc"
};

static NJS_MATERIAL room_volume_13_material = {
#include "stg22/fall_volumes/room_volume_13_material.inc"
};
static Sint16 room_volume_13_strips[5] = {
#include "stg22/fall_volumes/room_volume_13_strips.inc"
};
static NJS_MESHSET room_volume_13_meshset = {
#include "stg22/fall_volumes/room_volume_13_meshset.inc"
};
static NJS_POINT3 room_volume_13_points[4] = {
#include "stg22/fall_volumes/room_volume_13_points.inc"
};
static NJS_VECTOR room_volume_13_normals[4] = {
#include "stg22/fall_volumes/room_volume_13_normals.inc"
};
static NJS_MODEL room_volume_13_model = {
#include "stg22/fall_volumes/room_volume_13_model.inc"
};
static NJS_OBJECT room_volume_13 = {
#include "stg22/fall_volumes/room_volume_13.inc"
};

static NJS_MATERIAL room_volume_14_material = {
#include "stg22/fall_volumes/room_volume_14_material.inc"
};
static Sint16 room_volume_14_strips[5] = {
#include "stg22/fall_volumes/room_volume_14_strips.inc"
};
static NJS_MESHSET room_volume_14_meshset = {
#include "stg22/fall_volumes/room_volume_14_meshset.inc"
};
static NJS_POINT3 room_volume_14_points[4] = {
#include "stg22/fall_volumes/room_volume_14_points.inc"
};
static NJS_VECTOR room_volume_14_normals[4] = {
#include "stg22/fall_volumes/room_volume_14_normals.inc"
};
static NJS_MODEL room_volume_14_model = {
#include "stg22/fall_volumes/room_volume_14_model.inc"
};
static NJS_OBJECT room_volume_14 = {
#include "stg22/fall_volumes/room_volume_14.inc"
};

static NJS_MATERIAL room_volume_15_material = {
#include "stg22/fall_volumes/room_volume_15_material.inc"
};
static Sint16 room_volume_15_strips[5] = {
#include "stg22/fall_volumes/room_volume_15_strips.inc"
};
static NJS_MESHSET room_volume_15_meshset = {
#include "stg22/fall_volumes/room_volume_15_meshset.inc"
};
static NJS_POINT3 room_volume_15_points[4] = {
#include "stg22/fall_volumes/room_volume_15_points.inc"
};
static NJS_VECTOR room_volume_15_normals[4] = {
#include "stg22/fall_volumes/room_volume_15_normals.inc"
};
static NJS_MODEL room_volume_15_model = {
#include "stg22/fall_volumes/room_volume_15_model.inc"
};
static NJS_OBJECT room_volume_15 = {
#include "stg22/fall_volumes/room_volume_15.inc"
};

static NJS_MATERIAL room_volume_16_material = {
#include "stg22/fall_volumes/room_volume_16_material.inc"
};
static Sint16 room_volume_16_strips[5] = {
#include "stg22/fall_volumes/room_volume_16_strips.inc"
};
static NJS_MESHSET room_volume_16_meshset = {
#include "stg22/fall_volumes/room_volume_16_meshset.inc"
};
static NJS_POINT3 room_volume_16_points[4] = {
#include "stg22/fall_volumes/room_volume_16_points.inc"
};
static NJS_VECTOR room_volume_16_normals[4] = {
#include "stg22/fall_volumes/room_volume_16_normals.inc"
};
static NJS_MODEL room_volume_16_model = {
#include "stg22/fall_volumes/room_volume_16_model.inc"
};
static NJS_OBJECT room_volume_16 = {
#include "stg22/fall_volumes/room_volume_16.inc"
};

static NJS_MATERIAL room_volume_17_material = {
#include "stg22/fall_volumes/room_volume_17_material.inc"
};
static Sint16 room_volume_17_strips[5] = {
#include "stg22/fall_volumes/room_volume_17_strips.inc"
};
static NJS_MESHSET room_volume_17_meshset = {
#include "stg22/fall_volumes/room_volume_17_meshset.inc"
};
static NJS_POINT3 room_volume_17_points[4] = {
#include "stg22/fall_volumes/room_volume_17_points.inc"
};
static NJS_VECTOR room_volume_17_normals[4] = {
#include "stg22/fall_volumes/room_volume_17_normals.inc"
};
static NJS_MODEL room_volume_17_model = {
#include "stg22/fall_volumes/room_volume_17_model.inc"
};
static NJS_OBJECT room_volume_17 = {
#include "stg22/fall_volumes/room_volume_17.inc"
};

static NJS_MATERIAL room_volume_18_material = {
#include "stg22/fall_volumes/room_volume_18_material.inc"
};
static Sint16 room_volume_18_strips[5] = {
#include "stg22/fall_volumes/room_volume_18_strips.inc"
};
static NJS_MESHSET room_volume_18_meshset = {
#include "stg22/fall_volumes/room_volume_18_meshset.inc"
};
static NJS_POINT3 room_volume_18_points[4] = {
#include "stg22/fall_volumes/room_volume_18_points.inc"
};
static NJS_VECTOR room_volume_18_normals[4] = {
#include "stg22/fall_volumes/room_volume_18_normals.inc"
};
static NJS_MODEL room_volume_18_model = {
#include "stg22/fall_volumes/room_volume_18_model.inc"
};
static NJS_OBJECT room_volume_18 = {
#include "stg22/fall_volumes/room_volume_18.inc"
};

static NJS_MATERIAL room_volume_19_material = {
#include "stg22/fall_volumes/room_volume_19_material.inc"
};
static Sint16 room_volume_19_strips[16] = {
#include "stg22/fall_volumes/room_volume_19_strips.inc"
};
static NJS_MESHSET room_volume_19_meshset = {
#include "stg22/fall_volumes/room_volume_19_meshset.inc"
};
static NJS_POINT3 room_volume_19_points[8] = {
#include "stg22/fall_volumes/room_volume_19_points.inc"
};
static NJS_VECTOR room_volume_19_normals[8] = {
#include "stg22/fall_volumes/room_volume_19_normals.inc"
};
static NJS_MODEL room_volume_19_model = {
#include "stg22/fall_volumes/room_volume_19_model.inc"
};
static NJS_OBJECT room_volume_19 = {
#include "stg22/fall_volumes/room_volume_19.inc"
};

static NJS_MATERIAL room_volume_20_material = {
#include "stg22/fall_volumes/room_volume_20_material.inc"
};
static Sint16 room_volume_20_strips[5] = {
#include "stg22/fall_volumes/room_volume_20_strips.inc"
};
static NJS_MESHSET room_volume_20_meshset = {
#include "stg22/fall_volumes/room_volume_20_meshset.inc"
};
static NJS_POINT3 room_volume_20_points[4] = {
#include "stg22/fall_volumes/room_volume_20_points.inc"
};
static NJS_VECTOR room_volume_20_normals[4] = {
#include "stg22/fall_volumes/room_volume_20_normals.inc"
};
static NJS_MODEL room_volume_20_model = {
#include "stg22/fall_volumes/room_volume_20_model.inc"
};
static NJS_OBJECT room_volume_20 = {
#include "stg22/fall_volumes/room_volume_20.inc"
};

static NJS_MATERIAL room_volume_21_material = {
#include "stg22/fall_volumes/room_volume_21_material.inc"
};
static Sint16 room_volume_21_strips[5] = {
#include "stg22/fall_volumes/room_volume_21_strips.inc"
};
static NJS_MESHSET room_volume_21_meshset = {
#include "stg22/fall_volumes/room_volume_21_meshset.inc"
};
static NJS_POINT3 room_volume_21_points[4] = {
#include "stg22/fall_volumes/room_volume_21_points.inc"
};
static NJS_VECTOR room_volume_21_normals[4] = {
#include "stg22/fall_volumes/room_volume_21_normals.inc"
};
static NJS_MODEL room_volume_21_model = {
#include "stg22/fall_volumes/room_volume_21_model.inc"
};
static NJS_OBJECT room_volume_21 = {
#include "stg22/fall_volumes/room_volume_21.inc"
};

static NJS_MATERIAL room_volume_00_material = {
#include "stg22/fall_volumes/room_volume_00_material.inc"
};
static Sint16 room_volume_00_strips[10] = {
#include "stg22/fall_volumes/room_volume_00_strips.inc"
};
static NJS_MESHSET room_volume_00_meshset = {
#include "stg22/fall_volumes/room_volume_00_meshset.inc"
};
static NJS_POINT3 room_volume_00_points[6] = {
#include "stg22/fall_volumes/room_volume_00_points.inc"
};
static NJS_VECTOR room_volume_00_normals[6] = {
#include "stg22/fall_volumes/room_volume_00_normals.inc"
};
static NJS_MODEL room_volume_00_model = {
#include "stg22/fall_volumes/room_volume_00_model.inc"
};
static NJS_OBJECT room_volume_00 = {
#include "stg22/fall_volumes/room_volume_00.inc"
};

static NJS_MATERIAL room_volume_01_material = {
#include "stg22/fall_volumes/room_volume_01_material.inc"
};
static Sint16 room_volume_01_strips[5] = {
#include "stg22/fall_volumes/room_volume_01_strips.inc"
};
static NJS_MESHSET room_volume_01_meshset = {
#include "stg22/fall_volumes/room_volume_01_meshset.inc"
};
static NJS_POINT3 room_volume_01_points[4] = {
#include "stg22/fall_volumes/room_volume_01_points.inc"
};
static NJS_VECTOR room_volume_01_normals[4] = {
#include "stg22/fall_volumes/room_volume_01_normals.inc"
};
static NJS_MODEL room_volume_01_model = {
#include "stg22/fall_volumes/room_volume_01_model.inc"
};
static NJS_OBJECT room_volume_01 = {
#include "stg22/fall_volumes/room_volume_01.inc"
};

static NJS_MATERIAL room_volume_02_material = {
#include "stg22/fall_volumes/room_volume_02_material.inc"
};
static Sint16 room_volume_02_strips[5] = {
#include "stg22/fall_volumes/room_volume_02_strips.inc"
};
static NJS_MESHSET room_volume_02_meshset = {
#include "stg22/fall_volumes/room_volume_02_meshset.inc"
};
static NJS_POINT3 room_volume_02_points[4] = {
#include "stg22/fall_volumes/room_volume_02_points.inc"
};
static NJS_VECTOR room_volume_02_normals[4] = {
#include "stg22/fall_volumes/room_volume_02_normals.inc"
};
static NJS_MODEL room_volume_02_model = {
#include "stg22/fall_volumes/room_volume_02_model.inc"
};
static NJS_OBJECT room_volume_02 = {
#include "stg22/fall_volumes/room_volume_02.inc"
};

static NJS_MATERIAL room_volume_03_material = {
#include "stg22/fall_volumes/room_volume_03_material.inc"
};
static Sint16 room_volume_03_strips[5] = {
#include "stg22/fall_volumes/room_volume_03_strips.inc"
};
static NJS_MESHSET room_volume_03_meshset = {
#include "stg22/fall_volumes/room_volume_03_meshset.inc"
};
static NJS_POINT3 room_volume_03_points[4] = {
#include "stg22/fall_volumes/room_volume_03_points.inc"
};
static NJS_VECTOR room_volume_03_normals[4] = {
#include "stg22/fall_volumes/room_volume_03_normals.inc"
};
static NJS_MODEL room_volume_03_model = {
#include "stg22/fall_volumes/room_volume_03_model.inc"
};
static NJS_OBJECT room_volume_03 = {
#include "stg22/fall_volumes/room_volume_03.inc"
};

static NJS_MATERIAL room_volume_22_material = {
#include "stg22/fall_volumes/room_volume_22_material.inc"
};
static Sint16 room_volume_22_strips[5] = {
#include "stg22/fall_volumes/room_volume_22_strips.inc"
};
static NJS_MESHSET room_volume_22_meshset = {
#include "stg22/fall_volumes/room_volume_22_meshset.inc"
};
static NJS_POINT3 room_volume_22_points[4] = {
#include "stg22/fall_volumes/room_volume_22_points.inc"
};
static NJS_VECTOR room_volume_22_normals[4] = {
#include "stg22/fall_volumes/room_volume_22_normals.inc"
};
static NJS_MODEL room_volume_22_model = {
#include "stg22/fall_volumes/room_volume_22_model.inc"
};
static NJS_OBJECT room_volume_22 = {
#include "stg22/fall_volumes/room_volume_22.inc"
};

struct gadget_kill_volume gadget_kill_volumes[GADGET_KILL_VOLUMES] = {
#include "stg22/fall_volumes/gadget_kill_volumes.inc"
};
