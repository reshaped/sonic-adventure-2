/* The object archive used in both Wild Canyon stages. */
#ifndef STG16_OBJECT_ARCHIVE_H
#define STG16_OBJECT_ARCHIVE_H

#include <ninja.h>

static NJS_TEXNAME stage_archive_names[24] = {
#include "stg16/object_archive/stage_archive_names.inc"
};

static NJS_TEXLIST stage_archive = {
#include "stg16/object_archive/stage_archive.inc"
};

#endif
