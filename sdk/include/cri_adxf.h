/* CRI's ADX file system (ADXF), as far as the sources use it */
#ifndef CRI_ADXF_H
#define CRI_ADXF_H
#include <adx_stmc.h>
#include <sg_xpt.h>
#include <sj.h>

/* the bytes a handle or a partition keeps of a file's name. a name of
 * ADXF_FNAME_MAX bytes or more is kept without a terminator */
#define ADXF_FNAME_MAX 40
/* the number of partitions, which are numbered from 0 */
#define ADXF_PART_MAX 256
/* the most files an AFS archive may list */
#define ADXF_FILE_MAX 65536
/* the most files open at once */
#define ADXF_OBJ_MAX 16

/* a handle's states: stopped, reading, done with its request, and failed.
 * while a request runs, the handle takes its stream's state */
#define ADXF_STAT_STOP ADXSTM_STAT_STOP
#define ADXF_STAT_READING ADXSTM_STAT_READING
#define ADXF_STAT_READEND ADXSTM_STAT_READEND
#define ADXF_STAT_ERROR ADXSTM_STAT_ERROR

/* ADXF_Seek's origins: the file's start, the read position and the file's
 * end */
#define ADXF_SEEK_SET 0
#define ADXF_SEEK_CUR 1
#define ADXF_SEEK_END 2

/* the calls' results. ADXF_ERR_FATAL is a failure the others don't name,
 * such as a file that won't open or read. ADXF_ERR_INTERNAL says that
 * ADXF_ReadNw32 couldn't make its joint. ADXF_ERR_PRM is a bad parameter.
 * ADXF_ERR_AFS_FILE is an archive of more than ADXF_FILE_MAX files. a call
 * reports most failures to the library's error function before it returns */
#define ADXF_ERR_OK 0
#define ADXF_ERR_FATAL (-1)
#define ADXF_ERR_INTERNAL (-2)
#define ADXF_ERR_PRM (-3)
#define ADXF_ERR_AFS_FILE (-4)

/* a sector's bytes, and the boundary a read's buffer must lie on */
#define ADXF_DEF_SCT_SIZE 2048
#define ADXF_DEF_DMA_ALIGN 32
/* the sectors a handle's stream asks for at a time, until ADXF_SetReqRdSct
 * sets another count */
#define ADXF_DEF_REQ_RD_SCT 512

/* the entries the command history holds. each new entry replaces the
 * oldest */
#define ADXF_CMD_HSTRY_MAX 256
/* the logged calls' numbers. the history counts each number's calls, for
 * numbers below ADXF_CMD_NUM_MAX */
#define ADXF_CMD_OPEN 1
#define ADXF_CMD_OPEN_AFS 2
#define ADXF_CMD_CLOSE 3
#define ADXF_CMD_READ_NW32 4
#define ADXF_CMD_STOP 5
#define ADXF_CMD_SEEK 6
#define ADXF_CMD_NUM_MAX 16

/* the bytes of ADXF_PTINFO before top, where its file table starts */
#define ADXF_PTIF_CMN_SZ ((5 * sizeof(long)) + (ADXF_FNAME_MAX * sizeof(char)))
/* the bytes of a loaded partition of n files, whose table holds a short for
 * the first file's offset and one for each file's size.
 * ADXF_CALC_PTINFO_SIZE rounds them up to a whole word */
#define ADXF_CALC_PTINFO_REAL_SIZE(n)                                          \
    (ADXF_PTIF_CMN_SZ + (((n) + 1) * sizeof(short)))
#define ADXF_CALC_PTINFO_SIZE(n)                                               \
    ((ADXF_CALC_PTINFO_REAL_SIZE(n) + sizeof(short)) / sizeof(long) *          \
     sizeof(long))
/* the bytes of an added partition of n entries */
#define ADXF_CALC_ADD_PTINFO_SIZE(n)                                           \
    (ADXF_PTIF_CMN_SZ + ((n) * sizeof(ADXF_ADD_INFO)))

/* a handle: an open file, read a run of sectors at a time from its read
 * position. a file of an AFS archive is a run of the archive's sectors */
