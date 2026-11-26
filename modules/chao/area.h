/* the Chao areas: the seven overlays CHAO.PRS reads, one at a time, into a
 * slot above itself */
#ifndef CHAO_AREA_H
#define CHAO_AREA_H

#include "system/loader.h"

/* the buffer that the Neutral Garden, the Hero Garden and the race entrance
 * compile their land into. it starts past the last byte of the area's
 * storage. retail is its address in the image. world_build_land rounds the
 * buffer's address up to a multiple of 32 */
#ifdef NONMATCHING
/* this side takes end, which GNU ld places after the area's last occupied
 * section. in the Neutral and Hero Gardens, that section is the initialized
 * data */
#define CHAO_LAND_BUFFER(retail) end
#else
#define CHAO_LAND_BUFFER(retail) (retail)
#endif

#endif
