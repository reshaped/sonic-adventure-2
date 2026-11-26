/* the player's setup and teardown (sfdfx_mw.h), which nest. the first
 * mwPlyInitSfdFx starts the libraries the player runs on and enters its
 * servers with the manager. the last mwPlyFinishSfdFx stops them. while the
 * player runs, the operand cache keeps its index mode */
#include <adx_sfa.h>
#include <cri_adxt.h>
#include <cri_lsc.h>
#include <mwsfd.h>
#include <sfd_player.h>
#include <sfdfx_mw.h>
#include <sg_mw.h>
#include <sg_mwgsc.h>
#include <sg_mwply.h>
#include <sg_mwrv2.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <string.h>

#define CACHE_ENABLED                                                          \
    (SYD_CACHE_FORM_IC_ENABLE | SYD_CACHE_FORM_P1_CB | SYD_CACHE_FORM_OC_ENABLE)
#define OPERAND_CACHE_INDEX_MODE 0x80

/* the calls of mwPlyInitSfdFx that no mwPlyFinishSfdFx has answered. the
 * member exports this name, though no header of the SDK declares it */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Sint32 mwg_sfd_init_cnt = 0;

/* the manager calls an idle function with a void *, and mwSfdExecDecServer
 * takes none */
#ifdef NONMATCHING
/* this side calls it through a function of the idle function's type: a call
 * through another function type is undefined in C */
static void decode_idle(void *user)
{
    mwSfdExecDecServer();
}
#else
#define decode_idle ((void (*)(void *))mwSfdExecDecServer)
#endif

static void mwSfdTrapLscError(void *user, char *message)
{
    mwMngError(message);
}

/* the call takes 2 from display->latency, down to 0, and the caller's record
 * keeps the change. mwPlySetDispMode then gets a caller's latency of 3 to 5
 * as 1 to 3. those are the latencies it sets the renderers up for */
void mwPlyInitSfdFx(MWS_PLY_INIT_SFD *display)
{
    Sint32 mask;
    Sint32 result;

    /* @bug memory: reads and writes a null display's latency at 0x8c5453ec,
     * before the test for null at 0x8c545474 */
    display->latency -= 2;
    if (display->latency < 0) {
        display->latency = 0;
    }
    if (mwg_sfd_init_cnt == 0) {
        mwMngInit();
        ADXT_Init();
        SFA_Init();
        mwRnv2Init();
        SJRBF_Init();
        SJMEM_Init();
        SJUNI_Init();
        result = mwGscInitStatic();
        if (result != 0) {
            mwSfdSetErrCode(MWSFD_ERR_GSC_INIT);
            mwMngError("E2005 mwPlyInitSofdec: can't init GSC");
        }
        mask = mwPlyLock();
        memset(&mwg_ply_work, 0, sizeof(mwg_ply_work));
        syCacheInit(CACHE_ENABLED | OPERAND_CACHE_INDEX_MODE);
        mwMngEntryIdleFunc(decode_idle, NULL);
        if (display != NULL) {
            mwPlySetDispMode(display->mode, display->frame, display->count,
                             display->latency);
        }
        mwg_vcnt = 0;
        mwMngEntryServerFunc(mwSfdExecServer, NULL);
        mwMngEntryVsyncFunc(mwSfdVsync, NULL);
        mwMngEntryStartFrameFunc(mwSfdStartFrame, NULL);
        result = mwPlySfdInit(NULL, MWD_PLY_FTYPE_SFD, mwg_vsync_mhz);
        /* a decoder library that can't start stops the program here */
        if (result != 0) {
            for (;;) {
            }
        }
        mwg_sfd_init_flag = TRUE;
        mwg_sleep_enable_flag = TRUE;
        LSC_Init();
        LSC_EntryErrFunc(mwSfdTrapLscError, NULL);
        mwPlyUnlock(mask);
    }
    mwg_sfd_init_cnt++;
}

void mwPlyResetSfdFx(MWS_PLY_INIT_SFD *display)
{
    if (mwg_sfd_init_cnt != 0) {
        mwg_sfd_init_cnt = 0;
        mwPlyInitSfdFx(display);
    }
}

void mwPlyFinishSfdFx(void)
{
    Sint32 mask;
    Sint32 i;
    struct mwsfd_player *player;

    mwg_sfd_init_cnt--;
    if (mwg_sfd_init_cnt == 0) {
        mask = mwPlyLock();
        mwg_sleep_enable_flag = FALSE;
        for (i = 0; i < MWSFD_MAX_PLAYERS; i++) {
            player = &mwg_ply_work.players[i];
            if (player->used == TRUE) {
                /* @hack type: the player cast to MWPLY, where &player->base
                 * would do */
                mwSfdDestroy((MWPLY)player);
            }
        }
        LSC_Finish();
        SFD_Finish();
        mwMngDeleteIdleFunc(decode_idle);
        mwMngDeleteStartFrameFunc(mwSfdStartFrame);
        mwMngDeleteVsyncFunc(mwSfdVsync);
        mwMngDeleteServerFunc(mwSfdExecServer);
        ADXT_Finish();
        mwRnv2Finish();
        mwGscFinishStatic();
        SJUNI_Finish();
        SJMEM_Finish();
        SJRBF_Finish();
        syCacheInit(CACHE_ENABLED);
        mwMngFinish();
        mwPlyUnlock(mask);
    }
}
