/* Kamui's video memory allocator, for frame buffers, textures and the pages
 * small VQ textures share. its offsets are in video memory's 64-bit area,
 * which it splits into two banks. each bank lists its free blocks and its used
 * ones. a pair is two blocks of one size at the same offset into each bank */
#include <kamui.h>
#include <kamui_buffer.h>
#include <kamui_internal.h>
#include <kamui_texture.h>

/* the console's 8 MB of video memory, half of it, and the 16 MB the profile's
 * EnableTexture16MB gives */
#define MEMORY_BYTES 0x800000
#define HALF_MEMORY 0x400000
#define MEMORY_16MB_BYTES 0x1000000
/* the 64-bit area alternates the memory's two halves every LANE_BYTES */
#define LANE_BYTES 4
/* block sizes, and the offsets a search tries, are multiples of ALIGNMENT */
#define ALIGNMENT 32
/* mipmaps and VQ pages are placed by the boundaries BOUNDARY bytes apart */
#define BOUNDARY 0x800
/* a 16-bit mipmap's 32 by 32 level starts MIP_32X32 bytes in, after the
 * smaller levels */
#define MIP_32X32 0x2b0
/* the codebook bytes a VQ page's slots hold between them */
#define CODEBOOK_BYTES 0x800
/* move_frame copies CHUNK bytes at a time through a buffer, which it purges a
 * CACHE_LINE at a time. a line holds CACHE_LINE_WORDS KMDWORDs */
#define CHUNK 0x800
#define CACHE_LINE 32
#define CACHE_LINE_WORDS 8
/* the records each pool holds, and the slots a VQ page has */
#define BLOCK_COUNT 4096
#define PAGE_COUNT 1024
#define PAGE_SLOTS 16

/* a block's flags. TAKEN marks a record that take_block handed out. a PAIRED
 * block has a partner at the same offset into the other bank. an OCCUPIED
 * block holds its owner: a surface, a VQ page or a TA buffer. an idle block
 * is a PAIRED one that isn't OCCUPIED, and it stays on its used list. the
 * other flags say what the block is for, and in which bank. allocate_at gives
 * a FIXED_BANK_ block the offset its caller names, and allocate_vq_at a
 * FIXED_PAGE_BANK_ block */
#define TAKEN 1
#define PAIRED 2
#define OCCUPIED 4
#define FRAME_BUFFER 8
#define TEXTURE_BANK_0 0x20
#define TEXTURE_BANK_1 0x40
#define FIXED_BANK_0 0x80
#define FIXED_BANK_1 0x100
#define PAGE_BANK_0 0x200
#define PAGE_BANK_1 0x400
#define FIXED_PAGE_BANK_0 0x800
#define FIXED_PAGE_BANK_1 0x1000
#define TA_BUFFER 0x2000

/* a page's flags: TAKEN, PAGE_HAS_TEXTURE and its kind. PAGE_HAS_TEXTURE
 * marks a page once a slot has held a texture. the kind gives the textures'
 * width and whether they have mipmaps */
#define PAGE_HAS_TEXTURE 2
#define PAGE_KIND 0xf000
enum page_kind {
    PAGE_8 = 0x1000,
    PAGE_MIP_8 = 0x2000,
    PAGE_16 = 0x3000,
    PAGE_MIP_16 = 0x4000,
    PAGE_32 = 0x5000,
    PAGE_MIP_32 = 0x6000,
    PAGE_64 = 0x7000,
    PAGE_MIP_64 = 0x8000
};

/* a block small VQ textures of one kind share, a slot each. slot i's texture
 * starts i codebook strides past offset */
struct vq_page {
    unsigned short flags;
    struct vq_page *next;
    KMINT32 offset;
    PKMSURFACEDESC slot[PAGE_SLOTS];
};

/* a run of one bank's video memory, free or used */
struct frame_block {
    unsigned short flags;
    struct frame_block *previous, *next;
    KMINT32 offset, bytes;
    union {
        PKMSURFACEDESC surface;
        struct vq_page *page;
        KMDWORD *ta_offset; /* kmiTAallocEx's first, which took the offset */
    } owner;
};

struct frame_bank {
    struct frame_block *used_first, *used_last, *free_first, *free_last;
    KMDWORD free_bytes;
};

struct vq_layout {
    KMDWORD kind, codebook_stride, texel_bytes, page_bytes, slots;
};

KMBOOLEAN KM_TEXTURE_ONE_BANK = 0;
KMDWORD KM_TEXTURE_BANK_SIZE = MEMORY_BYTES;
static KMINT32 first_base = 0;
static KMINT32 first_span = HALF_MEMORY;
static KMINT32 second_base = HALF_MEMORY;
/* @hack type: second_span unsigned, where KMINT32, first_span's type, would
 * do */
static KMDWORD second_span = HALF_MEMORY;
static struct frame_bank bank[2] = {{0}};
static struct vq_page *pages = 0;
/* the 64-bit offset of the block kmiFBalloc took last, and the bytes of each
 * frame buffer in it. its second frame buffer waits for the next request of
 * that size. both are 0 while none waits */
static KMDWORD pending_frame = 0;
static struct {
    KMDWORD bytes;
    KMDWORD reserved[2];
} pending_size = {0};
static struct frame_block block_pool[BLOCK_COUNT];
static struct vq_page page_pool[PAGE_COUNT];

static KMINT32 mipmap_offset(struct frame_block *block, KMDWORD bytes,
                             PKMSURFACEDESC surface);
static KMINT32 find_pair(KMINT32 bytes, PKMSURFACEDESC surface);
static KMSTATUS check_free(short which, KMINT32 offset, KMINT32 bytes);
static KMSTATUS allocate_at(short which, KMINT32 offset, KMINT32 bytes,
                            PKMSURFACEDESC surface, KMBOOLEAN paired,
                            KMBOOLEAN occupied);
static KMSTATUS allocate_frame(short which, KMINT32 bytes, void *owner,
                               short kind);
static KMSTATUS allocate_texture(short which, KMINT32 bytes,
                                 PKMSURFACEDESC surface);
static KMSTATUS allocate_vq(short which, KMINT32 bytes, struct vq_page *page);
static struct frame_block *find_idle(short which, KMINT32 bytes,
                                     PKMSURFACEDESC surface);
static struct frame_block *find_block(KMINT32 offset, short *which);
static KMSTATUS release_block(short which, struct frame_block *block);
static struct frame_block *take_block(void);
static void drop_block(struct frame_block *block);
static void prepend_block(struct frame_block **first, struct frame_block **last,
                          struct frame_block *block);
static void insert_after(struct frame_block **tail, struct frame_block *block,
                         struct frame_block *previous);
static void unlink_block(struct frame_block **first, struct frame_block **last,
                         struct frame_block *block);
static void move_frame(KMDWORD destination, KMDWORD source, KMDWORD bytes);
static void sort_blocks(struct frame_block **first, struct frame_block **last);
static KMSTATUS compact_blocks(struct frame_block **free_first,
                               struct frame_block **free_last,
                               struct frame_block **used_first,
                               struct frame_block **used_last);
static KMSTATUS allocate_vq_at(short which, KMINT32 offset, KMINT32 bytes,
                               struct vq_page *page, KMBOOLEAN paired,
                               KMBOOLEAN occupied);
static KMBOOLEAN vq_shape(struct vq_layout *shape, PKMSURFACEDESC surface);
static struct vq_page *take_vq(void);
static void drop_vq(struct vq_page *page);
static void prepend_vq(struct vq_page **first, struct vq_page *page);
static void unlink_vq(struct vq_page **first, struct vq_page *page);
static struct frame_block *find_idle_vq(short which, KMINT32 bytes,
                                        struct vq_page *page);
static KMINT32 find_pair_space(KMINT32 bytes);
static KMDWORD to_32bit(KMDWORD offset);
static KMDWORD to_64bit(KMDWORD offset);

#define CLEAR_LIST(head, node, next_node, release)                             \
    {                                                                          \
        (node) = (head);                                                       \
        while (node) {                                                         \
            (next_node) = (node)->next;                                        \
            release(node);                                                     \
            (node) = (next_node);                                              \
        }                                                                      \
    }