typedef struct _adx_fs {
    Sint8 used;
    Sint8 stat; /* an ADXF_STAT_ value */
    /* TRUE when the last request read into its caller's joint, which the
     * handle never destroys */
    Sint8 sjflag;
    Sint8 rev;
    ADXSTM stm; /* the request's stream, or null */
    SJ sj; /* the joint ADXF_ReadNw32 made for the request, or null */
    Sint32 fnsct;
    /* the file's size in bytes, or fnsct * ADXF_DEF_SCT_SIZE for a file of
     * an archive */
    Sint32 fsize;
    Sint32 skpos; /* the read position, in sectors from the file's start */
    Sint32 rdstpos; /* the request's first sector in fname */
    Sint32 rqsct;
    Sint32 rdsct; /* the sectors of the request read so far */
    char *buf; /* the buffer ADXF_ReadNw32 reads into */
    Sint32 bsize; /* buf's size in bytes */
    Sint32 rqrdsct; /* the sectors the stream asks for at a time */
    Sint32 ofst; /* the file's first sector in fname */
    /* the file the stream opens: the file itself, or its archive */
    char fname[ADXF_FNAME_MAX];
    void *dir; /* the directory record fname is found in, or null */
} ADX_FS;
typedef ADX_FS *ADXF;

/* a partition: a list of an AFS archive's files, which ADXF_OpenAfs opens by
 * their numbers. a loaded partition lists an archive's files in order. an
 * added partition lists files of another archive.
 *
 * each partition number names a chain of partitions, linked by next. the
 * chain starts with the added partitions, the latest first, and ends with a
 * loaded one.
 *
 * the caller gives each partition's storage. the file table starts at top,
 * and runs on past the struct's end */
typedef struct _adxf_ptinfo {
    /* the next partition of the chain, or null for a loaded partition */
    struct _adxf_ptinfo *next;
    /* the bytes of this partition and of those after it in the chain */
    Sint32 size;
    /* the archive's files this partition covers, or 0 before the archive's
     * header is read */
    Sint32 nfile;
    /* an added partition's entries. a loaded partition keeps nfile here */
    Uint16 nentry;
    Sint8 type; /* ADXF_PT_LOADED or ADXF_PT_ADDED */
    Sint8 rev;
    char fname[ADXF_FNAME_MAX]; /* the archive's name */
    /* the directory record the archive is found in, or null */
    void *curdir;
    /* the first word of the file table. a loaded partition's table holds
     * shorts: the first file's offset, then each file's size, in sectors. an
     * added partition's holds an ADXF_ADD_INFO for each file it lists */
    Sint32 top;
} ADXF_PTINFO;

#define ADXF_PT_LOADED 0
#define ADXF_PT_ADDED 1

/* an added partition's entry: a file's number, and its size and its offset
 * from the archive's start, in sectors */
typedef struct _adxf_add_info {
    Uint16 flid;
    Uint16 fnsct;
    Sint32 ofst;
} ADXF_ADD_INFO;

/* an entry of the command history. a logged call writes one entry as it
 * starts, and another as it returns. a call that returns early writes no
 * second entry. ADXF_Close, ADXF_ReadNw32, ADXF_Stop and ADXF_Seek return
 * early where they refuse an argument. ADXF_ReadNw32 also does while the
 * handle reads, where no joint is free, and where its last joint remains.
 * ADXF_Stop also does while no read runs */
typedef struct _adxf_cmd_hstry {
    Uint8 cmdid; /* an ADXF_CMD_ value */
    /* ADXF_HSTRY_ENTRY as the call starts, ADXF_HSTRY_EXIT as it returns */
    Uint8 fg;
    Uint16 ncall; /* the command's calls so far, this one included */
    /* the call's parameters. ADXF_HSTRY_NO_PRM stands for one the call
     * doesn't have */
    Sint32 prm[3];
} ADXF_CMD_HSTRY;

#define ADXF_HSTRY_ENTRY 0
#define ADXF_HSTRY_EXIT 1
#define ADXF_HSTRY_NO_PRM (-1)

