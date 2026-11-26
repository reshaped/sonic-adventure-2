#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmGetCurAdr(SDPSTM handle, const Sint8 target_slot,
                        Sint32 *cur_smp_adr)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint8 port;
    Sint8 bank;
    Sint32 value;
    Sint32 bank_adr;
    Sint32 bank_size;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (cur_smp_adr == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    /* @hack spelling: bank, a local read ahead of target_slot's test, where
     * ext->ring_bank[target_slot] in sdSndMemGetBankStat's call would do for
     * a target_slot inside ring_bank */
    bank = ext->ring_bank[target_slot];
    if (target_slot < 0 || ext->slot_num <= target_slot) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    sdSndMemReadSint32(
        (Sint32)&SD_SND_SLOT_STATUS(ext->slot_no[target_slot])->cur_adr,
        &value);
    /* nothing reads bank_adr or bank_size. the call's only effect is
     * sdSysErrTrap's report of a bank it refuses */
    sdSndMemGetBankStat(SDE_DATA_TYPE_PSTM_RING_BUF, bank, &bank_adr,
                        &bank_size);
    switch (ext->pcm_type) {
    case SDE_PCM_TYPE_16BIT_PCM:
        value *= 2;
        break;
    case SDE_PCM_TYPE_8BIT_PCM:
        break;
    case SDE_PCM_TYPE_4BIT_ADPCM:
        value /= 2;
        break;
    }
    *cur_smp_adr = value;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmGetCurAdr, result);
    }
    return result;
}
