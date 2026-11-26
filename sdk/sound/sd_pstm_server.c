#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* step the fading level of port's channel ch, in ext, toward its target,
 * and stop it there. set tells the driver the level whenever the step moves
 * it to another multiple of SD_LEVEL_UNIT, rounded down. prev takes the
 * level from before the step */
#define FADE(port, ext, ch, prev, level, set)                                  \
    if ((ext)->step[ch].level != 0) {                                          \
        (prev) = (ext)->cur[ch].level;                                         \
        (ext)->cur[ch].level += (ext)->step[ch].level;                         \
        if ((ext)->step[ch].level < 0 &&                                       \
            (ext)->cur[ch].level <= (ext)->target[ch].level) {                 \
            (ext)->step[ch].level = 0;                                         \
            (ext)->cur[ch].level = (ext)->target[ch].level;                    \
        }                                                                      \
        /* @hack spelling: target <= cur for a step above 0, where cur >=      \
         * target would do */                                                  \
        if ((ext)->step[ch].level > 0 &&                                       \
            (ext)->target[ch].level <= (ext)->cur[ch].level) {                 \
            (ext)->step[ch].level = 0;                                         \
            (ext)->cur[ch].level = (ext)->target[ch].level;                    \
        }                                                                      \
        if (((prev) & ~(SD_LEVEL_UNIT - 1)) !=                                 \
            ((ext)->cur[ch].level & ~(SD_LEVEL_UNIT - 1))) {                   \
            (set)((SDPSTM)(port), (ch), (ext)->cur[ch].level / SD_LEVEL_UNIT); \
        }                                                                      \
    }

SDE_ERR sdPstmServer(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_port *port;
    struct sd_pstm_port_ext *ext;
    Sint32 prev;
    Sint32 i;
    Sint32 ch;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        for (i = 0; i < SD_PSTM_PORT_NUM; i++) {
            /* @hack spelling: pstm[i] indexed for the test of opened and
             * again to set port, where port set first and port->opened
             * tested would do */
            if (sdgWrkPtr->modules->by_kind.pstm[i].opened == 1) {
                port = &sdgWrkPtr->modules->by_kind.pstm[i];
                ext = &sdgWrkPtr->pstm_ext[i];
                result = sdPstmOnMemServer((SDPSTM)port);
                if (result != SDE_ERR_NOTHING) {
                    break;
                }
                for (ch = 0; ch < ext->slot_max; ch++) {
                    FADE(port, ext, ch, prev, vol, sdDrvPstmSetVol);
                    FADE(port, ext, ch, prev, pan, sdDrvPstmSetPan);
                    FADE(port, ext, ch, prev, pitch, sdDrvPstmSetPitch);
                }
            }
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmServer, result);
    }
    return result;
}