/* the partition loads: ADXF_LoadPartition, ADXF_LoadPartitionEx,
 * ADXF_LoadPartitionNw and ADXF_AddPartition. each lists files of the AFS
 * archive fname in the storage ptinfo, and makes ptid name ptinfo. ptinfo
 * must stay put while ptid names it.
 *
 * a ptid outside 0 to ADXF_PART_MAX - 1, or a null ptinfo, returns
 * ADXF_ERR_PRM. an fname that won't open returns ADXF_ERR_FATAL. once fname
 * opens, ptid names ptinfo even where the load then fails.
 *
 * each load but ADXF_LoadPartitionNw reads the archive's header a sector at a
 * time, and waits for each read. while it waits, it runs gdFsExecServer,
 * ADXSTM_ExecServer and ADXF_ExecServer. it returns ADXF_ERR_FATAL after three
 * failed reads in a row. a read that outlasts the wait counts as failed, but
 * runs on. the next read then gets the sector after the late one. the load
 * lists that sector's files in the late one's place.
 *
 * ADXF_LoadPartition makes ptinfo a loaded partition, which replaces ptid's
 * chain. it lists the archive's first nfile files, or all of a smaller
 * archive's. ptinfo must hold ADXF_CALC_PTINFO_SIZE(nfile) bytes. the call
 * returns ADXF_ERR_OK, or ADXF_ERR_AFS_FILE for an archive of over
 * ADXF_FILE_MAX files.
 *
 * ADXF_LoadPartitionEx lists every file of an archive found in the directory
 * record dir, which may be null. the files then open in dir. ptinfo must hold
 * ADXF_CALC_PTINFO_SIZE of the archive's file count. the call returns as
 * ADXF_LoadPartition does */
Sint32 ADXF_LoadPartition(Sint32 ptid, char *fname, void *ptinfo, Sint32 nfile);
Sint32 ADXF_LoadPartitionEx(Sint32 ptid, char *fname, void *dir, void *ptinfo);
/* start ADXF_LoadPartitionEx's load without waiting for it. ADXF_GetPtStat
 * moves the load on. one load runs at a time: a second returns ADXF_ERR_FATAL.
 *
 * the call returns ADXF_ERR_OK where its first read starts. where that read
 * won't start, the call frees the load's handle, and returns ADXF_ReadNw32's
 * error. the load still stands: each later ADXF_LoadPartitionNw returns
 * ADXF_ERR_FATAL. ADXF_Finish ends the load */
Sint32 ADXF_LoadPartitionNw(Sint32 ptid, char *fname, void *dir, void *ptinfo);
/* move ADXF_LoadPartitionNw's load of ptid on, and return its state. between
 * calls, the caller runs gdFsExecServer, ADXSTM_ExecServer and
 * ADXF_ExecServer, which move the load's read on.
 *
 * the call returns ADXF_STAT_READING while a read runs. once every file is
 * listed, it ends the load and returns ADXF_STAT_READEND. it ends the load
 * and returns ADXF_STAT_ERROR where the header doesn't start with "AFS". it
 * does the same for a header of over ADXF_FILE_MAX files, and where its next
 * read won't start. a read that fails returns ADXF_STAT_ERROR too, but leaves
 * the load standing. a ptid that isn't the load's returns ADXF_ERR_PRM */
Sint32 ADXF_GetPtStat(Sint32 ptid);
/* add a partition of files of the archive fname to ptid's chain, at its
 * front. ptid must already name a partition. the new partition's next is the
 * partition that ptid named before.
 *
 * the new partition lists the archive's first nfile files. it leaves out each
 * file whose offset in the archive's header is 0. the partitions after it
 * still give each file it leaves out. ptinfo must hold
 * ADXF_CALC_ADD_PTINFO_SIZE of the files it lists. the call reads and returns
 * as ADXF_LoadPartition does. it leaves the new partition's curdir as ptinfo
 * held it */
Sint32 ADXF_AddPartition(Sint32 ptid, char *fname, void *ptinfo, Sint32 nfile);
/* the bytes of every partition of ptid's chain. ptid must name a partition */
Sint32 ADXF_GetPtinfoSize(Sint32 ptid);

/* ADXF_Init starts the file system with no handle open, no partition and an
 * empty command history. ADXF_Finish closes every handle and puts the file
 * system back in that state */
void ADXF_Init(void);
void ADXF_Finish(void);
/* open the file fname, found in the directory record atr, which may be null.
 * it returns its handle, or null when every handle is open, fname is null, no
 * joint is free or the file won't open */
ADXF ADXF_Open(char *fname, void *atr);
/* open file flid of partition ptid. it returns its handle, or null when every
 * handle is open or ADXF_GetFnameRangeEx refuses ptid or flid */
ADXF ADXF_OpenAfs(Sint32 ptid, Sint32 flid);
/* stop the handle's request, and free the handle. a null adxf is ignored.
 * ADXF_CloseAll does the same for every open handle */
void ADXF_Close(ADXF adxf);
void ADXF_CloseAll(void);
/* start a request that reads nsct sectors from the read position into sj. it
 * returns the sectors the request asks for: nsct, or fewer where the file has
 * fewer left. the handle never destroys sj. a request of 0 sectors still
 * starts, and returns 0. while the handle reads, the call starts nothing and
 * returns 0. it returns ADXF_ERR_PRM for a null adxf or sj, or a negative nsct.
 * it returns ADXF_ERR_FATAL where the stream won't open, or the last request's
 * stream remains */
