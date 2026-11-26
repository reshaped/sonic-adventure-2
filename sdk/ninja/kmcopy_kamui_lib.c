/* Kamui's copies by longs and by pairs of longs, and its write-back of a cache
 * line */
#include <kamui.h>
#include <kamui_internal.h>
#include "../ninja/copy_elements.h"

#ifdef NONMATCHING
#include <ninja.h>
#include <stdint.h>

/* GCC's layout may put kmiMemCopy4 out of reach of the assembly's BF, so GCC
 * compiles this C instead. njMemCopy8 copies the pairs and leaves SZ clear, as
 * kmiMemCopy8 must */
void kmiMemCopy8(KMDWORD *dst, const KMDWORD *src, KMDWORD size)
{
    uintptr_t alignment = (uintptr_t)dst | (uintptr_t)src | size;

    if (size < sizeof(*dst)) {
        return;
    }
    if ((alignment & 7) == 0) {
        njMemCopy8(dst, src, (Sint32)(size / 8));
    } else {
        kmiMemCopy4(dst, src, size);
    }
}
#else
#ifdef DREAMCAST
/* a BF reaches only within its own section, so kmiMemCopy8 and kmiMemCopy4
 * share PSGWORD. the block below switches back to PSG at its end. #pragma
 * section WORD then puts kmiMemCopy4 in PSGWORD, 0x1e bytes in, right after
 * kmiMemCopy8 */
/* clang-format off */
/* @asm operand: fschg-slot, FSCHG at 0x8c14d1dc restores SZ in the RTS slot */
#pragma asm
        .SECTION    PSGWORD,CODE,ALIGN=32
        .EXPORT     _kmiMemCopy8

; void kmiMemCopy8(KMDWORD *dstPtr, const KMDWORD *SrcAdr, KMDWORD Size)
; kmasm_kamui_lib_, an R11b ninja.lib member, holds it from 0x8c14d1c0
_kmiMemCopy8:
        MOV         R4,R0
        OR          R5,R0
        OR          R6,R0
        TST         #7,R0
        BF          _kmiMemCopy4
        FSCHG                               ; SZ to 1, for the pairs
        SHLR2       R6
        SHLR        R6
?pairs:
        FMOV        @R5+,DR0
        DT          R6
        FMOV        DR0,@R4
        BF/S        ?pairs
        ADD         #8,R4
        RTS
        FSCHG                               ; SZ to 0 again
        .SECTION    PSG,CODE,ALIGN=32
#pragma endasm
#pragma section WORD
/* clang-format on */
#endif
#endif

void kmiMemCopy4(void *dst, const void *src, KMDWORD size)
{
#ifdef NONMATCHING
    volatile KMDWORD *to = dst;
    const volatile KMDWORD *from = src;

    /* either end may be a register or video memory, so each long is read
     * and written once */
    for (size >>= 2; size != 0; --size) {
        *to++ = *from++;
    }
#else
    size >>= 2;
    COPY_ELEMENTS(KMDWORD, dst, src, size);
#endif
}

#ifdef DREAMCAST
#ifdef NONMATCHING
/* the assembly's write-back below. the memory clobber keeps GCC's own loads
 * and stores on their side of it */
void kmiPurgeCache(void *address)
{
    __asm__ volatile("ocbwb @%0" : : "r"(address) : "memory");
}
#else
/* clang-format off */
/* @asm opcode: cache-line, OCBWB @R4 at 0x8c14d1ee writes the line back */
#pragma asm
        .EXPORT     _kmiPurgeCache
; void kmiPurgeCache(void *addr)
; write the operand cache line that holds addr back to memory if it is dirty.
; the line stays valid
_kmiPurgeCache:
        OCBWB       @R4
        RTS
        NOP
        .SECTION    PSG,CODE,ALIGN=32
#pragma endasm
#pragma section
/* clang-format on */
#endif
#endif
