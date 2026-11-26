#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdModuleCheckPort(struct sd_port *port)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    /* @hack spelling: record, a copy of port for the read of opened, where
     * port itself would do */
    struct sd_port *record = port;

    if (port == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    if (port == (struct sd_port *)-1 || record->opened != 1) {
        result = SDE_ERR_HANDLE_ILLEGAL_VALUE;
        /* @hack redundant: a goto to the label that follows it */
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdModuleCheckPort, result);
    }
    return result;
}
