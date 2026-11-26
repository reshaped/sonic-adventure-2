/* the sound library's stream calls as the middleware makes them */
#ifndef SD_MW_H
#define SD_MW_H
#include <sg_sd.h>
#include <sg_xpt.h>

/* each of these calls the function its name gives without Mw. it passes the
 * same arguments, and returns the result */
SDE_ERR sdPstmClosePortMw(SDPSTM handle);
SDE_ERR sdPstmGetCurAdrMw(SDPSTM handle, const Sint8 target_slot,
                          Sint32 *cur_smp_adr);
SDE_ERR sdPstmResetPrmMw(SDPSTM handle, const Sint8 target_slot);
SDE_ERR sdPstmSetDrctLevMw(SDPSTM handle, const Sint8 target_slot,
                           const Sint8 drct_lev);
SDE_ERR sdPstmSetFxChMw(SDPSTM handle, const Sint8 target_slot,
                        const Sint8 fx_in_ch, const Sint8 fx_base_lev);
SDE_ERR sdPstmSetPanMw(SDPSTM handle, const Sint8 target_slot, const Sint8 pan,
                       const Sint32 fade_time);
SDE_ERR sdPstmSetPitchMw(SDPSTM handle, const Sint8 target_slot,
                         const Sint16 pitch, const Sint32 fade_time);
SDE_ERR sdPstmSetVolMw(SDPSTM handle, const Sint8 target_slot, const Sint8 vol,
                       const Sint32 fade_time);
SDE_ERR sdPstmStopMw(SDPSTM handle);
/* call sdPstmSetUseSlotNum with the same arguments, and return its result */
SDE_ERR sdPstmSetUseChMw(SDPSTM handle, const Sint8 use_slot_num);
/* open a stream port as sdPstmOpenPort does. the call holds a copy of
 * sdPstmOpenPort's code, and doesn't call it. as shipped, a failure reaches
 * sdSysErrTrap under sdPstmOpenPort's address, not this call's */
SDE_ERR sdPstmOpenPortMw(SDPSTM *handle, const Sint8 use_slot_num, ...);
/* call sdPstmPlay, and return its result. sdPstmPlay has passed a failure
 * to sdSysErrTrap already, and this call passes it again under its own
 * address. it doesn't read priority */
SDE_ERR sdPstmPlayMw(SDPSTM handle, const SDE_PCM_TYPE pcm_type,
                     const Uint16 freq, const Sint8 priority);
/* call sdPstmSetBasePrm with a base_filter_lev of
 * SDD_PSTM_PORT_SLOT_FILTER_THROUGH and a base_filter_q of 0, and return
 * its result. base_pan counts from 0 for the middle: the call passes on
 * half of it plus SD_PSTM_BASE_PAN_CENTER */
SDE_ERR sdPstmSetBasePrmMw(SDPSTM handle, const Sint8 target_slot,
                           const Sint8 base_vol, const Sint8 base_pan,
                           const Sint8 fx_in_ch, const Sint8 base_fx_lev,
                           const Sint8 base_drct_lev);

#endif
