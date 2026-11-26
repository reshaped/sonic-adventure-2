#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* a command for the stop. sdPstmOnMemStop sends the driver nothing */
static struct sd_host_cmd pstm_on_mem_stop_cmd;

SDE_ERR sdPstmOnMemStop(SDPSTM handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint8 port;
    Sint32 i;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
    } else {
        /* @hack spelling: port, a local for the port's number, where
         * ((struct sd_port_head *)handle)->port as the index would do */
        port = ((struct sd_port_head *)handle)->port;
        ext = &sdgWrkPtr->pstm_ext[port];
        ext->on_mem = FALSE;
        for (i = 0; i < ext->slot_num; i++) {
            ext->on_mem_transfer_ofs[i] = 0;
        }
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmOnMemStop, result);
    }
    return result;
}
