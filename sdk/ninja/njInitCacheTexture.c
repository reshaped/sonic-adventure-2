/* the texture cache's allocator: a ring of blocks in the buffer that
 * njInitCacheTextureBuffer gives it */
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the mark every block's header starts with */
#define BLOCK_MARK 0xa5
/* the bytes ahead of the address a block hands out */
#define BLOCK_HEADER 32
/* a request's bytes, its header's included, round up to a multiple of this */
#define BLOCK_ALIGN 32
/* the most bytes a taken block holds past what its request needs. a free
 * block with more to spare is split, and the rest stays free */
#define SLACK_MAX 64

struct cache_block {
    Sint16 mark;
    Sint16 taken;
    Int size; /* in bytes, the header included */
    struct cache_block *next;
    struct cache_block *prev;
    Uint8 unk_10[20];
};

/* the ring's sentinel, outside the buffer and always taken, so that no block
 * merges with it */
static struct cache_block ring;
/* the buffer's first block */
static struct cache_block *first;

void njInitCacheTextureBuffer(Sint8 *addr, Uint32 size)
{
    struct cache_block *block = (struct cache_block *)addr;

    ring.size = sizeof(ring);
    ring.mark = BLOCK_MARK;
    ring.taken = TRUE;
    ring.next = ring.prev = block;
    block->size = size;
    block->mark = BLOCK_MARK;
    block->taken = FALSE;
    block->next = block->prev = &ring;
    /* @hack spelling: first read back from ring.next, where block would do */
    first = ring.next;
}

void *njCacheTextureMalloc(Uint32 size)
{
    struct cache_block *block;
    struct cache_block *rest;
    Int need = (size + BLOCK_HEADER + BLOCK_ALIGN - 1) & ~(BLOCK_ALIGN - 1);

    for (block = ring.next; block != &ring; block = block->next) {
        if (!block->taken && block->size >= need) {
            if (block->size - need > SLACK_MAX) {
                rest = (struct cache_block *)((Uint8 *)block + need);
                rest->size = block->size - need;
                rest->mark = BLOCK_MARK;
                rest->taken = FALSE;
                block->next->prev = rest;
                rest->next = block->next;
                block->next = rest;
                rest->prev = block;
                block->size = need;
            }
            block->taken = TRUE;
            return (Uint8 *)block + BLOCK_HEADER;
        }
    }
    return NULL;
}

void njCacheTextureFree(void *addr)
{
    struct cache_block *block =
        (struct cache_block *)((Uint8 *)addr - BLOCK_HEADER);

    if (block->mark == BLOCK_MARK) {
        if (!block->prev->taken) {
            /* @hack redundant: join, a label nothing jumps to */
        join: /* NOLINT(clang-diagnostic-unused-label) */
            block->next->prev = block->prev;
            block->prev->next = block->next;
            block->prev->size += block->size;
            block = block->prev;
        }
        if (!block->next->taken) {
            block->next->next->prev = block;
            block->size += block->next->size;
            block->next = block->next->next;
        }
        block->taken = FALSE;
    }
}