#define CLEAR_BANK(i)                                                          \
    {                                                                          \
        bank[i].used_first = 0;                                                \
        bank[i].used_last = 0;                                                 \
        bank[i].free_first = 0;                                                \
        bank[i].free_last = 0;                                                 \
        bank[i].free_bytes = 0;                                                \
    }

KMSTATUS kmiFBreset(void)
{
    struct frame_block *block;
    struct frame_block *next_block;
    struct vq_page *page;
    struct vq_page *next_page;
    KMINT32 i;
    CLEAR_LIST(bank[0].free_first, block, next_block, drop_block);
    CLEAR_LIST(bank[0].used_first, block, next_block, drop_block);
    CLEAR_LIST(bank[1].free_first, block, next_block, drop_block);
    CLEAR_LIST(bank[1].used_first, block, next_block, drop_block);
    for (i = 0; i < BLOCK_COUNT; i++) {
        block_pool[i].flags = 0;
    }
    CLEAR_BANK(0);
    CLEAR_BANK(1);
    CLEAR_LIST(pages, page, next_page, drop_vq);
    pages = 0;
    /* @bug logic: kmiFBreset frees every block, but leaves pending_frame and
     * pending_size as they were. the next kmiFBalloc of that size then hands
     * out the freed block's second frame buffer (the return at 0x8c167e3c,
     * and the use at 0x8c1680d0) */
#ifdef NONMATCHING
    pending_frame = 0;
    pending_size.bytes = 0;
#endif
    return KMSTATUS_SUCCESS;
}

#define SET_BANKS(total, first, second)                                        \
    {                                                                          \
        kmiDeviceExtension.texture_memory_size = (total);                      \
        KM_TEXTURE_BANK_SIZE = (first);                                        \
        first_base = 0;                                                        \
        first_span = (first);                                                  \
        second_base = (first);                                                 \
        second_span = (second);                                                \
        kmiWriteRegister(KMI_TEXTURE_CONTROL, 0);                              \
    }
#define SEED_BANK(i, base, size)                                               \
    {                                                                          \
        block = take_block();                                                  \
        if (!block)                                                            \
            return KMSTATUS_NOT_ENOUGH_MEMORY;                                 \
        block->offset = (base);                                                \
        block->bytes = (size);                                                 \
        block->next = bank[i].free_first;                                      \
        bank[i].free_first = block;                                            \
        block->previous = bank[i].free_last;                                   \
        bank[i].free_last = block;                                             \
    }

KMSTATUS kmiFBinit(void)
{
    struct frame_block *block;
    if (KM_TEXTURE_ONE_BANK == 1) {
        if (kmiGetProfileInt("HARDWARE", "EnableTexture16MB", 0) == 1) {
            SET_BANKS(MEMORY_16MB_BYTES, MEMORY_16MB_BYTES, 0);
        } else {
            SET_BANKS(MEMORY_BYTES, MEMORY_BYTES, 0);
        }
        kmiFBreset();
        bank[0].free_bytes = first_span;
        bank[1].free_bytes = second_span;
        SEED_BANK(0, first_base, first_span);
        return KMSTATUS_SUCCESS;
    }
    if (kmiGetProfileInt("HARDWARE", "EnableTexture16MB", 0) == 1) {
        SET_BANKS(MEMORY_16MB_BYTES, MEMORY_BYTES, MEMORY_BYTES);
    } else {
        SET_BANKS(MEMORY_BYTES, HALF_MEMORY, HALF_MEMORY);
    }
    kmiFBreset();
    bank[0].free_bytes = first_span;
    bank[1].free_bytes = second_span;
    SEED_BANK(0, first_base, first_span);
    SEED_BANK(1, second_base, second_span);
    return KMSTATUS_SUCCESS;
}

#define ALIGN_UP(size)                                                         \
    {                                                                          \
        KMINT32 remainder = (size) % ALIGNMENT;                                \
        if (remainder)                                                         \
            (size) += ALIGNMENT - remainder;                                   \
    }

/* give surface a frame buffer of bytes. a call takes one block of twice
 * bytes, whose two lanes hold two frame buffers. the first starts at the
 * block's offset, and the second LANE_BYTES on. the call sets
 * surface->pSurface to the first's 32-bit offset. the next call of the same
 * size sets it to the second's, and takes no block. the result is the
 * allocation's status */
KMSTATUS kmiFBalloc(KMINT32 bytes, PKMSURFACEDESC surface)
{
    KMSTATUS status;
    KMDWORD offset;
    ALIGN_UP(bytes);
    if (pending_size.bytes != (KMDWORD)bytes) {
        if (kmiDeviceExtension.capabilities & KMI_CHOOSE_BANK) {
            if (bank[0].free_bytes < bank[1].free_bytes) {
                status = allocate_frame(1, bytes * 2, surface, FRAME_BUFFER);
            } else {
                status = allocate_frame(0, bytes * 2, surface, FRAME_BUFFER);
            }
        } else {
            status = allocate_frame(0, bytes * 2, surface, FRAME_BUFFER);
        }
        if (!status) {
            pending_size.bytes = bytes;
            pending_frame = (KMDWORD)surface->pSurface;
            offset = (KMDWORD)surface->pSurface;
            surface->uSize.dwFrameBufferSize = bytes;
            surface->pSurface = (KMDWORD *)to_32bit(offset);
        }
        return status;
    } else {
        surface->uSize.dwFrameBufferSize = bytes;
        surface->pSurface = (KMDWORD *)to_32bit(pending_frame + LANE_BYTES);
        pending_size.bytes = 0;
        pending_frame = 0;
        return KMSTATUS_SUCCESS;
    }
}

KMSTATUS kmiFBallocEx(KMINT32 bytes, PKMSURFACEDESC first,
                      PKMSURFACEDESC second)
{
    KMSTATUS status;
    KMDWORD offset;
    ALIGN_UP(bytes);
    if (kmiDeviceExtension.capabilities & KMI_CHOOSE_BANK) {
        if (bank[0].free_bytes <= bank[1].free_bytes) {
            status = allocate_frame(1, bytes * 2, first, FRAME_BUFFER);
        } else {
            status = allocate_frame(0, bytes * 2, first, FRAME_BUFFER);
        }
    } else {
        status = allocate_frame(0, bytes * 2, first, FRAME_BUFFER);
    }
    if (!status) {
        offset = (KMDWORD)first->pSurface;
        first->uSize.dwFrameBufferSize = bytes;
        first->pSurface = (KMDWORD *)to_32bit(offset);
        second->uSize.dwFrameBufferSize = bytes;
        second->pSurface = (KMDWORD *)to_32bit(offset + LANE_BYTES);
    }
    return status;
}

KMSTATUS kmiTAallocEx(KMINT32 bytes, KMDWORD *first, KMDWORD *second)
{
    KMSTATUS status;
    KMDWORD offset;
    ALIGN_UP(bytes);
    status = allocate_frame(0, bytes * 2, first, TA_BUFFER);
    if (!status) {
        offset = *first;
        *first = to_32bit(offset);
        *second = to_32bit(offset + LANE_BYTES);
    }
    return status;
}

#define ALLOC_TEXTURE_BANK(allocate, size, owner)                              \
    {                                                                          \
        if (kmiDeviceExtension.capabilities & KMI_CHOOSE_BANK) {               \
            kmiFBgetfreememEx(&first, &second);                                \
            if (first < second)                                                \
                status = allocate(1, size, owner);                             \
            else                                                               \
                status = allocate(0, size, owner);                             \
        } else {                                                               \
            status = allocate(1, size, owner);                                 \
        }                                                                      \
    }

KMSTATUS kmiTXalloc(KMINT32 bytes, PKMSURFACEDESC surface)
{
    KMINT32 first;
    KMINT32 second;
    KMSTATUS status;
    ALLOC_TEXTURE_BANK(allocate_texture, bytes, surface);
    return status;
}

KMSTATUS kmiCBalloc(KMINT32 bytes, PKMSURFACEDESC first, PKMSURFACEDESC second)
{
    KMINT32 offset = find_pair(bytes, first);
    KMSTATUS status;
    if (offset == -1) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    status = allocate_at(0, offset, bytes, first, 1, 1);
    if (status) {
        return status;
    }
    status = allocate_at(1, offset + first_span, bytes, second, 1, 1);
    if (status) {
        kmiFBfree((KMINT32)first->pSurface);
    }
    return status;
}

