/* the ADX file system's calls on the SH-4's operand cache. they purge the
 * whole cache, or drop the lines over a buffer. a line dropped is
 * invalidated, and nothing is written back. a line purged is written back if
 * the cache changed it, then dropped */
#include <adx_fcch.h>
#include <machine.h>
#include <sg_xpt.h>

/* CCR, the cache control register, at CCR_ADDRESS */
#define CCR_ADDRESS 0xff00001c
#ifdef NONMATCHING
/* GCC would merge ADXF_InitCache_P2_'s three stores to a plain CCR into one.
 * through a volatile CCR, GCC's code makes all three, as SHC's does */
#define CCR (*(volatile Uint32 *)CCR_ADDRESS)
#else
/* @hack type: CCR as a plain Uint32, where a volatile one would do */
#define CCR (*(Uint32 *)CCR_ADDRESS)
#endif
/* a 1 written to CCR_OCI drops every line of the operand cache. a 1 written
 * to CCR_ICI drops every line of the instruction cache */
#define CCR_OCI 0x008
#define CCR_ICI 0x800
/* CCR's ORA and OIX bits in ccr, a value read from CCR, as 0 or 1. ORA
 * selects RAM mode, in which the operand cache keeps some of its entries as
 * RAM. OIX selects index mode, in which address bit 25 picks an entry's half
 * in place of bit 13 */
#define CCR_ORA_SHIFT 5
#define CCR_OIX_SHIFT 7
#define CCR_ORA(ccr) (((ccr) >> CCR_ORA_SHIFT) & 1)
#define CCR_OIX(ccr) (((ccr) >> CCR_OIX_SHIFT) & 1)
#define LINE_SHIFT 5
#define LINE_SIZE (1 << LINE_SHIFT)
/* entry i's word in the operand cache's address array: its tag and state.
 * the tag is the word's TAG_MASK bits, which hold the same bits of the line's
 * address. a store of 0 purges the entry */
#define OC_ADDRESS_ARRAY 0xf4000000
#define OC_ENTRY(i)                                                            \
    (*(volatile Uint32 *)(OC_ADDRESS_ARRAY | ((i) << LINE_SHIFT)))
#define TAG_MASK 0x1ffffc00
/* the array's OC_ENTRIES entries form two halves of OC_HALF. in RAM mode,
 * only the first OC_CACHED entries of each half cache lines */
#define OC_ENTRIES 512
#define OC_HALF (OC_ENTRIES / 2)
#define OC_CACHED 128
/* PHYSICAL_LINE_MASK gives an address's line as a 29-bit physical address.
 * the entry walks compare lines in that form */
#define PHYSICAL_LINE_MASK 0x1fffffe0

/* UNCACHED(walk) is a walk over the cache's entries, run in P2 as the
 * shipped code runs it. P2 is the uncached mirror of memory */
#ifdef NONMATCHING
/* GCC calls the walk as a function, so this side calls its P2 address */
#define P2_BASE 0xa0000000
#define P2(address) ((Uint32)(address) | P2_BASE)
#define UNCACHED(walk) ((void (*)(Uint32, Uint32, Uint32))P2(walk))
#else
/* SHC expands the walk inline, in code that enter_p2 has moved to P2 */
#define UNCACHED(walk) walk
#endif

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void adxf_purge_all(void);
void ADXF_InitCache_P2_(Uint32 ccr);

/* the member's version */
const Uint8 adxf_cch_ver[4] = {1, 3, 5, 0};
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* a call can't move its caller to P2, so this side does nothing. ADXF_Ocbi
 * calls its walks through UNCACHED instead */
static inline __attribute__((always_inline)) void enter_p2(void)
{
}
#else
/* go on at the next instruction's address in P2, so that the rest of the
 * caller runs uncached */
