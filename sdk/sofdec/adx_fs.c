/* CRI's ADX file system: handles that read files of the disc by sectors, and
 * partitions that list the files of AFS archives */
#include <adx_crs.h>
#include <adx_errs.h>
#include <adx_fcch.h>
#include <adx_fini.h>
#include <adx_stmc.h>
#include <cri_adxf.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <string.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void adxf_SetCmdHstry(Sint32 cmdid, Sint32 fg, Sint32 prm1, Sint32 prm2,
                      Sint32 prm3);
void adxf_wait_1ms(void);
Sint32 adxf_ChkPrmPt(Sint32 ptid, ADXF_PTINFO *ptinfo);
Sint32 adxf_LoadData(ADXF adxf, Sint32 nsct, void *buf);
void adxf_CloseLdptnwHn(void);
void adxf_stop_partition_load(void);
ADXF adxf_AllocAdxFs(void);
ADXF adxf_CreateAdxFs(void);
Sint32 adxf_SetFileInfo(ADXF adxf, char *fname);
Sint32 adxf_SetFileInfoEx(ADXF adxf, char *fname, void *dir);
Sint32 adxf_ChkPrmGfr(Sint32 ptid, Sint32 flid);
Sint32 adxf_SetAfsFileInfo(ADXF adxf, Sint32 ptid, Sint32 flid);
void adxf_CloseSjStm(ADXF adxf);
Sint32 adxf_read_sj32(ADXF adxf, Sint32 nsct, SJ sj);
void adxf_ExecOne(ADXF adxf);
void ADXF_SetOcbiSw(Sint32 sw);
/* NOLINTEND(misc-use-internal-linkage) */

/* an AFS archive's header, in words. it starts with the text AFS_MAGIC. the
 * word at AFS_NFILE holds the file count. from AFS_FILES on, a pair of
 * AFS_PAIR words gives each file's offset and size in bytes. the size lies
 * at AFS_SIZE in its pair. a sector holds SCT_WORDS words, an even count,
 * and the first pair starts at an even word. so every sector starts with a
 * pair, and its first size lies at word AFS_SIZE */
#define AFS_MAGIC "AFS"
#define AFS_NFILE 1
#define AFS_FILES 2
#define AFS_PAIR 2
#define AFS_SIZE 1
#define SCT_WORDS (ADXF_DEF_SCT_SIZE / 4)
/* a partition load gives up after LOAD_RETRIES failed reads in a row */
#define LOAD_RETRIES 3
/* adxf_LoadData makes at most LOAD_PASSES passes while its read won't start.
 * it makes as many again while it waits for the read to end. a pass that
 * doesn't end its loop runs gdFsExecServer, ADXSTM_ExecServer and
 * ADXF_ExecServer, then waits in adxf_wait_1ms */
#define LOAD_PASSES 10000
/* the turns of adxf_wait_1ms's empty loop */
#define WAIT_TURNS 50000
/* the bytes of the buffer adxf_SetFileInfoEx gives its stream. the stream
 * only measures the file, and closes before it starts to read */
#define MEASURE_BUF_SIZE 32

/* the partition loads' sector buffer. a load reads one sector into it at
 * ALIGNED_SCT_BUF. the sector and its alignment take at most a quarter of
 * the array */
static Sint32 adxf_sct_buf[ADXF_DEF_SCT_SIZE + ADXF_DEF_DMA_ALIGN];
/* ALIGNED_SCT_BUF, as each load stores it. only the load's store into
 * adxf_sct reads it */
static Sint32 *adxf_sct_base;
/* the sector a load reads, at ALIGNED_SCT_BUF */
static Sint32 *adxf_sct;
/* the first ADXF_DEF_DMA_ALIGN boundary after adxf_sct_buf's start */
#define ALIGNED_SCT_BUF                                                        \
    ((Sint32 *)(((Uint32)adxf_sct_buf + ADXF_DEF_DMA_ALIGN) &                  \
                ~(ADXF_DEF_DMA_ALIGN - 1)))

/* log a call of command cmdid in the command history, with its parameters
 * prm1 to prm3. fg is ADXF_HSTRY_ENTRY as the call starts, which counts it in
 * adxf_cmd_ncall, or ADXF_HSTRY_EXIT as it returns */
void adxf_SetCmdHstry(Sint32 cmdid, Sint32 fg, Sint32 prm1, Sint32 prm2,
                      Sint32 prm3)
{
    ADXF_CMD_HSTRY *hstry;

    adxf_hstry_no %= ADXF_CMD_HSTRY_MAX;
    hstry = &adxf_cmd_hstry[adxf_hstry_no];
    if (fg == ADXF_HSTRY_ENTRY) {
        adxf_cmd_ncall[cmdid]++;
    }
    hstry->cmdid = cmdid;
    hstry->fg = fg;
    hstry->ncall = adxf_cmd_ncall[cmdid];
    hstry->prm[0] = prm1;
    hstry->prm[1] = prm2;
    hstry->prm[2] = prm3;
    adxf_hstry_no++;
}

void adxf_wait_1ms(void)
{
    Sint32 i;

    for (i = 0; i < WAIT_TURNS; i++) {
    }
}

/* ADXF_ERR_OK, or ADXF_ERR_PRM, with a report, for a ptid outside the
 * partitions or a null ptinfo */
Sint32 adxf_ChkPrmPt(Sint32 ptid, ADXF_PTINFO *ptinfo)
{
    if (ptid < 0 || ptid >= ADXF_PART_MAX) {
        ADXERR_CallErrFunc1("E9040801:'ptid' is range outside.(adxf_ChkPrmPt)");
        return ADXF_ERR_PRM;
    }
    if (ptinfo == NULL) {
        ADXERR_CallErrFunc1("E9040802:'ptinfo' is NULL.(adxf_ChkPrmPt)");
        return ADXF_ERR_PRM;
    }
    return ADXF_ERR_OK;
}

/* read nsct sectors at the read position into buf, and wait for the read to
 * end. it returns the sectors read, or 0 where the read won't start. a read
 * that fails holds the wait for all LOAD_PASSES passes */
