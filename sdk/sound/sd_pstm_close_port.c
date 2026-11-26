#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>

SDE_ERR sdPstmClosePort(SDPSTM handle)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    struct sd_pstm_port_ext *ext;
    Sint32 port;
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
    result = sdPstmStop(handle);
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    sdPstmResetPrm(handle, SDD_PSTM_PORT_SLOT_ALL);
    /* @bug logic: a reset that fails doesn't stop the close. the close drops
     * sdPstmResetPrm's result, and this test reads sdPstmStop's again (TST at
     * 0x8c17796c) */
    if (result != SDE_ERR_NOTHING) {
        goto exit;
    }
    /* @bug memory: closing a port twice writes -1 over pstm_slot_used, then
     * takes slot_max from it again. the first close leaves -1 in each slot_no
     * (MOV.B R4,@R2 at 0x8c177996), so the second writes to
     * pstm_slot_port[-1], which is pstm_slot_used (MOV.B R4,@R2 at
     * 0x8c177988), ahead of the SUB at 0x8c1779aa. sdModuleClosePort refuses
     * the closed port only after both */
    for (i = 0; i < ext->slot_max; i++) {
        sdgWrkPtr->pstm_slot_port[ext->slot_no[i]] = -1;
        ext->slot_no[i] = -1;
        ext->ring_bank[i] = -1;
    }
    sdgWrkPtr->pstm_slot_used -= ext->slot_max;
    result = sdModuleClosePort((struct sd_port *)handle);
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdPstmClosePort, result);
    }
    return result;
}
