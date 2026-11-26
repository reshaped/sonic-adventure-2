/* CRI's linking stream controller (LSC), as far as the sources use it. a
 * handle keeps a queue of files and reads them, one after another, into one
 * stream joint. it reads each file through an ADXSTM, a stream of
 * adx_stmc.h. each queued file has a stream ID, which names it to the
 * queries */
#ifndef CRI_LSC_H
#define CRI_LSC_H
#include <adx_stmc.h>
#include <sg_xpt.h>
#include <sj.h>

/* the most files a handle's queue holds */
#define LSC_STM_MAX 16
/* the number of handles */
#define LSC_OBJ_MAX 8
/* the bytes a queued file keeps of its name. a name of LSC_FNAME_MAX bytes
 * or more is kept without a terminator */
#define LSC_FNAME_MAX 40

/* a handle's states: stopped, started with no file left to read, reading its
 * files, and failed */
#define LSC_STAT_STOP 0
#define LSC_STAT_WAIT 1
#define LSC_STAT_EXEC 2
#define LSC_STAT_ERR 3

/* a queued file's states: not yet read, being read, and read to its end */
#define LSC_STM_STAT_WAIT 0
#define LSC_STM_STAT_READ 1
#define LSC_STM_STAT_END 2

/* the error function. it is passed the obj entered with it and the error's
 * text. the next error overwrites the text */
typedef void (*LSC_ERRFN)(void *obj, char *msg);
/* what LSC_CallStatFunc calls, with obj1 and obj2 as LSC_EntryChgStatFunc was
 * given them */
typedef void (*LSC_STATFN)(void *obj1, void *obj2);

/* a file of a handle's queue: fsct sectors of the file fname, from that
 * file's sector ofst */
typedef struct _lsc_sinfo {
    Sint32 sid; /* the file's stream ID */
    char fname[LSC_FNAME_MAX];
    void *dir; /* the directory record fname is found in, or null */
    Sint32 ofst;
    Sint32 fsct;
    Sint32 stat; /* LSC_STM_STAT_WAIT to LSC_STM_STAT_END */
    Sint32 rdsct; /* the sectors read, as LSC_ExecServer last found them */
} LSC_SINFO;

/* what a handle reads a file through: a stream of adx_stmc.h's real-time
 * pool */
typedef ADXSTM LSC_FP;

/* a handle. its queue is a ring over sinfo: nstm files from slot rpos on,
 * the first of them the file being read. the next file entered goes to slot
 * wpos */
typedef struct _lsc_obj {
    Sint8 used; /* TRUE from LSC_Create until LSC_Destroy */
    Sint8 stat; /* LSC_STAT_STOP to LSC_STAT_ERR */
    Sint8 rdflg; /* TRUE once fp has been started */
    Sint8 lpflg; /* the loop flag, which LSC_SetLpFlg sets */
    Sint8 pause; /* TRUE while LSC_ExecServer skips the handle */
    Sint8 rsv1;
    Sint16 rsv2;
    SJ sj; /* the joint LSC_Create was given, which the files are read into */
    SJCK ck;
    /* the flow limit: a file's stream starts a read only while sj's data
     * line holds fewer bytes than this */
    Sint32 bufmin;
    /* the bytes sj's free line and its data line held at LSC_Create,
     * together */
    Sint32 bsize;
    Sint32 wpos;
    Sint32 rpos;
    Sint32 nstm;
    LSC_FP fp; /* the stream of the file being read, or null */
    Sint32 fsct; /* fp's size in sectors */
    Sint32 reqsct;
    Sint32 errcnt;
    LSC_SINFO sinfo[LSC_STM_MAX];
} LSC_OBJ;
typedef LSC_OBJ *LSC;

/* the function LSC_CallStatFunc calls, or null, and what it is called with */
extern LSC_STATFN lsc_stat_func;
extern void *lsc_stat_obj1;
extern void *lsc_stat_obj2;

/* pairs of LSC_Init and LSC_Finish nest. the outermost LSC_Init frees every
 * handle, and leaves LSC with no error function. the outermost LSC_Finish
 * destroys every handle in use, and leaves LSC with no error function too */
void LSC_Init(void);
void LSC_Finish(void);
/* have func take each error, with obj, from now on. an error comes as a
 * text, such as "E0003: Illigal parameter lsc=NULL". a null func takes none,
 * and sets obj null too */
void LSC_EntryErrFunc(LSC_ERRFN func, void *obj);
/* make a handle that reads into sj. the handle is stopped, and its queue is
 * empty. its flow limit is eight tenths of the bytes sj's two lines hold
 * together. it returns the handle. it returns null for a null sj, or when
 * every handle is in use, and reports either */
LSC LSC_Create(SJ sj);
/* stop the handle and free it. sj stays the caller's. a null lsc is
 * ignored */
