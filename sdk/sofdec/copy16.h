/* COPY_SIXTEEN copies sixteen elements from src to dst, and leaves both past
 * the last. it reads four into a to d before it stores them. no store's
 * offset passes 15 elements, the most an SH-4 displacement holds */
#ifndef SOFDEC_COPY16_H
#define SOFDEC_COPY16_H
#define COPY_FOUR(dst, src, at, a, b, c, d)                                    \
    do {                                                                       \
        (a) = *(src)++;                                                        \
        (b) = *(src)++;                                                        \
        (c) = *(src)++;                                                        \
        (d) = *(src)++;                                                        \
        (dst)[(at)] = (a);                                                     \
        (dst)[(at) + 1] = (b);                                                 \
        (dst)[(at) + 2] = (c);                                                 \
        (dst)[(at) + 3] = (d);                                                 \
    } while (0)
#define COPY_SIXTEEN(dst, src, a, b, c, d)                                     \
    do {                                                                       \
        COPY_FOUR(dst, src, 0, a, b, c, d);                                    \
        COPY_FOUR(dst, src, 4, a, b, c, d);                                    \
        COPY_FOUR(dst, src, 8, a, b, c, d);                                    \
        COPY_FOUR(dst, src, 12, a, b, c, d);                                   \
        (dst) += 16;                                                           \
    } while (0)
#endif
