/* njTexMemCopy and its 32-, 16- and 8-byte copies for GCC. make dc compiles
 * this in place of sdk/ninja/njTexMemCopy.src. the shipped njTexMemCopy runs
 * on past that file's end, into the branch SHC's njTexScalar.c opens with.
 * GCC's njTexScalar.c has no such branch, so this njTexMemCopy calls the copy
 * it picks */
#include <ninja.h>
#include <nj_texlist.h>
#include <sg_xpt.h>
#include <stdint.h>
#include "../ninja/tex_copy.h"

/* a size below a copy's width gives its njMemCopy call a count of 0.
 * njMemCopy32, njMemCopy16 and njMemCopy8 copy 2^32 blocks for a count of 0 */
void njTexMemCopy32(void *dst, const void *src, Uint32 size)
{
    if (size >= 32) {
        njMemCopy32(dst, src, (Sint32)(size / 32));
    }
}

void njTexMemCopy16(void *dst, const void *src, Uint32 size)
{
    if (size >= 16) {
        njMemCopy16(dst, src, (Sint32)(size / 16));
    }
}

void njTexMemCopy8(void *dst, const void *src, Uint32 size)
{
    if (size >= 8) {
        njMemCopy8(dst, src, (Sint32)(size / 8));
    }
}

void njTexMemCopy(void *dst, void *src, Uint32 size)
{
    uintptr_t combined = (uintptr_t)dst | (uintptr_t)src | size;

    if (combined % 32 == 0) {
        njTexMemCopy32(dst, src, size);
    } else if (combined % 16 == 0) {
        njTexMemCopy16(dst, src, size);
    } else if (combined % 8 == 0) {
        njTexMemCopy8(dst, src, size);
    } else if (combined % 4 == 0) {
        njTexMemCopy4(dst, src, size);
    } else if (combined % 2 == 0) {
        njTexMemCopy2(dst, src, size);
    } else {
        njTexMemCopy1(dst, src, size);
    }
}
