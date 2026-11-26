#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <string.h>

SDE_ERR sdModuleRefInit(void)
{
    int i;
    int j;

    memset(&sdgModuleRef, SD_UNSET_FILL, sizeof(sdgModuleRef));
    memset(sdgPstmPortExtRef, SD_UNSET_FILL, sizeof(sdgPstmPortExtRef));
    sdgWrkPtr->modules = &sdgModuleRef;
    sdgWrkPtr->pstm_ext = sdgPstmPortExtRef;
    sdgWrkPtr->unk_037 = 0;
    sdgWrkPtr->unk_038 = 0;
    sdgWrkPtr->pstm_slot_used = 0;
    for (i = 0; i < SDD_PSTM_SLOT_NUM; i++) {
        sdgWrkPtr->pstm_slot_port[i] = -1;
    }
    /* @bug logic: leaves the last record, the last stream port's, out of the
     * loop. that record keeps the all ones the memset of sdgModuleRef gave
     * unk_01 and opened. the loop's bound is H'26F0 bytes, 89 records of
     * H'70 (MOV.W at 0x8c16c002), where SD_PORT_NUM is 90. every test of
     * opened compares it with 1, so the port still reads as closed */
    for (i = 0; i < SD_PORT_NUM - 1; i++) {
        sdgWrkPtr->modules->all[i].head.unk_01 = 0;
        sdgWrkPtr->modules->all[i].opened = 0;
    }
    for (i = 0; i < SDD_MIDI_PORT_NUM; i++) {
        sdgWrkPtr->modules->by_kind.midi[i].kind = SD_PORT_KIND_MIDI;
        sdgWrkPtr->modules->by_kind.midi[i].head.port = i;
    }
    for (i = 0; i < SD_SHOT_PORT_NUM; i++) {
        sdgWrkPtr->modules->by_kind.shot[i].kind = SD_PORT_KIND_SHOT;
        sdgWrkPtr->modules->by_kind.shot[i].head.port = i;
    }
    for (i = 0; i < SD_PSTM_PORT_NUM; i++) {
        sdgWrkPtr->modules->by_kind.pstm[i].kind = SD_PORT_KIND_PSTM;
        sdgWrkPtr->modules->by_kind.pstm[i].head.port = i;
        /* @hack redundant: a loop whose test, j < -1, fails at once */
        for (j = 0; j < -1; j++) {
            sdgWrkPtr->pstm_ext[i].on_mem_blk[j] = NULL;
        }
        sdgWrkPtr->pstm_ext[i].kind = SD_PORT_KIND_PSTM;
        sdgWrkPtr->pstm_ext[i].port = i;
        sdgWrkPtr->pstm_ext[i].on_mem_play_req = FALSE;
        sdgWrkPtr->pstm_ext[i].on_mem = FALSE;
        sdgWrkPtr->pstm_ext[i].on_mem_transfer_size = 0;
        sdgWrkPtr->pstm_ext[i].on_mem_loop = FALSE;
    }
    sdgWrkPtr->modules->by_kind.global.kind = SD_PORT_KIND_GLOBAL;
    sdgWrkPtr->modules->by_kind.global.head.port = 0;
    sdgWrkPtr->modules->by_kind.gdda.kind = SD_PORT_KIND_GDDA;
    sdgWrkPtr->modules->by_kind.gdda.head.port = 0;
    return SDE_ERR_NOTHING;
}
