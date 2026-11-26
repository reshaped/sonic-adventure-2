/* the CPU's exception and interrupt entry: syIntInit copies it from
 * syIntIsr.src to VBR. syIntEnableInt sets the interrupt mask and clears
 * SR.BL */
#include <machine.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sy_int.h>

/* CCR, the cache control register, and its bit that invalidates the
 * instruction cache */
/* @hack type: CCR as a plain Uint32, where a volatile one would do */
#define CCR (*(Uint32 *)0xff00001c)
#define CCR_ICI 0x800

/* the exception and interrupt codes syIntInit gives syIntBrkRteRte */
#define FIRST_CODE 0x40
#define CODE_END 0x820
#define CODE_STEP 0x20
/* the offsets from VBR of syIntInit's copies. the first holds the return
 * that leaves an interrupt. the others are the vectors the CPU enters for a
 * general exception, a TLB miss and an interrupt */
#define RTE_OFFSET 0x000
#define GENERAL_VECTOR 0x100
#define TLB_MISS_VECTOR 0x400
#define INTERRUPT_VECTOR 0x600

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *sygIntBuild = "\nsyInt Ver 2.02 Build:Jan 18 2001 22:55:12\n";
const Uint16 sygIntVer[2] = {2, 1};
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* write back and invalidate bytes / 32 + 1 operand cache lines from
 * address */
/* @asm opcode: cache-line, OCBP @R4 at 0x8c1353a0, 0x8c1353e8, 0x8c135430
 * and 0x8c135478, in syIntInit, writes back and invalidates a line */
#pragma inline_asm(purge_lines)
static void purge_lines(void *address, Sint32 bytes)
{
        SHLR2       R5
        SHLR2       R5
        SHLR        R5
        ADD         #1,R5
?LINE:
        OCBP        @R4
        DT          R5
        BF/S        ?LINE
        ADD         #32,R4
}

/* clear SR.BL. the body runs inside its caller, so it branches over its
 * own pool */
/* @asm open: every instruction is one SHC writes for C */
#pragma inline_asm(unblock_exceptions)
static void unblock_exceptions(void)
{
        STC         SR,R0
        MOV.L       ?BLOCK,R1
        AND         R1,R0
        LDC         R0,SR
        BRA         ?DONE
        NOP
        .ALIGN      4
?BLOCK:
        .DATA.L     H'EFFFFFFF
?DONE:
}

/* @asm epilogue: dead-return, SHC's unreachable RTS at 0x8c1352fc follows the
 * body's own return at 0x8c1352f2 */
#pragma inline_asm(syIntEnableInt)
void syIntEnableInt(Uint32 level)
{
        SHLL        R4
        SHLL        R4
        SHLL        R4
        SHLL        R4
        STC         SR,R0
        MOV.L       ?MASK,R1
        AND         R1,R0
        OR          R4,R0
        LDC         R0,SR
        RTS
        NOP
        .ALIGN      4
?MASK:
        .DATA.L     H'EFFFFF0F
}
#else
/* without DREAMCAST, the inline bodies above aren't compiled, and these
 * declarations stand in for them */
/* NOLINTBEGIN(clang-diagnostic-undefined-internal) */
static void purge_lines(void *address, Sint32 bytes);
static void unblock_exceptions(void);
/* NOLINTEND(clang-diagnostic-undefined-internal) */
#endif
/* clang-format on */

/* top and bottom are separate objects to C, so the walk from one to the
 * other is undefined. SHC compiles it as plain address arithmetic */
#define INSTALL(top, bottom, offset)                                           \
    {                                                                          \
        Uint32 *to = (Uint32 *)((char *)get_vbr() + (offset));                 \
        Uint32 *from = (Uint32 *)&(top);                                       \
        while (from <= (Uint32 *)&(bottom)) {                                  \
            *to++ = *from++;                                                   \
        }                                                                      \
        /* @bug logic: purges none of the copies but the one at VBR. offset    \
         * is added to a Uint32 pointer, so the copies at VBR + 0x100, 0x400   \
         * and 0x600 have VBR + 0x400, 0x1000 and 0x1800 purged in their       \
         * place (ADD R3,R4 at 0x8c1353c8, 0x8c135410 and 0x8c135458). the     \
         * copies can stay in the operand cache while the CPU fetches the      \
         * code at those vectors from memory */                                \
        purge_lines((Uint32 *)get_vbr() + (offset),                            \
                    (char *)to - ((char *)get_vbr() + (offset)));              \
    }

void syIntInit(SY_INT_MODE mode, void *stack)
{
    Sint32 mask = get_imask();
    Sint32 code;

    set_imask(IMASK_ALL);
    CCR |= CCR_ICI;
    if (mode == SYD_INT_MODE_INTSTACK) {
        syIntExpMastISR = syIntMasterISR_2;
        syIntMastISR = syIntMasterISR_2;
        syIntNestCount = 0;
        syIntIntStack = stack;
    } else {
        syIntExpMastISR = syIntMasterISR;
        syIntMastISR = syIntMasterISR;
        syIntNestCount = 0;
        syIntIntStack = NULL;
    }
    INSTALL(syIntISRRTE, syIntISRRTE_Bottom, RTE_OFFSET);
    INSTALL(syIntExpISRTop, syIntExpISRBottom, GENERAL_VECTOR);
    INSTALL(syIntExpISRTop, syIntExpISRBottom, TLB_MISS_VECTOR);
    INSTALL(syIntISRTop, syIntISRBottom, INTERRUPT_VECTOR);
    set_imask(mask);
    for (code = FIRST_CODE; code < CODE_END; code += CODE_STEP) {
        USER_VECT(code) = (Uint32)syIntBrkRteRte;
    }
    unblock_exceptions();
}
