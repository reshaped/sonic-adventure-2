#include <ninja.h>
#include <sg_xpt.h>

#include "stg31/sky.h"

/* the objects of Green Hill's two kill areas, the water */

static NJS_MATERIAL water_a_material[1] = {
#include "stg31/water_areas/water_a_material.inc"
};

static Sint16 water_a_strips[21] = {
#include "stg31/water_areas/water_a_strips.inc"
};

static NJS_MESHSET water_a_meshes[1] = {
#include "stg31/water_areas/water_a_meshes.inc"
};

static NJS_POINT3 water_a_points[17] = {
#include "stg31/water_areas/water_a_points.inc"
};

static NJS_VECTOR water_a_normals[17] = {
#include "stg31/water_areas/water_a_normals.inc"
};

static NJS_MODEL water_a_model = {
#include "stg31/water_areas/water_a_model.inc"
};

NJS_OBJECT green_hill_water_area_a = {
#include "stg31/water_areas/green_hill_water_area_a.inc"
};

static NJS_MATERIAL water_b_material[1] = {
#include "stg31/water_areas/water_b_material.inc"
};

static Sint16 water_b_strips[30] = {
#include "stg31/water_areas/water_b_strips.inc"
};

static NJS_MESHSET water_b_meshes[1] = {
#include "stg31/water_areas/water_b_meshes.inc"
};

static NJS_POINT3 water_b_points[18] = {
#include "stg31/water_areas/water_b_points.inc"
};

static NJS_VECTOR water_b_normals[18] = {
#include "stg31/water_areas/water_b_normals.inc"
};

static NJS_MODEL water_b_model = {
#include "stg31/water_areas/water_b_model.inc"
};

NJS_OBJECT green_hill_water_area_b = {
#include "stg31/water_areas/green_hill_water_area_b.inc"
};
