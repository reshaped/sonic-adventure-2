#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmStopWrk(SDPSTM handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint8 port;
    Sint32 slot_max;
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
    /* @hack spelling: slot_max, a local read ahead of sdPstmOnMemStop's
     * call, where ext->slot_max in the loop's test would do */
    slot_max = ext->slot_max;
    if (ext->on_mem == TRUE) {
        sdPstmOnMemStop(handle);
    }
    for (i = 0; i < slot_max; i++) {
        ext->first_transfer[i] = TRUE;
        ext->transfer_adr[i] = 0;
        ext->transfer_wraps[i] = 0;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        /* @bug logic: reports a failure under sdPstmStop's address, not its
         * own (the pool word at 0x8c1750f4) */
        sdSysErrTrap((void *)sdPstmStop, result);
    }
    return result;
}
