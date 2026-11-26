#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the driver's bank map. the pointer counts bytes from sound memory's
 * start, so the CPU can't read or write through it. sdSndMemWriteSint32
 * writes an entry's fields instead, given their addresses */
#define DRV_BANK_MAP ((struct sd_bank_map *)sdgWrkPtr->data_map_adr)

SDE_ERR sdSndMemAllocateBank(const SDE_DATA_TYPE data_type,
                             const Sint8 bank_num, Sint32 adr, Sint32 size)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    /* the bank's entries in the driver's map and in the library's copy */
    struct sd_bank_stat *drv_stat;
    struct sd_bank_stat *stat;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (bank_num < 0) {
        result = SDE_ERR_BANK_ILLEGAL_NUM;
        goto exit;
    }
    switch (data_type) {
    case SDE_DATA_TYPE_MIDI_SEQ_BANK:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->midi_seq[bank_num];
        stat = &sdgWrkPtr->bank_map.midi_seq[bank_num];
        break;
    case SDE_DATA_TYPE_MIDI_PRG_BANK:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->midi_prg[bank_num];
        stat = &sdgWrkPtr->bank_map.midi_prg[bank_num];
        break;
    case SDE_DATA_TYPE_MIDI_DRM_BANK:
        if (bank_num >= SD_DRM_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->midi_drm[bank_num];
        stat = &sdgWrkPtr->bank_map.midi_drm[bank_num];
        break;
    case SDE_DATA_TYPE_SHOT_BANK:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->shot[bank_num];
        stat = &sdgWrkPtr->bank_map.shot[bank_num];
        break;
    case SDE_DATA_TYPE_PSTM_RING_BUF:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->pstm_ring[bank_num];
        stat = &sdgWrkPtr->bank_map.pstm_ring[bank_num];
        break;
    case SDE_DATA_TYPE_FX_PRG_BANK:
        if (bank_num >= SD_FX_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->fx_prg;
        stat = &sdgWrkPtr->bank_map.fx_prg;
        break;
    case SDE_DATA_TYPE_FX_OUT_BANK:
        if (bank_num >= SD_FX_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->fx_out;
        stat = &sdgWrkPtr->bank_map.fx_out;
        break;
    case SDE_DATA_TYPE_FX_PRG_WRK:
        if (bank_num >= SD_FX_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        drv_stat = &DRV_BANK_MAP->fx_prg_wrk;
        stat = &sdgWrkPtr->bank_map.fx_prg_wrk;
        break;
    /* @hack redundant: the cases that fall into default */
    case SDE_DATA_TYPE_DRV:
    case SDE_DATA_TYPE_MULTI_UNIT:
    default:
        result = SDE_ERR_BANK_ILLEGAL_TYPE;
        goto exit;
    }
    sdSndMemWriteSint32((Sint32)&drv_stat->adr, adr);
    sdSndMemWriteSint32((Sint32)&drv_stat->size, size);
    stat->adr = adr;
    stat->size = size;
exit:
    /* @hack redundant: a call of sdSysErrTrap that no path reaches */
    if (0) {
        sdSysErrTrap((void *)sdSndMemAllocateBank, result);
    }
    return result;
}
