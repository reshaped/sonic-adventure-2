#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdMidiGetStat(SDMIDI handle, SDS_MIDI_STAT *midi_stat)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (midi_stat == NULL) {
        result = SDE_ERR_PTR_NULL;
        goto exit;
    }
    midi_stat->rsv = 0;
    sdMidiGetErr(handle, &midi_stat->m_Err);
    sdMidiGetFlg(handle, &midi_stat->m_Flg);
    sdMidiGetVol(handle, &midi_stat->m_Vol);
    sdMidiGetPan(handle, &midi_stat->m_Pan);
    sdMidiGetPitch(handle, &midi_stat->m_Pitch);
    sdMidiGetSpeed(handle, &midi_stat->m_Speed);
    sdMidiGetFxLev(handle, &midi_stat->m_FxLev);
    sdMidiGetDrctLev(handle, &midi_stat->m_DrctLev);
    sdMidiGetCurAdr(handle, &midi_stat->m_CurAdr);
    sdMidiGetTotalBeatTime(handle, &midi_stat->m_TotalBeatTime);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiGetStat, result);
    }
    return result;
}
