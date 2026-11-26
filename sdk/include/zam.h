/* what the non-matching code takes with SH4ZAM's headers. each of the tree's
 * headers that reads SH4ZAM's includes this one first. it checks GCC's flags.
 * it provides the inline markers, the matrix stack's store, the test for
 * system RAM and the type check.
 *
 * XMTRX, XF0 to XF15 as a matrix, is state GCC doesn't see. GCC keeps
 * volatile asms in order among themselves, and the tree's C relies on that
 * order for XMTRX. so an asm that reads or writes XMTRX must be volatile. each
 * of SH4ZAM's XMTRX entries that the tree calls runs its asm volatile */
#ifndef ZAM_H
#define ZAM_H
#include <sg_xpt.h>
#include <sh4zam/shz_cdefs.h>
#if SHZ_BACKEND != SHZ_SH4
#error "the helpers run SH4ZAM's SH-4 back end, which -m4-single-only selects"
#endif
#ifdef __FAST_MATH__
#error "play's math takes no fast-math, which would let GCC reorder it"
#endif

/* a helper that GCC copies into each caller, at every level of optimization */
#define ZAM_INLINE static inline __attribute__((always_inline))
/* ZAM_INLINE, and GCC copies each function the helper calls into it too. at
 * -Os GCC would keep a callee of SH4ZAM's out of line, and call it */
#define ZAM_INLINE_FLAT static inline __attribute__((always_inline, flatten))

/* Store XMTRX in a 32-byte-aligned slot. MOVCA.L claims both cache lines.
 * The paired stores overwrite R0's temporary word in each line. */
ZAM_INLINE void zam_store_slot(float slot[16])
{
    float *at = slot;

    __asm__ volatile("movca.l r0,@%0\n\t"
                     "add #32,%0\n\t"
                     "movca.l r0,@%0\n\t"
                     "add #32,%0\n\t"
                     "fschg\n\t"
                     "fmov xd14,@-%0\n\t"
                     "fmov xd12,@-%0\n\t"
                     "fmov xd10,@-%0\n\t"
                     "fmov xd8,@-%0\n\t"
                     "fmov xd6,@-%0\n\t"
                     "fmov xd4,@-%0\n\t"
                     "fmov xd2,@-%0\n\t"
                     "fmov xd0,@-%0\n\t"
                     "fschg"
                     : "+r"(at), "=m"(*(float (*)[16])slot));
}

/* system RAM is area 3. ZAM_SYSTEM_RAM_BITS picks bits 26 to 28 and bit 30 of
 * an address. they hold ZAM_SYSTEM_RAM exactly where the address reaches
 * system RAM through P0 below 0x40000000, P1 or P2. the test of bit 30 refuses
 * P4, and with it system RAM through P3 */
#define ZAM_SYSTEM_RAM_BITS 0x5c000000U
#define ZAM_SYSTEM_RAM 0x0c000000U

/* whether address reaches system RAM, where SH4ZAM's copies and sets may
 * store as they do. another area may take only stores of its own width, in
 * the order the shipped routine stores */
ZAM_INLINE Bool zam_in_system_ram(uintptr_t address)
{
    return (address & ZAM_SYSTEM_RAM_BITS) == ZAM_SYSTEM_RAM;
}

/* fail the compile unless zam_##name's type is compatible with name's. a
 * function-like macro of name calls the helper zam_##name in name's place,
 * so the two must agree. name must be declared above */
#define ZAM_ASSERT_COMPATIBLE(name)                                            \
    _Static_assert(__builtin_types_compatible_p(__typeof__(name),              \
                                                __typeof__(zam_##name)),       \
                   "zam_" #name " doesn't take and return what " #name         \
                   " does")
#endif