KMSTATUS kmiABSalloc(KMINT32 bytes, PKMSURFACEDESC surface)
{
    KMINT32 offset = (KMINT32)surface->pSurface;
    KMSTATUS status;
    if (offset < second_base) {
        status = allocate_at(0, offset, bytes, surface, 0, 1);
    } else {
        status = allocate_at(1, offset, bytes, surface, 0, 1);
    }
    return status;
}

KMSTATUS kmiMIPalloc(KMINT32 bytes, PKMSURFACEDESC surface)
{
    KMINT32 offset;
    KMSTATUS status;
    struct frame_block *block;
    KMSURFACEDESC partner;
    ALIGN_UP(bytes);
    block = find_idle(0, bytes, surface);
    if (!block) {
        block = find_idle(1, bytes, surface);
    }
    if (block) {
        surface->pSurface = (KMDWORD *)block->offset;
        block->owner.surface = surface;
        block->flags |= OCCUPIED;
        return KMSTATUS_SUCCESS;
    }
    /* @hack redundant: a call that never runs, the only call of
     * find_pair_space */
    if (0) {
        find_pair_space(bytes);
    }
    offset = find_pair(bytes, surface);
    if (offset == -1) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    status = allocate_at(0, offset, bytes, surface, 1, 1);
    if (!status) {
        status = allocate_at(1, offset + first_span, bytes, &partner, 1, 0);
        /* @bug memory: where the second half's allocation fails, the call
         * returns its error and keeps the first half, which nothing frees (the
         * failure at 0x8c16841a, and the first half's allocation at
         * 0x8c1683fc) */
#ifdef NONMATCHING
        /* this side repairs the tag above: a failed second half frees the
         * first */
        if (status) {
            kmiFBfree(offset);
        }
#endif
    }
    return status;
}

/* @hack spelling: the stride in words times a word's bytes, where the stride
 * in bytes would do */
#define SLOT_OFFSET(page, index, layout)                                       \
    ((page)->offset +                                                          \
     ((index) * ((layout).codebook_stride >> 2) * sizeof(KMDWORD)))

