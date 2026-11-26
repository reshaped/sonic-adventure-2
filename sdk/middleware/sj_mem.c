/* CRI's memory joints: stream joints whose data line is the caller's buffer,
 * and whose free line stays empty */
#include <machine.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

/* how many memory joints there are */
#define OBJ_MAX 32

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* a memory joint. its data line is a run of buf's first bsize bytes */
struct sjmem {
    SJ_OBJ sj;
    Bool used;
    UUID *uuid;
    Sint32 ndata; /* the bytes the data line holds */
    Sint32 data_pos; /* the data line's front, in bytes from buf */
    char *buf;
    Sint32 bsize;
    /* the error function, or null, and the obj it is passed */
    void (*err_func)(void *obj, Sint32 ecode);
    void *err_obj;
};

static const UUID sjmem_uuid = {
    0xdd9eee41,
    0x1679,
    0x11d2,
    {0x93, 0x6c, 0x00, 0x60, 0x08, 0x94, 0x48, 0xbc},
};

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 SJMEM_Lock(struct sjmem *mem);
void SJMEM_Unlock(struct sjmem *mem, Sint32 mask);
void SJMEM_Error(void *obj, Sint32 ecode);
void SJMEM_Destroy(SJ sj);
UUID *SJMEM_GetUuid(SJ sj);
void SJMEM_EntryErrFunc(SJ sj, void (*func)(void *obj, Sint32 ecode),
                        void *obj);
void SJMEM_Reset(SJ sj);
Sint32 SJMEM_GetNumData(SJ sj, Sint32 id);
void SJMEM_GetChunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck);
void SJMEM_PutChunk(SJ sj, Sint32 id, SJCK *ck);
void SJMEM_UngetChunk(SJ sj, Sint32 id, SJCK *ck);
Sint32 SJMEM_IsGetChunk(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte);
/* the data and the bsize that sj, a memory joint, was created with */
char *SJMEM_GetBufPtr(SJ sj);
Sint32 SJMEM_GetBufSize(SJ sj);

SJ_IF sjmem_vtbl = {
    NULL,
    NULL,
    NULL,
    SJMEM_Destroy,
    SJMEM_GetUuid,
    SJMEM_Reset,
    SJMEM_GetChunk,
    SJMEM_UngetChunk,
    SJMEM_PutChunk,
    SJMEM_GetNumData,
    SJMEM_IsGetChunk,
    SJMEM_EntryErrFunc,
};
/* the SJMEM_Init calls that no SJMEM_Finish has answered */
Sint32 sjmem_init_cnt = 0;
struct sjmem sjmem_obj[OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

/* mask every interrupt, and return the old mask for SJMEM_Unlock. mem is
 * not read */
Sint32 SJMEM_Lock(struct sjmem *mem)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    return mask;
}

void SJMEM_Unlock(struct sjmem *mem, Sint32 mask)
{
    set_imask(mask);
}

/* the error function SJMEM_Create sets, which never returns */
void SJMEM_Error(void *obj, Sint32 ecode)
{
    for (;;) {
    }
}

void SJMEM_Init(void)
{
    if (sjmem_init_cnt == 0) {
        memset(sjmem_obj, 0, sizeof(sjmem_obj));
    }
    sjmem_init_cnt++;
}

void SJMEM_Finish(void)
{
    sjmem_init_cnt--;
    if (sjmem_init_cnt == 0) {
        memset(sjmem_obj, 0, sizeof(sjmem_obj));
    }
}

