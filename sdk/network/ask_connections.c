#include <sg_xpt.h>
#include "ask_dictionary.h"
#include "ask_grammar.h"

/* kinds from CONJUGATING_ENDINGS on are endings, and those from
 * PLAIN_ENDINGS on don't conjugate */
#define CONJUGATING_ENDINGS 0x80
#define PLAIN_ENDINGS 0xc0
#define CONJUGATING_ROWS 51
#define PLAIN_ROWS 63

/* an ending's bits in connections, one set for each kind of word it can
 * follow. a kind below CONJUGATING_ENDINGS or from PLAIN_ENDINGS has its bit
 * at base + part_offsets[kind]. a conjugating kind runs from
 * CONJUGATING_ENDINGS up to PLAIN_ENDINGS. it has a bit for each form from 1,
 * from conjugation + part_offsets[kind] */
struct connection_row {
    Sint16 base, conjugation;
};
#pragma section ATA
/* bit n is bit n % 8 of byte n / 8, from the low end */
static Uint32 connections[248] = {
#include "network/ask_connections/connections.inc"
};
/* the plain endings' rows, then the conjugating endings' */
static struct connection_row endings[PLAIN_ROWS + CONJUGATING_ROWS] = {
#include "network/ask_connections/endings.inc"
};
/* each kind's offset from a row's base or conjugation, or NO_OFFSET where no
 * ending can follow it */
#define NO_OFFSET 255
static Uint8 part_offsets[256] = {
#include "network/ask_connections/part_offsets.inc"
};
static Uint8 masks[8] = {
#include "network/ask_connections/masks.inc"
};
#pragma section
static int bit_on(Sint16 bit)
{
#ifdef NONMATCHING
    /* this side refuses a bit past connections, and reads the bit's byte from
     * the word that holds it, low byte first, the byte the matching side's read
     * finds on the SH-4 */
    if (bit < 0 || bit >= sizeof(connections) * 8) {
        return 0;
    }
    return (connections[(unsigned)bit >> 5] >> (bit & 24)) & masks[bit & 7];
#else
    return ((Uint8 *)connections)[bit >> 3] & masks[bit & 7];
#endif
}
static Uint8 conjugated_kind(Uint8 kind)
{
    if (kind >= 1 && kind <= ASK_LAST_CONJUGATING) {
        return kind + (CONJUGATING_ENDINGS - 1);
    }
    return 0;
}
int goku_connect(Uint8 left, Uint8 form, Uint8 right)
{
    Sint16 offset;
    const struct connection_row *row;
    if (right < CONJUGATING_ENDINGS) {
        return 0;
    }
    if (left < ASK_FORM_KINDS && form) {
        if (!(left = conjugated_kind(left))) {
            return 0;
        }
    }
    if ((offset = part_offsets[left]) == NO_OFFSET) {
        return 0;
    }
#ifdef NONMATCHING
    /* this side refuses a right ending between the conjugating rows and the
     * plain endings, which indexes no row of endings */
    if (right >= CONJUGATING_ENDINGS + CONJUGATING_ROWS &&
        right < PLAIN_ENDINGS) {
        return 0;
    }
#endif
    row = right >= PLAIN_ENDINGS
              ? &endings[right - PLAIN_ENDINGS]
              : &endings[right - CONJUGATING_ENDINGS] + PLAIN_ROWS;
    if (left < CONJUGATING_ENDINGS || left >= PLAIN_ENDINGS) {
        return bit_on(row->base + offset);
    }
    return bit_on(row->conjugation + offset + form - 1);
}
int setu_end(Uint8 kind, Uint8 form)
{
    if (kind >= PLAIN_ENDINGS) {
        return 1;
    }
    if (kind < CONJUGATING_ENDINGS) {
        if (kind < ASK_FORM_KINDS && form) {
            if (!(kind = conjugated_kind(kind))) {
                return 0;
            }
        } else {
            if (kind == 13 || (kind >= 19 && kind <= 52) || kind == 56) {
                return 1;
            }
            return 0;
        }
    }
    switch (kind) {
    case 0x8b:
    case 0x8d:
    case 0x93:
    case 0x96:
    case 0x99:
    case 0x9a:
    case 0x9b:
    case 0x9e:
    case 0xa1:
    case 0xa2:
    case 0xa7:
    case 0xa8:
    case 0xb1:
        if ((form >= 5 && form <= 8) || form >= 10) {
            return 1;
        }
        return 0;
    case 0x97:
        if (form == 8) {
            return 0;
        }
        /* fall through */
    case 0x8e:
    case 0x92:
    case 0x94:
    case 0x95:
    case 0x98:
    case 0xa0:
        if (form >= 5) {
            return 1;
        }
        return 0;
    default:
        if (form == 4 || form == 7 || form == 8) {
            return 1;
        }
        if (form >= 10) {
            return 1;
        }
        return 0;
    }
}