Sint32 adxf_LoadData(ADXF adxf, Sint32 nsct, void *buf)
{
    Sint32 i;
    /* @hack spelling: n, a local for the read's result, which the test reads
     * once, where the call in the test would do */
    Sint32 n;

    for (i = 0; i < LOAD_PASSES; i++) {
        n = ADXF_ReadNw32(adxf, nsct, buf);
        if (n > 0) {
            break;
        }
        gdFsExecServer();
        ADXSTM_ExecServer();
        ADXF_ExecServer();
        adxf_wait_1ms();
    }
    if (i == LOAD_PASSES) {
        return 0;
    }
    for (i = 0; i < LOAD_PASSES; i++) {
        gdFsExecServer();
        ADXSTM_ExecServer();
        ADXF_ExecServer();
        if (ADXF_GetStat(adxf) == ADXF_STAT_READEND) {
            break;
        }
        adxf_wait_1ms();
    }
    /* @bug logic: leaves a read that outlasts the wait running, with no
     * ADXF_Stop before the tail call (JMP @R2 at 0x8c131176). the partition
     * load counts that read as failed, and asks again. its next read starts
     * once the late one ends, at the sector after it. the load then lists
     * that sector's files in the late one's place */
    return ADXF_GetNumReadSct(adxf);
}

Sint32 ADXF_LoadPartition(Sint32 ptid, char *fname, void *ptinfo, Sint32 nfile)
{
    ADXF_PTINFO *pt;
    ADXF adxf;
    Sint32 ret;
    Uint16 *first;
    Uint16 *sizes;
    Sint32 i;
    Sint32 j;
    /* @hack spelling: nword, a local for SCT_WORDS, which the loop tests,
     * where SCT_WORDS in the test would do */
    Sint32 nword;
    Sint32 retry;

    ret = adxf_ChkPrmPt(ptid, ptinfo);
    if (ret < 0) {
        return ret;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxf = ADXF_Open(fname, NULL)) == NULL) {
        ADXERR_CallErrFunc1("E9040804:can't open file.(ADXF_LoadPartition)");
        return ADXF_ERR_FATAL;
    }
    ret = ADXF_ERR_FATAL;
    memset(ptinfo, 0, sizeof(ADXF_PTINFO));
    pt = ptinfo;
    adxf_ptinfo[ptid] = pt;
    pt->next = NULL;
    strncpy(pt->fname, fname, ADXF_FNAME_MAX);
    pt->type = ADXF_PT_LOADED;
    first = (Uint16 *)&pt->top;
    sizes = first + 1;
    adxf_sct_base = ALIGNED_SCT_BUF;
    adxf_sct = adxf_sct_base;
    i = 0;
    pt->nfile = 0;
    retry = 0;
    while (retry < LOAD_RETRIES) {
        if (adxf_LoadData(adxf, 1, adxf_sct) > 0) {
            if (pt->nfile == 0) {
                if (adxf_sct[AFS_NFILE] > ADXF_FILE_MAX) {
                    adxf_sct[AFS_NFILE] = ADXF_FILE_MAX;
                    ret = ADXF_ERR_AFS_FILE;
                    goto end;
                }
                /* @hack spelling: nfile and nentry set in one statement,
                 * where pt->nentry = ... and then pt->nfile = pt->nentry
                 * would do */
                /* @bug arithmetic: an archive of ADXF_FILE_MAX files lists
                 * none, unless nfile is smaller. pt->nfile takes the count
                 * as stored in nentry, a Uint16, which holds ADXF_FILE_MAX
                 * as 0 (EXTU.W R0,R0 at 0x8c131280) */
                pt->nfile = pt->nentry =
                    (nfile < adxf_sct[AFS_NFILE]) ? nfile : adxf_sct[AFS_NFILE];
                pt->size = ADXF_CALC_PTINFO_SIZE(pt->nfile);
                *first = adxf_sct[AFS_FILES] / ADXF_DEF_SCT_SIZE;
                j = AFS_FILES + AFS_SIZE;
            } else {
                j = AFS_SIZE;
            }
            nword = SCT_WORDS;
            for (; j < nword; j += AFS_PAIR) {
                sizes[i++] =
                    (adxf_sct[j] + ADXF_DEF_SCT_SIZE - 1) / ADXF_DEF_SCT_SIZE;
                if (i >= pt->nfile) {
                    ret = ADXF_ERR_OK;
                    goto end;
                }
            }
            retry = 0;
        } else {
            retry++;
        }
    }
    /* @hack redundant: the test of retry, which always holds here */
    if (retry == LOAD_RETRIES) {
        ADXERR_CallErrFunc1("E9040805:read error.(ADXF_LoadPartition)");
    }
end:
    if (ret == ADXF_ERR_AFS_FILE) {
        ADXERR_CallErrFunc1("E9040806:file is broken.(ADXF_LoadPartition)");
    }
    ADXF_Close(adxf);
    return ret;
}

