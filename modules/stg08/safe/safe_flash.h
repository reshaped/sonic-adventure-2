/* Safe flash geometry. */
#ifndef STG08_SAFE_SAFE_FLASH_H
#define STG08_SAFE_SAFE_FLASH_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME safe_flash_names[1] = {
#include "stg08/safe/safe_flash/safe_flash_names.inc"
};

static NJS_TEXLIST security_safe_flash_texlist = {
#include "stg08/safe/safe_flash/security_safe_flash_texlist.inc"
};

static Sint16 safe_flash_polygons[91] = {
#include "stg08/safe/safe_flash/safe_flash_polygons.inc"
};

static Sint32 safe_flash_vertices[67] = {
#include "stg08/safe/safe_flash/safe_flash_vertices.inc"
};

static NJS_CNK_MODEL safe_flash_model = {
#include "stg08/safe/safe_flash/safe_flash_model.inc"
};

static NJS_CNK_OBJECT security_safe_flash[1] = {
#include "stg08/safe/safe_flash/security_safe_flash.inc"
};

#endif
