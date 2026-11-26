#include <ninja.h>
#include <sg_xpt.h>

#include "../ninja/copy_elements.h"

void njMemCopy4(void *dst, const void *src, Sint32 num)
{
    COPY_ELEMENTS(Uint32, dst, src, num);
}

/* njMemCopy2 starts at the next four-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      4
#pragma endasm
#endif
#endif
/* clang-format on */

void njMemCopy2(void *dst, const void *src, Sint32 num)
{
    COPY_ELEMENTS(Sint16, dst, src, num);
}

/* njMemCopy starts at the next four-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      4
#pragma endasm
#endif
#endif
/* clang-format on */

void njMemCopy(void *dst, const void *src, Sint32 num)
{
    COPY_ELEMENTS(Sint8, dst, src, num);
}
