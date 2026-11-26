/* the GSC, the middleware's reader of disc files into stream joints, as far
 * as the sources use it. a joint's free line holds the room the GSC reads into.
 * its data line holds the bytes read and not yet taken. a chunk is a run of
 * bytes taken from a line, and a sector is GDD_FS_SCTSIZE bytes.
 *
 * the mwGscGd calls work on a stream's file, through the mwGscFsi calls on a
 * file system's table. the mwGscSj calls work on the joint a stream reads
 * into. a call that returns a negative error records it first through
 * mwGscSetErrCode, unless its comment says otherwise */
#ifndef SG_MWGSC_H
#define SG_MWGSC_H
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>

/* a stream's states: stopped, reading, and read to its range's end. an
 * unused stream's state is 0 */
#define GSC_STAT_STOP 1
#define GSC_STAT_EXEC 2
#define GSC_STAT_END 3

/* the GSC's errors that more than one of its parts returns */
/* a call refused once mwGscInitStatic has laid out its stream */
#define GSC_ERR_STATIC (-1)
#define GSC_ERR_RANGE (-3) /* an argument out of range */
#define GSC_ERR_HN (-14) /* a null stream */

/* an end that mwGscSetOfstRange takes as the file's end */
#define GSC_OFST_END 0x7fffffff

/* a stream: a file the server reads into the joint sj */
struct gsc_stream {
    Bool used;
    Sint32 stat; /* a GSC_STAT_ value */
    Sint32 bufsize; /* the joint's size when the stream opened */
    /* a paused stream resumes once its data line holds flow_low bytes or
     * fewer */
    Sint32 flow_low;
    /* a chunk never fills the data line past flow_high bytes. a stream pauses
     * where not one sector of room is left below flow_high */
    Sint32 flow_high;
    Sint32 cur_ofst; /* the read position, in bytes from the file's start */
    Sint32 stm_size; /* the file's size, in bytes */
    /* the most sectors a chunk holds, and the most one transfer moves */
    Sint32 rd_sct;
    Sint32 tr_sct;
    /* the range mwGscSetOfstRange sets, in bytes. reading stops at
     * ofst_end, and nothing reads ofst_start */
    Sint32 ofst_start;
    Sint32 ofst_end;
    Sint32 unk_2c;
    Bool paused; /* while set, the server reads nothing for the stream */
    Sint32 unk_34;
    Sint32 unk_38;
    SJ sj;
    GDFS fs; /* the file, as mwGscGdOpenFname or mwGscGdOpenRange opens it */
};

/* the GSC's interface, its table of calls. it opens with three null words,
 * as sj.h's table of calls opens with QueryInterface, AddRef and Release */
struct gsc_lif {
    void *unk_00[3];
    Sint32 (*Init)(Sint32 nhn, void *work);
    Sint32 (*Finish)(void);
    void (*ExecServer)(void);
    struct gsc_stream *(*OpenFname)(char *fname, SJ sj);
    struct gsc_stream *(*OpenRange)(Sint32 stsct, Sint32 nsct, SJ sj);
    Sint32 (*Close)(struct gsc_stream *hn);
    void *unk_24;
};

/* a file system's table of calls, through which the mwGscFsi calls go. it
 * opens with three null words, as gsc_lif does */
struct gsc_fs_if {
    void *unk_00[3];
    Sint32 (*init)(void);
    Sint32 (*reinit)(void);
    void (*finish)(void);
    GDFS_DIRREC (*create_dirhn)(void *dirbuf, Sint32 max_dirent);
    Sint32 (*load_dir)(const char *dirname, GDFS_DIRREC dirrec);
    Sint32 (*set_dir)(GDFS_DIRREC dirrec);
    Sint32 (*change_dir)(const char *dirname);
    GDFS (*open)(const char *fname, GDFS_DIRREC dirrec);
    GDFS (*open_range)(Sint32 stsct, Sint32 nsct);
    void (*close)(GDFS gdfs);
    Sint32 (*seek)(GDFS gdfs, Sint32 sctno, Sint32 type);
    Sint32 (*tell)(GDFS gdfs);
    Sint32 (*check_eof)(GDFS gdfs, Bool *iseof);
    Bool (*get_file_size)(GDFS gdfs, Sint32 *fsize);
    Bool (*get_file_sct_size)(GDFS gdfs, Sint32 *fsctsize);
    Sint32 (*req_rd32)(GDFS gdfs, Sint32 nsct, void *buf);
    Sint32 (*get_stat)(GDFS gdfs);
    Sint32 (*get_err_stat)(GDFS gdfs);
    Sint32 (*get_num_rd)(GDFS gdfs);
    Sint32 (*stop_rd)(GDFS gdfs);
    void (*exec_server)(void);
};