Sint32 ADXF_LoadPartitionEx(Sint32 ptid, char *fname, void *dir, void *ptinfo)
{
    ADXF_PTINFO *pt;
    ADXF adxf;
    Sint32 ret;
    Uint16 *first;
    Uint16 *sizes;
    Sint32 i;
    Sint32 j;
    /* @hack spelling: nword, a local for SCT_WORDS, which the loop tests,
     * where SCT_WORDS in the test would do */
    Sint32 nword;
    Sint32 retry;

    ret = adxf_ChkPrmPt(ptid, ptinfo);
    if (ret < 0) {
        return ret;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxf = ADXF_Open(fname, dir)) == NULL) {
        /* @bug logic: the report names ADXF_LoadPartition (MOV.L of the
         * message at 0x8c13135c) */
        ADXERR_CallErrFunc1("E9040804:can't open file.(ADXF_LoadPartition)");
        return ADXF_ERR_FATAL;
    }
    ret = ADXF_ERR_FATAL;
    memset(ptinfo, 0, sizeof(ADXF_PTINFO));
    /* @hack spelling: pt and the partition's slot set in one statement,
     * where pt = ptinfo and then adxf_ptinfo[ptid] = pt would do */
    pt = adxf_ptinfo[ptid] = ptinfo;
    pt->next = NULL;
    strncpy(pt->fname, fname, ADXF_FNAME_MAX);
    pt->curdir = dir;
    pt->type = ADXF_PT_LOADED;
    first = (Uint16 *)&pt->top;
    sizes = first + 1;
    adxf_sct_base = ALIGNED_SCT_BUF;
    adxf_sct = adxf_sct_base;
    i = 0;
    pt->nfile = 0;
    retry = 0;
    while (retry < LOAD_RETRIES) {
        if (adxf_LoadData(adxf, 1, adxf_sct) > 0) {
            if (pt->nfile == 0) {
                if (adxf_sct[AFS_NFILE] > ADXF_FILE_MAX) {
                    adxf_sct[AFS_NFILE] = ADXF_FILE_MAX;
                    ret = ADXF_ERR_AFS_FILE;
                    /* @bug logic: the report names ADXF_LoadPartition (MOV.L
                     * of the message at 0x8c1313f0) */
                    ADXERR_CallErrFunc1(
                        "E9040806:file is broken.(ADXF_LoadPartition)");
                    goto end;
                }
                /* @hack spelling: nfile and nentry set in one statement,
                 * where pt->nentry = ... and then pt->nfile = pt->nentry
                 * would do */
                /* @bug arithmetic: an archive of ADXF_FILE_MAX files lists
                 * none, as in ADXF_LoadPartition (EXTU.W R0,R0 at
                 * 0x8c131406) */
                pt->nfile = pt->nentry = adxf_sct[AFS_NFILE];
                pt->size = ADXF_CALC_PTINFO_SIZE(pt->nfile);
                *first = adxf_sct[AFS_FILES] / ADXF_DEF_SCT_SIZE;
                j = AFS_FILES + AFS_SIZE;
            } else {
                j = AFS_SIZE;
            }
            nword = SCT_WORDS;
            for (; j < nword; j += AFS_PAIR) {
                sizes[i++] =
                    (adxf_sct[j] + ADXF_DEF_SCT_SIZE - 1) / ADXF_DEF_SCT_SIZE;
                if (i >= pt->nfile) {
                    ret = ADXF_ERR_OK;
                    goto end;
                }
            }
            retry = 0;
        } else {
            retry++;
        }
    }
    /* @hack redundant: the test of retry, which always holds here */
    if (retry == LOAD_RETRIES) {
        /* @bug logic: the report names ADXF_LoadPartition (MOV.L of the
         * message at 0x8c1314a8) */
        ADXERR_CallErrFunc1("E9040805:read error.(ADXF_LoadPartition)");
    }
end:
    ADXF_Close(adxf);
    return ret;
}

Sint32 ADXF_LoadPartitionNw(Sint32 ptid, char *fname, void *dir, void *ptinfo)
{
    ADXF_PTINFO *pt;
    Sint32 ret;
    /* @hack spelling: err, a second local for the first read's result, where
     * ret would do */
    Sint32 err;

    ret = adxf_ChkPrmPt(ptid, ptinfo);
    if (ret < 0) {
        return ret;
    }
    if (adxf_ldptnw_ptid >= 0) {
        ADXERR_CallErrFunc1(
            "E0042401:multi-load partition.(ADXF_LoadPartitionNw)");
        return ADXF_ERR_FATAL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxf_ldptnw_hn = ADXF_Open(fname, dir)) == NULL) {
        ADXERR_CallErrFunc1("E9040804:can't open file.(ADXF_LoaddPartitionNw)");
        return ADXF_ERR_FATAL;
    }
    adxf_ldptnw_ptid = ptid;
    adxf_flno = 0;
    memset(ptinfo, 0, sizeof(ADXF_PTINFO));
    adxf_ptinfo[ptid] = ptinfo;
    pt = ptinfo;
    pt->next = NULL;
    strncpy(pt->fname, fname, ADXF_FNAME_MAX);
    pt->curdir = dir;
    pt->type = ADXF_PT_LOADED;
    adxf_sct_base = ALIGNED_SCT_BUF;
    adxf_sct = adxf_sct_base;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((err = ADXF_ReadNw32(adxf_ldptnw_hn, 1, adxf_sct)) < 0) {
        /* @bug logic: frees the load's handle without ending the load. the
         * BSR to ADXF_Close at 0x8c131562 leaves the handle and ptid in
         * adxf_ldptnw_hn and adxf_ldptnw_ptid. adxf_CloseLdptnwHn would
         * clear both. nothing in the game calls ADXF_LoadPartitionNw */
        ADXF_Close(adxf_ldptnw_hn);
        return err;
    }
    pt->nfile = 0;
    return ADXF_ERR_OK;
}

/* end ADXF_LoadPartitionNw's load, and free its handle */
void adxf_CloseLdptnwHn(void)
{
    ADXF_Close(adxf_ldptnw_hn);
    adxf_ldptnw_hn = NULL;
    adxf_ldptnw_ptid = ADXF_LDPTNW_NONE;
    adxf_flno = 0;
}

/* end ADXF_LoadPartitionNw's load, if one stands, and stop its read first.
 * where the read failed, ADXF_Stop reports an error. nothing in the game
 * calls it */
void adxf_stop_partition_load(void)
{
    /* @hack spelling: two tests, one inside the other, where one test with
     * && would do */
    if (adxf_ldptnw_hn != NULL) {
        if (adxf_ldptnw_ptid >= 0) {
            if (ADXF_GetStat(adxf_ldptnw_hn) != ADXF_STAT_STOP) {
                ADXF_Stop(adxf_ldptnw_hn);
            }
            adxf_CloseLdptnwHn();
        }
    }
}

