/* CRI's general joints: stream joints whose lines are lists of the chunks
 * put on them */
#include <machine.h>
#include <sg_xpt.h>
#include <sj.h>
#include <string.h>

/* how many general joints there are, and how many lines each has */
#define OBJ_MAX 64
#define LIN_MAX 4

/* a chunk on a line, or a free node of the pool */
struct sjuni_node {
    struct sjuni_node *next; /* null at a list's end */
    Sint32 unk_04;
    SJCK ck;
};

/* a general joint. its nodes lie in work, the caller's storage. each line
 * is a list of nodes, from its front chunk to its last. pool is the list of
 * the free nodes */
struct sjuni {
    SJ_OBJ sj;
    Sint8 used; /* TRUE while the joint is in use */
    Sint8 mode; /* SJUNI_MODE_SEPA or SJUNI_MODE_JOIN */
    UUID *uuid;
    struct sjuni_node *work;
    Sint32 nnode; /* the nodes work holds */
    struct sjuni_node *pool;
    struct sjuni_node *line[LIN_MAX];
    /* the error function, or null, and the obj it is passed */
    void (*err_func)(void *obj, Sint32 ecode);
    void *err_obj;
};

static const UUID sjuni_uuid = {
    0x2e534fa3,
    0xaf97,
    0x11d2,
    {0xa5, 0x27, 0x00, 0x60, 0x08, 0x94, 0x48, 0xbc},
};

/* the member exports these functions and the data down to sjuni_obj, though
 * no header of the SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 SJUNI_Lock(struct sjuni *uni);
void SJUNI_Unlock(struct sjuni *uni, Sint32 mask);
void SJUNI_Error(void *obj, Sint32 ecode);
void SJUNI_Destroy(SJ sj);
UUID *SJUNI_GetUuid(SJ sj);
void SJUNI_EntryErrFunc(SJ sj, void (*func)(void *obj, Sint32 ecode),
                        void *obj);
void SJUNI_Reset(SJ sj);
Sint32 SJUNI_GetNumData(SJ sj, Sint32 id);
void SJUNI_GetChunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck);
void SJUNI_PutChunk(SJ sj, Sint32 id, SJCK *ck);
void SJUNI_UngetChunk(SJ sj, Sint32 id, SJCK *ck);
Sint32 SJUNI_IsGetChunk(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte);

SJ_IF sjuni_vtbl = {
    NULL,
    NULL,
    NULL,
    SJUNI_Destroy,
    SJUNI_GetUuid,
    SJUNI_Reset,
    SJUNI_GetChunk,
    SJUNI_UngetChunk,
    SJUNI_PutChunk,
    SJUNI_GetNumData,
    SJUNI_IsGetChunk,
    SJUNI_EntryErrFunc,
};
/* the SJUNI_Init calls that no SJUNI_Finish has answered */
Sint32 sjuni_init_cnt = 0;
struct sjuni sjuni_obj[OBJ_MAX];
/* NOLINTEND(misc-use-internal-linkage) */

/* mask every interrupt, and return the old mask for SJUNI_Unlock. uni is
 * not read */
Sint32 SJUNI_Lock(struct sjuni *uni)
{
    Sint32 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    return mask;
}

void SJUNI_Unlock(struct sjuni *uni, Sint32 mask)
{
    set_imask(mask);
}

/* the error function SJUNI_Create sets, which never returns */
void SJUNI_Error(void *obj, Sint32 ecode)
{
    for (;;) {
    }
}

void SJUNI_Init(void)
{
    if (sjuni_init_cnt == 0) {
        memset(sjuni_obj, 0, sizeof(sjuni_obj));
    }
    sjuni_init_cnt++;
}

void SJUNI_Finish(void)
{
    sjuni_init_cnt--;
    if (sjuni_init_cnt == 0) {
        memset(sjuni_obj, 0, sizeof(sjuni_obj));
    }
}