/* the GSC's own interface, which mwGscInitOvl installs */
extern struct gsc_lif mwg_gsc_lif;
/* the mwGscInitStatic calls that returned 0, which mwGscFinishStatic counts
 * down */
extern Sint32 mwg_gsc_prevcnt;
/* set once mwGscInitStatic has laid out its stream. nothing clears it */
extern Bool mwg_gsc_execstatic;
/* record code as the last error, and return it. a nonzero code also goes to
 * the error function, if one is set */
Sint32 mwGscSetErrCode(Sint32 code);

Bool mwGscIsHnNull(void *hn);
/* store hn's GSC_STAT_ state in *stat, and return 0. a null hn returns
 * GSC_ERR_HN */
Sint32 mwGscGetStat(struct gsc_stream *hn, Sint32 *stat);
/* set hn's range to ofst through end, in bytes, and return 0. an end of
 * GSC_OFST_END stands for hn->stm_size. the read position stays where it was.
 * a null hn returns GSC_ERR_HN. unless 0 <= ofst <= end <= hn->stm_size, the
 * call returns GSC_ERR_RANGE and sets nothing */
Sint32 mwGscSetOfstRange(struct gsc_stream *hn, Sint32 ofst, Sint32 end);

/* install mwg_gsc_lif as the GSC's interface, and lay out nhn streams in
 * work through its Init. the call returns Init's result, unrecorded */
Sint32 mwGscInitOvl(Sint32 nhn, void *work);

/* the static GSC reads files into stream joints the player owns */
Sint32 mwGscInitStatic(void);
Sint32 mwGscSetRdSct(struct gsc_stream *hn, Sint32 nsct);
Sint32 mwGscSetTrSct(struct gsc_stream *hn, Sint32 nsct);
Sint32 mwGscStart(struct gsc_stream *hn);
Sint32 mwGscIsFsStatErr(struct gsc_stream *hn);
Sint32 mwGscFinishStatic(void);
void mwGscExecServer(void);
struct gsc_stream *mwGscOpenFname(char *fname, SJ sj);
Sint32 mwGscClose(struct gsc_stream *hn);
/* the GSC's file system layer. a call that fails returns a negative error,
 * or NULL where it opens a file. a call that takes hn reads hn->fs
 * untested, so hn must not be NULL. a null hn->fs fails.
 *
 * mwGscGdInit starts the layer through mwGscFsiInit, mwGscGdReset through
 * mwGscFsiReinit, and mwGscGdFinish ends it through mwGscFsiFinish */
Sint32 mwGscGdInit(void);
Sint32 mwGscGdReset(void);
void mwGscGdFinish(void);
/* open the file fname of the current directory, and store its size in
 * bytes in *fsize. where the size can't be read, the call fails and leaves
 * the file open */
GDFS mwGscGdOpenFname(char *fname, Sint32 *fsize);
/* open the nsct sectors from sector stsct as a file. store its size,
 * nsct * GDD_FS_SCTSIZE bytes, in *fsize */
GDFS mwGscGdOpenRange(Sint32 stsct, Sint32 nsct, Sint32 *fsize);
/* close hn's file, and return 0 */
Sint32 mwGscGdClose(struct gsc_stream *hn);
/* return 0. the call stops nothing: a read under way goes on */
Sint32 mwGscGdStop(struct gsc_stream *hn);
/* ask hn's file to read nsct sectors into buf. store in *nreq the sectors
 * the file system asked the drive for. a negative count from the file
 * system fails, and leaves *nreq unset */
Sint32 mwGscGdReqRd(struct gsc_stream *hn, Sint32 nsct, void *buf,
                    Sint32 *nreq);
/* store in *nbyte the bytes the file's read has moved. a negative count
 * fails, and stays in *nbyte */
Sint32 mwGscGdGetNumRd(struct gsc_stream *hn, Sint32 *nbyte);
/* store the file's status in *stat, as the file system gives it. a status
 * outside GDD_STAT_IDLE to GDD_STAT_ERR fails, and stays in *stat. a null
 * hn->fs leaves *stat unset */