Sint32 ADXF_GetPtStat(Sint32 ptid)
{
    ADXF_PTINFO *pt;
    Sint32 stat;
    Uint16 *first;
    Uint16 *sizes;
    Sint32 j;

    /* @hack redundant: a division of *adxf_sct that never runs */
    if (0) {
        *adxf_sct /= ADXF_DEF_SCT_SIZE;
    }
    if (ptid != adxf_ldptnw_ptid) {
        ADXERR_CallErrFunc1(
            "E0041303:illigal parameter 'ptid'.(ADXF_GetPtStat)");
        return ADXF_ERR_PRM;
    }
    stat = ADXF_GetStat(adxf_ldptnw_hn);
    if (stat != ADXF_STAT_READEND) {
        return stat;
    }
    pt = adxf_ptinfo[ptid];
    first = (Uint16 *)&pt->top;
    sizes = first + 1;
    /* @hack spelling: j stepped through the header, where the indexes
     * AFS_NFILE and AFS_FILES would do, with j then set to AFS_FILES */
    j = 0;
    if (pt->nfile == 0) {
        if (memcmp(adxf_sct, AFS_MAGIC, sizeof(AFS_MAGIC) - 1) != 0) {
            ADXERR_CallErrFunc1(
                "E0040701:Illigal format(not AFS).(ADXF_GetPtStat)");
            adxf_CloseLdptnwHn();
            return ADXF_STAT_ERROR;
        }
        j++;
        if (adxf_sct[j] > ADXF_FILE_MAX) {
            ADXERR_CallErrFunc1(
                "E0040702:Illigal number of file.(ADXF_GetPtStat)");
            adxf_CloseLdptnwHn();
            return ADXF_STAT_ERROR;
        }
        /* @hack spelling: nfile and nentry set in one statement, where
         * pt->nentry = ... and then pt->nfile = pt->nentry would do */
        /* @bug arithmetic: an archive of ADXF_FILE_MAX files lists none, as
         * in ADXF_LoadPartition (EXTU.W R0,R0 at 0x8c13168a) */
        pt->nfile = pt->nentry = adxf_sct[j];
        pt->size = ADXF_CALC_PTINFO_SIZE(pt->nfile);
        j++;
        *first = adxf_sct[j] / ADXF_DEF_SCT_SIZE;
    }
    /* @hack spelling: j stepped as j = j + AFS_SIZE and j = j + AFS_PAIR,
     * where j += AFS_SIZE and j += AFS_PAIR would do */
    for (j = j + AFS_SIZE; j < SCT_WORDS; j = j + AFS_PAIR) {
        sizes[adxf_flno++] =
            (adxf_sct[j] + ADXF_DEF_SCT_SIZE - 1) / ADXF_DEF_SCT_SIZE;
        if (adxf_flno >= pt->nfile) {
            stat = ADXF_STAT_READEND;
            adxf_CloseLdptnwHn();
            break;
        }
    }
    if (j >= SCT_WORDS) {
        if (ADXF_ReadNw32(adxf_ldptnw_hn, 1, adxf_sct) < 0) {
            stat = ADXF_STAT_ERROR;
            adxf_CloseLdptnwHn();
        } else {
            stat = adxf_ldptnw_hn->stat;
        }
    }
    return stat;
}