KMSTATUS kmiVQalloc(PKMSURFACEDESC surface)
{
    struct vq_layout layout;
    struct vq_page *page;
    struct vq_page *available;
    KMDWORD i;
    KMDWORD slot;
    KMINT32 first;
    KMINT32 second;
    KMINT32 bytes;
    KMSTATUS status;
    /* @bug uninitialized: the search reads layout.kind unset for a width
     * vq_shape doesn't take, for which vq_shape returns 0 and fills in no
     * layout (the return at 0x8c16a0f0, and the read at 0x8c16845a) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a width vq_shape doesn't take returns
     * KMSTATUS_INVALID_TEXTURE_TYPE before the search */
    if (!vq_shape(&layout, surface)) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
#else
    vq_shape(&layout, surface);
#endif
    available = 0;
    page = pages;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (page) {
        do {
            if ((page->flags & PAGE_KIND) == layout.kind) {
                for (i = 0; i < layout.slots; i++) {
                    /* @hack spelling: both loops ended by setting i and page
                     * past their tests, where break would do */
                    if (!page->slot[i]) {
                        available = page;
                        slot = i;
                        i = layout.slots;
                        page = 0;
                    }
                }
            }
            if (!available) {
                page = page->next;
            }
        } while (page);
    }
    if (!available) {
        available = take_vq();
        if (!available) {
            return KMSTATUS_NOT_ENOUGH_MEMORY;
        }
        for (i = 0; i < PAGE_SLOTS; i++) {
            available->slot[i] = 0;
        }
        slot = 0;
        prepend_vq(&pages, available);
        available->flags |= layout.kind;
        bytes = layout.page_bytes;
        ALLOC_TEXTURE_BANK(allocate_vq, bytes, available);
        if (status) {
            /* @bug logic: a page whose block can't be allocated goes back to
             * the pool still linked into pages (prepend_vq at 0x8c1684bc,
             * drop_vq at 0x8c168542) */
#ifdef NONMATCHING
            unlink_vq(&pages, available);
#endif
            drop_vq(available);
            return KMSTATUS_NOT_ENOUGH_MEMORY;
        }
    }
    surface->pSurface = (KMDWORD *)SLOT_OFFSET(available, slot, layout);
    available->slot[slot] = surface;
    available->flags |= PAGE_HAS_TEXTURE;
    /* @hack redundant: a call that never runs, the only call of
     * find_idle_vq */
    if (0) {
        find_idle_vq(0, layout.page_bytes, available);
    }
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmiFBfree(KMINT32 offset)
{
    short which;
    short other_bank;
    struct frame_block *block;
    struct frame_block *other;
    KMSTATUS status;
    KMINT32 partner;
    block = find_block(offset, &which);
    if (!block) {
        return KMSTATUS_INVALID_ADDRESS;
    }
    if (block->flags & PAIRED) {
        if (which == 0) {
            partner = first_span + offset;
        } else {
            partner = offset - first_span;
        }
        other = find_block(partner, &other_bank);
        if (!other) {
            release_block(which, block);
            return KMSTATUS_SUCCESS;
        }
        if (other->flags & OCCUPIED) {
            block->flags &= ~OCCUPIED;
            status = KMSTATUS_SUCCESS;
        } else {
            release_block(which, block);
            status = release_block(other_bank, other);
        }
    } else {
        status = release_block(which, block);
    }
    return status;
}

KMSTATUS kmiFBfreeEx(KMDWORD first, KMDWORD second)
{
    KMSTATUS status;
    /* @hack type: the offsets copied into volatile locals, where first and
     * second would do */
    volatile KMDWORD offset;
    volatile KMDWORD other;
    offset = first;
    other = second;
    status = kmiFBfree(offset = to_64bit(offset));
    if (status) {
        status = kmiFBfree(offset = to_64bit(other));
    }
    return status;
}

KMSTATUS kmiVQFBfree(PKMSURFACEDESC surface)
{
    struct vq_layout layout;
    struct vq_page *page;
    struct vq_page *found;
    KMDWORD i;
    KMDWORD slot;
    KMBOOLEAN occupied;
    KMSTATUS status;
    KMDWORD address;
    /* @bug uninitialized: the search reads layout.slots unset for a width
     * vq_shape doesn't take, for which vq_shape returns 0 and fills in no
     * layout (the return at 0x8c16a0f0, and the read at 0x8c168694) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a width vq_shape doesn't take returns
     * KMSTATUS_INVALID_TEXTURE_TYPE before the search */
    if (!vq_shape(&layout, surface)) {
        return KMSTATUS_INVALID_TEXTURE_TYPE;
    }
#else
    vq_shape(&layout, surface);
#endif
    found = 0;
    page = pages;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (page) {
        do {
            if ((page->flags & PAGE_KIND) == layout.kind) {
                for (i = 0; i < layout.slots; i++) {
                    if (page->slot[i]) {
                        address = SLOT_OFFSET(page, i, layout);
                        /* @hack spelling: both loops ended by setting i and
                         * page past their tests, where break would do */
                        if (address == (KMDWORD)surface->pSurface) {
                            found = page;
                            slot = i;
                            i = layout.slots;
                            page = 0;
                        }
                    }
                }
            }
            if (!found) {
                page = page->next;
            }
        } while (page);
    }
    if (found) {
        found->slot[slot] = 0;
        occupied = 0;
        for (i = 0; i < layout.slots; i++) {
            if (found->slot[i]) {
                occupied = 1;
            }
        }
        if (!occupied) {
            status = kmiFBfree(found->offset);
            if (status) {
                return status;
            }
            unlink_vq(&pages, found);
            drop_vq(found);
        }
        return KMSTATUS_SUCCESS;
    }
    return KMSTATUS_INVALID_ADDRESS;
}

/* an idle block counts as free, though it is on its used list. largest gets
 * the largest free or idle block's bytes */
#define COUNT_FREE(i, total, idle, largest)                                    \
    {                                                                          \
        block = bank[i].free_first;                                            \
        while (block) {                                                        \
            if (block->bytes > (largest))                                      \
                (largest) = block->bytes;                                      \
            (total) += block->bytes;                                           \
            block = block->next;                                               \
        }                                                                      \
        block = bank[i].used_first;                                            \
        while (block) {                                                        \
            if (!(block->flags & OCCUPIED)) {                                  \
                if (block->bytes > (largest))                                  \
                    (largest) = block->bytes;                                  \
                (idle) += block->bytes;                                        \
            }                                                                  \
            block = block->next;                                               \
        }                                                                      \
    }
/* the bytes each empty slot of a page of kind counts as, and its slots */
#define PAGE_CAPACITY(kind, bytes, count)                                      \
    case kind:                                                                 \
        slot_bytes = (bytes);                                                  \
        slots = (count);                                                       \
        break

KMSTATUS kmiFBgetfreemem(KMINT32 *first, KMINT32 *second, KMINT32 *vq,
                         KMINT32 *largest)
{
    KMINT32 total_first;
    KMINT32 total_second;
    KMINT32 idle_first;
    KMINT32 idle_second;
    KMINT32 largest_first;
    KMINT32 largest_second;
    KMINT32 vq_bytes;
    KMINT32 slot_bytes;
    KMINT32 slots;
    KMINT32 i;
    struct frame_block *block;
    struct vq_page *page;
    total_first = total_second = idle_first = idle_second = 0;
    largest_first = largest_second = 0;
    vq_bytes = 0;
    COUNT_FREE(0, total_first, idle_first, largest_first);
    COUNT_FREE(1, total_second, idle_second, largest_second);
    page = pages;
    while (page) {
        switch (page->flags & PAGE_KIND) {
            PAGE_CAPACITY(PAGE_8, 0x90, 16);
            PAGE_CAPACITY(PAGE_MIP_8, 0xa0, 16);
            PAGE_CAPACITY(PAGE_16, 0xc0, 16);
            PAGE_CAPACITY(PAGE_MIP_16, 0xe0, 16);
            PAGE_CAPACITY(PAGE_32, 0x200, 8);
            PAGE_CAPACITY(PAGE_MIP_32, 0x360, 4);
            PAGE_CAPACITY(PAGE_64, 0x800, 2);
            PAGE_CAPACITY(PAGE_MIP_64, 0xd60, 1);
        default:
            slot_bytes = 0;
            slots = 0;
            break;
        }
        for (i = 0; i < slots; i++) {
            if (!page->slot[i]) {
                vq_bytes += slot_bytes;
            }
        }
        page = page->next;
    }
    *first = total_first + idle_first;
    *second = total_second + idle_second;
    *vq = vq_bytes;
    *largest = largest_first < largest_second ? largest_second : largest_first;
    return KMSTATUS_SUCCESS;
}

#define LARGEST_FREE(i, largest)                                               \
    {                                                                          \
        block = bank[i].free_first;                                            \
        while (block) {                                                        \
            if (block->bytes > (largest))                                      \
                (largest) = block->bytes;                                      \
            block = block->next;                                               \
        }                                                                      \
        block = bank[i].used_first;                                            \
        while (block) {                                                        \
            if (!(block->flags & OCCUPIED) && block->bytes > (largest))        \
                (largest) = block->bytes;                                      \
            block = block->next;                                               \
        }                                                                      \
    }

KMSTATUS kmiFBgetfreememEx(KMINT32 *first, KMINT32 *second)
{
    KMINT32 first_size;
    KMINT32 second_size;
    struct frame_block *block;
    first_size = second_size = 0;
    LARGEST_FREE(0, first_size);
    LARGEST_FREE(1, second_size);
    *first = first_size;
    *second = second_size;
    return KMSTATUS_SUCCESS;
}

#define FREE_LIST(i) &bank[i].free_first, &bank[i].free_last
#define USED_LIST(i) &bank[i].used_first, &bank[i].used_last

KMSTATUS kmiFBSortQueue(void)
{
    sort_blocks(USED_LIST(0));
    sort_blocks(USED_LIST(1));
    sort_blocks(FREE_LIST(0));
    sort_blocks(FREE_LIST(1));
    return KMSTATUS_SUCCESS;
}

KMSTATUS kmiFBGarbageCollect(void)
{
    KMSTATUS status = compact_blocks(FREE_LIST(0), USED_LIST(0));
    if (!status) {
        status = compact_blocks(FREE_LIST(1), USED_LIST(1));
    }
    return status;
}

/* where a 16-bit mipmap of bytes starts in block, or -1. a mipmap of up to
 * MIP_32X32 bytes crosses no BOUNDARY. a larger one's 32 by 32 level starts at
 * one. find_pair checks that the mipmap ends within block */
static KMINT32 mipmap_offset(struct frame_block *block, KMDWORD bytes,
                             PKMSURFACEDESC surface)
{
    KMDWORD aligned = (block->offset + BOUNDARY) & -BOUNDARY;
    KMDWORD end = block->offset + block->bytes;
    KMINT32 offset;
    if (bytes <= MIP_32X32) {
        if (aligned >= end || aligned - block->offset >= bytes) {
            offset = block->offset;
        } else if (end - aligned >= bytes) {
            offset = aligned;
        } else {
            offset = -1;
        }
    } else {
        if (aligned - block->offset >= MIP_32X32) {
            if (aligned - block->offset + bytes - MIP_32X32 <= end) {
                offset = aligned - MIP_32X32;
            } else {
                offset = -1;
            }
        } else {
            aligned += BOUNDARY;
            if (aligned - block->offset + bytes - MIP_32X32 <= end) {
                offset = aligned - MIP_32X32;
            } else {
                offset = -1;
            }
        }
    }
    return offset;
}

/* the offset of a free run of bytes in bank 0, or -1. the run's partner in bank
 * 1 is free too. a 16-bit mipmap takes the first that mipmap_offset places, and
 * else the first any block offers */
static KMINT32 find_pair(KMINT32 bytes, PKMSURFACEDESC surface)
{
    struct frame_block *block;
    KMINT32 fallback;
    enum { SEARCHED_NOTHING, SEARCHED_PLAIN, SEARCHED_MIPMAP } searched;
    KMINT32 limit;
    KMINT32 offset;
    KMINT32 proposed;
    KMSTATUS status;
    fallback = 0;
    searched = SEARCHED_NOTHING;
    ALIGN_UP(bytes);
    block = bank[0].free_first;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (block) {
        do {
            if (block->bytes >= bytes) {
                limit = block->offset + block->bytes - bytes;
                if ((surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) &&
                    !(surface->fSurfaceFlags & KM_SURFACEFLAGS_VQ) &&
                    !(surface->fSurfaceFlags & KM_SURFACEFLAGS_PALETTIZED)) {
                    proposed = mipmap_offset(block, bytes, surface);
                    if (proposed >= 0) {
                        for (offset = proposed; offset <= limit;
                             offset += BOUNDARY) {
                            status = check_free(1, offset + first_span, bytes);
                            if (!status) {
                                return offset;
                            }
                        }
                    }
                    searched = SEARCHED_MIPMAP;
                    if (!fallback) {
                        for (offset = block->offset; offset <= limit;
                             offset += ALIGNMENT) {
                            status = check_free(1, offset + first_span, bytes);
                            /* @hack spelling: the loop ended by setting offset
                             * to limit, where break would do */
                            if (!status) {
                                fallback = offset;
                                offset = limit;
                            }
                        }
                    }
                } else {
                    searched = SEARCHED_PLAIN;
                    for (offset = block->offset; offset <= limit;
                         offset += ALIGNMENT) {
                        status = check_free(1, offset + first_span, bytes);
                        if (!status) {
                            return offset;
                        }
                    }
                }
            }
            block = block->next;
        } while (block);
    }
    if (searched == SEARCHED_MIPMAP && fallback) {
        return fallback;
    }
    return -1;
}

#define FIND_FREE_BLOCK(matches)                                               \
    {                                                                          \
        found = 0;                                                             \
        block = bank[which].free_first;                                        \
        /* @hack spelling: a do-while under its own test, where while would    \
         * do */                                                               \
        if (block) {                                                           \
            do {                                                               \
                /* @hack spelling: the search ended by setting block to 0,     \
                 * where break would do */                                     \
                if (matches) {                                                 \
                    found = block;                                             \
                    block = 0;                                                 \
                } else {                                                       \
                    block = block->next;                                       \
                }                                                              \
            } while (block);                                                   \
        }                                                                      \
    }
#define CONTAINS_RANGE                                                         \
    (block->offset <= offset && block->offset + block->bytes > offset &&       \
     block->offset <= end && block->offset + block->bytes >= end)

/* whether bank which has one free block that holds the bytes from offset to
 * offset + bytes. the result is KMSTATUS_SUCCESS if it has, and else
 * KMSTATUS_NOT_ENOUGH_MEMORY */
static KMSTATUS check_free(short which, KMINT32 offset, KMINT32 bytes)
{
    KMINT32 end = offset + bytes;
    struct frame_block *block;
    struct frame_block *found;
    /* @hack redundant: an empty test of bytes' remainder */
    if (bytes % ALIGNMENT) {
    }
    FIND_FREE_BLOCK(CONTAINS_RANGE);
    if (!found) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    return KMSTATUS_SUCCESS;
}

#define BIND_SURFACE(block)                                                    \
    {                                                                          \
        surface->pSurface = (KMDWORD *)(block)->offset;                        \
        (block)->owner.surface = surface;                                      \
    }
#define BIND_PAGE(block)                                                       \
    {                                                                          \
        page->offset = (block)->offset;                                        \
        (block)->owner.page = page;                                            \
    }

#define BANK_FLAGS(block, bank_0_flag, bank_1_flag)                            \
    {                                                                          \
        if (which == 0)                                                        \
            (block)->flags |= (bank_0_flag);                                   \
        else                                                                   \
            (block)->flags |= (bank_1_flag);                                   \
    }
#define SET_FIXED_FLAGS(block, bank_0_flag, bank_1_flag)                       \
    {                                                                          \
        BANK_FLAGS(block, bank_0_flag, bank_1_flag);                           \
        if (paired)                                                            \
            (block)->flags |= PAIRED;                                          \
        if (occupied)                                                          \
            (block)->flags |= OCCUPIED;                                        \
        bank[which].free_bytes -= (block)->bytes;                              \
    }
#define TAKE_BLOCK(block)                                                      \
    {                                                                          \
        (block) = take_block();                                                \
        if (!(block))                                                          \
            return KMSTATUS_NOT_ENOUGH_MEMORY;                                 \
    }
#define USE_WHOLE_BLOCK()                                                      \
    {                                                                          \
        unlink_block(FREE_LIST(which), found);                                 \
        prepend_block(USED_LIST(which), found);                                \
    }
/* @hack spelling: found's new size computed into remaining before its store,
 * where found->bytes -= bytes would do */
#define SHRINK_FOUND()                                                         \
    {                                                                          \
        remaining = found->bytes - bytes;                                      \
        found->bytes = remaining;                                              \
    }
#define USE_PREFIX()                                                           \
    {                                                                          \
        TAKE_BLOCK(allocated);                                                 \
        allocated->offset = found->offset;                                     \
        allocated->bytes = bytes;                                              \
        SHRINK_FOUND();                                                        \
        found->offset += bytes;                                                \
        prepend_block(USED_LIST(which), allocated);                            \
    }

/* allocate the bytes at offset from the free block of bank which that holds
 * them. those bytes are all of the free block, its start, its end, or a run
 * within it. a run within it leaves the bytes above it a free block of its
 * own */
#define ALLOCATE_FIXED(bind_found, bind_allocated, bank_0_flag, bank_1_flag)   \
    {                                                                          \
        FIND_FREE_BLOCK(CONTAINS_RANGE);                                       \
        if (!found)                                                            \
            return KMSTATUS_NOT_ENOUGH_MEMORY;                                 \
        if (found->bytes == bytes) {                                           \
            USE_WHOLE_BLOCK();                                                 \
            bind_found;                                                        \
            found->flags = TAKEN;                                              \
            SET_FIXED_FLAGS(found, bank_0_flag, bank_1_flag);                  \
        } else if (found->offset == offset) {                                  \
            USE_PREFIX();                                                      \
            bind_allocated;                                                    \
            SET_FIXED_FLAGS(allocated, bank_0_flag, bank_1_flag);              \
        } else if (found->offset + found->bytes == end) {                      \
            TAKE_BLOCK(allocated);                                             \
            SHRINK_FOUND();                                                    \
            allocated->offset = offset;                                        \
            allocated->bytes = bytes;                                          \
            prepend_block(USED_LIST(which), allocated);                        \
            bind_allocated;                                                    \
            SET_FIXED_FLAGS(allocated, bank_0_flag, bank_1_flag);              \
        } else {                                                               \
            TAKE_BLOCK(allocated);                                             \
            tail = take_block();                                               \
            if (!tail) {                                                       \
                drop_block(allocated);                                         \
                return KMSTATUS_NOT_ENOUGH_MEMORY;                             \
            }                                                                  \
            old_size = found->bytes;                                           \
            prefix = offset - found->offset;                                   \
            found->bytes = prefix;                                             \
            allocated->offset = offset;                                        \
            allocated->bytes = bytes;                                          \
            prepend_block(USED_LIST(which), allocated);                        \
            tail->offset = offset + bytes;                                     \
            tail->bytes = old_size - bytes - prefix;                           \
            insert_after(&bank[which].free_last, tail, bank[which].free_last); \
            bind_allocated;                                                    \
            SET_FIXED_FLAGS(allocated, bank_0_flag, bank_1_flag);              \
        }                                                                      \
    }

static KMSTATUS allocate_at(short which, KMINT32 offset, KMINT32 bytes,
                            PKMSURFACEDESC surface, KMBOOLEAN paired,
                            KMBOOLEAN occupied)
{
    /* @bug logic: the free block found can end short of the allocation, which
     * then takes bytes past it. end is offset plus bytes before bytes rounds
     * up (the sum at 0x8c168d98) */
#ifdef NONMATCHING
    KMINT32 end;
#else
    KMINT32 end = offset + bytes;
#endif
    KMINT32 old_size;
    KMINT32 prefix;
    KMINT32 remaining;
    struct frame_block *block;
    struct frame_block *found;
    struct frame_block *allocated;
    struct frame_block *tail;
#ifdef NONMATCHING
    if (offset < 0 || bytes <= 0 ||
        (KMDWORD)bytes > kmiDeviceExtension.texture_memory_size ||
        (KMDWORD)offset > kmiDeviceExtension.texture_memory_size) {
        return KMSTATUS_INVALID_ADDRESS;
    }
#endif
    ALIGN_UP(bytes);
#ifdef NONMATCHING
    end = offset + bytes;
#endif
    ALLOCATE_FIXED(BIND_SURFACE(found), BIND_SURFACE(allocated), FIXED_BANK_0,
                   FIXED_BANK_1);
    return KMSTATUS_SUCCESS;
}

#define BIND_FRAME(block)                                                      \
    {                                                                          \
        switch (kind) {                                                        \
        case FRAME_BUFFER:                                                     \
            surface = owner;                                                   \
            BIND_SURFACE(block);                                               \
            (block)->flags |= FRAME_BUFFER;                                    \
            break;                                                             \
        case TA_BUFFER:                                                        \
            address = owner;                                                   \
            *address = (block)->offset;                                        \
            (block)->owner.ta_offset = address;                                \
            (block)->flags |= TA_BUFFER;                                       \
            break;                                                             \
        default:                                                               \
            return KMSTATUS_NOT_ENOUGH_MEMORY;                                 \
        }                                                                      \
        (block)->flags |= OCCUPIED;                                            \
        bank[which].free_bytes -= (block)->bytes;                              \
    }

static KMSTATUS allocate_frame(short which, KMINT32 bytes, void *owner,
                               short kind)
{
    KMINT32 remaining;
    struct frame_block *block;
    struct frame_block *found;
    struct frame_block *allocated;
    PKMSURFACEDESC surface;
    KMDWORD *address;
    ALIGN_UP(bytes);
    FIND_FREE_BLOCK(block->bytes >= bytes);
    if (!found) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    if (found->bytes == bytes) {
        USE_WHOLE_BLOCK();
        found->flags = TAKEN;
        BIND_FRAME(found);
    } else {
        TAKE_BLOCK(allocated);
        allocated->offset = found->offset;
        allocated->bytes = bytes;
        SHRINK_FOUND();
        /* @hack spelling: the new offset computed in bytes before its store,
         * where found->offset += bytes would do */
        bytes += found->offset;
        found->offset = bytes;
        prepend_block(USED_LIST(which), allocated);
        BIND_FRAME(allocated);
    }
    return KMSTATUS_SUCCESS;
}

#define TAKE_TEXTURE_SPACE(bind_found, bind_allocated, bank_0_flag,            \
                           bank_1_flag)                                        \
    {                                                                          \
        if (found->bytes == bytes) {                                           \
            USE_WHOLE_BLOCK();                                                 \
            bind_found;                                                        \
            found->flags = TAKEN;                                              \
            BANK_FLAGS(found, bank_0_flag, bank_1_flag);                       \
            found->flags |= OCCUPIED;                                          \
            bank[which].free_bytes -= found->bytes;                            \
        } else {                                                               \
            USE_PREFIX();                                                      \
            bind_allocated;                                                    \
            BANK_FLAGS(allocated, bank_0_flag, bank_1_flag);                   \
            allocated->flags |= OCCUPIED;                                      \
            bank[which].free_bytes -= allocated->bytes;                        \
        }                                                                      \
    }