void LSC_Destroy(LSC lsc);
/* enter a file at the queue's end: nsct sectors of the file fname, from its
 * sector ofst. dir is the directory record fname is found in, or null. a
 * handle in LSC_STAT_WAIT becomes LSC_STAT_EXEC.
 *
 * it returns the file's stream ID. the file goes to slot wpos, and its ID is
 * one more than the ID in the slot before wpos in the ring. the ID after
 * 0x7fffffff is 0. a new handle's slots hold zeros, so its first ID is 1.
 * LSC_Stop moves wpos to slot 0 and leaves each slot its ID, so an ID may
 * repeat after it.
 *
 * it returns -1 for a null lsc or fname, which it reports, and for a full
 * queue, which it doesn't. the report of a null fname reads a string through
 * that null pointer */
Sint32 LSC_EntryFileRange(LSC lsc, char *fname, void *dir, Sint32 ofst,
                          Sint32 nsct);
/* enter the whole of the file fname, with a null dir. it returns
 * LSC_EntryFileRange's result. it first opens a stream on the file, takes
 * the file's size from it and closes it.
 *
 * when ADXSTM_OpenFname returns null, it reports the file as one that can't
 * be opened. it then enters the file all the same, with a size read through
 * the null stream. ADXSTM_OpenFname returns null for a file that won't open,
 * for a name of ADXSTM_FNAME_MAX bytes or more, and while the real-time pool
 * has no free stream. lsc must not be null */
Sint32 LSC_EntryFname(LSC lsc, char *fname);
/* start the handle. it becomes LSC_STAT_EXEC, or LSC_STAT_WAIT while its
 * queue is empty. a handle that isn't stopped is stopped first, which
 * empties its queue. a null lsc is reported */
void LSC_Start(LSC lsc);
/* stop the handle: close the stream of the file being read, and empty the
 * queue. a null lsc is reported */
void LSC_Stop(LSC lsc);
/* with sw TRUE, have LSC_ExecServer skip the handle. any other sw has it
 * move the handle on again. the stream of the file being read isn't
 * stopped. a null lsc is reported */
void LSC_Pause(LSC lsc, Sint32 sw);
/* move on each handle that is in LSC_STAT_EXEC, has a file queued and isn't
 * paused. the file being read gets a stream over its sectors, which starts
 * with the handle's flow limit. a file read to its end leaves the queue, and
 * the next one starts. a failed stream makes the handle LSC_STAT_ERR. once
 * the queue is empty, the call runs LSC_CallStatFunc, and the handle becomes
 * LSC_STAT_WAIT.
 *
 * where ADXSTM_OpenFileRangeExRt returns null for a file, the server reports
 * it. it then reads and writes through the null stream. the error
 * function never gets the text of the server's own reports. the server
 * passes LSC_CallErrFunc each format in a register, but LSC_CallErrFunc
 * reads it from the stack */
void LSC_ExecServer(void);
/* the handle's state, LSC_STAT_STOP to LSC_STAT_ERR. it reports a null lsc,
 * and returns -1 */
Sint32 LSC_GetStat(LSC lsc);
/* the files in the queue. it reports a null lsc, and returns -1 */
Sint32 LSC_GetNumStm(LSC lsc);
/* the stream ID of the queue's file no, counted from 0 at the file being
 * read. it reports a null lsc, or a no outside the queue, and returns -1 */
Sint32 LSC_GetStmId(LSC lsc, Sint32 no);
/* LSC_GetStmFname, LSC_GetStmStat and LSC_GetStmRdSct return the name, the
 * state and the sectors read of the file whose stream ID is sid. they look
 * through all LSC_STM_MAX slots of sinfo, and take the first that holds sid.
 * a slot keeps its file's record after the file has left the queue, so sid
 * may name a file already read.
 *
 * they report a null lsc, and a sid that no slot holds. for either,
 * LSC_GetStmFname returns null, LSC_GetStmStat -1 and LSC_GetStmRdSct 0 */
char *LSC_GetStmFname(LSC lsc, Sint32 sid);
Sint32 LSC_GetStmStat(LSC lsc, Sint32 sid);
Sint32 LSC_GetStmRdSct(LSC lsc, Sint32 sid);
/* set the flow limit to min bytes. min must lie from 0 to the handle's
 * bsize. it reports a min outside that range, or a null lsc, and leaves the
 * limit as it was. a stream takes the limit when it starts, so the stream of
 * the file being read keeps the limit it started with */
void LSC_SetFlowLimit(LSC lsc, Sint32 min);
/* the flow limit, in bytes. it reports a null lsc, and returns -1 */
Sint32 LSC_GetFlowLimit(LSC lsc);
/* have LSC_CallStatFunc call func with obj1 and obj2. a null func has it
 * call nothing, and sets both null too */
void LSC_EntryChgStatFunc(LSC_STATFN func, void *obj1, void *obj2);
/* call the function LSC_EntryChgStatFunc entered, if any, with obj1 and
 * obj2 */
void LSC_CallStatFunc(void);
/* set the handle's loop flag to sw. while it is TRUE, LSC_ExecServer enters
 * each file again once it is read. a null lsc is reported */
void LSC_SetLpFlg(LSC lsc, Sint32 sw);

#endif
