#include <machine.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

/* the first address in array that is a multiple of SD_BUFFER_ALIGN */
#define ALIGNED_START(array)                                                   \
    (((Uint32)(array) + SD_BUFFER_ALIGN - 1) & ~(SD_BUFFER_ALIGN - 1))

SDE_ERR sdSysWrkInit(Sint32 mem_blk_handle_max, Sint32 mem_blk_queue_max,
                     Sint32 second_host_cmd_max)
{
    Sint32 mask;
    void *prim_buf;

    mask = get_imask();
    set_imask(IMASK_ALL);
    /* @hack spelling: SD_HOST_CMD_QUEUE_NUM carried to sdHostCmd2ndBufInit
     * in the parameter second_host_cmd_max, where SD_HOST_CMD_QUEUE_NUM
     * passed directly would do */
    second_host_cmd_max = SD_HOST_CMD_QUEUE_NUM;
    sdgWrkPtr = (struct sd_work *)ALIGNED_START(sdgGlobalWrk);
    memset(sdgWrkPtr, SD_UNSET_FILL, sizeof(struct sd_work));
    sdgWrkPtr->drv_adr = 0;
    /* @hack redundant: the reads of host_cmd.sending and batches_started,
     * which nothing uses */
    (void)sdgWrkPtr->host_cmd.sending;
    (void)sdgWrkPtr->batches_started;
    prim_buf = (void *)ALIGNED_START(sdgHostCmdPrimBuf);
    sdMemBlkInit(sdgMemBlk, SDD_MEMBLK_HANDLE_NUM);
    sdMemBlkQueueInit(sdgMemBlkQueue, SDD_MEMBLK_QUEUE_NUM);
    sdHostCmdPrimBufInit(prim_buf);
    sdHostCmd2ndBufInit(sdgHostCmd2ndBuf, second_host_cmd_max);
    sdgWrkPtr->batches_started = 0;
    sdgWrkPtr->batches_ended = 0;
    /* @hack redundant: a store of FALSE to host_cmd.sending, which
     * sdHostCmd2ndBufInit has stored already */
    sdgWrkPtr->host_cmd.sending = FALSE;
    sdModuleRefInit();
    sdgExistSndDrv = FALSE;
    sdMemBlkSetTransferMode(SDE_MEMBLK_TRANSFER_MODE_DMA);
    sdMemBlkCreate(&sdgWrkPtr->unit_blk);
    sdMemBlkCreate(&sdgWrkPtr->unk_430);
    sdSysSetErrCallback(NULL);
    set_imask(mask);
    sdSysSetSlotMax(SD_MIDI_SLOT_DEFAULT, SD_SHOT_SLOT_DEFAULT,
                    SD_PSTM_SLOT_DEFAULT);
    return SDE_ERR_NOTHING;
}
