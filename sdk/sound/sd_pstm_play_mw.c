#include <sd_mw.h>
#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmPlayMw(SDPSTM handle, const SDE_PCM_TYPE pcm_type,
                     const Uint16 freq, const Sint8 priority)
{
    SDE_ERR result = sdPstmPlay(handle, pcm_type, freq);

    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmPlayMw, result);
    }
    return result;
}
