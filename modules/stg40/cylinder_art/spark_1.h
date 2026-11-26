#ifndef STG40_CYLINDER_ART_SPARK_1_H
#define STG40_CYLINDER_ART_SPARK_1_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME textures_spark_1[1] = {
#include "stg40/cylinder_art/spark_1/textures_spark_1.inc"
};

static NJS_TEXLIST gravity_cylinder_spark_1_texlist = {
#include "stg40/cylinder_art/spark_1/gravity_cylinder_spark_1_texlist.inc"
};

static Sint16 gravity_cylinder_spark_1_object_polygons[117] = {
#include "stg40/cylinder_art/spark_1/gravity_cylinder_spark_1_object_polygons.inc"
};

static Sint32 gravity_cylinder_spark_1_object_vertices[131] = {
#include "stg40/cylinder_art/spark_1/gravity_cylinder_spark_1_object_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_spark_1_object_model = {
#include "stg40/cylinder_art/spark_1/gravity_cylinder_spark_1_object_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_spark_1_object = {
#include "stg40/cylinder_art/spark_1/gravity_cylinder_spark_1_object.inc"
};

#endif