Sint32 ADXF_AddPartition(Sint32 ptid, char *fname, void *ptinfo, Sint32 nfile)
{
    ADXF_PTINFO *pt = ptinfo;
    ADXF adxf;
    Sint32 ret;
    ADXF_ADD_INFO *add;
    ADXF_ADD_INFO *entry;
    Sint32 nentry;
    Sint32 flid;
    Sint32 j;
    /* @hack spelling: nword, a local for SCT_WORDS, which the loop tests,
     * where SCT_WORDS in the test would do */
    Sint32 nword;
    Sint32 retry;

    ret = adxf_ChkPrmPt(ptid, ptinfo);
    if (ret < 0) {
        return ret;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((adxf = ADXF_Open(fname, NULL)) == NULL) {
        ADXERR_CallErrFunc1("E9040808:can't open.(ADXF_AddPartition)");
        return ADXF_ERR_FATAL;
    }
    pt->next = adxf_ptinfo[ptid];
    adxf_ptinfo[ptid] = ptinfo;
    ret = ADXF_ERR_FATAL;
    strncpy(pt->fname, fname, ADXF_FNAME_MAX);
    pt->type = ADXF_PT_ADDED;
    add = (ADXF_ADD_INFO *)&pt->top;
    adxf_sct_base = ALIGNED_SCT_BUF;
    adxf_sct = adxf_sct_base;
    flid = 0;
    nentry = 0;
    pt->nfile = 0;
    retry = 0;
    while (retry < LOAD_RETRIES) {
        if (adxf_LoadData(adxf, 1, adxf_sct) > 0) {
            j = 0;
            if (pt->nfile == 0) {
                if (adxf_sct[AFS_NFILE] > ADXF_FILE_MAX) {
                    adxf_sct[AFS_NFILE] = ADXF_FILE_MAX;
                    ret = ADXF_ERR_AFS_FILE;
                    /* @bug logic: the report names ADXF_LoadPartition (MOV.L
                     * of the message at 0x8c131808) */
                    ADXERR_CallErrFunc1(
                        "E9040810:file is broken.(ADXF_LoadPartition)");
                    goto end;
                }
                pt->nfile =
                    (nfile < adxf_sct[AFS_NFILE]) ? nfile : adxf_sct[AFS_NFILE];
                j = AFS_FILES;
            }
            nword = SCT_WORDS;
            while (j < nword) {
                if (adxf_sct[j] == 0) {
                    j += AFS_PAIR;
                } else {
                    entry = &add[nentry++];
                    entry->flid = flid;
                    entry->ofst = (adxf_sct[j++] + ADXF_DEF_SCT_SIZE - 1) /
                                  ADXF_DEF_SCT_SIZE;
                    entry->fnsct = (adxf_sct[j++] + ADXF_DEF_SCT_SIZE - 1) /
                                   ADXF_DEF_SCT_SIZE;
                }
                flid++;
                if (flid >= pt->nfile) {
                    ret = ADXF_ERR_OK;
                    goto end;
                }
            }
            retry = 0;
        } else {
            retry++;
        }
    }
    /* @hack redundant: the test of retry, which always holds here */
    if (retry == LOAD_RETRIES) {
        /* @bug logic: the report names ADXF_LoadPartition (MOV.L of the
         * message at 0x8c1318ce) */
        ADXERR_CallErrFunc1("E9040809:read error.(ADXF_LoadPartition)");
    }
end:
    /* @bug arithmetic: a partition of ADXF_FILE_MAX entries counts none, as
     * ADXF_LoadPartition's nentry does (MOV.W R0,@(12,R13) at 0x8c1318d6).
     * nothing in the game calls ADXF_AddPartition */
    pt->nentry = nentry;
    pt->size = pt->next->size + ADXF_CALC_ADD_PTINFO_SIZE(pt->nentry);
    ADXF_Close(adxf);
    return ret;
}

Sint32 ADXF_GetPtinfoSize(Sint32 ptid)
{
    /* @hack spelling: pt, a local for the partition, which the return reads
     * once, where adxf_ptinfo[ptid]->size in the return would do */
    ADXF_PTINFO *pt;

    pt = adxf_ptinfo[ptid];
    return pt->size;
}

/* a free handle, not yet opened, or null when every handle is open */
ADXF adxf_AllocAdxFs(void)
{
    /* @hack spelling: adxf, a local that starts null and takes the handle
     * the loop finds, where the handle returned from the loop and null after
     * it would do */
    ADXF adxf = NULL;
    Sint32 i;

    for (i = 0; i < ADXF_OBJ_MAX; i++) {
        if (adxf_obj[i].used == FALSE) {
            adxf = &adxf_obj[i];
            break;
        }
    }
    return adxf;
}

/* open a free handle, stopped and without a request. it returns null when
 * every handle is open. the fields it doesn't set keep the zeros of a free
 * handle */
ADXF adxf_CreateAdxFs(void)
{
    ADXF adxf;

    adxf = adxf_AllocAdxFs();
    if (adxf == NULL) {
        return NULL;
    }
    adxf->stat = ADXF_STAT_STOP;
    adxf->stm = NULL;
    adxf->rdstpos = 0;
    adxf->rqsct = 0;
    adxf->rdsct = 0;
    adxf->rqrdsct = ADXF_DEF_REQ_RD_SCT;
    adxf->sjflag = FALSE;
    adxf->sj = NULL;
    adxf->used = TRUE;
    return adxf;
}

/* give the handle the name and sizes of the file fname. it returns
 * ADXF_ERR_OK, ADXF_ERR_FATAL for a null fname, or a size call's negative
 * result. nothing in the game calls it */
Sint32 adxf_SetFileInfo(ADXF adxf, char *fname)
{
    Sint32 fsize;
    Sint32 fnsct;
    Sint32 ret;

    if (fname == NULL) {
        ADXERR_CallErrFunc1(
            "E9081901:illigal parameter fname=null.(ADXF_Open)");
        return ADXF_ERR_FATAL;
    }
    ret = ADXSTM_GetFsizeSct(fname, &fnsct);
    if (ret < 0) {
        ADXERR_CallErrFunc1("E9040902:can't get file size(sct).(ADXF_Open)");
        return ret;
    }
    ret = ADXSTM_GetFsizeByte(fname, &fsize);
    if (ret < 0) {
        ADXERR_CallErrFunc1("E9040903:can't get file size(byte).(ADXF_Open)");
        return ret;
    }
    strncpy(adxf->fname, fname, ADXF_FNAME_MAX);
    adxf->fnsct = fnsct;
    adxf->fsize = fsize;
    return ADXF_ERR_OK;
}

/* give the handle the name, directory record and sizes of the file fname,
 * found in dir. a stream opens on the file only to measure it. it returns
 * ADXF_ERR_OK. it returns ADXF_ERR_FATAL for a null fname, a file that
 * won't open, or no free ring buffer joint */
Sint32 adxf_SetFileInfoEx(ADXF adxf, char *fname, void *dir)
{
    SJ sj;
    ADXSTM stm;
    Sint32 fsize;
    /* @hack spelling: fnsct, a local for the size in sectors, which one
     * store reads, where the division in the store would do */
    Sint32 fnsct;
    char buf[MEASURE_BUF_SIZE];

    if (fname == NULL) {
        ADXERR_CallErrFunc1(
            "E9081901:illigal parameter fname=null.(ADXF_Open)");
        return ADXF_ERR_FATAL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((sj = SJRBF_Create(buf, sizeof(buf), 0)) == NULL) {
        ADXERR_CallErrFunc1("E0041301:can't create SJ.(ADXF_Open)");
        return ADXF_ERR_FATAL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((stm = ADXSTM_OpenFnameEx(fname, dir, sj)) == NULL) {
        ADXERR_CallErrFunc1("E0041201:can't open file.(ADXF_Open)");
        SJ_Destroy(sj);
        return ADXF_ERR_FATAL;
    }
    fsize = ADXSTM_GetFileLen(stm);
    ADXSTM_Close(stm);
    SJ_Destroy(sj);
    fnsct = (fsize + ADXF_DEF_SCT_SIZE - 1) / ADXF_DEF_SCT_SIZE;
    strncpy(adxf->fname, fname, ADXF_FNAME_MAX);
    adxf->dir = dir;
    adxf->fnsct = fnsct;
    adxf->fsize = fsize;
    return ADXF_ERR_OK;
}

ADXF ADXF_Open(char *fname, void *atr)
{
    ADXF adxf;
    /* @hack spelling: ret, a local for adxf_SetFileInfoEx's result, which
     * the test reads once, where the call in the test would do */
    Sint32 ret;

    adxf_SetCmdHstry(ADXF_CMD_OPEN, ADXF_HSTRY_ENTRY, (Sint32)fname,
                     (Sint32)atr, ADXF_HSTRY_NO_PRM);
    ADXCRS_Lock();
    adxf = adxf_CreateAdxFs();
    if (adxf != NULL) {
        ret = adxf_SetFileInfoEx(adxf, fname, atr);
        if (ret < 0) {
            ADXF_Close(adxf);
            adxf = NULL;
        }
    }
    ADXCRS_Unlock();
    adxf_SetCmdHstry(ADXF_CMD_OPEN, ADXF_HSTRY_EXIT, (Sint32)fname, (Sint32)atr,
                     ADXF_HSTRY_NO_PRM);
    return adxf;
}

/* give the handle file flid of partition ptid. the handle takes the
 * archive's name and directory record, and the file's first sector in the
 * archive. a partition keeps sizes only in sectors. so the handle's size in
 * bytes is a whole number of them. it returns ADXF_ERR_OK, or ADXF_ERR_PRM
 * where ADXF_GetFnameRangeEx refuses */
Sint32 adxf_SetAfsFileInfo(ADXF adxf, Sint32 ptid, Sint32 flid)
{
    /* @hack spelling: ret, a local for ADXF_GetFnameRangeEx's result, which
     * the test reads once, where the call in the test would do */
    Sint32 ret;
    Sint32 fnsct;
    void *dir;
    Sint32 ofst;
    char fname[ADXF_FNAME_MAX];

    ret = ADXF_GetFnameRangeEx(ptid, flid, fname, &dir, &ofst, &fnsct);
    if (ret < 0) {
        return ADXF_ERR_PRM;
    }
    strncpy(adxf->fname, fname, ADXF_FNAME_MAX);
    adxf->dir = dir;
    adxf->ofst = ofst;
    adxf->fnsct = fnsct;
    adxf->fsize = fnsct * ADXF_DEF_SCT_SIZE;
    return ADXF_ERR_OK;
}

ADXF ADXF_OpenAfs(Sint32 ptid, Sint32 flid)
{
    ADXF adxf;
    /* @hack spelling: ret, a local for adxf_SetAfsFileInfo's result, which
     * the test reads once, where the call in the test would do */
    Sint32 ret;

    adxf_SetCmdHstry(ADXF_CMD_OPEN_AFS, ADXF_HSTRY_ENTRY, ptid, flid,
                     ADXF_HSTRY_NO_PRM);
    ADXCRS_Lock();
    adxf = adxf_CreateAdxFs();
    if (adxf != NULL) {
        ret = adxf_SetAfsFileInfo(adxf, ptid, flid);
        if (ret < 0) {
            ADXF_Close(adxf);
            adxf = NULL;
        }
    }
    ADXCRS_Unlock();
    adxf_SetCmdHstry(ADXF_CMD_OPEN_AFS, ADXF_HSTRY_EXIT, ptid, flid,
                     ADXF_HSTRY_NO_PRM);
    return adxf;
}

/* close the request's stream, and destroy its joint where the handle made
 * it */
void adxf_CloseSjStm(ADXF adxf)
{
    if (adxf->stm != NULL) {
        ADXSTM_Close(adxf->stm);
        adxf->stm = NULL;
    }
    if (adxf->sj != NULL && adxf->sjflag == FALSE) {
        if (adxf_ocbi_fg == TRUE) {
            ADXF_Ocbi(adxf->buf, adxf->bsize);
        }
        SJ_Destroy(adxf->sj);
        adxf->sj = NULL;
    }
}

void ADXF_Close(ADXF adxf)
{
    adxf_SetCmdHstry(ADXF_CMD_CLOSE, ADXF_HSTRY_ENTRY, (Sint32)adxf,
                     ADXF_HSTRY_NO_PRM, ADXF_HSTRY_NO_PRM);
    if (adxf == NULL) {
        return;
    }
    ADXCRS_Lock();
    if (adxf->stat == ADXF_STAT_READING) {
        ADXF_Stop(adxf);
    }
    memset(adxf, 0, sizeof(ADX_FS));
    ADXCRS_Unlock();
    adxf_SetCmdHstry(ADXF_CMD_CLOSE, ADXF_HSTRY_EXIT, (Sint32)adxf,
                     ADXF_HSTRY_NO_PRM, ADXF_HSTRY_NO_PRM);
}

void ADXF_CloseAll(void)
{
    Sint32 i;

    for (i = 0; i < ADXF_OBJ_MAX; i++) {
        if (adxf_obj[i].used == TRUE) {
            ADXF_Close(&adxf_obj[i]);
        }
    }
}

/* start a request that reads nsct sectors from the read position into sj. it
 * returns the sectors the request asks for, or ADXF_ERR_FATAL */
Sint32 adxf_read_sj32(ADXF adxf, Sint32 nsct, SJ sj)
{
    ADXSTM stm;
    Sint32 ret;

    if (adxf->stm != NULL) {
        ADXERR_CallErrFunc1("E9040815:'adxf->stm' isn't NULL.(adxf_read_sj32)");
        return ADXF_ERR_FATAL;
    }
    ADXCRS_Lock();
    adxf->rdstpos = adxf->ofst + adxf->skpos;
    adxf->rqsct =
        (nsct < adxf->fnsct - adxf->skpos) ? nsct : adxf->fnsct - adxf->skpos;
    adxf->rdsct = 0;
    stm = ADXSTM_OpenFileRangeEx(adxf->fname, adxf->dir, adxf->rdstpos,
                                 adxf->rqsct, sj);
    if (stm == NULL) {
        ADXERR_CallErrFunc1("E0041302:can't open file.(adxf_read_sj32)");
        ret = ADXF_ERR_FATAL;
    } else {
        adxf->stm = stm;
        ADXSTM_SetReqRdSize(stm, adxf->rqrdsct);
        adxf->stat = ADXF_STAT_READING;
        /* @hack spelling: adxf->stm, which holds stm here, where
         * ADXSTM_Start(stm) would do */
        ADXSTM_Start(adxf->stm);
        ret = adxf->rqsct;
    }
    ADXCRS_Unlock();
    return ret;
}

Sint32 ADXF_ReadSj32(ADXF adxf, Sint32 nsct, SJ sj)
{
    Sint32 ret;

    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040811:'adxf' is NULL.(ADXF_ReadSj32)");
        return ADXF_ERR_PRM;
    }
    if (nsct < 0) {
        ADXERR_CallErrFunc1("E9040812:'nsct'is negative.(ADXF_ReadSj32)");
        return ADXF_ERR_PRM;
    }
    if (sj == NULL) {
        ADXERR_CallErrFunc1("E9040813:'sj'is NULL.(ADXF_ReadSj32)");
        return ADXF_ERR_PRM;
    }
    if (adxf->stat == ADXF_STAT_READING) {
        return 0;
    }
    ADXCRS_Lock();
    ret = adxf_read_sj32(adxf, nsct, sj);
    adxf->sjflag = TRUE;
    ADXCRS_Unlock();
    return ret;
}

Sint32 ADXF_ReadNw32(ADXF adxf, Sint32 nsct, void *buf)
{
    SJ sj;
    Sint32 ret;
    /* @hack spelling: addr, a local for buf's address, which the test reads
     * once, where the cast in the test would do */
    Uint32 addr;

    adxf_SetCmdHstry(ADXF_CMD_READ_NW32, ADXF_HSTRY_ENTRY, (Sint32)adxf, nsct,
                     (Sint32)buf);
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040816:'adxf' is NULL.(ADXF_ReadNw32)");
        return ADXF_ERR_PRM;
    }
    if (nsct < 0) {
        ADXERR_CallErrFunc1("E9040817:'nsct' is negative.(ADXF_ReadNw32)");
        return ADXF_ERR_PRM;
    }
    if (buf == NULL) {
        ADXERR_CallErrFunc1("E9040818:'buf' is NULL.(ADXF_ReadNw32)");
        return ADXF_ERR_PRM;
    }
    addr = (Uint32)buf;
    if ((addr & (ADXF_DEF_DMA_ALIGN - 1)) != 0) {
        ADXERR_CallErrFunc1(
            "E9040819:'buf' isn't 32byte alignment.(ADXF_ReadNw32)");
        return ADXF_ERR_PRM;
    }
    if (adxf->stat == ADXF_STAT_READING) {
        return 0;
    }
    if (adxf->sj != NULL) {
        /* @bug logic: the report says the joint is null, where the test
         * found one (TST R2,R2 at 0x8c131e84) */
        ADXERR_CallErrFunc1("E9040821:'sj' is NULL.(ADXF_ReadNw32)");
        return ADXF_ERR_FATAL;
    }
    sj = SJRBF_Create(buf, nsct * ADXF_DEF_SCT_SIZE, 0);
    if (sj == NULL) {
        return ADXF_ERR_INTERNAL;
    }
    adxf->buf = buf;
    adxf->bsize = nsct * ADXF_DEF_SCT_SIZE;
    ADXCRS_Lock();
    if (adxf_ocbi_fg == TRUE) {
        ADXF_Ocbi(adxf->buf, adxf->bsize);
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((ret = adxf_read_sj32(adxf, nsct, sj)) <= 0) {
        SJ_Destroy(sj);
    } else {
        adxf->sj = sj;
    }
    adxf->sjflag = FALSE;
    ADXCRS_Unlock();
    adxf_SetCmdHstry(ADXF_CMD_READ_NW32, ADXF_HSTRY_EXIT, (Sint32)adxf, nsct,
                     (Sint32)buf);
    return ret;
}

Sint32 ADXF_Stop(ADXF adxf)
{
    /* @hack spelling: ofst, a local for the sectors read, which
     * ADXSTM_GetCurOfst always stores, where &adxf->rdsct passed in its
     * place would do */
    Sint32 ofst;

    adxf_SetCmdHstry(ADXF_CMD_STOP, ADXF_HSTRY_ENTRY, (Sint32)adxf,
                     ADXF_HSTRY_NO_PRM, ADXF_HSTRY_NO_PRM);
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040822:'adxf' is NULL.(ADXF_Stop)");
        return ADXF_ERR_PRM;
    }
    if (adxf->stat != ADXF_STAT_STOP) {
        if (adxf->stat == ADXF_STAT_READEND) {
            adxf->stat = ADXF_STAT_STOP;
        } else {
            if (adxf->stm == NULL) {
                ADXERR_CallErrFunc1("E9040823:'adxf->stm' is NULL.(ADXF_Stop)");
                return ADXF_ERR_FATAL;
            }
            ADXCRS_Lock();
            ADXSTM_Stop(adxf->stm);
            ADXSTM_GetCurOfst(adxf->stm, &ofst);
            adxf->rdsct = ofst;
            adxf_CloseSjStm(adxf);
            adxf->stat = ADXF_STAT_STOP;
            ADXCRS_Unlock();
            adxf_SetCmdHstry(ADXF_CMD_STOP, ADXF_HSTRY_EXIT, (Sint32)adxf,
                             ADXF_HSTRY_NO_PRM, ADXF_HSTRY_NO_PRM);
        }
    }
    return adxf->skpos;
}

/* ADXF_ExecServer's work for one handle */
void adxf_ExecOne(ADXF adxf)
{
    /* @hack spelling: ofst, a local for the sectors read, which
     * ADXSTM_GetCurOfst always stores, where &adxf->rdsct passed in its
     * place would do */
    Sint32 ofst;

    if (adxf->stat != ADXF_STAT_READING) {
        return;
    }
    adxf->stat = ADXSTM_GetStat(adxf->stm);
    ADXSTM_GetCurOfst(adxf->stm, &ofst);
    adxf->rdsct = ofst;
    if (adxf->stat == ADXF_STAT_READEND || adxf->stat == ADXF_STAT_ERROR) {
        adxf->skpos += adxf->rdsct;
        adxf_CloseSjStm(adxf);
    }
}

void ADXF_ExecServer(void)
{
    Sint32 i;
    /* @hack spelling: adxf, a local for each handle, where adxf_obj[i] in
     * place would do */
    ADXF adxf;

    ADXCRS_Lock();
    for (i = 0; i < ADXF_OBJ_MAX; i++) {
        adxf = &adxf_obj[i];
        if (adxf->used == TRUE) {
            adxf_ExecOne(adxf);
        }
    }
    ADXCRS_Unlock();
}

Sint32 ADXF_Seek(ADXF adxf, Sint32 pos, Sint32 type)
{
    adxf_SetCmdHstry(ADXF_CMD_SEEK, ADXF_HSTRY_ENTRY, (Sint32)adxf, pos, type);
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040825:'adxf' is NULL.(ADXF_Seek)");
        return ADXF_ERR_PRM;
    }
    if (adxf->stat == ADXF_STAT_READING) {
        ADXF_Stop(adxf);
    }
    if (type == ADXF_SEEK_SET) {
        adxf->skpos = pos;
    } else if (type == ADXF_SEEK_CUR) {
        adxf->skpos += pos;
    } else if (type == ADXF_SEEK_END) {
        adxf->skpos = adxf->fnsct + pos;
    } else {
        ADXERR_CallErrFunc1("E9040826:'type' is illigal.(ADXF_Seek)");
        return ADXF_ERR_PRM;
    }
    if (adxf->skpos < 0) {
        adxf->skpos = 0;
    } else if (adxf->skpos > adxf->fnsct) {
        adxf->skpos = adxf->fnsct;
    }
    adxf_SetCmdHstry(ADXF_CMD_SEEK, ADXF_HSTRY_EXIT, (Sint32)adxf, pos, type);
    return adxf->skpos;
}

