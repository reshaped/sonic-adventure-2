/* the Shinobi heap. its calls and the C library's malloc, free, realloc and
 * calloc all go through sygMallocIFPtr */
#include <machine.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#ifdef NONMATCHING
/* this side's realloc moves a block with memmove */
#include <string.h>
#endif

/* the heap's unit of UNIT_BYTES bytes. a block is a run of units, the first
 * its header */
#define UNIT_BYTES 32
union heap_unit {
    struct {
        union heap_unit *next; /* the next free block, while this is free */
        Uint32 units; /* the block's size in units, the header included */
    } header;
    char bytes[UNIT_BYTES];
};

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void *syMalloc_default_malloc(Uint32 size);
void syMalloc_default_free(void *ptr);
void *syMalloc_default_calloc(Uint32 nmemb, Uint32 size);
void *syMalloc_default_realloc(void *ptr, Uint32 size);

const Uint8 sygMallocVer[4] = {2, 0, 1, 0};
const char *sygMallocBuild = "\nsyMalloc Ver 2.01 Build:Aug 09 2000 17:33:28\n";
/* the heap's own functions, which syMallocChangeIF(NULL) selects */
const SYS_MALLOC_IF sygMallocIF = {
    syMalloc_default_malloc, syMalloc_default_free, syMalloc_default_calloc,
    syMalloc_default_realloc};

SYS_MALLOC_IF *sygMallocIFPtr;
/* the free blocks form a circular list in address order. the list always
 * holds sygMallocBase, a block of 0 units outside the heap */
union heap_unit sygMallocBase;
/* the free block where the last search or free stopped. the next search
 * starts after it */
union heap_unit *sygMallocFreep;
/* NOLINTEND(misc-use-internal-linkage) */
/* heap_base and heap_first both hold the heap's first byte, and heap_size
 * its length in bytes. syMallocInit reads heap_base, and
 * syMalloc_default_realloc bounds the free blocks it meets by heap_first and
 * heap_size */
static char *heap_base;
static Uint32 heap_size;
static char *heap_first;

void syMallocInit(void *heap, Uint32 size)
{
    union heap_unit *block;

#ifdef NONMATCHING
    /* this side repairs the bug tagged below: the heap starts at heap rounded
     * up to a unit, and size loses the bytes the rounding passed. a null heap,
     * or one too small for the rounding, holds no bytes */
    size_t prefix = -(size_t)heap & (UNIT_BYTES - 1);

    if (heap == NULL || size < prefix) {
        size = 0;
    } else {
        heap = (char *)heap + prefix;
        size -= prefix;
    }
    heap_first = heap_base = heap;
#else
    /* @hack spelling: both statics set in one statement, where two
     * statements would do */
    heap_first = heap_base =
        (char *)(((Uint32)heap + sizeof(union heap_unit) - 1) &
                 ~(sizeof(union heap_unit) - 1));
    /* @bug memory: with heap off a 32-byte boundary, the heap can end up to
     * 31 bytes past heap + size. it starts at heap rounded up to 32 bytes, but
     * takes size rounded down as its length (ADD #31,R4 at 0x8c110118, AND
     * R5,R4 at 0x8c110120 and AND R5,R14 at 0x8c110126) */
#endif
    size &= ~(sizeof(union heap_unit) - 1);
    heap_size = size;
    /* @hack spelling: the head's next and sygMallocFreep set in one
     * statement, where two statements would do */
    sygMallocBase.header.next = sygMallocFreep = &sygMallocBase;
    sygMallocBase.header.units = 0;
    syMallocChangeIF(NULL);
#ifdef NONMATCHING
    /* this side puts no block on the free list for a heap of no units */
    if (size == 0) {
        return;
    }
#endif
    block = (union heap_unit *)heap_base;
    block->header.units = size / sizeof(union heap_unit);
    free(block + 1);
}

SYS_MALLOC_IF *syMallocChangeIF(SYS_MALLOC_IF *malloc_if)
{
    SYS_MALLOC_IF *before = sygMallocIFPtr;

    if (malloc_if == NULL) {
        sygMallocIFPtr = (SYS_MALLOC_IF *)&sygMallocIF;
    } else {
        sygMallocIFPtr = malloc_if;
    }
    return before;
}

void syMallocFinish(void)
{
}

