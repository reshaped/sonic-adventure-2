#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdSysFlushHostCmd(void)
{
    SDE_ERR result;

    result = sdHostCmdServer();
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSysFlushHostCmd, result);
    }
    return result;
}