Sint32 ADXF_ReadSj32(ADXF adxf, Sint32 nsct, SJ sj);
/* start a request as ADXF_ReadSj32 does, into the nsct sectors at buf, and
 * return as it does. buf must lie on ADXF_DEF_DMA_ALIGN. the request reads
 * through a ring buffer joint over buf. the handle destroys that joint once the
 * request is done. a null or misaligned buf returns ADXF_ERR_PRM. it returns
 * ADXF_ERR_INTERNAL when every ring buffer joint is in use. it returns
 * ADXF_ERR_FATAL while the last request's joint remains */
Sint32 ADXF_ReadNw32(ADXF adxf, Sint32 nsct, void *buf);
/* stop the request, and return the read position. a request stopped before
 * it ends leaves the read position where the request started.
 * ADXF_GetNumReadSct then gives the sectors the request read. the call returns
 * ADXF_ERR_PRM for a null adxf, and ADXF_ERR_FATAL for a handle in
 * ADXF_STAT_ERROR */
Sint32 ADXF_Stop(ADXF adxf);
/* follow each open handle's request. when a request ends or fails, the
 * handle's read position moves past the sectors it read. the request's
 * stream then closes, as does a joint ADXF_ReadNw32 made for it */
void ADXF_ExecServer(void);
/* stop the request, and move the read position pos sectors from type's
 * origin, held within the file. it returns the read position, or
 * ADXF_ERR_PRM for a null adxf or an unknown type */
Sint32 ADXF_Seek(ADXF adxf, Sint32 pos, Sint32 type);
/* the read position. a null adxf returns ADXF_ERR_PRM */
Sint32 ADXF_Tell(ADXF adxf);
/* the file's size in sectors. a null adxf returns ADXF_ERR_PRM */
Sint32 ADXF_GetFsizeSct(ADXF adxf);
/* the file's size in bytes. for a file of an archive, the size is
 * ADXF_GetFsizeSct's result times ADXF_DEF_SCT_SIZE. a null adxf returns
 * ADXF_ERR_PRM */
Sint32 ADXF_GetFsizeByte(ADXF adxf);
/* the request's sectors. *seekpos gets the request's first sector, counted
 * from the file's start. for a file of an archive, it counts from the
 * archive's start. a null adxf returns ADXF_ERR_PRM, and sets *seekpos to 0 */
Sint32 ADXF_GetNumReqSct(ADXF adxf, Sint32 *seekpos);
/* the sectors of the request read, as ADXF_ExecServer or ADXF_Stop last
 * found them. a null adxf returns ADXF_ERR_PRM */
Sint32 ADXF_GetNumReadSct(ADXF adxf);
/* the handle's state, an ADXF_STAT_ value. a null adxf returns
 * ADXF_ERR_PRM */
Sint32 ADXF_GetStat(ADXF adxf);
/* find file flid of partition ptid. fname gets the name of the file's archive,
 * and must hold ADXF_FNAME_MAX bytes. *ofst gets the file's offset from the
 * archive's start, and *fnsct its size, both in sectors. the first partition
 * of ptid's chain that lists flid gives them.
 *
 * the call returns ADXF_ERR_OK. it returns ADXF_ERR_PRM, and sets *ofst and
 * *fnsct to -1, for a ptid that names no partition. it does the same for a flid
 * outside 0 to the first partition's nfile - 1. a flid that no added partition
 * lists must lie within the loaded partition's files. otherwise the call reads
 * past that partition's table.
 *
 * ADXF_GetFnameRangeEx also stores the archive's directory record in *dir, or
 * null where it returns ADXF_ERR_PRM */
Sint32 ADXF_GetFnameRange(Sint32 ptid, Sint32 flid, char *fname, Sint32 *ofst,
                          Sint32 *fnsct);
Sint32 ADXF_GetFnameRangeEx(Sint32 ptid, Sint32 flid, char *fname, void **dir,
                            Sint32 *ofst, Sint32 *fnsct);
/* set the sectors the handle's stream asks for at a time, from its next
 * request on. while the handle reads, the call reports an error and changes
 * nothing. adxf must not be null */
void ADXF_SetReqRdSct(ADXF adxf, Sint32 nsct);

#endif
