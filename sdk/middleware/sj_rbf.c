/* CRI's ring buffer joints: stream joints whose two lines share one ring of
 * bytes */
#include <machine.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

/* how many ring buffer joints there are */
#define OBJ_MAX 64

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* a ring buffer joint. the ring is buf's first bsize bytes. each line is a
 * run of the ring's bytes from the line's front. a run wraps at the ring's
 * end */
struct sjrbf {
    SJ_OBJ sj;
    Bool used;
    UUID *uuid;
    Sint32 ndata; /* the bytes the data line holds */
    Sint32 nfree; /* the bytes the free line holds */
    Sint32 free_pos; /* the free line's front, in bytes from buf */
    Sint32 data_pos; /* the data line's front, in bytes from buf */
    char *buf;
    Sint32 bsize;
    Sint32 xsize; /* the bytes past the ring, which hold a copy of its start */
    /* the error function, or null, and the obj it is passed */
    void (*err_func)(void *obj, Sint32 ecode);
    void *err_obj;
};

static const UUID sjrbf_uuid = {
    0x3b9a9e81,
    0x0dbb,
    0x11d2,
    {0xa6, 0xbf, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},
};

/* the member exports these functions and the data down to sjrbf_obj, though
 * no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 SJRBF_Lock(struct sjrbf *rbf);
void SJRBF_Unlock(struct sjrbf *rbf, Sint32 mask);
void SJRBF_Error(void *obj, Sint32 ecode);
void SJRBF_Destroy(SJ sj);
UUID *SJRBF_GetUuid(SJ sj);
void SJRBF_EntryErrFunc(SJ sj, void (*func)(void *obj, Sint32 ecode),
                        void *obj);
void SJRBF_Reset(SJ sj);
Sint32 SJRBF_GetNumData(SJ sj, Sint32 id);
void SJRBF_GetChunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck);
void SJRBF_PutChunk(SJ sj, Sint32 id, SJCK *ck);
void SJRBF_UngetChunk(SJ sj, Sint32 id, SJCK *ck);
Sint32 SJRBF_IsGetChunk(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte);

const char *sj_build = "\nSJ Ver.5.53 Build:Feb 25 2000 10:11:30\n";
SJ_IF sjrbf_vtbl = {
    NULL,
    NULL,
    NULL,
    SJRBF_Destroy,
    SJRBF_GetUuid,
    SJRBF_Reset,
    SJRBF_GetChunk,
    SJRBF_UngetChunk,
    SJRBF_PutChunk,
    SJRBF_GetNumData,
    SJRBF_IsGetChunk,
    SJRBF_EntryErrFunc,
};
/* the SJRBF_Init calls that no SJRBF_Finish has answered */
Sint32 sjrbf_init_cnt = 0;
struct sjrbf sjrbf_obj[OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

/* mask every interrupt, and return the old mask for SJRBF_Unlock. rbf is
 * not read */
Sint32 SJRBF_Lock(struct sjrbf *rbf)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    return mask;
}

void SJRBF_Unlock(struct sjrbf *rbf, Sint32 mask)
{
    set_imask(mask);
}

/* the error function SJRBF_Create sets, which never returns */
void SJRBF_Error(void *obj, Sint32 ecode)
{
    for (;;) {
    }
}

void SJRBF_Init(void)
{
    if (sjrbf_init_cnt == 0) {
        memset(sjrbf_obj, 0, sizeof(sjrbf_obj));
    }
    sjrbf_init_cnt++;
}

void SJRBF_Finish(void)
{
    sjrbf_init_cnt--;
    if (sjrbf_init_cnt == 0) {
        memset(sjrbf_obj, 0, sizeof(sjrbf_obj));
    }
}

SJ SJRBF_Create(char *buf, Sint32 bsize, Sint32 xsize)
{
    struct sjrbf *rbf;
    Sint32 i;

    for (i = 0; i < OBJ_MAX; i++) {
        if (sjrbf_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == OBJ_MAX) {
        return NULL;
    }
    rbf = &sjrbf_obj[i];
    rbf->used = TRUE;
    rbf->sj.vtbl = &sjrbf_vtbl;
    rbf->buf = buf;
    rbf->bsize = bsize;
    rbf->xsize = xsize;
    rbf->uuid = (UUID *)&sjrbf_uuid;
    rbf->err_func = SJRBF_Error;
    rbf->err_obj = rbf;
    SJRBF_Reset(&rbf->sj);
    return &rbf->sj;
}

void SJRBF_Destroy(SJ sj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    if (rbf != NULL) {
        memset(rbf, 0, sizeof(struct sjrbf));
        /* @hack redundant: the store to used, which memset has just
         * cleared */
        rbf->used = FALSE;
    }
}

UUID *SJRBF_GetUuid(SJ sj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    return rbf->uuid;
}

void SJRBF_EntryErrFunc(SJ sj, void (*func)(void *obj, Sint32 ecode), void *obj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    rbf->err_func = func;
    rbf->err_obj = obj;
}

void SJRBF_Reset(SJ sj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    rbf->ndata = 0;
    rbf->nfree = rbf->bsize;
    rbf->free_pos = 0;
    rbf->data_pos = 0;
}

Sint32 SJRBF_GetNumData(SJ sj, Sint32 id)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    if (id == SJ_LIN_DATA) {
        return rbf->ndata;
    }
    if (id == SJ_LIN_FREE) {
        return rbf->nfree;
    }
    if (rbf->err_func != NULL) {
        rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
    }
    return 0;
}