void syMallocStat(Uint32 *wholeFreeSize, Uint32 *biggestFreeBlockSize)
{
    Uint32 largest;
    Uint32 whole;
    union heap_unit *block;
    union heap_unit *before;
    Uint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    before = sygMallocFreep;
    whole = 0;
    largest = 0;
    for (block = before->header.next;;
         before = block, block = block->header.next) {
        if (block->header.units > 0) {
            whole += block->header.units - 1;
            if (largest < block->header.units - 1) {
                largest = block->header.units - 1;
            }
        }
        if (block == sygMallocFreep) {
            break;
        }
    }
    *wholeFreeSize = whole * sizeof(union heap_unit);
    *biggestFreeBlockSize = largest * sizeof(union heap_unit);
    set_imask(mask);
}

void *syMalloc(Uint32 size)
{
    return sygMallocIFPtr->malloc(size);
}

void *malloc(size_t size)
{
    return sygMallocIFPtr->malloc(size);
}

/* take the first free block after sygMallocFreep with room for size bytes and a
 * header. a larger block gives up its end */
void *syMalloc_default_malloc(Uint32 size)
{
    union heap_unit *block;
    union heap_unit *before;
    Uint32 units;
    Uint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* this side rounds size up to units without the sum that wraps for a size
     * within 31 of 2^32 */
    units = size / UNIT_BYTES + (size % UNIT_BYTES != 0) + 1;
#else
    units =
        ((size + sizeof(union heap_unit) - 1) / sizeof(union heap_unit)) + 1;
#endif
    before = sygMallocFreep;
    for (block = before->header.next;;
         before = block, block = block->header.next) {
        if (block->header.units >= units) {
            if (block->header.units == units) {
                before->header.next = block->header.next;
            } else {
                block->header.units -= units;
                block += block->header.units;
                block->header.units = units;
            }
            sygMallocFreep = before;
            set_imask(mask);
            return block + 1;
        }
        if (block == sygMallocFreep) {
            set_imask(mask);
            return NULL;
        }
    }
    /* @hack redundant: a restore of the mask that nothing reaches */
    set_imask(mask);
}

void syFree(void *ptr)
{
    sygMallocIFPtr->free(ptr);
}

void free(void *ptr)
{
    sygMallocIFPtr->free(ptr);
}

/* return the block to the free list, merged with an adjacent free block on
 * either side. a null ptr does nothing */
void syMalloc_default_free(void *ptr)
{
    union heap_unit *block;
    union heap_unit *before;
    Uint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
    if (ptr != NULL) {
        block = (union heap_unit *)ptr - 1;
        /* find the free block just below block in address order. where block
         * lies above every free block or below them all, take the highest
         * free block */
        for (before = sygMallocFreep;
             !(block > before && block < before->header.next);
             before = before->header.next) {
            if (before >= before->header.next &&
                (block > before || block < before->header.next)) {
                break;
            }
        }
        if (block + block->header.units == before->header.next) {
            block->header.units += before->header.next->header.units;
            block->header.next = before->header.next->header.next;
        } else {
            block->header.next = before->header.next;
        }
        if (before + before->header.units == block) {
            before->header.units += block->header.units;
            before->header.next = block->header.next;
        } else {
            before->header.next = block;
        }
        sygMallocFreep = before;
    }
    set_imask(mask);
}

void *syRealloc(void *ptr, Uint32 size)
{
    return sygMallocIFPtr->realloc(ptr, size);
}

void *realloc(void *ptr, size_t size)
{
    return sygMallocIFPtr->realloc(ptr, size);
}

/* free ptr's block, then move its data, as much as fits, to a block of size
 * bytes taken as syMalloc_default_malloc takes one. ptr must not be NULL. a
 * block that grows first needs room. room is one free block of the new size,
 * or free neighbors that bring ptr's block to it. the result is the new block.
 * without room, it is NULL and ptr's block stays as it was. it is NULL too
 * where the search for room meets a free block outside the heap other than
 * sygMallocBase */
