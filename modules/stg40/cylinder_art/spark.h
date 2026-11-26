#ifndef STG40_CYLINDER_ART_SPARK_H
#define STG40_CYLINDER_ART_SPARK_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME textures_spark[2] = {
#include "stg40/cylinder_art/spark/textures_spark.inc"
};

static NJS_TEXLIST gravity_cylinder_spark_texlist = {
#include "stg40/cylinder_art/spark/gravity_cylinder_spark_texlist.inc"
};

static Sint16 gravity_cylinder_spark_object_polygons[747] = {
#include "stg40/cylinder_art/spark/gravity_cylinder_spark_object_polygons.inc"
};

static Sint32 gravity_cylinder_spark_object_vertices[519] = {
#include "stg40/cylinder_art/spark/gravity_cylinder_spark_object_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_spark_object_model = {
#include "stg40/cylinder_art/spark/gravity_cylinder_spark_object_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_spark_object[1] = {
#include "stg40/cylinder_art/spark/gravity_cylinder_spark_object.inc"
};

#endif
