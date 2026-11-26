/* the MMU's unified TLB, an entry at a time */
#include <sg_xpt.h>
#include <shinobi.h>

/* MMUCR, the MMU control register, by its SH-4 name. AT turns address
 * translation on, and TI invalidates every UTLB entry. URB, the replace
 * boundary, is the entry syMmuAddEntry's search starts at */
/* @hack type: MMUCR not volatile, where a volatile register would do */
#define MMUCR (*(Uint32 *)0xff000010)
#define MMUCR_AT 0x1
#define MMUCR_TI 0x4
#define MMUCR_URB_SHIFT 18
#define MMUCR_URB_MASK 63
#define MMUCR_START (MMUCR_AT | MMUCR_TI | (1 << MMUCR_URB_SHIFT))
/* the UTLB's entries, each a word of the address array and a word of the
 * data array */
#define UTLB_ENTRIES 64
#define UTLB_ADDRESS_ARRAY 0xf6000000
#define UTLB_DATA_ARRAY 0xf7000000
#define UTLB_ENTRY_SHIFT 8
/* @hack type: the UTLB's arrays not volatile, where volatile ones would
 * do */
#define UTLB_ADDRESS(entry)                                                    \
    (*(Uint32 *)(UTLB_ADDRESS_ARRAY + ((entry) << UTLB_ENTRY_SHIFT)))
#define UTLB_DATA(entry)                                                       \
    (*(Uint32 *)(UTLB_DATA_ARRAY + ((entry) << UTLB_ENTRY_SHIFT)))
/* the address array's V bit, which marks a valid entry, and its virtual page
 * number */
#define UTLB_ADDRESS_V 0x100
#define UTLB_ADDRESS_VPN 0xfffffc00
/* the data array's bits, by their SH-4 names. V marks the entry valid, and
 * SZ1 and SZ0 give its page's size. a PR of 01 lets privileged code alone
 * read and write the page. C marks the page cached, D written, SH shared and
 * WT write-through */
#define UTLB_DATA_V 0x100
#define UTLB_DATA_SZ1 0x080
#define UTLB_DATA_PR_PRIVILEGED 0x020
#define UTLB_DATA_SZ0 0x010
#define UTLB_DATA_C 0x008
#define UTLB_DATA_D 0x004
#define UTLB_DATA_SH 0x002
#define UTLB_DATA_WT 0x001
/* the data array's PPN, the physical page number, for each page size */
#define UTLB_DATA_PPN_1K 0x1ffffc00
#define UTLB_DATA_PPN_4K 0x1ffff000
#define UTLB_DATA_PPN_64K 0x1fff0000
#define UTLB_DATA_PPN_1M 0x1ff00000
/* the bits syMmuAddEntry gives every entry, and those syMmuCacheCtrl
 * replaces */
#define UTLB_DATA_FLAGS                                                        \
    (UTLB_DATA_V | UTLB_DATA_PR_PRIVILEGED | UTLB_DATA_SZ0 | UTLB_DATA_C |     \
     UTLB_DATA_D | UTLB_DATA_SH)
#define UTLB_CACHE_MODE (UTLB_DATA_C | UTLB_DATA_WT)

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const sygMmuBuild = "\nsyMmu Ver 1.50 Build:May 31 1999 16:25:13\n";
const Uint32 sygMmuVer = 0x501;
/* NOLINTEND(misc-use-internal-linkage) */

void syMmuInit(Uint8 urb)
{
    MMUCR = MMUCR_START;
}

SYMMU syMmuAddEntry(void *paddress, void *vaddress, SY_MMU_PAGESIZE size)
{
    Uint32 data;
    SYMMU entry;
    SYMMU result;
    Uint32 address;
    Uint32 ppn_mask;
    Sint32 size_bits;

    entry = (MMUCR >> MMUCR_URB_SHIFT) & MMUCR_URB_MASK;
    for (; entry < UTLB_ENTRIES; ++entry) {
        if ((UTLB_ADDRESS(entry) & UTLB_ADDRESS_V) != UTLB_ADDRESS_V) {
            break;
        }
    }
    if (entry == UTLB_ENTRIES) {
        result = MMU_NO_ENTRY;
    } else {
        address = ((Uint32)vaddress & UTLB_ADDRESS_VPN) | UTLB_ADDRESS_V;
        /* @hack spelling: size_bits set ahead of ppn_mask in the last two
         * cases, where the first two's order would do */
        switch (size) {
        case SYD_MMU_PAGESIZE_1K:
            ppn_mask = UTLB_DATA_PPN_1K;
            size_bits = 0;
            break;
        case SYD_MMU_PAGESIZE_4K:
            ppn_mask = UTLB_DATA_PPN_4K;
            size_bits = UTLB_DATA_SZ0;
            break;
        case SYD_MMU_PAGESIZE_64K:
            size_bits = UTLB_DATA_SZ1;
            ppn_mask = UTLB_DATA_PPN_64K;
            break;
        case SYD_MMU_PAGESIZE_1M:
            size_bits = UTLB_DATA_SZ1 | UTLB_DATA_SZ0;
            ppn_mask = UTLB_DATA_PPN_1M;
            break;
#ifdef NONMATCHING
        /* this side keeps the shipped result for a size none of the four:
         * ppn_mask and size_bits take address and size, which the image's
         * registers hold there */
        default:
            ppn_mask = address;
            size_bits = size;
            break;
#endif
        }
        /* @bug logic: SYD_MMU_PAGESIZE_1K maps a 4 KB page, and
         * SYD_MMU_PAGESIZE_64K a 1 MB one. UTLB_DATA_FLAGS holds
         * UTLB_DATA_SZ0, which OR R3,R4 at 0x8c15af10 sets whatever size_bits
         * says */
        data = ((Uint32)paddress & ppn_mask) | size_bits | UTLB_DATA_FLAGS;
        UTLB_ADDRESS(entry) = address;
        UTLB_DATA(entry) = data;
        result = entry;
    }
    return result;
}

void syMmuDeleteEntry(SYMMU mmuPageNum)
{
    Uint32 address;

    /* @hack redundant: the empty branch for an entry past the UTLB, where
     * the test for one inside it would do */
    if (mmuPageNum >= UTLB_ENTRIES) {
    } else {
        address = UTLB_ADDRESS(mmuPageNum) & ~UTLB_ADDRESS_V;
        UTLB_ADDRESS(mmuPageNum) = address;
        UTLB_DATA(mmuPageNum) = 0;
    }
}

void syMmuCacheCtrl(SYMMU mmuPageNum, SY_MMU_CACHE cacheCtrl)
{
    Uint32 data;

    if (mmuPageNum < UTLB_ENTRIES) {
        /* @hack spelling: the data word's address built in mmuPageNum, where
         * UTLB_DATA(mmuPageNum) would do */
        data = UTLB_DATA(mmuPageNum);
        mmuPageNum <<= UTLB_ENTRY_SHIFT;
        data &= ~UTLB_CACHE_MODE;
        mmuPageNum += UTLB_DATA_ARRAY;
        data |= cacheCtrl;
        *(Uint32 *)mmuPageNum = data;
    }
}
