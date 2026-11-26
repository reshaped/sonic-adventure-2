/* the GSC: its table of streams, its server and its calls.
 *
 * the GSC reads files from the disc into stream joints, one stream to a
 * file. a joint is sj.h's SJ, which passes bytes from a writer to a reader.
 * its free line holds the room to write into. its data line holds the bytes
 * written and not yet read. a chunk is a run of bytes taken from a line.
 *
 * the server serves the reading streams in turn. for a stream, it takes a
 * chunk of up to rd_sct sectors from the joint's free line. transfers of up to
 * tr_sct sectors read the file into the chunk, one at a time. each transfer's
 * bytes go on the joint's data line. a sector is GDD_FS_SCTSIZE bytes */
#include <sg_mwgsc.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <string.h>

/* the GSC's error codes that sg_mwgsc.h doesn't name */

/* a stream in none of the GSC_STAT_ states, or a free chunk shorter than
 * asked */
#define GSC_ERR_STAT (-2)
#define GSC_ERR_NHN (-12) /* a stream count below 1 */
#define GSC_ERR_FULL (-13) /* every stream in use */
/* a transfer that completes short of its sectors */
#define GSC_ERR_SHORT (-17)
#define GSC_ERR_STALL (-19) /* a transfer that stalls while a stop waits */
/* a transfer the file system asks the drive for only in part */
#define GSC_ERR_PARTIAL (-20)

/* the alignment in bytes of the stream table mwGscInitG lays out */
#define GSC_WORK_ALIGN 32

/* the sectors that hold nbyte bytes, rounded up. the division rounds toward
 * 0, so an nbyte from -2046 to 0 gives 1 */
#define BYTE2SCT(nbyte) ((((nbyte) - 1) / GDD_FS_SCTSIZE) + 1)
#define NONNEG(n) (((n) < 0) ? 0 : (n))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define RD_SCT_DEFAULT 100
#define TR_SCT_DEFAULT 10

/* the stall count past which gsc_wait_transfer gives up on a transfer */
#define STALL_MAX 500000

/* the GSC's streams, the chunk the server is filling, and the error
 * function */
struct gsc_work {
    Sint32 nhn;
    struct gsc_stream *table;
    Bool reading; /* a transfer is in flight */
    Sint32 chunk_sct;
    Sint32 transfer_sct;
    Sint32 filled; /* the bytes the chunk's transfers have moved */
    Sint32 num_rd; /* the bytes the transfer had moved when last polled */
    Sint32 unk_1c;
    Sint32 unk_20;
    /* the index in table of the stream the server is on, the one the chunk
     * is for. the server's next turn starts after it */
    Sint32 cur;
    SJCK ck; /* the part of the chunk no transfer has filled */
    void (*err_func)(void *obj, Sint32 code);
    void *err_obj;
    Sint32 err_code; /* the last code mwGscSetErrCode recorded, never read */
};

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mwGscInit(Sint32 nhn, void *work);
Sint32 mwGscInitG(Sint32 nhn, void *work);
Sint32 mwGscReset(void);
Sint32 mwGscFinishG(void);
void mwGscExecServerG(void);
struct gsc_stream *mwGscOpenFnameG(char *fname, SJ sj);
struct gsc_stream *mwGscOpenRangeG(Sint32 stsct, Sint32 nsct, SJ sj);
Sint32 mwGscCloseG(struct gsc_stream *hn);
Sint32 mwGscStop(struct gsc_stream *hn);
Sint32 mwGscSeekSct(struct gsc_stream *hn, Sint32 ofst, Sint32 whence);
void mwGscEntryErrFunc(void (*func)(void *obj, Sint32 code), void *obj);

const char mwg_gsc_copyright[32] = "Copyright (c) 1998 SEGA";
const char mwg_gsc_version[32] = "MWGSC Version 1.31 1999-03-09";
const char mwg_gsc_build[] = "\nmwGsc Ver.1.31 Build:Mar 07 2000 19:22:24\n";
/* NOLINTEND(misc-use-internal-linkage) */

struct gsc_lif mwg_gsc_lif = {
    {NULL, NULL, NULL}, mwGscInitG,      mwGscFinishG, mwGscExecServerG,
    mwGscOpenFnameG,    mwGscOpenRangeG, mwGscCloseG,  NULL,
};
Sint32 mwg_gsc_prevcnt = 0;
Bool mwg_gsc_execstatic = FALSE;

static struct gsc_work gsc;
/* the server steps gsc_exec_server_counted has run since mwGscInitG. the
 * image's bss section, BSG, holds gsc first. so the count's definition
 * follows gsc's, outside the exports' NOLINT block */