Sint32 ADXF_Tell(ADXF adxf)
{
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040827:'adxf' is NULL.(ADXF_Tell)");
        return ADXF_ERR_PRM;
    }
    return adxf->skpos;
}

Sint32 ADXF_GetFsizeSct(ADXF adxf)
{
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040828:'adxf' is NULL.(ADXF_GetFsizeSct)");
        return ADXF_ERR_PRM;
    }
    return adxf->fnsct;
}

Sint32 ADXF_GetFsizeByte(ADXF adxf)
{
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040829:'adxf' is NULL.(ADXF_GetFsizeByte)");
        return ADXF_ERR_PRM;
    }
    return adxf->fsize;
}

Sint32 ADXF_GetNumReqSct(ADXF adxf, Sint32 *seekpos)
{
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040830:'adxf' is NULL.(ADXF_GetNumReqSct)");
        *seekpos = 0;
        return ADXF_ERR_PRM;
    }
    *seekpos = adxf->rdstpos;
    return adxf->rqsct;
}

Sint32 ADXF_GetNumReadSct(ADXF adxf)
{
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040831:'adxf' is NULL.(ADXF_GetNumReadSct)");
        return ADXF_ERR_PRM;
    }
    return adxf->rdsct;
}

Sint32 ADXF_GetStat(ADXF adxf)
{
    if (adxf == NULL) {
        ADXERR_CallErrFunc1("E9040832:'adxf' is NULL.(ADXF_GetStat)");
        return ADXF_ERR_PRM;
    }
    return adxf->stat;
}

