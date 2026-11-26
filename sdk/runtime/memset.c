#include <string.h>
#ifdef DREAMCAST
#ifdef NONMATCHING
#include <zam.h>
#include <sh4zam/shz_mem.h>
#include <stdint.h>

/* memset sets SET_PAIRS_FROM bytes or more by set_pairs. from that many on,
 * set_pairs runs fewer instructions than memset's loop, wherever s starts */
#define SET_PAIRS_FROM 12
/* the bytes of a pair of longs, which shz_memset8 sets whole, from an
 * address a multiple of PAIR_BYTES */
#define PAIR_BYTES 8

/* shz_memset8's 64-bit value as two equal longs. GCC builds this union with
 * one MOV, and (uint64_t)word << 32 | word with a DMULU */
union pair {
    uint64_t value;
    uint32_t longs[2];
};

/* set n bytes from s, for an n of PAIR_BYTES - 1 or more. shz_memset8 sets
 * the whole pairs. byte, half and long stores set the head before the first
 * pair and the tail after the last. each store lies on a boundary of its
 * size. with such an n, the head and the tail never overlap. they are set
 * before shz_memset8 runs, so s is the only value that must survive its
 * call. noinline keeps memset from saving registers for a short set */
static __attribute__((noinline)) void *set_pairs(void *s, int c, size_t n)
{
    _Static_assert(SET_PAIRS_FROM >= PAIR_BYTES - 1,
                   "set_pairs needs n of PAIR_BYTES - 1 or more");
    unsigned char *to = s;
    unsigned char *end = to + n;
    unsigned char byte = (unsigned char)c;
    uint32_t word = byte * 0x01010101U;
    union pair pair = {.longs = {word, word}};
    size_t head = -(uintptr_t)to % PAIR_BYTES;
    size_t tail = (uintptr_t)end % PAIR_BYTES;

    if (head & 1) {
        *to++ = byte;
    }
    if (head & 2) {
        *(uint16_t *)to = (uint16_t)word;
        to += 2;
    }
    if (head & 4) {
        *(uint32_t *)to = word;
        to += 4;
    }
    if (tail & 1) {
        *--end = byte;
    }
    if (tail & 2) {
        end -= 2;
        *(uint16_t *)end = (uint16_t)word;
    }
    if (tail & 4) {
        end -= 4;
        *(uint32_t *)end = word;
    }
    shz_memset8(to, pair.value, end - to);
    return s;
}
#endif
#endif

void *memset(void *s, int c, size_t n)
{
    size_t i;
    unsigned char *to = s;

#ifdef DREAMCAST
#ifdef NONMATCHING
    /* this side sets the same bytes. only a block in system RAM is set by
     * pairs. shz_memset8's stores are 64-bit, which a 32-bit register isn't
     * known to take */
    if (n >= SET_PAIRS_FROM && zam_in_system_ram((uintptr_t)s)) {
        return set_pairs(s, c, n);
    }
#endif
#endif
    for (i = 0; i < n; ++i) {
        *to++ = (unsigned char)c;
    }
    return s;
}
