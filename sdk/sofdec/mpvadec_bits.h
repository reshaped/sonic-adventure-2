/* the MPEG-1 video decoder's bit reader, which reads fields high bit first.
 * it works in the locals bits, lookahead, nused and cursor of its caller.
 * bits holds the current word, its unread bits at the top. nused counts the
 * bits read from it. lookahead holds the next word, and cursor points past
 * it. the reader reads up to two words past the word that holds a stream's
 * last bit */
#ifndef MPVADEC_BITS_H
#define MPVADEC_BITS_H

/* a word's bits and bytes. cursor runs AHEAD_BYTES past the word bits came
 * from */
#define WORD_BITS 32
#define WORD_BYTES 4
#define AHEAD_BYTES 8

/* @hack type: the first byte read as an Sint8, where the Uint8 it is would
 * do: the shifts push its sign bits out */
/* @hack spelling: each step of cursor as cursor = cursor + 1, where cursor++
 * would do */
#define LOAD_WORD(word)                                                        \
    {                                                                          \
        (word) = (Sint8) * cursor;                                             \
        cursor = cursor + 1;                                                   \
        (word) <<= 8;                                                          \
        (word) |= *cursor;                                                     \
        cursor = cursor + 1;                                                   \
        (word) <<= 8;                                                          \
        (word) |= *cursor;                                                     \
        cursor = cursor + 1;                                                   \
        (word) <<= 8;                                                          \
        (word) |= *cursor;                                                     \
        cursor = cursor + 1;                                                   \
    }

/* start the reader at the header past the four-byte start code at the
 * address *data holds. the reader loads the aligned word around that
 * address, and shifts out the bits ahead of the header */
/* @hack type: nused taken from *data read again as an Sint32, where the
 * address cursor came from would do */
#define OPEN_HEADER(data)                                                      \
    {                                                                          \
        cursor = (Uint8 *)(((Uint32) * (data) + 4) & ~3UL);                    \
        nused = *(const Sint32 *)(data) + 4;                                   \
        nused -= (Sint32)cursor;                                               \
        nused *= 8;                                                            \
        LOAD_WORD(bits);                                                       \
        bits <<= nused;                                                        \
        LOAD_WORD(lookahead);                                                  \
    }

#define TAKE_FIELD(width, value)                                               \
    {                                                                          \
        if (nused >= 32 - (width)) {                                           \
            nused -= 32 - (width);                                             \
            if (nused != 0) {                                                  \
                bits |= lookahead >> ((width) - nused);                        \
                (value) = bits >> (32 - (width));                              \
                bits = lookahead << nused;                                     \
            } else {                                                           \
                (value) = bits >> (32 - (width));                              \
                bits = lookahead;                                              \
            }                                                                  \
            LOAD_WORD(lookahead);                                              \
        } else {                                                               \
            nused += (width);                                                  \
            (value) = bits >> (32 - (width));                                  \
            bits <<= (width);                                                  \
        }                                                                      \
    }

#define SKIP_BITS(width)                                                       \
    {                                                                          \
        nused += (width);                                                      \
        if (nused >= 32) {                                                     \
            nused -= 32;                                                       \
            bits = lookahead << nused;                                         \
            LOAD_WORD(lookahead);                                              \
        } else {                                                               \
            bits <<= (width);                                                  \
        }                                                                      \
    }

/* @hack spelling: bits = bits << 1, where bits <<= 1 would do */
#define TAKE_FLAG(value)                                                       \
    {                                                                          \
        (value) = (Sint32)bits < 0;                                            \
        if (nused == 31) {                                                     \
            nused = 0;                                                         \
            bits = lookahead;                                                  \
            LOAD_WORD(lookahead);                                              \
        } else {                                                               \
            nused++;                                                           \
            bits = bits << 1;                                                  \
        }                                                                      \
    }

#define PEEK_FIELD(width, value)                                               \
    {                                                                          \
        (value) = bits >> (32 - (width));                                      \
        if (nused > 32 - (width)) {                                            \
            (value) |= lookahead >> (64 - (width) - nused);                    \
        }                                                                      \
    }

#define LOAD_READER(mpv)                                                       \
    {                                                                          \
        bits = (mpv)->bits;                                                    \
        lookahead = (mpv)->next;                                               \
        nused = (mpv)->nused;                                                  \
        cursor = (Uint8 *)(mpv)->rd;                                           \
    }
#define SAVE_READER(mpv)                                                       \
    {                                                                          \
        (mpv)->bits = bits;                                                    \
        (mpv)->next = lookahead;                                               \
        (mpv)->nused = nused;                                                  \
        (mpv)->rd = (const Uint32 *)cursor;                                    \
    }

#endif