static KMSTATUS allocate_texture(short which, KMINT32 bytes,
                                 PKMSURFACEDESC surface)
{
    KMINT32 remaining;
    struct frame_block *block;
    struct frame_block *found;
    struct frame_block *allocated;
    ALIGN_UP(bytes);
    FIND_FREE_BLOCK(block->bytes >= bytes);
    if (!found) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    TAKE_TEXTURE_SPACE(BIND_SURFACE(found), BIND_SURFACE(allocated),
                       TEXTURE_BANK_0, TEXTURE_BANK_1);
    return KMSTATUS_SUCCESS;
}

enum page_placement { NO_FIT, AT_START, AT_BOUNDARY };

/* allocate a VQ page of bytes in bank which for page. the page starts at a
 * BOUNDARY or crosses none, in the first free block that allows it. failing
 * that, it starts at the start of the first free block that holds it */
static KMSTATUS allocate_vq(short which, KMINT32 bytes, struct vq_page *page)
{
    KMINT32 aligned;
    KMINT32 end;
    KMINT32 remaining;
    enum page_placement placement = NO_FIT;
    struct frame_block *block;
    struct frame_block *found;
    struct frame_block *fallback;
    struct frame_block *allocated;
    ALIGN_UP(bytes);
    fallback = 0;
    found = 0;
    block = bank[which].free_first;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (block) {
        do {
            if (block->bytes >= bytes) {
                if (!fallback) {
                    fallback = block;
                }
                if (block->offset % BOUNDARY) {
                    aligned = (block->offset + BOUNDARY) & -BOUNDARY;
                    end = block->offset + block->bytes;
                    /* @hack spelling: each placement ends the search by
                     * setting block to 0, where break would do */
                    if (aligned >= end) {
                        placement = AT_START;
                        found = block;
                        block = 0;
                    } else if (aligned - block->offset >= bytes) {
                        placement = AT_START;
                        found = block;
                        block = 0;
                    } else if (end - aligned >= bytes) {
                        placement = AT_BOUNDARY;
                        found = block;
                        block = 0;
                    } else {
                        block = block->next;
                    }
                } else {
                    aligned = block->offset;
                    placement = AT_START;
                    found = block;
                    block = 0;
                }
            } else {
                block = block->next;
            }
        } while (block);
    }
    if (!placement && fallback) {
        placement = AT_START;
        found = fallback;
    }
    if (!found) {
        return KMSTATUS_NOT_ENOUGH_MEMORY;
    }
    if (placement == AT_BOUNDARY) {
        return allocate_vq_at(which, aligned, bytes, page, 0, 1);
    }
    TAKE_TEXTURE_SPACE(BIND_PAGE(found), BIND_PAGE(allocated), PAGE_BANK_0,
                       PAGE_BANK_1);
    return KMSTATUS_SUCCESS;
}

