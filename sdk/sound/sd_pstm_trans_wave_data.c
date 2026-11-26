#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* what the member keeps of each stream channel's transfers.
 * pstm_transfer_callback gets a channel's record as its argument */
struct pstm_transfer {
    /* TRUE once any call has given the channel a block. nothing reads it */
    Bool blk_given;
    Sint32 unk_04; /* nothing reads or writes it */
    /* the block the channel's last call gave it. a call sets it before its
     * range tests, so a call they refuse sets it too */
    SDMEMBLK blk;
    /* TRUE once a transfer starts at the ring buffer's first byte. the
     * channel's next call that isn't refused clears it. that call copies the
     * first long to the long just past the part the stream uses */
    Bool head_copy_due;
};

static struct pstm_transfer pstm_transfers[SD_PSTM_PORT_NUM]
                                          [SDD_PSTM_PORT_SLOT_NUM];

static void pstm_transfer_callback(void *first_arg);

SDE_ERR sdPstmTransferWaveData(SDPSTM handle, const Sint8 target_slot,
                               SDMEMBLK memblk)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 ring_adr;
    Sint32 ring_size;
    Sint32 size_max;
    Sint32 data_sz;
    Sint32 transfer_adr;
    Uint32 head;
    Sint8 port;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL || memblk == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    /* @bug memory: writes past the port's row of pstm_transfers for a
     * target_slot of SDD_PSTM_PORT_SLOT_NUM or more (MOV.L R2,@R5 at
     * 0x8c178224). it reads past ext->transfer_adr as well (MOV.L @R3,R2 at
     * 0x8c17823c). the test below refuses such a target_slot only after both
     * (CMP/GT at 0x8c178244) */
    pstm_transfers[port][target_slot].blk_given = TRUE;
    pstm_transfers[port][target_slot].blk = memblk;
    sdMemBlkSetRealUseCallback(memblk, pstm_transfer_callback,
                               &pstm_transfers[port][target_slot]);
    /* @hack spelling: data_sz assigned as *&data_sz, where a plain data_sz
     * would do */
    *&data_sz = ((struct sd_memblk *)memblk)->size;
    /* @hack spelling: transfer_adr, a local read ahead of target_slot's
     * test, where ext->transfer_adr[target_slot] in sdMemBlkTransfer's call
     * would do */
    transfer_adr = ext->transfer_adr[target_slot];
    /* @bug memory: indexes below the port's row of pstm_transfers and below
     * ext's arrays for a negative target_slot, which this test lets through
     * (CMP/GT at 0x8c178244) */
    if (ext->slot_num <= target_slot) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    sdSndMemGetBankStat(SDE_DATA_TYPE_PSTM_RING_BUF,
                        ext->ring_bank[target_slot], &ring_adr, &ring_size);
#ifdef NONMATCHING
    /* GCC can't see that no call reaches the uninitialized tag below. only
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
    /* @bug uninitialized: reads size_max unset where pcm_type is none of the
     * three kinds above (CMP/GE at 0x8c178282). sdPstmPlay stores a pcm_type
     * it refuses, so pcm_type can hold one that no case names */
    if (size_max < ring_size) {
        ring_size = size_max;
    }
    if (ring_size < ext->transfer_adr[target_slot] + data_sz) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    if (pstm_transfers[port][target_slot].head_copy_due == TRUE) {
        sdSndMemReadUint32(ring_adr, &head);
        sdSndMemWriteUint32(ring_adr + ring_size, head);
        pstm_transfers[port][target_slot].head_copy_due = FALSE;
    }
    if (ext->transfer_adr[target_slot] == 0) {
        pstm_transfers[port][target_slot].head_copy_due = TRUE;
    }
    /* @bug logic: ignores sdMemBlkTransfer's result (JSR at 0x8c178326). a
     * block it refuses still moves transfer_adr on by the block's size, so
     * the channel's next transfer starts past bytes that never arrived.
     * sdPstmTransferWaveData still returns SDE_ERR_NOTHING */
    sdMemBlkTransfer(memblk, ring_adr + transfer_adr);
    ext->transfer_adr[target_slot] += data_sz;
    if (ext->transfer_adr[target_slot] == ring_size) {
        ext->transfer_adr[target_slot] = 0;
        ext->transfer_wraps[target_slot]++;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmTransferWaveData, result);
    }
    return result;
}

/* the callback sdPstmTransferWaveData gives each block. it puts back the
 * callback and argument that sdMemBlkSetPrm recorded for the block. it then
 * hands the block to sdMemBlkExecuteCallback */
static void pstm_transfer_callback(void *first_arg)
{
    struct pstm_transfer *transfer = first_arg;
    SD_MEMBLK_CALLBACK_FUNC callback;
    void *arg;

    /* @hack spelling: callback and arg assigned as *&callback and *&arg,
     * where a plain callback and arg would do */
    *&callback = ((struct sd_memblk *)transfer->blk)->set_callback;
    *&arg = (void *)((struct sd_memblk *)transfer->blk)->set_arg;
    sdMemBlkSetRealUseCallback(transfer->blk, callback, arg);
    sdMemBlkExecuteCallback(transfer->blk);
}