/* check ptid and flid for ADXF_GetFnameRange and ADXF_GetFnameRangeEx */
Sint32 adxf_ChkPrmGfr(Sint32 ptid, Sint32 flid)
{
    ADXF_PTINFO *pt;

    /* @hack spelling: ptid's range and adxf_ptinfo[ptid] tested apart, each
     * followed by the same report, where one test with || would do */
    if (ptid < 0 || ptid >= ADXF_PART_MAX) {
        ADXERR_CallErrFunc1("E9040828:'ptid' is range outside.");
        return ADXF_ERR_PRM;
    }
    pt = adxf_ptinfo[ptid];
    if (pt == NULL) {
        /* @bug logic: the report says ptid is out of range, where the test
         * found adxf_ptinfo[ptid] null (TST R4,R4 at 0x8c1321da falls into
         * the range's report at 0x8c1321de) */
        ADXERR_CallErrFunc1("E9040828:'ptid' is range outside.");
        return ADXF_ERR_PRM;
    }
    if (flid < 0 || flid >= pt->nfile) {
        ADXERR_CallErrFunc1("E9040828:'flid' is range outside.");
        return ADXF_ERR_PRM;
    }
    return ADXF_ERR_OK;
}

Sint32 ADXF_GetFnameRange(Sint32 ptid, Sint32 flid, char *fname, Sint32 *ofst,
                          Sint32 *fnsct)
{
    ADXF_PTINFO *pt;
    ADXF_ADD_INFO *add;
    /* @hack spelling: entry, a local for the entry the loop tests, where
     * add[i] in place would do */
    ADXF_ADD_INFO *entry;
    Uint16 *first;
    Uint16 *sizes;
    Sint32 start;
    Sint32 ret;
    Sint32 i;

    ret = adxf_ChkPrmGfr(ptid, flid);
    if (ret < 0) {
        *ofst = -1;
        *fnsct = -1;
        return ret;
    }
    for (pt = adxf_ptinfo[ptid]; pt->next != NULL; pt = pt->next) {
        add = (ADXF_ADD_INFO *)&pt->top;
        for (i = 0; i < pt->nentry; i++) {
            entry = &add[i];
            if (entry->flid == flid) {
                strncpy(fname, pt->fname, ADXF_FNAME_MAX);
                *ofst = entry->ofst;
                *fnsct = entry->fnsct;
                /* @hack spelling: ret, which holds ADXF_ERR_OK here and at
                 * the end, where return ADXF_ERR_OK at both would do */
                return ret;
            }
        }
    }
    first = (Uint16 *)&pt->top;
    start = *first;
    sizes = first + 1;
    for (i = 0; i < flid; i++) {
        start += sizes[i];
    }
    strncpy(fname, pt->fname, ADXF_FNAME_MAX);
    *ofst = start;
    *fnsct = sizes[flid];
    return ret;
}