/* @asm open: no inspect rule reads its jump to an address it computes */
#pragma inline_asm(enter_p2)
static void enter_p2(void)
{
        MOVA        ?uncached,R0
        MOV.L       ?p2,R1
        OR          R0,R1
        JMP         @R1
        NOP
        .ALIGN      4
?p2:
        .DATA.L     H'A0000000
?uncached:
}
#endif

/* drop the lines over nbyte bytes from first, a line's address. nbyte must
 * not be 0 */
/* @asm opcode: cache-line, OCBI @R4 at 0x8c158590 drops one line */
#pragma inline_asm(ocbi_lines)
static void ocbi_lines(Uint32 first, Uint32 nbyte)
{
        ADD         #31,R5
        SHLR2       R5
        SHLR2       R5
        SHLR        R5
?line:
        OCBI        @R4
        DT          R5
        BF/S        ?line
        ADD         #32,R4
}

/* drop each line of entries 0 to 255 that lies from first to last. first
 * and last are lines as PHYSICAL_LINE_MASK gives them. with ram set, the
 * walk stops after entry 127. an entry's line is its tag ORed with its
 * offset in the array. the body holds OC_ADDRESS_ARRAY in R8 and TAG_MASK in
 * R9. R0 steps by LINE_SIZE through the entries' offsets, up to R2. R3 holds
 * entry 127's offset */
/* @asm opcode: cache-line, OCBI @R1 at 0x8c158642 drops an entry's line */
#pragma inline_asm(ocbi_low_entries)
static void ocbi_low_entries(Uint32 first, Uint32 last, Uint32 ram)
{
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV.L       #H'00000000,R0
        MOV.L       #H'F4000000,R8
        MOV.L       #H'1FFFFC00,R9
        MOV.L       #H'00001FFF,R10
        MOV.L       #H'00002000,R2
        MOV.L       #H'00000FE0,R3
        BRA         ?test
        NOP
?entry:
        MOV         R0,R1
        OR          R8,R1
        MOV.L       @R1,R1
        AND         R9,R1
        OR          R0,R1
        CMP/HI      R1,R4
        BT          ?next
        CMP/HI      R5,R1
        BT          ?next
        OCBI        @R1
?next:
        TST         R6,R6
        BT          ?step
        CMP/EQ      R0,R3
        BT          ?done
?step:
        ADD         #32,R0
?test:
        CMP/HI      R0,R2
        BT          ?entry
?done:
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
}

/* ocbi_low_entries over entries 256 to 511, outside index mode, stopping
 * after entry 383 with ram set */
/* @asm opcode: cache-line, OCBI @R1 at 0x8c15872e drops an entry's line */
#pragma inline_asm(ocbi_high_entries)
static void ocbi_high_entries(Uint32 first, Uint32 last, Uint32 ram)
{
        MOV.L       R11,@-R15
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV.L       #H'00002000,R0
        MOV.L       #H'F4000000,R8
        MOV.L       #H'1FFFFC00,R9
        MOV.L       #H'00001FFF,R10
        MOV.L       #H'00004000,R2
        MOV.L       #H'00002FE0,R3
        BRA         ?test
        NOP
?entry:
        MOV         R0,R1
        OR          R8,R1
        MOV.L       @R1,R1
        AND         R9,R1
        OR          R0,R1
        CMP/HI      R1,R4
        BT          ?next
        CMP/HI      R5,R1
        BT          ?next
        OCBI        @R1
?next:
        TST         R6,R6
        BT          ?step
        CMP/EQ      R0,R3
        BT          ?done
?step:
        ADD         #32,R0
?test:
        CMP/HI      R0,R2
        BT          ?entry
?done:
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        MOV.L       @R15+,R11
}

#ifdef NONMATCHING
/* this side keeps the shipped walk and both its bugs. the shipped body,
 * the inline ocbi_high_entries_ix below, reads R12, which SHC's code for
 * ADXF_Ocbi leaves holding last. this side reads last. like purge_entries,
 * the function runs in P2 and calls nothing */
