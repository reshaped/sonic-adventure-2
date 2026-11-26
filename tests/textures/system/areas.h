/* A host buffer stands in for the fixed Dreamcast work and stage areas. */
#ifndef SYSTEM_AREAS_H
#define SYSTEM_AREAS_H
#include <sg_xpt.h>

extern Uint8 texture_workspace[8192];
#define WORK_AREA texture_workspace
#define WORK_AREA_END (WORK_AREA + sizeof(texture_workspace))
#define STAGE_SLOT WORK_AREA
#define DEMO_AREA WORK_AREA_END
#endif
