/* CRI ADX's file readers. a reader reads a file into the stream joint sj
 * through cvfs, in runs of sectors. each read takes room from sj's free line,
 * SJ_LIN_FREE. it adds the sectors it reads to sj's data line, SJ_LIN_DATA. a
 * reader moves on only in ADXSTM_ExecServer */
#ifndef ADX_STMC_H
#define ADX_STMC_H
#include <sg_xpt.h>
#include <sj.h>

/* a reader's handle. only adx_stmc.c sees the record behind it */
typedef struct _adxstm_obj *ADXSTM;

/* a reader's states: stopped, reading, read to its end, and failed */
#define ADXSTM_STAT_STOP 1
#define ADXSTM_STAT_READING 2
#define ADXSTM_STAT_READEND 3
#define ADXSTM_STAT_ERROR 4

/* the bytes a reader keeps of its file's name, its terminator included */
#define ADXSTM_FNAME_MAX 40
/* the bytes of a sector a reader reads, and the sectors that hold nbyte
 * bytes */
#define ADXSTM_SCT_SIZE 2048
#define ADXSTM_SCT_COUNT(nbyte)                                                \
    (((nbyte) + ADXSTM_SCT_SIZE - 1) / ADXSTM_SCT_SIZE)

/* start cvfs with the disc as its default device, and free every reader.
 * the file of a reader still open stays open, with no reader to close it. it
 * returns 1 */
Sint32 ADXSTM_Init(void);
/* end cvfs */
void ADXSTM_Finish(void);

/* each open returns a new reader that reads the file fname into sj, or
 * null. dir is the directory record that holds fname, or null. the reader
 * never destroys sj.
 *
 * ADXSTM_OpenFileRangeEx and ADXSTM_OpenFileRangeExRt read nsct sectors of
 * the file, from the file's sector ofst. ADXSTM_OpenFnameEx and
 * ADXSTM_OpenFname read the whole file. ADXSTM_OpenFname finds fname as
 * ADXSTM_OpenFnameEx does with a null dir.
 *
 * the library's readers form two pools, a normal one and a real-time one.
 * ADXSTM_OpenFileRangeExRt and ADXSTM_OpenFname take a free reader of the
 * real-time pool. the other two opens take a free reader of the normal pool.
 *
 * the new reader is stopped, with its read position at its first sector.
 * its read limit is its size in sectors, as ADXSTM_SetEos says. its bufsize
 * and rdsize both start as the bytes sj's free line holds.
 * ADXSTM_SetBufSize sets them.
 *
 * an open fails where the file won't open, and reports the failure to the
 * library's error function. a name of ADXSTM_FNAME_MAX bytes or more fails
 * every open but ADXSTM_OpenFnameEx, with a report. ADXSTM_OpenFnameEx
 * keeps the name's first ADXSTM_FNAME_MAX - 1 bytes.
 *
 * an open also fails where its pool has no free reader, with no report.
 * that open leaves the file open, with no reader to close it */
ADXSTM ADXSTM_OpenFileRangeEx(char *fname, void *dir, Sint32 ofst, Sint32 nsct,
                              SJ sj);
ADXSTM ADXSTM_OpenFileRangeExRt(char *fname, void *dir, Sint32 ofst,
                                Sint32 nsct, SJ sj);
ADXSTM ADXSTM_OpenFnameEx(char *fname, void *dir, SJ sj);
ADXSTM ADXSTM_OpenFname(char *fname, SJ sj);
/* stop the reader, close its file and free it. a null stm is ignored */
void ADXSTM_Close(ADXSTM stm);
/* the reader's state, an ADXSTM_STAT_ value */
Sint32 ADXSTM_GetStat(ADXSTM stm);
/* move the read position to the reader's sector sct. it returns the read
 * position */
Sint32 ADXSTM_Seek(ADXSTM stm, Sint32 sct);
/* the read position, in sectors from the reader's first. a reader that
 * isn't open gives 0 */
Sint32 ADXSTM_Tell(ADXSTM stm);
/* start reading from the read position. the reader becomes
 * ADXSTM_STAT_READING, or ADXSTM_STAT_READEND at once when it has no bytes.
 * it returns 1 */
Sint32 ADXSTM_Start(ADXSTM stm);
/* set the reader's read limit, the sector no read passes, to eossct,
 * counted from the reader's first. a negative eossct sets it to the reader's
 * size in sectors, rounded up, as each open does. the reader still becomes
 * ADXSTM_STAT_READEND only where its read position reaches that size */
void ADXSTM_SetEos(ADXSTM stm, Sint32 eossct);
/* stop the read under way, if any. the reader becomes ADXSTM_STAT_STOP. the
 * room that a stopped read took from sj's free line doesn't go back to that
 * line */
void ADXSTM_Stop(ADXSTM stm);
/* move each reader in ADXSTM_STAT_READING on, starting a read as
 * ADXSTM_SetBufSize says. a read's sectors join sj's data line. the rest of
 * its room goes back to the free line. a failed read gives all its room
 * back, and the twelfth in a row makes the reader ADXSTM_STAT_ERROR. a reader
 * becomes ADXSTM_STAT_READEND once its read position reaches its size in
 * sectors */
void ADXSTM_ExecServer(void);
/* *ofst gets the sectors the read position has moved since ADXSTM_Start. it
 * returns 1 */
Sint32 ADXSTM_GetCurOfst(ADXSTM stm, Sint32 *ofst);
/* set the reader's bufsize and rdsize, in bytes. a read starts while sj's data
 * line holds fewer than bufsize bytes. it asks sj for rdsize bytes of room. it
 * returns 1 */
Sint32 ADXSTM_SetBufSize(ADXSTM stm, Sint32 bufsize, Sint32 rdsize);
/* set the most sectors a read asks for. a new reader's reads ask for 512
 * sectors at most. it returns 1 */
Sint32 ADXSTM_SetReqRdSize(ADXSTM stm, Sint32 nsct);
/* the reader's size in bytes: its range's, or its whole file's */
Sint32 ADXSTM_GetFileLen(ADXSTM stm);
/* ADXSTM_GetFsizeSct stores the size of the file fname in *fnsct, in
 * sectors, rounded up. ADXSTM_GetFsizeByte stores it in *fsize, in bytes.
 * each returns 1 */
Sint32 ADXSTM_GetFsizeSct(char *fname, Sint32 *fnsct);
Sint32 ADXSTM_GetFsizeByte(char *fname, Sint32 *fsize);

/* run func with obj when a read reaches the reader's end sector */
void ADXSTM_EntryEosFunc(ADXSTM stm, void (*func)(void *obj), void *obj);

#endif