SJ SJMEM_Create(char *data, Sint32 bsize)
{
    struct sjmem *mem;
    Sint32 i;

    for (i = 0; i < OBJ_MAX; i++) {
        if (sjmem_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == OBJ_MAX) {
        return NULL;
    }
    mem = &sjmem_obj[i];
    mem->used = TRUE;
    mem->sj.vtbl = &sjmem_vtbl;
    mem->buf = data;
    mem->bsize = bsize;
    mem->uuid = (UUID *)&sjmem_uuid;
    mem->err_func = SJMEM_Error;
    mem->err_obj = mem;
    SJMEM_Reset(&mem->sj);
    return &mem->sj;
}

void SJMEM_Destroy(SJ sj)
{
    struct sjmem *mem = (struct sjmem *)sj;

    if (mem != NULL) {
        memset(mem, 0, sizeof(*mem));
        /* @hack redundant: the store of FALSE to used, which the memset has
         * already made */
        mem->used = FALSE;
    }
}

UUID *SJMEM_GetUuid(SJ sj)
{
    return ((struct sjmem *)sj)->uuid;
}

void SJMEM_EntryErrFunc(SJ sj, void (*func)(void *obj, Sint32 ecode), void *obj)
{
    struct sjmem *mem = (struct sjmem *)sj;

    mem->err_func = func;
    mem->err_obj = obj;
}

void SJMEM_Reset(SJ sj)
{
    struct sjmem *mem = (struct sjmem *)sj;

    mem->ndata = mem->bsize;
    mem->data_pos = 0;
}

Sint32 SJMEM_GetNumData(SJ sj, Sint32 id)
{
    struct sjmem *mem = (struct sjmem *)sj;

    if (id == SJ_LIN_DATA) {
        return mem->ndata;
    }
    if (id == SJ_LIN_FREE) {
        return 0;
    }
    if (mem->err_func != NULL) {
        mem->err_func(mem->err_obj, SJ_ERR_PRM);
    }
    return 0;
}

void SJMEM_GetChunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck)
{
    struct sjmem *mem = (struct sjmem *)sj;
    Sint32 mask;
    Sint32 ndata;

    mask = SJMEM_Lock(mem);
    if (id == SJ_LIN_FREE) {
        ck->len = 0;
        ck->data = NULL;
    } else if (id == SJ_LIN_DATA) {
        /* @hack spelling: ndata, a copy of mem->ndata, where mem->ndata read
         * in the MIN would do */
        ndata = mem->ndata;
        ck->len = MIN(ndata, nbyte);
        ck->data = mem->buf + mem->data_pos;
        mem->data_pos += ck->len;
        mem->ndata -= ck->len;
    } else {
        ck->len = 0;
        ck->data = NULL;
        if (mem->err_func != NULL) {
            mem->err_func(mem->err_obj, SJ_ERR_PRM);
        }
    }
    SJMEM_Unlock(mem, mask);
}

/* a chunk put on either line joins neither. the data line holds only the
 * buffer's bytes, and the free line stays empty */
void SJMEM_PutChunk(SJ sj, Sint32 id, SJCK *ck)
{
    struct sjmem *mem = (struct sjmem *)sj;
    Sint32 mask;

    if (ck->len <= 0 || ck->data == NULL) {
        return;
    }
    mask = SJMEM_Lock(mem);
    /* @hack spelling: the two empty arms, where one test that id is neither
     * line would do */
    if (id == SJ_LIN_FREE) {
    } else if (id == SJ_LIN_DATA) {
    } else {
        ck->len = 0;
        ck->data = NULL;
        if (mem->err_func != NULL) {
            mem->err_func(mem->err_obj, SJ_ERR_PRM);
        }
    }
    SJMEM_Unlock(mem, mask);
}

/* on the data line, move the front back by ck->len bytes, but not before
 * buf. the line then holds ck->len bytes more, up to bsize. a ck that doesn't
 * start at the new front is reported, but the move stands. on the free line,
 * ck is reported and changes nothing */
void SJMEM_UngetChunk(SJ sj, Sint32 id, SJCK *ck)
{
    struct sjmem *mem = (struct sjmem *)sj;
    Sint32 mask;
    /* the line's front, moved back by ck->len bytes */
    Sint32 front;
    /* @hack spelling: start, a local for where ck starts in the buffer,
     * where the difference in the test would do */
    Sint32 start;

    if (ck->len <= 0 || ck->data == NULL) {
        return;
    }
    mask = SJMEM_Lock(mem);
    if (id == SJ_LIN_FREE) {
        if (mem->err_func != NULL) {
            mem->err_func(mem->err_obj, SJ_ERR_PRM);
        }
    } else if (id == SJ_LIN_DATA) {
        front = mem->data_pos - ck->len;
        front = MAX(front, 0);
        mem->data_pos = front;
        mem->ndata = MIN(mem->bsize, mem->ndata + ck->len);
        start = ck->data - mem->buf;
        if (front != start && mem->err_func != NULL) {
            mem->err_func(mem->err_obj, SJ_ERR_PRM);
        }
    } else {
        ck->len = 0;
        ck->data = NULL;
        if (mem->err_func != NULL) {
            mem->err_func(mem->err_obj, SJ_ERR_PRM);
        }
    }
    SJMEM_Unlock(mem, mask);
}

Sint32 SJMEM_IsGetChunk(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte)
{
    struct sjmem *mem = (struct sjmem *)sj;
    Sint32 mask;
    Sint32 n;

    mask = SJMEM_Lock(mem);
    if (id == SJ_LIN_FREE) {
        n = 0;
    } else if (id == SJ_LIN_DATA) {
        n = MIN(mem->ndata, nbyte);
    } else {
        n = 0;
        if (mem->err_func != NULL) {
            mem->err_func(mem->err_obj, SJ_ERR_PRM);
        }
    }
    *rbyte = n;
    SJMEM_Unlock(mem, mask);
    return n == nbyte;
}

char *SJMEM_GetBufPtr(SJ sj)
{
    return ((struct sjmem *)sj)->buf;
}

Sint32 SJMEM_GetBufSize(SJ sj)
{
    return ((struct sjmem *)sj)->bsize;
}
