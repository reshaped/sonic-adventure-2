#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmIsTransferWaveData(SDPSTM handle, const Sint8 target_slot,
                                 const Sint32 data_sz, Bool *flg)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 ring_size;
    Sint32 cur_adr;
    Sint32 ring_adr;
    Sint32 size_max;
    Sint32 room;
    Sint32 transfer_adr;
    Sint8 port;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (flg == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    *flg = FALSE;
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    sdSndMemGetBankStat(SDE_DATA_TYPE_PSTM_RING_BUF,
                        ext->ring_bank[target_slot], &ring_adr, &ring_size);
#ifdef NONMATCHING
    /* GCC can't see that no call reaches the tag below. only
     * sdPstmOnMemServer calls this, for a port whose on_mem is TRUE, and
     * nothing stores TRUE there. size_max starts at ring_size, which leaves
     * ring_size as it is */
    size_max = ring_size;
#endif
    switch (ext->pcm_type) {
    case SDE_PCM_TYPE_4BIT_ADPCM:
        size_max = SD_PSTM_RING_MAX_ADPCM;
        break;
    case SDE_PCM_TYPE_8BIT_PCM:
        size_max = SD_PSTM_RING_MAX_8BIT;
        break;
    case SDE_PCM_TYPE_16BIT_PCM:
        size_max = SD_PSTM_RING_MAX_16BIT;
        break;
    }
    /* @bug uninitialized: reads size_max unset for a pcm_type that no case
     * above names (CMP/GE at 0x8c177a80). sdPstmPlay stores such a pcm_type
     * before it refuses it */
    if (size_max < ring_size) {
        ring_size = size_max;
    }
    if (ext->first_transfer[target_slot] == TRUE) {
        if (ring_size < data_sz) {
            result = SDE_ERR_PRM_OVER_RANGE;
            goto exit;
        }
        room = ring_size;
    } else {
        sdPstmGetCurAdr(handle, target_slot, &cur_adr);
        /* @hack spelling: transfer_adr, a local for the last transfer's
         * end, where ext->transfer_adr[target_slot] in the test and in both
         * differences would do */
        transfer_adr = ext->transfer_adr[target_slot];
        if (transfer_adr <= cur_adr) {
            room = cur_adr - transfer_adr;
        } else {
            room = ring_size - transfer_adr;
        }
    }
    /* @hack spelling: the test with ? TRUE : FALSE, where data_sz <= room
     * alone would do */
    *flg = (data_sz <= room) ? TRUE : FALSE;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmIsTransferWaveData, result);
    }
    return result;
}