/* the smallest idle FIXED_BANK_ block in bank which that holds bytes, or
 * NULL. its partner's surface has surface's flags. a block of
 * KM_TEXTURE_BANK_SIZE bytes or more is never chosen */
static struct frame_block *find_idle(short which, KMINT32 bytes,
                                     PKMSURFACEDESC surface)
{
    struct frame_block *block;
    struct frame_block *best;
    struct frame_block *other;
    short other_bank;
    KMINT32 best_size = KM_TEXTURE_BANK_SIZE;
    KMINT32 partner;
    block = bank[which].used_first;
    best = 0;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (block) {
        do {
            if (!(block->flags & OCCUPIED) && (block->flags & PAIRED) &&
                (block->flags & (FIXED_BANK_0 | FIXED_BANK_1)) &&
                block->bytes >= bytes && block->bytes < best_size) {
                if (which == 0) {
                    partner = block->offset + first_span;
                } else {
                    partner = block->offset - first_span;
                }
                other = find_block(partner, &other_bank);
                if (other && other->owner.surface->fSurfaceFlags ==
                                 surface->fSurfaceFlags) {
                    best = block;
                    best_size = block->bytes;
                }
            }
            block = block->next;
        } while (block);
    }
    return best;
}

/* the used block at offset, or NULL. *which gets offset's bank either way */
static struct frame_block *find_block(KMINT32 offset, short *which)
{
    struct frame_block *block;
    struct frame_block *found;
    if (offset < second_base) {
        *which = 0;
    } else {
        *which = 1;
    }
    found = 0;
    block = bank[*which].used_first;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (block) {
        do {
            /* @hack spelling: the search ended by setting block to 0, where
             * break would do */
            if (block->offset == offset) {
                found = block;
                block = 0;
            } else {
                block = block->next;
            }
        } while (block);
    }
    return found;
}

#define FIND_ADJACENT()                                                        \
    {                                                                          \
        adjacent = 0;                                                          \
        cursor = bank[which].free_first;                                       \
        /* @hack spelling: a do-while under its own test, where while would    \
         * do */                                                               \
        if (cursor) {                                                          \
            do {                                                               \
                /* @hack spelling: the search ended by setting cursor to 0,    \
                 * where break would do */                                     \
                if (cursor->offset == block->offset + block->bytes ||          \
                    block->offset == cursor->offset + cursor->bytes) {         \
                    adjacent = cursor;                                         \
                    cursor = 0;                                                \
                } else {                                                       \
                    cursor = cursor->next;                                     \
                }                                                              \
            } while (cursor);                                                  \
        }                                                                      \
    }

/* move block from bank which's used list to its free list, merged with each
 * free block it touches */
static KMSTATUS release_block(short which, struct frame_block *block)
{
    struct frame_block *cursor;
    struct frame_block *adjacent;
    block->flags &= ~OCCUPIED;
    unlink_block(USED_LIST(which), block);
    bank[which].free_bytes += block->bytes;
    FIND_ADJACENT();
    /* @hack spelling: a do-while under its own test, with the prepend in each
     * arm, where while and one prepend after it would do */
    if (adjacent) {
        do {
            /* @hack spelling: the size added in each arm, where an add after
             * the test would do */
            if (adjacent->offset == block->offset + block->bytes) {
                adjacent->offset = block->offset;
                adjacent->bytes += block->bytes;
            } else {
                adjacent->bytes += block->bytes;
            }
            drop_block(block);
            unlink_block(FREE_LIST(which), adjacent);
            block = adjacent;
            FIND_ADJACENT();
        } while (adjacent);
        prepend_block(FREE_LIST(which), block);
    } else {
        prepend_block(FREE_LIST(which), block);
    }
    return KMSTATUS_SUCCESS;
}