Sint32 mwGscGdGetFsiStat(struct gsc_stream *hn, Sint32 *stat);
/* TRUE where the file's status is GDD_STAT_ERR, and else FALSE */
Sint32 mwGscGdIsFsiStatErr(struct gsc_stream *hn);
/* move the file's next read to sector sct, counted from the file's start */
Sint32 mwGscGdSeek(struct gsc_stream *hn, Sint32 sct);
/* run the file system's server once, with every interrupt masked */
void mwGscGdExecServer(void);

/* return 0 for a joint the GSC can read into, and otherwise the error. such
 * a joint isn't null, and holds whole sectors. its free line's first chunk
 * starts on a 32-byte boundary. the call takes that chunk to test it, and
 * puts it back */
Sint32 mwGscSjChkBuf(SJ sj);
/* store in *nbyte the joint's size, the bytes of its free and data lines
 * together. a failure leaves *nbyte unset */
Sint32 mwGscSjGetTotBufSize(SJ sj, Sint32 *nbyte);
/* store in *nbyte the bytes on the joint's data line. a failure leaves
 * *nbyte unset */
Sint32 mwGscSjGetDataBsize(SJ sj, Sint32 *nbyte);
/* store in *rsct the sectors to read next. the count is at most nsct, and
 * fits on the data line below limit bytes. the call returns 1 where the
 * free line can give that count as one chunk. otherwise it returns 0, and
 * cuts *rsct to the whole sectors of the chunk the line can give. *rsct is
 * 0 where no sector fits or the data line can't be measured. the call
 * records no error of its own */
Sint32 mwGscSjIsGetFreeChunk(SJ sj, Sint32 nsct, Sint32 limit, Sint32 *rsct);
/* take a chunk of nsct sectors from the free line into *ck, and return 1.
 * where the line gives a shorter chunk, the call records an error and
 * returns 0. that chunk is lost: it leaves the free line, and *ck doesn't
 * get it */
Sint32 mwGscSjGetFreeChunk(SJ sj, Sint32 nsct, SJCK *ck);
/* put the first nbyte bytes of *ck on the data line, and leave the rest in
 * *ck */
void mwGscSjPutDataChunk(SJ sj, Sint32 nbyte, SJCK *ck);
/* where *ck holds bytes, return it to the free line and empty it. a ck->len
 * of 0 or less leaves the line and *ck as they are */
void mwGscSjUngetFreeChunk(SJ sj, SJCK *ck);
/* the same as SJ_Reset(sj) */
void mwGscSjReset(SJ sj);

/* the GSC's calls on a file system go through the table mwGscFsiInit sets,
 * and record no error. each call from mwGscFsiOpen on passes its arguments to
 * the table's call of its name. it returns that call's result.
 *
 * without a table, mwGscFsiOpen and mwGscFsiOpenRange return NULL,
 * mwGscFsiGetFileSize FALSE and mwGscFsiGetNumRd 0. mwGscFsiClose and
 * mwGscFsiExecServer do nothing. every other call returns GSC_ERR_NOFSI */
#define GSC_ERR_NOFSI (-400)
/* set the table to mwg_gsc_fsg_i, and return 0. the call doesn't call the
 * table's init */
Sint32 mwGscFsiInit(void);
/* return 0 while a table is set. the call doesn't call the table's reinit */
Sint32 mwGscFsiReinit(void);
/* clear the table. the call doesn't call the table's finish */
void mwGscFsiFinish(void);
GDFS mwGscFsiOpen(const char *fname, GDFS_DIRREC dirrec);
GDFS mwGscFsiOpenRange(Sint32 stsct, Sint32 nsct);
void mwGscFsiClose(GDFS gdfs);
Sint32 mwGscFsiSeek(GDFS gdfs, Sint32 sctno, Sint32 type);
Bool mwGscFsiGetFileSize(GDFS gdfs, Sint32 *fsize);
Sint32 mwGscFsiReqRd32(GDFS gdfs, Sint32 nsct, void *buf);
Sint32 mwGscFsiGetStat(GDFS gdfs);
Sint32 mwGscFsiGetNumRd(GDFS gdfs);
void mwGscFsiExecServer(void);

/* GDFS's table of calls, the one mwGscFsiInit sets */
extern struct gsc_fs_if mwg_gsc_fsg_i;
/* the same as gdFsEntryErrFunc(gdfs, erfunc, obj) */
void mwGscFsgEntryErrFunc(GDFS gdfs, GDFS_ERRFUNC erfunc, void *obj);

#endif
