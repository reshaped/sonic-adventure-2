#ifndef STG40_CYLINDER_ART_ANIM_H
#define STG40_CYLINDER_ART_ANIM_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

static Sint16 spark_1_scroll_uvs[82] = {
#include "stg40/cylinder_art/anim/spark_1_scroll_uvs.inc"
};

static struct texture_anim gravity_cylinder_spark_1_anim = {
#include "stg40/cylinder_art/anim/gravity_cylinder_spark_1_anim.inc"
};

static Sint16 spark_2_scroll_uvs[12] = {
#include "stg40/cylinder_art/anim/spark_2_scroll_uvs.inc"
};

static struct texture_anim gravity_cylinder_spark_2_anim = {
#include "stg40/cylinder_art/anim/gravity_cylinder_spark_2_anim.inc"
};

static Sint16 spark_3_scroll_uvs[12] = {
#include "stg40/cylinder_art/anim/spark_3_scroll_uvs.inc"
};

static struct texture_anim gravity_cylinder_spark_3_anim = {
#include "stg40/cylinder_art/anim/gravity_cylinder_spark_3_anim.inc"
};

static Sint16 spark_4_scroll_uvs[12] = {
#include "stg40/cylinder_art/anim/spark_4_scroll_uvs.inc"
};

static struct texture_anim gravity_cylinder_spark_4_anim = {
#include "stg40/cylinder_art/anim/gravity_cylinder_spark_4_anim.inc"
};

#endif
