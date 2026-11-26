/* COPY_ELEMENTS copies count elements of type from src to dst, leaving both
 * past the last. count must be above 0: the loop copies before it tests */
#ifndef NINJA_COPY_ELEMENTS_H
#define NINJA_COPY_ELEMENTS_H

#ifdef NONMATCHING
/* this side copies by a statement, the plain form the hack tag below gives */
#define COPY_ELEMENT(dst, src) (*(dst) = *(src))
#else
/* @hack spelling: the store as an empty switch's operand, where a statement
 * of its own would do */
#define COPY_ELEMENT(dst, src)                                                 \
    switch (*(dst) = *(src)) {                                                 \
    }
#endif

#define COPY_ELEMENTS(type, dst, src, count)                                   \
    do {                                                                       \
        COPY_ELEMENT((type *)(dst), (const type *)(src));                      \
        (src) = (const type *)(src) + 1;                                       \
        (dst) = (type *)(dst) + 1;                                             \
    } while (--(count) != 0)

#endif
