/* the texture copy's entries for each unit, from 32 bytes down to 1, which
 * njTexMemCopy picks among */
#ifndef NINJA_TEX_COPY_H
#define NINJA_TEX_COPY_H

#include <sg_xpt.h>

/* each copies size bytes from src to dst, a unit at a time, the unit its
 * name gives. dst, src and size must be multiples of the unit, and size above
 * 0. the 32, 16 and 8 copies need FPSCR.SZ clear, and leave it clear.
 * njTexMemCopy takes the largest unit that divides all three */
void njTexMemCopy32(void *dst, const void *src, Uint32 size);
void njTexMemCopy16(void *dst, const void *src, Uint32 size);
void njTexMemCopy8(void *dst, const void *src, Uint32 size);
void njTexMemCopy4(void *dst, const void *src, Uint32 size);
void njTexMemCopy2(void *dst, const void *src, Uint32 size);
void njTexMemCopy1(void *dst, const void *src, Uint32 size);

#endif