void *syMalloc_default_realloc(void *ptr, Uint32 size)
{
    union heap_unit *to;
    union heap_unit *from;
    union heap_unit *before;
    union heap_unit *block;
    union heap_unit *old;
    Uint32 units;
    Uint32 kept;
    Uint32 room;
    Uint16 mask;

    /* @hack spelling: old, a typed copy of ptr made before the mask is read,
     * not at its first use */
    old = (union heap_unit *)ptr;
    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* this side rounds size up to units without the sum that wraps for a size
     * within 31 of 2^32 */
    units = size / UNIT_BYTES + (size % UNIT_BYTES != 0) + 1;
#else
    units =
        ((size + sizeof(union heap_unit) - 1) / sizeof(union heap_unit)) + 1;
#endif
    to = old - 1;
    before = sygMallocFreep;
    if (units <= to->header.units) {
        kept = units;
        /* @hack redundant: an in-place shrink that never runs */
        if (0) {
            union heap_unit *rest = to + units;

            rest->header.units = kept - units;
            to->header.units = units;
            free(rest + 1);
            return old;
        }
    } else {
        kept = to->header.units;
        room = to->header.units;
        for (block = before->header.next;; block = block->header.next) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: the search weighs
             * before's block too, the last in the circular list, and ends there
             * with the test further on */
#else
            /* @bug logic: the search for room ends at sygMallocFreep's block
             * without weighing it (CMP/EQ R12,R4 at 0x8c1103c8), where
             * syMalloc_default_malloc's search weighs it last. so growing
             * fails where only that block, alone or beside ptr's, has the
             * room */
            if (block == before) {
                set_imask(mask);
                return NULL;
            }
#endif
            if (!(((char *)block >= heap_first &&
                   heap_first + heap_size >= (char *)block) ||
                  block == &sygMallocBase)) {
                set_imask(mask);
                return NULL;
            }
            if (block->header.units >= units) {
                break;
            }
            if (block + block->header.units == to) {
                room += block->header.units;
            }
            if (to + to->header.units == block) {
                room += block->header.units;
            }
            if (room >= units) {
                break;
            }
#ifdef NONMATCHING
            /* this side's half of the repair of the bug tagged above: the
             * search ends at before's block once it has weighed it */
            if (block == before) {
                set_imask(mask);
                return NULL;
            }
#endif
        }
    }
    from = old;
    free(old);
    before = sygMallocFreep;
    for (block = before->header.next;;
         before = block, block = block->header.next) {
        if (block->header.units >= units) {
            if (block->header.units == units) {
                before->header.next = block->header.next;
            } else {
                block->header.units -= units;
                block += block->header.units;
            }
#ifdef NONMATCHING
            /* The old and new payloads may overlap. A header-only block
             * contributes no units to copy. */
            memmove(block + 1, from, (kept - 1) * sizeof(*from));
#else
            /* @hack redundant: the test from != block + 1, which the two
             * tests inside it cover */
            if (from != block + 1) {
                if (from < block + 1) {
                    from += kept - 2;
                    for (to = block + kept - 1; to >= block + 1;) {
                        *to-- = *from--;
                    }
                } else if (from > block + 1) {
                    for (to = block + 1; to < block + kept;) {
                        *to++ = *from++;
                    }
                }
            }
#endif
            block->header.units = units;
            sygMallocFreep = before;
            set_imask(mask);
            return block + 1;
        }
        if (block == sygMallocFreep) {
            set_imask(mask);
            return NULL;
        }
    }
}

void *syCalloc(Uint32 count, Uint32 size)
{
    return sygMallocIFPtr->calloc(count, size);
}

void *calloc(size_t nmemb, size_t size)
{
    return sygMallocIFPtr->calloc(nmemb, size);
}

/* Allocate an array whose byte count fits the heap size type. */
void *syMalloc_default_calloc(Uint32 nmemb, Uint32 size)
{
    Uint8 *block;
    Uint32 at;
    Uint16 mask;

    mask = get_imask();
    set_imask(IMASK_ALL);
#ifdef NONMATCHING
    /* this side fails where nmemb times size passes 2^32 - 1, which the product
     * below would wrap */
    if (nmemb && size > (Uint32)-1 / nmemb) {
        set_imask(mask);
        return NULL;
    }
#endif
    size *= nmemb;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    /* @hack redundant: the else around the null return */
    if ((block = malloc(size)) != NULL) {
        /* a block is whole units, so the words cleared past size stay inside
         * it */
        for (at = 0; at < size; at += sizeof(Uint32)) {
            *(Uint32 *)(block + at) = 0;
        }
        /* the word loop has already cleared every byte this loop clears */
        for (at -= sizeof(Uint32); at < (size & (sizeof(Uint32) - 1)); at++) {
            block[at] = 0;
        }
        set_imask(mask);
        return block;
    } else {
        set_imask(mask);
        return NULL;
    }
}
