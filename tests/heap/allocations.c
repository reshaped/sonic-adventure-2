/* Exercise the default Shinobi heap without interposing on the host libc. */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define malloc heap_malloc
#define free heap_free
#define calloc heap_calloc
#define realloc heap_realloc

void *heap_malloc(size_t size);
void heap_free(void *ptr);
void *heap_calloc(size_t count, size_t size);
void *heap_realloc(void *ptr, size_t size);

static int interrupt_mask = 3;
static int _builtin_get_imask(void)
{
    return interrupt_mask;
}

static void _builtin_set_imask(int mask)
{
    interrupt_mask = mask;
}

#include "syMalloc.c"

static void stats(Uint32 expected)
{
    Uint32 total;
    Uint32 largest;

    syMallocStat(&total, &largest);
    assert(total == expected);
    assert(largest == expected);
    assert(interrupt_mask == 3);
}

static void spans(void)
{
    unsigned char arena[384];
    unsigned char *begin;
    unsigned char *end;
    unsigned char *result;
    Uint32 units;
    Uint32 prefix;
    Uint32 offset;
    Uint32 i;
    size_t j;
    static const Uint32 sizes[] = {0, 1, 31, 32, 33, 63, 64, 65, 192, 223};

    for (offset = 0; offset < 32; ++offset) {
        begin = arena + 32 + (-(uintptr_t)arena & 31) + offset;
        prefix = (32 - offset) % 32;
        for (j = 0; j < sizeof(sizes) / sizeof(sizes[0]); ++j) {
            memset(arena, 0x71, sizeof(arena));
            end = begin + sizes[j];
            units = sizes[j] > prefix ? (sizes[j] - prefix) / 32 : 0;
            syMallocInit(begin, sizes[j]);
            stats(units ? (units - 1) * 32 : 0);
            result = syMalloc(units ? (units - 1) * 32 : 0);
            if (units) {
                assert(result == begin + prefix + 32);
                assert(((uintptr_t)result & 31) == 0);
                memset(result, 0x42, (units - 1) * 32);
                assert(result + (units - 1) * 32 <= end);
            } else {
                assert(result == NULL);
            }
            stats(0);
            syFree(result);
            stats(units ? (units - 1) * 32 : 0);
            for (i = 0; i < sizeof(arena); ++i) {
                if (arena + i < begin || arena + i >= end) {
                    assert(arena[i] == 0x71);
                }
            }
        }
    }
}

static void resize(void)
{
    unsigned char arena[320];
    unsigned char *aligned = arena + (-(uintptr_t)arena & 31);
    void *upper;
    unsigned char *lower;
    unsigned char *grown;
    Uint32 i;

    syMallocInit(aligned, 256);
    stats(224);
    assert(syMalloc((Uint32)-1) == NULL);
    assert(syCalloc((Uint32)-1 / 4 + 1, 4) == NULL);
    stats(224);
    upper = syMalloc(32);
    lower = syCalloc(4, 8);
    assert(upper && lower);
    for (i = 0; i < 32; ++i) {
        assert(lower[i] == 0);
        lower[i] = (unsigned char)(i + 1);
    }
    syFree(upper);
    assert(syRealloc(lower, (Uint32)-1) == NULL);
    for (i = 0; i < 32; ++i) {
        assert(lower[i] == i + 1);
    }
    /* The final free-list entry supplies the rest of the enlargement. */
    grown = syRealloc(lower, 160);
    assert(grown != NULL);
    for (i = 0; i < 32; ++i) {
        assert(grown[i] == i + 1);
    }
    grown = syRealloc(grown, 0);
    assert(grown != NULL);
    syFree(grown);
    stats(224);
}

int main(void)
{
    spans();
    resize();
    return 0;
}
