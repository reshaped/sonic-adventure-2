#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdDrvGetInfo(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 address;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        address = sdgWrkPtr->drv_adr + SD_DRV_INFO_AT;
        sdgWrkPtr->drv_info_adr = address;
        sdSndMemReadUint8(SD_DRV_INFO_ADR(address, ver_major),
                          &sdgWrkPtr->ver_major);
        sdSndMemReadUint8(SD_DRV_INFO_ADR(address, ver_minor),
                          &sdgWrkPtr->ver_minor);
        sdSndMemReadUint8(SD_DRV_INFO_ADR(address, ver_local),
                          &sdgWrkPtr->ver_local);
        sdgWrkPtr->ver_end = 0;
        sdSndMemReadSint32(SD_DRV_INFO_ADR(address, unk_a0),
                           &sdgWrkPtr->unk_84);
        sdSndMemReadSint32(SD_DRV_INFO_ADR(address, snd_stat_adr),
                           &sdgWrkPtr->snd_stat_adr);
        sdSndMemReadSint32(SD_DRV_INFO_ADR(address, stat_adr),
                           &sdgWrkPtr->stat_adr);
        sdSndMemReadSint32(SD_DRV_INFO_ADR(address, data_map_adr),
                           &sdgWrkPtr->data_map_adr);
        sdSndMemReadSint32(SD_DRV_INFO_ADR(address, host_cmd_adr),
                           &sdgWrkPtr->host_cmd_adr);
        sdSndMemReadSint32(SD_DRV_INFO_ADR(address, unk_b4),
                           &sdgWrkPtr->unk_98);
        sdgWrkPtr->midi_slot_max = SD_MIDI_SLOT_DEFAULT;
        sdgWrkPtr->shot_slot_max = SD_SHOT_SLOT_DEFAULT;
        sdgWrkPtr->pstm_slot_max = SD_PSTM_SLOT_DEFAULT;
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvGetInfo, result);
    }
    return result;
}
