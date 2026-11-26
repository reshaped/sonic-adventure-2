#include <machine.h>
#include <sd_mw.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <stdarg.h>

SDE_ERR sdPstmOpenPortMw(SDPSTM *handle, const Sint8 use_slot_num, ...)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    va_list args;
    Sint32 mask;
    Sint32 taken;
    Sint32 i;
    Sint8 port;
    Sint8 bank[SDD_PSTM_PORT_SLOT_NUM];

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (use_slot_num < 1 || use_slot_num > SDD_PSTM_PORT_SLOT_NUM) {
        result = SDE_ERR_PRM_OVER_RANGE;
        goto exit;
    }
    if (sdgWrkPtr->pstm_slot_max - sdgWrkPtr->pstm_slot_used < use_slot_num) {
        result = SDE_ERR_MODULE_SLOT_NO_ENOUGH;
        goto exit;
    }
    /* @hack redundant: the fill of bank past use_slot_num with -1, which
     * nothing reads */
    for (i = use_slot_num; i < SDD_PSTM_PORT_SLOT_NUM; i++) {
        bank[i] = -1;
    }
    /* standard C leaves va_start undefined after a parameter that promotes,
     * as the Sint8 use_slot_num does. SHC passes use_slot_num on the stack
     * in four bytes, as it does an int. so the ints that follow it lie where
     * va_start looks */
    va_start(args, use_slot_num); /* NOLINT(clang-diagnostic-varargs) */
    for (i = 0; i < use_slot_num; i++) {
        bank[i] = va_arg(args, int);
        if (bank[i] < 0 || bank[i] >= SD_BANK_MAX) {
            result = SDE_ERR_PRM_OVER_RANGE;
            goto exit;
        }
    }
    va_end(args);
    result = sdModuleOpenPort(handle, SD_PORT_KIND_PSTM);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    port = ((struct sd_port_head *)*handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    taken = 0;
    for (i = 0; i < SDD_PSTM_SLOT_NUM; i++) {
        if (sdgWrkPtr->pstm_slot_port[i] < 0) {
            sdgWrkPtr->pstm_slot_port[i] = port;
            ext->slot_no[taken] = i;
            taken++;
            sdgWrkPtr->pstm_slot_used++;
            if (taken == use_slot_num) {
                break;
            }
        }
    }
    ext->slot_max = use_slot_num;
    sdPstmSetUseSlotNum(*handle, use_slot_num);
    sdPstmResetPrm(*handle, SDD_PSTM_PORT_SLOT_ALL);
    ext->pcm_type = SDE_PCM_TYPE_16BIT_LINEAR;
    ext->on_mem_pcm_type = SDE_PCM_TYPE_16BIT_LINEAR;
    /* @hack redundant: these four stores, which the four after
     * sdPstmSetBasePrm's call replace */
    ext->on_mem_play_req = FALSE;
    ext->on_mem = FALSE;
    ext->on_mem_transfer_size = -1;
    ext->on_mem_loop = FALSE;
    for (i = 0; i < SDD_PSTM_PORT_SLOT_NUM; i++) {
        ext->unk_270[i] = -1;
        ext->on_mem_transfer_ofs[i] = -1;
        ext->on_mem_blk[i] = (SDMEMBLK)-1;
    }
    sdPstmSetBasePrm(*handle, SDD_PSTM_PORT_SLOT_ALL, 0,
                     SD_PSTM_BASE_PAN_CENTER, 0, 0, 0,
                     SDD_PSTM_PORT_SLOT_FILTER_THROUGH, 0);
    ext->on_mem_play_req = FALSE;
    ext->on_mem = FALSE;
    ext->on_mem_transfer_size = 0;
    ext->on_mem_loop = FALSE;
    for (i = 0; i < use_slot_num; i++) {
        ext->on_mem_blk[i] = NULL;
        ext->unk_270[i] = 0;
        ext->on_mem_transfer_ofs[i] = 0;
    }
    for (i = 0; i < use_slot_num; i++) {
        ext->ring_bank[i] = bank[i];
        ext->first_transfer[i] = TRUE;
        ext->transfer_adr[i] = 0;
        ext->transfer_wraps[i] = 0;
    }
exit:
    set_imask(mask);
    if (result != SDE_ERR_NOTHING) {
        /* @bug logic: passes sdPstmOpenPort's address to sdSysErrTrap, where
         * its own belongs (the pool word at 0x8c1758f4) */
        sdSysErrTrap((void *)sdPstmOpenPort, result);
    }
    return result;
}
