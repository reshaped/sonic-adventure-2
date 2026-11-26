/* the SH-4's caches: CCR, and the operand cache's lines over a range or
 * whole. the file also hands out the pages the operand cache keeps as RAM.
 * a line written back goes to memory if the cache changed it, and stays
 * cached. a line dropped is invalidated, and nothing is written back. a line
 * purged is written back, then dropped */
#include <machine.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* CCR, the cache control register */
#ifdef NONMATCHING
/* GCC would merge syCacheInit_P2_'s three stores to a plain CCR into one.
 * SHC's code makes all three, as GCC's does through a volatile CCR */
#define CCR (*(volatile Uint32 *)0xff00001c)
#else
/* @hack type: CCR as a plain Uint32, where a volatile one would do */
#define CCR (*(Uint32 *)0xff00001c)
#endif
/* a 1 written to CCR_OCI drops every line of the operand cache. a 1 written
 * to CCR_ICI drops every line of the instruction cache */
#define CCR_OCI 0x008
#define CCR_ICI 0x800
/* CCR's ORA and OIX bits in ccr, a value read from CCR, as 0 or 1. ORA
 * selects RAM mode, in which the operand cache keeps some of its entries as
 * RAM. OIX selects index mode, in which address bit 25 picks an entry's
 * half in place of bit 13 */
#define CCR_ORA_SHIFT 5
#define CCR_OIX_SHIFT 7
#define CCR_ORA(ccr) (((ccr) >> CCR_ORA_SHIFT) & 1)
#define CCR_OIX(ccr) (((ccr) >> CCR_OIX_SHIFT) & 1)
#define LINE_SHIFT 5
#define LINE_SIZE (1 << LINE_SHIFT)
/* entry i's word in the operand cache's address array: its tag and state.
 * the tag holds bits 10 to 28 of the line's address. a store of 0 purges
 * the entry */
#define OC_ADDRESS_ARRAY 0xf4000000
#define OC_ENTRY(i) (*(Uint32 *)(OC_ADDRESS_ARRAY | ((i) << LINE_SHIFT)))
/* the array's OC_ENTRIES entries form two halves of OC_HALF. in RAM mode,
 * only the first OC_CACHED entries of each half cache lines */
#define OC_ENTRIES 512
#define OC_HALF (OC_ENTRIES / 2)
#define OC_CACHED 128
/* LINE_MASK gives an address's line with bits 29 and 30 clear. a P2
 * address then gives its P1 twin. PHYSICAL_LINE_MASK gives the line's 29-bit
 * physical address, the form in which the entry walks compare lines */
#define LINE_MASK 0x9fffffe0
#define PHYSICAL_LINE_MASK 0x1fffffe0
/* the operand cache's RAM, as the CPU addresses it, in RAM_PAGES pages of
 * 1 << PAGE_SHIFT bytes */
#define CACHE_RAM 0x7c000000
#define RAM_PAGES 8
#define PAGE_SHIFT 10
#define CACHE_RAM_SIZE (RAM_PAGES << PAGE_SHIFT)

/* UNCACHED(walk) is a walk over the cache's entries, run in P2 as the
 * shipped code runs it. P2 is the uncached mirror of memory */
#ifdef NONMATCHING
/* GCC calls the walk as a function, so its build calls the walk's P2
 * address */
#define P2(address) ((Uint32)(address) | P2_BASE)
#define UNCACHED(walk) ((void (*)(Uint32, Uint32, Bool))P2(walk))
#else
/* SHC expands the walk inline, in code that enter_p2 has moved to P2 */
#define UNCACHED(walk) walk
#endif

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
char *sygCacheBuild = "\nsyCache Ver 2.01 Build:Mar 01 2000 22:08:51\n";
const Uint32 sygCacheVer = 0x00010002;
/* prefetch size / LINE_SIZE lines, rounded up, from address's line on. a
 * size of 0 counts as 2^32 lines */
void syCachePREF_s(void *address, Uint32 size);
/* NOLINTEND(misc-use-internal-linkage) */

