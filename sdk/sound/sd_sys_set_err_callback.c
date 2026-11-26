#include <sd_work.h>
#include <sg_sd.h>

SDE_ERR sdSysSetErrCallback(const SD_ERR_CALLBACK_FUNC callback)
{
    SDE_ERR result = SDE_ERR_NOTHING;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
    } else {
        sdgWrkPtr->err_callback = callback;
    }
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdSysSetErrCallback, result);
    }
    return result;
}
