/* CRI ADX's sound output (ADXRNA), which passes its calls on to mw_rna, the
 * middleware's sound output */
#include <adx_rnas.h>
#include <cri_adxt.h>
#include <mw_rna.h>
#include <sg_xpt.h>
#include <sj.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* each passes its arguments on to the mwRna call of its name */
void ADXRNA_Start(ADXRNA rna);
void ADXRNA_Stop(ADXRNA rna);
Sint32 ADXRNA_GetStat(ADXRNA rna);
void ADXRNA_SetStartSmpl(ADXRNA rna, Sint32 smpl);
Sint32 ADXRNA_GetSfreq(ADXRNA rna);
Sint32 ADXRNA_GetOutVol(ADXRNA rna);
Sint32 ADXRNA_GetOutPan(ADXRNA rna, Sint32 ch);
Sint32 ADXRNA_GetBitPerSmpl(ADXRNA rna);
/* these three do nothing */
void ADXRNA_ClearBuf(ADXRNA rna);
void ADXRNA_SetPcmType(ADXRNA rna, Sint32 type);
void ADXRNA_SetWavFname(ADXRNA rna, char *fname);
/* NOLINTEND(misc-use-internal-linkage) */

/* the room ADXRNA_GetNumRoom reports whatever the output holds */
#define NUM_ROOM_UNLIMITED 0x7fffffff

void ADXRNA_Init(void)
{
    mwRnaInit();
}

void ADXRNA_Finish(void)
{
    mwRnaFinish();
}

ADXRNA ADXRNA_Create(SJ *sjo, Sint32 maxnch)
{
    return mwRnaCreate(sjo, maxnch);
}

void ADXRNA_Destroy(ADXRNA rna)
{
    mwRnaStop(rna);
    mwRnaDestroy(rna);
}

void ADXRNA_Start(ADXRNA rna)
{
    mwRnaStart(rna);
}

void ADXRNA_Stop(ADXRNA rna)
{
    mwRnaStop(rna);
}

void ADXRNA_SetTransSw(ADXRNA rna, Sint32 sw)
{
    mwRnaSetTransSw(rna, sw);
}

void ADXRNA_SetPlaySw(ADXRNA rna, Sint32 sw)
{
    mwRnaSetPlaySw(rna, sw);
}

void ADXRNA_GetTime(ADXRNA rna, Sint32 *ncount, Sint32 *tscale)
{
    mwRnaGetTime(rna, ncount, tscale);
}

Sint32 ADXRNA_GetNumData(ADXRNA rna)
{
    return mwRnaGetNumData(rna);
}

Sint32 ADXRNA_GetNumRoom(ADXRNA rna)
{
    return NUM_ROOM_UNLIMITED;
}

Sint32 ADXRNA_GetStat(ADXRNA rna)
{
    return mwRnaGetStat(rna);
}

void ADXRNA_ExecServer(void)
{
    mwRnaExecServer();
}

void ADXRNA_SetStartSmpl(ADXRNA rna, Sint32 smpl)
{
    mwRnaSetStartSmpl(rna, smpl);
}

void ADXRNA_SetNumChan(ADXRNA rna, Sint32 nch)
{
    mwRnaSetNumChan(rna, nch);
}

void ADXRNA_SetSfreq(ADXRNA rna, Sint32 sfreq)
{
    mwRnaSetSfreq(rna, sfreq);
}

void ADXRNA_SetOutVol(ADXRNA rna, Sint32 vol)
{
    mwRnaSetOutVol(rna, vol);
}

void ADXRNA_SetOutPan(ADXRNA rna, Sint32 ch, Sint32 pan)
{
    mwRnaSetOutPan(rna, ch, pan);
}

void ADXRNA_SetBitPerSmpl(ADXRNA rna, Sint32 bps)
{
    mwRnaSetBitPerSmpl(rna, bps);
}

Sint32 ADXRNA_GetSfreq(ADXRNA rna)
{
    return mwRnaGetSfreq(rna);
}

Sint32 ADXRNA_GetOutVol(ADXRNA rna)
{
    return mwRnaGetOutVol(rna);
}

Sint32 ADXRNA_GetOutPan(ADXRNA rna, Sint32 ch)
{
    return mwRnaGetOutPan(rna, ch);
}

Sint32 ADXRNA_GetBitPerSmpl(ADXRNA rna)
{
    return mwRnaGetBitPerSmpl(rna);
}

Sint32 ADXRNA_DiscardData(ADXRNA rna, Sint32 nsmpl)
{
    return mwRnaDiscardData(rna, nsmpl);
}

void ADXRNA_ClearBuf(ADXRNA rna)
{
}

void ADXRNA_SetPcmType(ADXRNA rna, Sint32 type)
{
}

void ADXRNA_SetTotalNumSmpl(ADXRNA rna, Sint32 nsmpl)
{
}

void ADXRNA_SetWavFname(ADXRNA rna, char *fname)
{
}

Sint32 ADXRNA_SetStmHdInfo(ADXRNA rna, char *hdinfo)
{
    Sint32 nset;

    mwRnaSetStmHdInfo(rna, hdinfo, &nset);
    return nset;
}
