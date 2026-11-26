#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmOnMemServer(SDPSTM handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 mask;
    struct sd_pstm_port_ext *ext;
    SDMEMBLK blk;
    Sint32 total;
    Sint32 src;
    SDE_MEMBLK_STAT stat;
    Bool fits;
    Sint32 i;
    Sint8 port;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    if (ext->on_mem != TRUE) {
        goto exit;
    }
    if (sdMemBlkCheck(ext->on_mem_blk[0]) != SDE_ERR_NOTHING) {
        goto exit;
    }
    /* @hack spelling: total assigned as *&total, where a plain total would
     * do */
    *&total = ((struct sd_memblk *)ext->on_mem_blk[0])->set_size;
    if (total <= ext->on_mem_transfer_ofs[0]) {
        if (ext->on_mem_loop == TRUE) {
            for (i = 0; i < ext->slot_num; i++) {
                ext->on_mem_transfer_ofs[i] = 0;
            }
            goto exit;
        } else {
            sdPstmStop(handle);
            goto exit;
        }
    }
    for (i = 0; i < ext->slot_num; i++) {
        blk = ext->on_mem_blk[i];
        sdPstmIsTransferWaveData(handle, i, ext->on_mem_transfer_size, &fits);
        if (fits != TRUE) {
            continue;
        }
        sdMemBlkGetStat(blk, &stat);
        if (stat == SDE_MEMBLK_STAT_TRANSFER_PROGRESS) {
            continue;
        }
        /* @hack spelling: src assigned as *&src, where a plain src would
         * do */
        *&src = (Sint32)((struct sd_memblk *)blk)->set_ptr;
        sdMemBlkSetRealUsePrm(blk, (void *)(src + ext->on_mem_transfer_ofs[i]),
                              ext->on_mem_transfer_size, SDD_MEMBLK_SYNC_FUNC,
                              NULL);
        sdPstmTransferWaveData(handle, i, blk);
        ext->on_mem_transfer_ofs[i] += ext->on_mem_transfer_size;
    }
    if (ext->on_mem_play_req == TRUE) {
        result = sdPstmPlay(handle, ext->on_mem_pcm_type, ext->on_mem_pcm_freq);
        if (result == SDE_ERR_NOTHING) {
            ext->on_mem_play_req = FALSE;
        }
    }
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmOnMemServer, result);
    }
    return result;
}