/* @hack type: volatile on mwg_gsc_svrcnt, where a plain Uint32 would do */
volatile Uint32 mwg_gsc_svrcnt; /* NOLINT(misc-use-internal-linkage) */
/* the work mwGscInitStatic lays its one stream out in */
static char gsc_static_work[sizeof(struct gsc_stream) + GSC_WORK_ALIGN];

static void gsc_clear_streams(Sint32 nhn, struct gsc_stream *table);
static void gsc_close_all(void);
static void gsc_start_transfer(void);
static void gsc_start_chunk(void);
static void gsc_resume_drained(void);
static Bool gsc_resumable(Sint32 no);
static struct gsc_stream *gsc_select_stream(void);
static Bool gsc_wants_read(Sint32 no);
static void gsc_size_chunk(struct gsc_stream *hn, Sint32 nfree, Sint32 *nsct,
                           Sint32 *nbyte);
static void gsc_request_transfer(void);
static Sint32 gsc_check_transfer(void);
static Sint32 gsc_poll_transfer(struct gsc_stream *hn, Sint32 *nbyte,
                                Bool *done);
static Bool gsc_transfer_over(struct gsc_stream *hn, Sint32 stat, Sint32 nbyte);
static Bool gsc_chunk_filled(void);
static Bool gsc_at_end(struct gsc_stream *hn);
static void gsc_end_stream(struct gsc_stream *hn);
static struct gsc_stream *gsc_setup_stream(struct gsc_stream *hn, GDFS fs,
                                           Sint32 fsize, SJ sj);
static struct gsc_stream *gsc_find_unused(void);
static void gsc_clear_stream(struct gsc_stream *hn);
static Sint32 gsc_stop_read(struct gsc_stream *hn);
static Sint32 gsc_wait_transfer(void);
static void gsc_stop_file(Sint32 no);
static void gsc_stream_index(struct gsc_stream *hn, Sint32 *no);
static Sint32 gsc_seek_target(struct gsc_stream *hn, Sint32 ofst, Sint32 whence,
                              Sint32 *sct);
static Sint32 gsc_seek_file(struct gsc_stream *hn, Sint32 sct);

/* start the file system layer, then lay out nhn streams in work through
 * mwGscInitOvl. return 0, or the first error. a failed layout leaves its
 * error unrecorded and the layer started. while mwg_gsc_execstatic is set,
 * return GSC_ERR_STATIC, unrecorded, and do nothing */
Sint32 mwGscInit(Sint32 nhn, void *work)
{
    Sint32 err;

    /* @hack spelling: the result built in err through an else, where a return
     * of each result as it is known would do */
    if (!mwg_gsc_execstatic) {
        err = mwGscGdInit();
        if (err == 0) {
            err = mwGscInitOvl(nhn, work);
        }
    } else {
        err = GSC_ERR_STATIC;
    }
    return err;
}

/* while mwg_gsc_prevcnt is 0, start the file system layer, lay out one
 * stream in gsc_static_work, and set mwg_gsc_execstatic. return 0, or
 * mwGscGdInit's error. a call that returns 0 adds 1 to mwg_gsc_prevcnt */
Sint32 mwGscInitStatic(void)
{
    Sint32 err;

    /* @hack spelling: a failed mwGscInitOvl carried in err to the count's
     * test, where a return after it would do, as after mwGscGdInit */
    err = 0;
    if (mwg_gsc_prevcnt == 0) {
        err = mwGscGdInit();
        if (err != 0) {
            return err;
        }
        err = mwGscInitOvl(1, gsc_static_work);
        if (err == 0) {
            mwg_gsc_execstatic = TRUE;
        }
    }
    if (err == 0) {
        mwg_gsc_prevcnt++;
    }
    return err;
}

/* lay out a table of nhn streams, none in use, in work. the server's turn
 * starts at the first stream. return 0, or GSC_ERR_NHN, unrecorded, for nhn
 * below 1.
 *
 * the table starts at work rounded up to a multiple of GSC_WORK_ALIGN. work
 * must hold GSC_WORK_ALIGN - 1 bytes more than the table. the clear starts
 * at work, not at the table. up to GSC_WORK_ALIGN - 1 bytes at the table's
 * end keep what they held */
Sint32 mwGscInitG(Sint32 nhn, void *work)
{
    if (nhn <= 0) {
        return GSC_ERR_NHN;
    }
    gsc_clear_streams(nhn, work);
    mwg_gsc_svrcnt = 0;
    /* @hack spelling: work rounded up in place, where the rounded value
     * stored in gsc.table directly would do */
    work =
        (void *)(((Uint32)work + GSC_WORK_ALIGN - 1) & ~(GSC_WORK_ALIGN - 1));
    gsc.nhn = nhn;
    gsc.table = work;
    gsc.cur = nhn - 1;
    return 0;
}

