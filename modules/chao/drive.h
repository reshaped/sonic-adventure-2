/* the Chaos Drives a player releases from the item stock into a garden */
#ifndef CHAO_DRIVE_H
#define CHAO_DRIVE_H
#include <ninja.h>
#include <sg_xpt.h>

/* make a drive of kind, a CHAOS_DRIVE_ color, at position and moving at
 * velocity */
#ifdef NONMATCHING
/* this side returns FALSE where an allocation fails or the garden has no slot
 * for the drive. FALSE leaves no drive */
Bool garden_drive_create(Uint8 kind, NJS_POINT3 *position,
                         NJS_VECTOR *velocity);
#else
void garden_drive_create(Uint8 kind, NJS_POINT3 *position,
                         NJS_VECTOR *velocity);
#endif

#endif
