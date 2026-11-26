#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the header that opens a sound driver's file. size counts the whole file,
 * this header included */
struct drv_header {
    Uint32 tag;
    Uint32 unk_04;
    Sint32 size;
    Uint32 unk_0c;
    Uint8 ver;
    Uint8 unk_11[0xf];
};
#define DRV_VER 2
/* the execute count the driver must reach before the call reads its stat */
#define DRV_RUN_COUNT 10

SDE_ERR sdDrvInit(SDMEMBLK handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    /* @hack redundant: header set null where it is declared, though the
     * assignment below replaces it */
    struct drv_header *header = NULL;
    Uint32 counter;
    Uint32 stat;
    SD_MEMBLK_CALLBACK_FUNC callback;
    void *arg;

    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: header assigned as *&header, where a plain header
     * would do */
    *&header = (struct drv_header *)((struct sd_memblk *)handle)->set_ptr;
    if (header->tag != SDE_DATA_TYPE_DRV) {
        result = SDE_ERR_DATA_ILLEGAL_TYPE;
        goto exit;
    }
    if (header->ver != DRV_VER) {
        result = SDE_ERR_DRV_ILLEGAL_VER;
        goto exit;
    }
    sdArm7ResetKeep();
    sdSndMemClear();
    sdMemBlkSetRealUsePrm(handle, header + 1,
                          header->size - sizeof(struct drv_header),
                          SDD_MEMBLK_SYNC_FUNC, NULL);
    sdMemBlkTransfer(handle, 0);
    sdDrvGetInfo();
    sdArm7ResetRel();
    sdgExistSndDrv = TRUE;
    do {
        sdDrvGetExecuteCounter(&counter);
    } while (counter == SD_SND_MEM_FILL);
    do {
        sdDrvGetExecuteCounter(&counter);
    } while (counter < DRV_RUN_COUNT);
    do {
        sdSndMemReadUint32(SD_DRV_INFO_ADR(sdgWrkPtr->drv_info_adr, stat),
                           &stat);
    } while (stat != SD_DRV_STAT_EXEC);
    /* @hack spelling: callback and arg assigned as *&callback and *&arg,
     * where a plain callback and arg would do */
    *&callback = ((struct sd_memblk *)handle)->set_callback;
    *&arg = (void *)((struct sd_memblk *)handle)->set_arg;
    sdMemBlkSetRealUseCallback(handle, callback, arg);
    sdMemBlkExecuteCallback(handle);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvInit, result);
    }
    return result;
}