static struct frame_block *take_block(void)
{
    KMINT32 i;
    for (i = 0; i < BLOCK_COUNT; i++) {
        if (!(block_pool[i].flags & TAKEN)) {
            block_pool[i].flags = TAKEN;
            return &block_pool[i];
        }
    }
    return 0;
}

static void drop_block(struct frame_block *block)
{
    KMINT32 i;
    for (i = 0; i < BLOCK_COUNT; i++) {
        if (&block_pool[i] == block) {
            block_pool[i].flags &= ~TAKEN;
        }
    }
}

static void prepend_block(struct frame_block **first, struct frame_block **last,
                          struct frame_block *block)
{
    struct frame_block *head;
    if (!*first) {
        block->previous = 0;
        block->next = 0;
        *first = block;
        *last = block;
    } else {
        head = *first;
        block->previous = head->previous;
        block->next = *first;
        head->previous = block;
        *first = block;
    }
}

static void insert_after(struct frame_block **tail, struct frame_block *block,
                         struct frame_block *previous)
{
    struct frame_block *next;
    if (!previous->next) {
        *tail = block;
        block->previous = previous;
        block->next = 0;
    } else {
        next = previous->next;
        next->previous = block;
        block->previous = previous;
        block->next = previous->next;
    }
    previous->next = block;
}

static void unlink_block(struct frame_block **first, struct frame_block **last,
                         struct frame_block *block)
{
    /* @hack type: the links copied into volatile locals, where block's own
     * would do */
    struct frame_block *volatile previous = block->previous;
    struct frame_block *volatile next = block->next;
    if (!block->previous) {
        *first = block->next;
    } else {
        previous->next = block->next;
    }
    if (!block->next) {
        *last = block->previous;
    } else {
        next->previous = block->previous;
    }
    /* @hack redundant: a store to previous that never runs */
    if (0) {
        previous = 0;
    }
}

/* purge each cache line of the CHUNK bytes at line */
#define PURGE_CHUNK()                                                          \
    {                                                                          \
        for (i = CHUNK / CACHE_LINE; i; i--) {                                 \
            kmiPurgeCache(line);                                               \
            line += CACHE_LINE_WORDS;                                          \
            /* @hack redundant: a continue that ends the loop's body */        \
            continue; /* NOLINT(readability-redundant-control-flow) */         \
        }                                                                      \
    }

/* copy bytes of video memory from source to destination, CHUNK bytes at a time.
 * destination must not be above source where the two overlap */
static void move_frame(KMDWORD destination, KMDWORD source, KMDWORD bytes)
{
    KMDWORD buffer[CHUNK / sizeof(KMDWORD)];
    KMDWORD *line;
    /* @hack type: data, a volatile pointer to buffer, where buffer would do */
    KMDWORD *volatile data;
    KMINT32 i;
    /* @hack redundant: a purge that never runs */
    if (0) {
        kmiPurgeCache((void *)bytes);
    }
    if (bytes > CHUNK) {
        data = buffer;
        do {
            kmiCopyFromFrameBuffer(data, source, CHUNK);
            line = data;
            /* @hack redundant: a store to bytes that never runs */
            if (0) {
                bytes = (KMDWORD)line + i;
            }
            PURGE_CHUNK();
            kmiCopyToFrameBuffer(destination, data, CHUNK);
            bytes -= CHUNK;
            source = source + CHUNK;
            destination = destination + CHUNK;
        } while (bytes > CHUNK);
    }
    kmiCopyFromFrameBuffer(data = buffer, source, bytes);
    line = data;
    PURGE_CHUNK();
    kmiCopyToFrameBuffer(destination, data, bytes);
}

/* sort a list by offset, lowest first */
static void sort_blocks(struct frame_block **first, struct frame_block **last)
{
    struct frame_block *cursor;
    struct frame_block *highest;
    struct frame_block *end;
    KMINT32 count = 0;
    KMINT32 i;
    KMINT32 j;
    KMINT32 remaining;
    if (!*first) {
        return;
    }
    cursor = *first;
    highest = cursor;
    do {
        if (cursor->offset > highest->offset) {
            highest = cursor;
        }
        cursor = cursor->next;
        count++;
    } while (cursor);
    if (count <= 1) {
        return;
    }
    if (*last != highest) {
        unlink_block(first, last, highest);
        insert_after(last, highest, *last);
    }
    count -= 2;
    remaining = count;
    for (i = 0; i < count; i++) {
        highest = end = *first;
        for (j = 0; j < remaining; j++) {
            end = end->next;
            if (end->offset > highest->offset) {
                highest = end;
            }
        }
        if (end != highest) {
            unlink_block(first, last, highest);
            insert_after(last, highest, end);
        }
        remaining--;
    }
}

/* compact a bank whose lists are sorted by offset. where a free block ends at
 * a texture's or a VQ page's block, that block moves down to the free block's
 * start. its surfaces take the new offset. paired blocks, frame buffers and TA
 * buffers stay. before a move, the free block splits at its first multiple of
 * ALIGNMENT */
static KMSTATUS compact_blocks(struct frame_block **free_first,
                               struct frame_block **free_last,
                               struct frame_block **used_first,
                               struct frame_block **used_last)
{
    struct frame_block *free_block;
    struct frame_block *used;
    struct frame_block *prefix;
    struct frame_block *next;
    struct frame_block *previous;
    enum { STAYS, MOVE_SURFACE, MOVE_PAGE } kind;
    KMINT32 i;
    KMDWORD distance;
    free_block = *free_first;
    used = *used_first;
    if (!free_block) {
        return KMSTATUS_SUCCESS;
    }
    if (!used) {
        return KMSTATUS_SUCCESS;
    }
    do {
        kind = STAYS;
        if ((used->flags & TEXTURE_BANK_0) || (used->flags & TEXTURE_BANK_1) ||
            (!(used->flags & PAIRED) && (used->flags & FIXED_BANK_0)) ||
            (!(used->flags & PAIRED) && (used->flags & FIXED_BANK_1))) {
            kind = MOVE_SURFACE;
        }
        if ((used->flags & PAGE_BANK_0) || (used->flags & PAGE_BANK_1) ||
            (!(used->flags & PAIRED) && (used->flags & FIXED_PAGE_BANK_0)) ||
            (!(used->flags & PAIRED) && (used->flags & FIXED_PAGE_BANK_1))) {
            kind = MOVE_PAGE;
        }
        if (kind) {
            if (free_block->offset + free_block->bytes == used->offset) {
                if (free_block->offset % ALIGNMENT) {
                    TAKE_BLOCK(prefix);
                    prefix->offset = free_block->offset;
                    free_block->offset =
                        (free_block->offset + ALIGNMENT - 1) & -ALIGNMENT;
                    prefix->bytes = free_block->offset - prefix->offset;
                    free_block->bytes -= prefix->bytes;
                    previous = free_block->previous;
                    if (!previous) {
                        prepend_block(free_first, free_last, prefix);
                    } else {
                        insert_after(free_last, prefix, previous);
                    }
                }
                move_frame(free_block->offset, used->offset, used->bytes);
                distance = used->offset - free_block->offset;
                used->offset = free_block->offset;
                free_block->offset += used->bytes;
                if (kind == MOVE_SURFACE) {
                    used->owner.surface->pSurface = (KMDWORD *)used->offset;
                } else {
                    used->owner.page->offset = used->offset;
                    for (i = 0; i < PAGE_SLOTS; i++) {
                        if (used->owner.page->slot[i]) {
                            used->owner.page->slot[i]->pSurface -=
                                distance / sizeof(KMDWORD);
                        }
                    }
                }
                next = free_block->next;
                if (next &&
                    free_block->offset + free_block->bytes == next->offset) {
                    free_block->bytes += next->bytes;
                    unlink_block(free_first, free_last, next);
                    drop_block(next);
                }
            }
            while (used && free_block->offset > used->offset) {
                used = used->next;
            }
            if (!used) {
                return KMSTATUS_SUCCESS;
            }
        } else {
            while (free_block && free_block->offset < used->offset) {
                free_block = free_block->next;
            }
            if (!free_block) {
                return KMSTATUS_SUCCESS;
            }
            while (used && free_block->offset > used->offset) {
                used = used->next;
            }
            if (!used) {
                return KMSTATUS_SUCCESS;
            }
        }
    } while (free_block);
    return KMSTATUS_SUCCESS;
}

