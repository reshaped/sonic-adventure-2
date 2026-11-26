#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdSndMemReadUint16(Sint32 address, Uint16 *value)
{
    Uint32 long_address;
    Uint32 offset;
    union sd_snd_mem_long data;

    if (address < SD_SND_MEM_SIZE) {
        /* @hack spelling: the masks of address computed ahead of the read,
         * from an unsigned copy in long_address, where address masked at each
         * use would do */
        long_address = address;
        offset = long_address & SD_SND_MEM_BYTE_BITS;
        long_address &= ~SD_SND_MEM_BYTE_BITS;
        sdSndMemReadUint32(long_address, &data.whole);
        *value = data.halves[offset / sizeof(Uint16)];
    }
    return SDE_ERR_NOTHING;
}
