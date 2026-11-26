#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

SDE_ERR sdSndMemReadUint32(Sint32 address, Uint32 *value)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (address & SD_SND_MEM_BYTE_BITS) {
        result = SDE_ERR_SND_MEM_ADR_ERR;
#ifdef NONMATCHING
        /* this side repairs the tag below: a negative address reads nothing,
         * as an address of SD_SND_MEM_SIZE or more does */
    } else if ((Uint32)address < SD_SND_MEM_SIZE) {
#else
        /* @bug memory: reads the bus below sound memory for a negative
         * address, which the signed test below lets through (CMP/GE R2,R14 at
         * 0x8c1543bc) */
    } else if (address < SD_SND_MEM_SIZE) {
#endif
        syG2Read((void *)(SD_SND_MEM_BASE + address), value,
                 SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
                 SYE_G2_ADR_MODE_FIX);
    }
    return result;
}