static void gsc_clear_streams(Sint32 nhn, struct gsc_stream *table)
{
    memset(table, 0, nhn * sizeof(struct gsc_stream));
}

/* run the server's step, and count it in mwg_gsc_svrcnt. nothing calls
 * this function */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
void gsc_exec_server_counted(void) /* NOLINT(misc-use-internal-linkage) */
{
    mwg_gsc_svrcnt++;
    mwGscExecServerG();
}

/* stop the stream the server is on. then reset each open stream's joint,
 * close the stream, and restart the file system layer. return 0, or the
 * first error. a failed stop closes nothing */
Sint32 mwGscReset(void)
{
    /* @hack spelling: nhn and table, copies of gsc's fields taken before the
     * stop and stored back after the clear, though nothing between changes
     * the fields */
    Sint32 nhn;
    struct gsc_stream *table;
    Sint32 err;

    nhn = gsc.nhn;
    table = gsc.table;
    err = mwGscStop(&table[gsc.cur]);
    if (err != 0) {
        return err;
    }
    gsc_close_all();
    gsc_clear_streams(nhn, table);
    gsc.nhn = nhn;
    gsc.table = table;
    gsc.cur = nhn - 1;
    err = mwGscGdReset();
    /* @hack redundant: a test that returns err either way, where
     * mwGscGdReset's result returned in place would do */
    if (err != 0) {
        return err;
    }
    return 0;
}

