/* middleware ring-buffer playback, one stream joint per channel */
#ifndef MW_RNA_H
#define MW_RNA_H
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>

/* the sound output reads one joint per channel and transfers the samples
 * to a sound buffer port. ADXT rebinds the joints when it identifies the
 * stream as ADX or AHX */
struct _adxrna_obj {
    Uint8 used;
    Uint8 stat;
    Uint8 maxnch;
    Uint8 nch;
    Uint8 ovrflw;
    Uint8 copying;
    Uint8 flush;
    Uint8 pause;
    Sint32 bps;
    Sint32 sfreq;
    SJ sj[2];
    SJCK ck[2];
    Sint32 bufsize; /* ring size in samples per channel */
    Sint32 wrpos;
    Sint32 rdpos;
    Sint32 ndata; /* samples written and awaiting playback */
    struct mw_sb *snd;
    Sint32 trnsnsmpl; /* samples in the current transfer */
    Sint32 stopsmpl;
    Sint32 startsmpl;
    Sint32 nplay;
    Sint32 ntotal;
    Sint32 flshpos;
    Sint32 nflsh;
    Sint32 flshnsmpl;
    Sint32 trnsing;
    Sint32 outvol;
    Sint32 outpan[2];
    Sint32 fxch;
    Sint32 fxlev;
    Sint32 semi;
    Sint32 fine;
    Sint32 coff;
    Sint32 q;
    Sint32 drctlvl;
    Sint32 swsave; /* stat's transfer and play bits, as mwRnaPause found them */
    Sint32 hold;
};

/* the bits of stat, and the places of the play and auto bits. mwRnaGetStat
 * returns stat, or -1 for a null rna. mwRnaStart sets MW_RNA_SW_AUTO. with
 * stat MW_RNA_SW_AUTO and MW_RNA_SW_TRANSFER alone and pause clear,
 * mwRnaExecServer turns play on once ndata reaches stopsmpl. with stat
 * MW_RNA_SW_AUTO and MW_RNA_SW_PLAY alone, it turns play off once ndata
 * reaches 0 */
#define MW_RNA_SW_TRANSFER 1
#define MW_RNA_PLAY_SHIFT 1
#define MW_RNA_SW_PLAY (1 << MW_RNA_PLAY_SHIFT)
#define MW_RNA_AUTO_SHIFT 2
#define MW_RNA_SW_AUTO (1 << MW_RNA_AUTO_SHIFT)

/* a sound port plays one or two channels. start enables automatic playback
 * once the buffered sample count reaches the chosen threshold */
void mwRnaInit(void);
void mwRnaFinish(void);
ADXRNA mwRnaCreate(SJ *joints, Sint32 channels);
void mwRnaDestroy(ADXRNA rna);
void mwRnaStart(ADXRNA rna);
void mwRnaStop(ADXRNA rna);
void mwRnaSetPlaySw(ADXRNA rna, Sint32 enabled);
void mwRnaSetTransSw(ADXRNA rna, Sint32 enabled);
void mwRnaExecServer(void);
void mwRnaAdjustBuf(ADXRNA rna);
Sint32 mwRnaDiscardData(ADXRNA rna, Sint32 samples);
void mwRnaGetTime(ADXRNA rna, Sint32 *count, Sint32 *scale);
Sint32 mwRnaGetStat(ADXRNA rna);
Sint32 mwRnaGetNumData(ADXRNA rna);
void mwRnaSetStartSmpl(ADXRNA rna, Sint32 samples);
void mwRnaSetNumChan(ADXRNA rna, Sint32 channels);
void mwRnaSetBitPerSmpl(ADXRNA rna, Sint32 bits);
Sint32 mwRnaGetBitPerSmpl(ADXRNA rna);
void mwRnaSetSfreq(ADXRNA rna, Sint32 rate);
Sint32 mwRnaGetSfreq(ADXRNA rna);
void mwRnaSetOutVol(ADXRNA rna, Sint32 volume);
Sint32 mwRnaGetOutVol(ADXRNA rna);
void mwRnaSetOutPan(ADXRNA rna, Sint32 channel, Sint32 pan);
Sint32 mwRnaGetOutPan(ADXRNA rna, Sint32 channel);
void mwRnaSetFx(ADXRNA rna, Sint32 channel, Sint32 level);
void mwRnaGetFx(ADXRNA rna, Sint32 *channel, Sint32 *level);
void mwRnaSetPitch(ADXRNA rna, Sint32 semitones, Sint32 cents);
void mwRnaGetPitch(ADXRNA rna, Sint32 *semitones, Sint32 *cents);
void mwRnaSetDrctLvl(ADXRNA rna, Sint32 level);
Sint32 mwRnaGetDrctLvl(ADXRNA rna);
void mwRnaSetFilter(ADXRNA rna, Sint32 cutoff, Sint32 resonance);
void mwRnaGetFilter(ADXRNA rna, Sint32 *cutoff, Sint32 *resonance);
void mwRnaSetStmHdInfo(ADXRNA rna, char *header, Sint32 *count);

void mwRnaInitCs(void);
void mwRnaFinishCs(void);
/* save the interrupt mask in *mask, and mask every interrupt */
void mwRnaLockCs(Sint32 *mask);
/* set the interrupt mask to *mask again */
void mwRnaUnlockCs(const Sint32 *mask);

/* the room for the sound output's error text, with its terminator */
#define MW_RNA_ERR_MSG_LEN 256

/* each registration clears the text, and a null function disables reporting */
typedef void (*MW_RNA_ERR_FUNC)(void *obj, char *message);
void mwRnaEntryErrFunc(MW_RNA_ERR_FUNC func, void *obj);
void mwRnaErrEntryFunc(MW_RNA_ERR_FUNC func, void *obj);

/* format an error's text as sprintf does, into the sound output's error
 * text. pass the text to the function mwRnaErrEntryFunc entered, if any. the
 * text and its terminator must fit: nothing tests that they do */
void mwRnaErrCallFunc(const char *format, ...);

#endif