/* the count of pages handed out, from page 0 up */
static Uint32 cache_ram_pages;
/* each page's byte offset in the RAM, which syCacheAllocCacheRAM records for
 * each page a request reaches */
static Uint16 cache_ram_page_offset[RAM_PAGES];

/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's pref_lines is syCachePREF_s, whose C prefetches the same lines as
 * the inline body SHC expands */
#define pref_lines syCachePREF_s
#else
/* prefetch the lines over size bytes from address, a line's address. a
 * size of 0 counts as 2^32 lines */
/* @asm open: every instruction is one SHC writes for C */
#pragma inline_asm(pref_lines)
static void pref_lines(void *address, Uint32 size)
{
        ADD         #31,R5
        SHLR2       R5
        SHLR2       R5
        SHLR        R5
?line:
        PREF        @R4
        DT          R5
        BF/S        ?line
        ADD         #32,R4
}
#endif

#ifdef NONMATCHING
/* a call can't move its caller to P2, so GCC's enter_p2 does nothing. the
 * callers call their walks through UNCACHED instead */
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

/* drop the lines over size bytes from address, a line's address. a size of
 * 0 counts as 2^32 lines */
/* @asm opcode: cache-line, OCBI @R4 at 0x8c10f440 drops one line */
#pragma inline_asm(ocbi_lines)
static void ocbi_lines(Uint32 address, Uint32 size)
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

/* drop each line from first to last that entries 0 to 255 hold. an entry's
 * line is its tag ORed with its offset in the array. first is a line as
 * LINE_MASK gives it, and last one as PHYSICAL_LINE_MASK does. the walk
 * compares lines with first's bit 31 clear, and drops a line with that bit
 * put back. with ram set, the walk stops after entry 127 */
