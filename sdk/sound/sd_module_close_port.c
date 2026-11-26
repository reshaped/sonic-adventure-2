#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdModuleClosePort(struct sd_port *port)
{
    SDE_ERR result;

    result = sdModuleCheckPort(port);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    port->opened = 0;
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdModuleClosePort, result);
    }
    return result;
}
