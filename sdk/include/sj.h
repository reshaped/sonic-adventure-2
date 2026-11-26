/* CRI stream joints, as far as the sources use them */
#ifndef SJ_H
#define SJ_H
#include <sg_xpt.h>

/* the two lines every joint has, by their ids. the free line holds the room
 * to write into, and the data line the bytes written and not yet read. a
 * general joint has two lines more, 2 and 3, which have no names */
#define SJ_LIN_FREE 0
#define SJ_LIN_DATA 1

/* the code a joint passes its error function. every error has this code:
 * an id that is no line, a chunk UngetChunk won't take, and a general joint
 * with no storage left */
#define SJ_ERR_PRM (-3)

/* the identifier of a kind of joint */
typedef struct _UUID {
    Uint32 Data1;
    Uint16 Data2;
    Uint16 Data3;
    Uint8 Data4[8];
} UUID;

/* a chunk: len bytes at data */
typedef struct {
    char *data;
    Sint32 len;
} SJCK;

/* a stream joint, which passes bytes from a writer to a reader. each of its
 * lines is a queue of bytes. bytes leave a line's front and join its end in
 * chunks. a joint begins with its kind's function table */
typedef struct {
    struct _sj_vtbl *vtbl;
} SJ_OBJ;
typedef SJ_OBJ *SJ;

/* a joint's function table. its first three slots are the calls that open
 * every interface of COM, Microsoft's component model. the SDK declares
 * them without parameters, and the ring buffer and general joints leave
 * them null.
 *
 * id is one of the joint's lines. a call that finds an id that is no line
 * reports an error, and takes and adds nothing. a joint reports an error by
 * calling its error function with SJ_ERR_PRM. a new joint's error function
 * never returns, so an error stops the program there until EntryErrFunc
 * sets another.
 *
 * GetChunk, UngetChunk and PutChunk mask every interrupt while they change
 * a line */
typedef struct _sj_vtbl {
    void (*QueryInterface)();
    void (*AddRef)();
    void (*Release)();
    /* free the joint. a null sj is ignored */
    void (*Destroy)(SJ sj);
    /* the identifier of the joint's kind */
    UUID *(*GetUuid)(SJ sj);
    /* put the joint's lines back as its create left them */
    void (*Reset)(SJ sj);
    /* take a chunk of nbyte bytes from the front of line id. *ck gets the
     * chunk, which may hold fewer bytes. where it takes none, ck->len gets
     * 0. nbyte must not be negative */
    void (*GetChunk)(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck);
    /* put *ck back at the front of line id. a chunk with a null data, or a
     * len of 0 or less, joins no line */
    void (*UngetChunk)(SJ sj, Sint32 id, SJCK *ck);
    /* add *ck at the end of line id. a chunk with a null data, or a len of
     * 0 or less, joins no line */
    void (*PutChunk)(SJ sj, Sint32 id, SJCK *ck);
    /* the bytes line id holds, or 0 for an id that is no line */
    Sint32 (*GetNumData)(SJ sj, Sint32 id);
    /* TRUE where GetChunk would take all of nbyte bytes from line id. FALSE
     * where it would take fewer. the call takes nothing itself.
     *
     * from a ring buffer joint, *rbyte gets the bytes GetChunk would take:
     * nbyte where the call returns TRUE. from a general joint, *rbyte gets
     * the front chunk's bytes, which may be more than nbyte. an empty line,
     * or an id that is no line, gives an *rbyte of 0 and FALSE.
     *
     * an nbyte of 0 can still give TRUE. a ring buffer joint gives TRUE for
     * it, whatever id is. a general joint gives TRUE for it only under
     * SJUNI_MODE_JOIN, from a line that holds a chunk */
    Sint32 (*IsGetChunk)(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte);
    /* have the joint call func with obj for each error. with a null func it
     * calls nothing */
    void (*EntryErrFunc)(SJ sj, void (*func)(void *obj, Sint32 ecode),
                         void *obj);
} SJ_IF;

/* each calls the function of its name in sj's table, with sj first */
#define SJ_Destroy(sj) (*(sj)->vtbl->Destroy)(sj)
#define SJ_Reset(sj) (*(sj)->vtbl->Reset)(sj)
#define SJ_GetChunk(sj, id, nbyte, ck)                                         \
    (*(sj)->vtbl->GetChunk)(sj, id, nbyte, ck)
#define SJ_UngetChunk(sj, id, ck) (*(sj)->vtbl->UngetChunk)(sj, id, ck)
#define SJ_PutChunk(sj, id, ck) (*(sj)->vtbl->PutChunk)(sj, id, ck)
#define SJ_GetNumData(sj, id) (*(sj)->vtbl->GetNumData)(sj, id)