SJ SJUNI_Create(Sint32 mode, char *work, Sint32 wksize)
{
    struct sjuni *uni;
    Sint32 i;

    for (i = 0; i < OBJ_MAX; i++) {
        if (sjuni_obj[i].used == FALSE) {
            break;
        }
    }
    if (i == OBJ_MAX) {
        return NULL;
    }
    uni = &sjuni_obj[i];
    uni->used = TRUE;
    uni->mode = mode;
    uni->sj.vtbl = &sjuni_vtbl;
    /* @hack redundant: a second store of mode */
    uni->mode = mode;
    uni->uuid = (UUID *)&sjuni_uuid;
    uni->work = (struct sjuni_node *)work;
    uni->nnode = wksize / sizeof(struct sjuni_node);
    uni->err_func = SJUNI_Error;
    uni->err_obj = uni;
    SJUNI_Reset(&uni->sj);
    return &uni->sj;
}

void SJUNI_Destroy(SJ sj)
{
    struct sjuni *uni = (struct sjuni *)sj;

    if (uni != NULL) {
        memset(uni, 0, sizeof(struct sjuni));
        /* @hack redundant: the store to used, which memset has just
         * cleared */
        uni->used = FALSE;
    }
}

UUID *SJUNI_GetUuid(SJ sj)
{
    struct sjuni *uni = (struct sjuni *)sj;

    return uni->uuid;
}

void SJUNI_EntryErrFunc(SJ sj, void (*func)(void *obj, Sint32 ecode), void *obj)
{
    struct sjuni *uni = (struct sjuni *)sj;

    uni->err_func = func;
    uni->err_obj = obj;
}

/* put every node in the pool, in work's order, and empty each line. the
 * stores after the loop end the pool at its last node. where nnode is 0,
 * they still write work's first node, which the pool then holds */
void SJUNI_Reset(SJ sj)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *work;
    Sint32 i;

    work = uni->work;
    uni->pool = work;
    for (i = 0; i < uni->nnode - 1; i++) {
        work[i].next = &work[i + 1];
        work[i].ck.data = NULL;
        work[i].ck.len = 0;
    }
    work[i].next = NULL;
    work[i].ck.data = NULL;
    work[i].ck.len = 0;
    for (i = 0; i < LIN_MAX; i++) {
        uni->line[i] = NULL;
    }
}

Sint32 SJUNI_GetNumData(SJ sj, Sint32 id)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *node;
    Sint32 nbyte;

    if (id < 0 || id >= LIN_MAX) {
        if (uni->err_func != NULL) {
            uni->err_func(uni->err_obj, SJ_ERR_PRM);
        }
        return 0;
    }
    nbyte = 0;
    for (node = uni->line[id]; node != NULL; node = node->next) {
        nbyte += node->ck.len;
    }
    return nbyte;
}

void SJUNI_GetChunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *node;
    Sint32 mask;
    /* @hack spelling: ck1 and ck2, local copies of the front chunk and of
     * the part a split leaves on the line, where node->ck read in place and
     * SJ_SplitChunk(&node->ck, nbyte, ck, &node->ck) would do */
    SJCK ck1;
    SJCK ck2;

    if (id < 0 || id >= LIN_MAX) {
        if (uni->err_func != NULL) {
            uni->err_func(uni->err_obj, SJ_ERR_PRM);
        }
        ck->data = NULL;
        ck->len = 0;
        return;
    }
    mask = SJUNI_Lock(uni);
    node = uni->line[id];
    if (node == NULL) {
        ck->data = NULL;
        ck->len = 0;
    } else {
        ck1 = node->ck;
        if (ck1.len <= nbyte) {
            *ck = ck1;
            uni->line[id] = node->next;
            node->next = uni->pool;
            uni->pool = node;
        } else if (uni->mode == SJUNI_MODE_JOIN) {
            SJ_SplitChunk(&ck1, nbyte, &ck1, &ck2);
            *ck = ck1;
            node->ck = ck2;
        } else {
            ck->data = NULL;
            ck->len = 0;
        }
    }
    SJUNI_Unlock(uni, mask);
}

