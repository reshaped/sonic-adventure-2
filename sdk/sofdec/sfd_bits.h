/* the Sofdec bit reader, which reads MPEG fields high bit first. it works in
 * the locals current, lookahead, bit and cursor of its caller. current holds
 * the current word, its unread bits at the top. bit counts the bits read from
 * it. lookahead holds the next word, and cursor points past it */
#ifndef SOFDEC_BITS_H
#define SOFDEC_BITS_H
#include <sg_xpt.h>

/* a word's bits and bytes, and a byte's bits. cursor runs AHEAD_BYTES past
 * the word current came from */
#define WORD_BITS 32
#define WORD_BYTES 4
#define BYTE_BITS 8
#define BYTE_SHIFT 3
#define AHEAD_BYTES 8

/* @hack type: the first byte read as an Sint8, where the Uint8 it is would
 * do: the shifts push its sign bits out */
#define READ_WORD(word)                                                        \
    {                                                                          \
        (word) = (Uint32)(Sint8) * cursor;                                     \
        cursor++;                                                              \
        (word) <<= 8;                                                          \
        (word) |= *cursor;                                                     \
        cursor++;                                                              \
        (word) <<= 8;                                                          \
        (word) |= *cursor;                                                     \
        cursor++;                                                              \
        (word) <<= 8;                                                          \
        (word) |= *cursor;                                                     \
        cursor++;                                                              \
    }

/* width must be from 1 to 31. where the field reaches the end of current,
 * lookahead moves into current. the next word then loads into lookahead */
#define GET_BITS(width, out)                                                   \
    {                                                                          \
        if (bit >= 32 - (width)) {                                             \
            bit -= 32 - (width);                                               \
            if (bit != 0) {                                                    \
                current |= lookahead >> ((width) - bit);                       \
                (out) = current >> (32 - (width));                             \
                current = lookahead << bit;                                    \
            } else {                                                           \
                (out) = current >> (32 - (width));                             \
                current = lookahead;                                           \
            }                                                                  \
            READ_WORD(lookahead);                                              \
        } else {                                                               \
            bit += (width);                                                    \
            (out) = current >> (32 - (width));                                 \
            current <<= (width);                                               \
        }                                                                      \
    }
#define READ_BITS(width, out)                                                  \
    do                                                                         \
    GET_BITS(width, out) while (0)

#define SKIP_BITS(width, word_bits)                                            \
    do {                                                                       \
        bit += (width);                                                        \
        if (bit >= (word_bits)) {                                              \
            bit -= (word_bits);                                                \
            current = lookahead << bit;                                        \
            READ_WORD(lookahead);                                              \
        } else {                                                               \
            current <<= (width);                                               \
        }                                                                      \
    } while (0)
#define READ_FLAG(out)                                                         \
    do {                                                                       \
        (out) = (Sint32)current < 0;                                           \
        if (bit == 31) {                                                       \
            bit = 0;                                                           \
            current = lookahead;                                               \
            READ_WORD(lookahead);                                              \
        } else {                                                               \
            bit++;                                                             \
            current <<= 1;                                                     \
        }                                                                      \
    } while (0)
#define PEEK_BITS(width, out)                                                  \
    do {                                                                       \
        (out) = current >> (32 - (width));                                     \
        if (bit > 32 - (width)) {                                              \
            (out) |= lookahead >> (64 - (width) - bit);                        \
        }                                                                      \
    } while (0)

#endif
