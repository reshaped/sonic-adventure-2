#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdSndMemReadSint8(Sint32 address, Sint8 *value)
{
    Uint32 offset;
    union sd_snd_mem_long data;

    if (address < SD_SND_MEM_SIZE) {
        /* @hack spelling: the masks of address computed ahead of the read,
         * where address masked at each use would do */
        offset = address & SD_SND_MEM_BYTE_BITS;
        address &= ~SD_SND_MEM_BYTE_BITS;
        sdSndMemReadUint32(address, &data.whole);
        *value = data.bytes[offset];
    }
    return SDE_ERR_NOTHING;
}
