#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdBankDownload(SDMEMBLK handle, const SDE_DATA_TYPE bank_type,
                       const Sint8 bank_num)
{
    SDE_ERR result;
    Sint32 adr;
    Sint32 size;

    result = sdSndMemGetBankStat(bank_type, bank_num, &adr, &size);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    if (adr == 0 || size <= 0) {
        result = SDE_ERR_BANK_NO_MAPPING;
        goto exit;
    }
    result = sdMemBlkTransfer(handle, adr);
    /* @hack redundant: a goto to the label that follows it */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdBankDownload, result);
    }
    return result;
}
