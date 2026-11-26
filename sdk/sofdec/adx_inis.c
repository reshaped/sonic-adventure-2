/* CRI ADX's start and end, its V-blank server, and its streams' records */
#include <adx_crs.h>
#include <adxahx.h>
#include <adx_errs.h>
#include <adx_inis.h>
#include <adx_rnas.h>
#include <adx_sjd.h>
#include <adx_stmc.h>
#include <cri_adxf.h>
#include <cri_adxt.h>
#include <cri_lsc.h>
#include <sg_chain.h>
#include <mw_rna.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

/* ADXT_VsyncProc's priority on the V-blank chain, which calls lower
 * priorities first */
#define VSYNC_PRIORITY 112

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ADXT_ConfigVsyncSvr(Bool flag);
void adxini_rnaerr_cbfn(void *obj, char *msg);

/* the text "MARK:adxt_obj", which nothing in ADX reads */
char adxt_obj_mark[] = "MARK:adxt_obj";
ADX_TALK adxt_obj[ADXT_MAX_OBJ] = {0};
/* the handler number syChainAddHandler gave ADXT_VsyncProc on the V-blank
 * chain, or 0 while it has none */
SYCHAIN adxt_chain = 0;
/* the ADXT_Init calls that no ADXT_Finish has matched yet */
Sint32 adxt_init_cnt = 0;
/* the outermost ADXT_Init adds ADXT_VsyncProc to the V-blank chain only
 * while this is TRUE */
Bool adxt_vsync_svr_flag = TRUE;
Sint32 adxt_vsync_cnt = 0;
/* NOLINTEND(misc-use-internal-linkage) */

void ADXT_ConfigVsyncSvr(Bool flag)
{
    adxt_vsync_svr_flag = flag;
}

/* the sound output's error function, which reports msg as the library's
 * error */
void adxini_rnaerr_cbfn(void *obj, char *msg)
{
    ADXERR_CallErrFunc1(msg);
}

void ADXT_VsyncProc(void)
{
    adxt_vsync_cnt++;
    ADXT_ExecServer();
}

void ADXT_Init(void)
{
    if (adxt_init_cnt == 0) {
        ADXCRS_Init();
        ADXCRS_Lock();
        SJUNI_Init();
        SJRBF_Init();
        SJMEM_Init();
        ADXERR_Init();
        ADXSTM_Init();
        ADXSJD_Init();
        ADXF_Init();
        ADXRNA_Init();
        adxahx_init();
        mwRnaEntryErrFunc(adxini_rnaerr_cbfn, NULL);
        memset(adxt_obj, 0, sizeof(adxt_obj));
        LSC_Init();
        if (adxt_vsync_svr_flag == TRUE) {
            /* the chain calls each handler with its arg, null here, which
             * ADXT_VsyncProc doesn't take. the argument arrives in a
             * register that ADXT_VsyncProc never reads */
            adxt_chain = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK,
                                           (void (*)(void *))ADXT_VsyncProc,
                                           VSYNC_PRIORITY, NULL);
        }
        adxt_vsync_cnt = 0;
        ADXCRS_Unlock();
    }
    adxt_init_cnt++;
}

void ADXT_ResetLibrary(void)
{
    adxt_init_cnt = 0;
    ADXT_Init();
}

void ADXT_Finish(void)
{
    adxt_init_cnt--;
    if (adxt_init_cnt != 0) {
        return;
    }
    ADXCRS_Lock();
    if (adxt_chain != 0) {
        syChainDeleteHandler(adxt_chain);
        adxt_chain = 0;
    }
    LSC_Finish();
    ADXCRS_Unlock();
    ADXRNA_Finish();
    ADXCRS_Lock();
    ADXF_Finish();
    ADXSJD_Finish();
    ADXSTM_Finish();
    ADXERR_Finish();
    SJMEM_Finish();
    SJRBF_Finish();
    SJUNI_Finish();
    ADXCRS_Unlock();
}