void SJRBF_GetChunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;
    Sint32 mask;

    mask = SJRBF_Lock(rbf);
    /* a chunk starts at the line's front. it may run up to xsize bytes past
     * the ring's end, into the copy. the front then moves past the chunk,
     * and wraps at the ring's end */
    if (id == SJ_LIN_FREE) {
        /* @hack spelling: the first minimum stored in ck->len, where a local
         * for it would do */
        ck->len = MIN(rbf->nfree, rbf->bsize - rbf->free_pos + rbf->xsize);
        ck->len = MIN(ck->len, nbyte);
        ck->data = rbf->buf + rbf->free_pos;
        rbf->free_pos = (rbf->free_pos + ck->len) % rbf->bsize;
        rbf->nfree -= ck->len;
    } else if (id == SJ_LIN_DATA) {
        /* @hack spelling: the first minimum stored in ck->len, where a local
         * for it would do */
        ck->len = MIN(rbf->ndata, rbf->bsize - rbf->data_pos + rbf->xsize);
        ck->len = MIN(ck->len, nbyte);
        ck->data = rbf->buf + rbf->data_pos;
        rbf->data_pos = (rbf->data_pos + ck->len) % rbf->bsize;
        rbf->ndata -= ck->len;
    } else {
        ck->len = 0;
        ck->data = NULL;
        if (rbf->err_func != NULL) {
            rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
        }
    }
    SJRBF_Unlock(rbf, mask);
}

void SJRBF_PutChunk(SJ sj, Sint32 id, SJCK *ck)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;
    Sint32 mask;
    Sint32 start;
    Sint32 end;
    Sint32 len;

    if (ck->len <= 0 || ck->data == NULL) {
        return;
    }
    mask = SJRBF_Lock(rbf);
    if (id == SJ_LIN_DATA) {
        rbf->ndata += ck->len;
        start = ck->data - rbf->buf;
        /* the chunk's bytes in the ring's first xsize are copied to the
         * same offset past the ring's end */
        if (start < rbf->xsize) {
            len = MIN(ck->len, rbf->xsize - start);
            memcpy(&rbf->buf[rbf->bsize + start], ck->data, len);
        }
        /* @hack spelling: end computed from ck->data again, where start +
         * ck->len would do */
        end = ck->data - rbf->buf + ck->len;
        /* the chunk's bytes past the ring's end are copied to the ring's
         * start */
        if (end > rbf->bsize) {
            len = MIN(ck->len, end - rbf->bsize);
            /* @bug logic: copies a chunk that starts past the ring's end to
             * the ring's start, not as far into the ring as the chunk starts
             * past its end. only a chunk that starts in the ring, or at its
             * end, is copied right. the destination is always buf (MOV.L
             * @R15,R4 at 0x8c134562) */
            memcpy(rbf->buf, &rbf->buf[end - len], len);
        }
    } else if (id == SJ_LIN_FREE) {
        rbf->nfree += ck->len;
    } else {
        ck->len = 0;
        ck->data = NULL;
        if (rbf->err_func != NULL) {
            rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
        }
    }
    SJRBF_Unlock(rbf, mask);
}

void SJRBF_UngetChunk(SJ sj, Sint32 id, SJCK *ck)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;
    Sint32 mask;
    /* the line's front, moved back by ck->len bytes */
    Sint32 front;
    /* @hack spelling: start, a local for where ck starts in the ring, where
     * the expression in the test would do */
    Sint32 start;

    if (ck->len <= 0 || ck->data == NULL) {
        return;
    }
    mask = SJRBF_Lock(rbf);
    if (id == SJ_LIN_FREE) {
        front = (rbf->free_pos + rbf->bsize - ck->len) % rbf->bsize;
        start = (ck->data - rbf->buf) % rbf->bsize;
        if (front == start) {
            rbf->free_pos = front;
            rbf->nfree += ck->len;
        } else if (rbf->err_func != NULL) {
            rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
        }
    } else if (id == SJ_LIN_DATA) {
        front = (rbf->data_pos + rbf->bsize - ck->len) % rbf->bsize;
        start = (ck->data - rbf->buf) % rbf->bsize;
        if (front == start) {
            rbf->data_pos = front;
            rbf->ndata += ck->len;
        } else if (rbf->err_func != NULL) {
            rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
        }
    } else {
        ck->len = 0;
        ck->data = NULL;
        if (rbf->err_func != NULL) {
            rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
        }
    }
    SJRBF_Unlock(rbf, mask);
}

Sint32 SJRBF_IsGetChunk(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;
    Sint32 mask;
    Sint32 len;

    mask = SJRBF_Lock(rbf);
    if (id == SJ_LIN_FREE) {
        len = MIN(rbf->nfree, rbf->bsize - rbf->free_pos + rbf->xsize);
        len = MIN(len, nbyte);
    } else if (id == SJ_LIN_DATA) {
        len = MIN(rbf->ndata, rbf->bsize - rbf->data_pos + rbf->xsize);
        len = MIN(len, nbyte);
    } else {
        len = 0;
        if (rbf->err_func != NULL) {
            rbf->err_func(rbf->err_obj, SJ_ERR_PRM);
        }
    }
    *rbyte = len;
    SJRBF_Unlock(rbf, mask);
    return len == nbyte;
}

char *SJRBF_GetBufPtr(SJ sj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    return rbf->buf;
}

Sint32 SJRBF_GetBufSize(SJ sj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    return rbf->bsize;
}

Sint32 SJRBF_GetXtrSize(SJ sj)
{
    struct sjrbf *rbf = (struct sjrbf *)sj;

    return rbf->xsize;
}