/* split *ck into *ck1, its first nbyte bytes, and *ck2, the rest. a chunk
 * of nbyte bytes or fewer goes whole to *ck1. *ck2 then gets a null data
 * and a len of 0. either ck1 or ck2 may be ck */
void SJ_SplitChunk(SJCK *ck, Sint32 nbyte, SJCK *ck1, SJCK *ck2);

/* pairs of SJRBF_Init and SJRBF_Finish nest. the outermost SJRBF_Init
 * marks every ring buffer joint unused, and so does its SJRBF_Finish */
void SJRBF_Init(void);
void SJRBF_Finish(void);
/* a ring buffer joint over buf, or null when every ring buffer joint is in
 * use. buf holds a ring of bsize bytes, then xsize bytes more. bsize must
 * be above 0. the free line starts with the whole ring, and the data line
 * with nothing.
 *
 * the two lines share the ring. the bytes that leave one line's front are
 * the next to join the other's end. PutChunk doesn't check that a chunk is
 * those bytes. it only adds the chunk's len to the bytes the line holds.
 *
 * UngetChunk takes only a chunk that ends at the line's front. GetChunk's
 * last chunk from the line ends at that front. for any other chunk,
 * UngetChunk reports an error and leaves the line as it was. PutChunk and
 * UngetChunk clear *ck for an id that is no line. *ck then holds a null
 * data and a len of 0.
 *
 * GetChunk takes one run of memory. so it may take fewer than nbyte bytes
 * of a line that holds more. a run may pass the ring's end by up to xsize
 * bytes. the xsize bytes past the ring's end hold a copy of the ring's
 * first xsize bytes. so a run that passes the ring's end reads as if it
 * wrapped to the ring's start.
 *
 * PutChunk on the data line keeps the copy current. it writes the chunk's
 * bytes in either range into the other. a chunk put on the data line must
 * start inside the ring or at its end. the write into the ring's start goes
 * wrong for a chunk that starts past the ring's end */
SJ SJRBF_Create(char *buf, Sint32 bsize, Sint32 xsize);
/* each returns a value that sj, a ring buffer joint, was created with.
 * SJRBF_GetBufPtr returns its buf, SJRBF_GetBufSize its bsize and
 * SJRBF_GetXtrSize its xsize */
char *SJRBF_GetBufPtr(SJ sj);
Sint32 SJRBF_GetBufSize(SJ sj);
Sint32 SJRBF_GetXtrSize(SJ sj);

/* pairs of SJMEM_Init and SJMEM_Finish nest. the outermost SJMEM_Init
 * marks every memory joint unused, and so does its SJMEM_Finish */
void SJMEM_Init(void);
void SJMEM_Finish(void);
/* a memory joint over the bsize bytes at data, or null when every memory
 * joint is in use. its data line starts with those bytes, and its free line
 * holds nothing */
SJ SJMEM_Create(char *data, Sint32 bsize);

/* a general joint's modes. in either, GetChunk takes the front chunk whole
 * where it holds nbyte bytes or fewer.
 *
 * with SJUNI_MODE_SEPA, a line keeps each chunk as it was put. where the
 * front chunk holds more than nbyte bytes, GetChunk takes nothing.
 *
 * with SJUNI_MODE_JOIN, GetChunk takes the first nbyte bytes of a front
 * chunk that holds more. PutChunk adds no chunk for a *ck that starts at the
 * end of the line's last chunk. it merges *ck into that chunk, which grows by
 * ck->len bytes. UngetChunk adds no chunk for a *ck that ends at the front
 * chunk's start. it merges *ck into the front chunk, which then starts at
 * ck->data and grows by ck->len bytes */
#define SJUNI_MODE_SEPA 0
#define SJUNI_MODE_JOIN 1

/* pairs of SJUNI_Init and SJUNI_Finish nest. the outermost SJUNI_Init
 * marks every general joint unused, and so does its SJUNI_Finish */
void SJUNI_Init(void);
void SJUNI_Finish(void);
/* a general joint of the given mode, or null when every general joint is in
 * use. each of its four lines starts empty, and holds the chunks put on it.
 *
 * work is the joint's storage, wksize bytes on a 4-byte boundary. each
 * chunk on a line takes 16 bytes of it. where no 16 bytes are left,
 * PutChunk and UngetChunk report an error and drop the chunk. a *ck that
 * SJUNI_MODE_JOIN merges into a chunk takes no bytes. wksize must be 16 or
 * more. the joint writes into the first 16 bytes of work whatever wksize
 * is */
SJ SJUNI_Create(Sint32 mode, char *work, Sint32 wksize);
/* how many chunks line id of sj, a general joint, holds. id must be 0 to
 * 3 */
Sint32 SJUNI_GetNumChunk(SJ sj, Sint32 id);
/* how many more chunks the lines of sj, a general joint, have storage for */
Sint32 SJUNI_GetNumChainPool(SJ sj);

#endif