static __attribute__((noipa)) void ocbi_high_entries_ix(Uint32 first,
                                                        Uint32 last,
                                                        Uint32 ram)
{
    Uint32 i;
    Uint32 line;

    for (i = OC_HALF; i < OC_ENTRIES; i++) {
        line = (OC_ENTRY(i) & TAG_MASK) | (i * LINE_SIZE) | last;
        if (line >= first && line <= last) {
            __asm__ volatile("ocbi @%0" : : "r"(line) : "memory");
        }
        if (ram && i == OC_HALF + OC_CACHED - 1) {
            break;
        }
    }
}
#else
/* ocbi_high_entries for index mode. there an entry's half stands for
 * address bit 25, not bit 13 */
/* @bug logic: drops no line whose address has bit 13 clear. OR R0,R1 at
 * 0x8c1586ac ORs in each entry's whole offset, as ocbi_high_entries does.
 * every offset in the second half has bit 13 set, which in index mode
 * stands for address bit 25. each address the walk tests then has bit 13
 * set */
/* @bug uninitialized: drops no line but the one at last. OR R12,R1 at
 * 0x8c1586b4 reads a register the body never loads. SHC's code for ADXF_Ocbi
 * holds last there. each address the walk tests then has last's bits set, so
 * only last itself lies in range */
/* @asm opcode: cache-line, OCBI @R1 at 0x8c1586be drops an entry's line */
#pragma inline_asm(ocbi_high_entries_ix)
static void ocbi_high_entries_ix(Uint32 first, Uint32 last, Uint32 ram)
{
        MOV.L       R11,@-R15
        MOV.L       R10,@-R15
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        MOV.L       #H'00002000,R0
        MOV.L       #H'F4000000,R8
        MOV.L       #H'1FFFFC00,R9
        MOV.L       #H'00001FFF,R10
        MOV.L       #H'00004000,R2
        MOV.L       #H'00002FE0,R3
        BRA         ?test
        NOP
?entry:
        MOV         R0,R1
        OR          R8,R1
        MOV.L       @R1,R1
        AND         R9,R1
        OR          R0,R1
        MOV         R0,R11
        AND         R10,R11
        OR          R11,R1
        OR          R12,R1
        CMP/HI      R1,R4
        BT          ?next
        CMP/HI      R5,R1
        BT          ?next
        OCBI        @R1
?next:
        TST         R6,R6
        BT          ?step
        CMP/EQ      R0,R3
        BT          ?done
?step:
        ADD         #32,R0
?test:
        CMP/HI      R0,R2
        BT          ?entry
?done:
        MOV.L       @R15+,R8
        MOV.L       @R15+,R9
        MOV.L       @R15+,R10
        MOV.L       @R15+,R11
}
#endif

#ifdef NONMATCHING
/* always inlined, so that the NOPs run in purge_entries, in P2. a call
 * would run them at their cached address */
static inline __attribute__((always_inline)) void eight_nops(void)
{
    __asm__ volatile(".rept 8\n\tnop\n\t.endr");
}
#else
/* eight instructions adxf_purge_all runs in P2 before it returns to cached
 * code. the SH-4 asks for eight between a write to CCR and a branch to
 * cached code. adxf_purge_all keeps that gap after the array's writes */
/* @asm open: every instruction is one SHC writes for C */
#pragma inline_asm(eight_nops)
static void eight_nops(void)
{
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
}
#endif
#else
/* without DREAMCAST, the inline bodies above aren't compiled, and these
 * declarations stand in for them */
/* NOLINTBEGIN(clang-diagnostic-undefined-internal) */
static void enter_p2(void);
static void ocbi_lines(Uint32 first, Uint32 nbyte);
static void ocbi_low_entries(Uint32 first, Uint32 last, Uint32 ram);
static void ocbi_high_entries(Uint32 first, Uint32 last, Uint32 ram);
static void ocbi_high_entries_ix(Uint32 first, Uint32 last, Uint32 ram);
static void eight_nops(void);
/* NOLINTEND(clang-diagnostic-undefined-internal) */
#endif
/* clang-format on */

