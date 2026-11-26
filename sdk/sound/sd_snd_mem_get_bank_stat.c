#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the size stored for a data_type that has no banks */
#define NO_BANK_SIZE (-1)

SDE_ERR sdSndMemGetBankStat(const SDE_DATA_TYPE data_type, const Sint8 bank_num,
                            Sint32 *allocate_ptr, Sint32 *allocate_size)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_bank_stat *stat;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (allocate_ptr == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    if (allocate_size == NULL) {
        result = SDE_ERR_PRM_OVER_RANGE;
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
        stat = &sdgWrkPtr->bank_map.midi_seq[bank_num];
        break;
    case SDE_DATA_TYPE_MIDI_PRG_BANK:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.midi_prg[bank_num];
        break;
    case SDE_DATA_TYPE_MIDI_DRM_BANK:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.midi_drm[bank_num];
        break;
    case SDE_DATA_TYPE_SHOT_BANK:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.shot[bank_num];
        break;
    case SDE_DATA_TYPE_PSTM_RING_BUF:
        if (bank_num >= SD_BANK_MAX) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.pstm_ring[bank_num];
        break;
    case SDE_DATA_TYPE_FX_PRG_BANK:
        if (bank_num != 0) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.fx_prg;
        break;
    case SDE_DATA_TYPE_FX_OUT_BANK:
        if (bank_num != 0) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.fx_out;
        break;
    case SDE_DATA_TYPE_FX_PRG_WRK:
        if (bank_num != 0) {
            result = SDE_ERR_BANK_ILLEGAL_NUM;
            goto exit;
        }
        stat = &sdgWrkPtr->bank_map.fx_prg_wrk;
        break;
    /* @hack redundant: the cases that fall into default */
    case SDE_DATA_TYPE_DRV:
    case SDE_DATA_TYPE_MULTI_UNIT:
    default:
        result = SDE_ERR_BANK_ILLEGAL_TYPE;
        stat = NULL;
        break;
    }
    if (stat != NULL) {
        *allocate_ptr = stat->adr;
        *allocate_size = stat->size;
    } else {
        /* @hack redundant: the tests of allocate_ptr and allocate_size,
         * which the tests above have found not null */
        if (allocate_ptr != NULL) {
            *allocate_ptr = 0;
        }
        if (allocate_size != NULL) {
            *allocate_size = NO_BANK_SIZE;
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSndMemGetBankStat, result);
    }
    return result;
}
