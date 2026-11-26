#ifndef STG40_CYLINDER_ART_FIELD_H
#define STG40_CYLINDER_ART_FIELD_H

#include <ninja.h>
#include <sg_xpt.h>

static Sint16 spark_field_polygons[25] = {
#include "stg40/cylinder_art/field/spark_field_polygons.inc"
};

static struct cylinder_spark_vertices gravity_cylinder_spark_points = {
#include "stg40/cylinder_art/field/gravity_cylinder_spark_points.inc"
};

static NJS_CNK_MODEL spark_field_model = {
#include "stg40/cylinder_art/field/spark_field_model.inc"
};

static NJS_CNK_OBJECT spark_field_object = {
#include "stg40/cylinder_art/field/spark_field_object.inc"
};

#endif