/* @asm opcode: cache-line, OCBI @R1 at 0x8c10f500 drops an entry's line */
#pragma inline_asm(ocbi_low_entries)
static void ocbi_low_entries(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
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
        OR          R7,R1
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

/* ocbi_low_entries over entries 256 to 511 in index mode, stopping after
 * entry 383 with ram set. index mode picks an entry's half by address bit
 * 25, which the tag already holds. so an entry's line takes only the low 13
 * bits of its offset in the array */
/* @asm opcode: cache-line, OCBI @R1 at 0x8c10f58c drops an entry's line */
#pragma inline_asm(ocbi_high_entries_ix)
static void ocbi_high_entries_ix(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
        BRA         ?test
        NOP
?entry:
        MOV         R0,R1
        OR          R8,R1
        MOV.L       @R1,R1
        AND         R9,R1
        MOV         R0,R11
        AND         R10,R11
        OR          R11,R1
        CMP/HI      R1,R4
        BT          ?next
        CMP/HI      R5,R1
        BT          ?next
        OR          R7,R1
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

/* ocbi_low_entries over entries 256 to 511, outside index mode, stopping
 * after entry 383 with ram set */
/* @asm opcode: cache-line, OCBI @R1 at 0x8c10f60c drops an entry's line */
#pragma inline_asm(ocbi_high_entries)
static void ocbi_high_entries(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
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
        OR          R7,R1
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

/* ocbi_lines, purging each line by OCBP */
/* @asm opcode: cache-line, OCBP @R4 at 0x8c10f690 purges one line */
#pragma inline_asm(ocbp_lines)
static void ocbp_lines(Uint32 address, Uint32 size)
{
        ADD         #31,R5
        SHLR2       R5
        SHLR2       R5
        SHLR        R5
?line:
        OCBP        @R4
        DT          R5
        BF/S        ?line
        ADD         #32,R4
}

/* ocbi_low_entries, purging each line by OCBP */
/* @asm opcode: cache-line, OCBP @R1 at 0x8c10f750 purges an entry's line */
#pragma inline_asm(ocbp_low_entries)
static void ocbp_low_entries(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
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
        OR          R7,R1
        OCBP        @R1
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

/* ocbi_high_entries_ix, purging each line by OCBP */
/* @asm opcode: cache-line, OCBP @R1 at 0x8c10f7dc purges an entry's line */
#pragma inline_asm(ocbp_high_entries_ix)
static void ocbp_high_entries_ix(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
        BRA         ?test
        NOP
?entry:
        MOV         R0,R1
        OR          R8,R1
        MOV.L       @R1,R1
        AND         R9,R1
        MOV         R0,R11
        AND         R10,R11
        OR          R11,R1
        CMP/HI      R1,R4
        BT          ?next
        CMP/HI      R5,R1
        BT          ?next
        OR          R7,R1
        OCBP        @R1
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

/* ocbi_high_entries, purging each line by OCBP */
/* @asm opcode: cache-line, OCBP @R1 at 0x8c10f85c purges an entry's line */
#pragma inline_asm(ocbp_high_entries)
static void ocbp_high_entries(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
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
        OR          R7,R1
        OCBP        @R1
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

/* ocbi_lines, writing each line back by OCBWB */
/* @asm opcode: cache-line, OCBWB @R4 at 0x8c10f8e0 writes back one line */
#pragma inline_asm(ocbwb_lines)
static void ocbwb_lines(Uint32 address, Uint32 size)
{
        ADD         #31,R5
        SHLR2       R5
        SHLR2       R5
        SHLR        R5
?line:
        OCBWB       @R4
        DT          R5
        BF/S        ?line
        ADD         #32,R4
}

/* ocbi_low_entries, writing each line back by OCBWB */
/* @asm opcode: cache-line, OCBWB @R1 at 0x8c10f9a0 writes back an entry's
 * line */
#pragma inline_asm(ocbwb_low_entries)
static void ocbwb_low_entries(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
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
        OR          R7,R1
        OCBWB       @R1
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

/* ocbi_high_entries_ix, writing each line back by OCBWB */
/* @asm opcode: cache-line, OCBWB @R1 at 0x8c10fa2c writes back an entry's
 * line */
#pragma inline_asm(ocbwb_high_entries_ix)
static void ocbwb_high_entries_ix(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
        BRA         ?test
        NOP
?entry:
        MOV         R0,R1
        OR          R8,R1
        MOV.L       @R1,R1
        AND         R9,R1
        MOV         R0,R11
        AND         R10,R11
        OR          R11,R1
        CMP/HI      R1,R4
        BT          ?next
        CMP/HI      R5,R1
        BT          ?next
        OR          R7,R1
        OCBWB       @R1
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

/* ocbi_high_entries, writing each line back by OCBWB */
/* @asm opcode: cache-line, OCBWB @R1 at 0x8c10faac writes back an entry's
 * line */
#pragma inline_asm(ocbwb_high_entries)
static void ocbwb_high_entries(Uint32 first, Uint32 last, Bool ram)
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
        MOV.L       #H'80000000,R7
        AND         R4,R7
        NOT         R7,R7
        AND         R7,R4
        NOT         R7,R7
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
        OR          R7,R1
        OCBWB       @R1
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
/* always inlined, so that the NOPs run in purge_entries, in P2. a call
 * would run them at their cached address */
static inline __attribute__((always_inline)) void eight_nops(void)
{
    __asm__ volatile(".rept 8\n\tnop\n\t.endr");
}
#else
/* eight instructions between syCachePurgeAll's last store to the address
 * array, in P2, and its return to cached code */
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
static void pref_lines(void *address, Uint32 size);
static void enter_p2(void);
static void ocbi_lines(Uint32 address, Uint32 size);
static void ocbi_low_entries(Uint32 first, Uint32 last, Bool ram);
static void ocbi_high_entries_ix(Uint32 first, Uint32 last, Bool ram);
static void ocbi_high_entries(Uint32 first, Uint32 last, Bool ram);
static void ocbp_lines(Uint32 address, Uint32 size);
static void ocbp_low_entries(Uint32 first, Uint32 last, Bool ram);
static void ocbp_high_entries_ix(Uint32 first, Uint32 last, Bool ram);
static void ocbp_high_entries(Uint32 first, Uint32 last, Bool ram);
static void ocbwb_lines(Uint32 address, Uint32 size);
static void ocbwb_low_entries(Uint32 first, Uint32 last, Bool ram);
static void ocbwb_high_entries_ix(Uint32 first, Uint32 last, Bool ram);
static void ocbwb_high_entries(Uint32 first, Uint32 last, Bool ram);
static void eight_nops(void);
/* NOLINTEND(clang-diagnostic-undefined-internal) */
#endif
/* clang-format on */

void syCachePREF_s(void *address, Uint32 size)
{
    /* @hack spelling: size rounded to lines in two statements, where one
     * would do */
    size += LINE_SIZE - 1;
    size >>= LINE_SHIFT;
    do {
        prefetch(address);
        address = (char *)address + LINE_SIZE;
    } while (--size);
}

void syCacheInit_P2_(SY_CACHE_FORM form)
{
    /* @hack type: mask as Sint16, where Uint16, as in the other calls, would
     * do */
    Sint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    syCachePurgeAll();
    /* @hack spelling: form written to CCR and read back, where form in the
     * next write would do */
    CCR = form;
    /* @hack redundant: the clearing of CCR_ICI and CCR_OCI before the ORs
     * that set them */
    CCR = (CCR & ~CCR_ICI) | CCR_ICI;
    CCR = (CCR & ~CCR_OCI) | CCR_OCI;
    set_imask(mask);
}

void syCacheFinish_P2_(void)
{
    /* @hack spelling: off, a local for the 0 written, where 0 would do */
    Uint32 off = 0;
    /* @hack type: mask as Sint16, where Uint16, as in the other calls, would
     * do */
    Sint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    syCachePurgeAll();
    CCR = off;
    set_imask(mask);
}

SY_CACHE_FORM syCacheGetForm_P2_(void)
{
    return CCR;
}

void syCacheSetForm_P2_(SY_CACHE_FORM form)
{
    /* @hack type: mask as Sint16, where Uint16, as in the other calls, would
     * do */
    Sint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    syCachePurgeAll();
    /* @hack spelling: form written to CCR and read back, where form in the
     * next write would do */
    CCR = form;
    /* @hack redundant: the clearing of CCR_ICI and CCR_OCI before the ORs
     * that set them */
    CCR = (CCR & ~CCR_ICI) | CCR_ICI;
    CCR = (CCR & ~CCR_OCI) | CCR_OCI;
    set_imask(mask);
}

void syCacheICI_P2_(void)
{
    CCR |= CCR_ICI;
}

void syCacheOCI_P2_(void)
{
    CCR |= CCR_OCI;
}

void syCacheOCBI(void *address, Uint32 size)
{
    Uint32 first;
    Uint32 last;
    Bool ram;
    Bool ix;
    Uint16 mask;

    if (size == 0) {
        return;
    }
    first = (Uint32)address & LINE_MASK;
    if (size < OCBI_WALK_MIN) {
        ocbi_lines(first, ((Uint32)address & (LINE_SIZE - 1)) + size);
    } else {
        mask = get_imask();
        set_imask(IMASK_ALL);
        enter_p2();
        last = ((Uint32)address + size - 1) & PHYSICAL_LINE_MASK;
        ram = CCR_ORA(CCR);
        ix = CCR_OIX(CCR);
        UNCACHED(ocbi_low_entries)(first, last, ram);
        if (ix) {
            UNCACHED(ocbi_high_entries_ix)(first, last, ram);
        } else {
            UNCACHED(ocbi_high_entries)(first, last, ram);
        }
        set_imask(mask);
    }
}

void syCacheOCBP(void *address, Uint32 size)
{
    Uint32 first;
    Uint32 last;
    Bool ram;
    Bool ix;
    Uint16 mask;

    if (size == 0) {
        return;
    }
    first = (Uint32)address & LINE_MASK;
    if (size < WRITE_BACK_WALK_MIN) {
        ocbp_lines(first, ((Uint32)address & (LINE_SIZE - 1)) + size);
    } else {
        mask = get_imask();
        set_imask(IMASK_ALL);
        enter_p2();
        last = ((Uint32)address + size - 1) & PHYSICAL_LINE_MASK;
        ram = CCR_ORA(CCR);
        ix = CCR_OIX(CCR);
        UNCACHED(ocbp_low_entries)(first, last, ram);
        if (ix) {
            UNCACHED(ocbp_high_entries_ix)(first, last, ram);
        } else {
            UNCACHED(ocbp_high_entries)(first, last, ram);
        }
        set_imask(mask);
    }
}

void syCacheOCWB(void *address, Uint32 size)
{
    Uint32 first;
    Uint32 last;
    Bool ram;
    Bool ix;
    Uint16 mask;

    if (size == 0) {
        return;
    }
    first = (Uint32)address & LINE_MASK;
    if (size < WRITE_BACK_WALK_MIN) {
        ocbwb_lines(first, ((Uint32)address & (LINE_SIZE - 1)) + size);
    } else {
        mask = get_imask();
        set_imask(IMASK_ALL);
        enter_p2();
        last = ((Uint32)address + size - 1) & PHYSICAL_LINE_MASK;
        ram = CCR_ORA(CCR);
        ix = CCR_OIX(CCR);
        UNCACHED(ocbwb_low_entries)(first, last, ram);
        if (ix) {
            UNCACHED(ocbwb_high_entries_ix)(first, last, ram);
        } else {
            UNCACHED(ocbwb_high_entries)(first, last, ram);
        }
        set_imask(mask);
    }
}

void syCachePREF(void *address, Uint32 size)
{
    pref_lines((void *)((Uint32)address & LINE_MASK),
               ((Uint32)address & (LINE_SIZE - 1)) + size);
}

/* clang-format off */
#ifdef DREAMCAST
/* @asm opcode: movca, MOVCA.L R0,@R4 at 0x8c10fb16 claims address's line */
#pragma inline_asm(syCacheMOVCAL)
void syCacheMOVCAL(void *address, Uint32 data)
{
        MOV         R5,R0
        MOVCA.L     R0,@R4
        RTS
        NOP
}
#endif
/* clang-format on */

#ifdef NONMATCHING
/* the stores and NOPs syCachePurgeAll runs in P2, for GCC's side to call
 * at its P2 address. noipa keeps GCC from splitting part of it out into a
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

void syCachePurgeAll(void)
{
    Uint32 i;
    Uint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* GCC can't move the rest of this function to P2, so it calls
     * purge_entries there */
    ((void (*)(void))P2(purge_entries))();
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
    eight_nops();
#endif
    set_imask(mask);
}

void syCacheAllocCacheRAM(void **address, Uint32 pages)
{
    Uint32 page;

    for (page = cache_ram_pages;
         page < cache_ram_pages + pages && page < RAM_PAGES; page++) {
        cache_ram_page_offset[page] = page << PAGE_SHIFT;
    }
    /* @bug logic: refuses every request that reaches the last page, so no
     * request gets it. page ends at RAM_PAGES for a request that ends on the
     * last page, as it does for one past it (CMP/EQ #8,R0 at 0x8c10fbf6) */
    /* @hack spelling: the goto past the stores, where an if around them would
     * do */
    if (page == RAM_PAGES) {
        goto full;
    }
    *address = (void *)((cache_ram_pages << PAGE_SHIFT) | CACHE_RAM);
    cache_ram_pages += pages;
full:;
}

void syCacheFreeCacheRAM(void *address)
{
    Uint32 page;

    /* @bug logic: a free never matches the page it names, since the loop
     * compares each page's byte offset with address's page number (CMP/EQ
     * R4,R3 at 0x8c10fc2a). each page handed out from page 1 up holds an
     * offset of 1 KB or more, above every page number. page cache_ram_pages,
     * the first not handed out, matches while its offset is 0 and address
     * lies in page 0. that match takes one page from the count */
    for (page = cache_ram_pages; page > 0; page--) {
        if (cache_ram_page_offset[page] ==
            (((Uint32)address & (CACHE_RAM_SIZE - 1)) >> PAGE_SHIFT)) {
            cache_ram_pages--;
        }
    }
}
