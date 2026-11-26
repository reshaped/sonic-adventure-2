#include <sd_work.h>
#include <sg_sd.h>
#include <sg_syg2.h>
#include <sg_xpt.h>

SDE_ERR sdSndMemWriteUint32(Sint32 address, Uint32 value)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (address & SD_SND_MEM_BYTE_BITS) {
        result = SDE_ERR_SND_MEM_ADR_ERR;
#ifdef NONMATCHING
        /* this side repairs the tag below: a negative address writes nothing,
         * as an address of SD_SND_MEM_SIZE or more does */
    } else if ((Uint32)address < SD_SND_MEM_SIZE) {
#else
        /* @bug memory: writes the bus below sound memory for a negative
         * address, which the signed test below lets through (CMP/GE R3,R14 at
         * 0x8c179d1c) */
    } else if (address < SD_SND_MEM_SIZE) {
#endif
        syG2Write(&value, (void *)(SD_SND_MEM_BASE + address),
                  SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_FIX,
                  SYE_G2_ADR_MODE_FIX);
    }
    return result;
}
