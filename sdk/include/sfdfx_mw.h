/* the middleware player's Sofdec calls, from its setup to the display of its
 * pictures through Ninja */
#ifndef SFDFX_MW_H
#define SFDFX_MW_H
#include <sg_xpt.h>
#include <sg_mwply.h>
#include <sj.h>

typedef struct {
    Sint32 mode, frame, count, latency;
    Sint32 rsv[4];
} MWS_PLY_INIT_SFD;
typedef struct {
    Sint32 ftype, max_bps, max_width, max_height, nfrm_pool_wk;
    Sint8 *work;
    Sint32 wksize, dtype, compo_mode;
} MWS_PLY_CPRM_SFD;
typedef struct {
    void *srf;
    Sint32 width, height;
} MWS_PLY_MVFRM;

#define MWD_PLY_FTYPE_SFD 1
#define MWD_PLY_FTYPE_MPV 2
#define MWD_PLY_DTYPE_AUTO 0
#define MWD_PLY_COMPO_OPEQ 0
#define MWD_PLY_DTYPE_FULL 1
#define MWD_PLY_DTYPE_WND 2
#define MWD_PLY_COMPO_TRNSP 1
#define MWD_PLY_COMPO_ADD 2
#define MWD_PLY_COMPO_LUMI 4
#define MWD_PLY_COMPO_ALPH3 5
#define MWD_PLY_COMPO_ALPH5 6
#define MWD_PLY_COMPO_ALPH256 7
#define MWD_PLY_COMPO_MAT 9 /* the mask is the matte mwPlySetMat copies */

void mwPlyInitSfdFx(MWS_PLY_INIT_SFD *display);
void mwPlyResetSfdFx(MWS_PLY_INIT_SFD *display);
void mwPlyFinishSfdFx(void);
void mwPlyInitSfdNj2(MWS_PLY_INIT_SFD *display);
void mwPlyFinishNj2(void);
void mwPlyInitSofdec(MWS_PLY_INIT_SFD *display);
void mwPlyFinishSofdec(void);
Sint32 mwPlyCalcWorkCprmSfd(MWS_PLY_CPRM_SFD *parameters);
Sint32 mwPlyCalcWorkSofdec(Sint32 type, Sint32 bps, Sint32 width, Sint32 height,
                           Sint32 frames);
MWPLY mwPlyCreateSofdec(MWS_PLY_CPRM_SFD *parameters);
typedef void (*mwsfd_effect_fn)(void *user, Sint32 frame, Sint32 time,
                                Sint32 scale);
struct sfd_handle;
struct sfd_play_info;
typedef void (*mwsfd_picture_fn)(struct sfd_handle *decoder, Sint32 coding_type,
                                 struct sfd_play_info *counts);

void mwPlySetVertexBuffer(void *buffer);
Sint32 mwPlyCalcCompoBufSize(Sint32 width, Sint32 height);
Sint32 mwPlyGetMskFrm(MWPLY handle, MWS_PLY_MVFRM *frame);
void mwPlySetAsync(MWPLY handle, Bool enabled);
SJ mwPlyGetInputSj(MWPLY handle);
void mwPlyEntryFxCb(MWPLY handle, mwsfd_effect_fn effect, void *user);
void mwPlyLinkStm(MWPLY handle, Bool enabled);
Sint32 mwPlyGetNumDecPool(MWPLY handle);
Sint32 mwPlyGetNumTotalDec(MWPLY handle);
Sint32 mwPlyGetNumTotalSkip(MWPLY handle);
Sint32 mwPlyGetNumIbufRoom(MWPLY handle);
Sint32 mwPlyGetNumDispWait(MWPLY handle);
Sint32 mwPlyGetNumRend(MWPLY handle);
void *mwPlyGetRnvImg(MWPLY handle);
void *mwPlyGetRnvMsk(MWPLY handle);
Sint32 mwPlyGetNumLoadFrm(MWPLY handle);
void mwPlySetFastHalfpel(MWPLY handle, Bool enabled);
void mwPlySetAdjustY16(MWPLY handle, Bool enabled);
void mwPlySetMat(MWPLY handle, const Uint8 *mat);
void mwPlySetDispSize(MWPLY handle, Float width, Float height);
void mwPlySetBrightOfst(MWPLY handle, Sint32 value);
Sint32 mwPlyGetBright(MWPLY handle);
Sint32 mwPlyGetBrightOfst(MWPLY handle);
Float mwPlyGetDispZ(MWPLY handle);
void mwPlySetIpicOnly(MWPLY handle, Bool enabled);
void mwPlySetNeverSkip(MWPLY handle, Bool enabled);
void mwPlyEntryDecExecCb(MWPLY handle, mwsfd_picture_fn decoded);
void mwPlyEntryDecSkipCb(MWPLY handle, mwsfd_picture_fn skipped);
Sint32 mwPlyGetNumDropFrm(MWPLY handle);
Bool mwPlyGetVideoSw(MWPLY handle);
void mwPlySetAlphTbl(MWPLY handle, Sint32 bank, const Float alpha[256]);
void mwPlySetDispPos(MWPLY handle, Float x, Float y);
void mwPlySetDispZ(MWPLY handle, Float z);
void mwPlySetCompoMode(MWPLY handle, Sint32 mode);
void mwPlySetVideoSw(MWPLY handle, Sint32 enabled);
void mwPlySetAudioSw(MWPLY handle, Sint32 enabled);
void mwPlySetBright(MWPLY handle, Sint32 value);
Sint32 mwPlyGetMvFrm(MWPLY handle, MWS_PLY_MVFRM *frame);
void mwPlyExecTexSvr(void);
void mwPlyExecDrawSvr(void);
#endif
