#ifndef STG28_MODELS_SPIDER_WEB_H
#define STG28_MODELS_SPIDER_WEB_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cave_spider_web_texture_names[1] = {
#include "stg28/models/spider_web/cave_spider_web_texture_names.inc"
};

static NJS_TEXLIST cave_spider_web_texlist = {
#include "stg28/models/spider_web/cave_spider_web_texlist.inc"
};

static Sint16 cave_spider_web_polygons[25] = {
#include "stg28/models/spider_web/cave_spider_web_polygons.inc"
};

static Sint32 cave_spider_web_vertices[19] = {
#include "stg28/models/spider_web/cave_spider_web_vertices.inc"
};

static NJS_CNK_MODEL cave_spider_web_model = {
#include "stg28/models/spider_web/cave_spider_web_model.inc"
};

static NJS_CNK_OBJECT cave_spider_web_object = {
#include "stg28/models/spider_web/cave_spider_web_object.inc"
};

#endif
