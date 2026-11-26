/* the store queues' address translation */
#include <sg_xpt.h>
#include <shinobi.h>

/* MMUCR, the MMU control register, and its bit that turns address
 * translation on */
#define MMUCR (*(volatile Uint32 *)0xff000010)
#define MMUCR_AT 1

/* the areas the pages map: main memory, then the TA FIFO's polygon path and
 * its YUV converter's. each SQ_ address is the store queue address its
 * area's first page lies at */
#define RAM 0x0c000000
#define RAM_END 0x0d000000
#define FIFO 0x10000000
#define FIFO_END 0x10800000
#define YUV FIFO_END
#define SQ_RAM 0xe0000000
#define SQ_FIFO 0xe1000000
#define SQ_YUV 0xe1800000
#define PAGE 0x100000
#define RAM_PAGES ((RAM_END - RAM) / PAGE)
#define FIFO_PAGES ((FIFO_END - FIFO) / PAGE)
#define YUV_PAGES 1

/* AREA gives an address's 16 MB area, the top 5 bits of its physical
 * address. AREA_OFFSET gives its offset in the area */
#define AREA_SHIFT 24
#define AREA_MASK 0x1f
#define AREA(address) (((Uint32)(address) >> AREA_SHIFT) & AREA_MASK)
#define AREA_OFFSET(address)                                                   \
    (((Uint32)(address) << (32 - AREA_SHIFT)) >> (32 - AREA_SHIFT))
/* main memory's start, through P1 */
#define RAM_P1 0x8c000000

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const sygSqBuild = "\nsySq Ver 1.50 Build:May 31 1999 16:26:00\n";
const Uint32 sygSqVer = 0x501;
/* the UTLB entry number of each page of each area */
SYMMU sygSqMmuWorkRAMEntry[RAM_PAGES];
SYMMU sygSqMmuTAFIFO_POLYEntry[FIFO_PAGES];
SYMMU sygSqMmuTAFIFO_YUVEntry[YUV_PAGES];
/* NOLINTEND(misc-use-internal-linkage) */

void sySqInit(void)
{
    Uint32 area;
    Uint32 sq;
    Uint32 offset;
    Uint32 i;

    if ((MMUCR & MMUCR_AT) == MMUCR_AT) {
        /* @hack spelling: each loop's area, queue address and offset, where
         * page i's addresses counted from i would do */
        for (area = RAM, sq = SQ_RAM, offset = 0, i = 0;
             area + offset < RAM_END; offset += PAGE, i++) {
            sygSqMmuWorkRAMEntry[i] =
                syMmuAddEntry((void *)(area + offset), (void *)(sq + offset),
                              SYD_MMU_PAGESIZE_1M);
        }
        for (area = FIFO, sq = SQ_FIFO, offset = 0, i = 0;
             area + offset < FIFO_END; offset += PAGE, i++) {
            sygSqMmuTAFIFO_POLYEntry[i] =
                syMmuAddEntry((void *)(area + offset), (void *)(sq + offset),
                              SYD_MMU_PAGESIZE_1M);
        }
        sygSqMmuTAFIFO_YUVEntry[0] =
            syMmuAddEntry((void *)YUV, (void *)SQ_YUV, SYD_MMU_PAGESIZE_1M);
    }
}

void sySqFinish(void)
{
    Uint32 i;

    for (i = 0; i < RAM_PAGES; i++) {
        syMmuDeleteEntry(sygSqMmuWorkRAMEntry[i]);
    }
    for (i = 0; i < FIFO_PAGES; i++) {
        syMmuDeleteEntry(sygSqMmuTAFIFO_POLYEntry[i]);
    }
    /* @hack spelling: the loop of one pass over the YUV table, where one
     * call would do */
    for (i = 0; i < YUV_PAGES; i++) {
        syMmuDeleteEntry(sygSqMmuTAFIFO_YUVEntry[i]);
    }
}

void *sySqStart(void *dstArray)
{
    Uint32 offset;

    /* @hack spelling: offset in each branch, where once ahead of the test
     * would do */
    /* @hack spelling: the result built in dstArray, where each branch could
     * return it */
    if (AREA(dstArray) < AREA(RAM_END)) {
        offset = AREA_OFFSET(dstArray);
        dstArray = (void *)SQ_RAM;
    } else {
        offset = AREA_OFFSET(dstArray);
        dstArray = (void *)SQ_FIFO;
    }
    dstArray = (void *)((Uint32)dstArray | offset);
    return dstArray;
}

void *sySqEnd(void *sqArray)
{
    Uint32 offset = AREA_OFFSET(sqArray);

    /* @hack spelling: the result built in sqArray, where each branch could
     * return it */
    if ((Uint32)sqArray < SQ_FIFO) {
        sqArray = (void *)RAM_P1;
    } else {
        sqArray = (void *)FIFO;
    }
    sqArray = (void *)((Uint32)sqArray | offset);
    return sqArray;
}
