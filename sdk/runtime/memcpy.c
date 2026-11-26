#include <string.h>
#ifdef DREAMCAST
#ifdef NONMATCHING
#include <zam.h>
#include <sh4zam/shz_mem.h>
#include <stdint.h>

/* this side names the shipped memcpy below memmove, since it copies
 * overlapping blocks as memmove must. this side's own memcpy follows it */
#define memcpy memmove
#endif
#endif

/* it copies by long words where s1, s2 and n are all multiples of 4. where
 * all three are even, it copies by words, and otherwise by bytes. it copies
 * backward when s1 is above s2, so blocks that overlap copy as memmove copies
 * them */
void *memcpy(void *s1, const void *s2, size_t n)
{
    unsigned long low;
    size_t words;
    size_t longs;

    if (s1 != s2 && n > 0) {
        low = (unsigned long)s1 | (unsigned long)s2 | n;
        longs = n >> 2;
        words = n >> 1;
        if (s1 < s2) {
            if ((low & 1) == 0) {
                if ((low & 3) == 0) {
                    long *to = s1;
                    const long *from = s2;

                    n = longs;
                    do {
                        *to++ = *from++;
                    } while (--n != 0);
                } else {
                    short *to = s1;
                    const short *from = s2;

                    n = words;
                    do {
                        *to++ = *from++;
                    } while (--n != 0);
                }
            } else {
                char *to = s1;
                const char *from = s2;

                do {
                    *to++ = *from++;
                } while (--n != 0);
            }
        } else {
            if ((low & 1) == 0) {
                if ((low & 3) == 0) {
                    long *to = (long *)((char *)s1 + n);
                    const long *from = (const long *)((const char *)s2 + n);

                    n = longs;
                    do {
                        *--to = *--from;
                    } while (--n != 0);
                } else {
                    short *to = (short *)((char *)s1 + n);
                    const short *from = (const short *)((const char *)s2 + n);

                    n = words;
                    do {
                        *--to = *--from;
                    } while (--n != 0);
                }
            } else {
                char *to = (char *)s1 + n;
                const char *from = (const char *)s2 + n;

                do {
                    *--to = *--from;
                } while (--n != 0);
            }
        }
    }
    return s1;
}

#ifdef DREAMCAST
#ifdef NONMATCHING
#undef memcpy

/* this side keeps the shipped result. memmove, the shipped memcpy, copies
 * blocks that overlap. it also copies blocks outside system RAM, where the
 * order of the stores can show. SH4ZAM copies the rest, in another order but
 * by the width the shipped memcpy picks. shz_memcpy isn't used. it picks
 * widths of its own, pairs of floats among them, and claims lines by
 * MOVCA.L */
void *memcpy(void *s1, const void *s2, size_t n)
{
    uintptr_t to = (uintptr_t)s1;
    uintptr_t from = (uintptr_t)s2;
    uintptr_t low = to | from | n;

    if (!zam_in_system_ram(to) || !zam_in_system_ram(from) ||
        (to < from + n && from < to + n)) {
        return memmove(s1, s2, n);
    }
    if (low % 4 == 0) {
        return shz_memcpy4(s1, s2, n);
    }
    if (low % 2 == 0) {
        return shz_memcpy2(s1, s2, n);
    }
    return shz_memcpy1(s1, s2, n);
}
#endif
#endif