static void gsc_close_all(void)
{
    Sint32 i;
    /* @hack spelling: table, a copy of gsc.table, where gsc.table read in
     * each pass would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;

    table = gsc.table;
    for (i = 0; i < gsc.nhn; i++) {
        hn = &table[i];
        if (hn->used) {
            mwGscSjReset(hn->sj);
            mwGscCloseG(hn);
        }
    }
}

/* reset as mwGscReset does, then finish the file system layer. return 0,
 * or mwGscReset's error, which leaves the layer unfinished */
Sint32 mwGscFinishG(void)
{
    Sint32 err;

    err = mwGscReset();
    if (err != 0) {
        return err;
    }
    mwGscGdFinish();
    return 0;
}

/* run the server's step: check the transfer in flight, or start the next.
 * the table must be laid out first: with no streams, gsc_select_stream
 * divides by a gsc.nhn of 0 */
void mwGscExecServerG(void)
{
    if (!gsc.reading) {
        gsc_start_transfer();
    } else {
        gsc_check_transfer();
    }
}

static void gsc_start_transfer(void)
{
    if (gsc_chunk_filled()) {
        gsc_start_chunk();
    } else {
        gsc_request_transfer();
    }
}

/* resume the drained streams. then take a chunk of the next reading stream's
 * joint, and request its first transfer. a stream with no sector of room
 * below flow_high pauses instead */
static void gsc_start_chunk(void)
{
    struct gsc_stream *hn;
    Sint32 nsct;
    Sint32 nfree;
    Sint32 nbyte;
    SJCK ck;
    /* @hack spelling: got, a local for mwGscSjGetFreeChunk's result, where
     * the call tested in place would do */
    Sint32 got;

    gsc_resume_drained();
    hn = gsc_select_stream();
    if (hn == NULL) {
        return;
    }
    mwGscSjIsGetFreeChunk(hn->sj, hn->rd_sct, hn->flow_high, &nfree);
    if (nfree <= 0) {
        hn->paused = TRUE;
        return;
    }
    gsc_size_chunk(hn, nfree, &nsct, &nbyte);
    /* @hack redundant: the test of nsct, which gsc_size_chunk leaves 0 only
     * where it leaves nbyte 0 */
    if (nsct <= 0 || nbyte <= 0) {
        return;
    }
    /* for a short chunk, mwGscSjGetFreeChunk records its own error, and this
     * call then records GSC_ERR_STAT */
    got = mwGscSjGetFreeChunk(hn->sj, nsct, &ck);
    if (got == 0) {
        mwGscSetErrCode(GSC_ERR_STAT);
        return;
    }
    gsc.ck = ck;
    gsc.chunk_sct = nsct;
    gsc.filled = 0;
    gsc_request_transfer();
}

static void gsc_resume_drained(void)
{
    Sint32 i;
    /* @hack spelling: table, a copy of gsc.table, where gsc.table read in
     * each pass would do */
    struct gsc_stream *table;

    table = gsc.table;
    for (i = 0; i < gsc.nhn; i++) {
        if (gsc_resumable(i)) {
            table[i].paused = FALSE;
        }
    }
}

/* TRUE for a paused stream whose data line has drained to flow_low bytes or
 * fewer */
static Bool gsc_resumable(Sint32 no)
{
    /* @hack spelling: table, a copy of gsc.table, where gsc.table indexed in
     * place would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;
    Sint32 nbyte;
    /* @hack spelling: err, a local for mwGscSjGetDataBsize's result, where
     * the call tested in place would do */
    Sint32 err;

    table = gsc.table;
    hn = &table[no];
    if (!hn->used) {
        return FALSE;
    }
    if (!hn->paused) {
        return FALSE;
    }
    err = mwGscSjGetDataBsize(hn->sj, &nbyte);
    if (err != 0) {
        return FALSE;
    }
    /* @hack spelling: the result through an if, where one return of the
     * comparison would do */
    if (nbyte > hn->flow_low) {
        return FALSE;
    }
    return TRUE;
}

/* return the first stream that wants a read, and make it gsc.cur. the search
 * starts after gsc.cur and wraps round the table. with none, return NULL and
 * move gsc.cur on by one */
static struct gsc_stream *gsc_select_stream(void)
{
    struct gsc_stream *found;
    /* @hack spelling: table, a copy of gsc.table, where gsc.table indexed in
     * place would do */
    struct gsc_stream *table;
    Sint32 i;
    Sint32 no;

    found = NULL;
    table = gsc.table;
    no = (gsc.cur + 1) % gsc.nhn;
    for (i = 0; i < gsc.nhn; i++) {
        if (gsc_wants_read(no)) {
            found = &table[no];
            break;
        }
        no = (no + 1) % gsc.nhn;
    }
    gsc.cur = no;
    return found;
}

/* TRUE for a stream in use that is reading and not paused. one in none of
 * the GSC_STAT_ states records GSC_ERR_STAT */
static Bool gsc_wants_read(Sint32 no)
{
    /* @hack spelling: table, a copy of gsc.table, where gsc.table indexed in
     * place would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;
    /* @hack spelling: req, a result the switch sets, where TRUE returned
     * from its case would do */
    Bool req;

    table = gsc.table;
    req = FALSE;
    /* @hack spelling: hn assigned inside the test, where a statement of its
     * own would do */
    if (!(hn = &table[no])->used) {
        return FALSE;
    }
    if (hn->paused) {
        return FALSE;
    }
    switch (hn->stat) {
    case GSC_STAT_STOP:
        break;
    case GSC_STAT_EXEC:
        req = TRUE;
        break;
    case GSC_STAT_END:
        break;
    default:
        mwGscSetErrCode(GSC_ERR_STAT);
        break;
    }
    return req;
}

/* store the chunk's size in *nsct and *nbyte. a chunk holds the bytes left
 * in the stream's range where they take fewer than nfree sectors. it holds
 * nfree whole sectors otherwise. both stay 0 unless nfree and the sectors
 * left are above 0. a stream at its range's end is marked ended first */
static void gsc_size_chunk(struct gsc_stream *hn, Sint32 nfree, Sint32 *nsct,
                           Sint32 *nbyte)
{
    Sint32 rest;
    Sint32 rest_sct;

    *nsct = 0;
    *nbyte = 0;
    if (gsc_at_end(hn)) {
        gsc_end_stream(hn);
    }
    rest = hn->ofst_end - hn->cur_ofst;
    rest_sct = BYTE2SCT(rest);
    /* @hack redundant: the test of nfree, which gsc_start_chunk has already
     * found above 0 */
    if (nfree > 0 && rest_sct > 0) {
        /* @bug logic: the server can stall on an empty file. BYTE2SCT counts
         * the file's rest of 0 bytes as one sector. with exactly one sector
         * of room, the else arm below then sizes a chunk of 2048 bytes
         * (CMP/GE R11,R4 at 0x8c1046ec, MOV.L R2,@R12 at 0x8c104700).
         * gdFsReqRd32 asks the drive for no sector, so each later step asks
         * again and serves no other stream. mwGscStop can't end this, since
         * the stream is already ended (CMP/EQ #2 at 0x8c104b14) */
        if (rest_sct < nfree) {
            *nsct = rest_sct;
            *nbyte = rest;
        } else {
            *nsct = nfree;
            /* @hack spelling: *nsct read back, where nfree * GDD_FS_SCTSIZE
             * would do */
            *nbyte = *nsct * GDD_FS_SCTSIZE;
        }
    }
}

/* request the chunk's next transfer: the sectors still to come, at most
 * tr_sct. the transfer is in flight only where the file system asks the
 * drive for all of them. otherwise the next step requests it again. where
 * the file system asks for some of them but not all, the call records
 * GSC_ERR_PARTIAL */
static void gsc_request_transfer(void)
{
    /* @hack spelling: ck, a copy of the chunk for its data, where
     * gsc.ck.data read in place would do */
    SJCK ck;
    /* @hack spelling: table and cur, copies of gsc's, where the stream taken
     * as &gsc.table[gsc.cur] would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;
    Sint32 cur;
    /* @hack spelling: done, a local for the sectors moved so far, where the
     * division inside the MIN would do */
    Sint32 done;
    Sint32 nsct;
    Sint32 nreq;
    /* @hack spelling: err, a local for mwGscGdReqRd's result, where the call
     * tested in place would do */
    Sint32 err;

    ck = gsc.ck;
    cur = gsc.cur;
    table = gsc.table;
    hn = &table[cur];
    done = gsc.filled / GDD_FS_SCTSIZE;
    nsct = MIN(hn->tr_sct, gsc.chunk_sct - done);
    err = mwGscGdReqRd(hn, nsct, ck.data, &nreq);
    if (err != 0 || nreq == 0) {
        return;
    }
    if (nreq != nsct) {
        mwGscGdStop(hn);
        mwGscSetErrCode(GSC_ERR_PARTIAL);
        return;
    }
    gsc.transfer_sct = nsct;
    gsc.num_rd = 0;
    gsc.reading = TRUE;
}

/* once the transfer is over, put its bytes on the joint's data line. move
 * the stream's read position past them. a stream read to its range's end is
 * marked ended once the chunk is filled. return 0, or the error
 * gsc_poll_transfer met */
static Sint32 gsc_check_transfer(void)
{
    /* @hack spelling: table and cur, copies of gsc's, where the stream taken
     * as &gsc.table[gsc.cur] would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;
    Sint32 cur;
    Sint32 nbyte;
    Bool done;
    /* @hack spelling: ck, a copy of the chunk that the put changes and
     * gsc.ck takes back, where &gsc.ck passed to the put would do */
    SJCK ck;
    Sint32 err;

    cur = gsc.cur;
    table = gsc.table;
    hn = &table[cur];
    ck = gsc.ck;
    err = gsc_poll_transfer(hn, &nbyte, &done);
    if (err != 0) {
        return err;
    }
    if (done) {
        mwGscSjPutDataChunk(hn->sj, nbyte, &ck);
        gsc.ck = ck;
        gsc.filled += nbyte;
        gsc.reading = FALSE;
        hn->cur_ofst += nbyte;
        if (gsc_chunk_filled() && gsc_at_end(hn)) {
            gsc_end_stream(hn);
        }
    }
    return 0;
}

/* store in *done whether the transfer is over, and in *nbyte the bytes it
 * has moved. a transfer that completes short of its sectors records
 * GSC_ERR_SHORT, and *nbyte then counts all its sectors. return 0, or a
 * call's error with *done unset */
static Sint32 gsc_poll_transfer(struct gsc_stream *hn, Sint32 *nbyte,
                                Bool *done)
{
    Sint32 stat;
    Sint32 err;

    err = mwGscGdGetFsiStat(hn, &stat);
    if (err != 0) {
        return err;
    }
    err = mwGscGdGetNumRd(hn, nbyte);
    if (err != 0) {
        return err;
    }
    gsc.num_rd = *nbyte;
    if (!gsc_transfer_over(hn, stat, *nbyte)) {
        *done = FALSE;
        return 0;
    }
    if (*nbyte < gsc.transfer_sct * GDD_FS_SCTSIZE) {
        mwGscSetErrCode(GSC_ERR_SHORT);
        *nbyte = gsc.transfer_sct * GDD_FS_SCTSIZE;
    }
    *done = TRUE;
    return 0;
}

/* @hack redundant: hn, which the test never reads */
static Bool gsc_transfer_over(struct gsc_stream *hn, Sint32 stat, Sint32 nbyte)
{
    /* @hack spelling: the result through an if, where one return of the
     * expression would do */
    if (stat != GDD_STAT_COMPLETE &&
        nbyte < gsc.transfer_sct * GDD_FS_SCTSIZE) {
        return FALSE;
    }
    return TRUE;
}

static Bool gsc_chunk_filled(void)
{
    /* @hack spelling: the result through an if, where one return of the
     * comparison would do */
    if (gsc.filled / GDD_FS_SCTSIZE >= gsc.chunk_sct) {
        return TRUE;
    }
    return FALSE;
}

static Bool gsc_at_end(struct gsc_stream *hn)
{
    /* @hack spelling: the result through an if, where one return of the
     * comparison would do */
    if (hn->ofst_end > hn->cur_ofst) {
        return FALSE;
    }
    return TRUE;
}

static void gsc_end_stream(struct gsc_stream *hn)
{
    hn->stat = GSC_STAT_END;
}

/* open the file fname as a stream into sj, and return it. the stream is
 * stopped at the file's start, and its range is the whole file. its rd_sct
 * and tr_sct are RD_SCT_DEFAULT and TR_SCT_DEFAULT. both its flow limits are
 * the joint's size.
 *
 * a failure returns NULL with its error recorded. once the file is open, a
 * failure leaves it open. a failed mwGscSetOfstRange or mwGscSeekSct also
 * leaves the stream in use */
struct gsc_stream *mwGscOpenFnameG(char *fname, SJ sj)
{
    struct gsc_stream *hn;
    GDFS fs;
    Sint32 fsize;
    /* @hack spelling: err, a local for mwGscSjChkBuf's result, where the
     * call tested in place would do */
    Sint32 err;

    err = mwGscSjChkBuf(sj);
    if (err != 0) {
        return NULL;
    }
    hn = gsc_find_unused();
    if (hn == NULL) {
        mwGscSetErrCode(GSC_ERR_FULL);
        return NULL;
    }
    fs = mwGscGdOpenFname(fname, &fsize);
    if (fs == NULL) {
        return NULL;
    }
    return gsc_setup_stream(hn, fs, fsize, sj);
}

/* set hn up over fs, a file of fsize bytes, as mwGscOpenFnameG says. return
 * hn, or NULL when a call fails */
static struct gsc_stream *gsc_setup_stream(struct gsc_stream *hn, GDFS fs,
                                           Sint32 fsize, SJ sj)
{
    Sint32 size;
    /* @hack spelling: err, a local for each call's result, where the calls
     * tested in place would do */
    Sint32 err;

    err = mwGscSjGetTotBufSize(sj, &size);
    if (err != 0) {
        return NULL;
    }
    hn->used = TRUE;
    hn->stat = GSC_STAT_STOP;
    hn->bufsize = size;
    hn->flow_low = size;
    hn->flow_high = size;
    hn->cur_ofst = 0;
    hn->stm_size = fsize;
    hn->rd_sct = RD_SCT_DEFAULT;
    hn->tr_sct = TR_SCT_DEFAULT;
    hn->paused = FALSE;
    hn->sj = sj;
    hn->fs = fs;
    /* @bug logic: leaves the stream in use and its file open when a call
     * below fails. used is set (MOV.L R4,@R14 at 0x8c104992) before the
     * failure returns NULL (MOV #0,R0 at 0x8c1049d8), and neither caller
     * closes the file. gsc_find_unused passes the stream over until
     * mwGscReset closes it */
    err = mwGscSetOfstRange(hn, 0, GSC_OFST_END);
    if (err != 0) {
        return NULL;
    }
    err = mwGscSeekSct(hn, 0, GDD_SEEK_SET);
    if (err != 0) {
        return NULL;
    }
    return hn;
}

static struct gsc_stream *gsc_find_unused(void)
{
    /* @hack spelling: found, a result the loop breaks out with, where the
     * stream returned from the loop would do */
    struct gsc_stream *found;
    /* @hack spelling: table, a copy of gsc.table, where gsc.table indexed in
     * place would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;
    Sint32 i;

    found = NULL;
    table = gsc.table;
    for (i = 0; i < gsc.nhn; i++) {
        hn = &table[i];
        if (!hn->used) {
            found = hn;
            break;
        }
    }
    return found;
}

/* open the nsct sectors from sector stsct as a stream into sj, otherwise as
 * mwGscOpenFnameG opens a file */
struct gsc_stream *mwGscOpenRangeG(Sint32 stsct, Sint32 nsct, SJ sj)
{
    struct gsc_stream *hn;
    GDFS fs;
    Sint32 fsize;
    /* @hack spelling: err, a local for mwGscSjChkBuf's result, where the
     * call tested in place would do */
    Sint32 err;

    err = mwGscSjChkBuf(sj);
    if (err != 0) {
        return NULL;
    }
    hn = gsc_find_unused();
    if (hn == NULL) {
        mwGscSetErrCode(GSC_ERR_FULL);
        return NULL;
    }
    fs = mwGscGdOpenRange(stsct, nsct, &fsize);
    if (fs == NULL) {
        return NULL;
    }
    return gsc_setup_stream(hn, fs, fsize, sj);
}

/* stop hn, close its file and free hn, even where the stop or the close
 * fails. return 0, or the close's error. the joint isn't reset */
Sint32 mwGscCloseG(struct gsc_stream *hn)
{
    Sint32 err;

    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    mwGscStop(hn);
    err = mwGscGdClose(hn);
    gsc_clear_stream(hn);
    return err;
}

static void gsc_clear_stream(struct gsc_stream *hn)
{
    memset(hn, 0, sizeof(struct gsc_stream));
}

/* make hn reading, so the server serves it. a stream not in use, whose
 * state is 0, returns GSC_ERR_STAT */
Sint32 mwGscStart(struct gsc_stream *hn)
{
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    switch (hn->stat) {
    case GSC_STAT_STOP:
        break;
    case GSC_STAT_EXEC:
        return 0;
    case GSC_STAT_END:
        break;
    default:
        return mwGscSetErrCode(GSC_ERR_STAT);
    }
    hn->stat = GSC_STAT_EXEC;
    return 0;
}

/* stop hn if it is reading. where the server is on hn, the stop waits for
 * the transfer in flight. the rest of the chunk then goes back to the free
 * line. a failed wait returns its error and leaves hn reading */
Sint32 mwGscStop(struct gsc_stream *hn)
{
    Sint32 err;

    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    if (hn->stat == GSC_STAT_EXEC) {
        err = gsc_stop_read(hn);
        /* @hack redundant: the else after the return */
        if (err != 0) {
            return err;
        } else {
            hn->stat = GSC_STAT_STOP;
        }
    }
    return 0;
}

/* where the server is on hn, wait for the transfer in flight. then give the
 * rest of the chunk back to the free line, even where the wait fails. return
 * 0, or the wait's error */
static Sint32 gsc_stop_read(struct gsc_stream *hn)
{
    Sint32 no;
    /* @hack spelling: sj, a copy of hn->sj taken first, where hn->sj read at
     * the call would do */
    SJ sj;
    Sint32 err;

    sj = hn->sj;
    gsc_stream_index(hn, &no);
    if (gsc.cur != no) {
        return 0;
    }
    err = gsc_wait_transfer();
    if (!gsc_chunk_filled()) {
        mwGscSjUngetFreeChunk(sj, &gsc.ck);
        gsc.filled = gsc.chunk_sct * GDD_FS_SCTSIZE;
    }
    gsc_stop_file(no);
    gsc.reading = FALSE;
    return err;
}

/* poll the transfer in flight until it is over, running the file system's
 * server between polls. return 0, the error gsc_check_transfer met, or
 * GSC_ERR_STALL once the stall count passes STALL_MAX */
static Sint32 gsc_wait_transfer(void)
{
    Sint32 err;
    Sint32 stall;
    Sint32 num_rd;

    err = 0;
    stall = 0;
    /* @hack spelling: the loop's test inside a for (;;), where while
     * (gsc.reading) would do */
    for (;;) {
        if (!gsc.reading) {
            break;
        }
        num_rd = gsc.num_rd;
        err = gsc_check_transfer();
        if (err != 0) {
            break;
        }
        if (num_rd == gsc.num_rd) {
            stall++;
        } else {
            stall = 0;
        }
        /* @bug logic: gives up on the 250001st pass in a row that moves no
         * bytes, not the 500001st. each such pass adds 1 to stall twice: in
         * the test of the bytes moved (ADD #1,R12 at 0x8c104bbe) and in the
         * test of the limit (ADD #1,R12 at 0x8c104bc6) */
        /* @hack spelling: the increment inside the test, where a statement
         * after it would do */
        if (stall++ > STALL_MAX) {
            err = mwGscSetErrCode(GSC_ERR_STALL);
            break;
        }
        mwGscGdExecServer();
    }
    return err;
}

static void gsc_stop_file(Sint32 no)
{
    /* @hack spelling: table and hn, locals for the stream, where
     * &gsc.table[no] passed would do */
    struct gsc_stream *table;
    struct gsc_stream *hn;

    table = gsc.table;
    hn = &table[no];
    mwGscGdStop(hn);
}

/* store hn's index in the table in *no, or -1 when the table doesn't hold
 * it */
static void gsc_stream_index(struct gsc_stream *hn, Sint32 *no)
{
    Sint32 i;
    /* @hack spelling: table, a copy of gsc.table, where gsc.table indexed in
     * place would do */
    struct gsc_stream *table;

    table = gsc.table;
    for (i = 0; i < gsc.nhn; i++) {
        if (hn == &table[i]) {
            *no = i;
            return;
        }
    }
    *no = -1;
}

/* move hn's read position to ofst sectors from whence. GDD_SEEK_SET counts
 * from the stream's start. GDD_SEEK_CUR counts from the sector that holds
 * the read position. GDD_SEEK_END counts from the stream's end, BYTE2SCT of
 * its size. another whence, or a sector before the start or past the end,
 * returns GSC_ERR_RANGE. a transfer in flight for hn ends first, as in
 * mwGscStop, but the stream keeps its state */
Sint32 mwGscSeekSct(struct gsc_stream *hn, Sint32 ofst, Sint32 whence)
{
    Sint32 sct;
    Sint32 nsct;
    Sint32 err;

    /* @bug memory: reads hn->stm_size before hn is tested (MOV.L
     * @(24,R14),R4 at 0x8c104c68), so a null hn reads address 24 before the
     * call returns GSC_ERR_HN */
    nsct = NONNEG(BYTE2SCT(hn->stm_size));
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    err = gsc_stop_read(hn);
    if (err != 0) {
        return err;
    }
    err = gsc_seek_target(hn, ofst, whence, &sct);
    if (err != 0) {
        return err;
    }
    if (sct < 0 || sct > nsct) {
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    err = gsc_seek_file(hn, sct);
    if (err != 0) {
        return err;
    }
    hn->cur_ofst = sct * GDD_FS_SCTSIZE;
    return 0;
}

/* store in *sct the sector ofst sectors from whence, as mwGscSeekSct counts
 * them, and return 0. another whence records GSC_ERR_RANGE and returns it */
static Sint32 gsc_seek_target(struct gsc_stream *hn, Sint32 ofst, Sint32 whence,
                              Sint32 *sct)
{
    /* @hack spelling: nsct and cur computed before the switch, where each
     * computed in its own case would do */
    Sint32 nsct;
    Sint32 cur;

    nsct = NONNEG(BYTE2SCT(hn->stm_size));
    cur = NONNEG(hn->cur_ofst / GDD_FS_SCTSIZE);
    /* @hack redundant: a store mwGscSeekSct never reads: it reads *sct only
     * after a return of 0, and each case that returns 0 stores *sct again */
    *sct = 0;
    switch (whence) {
    case GDD_SEEK_SET:
        *sct = ofst;
        break;
    case GDD_SEEK_CUR:
        *sct = ofst + cur;
        break;
    case GDD_SEEK_END:
        *sct = ofst + nsct;
        break;
    default:
        return mwGscSetErrCode(GSC_ERR_RANGE);
    }
    return 0;
}

/* move hn's file to sector sct. a reading stream goes through mwGscStart
 * again, which leaves it reading. a stream in none of the GSC_STAT_ states
 * records GSC_ERR_STAT. return 0, or a call's error */
static Sint32 gsc_seek_file(struct gsc_stream *hn, Sint32 sct)
{
    Sint32 stat;
    Sint32 err;

    /* @hack redundant: the test of hn, which mwGscSeekSct has already found
     * not null */
    if (mwGscIsHnNull(hn)) {
        return mwGscSetErrCode(GSC_ERR_HN);
    }
    err = mwGscGetStat(hn, &stat);
    if (err != 0) {
        return err;
    }
    err = mwGscGdSeek(hn, sct);
    if (err != 0) {
        return err;
    }
    switch (stat) {
    case GSC_STAT_STOP:
        break;
    case GSC_STAT_EXEC:
        err = mwGscStart(hn);
        break;
    case GSC_STAT_END:
        break;
    default:
        mwGscSetErrCode(GSC_ERR_STAT);
        break;
    }
    return err;
}

Sint32 mwGscIsFsStatErr(struct gsc_stream *hn)
{
    return mwGscGdIsFsiStatErr(hn);
}

/* have func take each nonzero code mwGscSetErrCode records, with obj, from
 * now on. a null func takes none */
void mwGscEntryErrFunc(void (*func)(void *obj, Sint32 code), void *obj)
{
    if (func == NULL) {
        gsc.err_func = NULL;
        gsc.err_obj = NULL;
        return;
    }
    gsc.err_func = func;
    gsc.err_obj = obj;
}

Sint32 mwGscSetErrCode(Sint32 code)
{
    gsc.err_code = code;
    if (code == 0) {
        return 0;
    }
    if (gsc.err_func != NULL) {
        gsc.err_func(gsc.err_obj, code);
    }
    return code;
}
