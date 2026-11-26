#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdModuleOpenPort(SDHANDLE **handle, Sint32 kind)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_port *port;
    int i;

    switch (kind) {
    case SD_PORT_KIND_MIDI:
        port = sdgWrkPtr->modules->by_kind.midi;
        for (i = 0; i < SDD_MIDI_PORT_NUM; i++, port++) {
            if (port->opened != 1) {
                break;
            }
        }
        if (i == SDD_MIDI_PORT_NUM) {
            port = NULL;
        }
        break;
    case SD_PORT_KIND_SHOT:
        port = sdgWrkPtr->modules->by_kind.shot;
        for (i = 0; i < SD_SHOT_PORT_NUM; i++, port++) {
            if (port->opened != 1) {
                break;
            }
        }
        if (i == SD_SHOT_PORT_NUM) {
            port = NULL;
        }
        break;
    case SD_PORT_KIND_PSTM:
        port = sdgWrkPtr->modules->by_kind.pstm;
        for (i = 0; i < SD_PSTM_PORT_NUM; i++, port++) {
            if (port->opened != 1) {
                break;
            }
        }
        if (i == SD_PSTM_PORT_NUM) {
            port = NULL;
        }
        break;
    default:
        result = SDE_ERR_UNKNOW_NUM;
        goto exit;
    }
    if (port != NULL) {
        port->opened = 1;
        /* @hack spelling: the fields of target, cur and step cleared in this
         * order, where struct sd_port_levels' order would do */
        port->target.vol = 0;
        port->target.pan = 0;
        port->target.speed = 0;
        port->target.pitch = 0;
        port->target.fx_ch = 0;
        port->target.fx_lev = 0;
        port->target.drct_lev = 0;
        port->cur.vol = 0;
        port->cur.pan = 0;
        port->cur.speed = 0;
        port->cur.pitch = 0;
        port->cur.fx_ch = 0;
        port->cur.fx_lev = 0;
        port->cur.drct_lev = 0;
        port->step.vol = 0;
        port->step.pan = 0;
        port->step.speed = 0;
        port->step.pitch = 0;
        port->step.fx_ch = 0;
        port->step.fx_lev = 0;
        port->step.drct_lev = 0;
        port->unk_68 = 0;
        *handle = (SDHANDLE *)port;
        /* @hack redundant: both the goto and the else that follows it, where
         * either alone would do */
        goto exit;
    } else {
        result = SDE_ERR_HANDLE_NO_ENOUGH;
        *handle = NULL;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdModuleOpenPort, result);
    }
    return result;
}