static KMSTATUS allocate_vq_at(short which, KMINT32 offset, KMINT32 bytes,
                               struct vq_page *page, KMBOOLEAN paired,
                               KMBOOLEAN occupied)
{
#ifdef NONMATCHING
    KMINT32 end;
#else
    KMINT32 end = offset + bytes;
#endif
    KMINT32 old_size;
    KMINT32 prefix;
    KMINT32 remaining;
    struct frame_block *block;
    struct frame_block *found;
    struct frame_block *allocated;
    struct frame_block *tail;
#ifdef NONMATCHING
    if (offset < 0 || bytes <= 0 ||
        (KMDWORD)bytes > kmiDeviceExtension.texture_memory_size ||
        (KMDWORD)offset > kmiDeviceExtension.texture_memory_size) {
        return KMSTATUS_INVALID_ADDRESS;
    }
#endif
    ALIGN_UP(bytes);
#ifdef NONMATCHING
    end = offset + bytes;
#endif
    ALLOCATE_FIXED(BIND_PAGE(found), BIND_PAGE(allocated), FIXED_PAGE_BANK_0,
                   FIXED_PAGE_BANK_1);
    return KMSTATUS_SUCCESS;
}

#define PAGE_LAYOUT(page_kind, stride, texels, bytes)                          \
    {                                                                          \
        shape->kind = (page_kind);                                             \
        shape->codebook_stride = (stride);                                     \
        shape->texel_bytes = (texels);                                         \
        shape->page_bytes = (bytes);                                           \
    }

/* fill in shape for surface's page and return 1. a width other than 8, 16, 32
 * and 64 returns 0 */
static KMBOOLEAN vq_shape(struct vq_layout *shape, PKMSURFACEDESC surface)
{
    switch (surface->u0.USize) {
    case 8:
        if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
            PAGE_LAYOUT(PAGE_MIP_8, 0x80, 0x20, 0xfa0);
        } else {
            PAGE_LAYOUT(PAGE_8, 0x80, 0x20, 0xf90);
        }
        break;
    case 16:
        if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
            PAGE_LAYOUT(PAGE_MIP_16, 0x80, 0x60, 0xfe0);
        } else {
            PAGE_LAYOUT(PAGE_16, 0x80, 0x40, 0xfc0);
        }
        break;
    case 32:
        if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
            PAGE_LAYOUT(PAGE_MIP_32, 0x200, 0x160, 0xf60);
        } else {
            PAGE_LAYOUT(PAGE_32, 0x100, 0x100, 0x1000);
        }
        break;
    case 64:
        if (surface->fSurfaceFlags & KM_SURFACEFLAGS_MIPMAPED) {
            PAGE_LAYOUT(PAGE_MIP_64, 0x800, 0x560, 0xd60);
        } else {
            PAGE_LAYOUT(PAGE_64, 0x400, 0x400, 0x1000);
        }
        break;
    default:
        return 0;
        /* @hack redundant: a break after the return */
        break;
    }
    shape->slots = (KMDWORD)CODEBOOK_BYTES / shape->codebook_stride;
    return 1;
}

static struct vq_page *take_vq(void)
{
    KMINT32 i;
    for (i = 0; i < PAGE_COUNT; i++) {
        if (!(page_pool[i].flags & TAKEN)) {
            page_pool[i].flags = TAKEN;
            return &page_pool[i];
        }
    }
    return 0;
}

static void drop_vq(struct vq_page *page)
{
    KMINT32 i;
    for (i = 0; i < PAGE_COUNT; i++) {
        if (&page_pool[i] == page) {
            page_pool[i].flags &= ~TAKEN;
        }
    }
}

static void prepend_vq(struct vq_page **first, struct vq_page *page)
{
    /* @hack spelling: the link written in each arm of a test of the list,
     * where page->next = *first alone would do */
    if (!*first) {
        page->next = 0;
        *first = page;
    } else {
        page->next = *first;
        *first = page;
    }
}

static void unlink_vq(struct vq_page **first, struct vq_page *page)
{
    struct vq_page *cursor = *first;
    struct vq_page *previous = 0;
    while (cursor) {
        /* @hack spelling: the search ended by setting cursor to 0, where break
         * would do */
        if (cursor->next == page) {
            previous = cursor;
            cursor = 0;
        } else {
            cursor = cursor->next;
        }
    }
    if (*first == page) {
        *first = page->next;
    } else {
        previous->next = page->next;
    }
}

/* find_idle for an idle FIXED_PAGE_BANK_ block, whose partner's page is of
 * page's kind. only kmiVQalloc's call that never runs names it */
static struct frame_block *find_idle_vq(short which, KMINT32 bytes,
                                        struct vq_page *page)
{
    struct frame_block *block;
    struct frame_block *best;
    struct frame_block *other;
    short other_bank;
    KMINT32 best_size = KM_TEXTURE_BANK_SIZE;
    KMINT32 partner;
    block = bank[which].used_first;
    best = 0;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (block) {
        do {
            if (!(block->flags & OCCUPIED) && (block->flags & PAIRED) &&
                (block->flags & (FIXED_PAGE_BANK_0 | FIXED_PAGE_BANK_1)) &&
                block->bytes >= bytes && block->bytes < best_size) {
                if (which == 0) {
                    partner = block->offset + first_span;
                } else {
                    partner = block->offset - first_span;
                }
                other = find_block(partner, &other_bank);
                if (other) {
                    if ((other->owner.page->flags & PAGE_KIND) ==
                        (page->flags & PAGE_KIND)) {
                        best_size = block->bytes;
                        best = block;
                    }
                }
            }
            block = block->next;
        } while (block);
    }
    return best;
}

/* find_pair for a surface that is no 16-bit mipmap. only kmiMIPalloc's call
 * that never runs names it */
static KMINT32 find_pair_space(KMINT32 bytes)
{
    struct frame_block *block;
    KMINT32 offset;
    KMINT32 limit;
    KMSTATUS status;
    ALIGN_UP(bytes);
    block = bank[0].free_first;
    /* @hack spelling: a do-while under its own test, where while would do */
    if (block) {
        do {
            if (block->bytes >= bytes) {
                limit = block->offset + block->bytes - bytes;
                for (offset = block->offset; offset <= limit;
                     offset += ALIGNMENT) {
                    status = check_free(1, offset + first_span, bytes);
                    if (!status) {
                        return offset;
                    }
                }
            }
            block = block->next;
        } while (block);
    }
    return -1;
}

/* an offset in video memory's 64-bit area as one in its 32-bit area. the 64-bit
 * area alternates the two halves of 8 MB, a lane at a time. the 32-bit area
 * holds the second half after the first. a frame buffer's surface holds a
 * 32-bit offset. in 16 MB, the second 8 MB of each area maps the same way */
static KMDWORD to_32bit(KMDWORD offset)
{
    KMDWORD result = offset & ~((2UL * LANE_BYTES) - 1);
    result >>= 1;
    result += offset & (LANE_BYTES - 1);
    if (offset & LANE_BYTES) {
        result += HALF_MEMORY;
    }
    if (offset >= MEMORY_BYTES) {
        result += HALF_MEMORY;
    }
    return result;
}

/* an offset in the 32-bit area as one in the 64-bit area */
static KMDWORD to_64bit(KMDWORD offset)
{
    KMDWORD lane = 0;
    KMDWORD result;
    if (offset < MEMORY_BYTES) {
        if (offset >= HALF_MEMORY) {
            lane = LANE_BYTES;
            offset -= HALF_MEMORY;
        }
        result = ((offset & ~(LANE_BYTES - 1UL)) << 1) +
                 (offset & (LANE_BYTES - 1)) + lane;
    } else {
        if (offset >= MEMORY_BYTES + HALF_MEMORY) {
            lane = LANE_BYTES;
            offset -= HALF_MEMORY;
        }
        result = ((offset & ~(LANE_BYTES - 1UL)) << 1) +
                 (offset & (LANE_BYTES - 1)) + lane - MEMORY_BYTES;
    }
    return result;
}
