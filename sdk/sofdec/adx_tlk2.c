/* CRI ADX's starts that open their own input. a start plays a file of an
 * ADXF partition, a file by name, or ADX data in memory */
#include <adx_crs.h>
#include <adx_errs.h>
#include <adx_stmc.h>
#include <adx_tlk.h>
#include <cri_adxf.h>
#include <cri_adxt.h>
#include <sg_xpt.h>
#include <sj.h>

/* the size of the buffer ADXT_StartAfs formats patid and fid into */
#define ITOA_SIZE 16
/* a word with its bytes reversed, as a Sint32. adx_bendian.h's ADX_SWAP32
 * gives a Uint32, which would make the test of no unsigned */
#define SWAP32(w)                                                              \
    ((((w) >> 24) & 0xff) | (((w) >> 8) & 0xff00) | (((w) << 8) & 0xff0000) |  \
     ((w) << 24))
/* ACX data, a set of ADX data, opens with a table of big-endian words. a
 * head of ACX_ENTRY_WORDS words comes first, and its word ACX_COUNT holds the
 * count of entries. each entry then takes as many words, the first its ADX
 * data's offset from the set's start */
#define ACX_COUNT 1
#define ACX_ENTRY_WORDS 2
#define ACX_OFST(no) (((no) + 1) * ACX_ENTRY_WORDS)

/* a string that names ADXT's version and build time. the member exports the
 * pointer, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *adxt_build = "\nADXT Ver.5.99M Build:Apr 17 2001 18:11:14\n";
/* NOLINTEND(misc-use-internal-linkage) */

/* the name of the archive ADXT_StartAfs plays from */
static char adxt_afs_fname[ADXF_FNAME_MAX];

void ADXT_StartAfs(ADXT adxt, Sint32 patid, Sint32 fid)
{
    void *dir;
    Sint32 ofst;
    Sint32 fnsct;
    char str[ITOA_SIZE];
    /* @hack spelling: ret, a local for ADXF_GetFnameRangeEx's result, which
     * the test reads once, where the call in the test would do */
    Sint32 ret;

    ADXT_Stop(adxt);
    ADXCRS_Lock();
    ret = ADXF_GetFnameRangeEx(patid, fid, adxt_afs_fname, &dir, &ofst, &fnsct);
    if (ret == ADXF_ERR_OK) {
        adxt->stm = ADXSTM_OpenFileRangeExRt(adxt_afs_fname, dir, ofst, fnsct,
                                             adxt->sjf);
        if (adxt->stm == NULL) {
            /* @bug logic: unlocks twice, as cri_adxt.h says (JSR to
             * ADXCRS_Unlock at 0x8c130c06, and again at 0x8c130c34) */
            ADXCRS_Unlock();
            ADXERR_ItoA2(patid, fid, str, ITOA_SIZE);
            ADXERR_CallErrFunc2("E8101202 ADXT_StartAfs: can't open ", str);
        } else {
            adxt_start_stm(adxt);
            adxt->lnkflg = FALSE;
        }
    }
    ADXCRS_Unlock();
}

void ADXT_StartFname(ADXT adxt, char *fname)
{
    ADXT_Stop(adxt);
    ADXCRS_Lock();
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxt->stm = ADXSTM_OpenFname(fname, adxt->sjf)) == NULL) {
        /* @bug logic: unlocks twice, as cri_adxt.h says (JSR to
         * ADXCRS_Unlock at 0x8c130c66, and JMP at 0x8c130c88) */
        ADXCRS_Unlock();
        ADXERR_CallErrFunc2("E8101205 ADXT_StartFname: can't open ", fname);
    } else {
        adxt_start_stm(adxt);
        adxt->lnkflg = FALSE;
    }
    ADXCRS_Unlock();
}

void ADXT_StartMem(ADXT adxt, void *adxdat)
{
    ADXT_StartMem2(adxt, adxdat, ADXT_MAX_DATASIZE);
}

void ADXT_StartMem2(ADXT adxt, void *adxdat, Sint32 datlen)
{
    SJ sj;

    ADXT_Stop(adxt);
    ADXCRS_Lock();
    sj = SJMEM_Create(adxdat, datlen);
    if (sj == NULL) {
        /* @bug logic: unlocks twice, as cri_adxt.h says (JSR to
         * ADXCRS_Unlock at 0x8c130cb6, and JMP at 0x8c130d1e) */
        ADXCRS_Unlock();
        ADXERR_CallErrFunc1("E8101207: can't create sj (ADXT_StartMem)\n");
    } else {
        adxt_start_sj(adxt, sj);
        adxt->pmode = ADXT_PMODE_MEM;
        adxt->lnkflg = FALSE;
    }
    ADXCRS_Unlock();
}

void ADXT_StartMemIdx(ADXT adxt, void *acx, Sint32 no)
{
    /* @hack spelling: word, acx as an array of words, where a record of the
     * set's head and entries would do */
    Sint32 *word = acx;
    /* @hack spelling: top, acx as bytes in a local, where acx cast in the
     * sum would do */
    char *top = acx;
    Sint32 ofst;
    /* @hack spelling: adr, a local for the entry's address, where the sum
     * passed to SJMEM_Create would do */
    char *adr;
    SJ sj;

    ADXT_Stop(adxt);
    /* @hack spelling: no's test against the count before its test for a
     * negative, where the other order would do */
    if (no >= SWAP32(word[ACX_COUNT]) || no < 0) {
        return;
    }
    ADXCRS_Lock();
    ofst = SWAP32(word[ACX_OFST(no)]);
    adr = top + ofst;
    sj = SJMEM_Create(adr, ADXT_MAX_DATASIZE);
    if (sj == NULL) {
        /* @bug logic: unlocks twice, as cri_adxt.h says (JSR to
         * ADXCRS_Unlock at 0x8c130db8, and JMP at 0x8c130de2) */
        ADXCRS_Unlock();
        ADXERR_CallErrFunc1("E8101207: can't create sj (ADXT_StartMemIdx)");
    } else {
        adxt_start_sj(adxt, sj);
        adxt->pmode = ADXT_PMODE_MEM;
        adxt->lnkflg = FALSE;
    }
    ADXCRS_Unlock();
}