#ifdef NONMATCHING
/* the stores and NOPs adxf_purge_all runs in P2, for GCC's side to call at
 * its P2 address. noipa keeps GCC from splitting part of it out into a
 * function that would run cached */
static __attribute__((noipa)) void purge_entries(void)
{
    Uint32 i;

    if (CCR_ORA(CCR)) {
        for (i = 0; i < OC_CACHED; i++) {
            OC_ENTRY(i) = 0;
        }
        for (i = OC_HALF; i < OC_HALF + OC_CACHED; i++) {
            OC_ENTRY(i) = 0;
        }
    } else {
        for (i = 0; i < OC_ENTRIES; i++) {
            OC_ENTRY(i) = 0;
        }
    }
    eight_nops();
}
#endif

/* purge the operand cache. in RAM mode, the entries the cache keeps as RAM
 * are left alone. the call masks every interrupt while it writes the address
 * array */
void adxf_purge_all(void)
{
    Uint32 i;
    /* @hack type: mask as a Uint16, where the int get_imask returns would
     * do */
    Uint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* GCC can't move the rest of this function to P2, so it calls
     * purge_entries there. this side restores the interrupt mask after
     * purge_entries' eight NOPs. the shipped code restores it before them */
    ((void (*)(void))P2(purge_entries))();
    set_imask(mask);
#else
    enter_p2();
    /* @hack redundant: the comparison with 1 of a bit read as 0 or 1 */
    if (CCR_ORA(CCR) == 1) {
        for (i = 0; i < OC_CACHED; i++) {
            OC_ENTRY(i) = 0;
        }
        for (i = OC_HALF; i < OC_HALF + OC_CACHED; i++) {
            OC_ENTRY(i) = 0;
        }
    } else {
        for (i = 0; i < OC_ENTRIES; i++) {
            OC_ENTRY(i) = 0;
        }
    }
    set_imask(mask);
    eight_nops();
#endif
}

/* purge the operand cache and write ccr to CCR. then set CCR_ICI and
 * CCR_OCI, which drop every line of both caches. code that writes CCR must
 * run in P2. the call doesn't enter P2, so its caller must call it at its
 * address there. nothing in the game calls it */
void ADXF_InitCache_P2_(Uint32 ccr)
{
    adxf_purge_all();
    /* @hack spelling: ccr written to CCR and read back, where ccr in the next
     * write would do */
    CCR = ccr;
    /* @hack redundant: the clearing of CCR_ICI and CCR_OCI before the ORs
     * that set them */
    CCR = (CCR & ~CCR_ICI) | CCR_ICI;
    CCR = (CCR & ~CCR_OCI) | CCR_OCI;
}

void ADXF_Ocbi(void *buf, Sint32 bsize)
{
    Uint32 first;
    Uint32 last;
    Uint32 ram;
    Uint32 ix;
    Uint32 ccr;
    /* @hack type: mask as a Uint16, where the int get_imask returns would
     * do */
    Uint16 mask;

    if (bsize == 0) {
        return;
    }
    first = (Uint32)buf & PHYSICAL_LINE_MASK;
    if ((Uint32)bsize < ADXF_OCBI_WALK_MIN) {
        ocbi_lines(first, ((Uint32)buf & (LINE_SIZE - 1)) + bsize);
    } else {
        mask = get_imask();
        set_imask(IMASK_ALL);
        enter_p2();
        last = ((Uint32)buf + bsize - 1) & PHYSICAL_LINE_MASK;
        ccr = CCR;
        ram = CCR_ORA(ccr);
        ix = CCR_OIX(ccr);
        UNCACHED(ocbi_low_entries)(first, last, ram);
        if (ix) {
            UNCACHED(ocbi_high_entries_ix)(first, last, ram);
        } else {
            UNCACHED(ocbi_high_entries)(first, last, ram);
        }
        set_imask(mask);
    }
}