void SJUNI_PutChunk(SJ sj, Sint32 id, SJCK *ck)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node **link;
    struct sjuni_node *last;
    struct sjuni_node *node;
    Sint32 mask;

    if (id < 0 || id >= LIN_MAX) {
        if (uni->err_func != NULL) {
            uni->err_func(uni->err_obj, SJ_ERR_PRM);
        }
        return;
    }
    if (ck->len <= 0 || ck->data == NULL) {
        return;
    }
    mask = SJUNI_Lock(uni);
    last = NULL;
    for (link = &uni->line[id]; *link != NULL; link = &(*link)->next) {
        last = *link;
    }
    if (uni->mode == SJUNI_MODE_JOIN && last != NULL &&
        last->ck.data + last->ck.len == ck->data) {
        last->ck.len += ck->len;
    } else {
        node = uni->pool;
        if (node == NULL) {
            if (uni->err_func != NULL) {
                uni->err_func(uni->err_obj, SJ_ERR_PRM);
            }
        } else {
            uni->pool = node->next;
            node->next = NULL;
            node->ck = *ck;
            *link = node;
        }
    }
    SJUNI_Unlock(uni, mask);
}

void SJUNI_UngetChunk(SJ sj, Sint32 id, SJCK *ck)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *node;
    Sint32 mask;

    if (id < 0 || id >= LIN_MAX) {
        if (uni->err_func != NULL) {
            uni->err_func(uni->err_obj, SJ_ERR_PRM);
        }
        return;
    }
    if (ck->len <= 0 || ck->data == NULL) {
        return;
    }
    mask = SJUNI_Lock(uni);
    node = uni->line[id];
    if (uni->mode == SJUNI_MODE_JOIN && node != NULL &&
        ck->data + ck->len == node->ck.data) {
        node->ck.data = ck->data;
        node->ck.len += ck->len;
    } else {
        node = uni->pool;
        if (node == NULL) {
            if (uni->err_func != NULL) {
                uni->err_func(uni->err_obj, SJ_ERR_PRM);
            }
        } else {
            uni->pool = node->next;
            /* @hack redundant: the null stored in node->next, which the
             * store after the copy replaces */
            node->next = NULL;
            node->ck = *ck;
            node->next = uni->line[id];
            uni->line[id] = node;
        }
    }
    SJUNI_Unlock(uni, mask);
}

Sint32 SJUNI_IsGetChunk(SJ sj, Sint32 id, Sint32 nbyte, Sint32 *rbyte)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *node;
    /* @hack spelling: ck, a local copy of the front chunk, where node->ck
     * in its place would do */
    SJCK ck;

    *rbyte = 0;
    if (id < 0 || id >= LIN_MAX) {
        if (uni->err_func != NULL) {
            uni->err_func(uni->err_obj, SJ_ERR_PRM);
        }
        return FALSE;
    }
    node = uni->line[id];
    if (node == NULL) {
        return FALSE;
    }
    ck = node->ck;
    *rbyte = ck.len;
    /* @hack spelling: TRUE returned under each mode's test and FALSE after
     * both, where returning each comparison would do */
    if (uni->mode == SJUNI_MODE_JOIN) {
        if (ck.len >= nbyte) {
            return TRUE;
        }
    } else if (ck.len == nbyte) {
        return TRUE;
    }
    return FALSE;
}

Sint32 SJUNI_GetNumChunk(SJ sj, Sint32 id)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *node;
    Sint32 num;

    /* @hack spelling: num set in the for statement's first clause, where a
     * statement of its own before the loop would do */
    for (node = uni->line[id], num = 0; node != NULL; node = node->next) {
        num++;
    }
    return num;
}

Sint32 SJUNI_GetNumChainPool(SJ sj)
{
    struct sjuni *uni = (struct sjuni *)sj;
    struct sjuni_node *node;
    Sint32 num;

    /* @hack spelling: num set in the for statement's first clause, where a
     * statement of its own before the loop would do */
    for (node = uni->pool, num = 0; node != NULL; node = node->next) {
        num++;
    }
    return num;
}
