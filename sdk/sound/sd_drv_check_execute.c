#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

/* the sound processor's VECTORS vectors open sound memory, one ARM
 * instruction each. a vector whose top byte is 0xea holds the ARM's branch
 * with no condition */
#define VECTORS 8
#define VECTORS_SIZE (VECTORS * (Sint32)sizeof(Uint32))
#define OPCODE_MASK 0xff000000
#define OPCODE_BRANCH 0xea000000

SDE_ERR sdDrvCheckExecute(void)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    Sint32 address;
    Uint32 vector;

    if (sdgWrkPtr == NULL || sdgExistSndDrv != TRUE) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    for (address = 0; address < VECTORS_SIZE; address += sizeof(vector)) {
        sdSndMemReadUint32(address, &vector);
        if ((vector & OPCODE_MASK) != OPCODE_BRANCH) {
            result = SDE_ERR_DRV_PROBLEM;
            goto exit;
        }
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdDrvCheckExecute, result);
    }
    return result;
}
