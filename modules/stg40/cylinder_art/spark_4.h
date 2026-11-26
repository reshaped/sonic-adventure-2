#ifndef STG40_CYLINDER_ART_SPARK_4_H
#define STG40_CYLINDER_ART_SPARK_4_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME textures_spark_4[1] = {
#include "stg40/cylinder_art/spark_4/textures_spark_4.inc"
};

static NJS_TEXLIST gravity_cylinder_spark_4_texlist = {
#include "stg40/cylinder_art/spark_4/gravity_cylinder_spark_4_texlist.inc"
};

static Sint16 gravity_cylinder_spark_4_object_polygons[25] = {
#include "stg40/cylinder_art/spark_4/gravity_cylinder_spark_4_object_polygons.inc"
};

static Sint32 gravity_cylinder_spark_4_object_vertices[19] = {
#include "stg40/cylinder_art/spark_4/gravity_cylinder_spark_4_object_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_spark_4_object_model = {
#include "stg40/cylinder_art/spark_4/gravity_cylinder_spark_4_object_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_spark_4_object = {
#include "stg40/cylinder_art/spark_4/gravity_cylinder_spark_4_object.inc"
};

#endif
