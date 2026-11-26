#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

static struct sd_port_levels pstm_reset_levels = {0};

SDE_ERR sdPstmResetPrmWrk(SDPSTM handle, Sint32 target_slot)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port as the index would do */
    port = ((struct sd_port_head *)handle)->port;
    ext = &sdgWrkPtr->pstm_ext[port];
    for (i = 0; i < SDD_PSTM_PORT_SLOT_NUM; i++) {
        if (target_slot == i || target_slot == SDD_PSTM_PORT_SLOT_ALL) {
            ext->target[i] = pstm_reset_levels;
            ext->cur[i] = pstm_reset_levels;
            ext->step[i] = pstm_reset_levels;
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmResetPrmWrk, result);
    }
    return result;
}
