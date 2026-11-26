#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <sy_g2_work.h>

/* the bus's DMA protection register, through P2. its word holds G2_KEY
 * from bit PROTECT_KEY_SHIFT. the first megabyte a transfer may reach starts
 * at bit PROTECT_FIRST_SHIFT, and the last at bit 0 */
#define G2_DMA_PROTECT (*(volatile Uint32 *)0xa05f78bc)
#define G2_KEY 0x4659
#define PROTECT_KEY_SHIFT 16
#define PROTECT_FIRST_SHIFT 8
/* the number of the megabyte that holds address: its MEGABYTE_BITS, shifted
 * down MEGABYTE_SHIFT bits */
#define MEGABYTE_BITS 0x07f00000
#define MEGABYTE_SHIFT 20
#define MEGABYTE(address) (((address) & MEGABYTE_BITS) >> MEGABYTE_SHIFT)

/* the member exports this name, though no header of the SDK declares it. it
 * holds the word syG2DmaSetProtect last gave the DMA protection register, or
 * 0 before its first call */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Uint32 syG2DmaProtect = 0;
/* NOLINTEND(misc-use-internal-linkage) */

SYE_G2_ERR syG2DmaSetProtect(Uint32 first, Uint32 last)
{
    Sint32 mask;
    /* @hack spelling: first_mb, last_mb and protect, locals each read once,
     * where one expression in the store would do */
    Uint32 first_mb;
    Uint32 last_mb;
    Uint32 protect;

    mask = get_imask();
    set_imask(IMASK_ALL);
    first_mb = MEGABYTE(first);
    last_mb = MEGABYTE(last);
    protect = (first_mb << PROTECT_FIRST_SHIFT) | last_mb;
    /* @hack redundant: a goto to the label that follows it */
    goto set;
set:
    /* @hack spelling: the register and syG2DmaProtect set in one statement,
     * where a statement for each would do */
    syG2DmaProtect = G2_DMA_PROTECT =
        protect | ((Uint32)G2_KEY << PROTECT_KEY_SHIFT);
    set_imask(mask);
    return SYE_G2_ERR_NOTHING;
}