Sint32 ADXF_GetFnameRangeEx(Sint32 ptid, Sint32 flid, char *fname, void **dir,
                            Sint32 *ofst, Sint32 *fnsct)
{
    ADXF_PTINFO *pt;
    ADXF_ADD_INFO *add;
    /* @hack spelling: entry, a local for the entry the loop tests, where
     * add[i] in place would do */
    ADXF_ADD_INFO *entry;
    Uint16 *first;
    Uint16 *sizes;
    Sint32 start;
    Sint32 ret;
    Sint32 i;

    ret = adxf_ChkPrmGfr(ptid, flid);
    if (ret < 0) {
        *dir = NULL;
        *ofst = -1;
        *fnsct = -1;
        return ret;
    }
    for (pt = adxf_ptinfo[ptid]; pt->next != NULL; pt = pt->next) {
        add = (ADXF_ADD_INFO *)&pt->top;
        for (i = 0; i < pt->nentry; i++) {
            entry = &add[i];
            if (entry->flid == flid) {
                strncpy(fname, pt->fname, ADXF_FNAME_MAX);
                /* @bug uninitialized: reads an added partition's curdir,
                 * which ADXF_AddPartition never sets (MOV.L @(56,R14),R3 at
                 * 0x8c13235a). the file then opens in whatever directory
                 * record curdir held before ADXF_AddPartition. nothing in the
                 * game calls ADXF_AddPartition */
                *dir = pt->curdir;
                *ofst = entry->ofst;
                *fnsct = entry->fnsct;
                /* @hack spelling: ret, which holds ADXF_ERR_OK here and at
                 * the end, where return ADXF_ERR_OK at both would do */
                return ret;
            }
        }
    }
    first = (Uint16 *)&pt->top;
    start = *first;
    sizes = first + 1;
    for (i = 0; i < flid; i++) {
        start += sizes[i];
    }
    strncpy(fname, pt->fname, ADXF_FNAME_MAX);
    *dir = pt->curdir;
    *ofst = start;
    *fnsct = sizes[flid];
    return ret;
}

void ADXF_SetOcbiSw(Sint32 sw)
{
    adxf_ocbi_fg = sw;
}

void ADXF_SetReqRdSct(ADXF adxf, Sint32 nsct)
{
    if (adxf->stat == ADXF_STAT_READING) {
        ADXERR_CallErrFunc1("E0041201:state is reading(ADXF_SetReqRdSct)");
        return;
    }
    adxf->rqrdsct = nsct;
}
