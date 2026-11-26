/* the buffer pool: max_mcb_entry pages of mcb_page_size bytes from
 * _streambuf. MP_Alloc cuts each into fragments of MP_MIN_SIZE bytes or more */
#include <access/at_types.h>
#include <access/ip_private.h>
#include <access/tcp_private.h>
#include <sg_xpt.h>
#define MP_FRAGMENTS                                                           \
    ((int)(sizeof(mcb_table->fragments) / sizeof(mcb_table->fragments[0])))
#define MP_MIN_SIZE 1024
/* a fragment's owner while its bytes are free, and while it holds none */
#define MP_FREE (-1)
#define MP_EMPTY 0
Uint32 mcb_page_size = AT_POOL_BYTES;
Uint32 mcb_struct_size = sizeof(struct memory_pool);
Uint8 *memplbuf;

Sint32 MP_Init(void)
{
    unsigned int page;
    unsigned int index;
    struct memory_pool *pool;

    memplbuf = _streambuf;
    for (page = 0; page < max_mcb_entry; page++) {
        pool = &mcb_table[page];
        pool->base = memplbuf;
        /* @hack type: the page size narrowed to a Uint16, where mcb_page_size
         * would do while it is below 65536 */
        memplbuf += (Uint16)mcb_page_size;
        pool->unowned_fragments = MP_FRAGMENTS;
        pool->free_size = mcb_page_size;
        pool->fragments[0].owner = MP_FREE;
        pool->fragments[0].offset = 0;
        pool->fragments[0].size = mcb_page_size;
        for (index = 1; index < MP_FRAGMENTS; index++) {
            pool->fragments[index].owner = MP_EMPTY;
            pool->fragments[index].offset = mcb_page_size;
            pool->fragments[index].size = 0;
        }
    }
    return 0;
}

Sint32 MP_Alloc(AT_Vptr *pointer, Sint32 size, Sint32 kind)
{
    unsigned int page;
    int index;
    struct memory_pool *pool;

#ifdef NONMATCHING
    /* this side refuses a missing pointer, a negative size, and a kind outside
     * 1 to 32767, which a fragment's Sint16 owner holds apart from MP_FREE and
     * MP_EMPTY */
    if (!pointer || size < 0 || kind <= 0 || kind > 32767) {
        return -1;
    }
#endif
    if (size < MP_MIN_SIZE) {
        size = MP_MIN_SIZE;
    }
    for (page = 0; page < max_mcb_entry; page++) {
        pool = &mcb_table[page];
        if (pool->unowned_fragments == 0 || size > pool->free_size) {
            continue;
        }
        for (index = 0; index < MP_FRAGMENTS; index++) {
            if (pool->fragments[index].owner == MP_EMPTY) {
                continue;
            }
            /* @hack type: size compared as unsigned, where its own type
             * would do while it is MP_MIN_SIZE or more */
            if (pool->fragments[index].owner == MP_FREE &&
                pool->fragments[index].size >= (unsigned int)size) {
                pointer->buff = ((Uint32)page << MP_PAGE_SHIFT) +
                                ((Uint32)index << MP_FRAGMENT_SHIFT);
                pointer->type = AT_PTR_TYPE_POOL;
                pool->fragments[index].owner = kind;
                if (index + 1 < MP_FRAGMENTS &&
                    pool->fragments[index].size - size != 0) {
                    if (pool->fragments[index + 1].owner == MP_EMPTY) {
                        pool->fragments[index + 1].offset =
                            pool->fragments[index].offset + size;
                        pool->fragments[index + 1].size =
                            pool->fragments[index].size - size;
                        pool->fragments[index + 1].owner = MP_FREE;
                        pool->fragments[index].size = size;
                    } else {
                        /* @hack redundant: the branch for a next fragment in
                         * use, which does nothing */
                    }
                }
                pool->unowned_fragments--;
                pool->free_size -= pool->fragments[index].size;
                return 0;
            }
        }
    }
    /* @hack redundant: the tests of kind, whose branches are empty */
    if (kind == MP_TCP_WINDOW) {
    } else if (kind == MP_IP_FRAGMENT) {
    }
    return -1;
}

Sint32 MP_Free(AT_Vptr *pointer)
{
    unsigned int page;
    int index;
    int next;
    int prev;
    struct memory_pool *pool;

#ifdef NONMATCHING
    /* this side refuses a missing pointer and one that names no pool
     * fragment */
    if (!pointer || pointer->type != AT_PTR_TYPE_POOL) {
        return -1;
    }
#endif
    page = pointer->buff >> MP_PAGE_SHIFT;
    index = (Uint8)(pointer->buff >> MP_FRAGMENT_SHIFT);
    /* @bug memory: reads and writes one past each table, for a page index of
     * max_mcb_entry or a fragment index of MP_FRAGMENTS. the tests let both
     * through (0x0cadb11e, 0x0cadb128) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it refuses a page of
     * max_mcb_entry and a fragment of MP_FRAGMENTS */
    if (page >= max_mcb_entry || index >= MP_FRAGMENTS) {
#else
    if (page > max_mcb_entry || index > MP_FRAGMENTS) {
#endif
        return -1;
    }
    pool = &mcb_table[page];
    /* @bug logic: frees a fragment that holds no bytes as if it were
     * allocated. the test lets through any owner but MP_FREE (0x0cadb148) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it refuses a fragment that holds
     * no bytes, as it does a free one */
    if (pool->fragments[index].owner <= MP_EMPTY) {
#else
    if (pool->fragments[index].owner == MP_FREE) {
#endif
        return -1;
    }
    pool->fragments[index].owner = MP_FREE;
    pool->unowned_fragments++;
    pool->free_size += pool->fragments[index].size;
    if (index == MP_FRAGMENTS - 1 ||
        pool->fragments[index + 1].owner != MP_FREE) {
        next = index;
    } else {
        next = index + 1;
    }
    for (prev = next - 1; prev >= 0 && pool->fragments[prev].owner <= MP_EMPTY;
         next--, prev--) {
        if (pool->fragments[prev].owner == MP_EMPTY) {
            pool->fragments[prev].owner = MP_FREE;
            pool->fragments[next].owner = MP_EMPTY;
            pool->fragments[prev].offset = pool->fragments[next].offset;
            pool->fragments[prev].size = pool->fragments[next].size;
            pool->fragments[next].offset += pool->fragments[prev].size;
            pool->fragments[next].size = 0;
        } else if (pool->fragments[prev].owner == MP_FREE) {
            pool->fragments[next].owner = MP_EMPTY;
            pool->fragments[prev].size += pool->fragments[next].size;
            pool->fragments[next].size = 0;
            pool->fragments[next].offset =
                pool->fragments[prev].offset + pool->fragments[prev].size;
        } else {
            break;
        }
    }
    return 0;
}
