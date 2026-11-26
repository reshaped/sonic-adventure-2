/* the test for an empty B picture. its one slice codes only its first and
 * last macroblocks. each is predicted from one reference picture, with a
 * zero motion vector and no coded blocks. the macroblocks between them are
 * skipped, so each repeats the prediction before it */
#include <sg_xpt.h>
#include <sofdec.h>
#include "../sofdec/sfd_bits.h"

/* the bits of a slice's quantizer_scale */
#define QUANTIZER_BITS 5

/* the codes that give a macroblock one reference, a zero vector and no coded
 * blocks. a peek of the next LOOKAHEAD_BITS bits reads each, and the forward
 * code fills the peek. the backward code, 01011, takes only BACKWARD_BITS, so
 * its peek is BACKWARD_ZERO or BACKWARD_ZERO + 1 */
#define LOOKAHEAD_BITS 6
#define BACKWARD_ZERO 22
#define BACKWARD_BITS 5
#define FORWARD_ZERO 11
#define FORWARD_BITS 6

/* macroblock_escape, its 11 bits, and the address increment it adds */
#define MBA_ESCAPE 8
#define ESCAPE_BITS 11
#define ESCAPE_INCREMENT 33

/* an entry holds an increment's code in its high byte, and the code's length
 * in its low */
#define MBA(code, bits) (((code) << 8) | (bits))
#define MBA_CODE(entry) ((Uint32)(entry) >> 8)
#define MBA_BITS(entry) ((entry) & 0xff)

/* macroblock_address_increment's code for each increment, from 1 to
 * ESCAPE_INCREMENT. entry 0 is no increment */
static Sint16 increment_codes[] = {
    MBA(0x00, 0),  MBA(0x01, 1),  MBA(0x03, 3),  MBA(0x02, 3),  MBA(0x03, 4),
    MBA(0x02, 4),  MBA(0x03, 5),  MBA(0x02, 5),  MBA(0x07, 7),  MBA(0x06, 7),
    MBA(0x0b, 8),  MBA(0x0a, 8),  MBA(0x09, 8),  MBA(0x08, 8),  MBA(0x07, 8),
    MBA(0x06, 8),  MBA(0x17, 10), MBA(0x16, 10), MBA(0x15, 10), MBA(0x14, 10),
    MBA(0x13, 10), MBA(0x12, 10), MBA(0x23, 11), MBA(0x22, 11), MBA(0x21, 11),
    MBA(0x20, 11), MBA(0x1f, 11), MBA(0x1e, 11), MBA(0x1d, 11), MBA(0x1c, 11),
    MBA(0x1b, 11), MBA(0x1a, 11), MBA(0x19, 11), MBA(0x18, 11), MBA(0x0f, 11),
    MBA(0x08, 11)};

Bool MPV_IsEmptyBpic(const Uint8 *data, Sint32 size, Sint32 macroblocks)
{
    /* @hack redundant: cursor, bit, current, lookahead and code declared
     * register, where plain locals would do */
    register const Uint8 *cursor;
    register Sint32 bit;
    register Uint32 current;
    register Uint32 lookahead;
    register Uint32 code;
    Sint16 entry;
    Sint32 width;
    Sint32 end;
    Sint32 used;

    /* code takes the start code, from data's first byte however aligned */
    cursor = (const Uint8 *)((Uint32)data & ~(WORD_BYTES - 1UL));
    bit = (Sint32)data - (Sint32)cursor;
    bit *= BYTE_BITS;
    READ_WORD(current);
    current <<= bit;
    READ_WORD(lookahead);
    if (bit != 0) {
        current |= lookahead >> (WORD_BITS - bit);
        code = current;
        current = lookahead << bit;
    } else {
        code = current;
        current = lookahead;
    }
    READ_WORD(lookahead);
    if (code != MPV_FIRST_SLICE_CODE) {
        return FALSE;
    }
    /* quantizer_scale, then extra_bit_slice, which must be 0 */
    GET_BITS(QUANTIZER_BITS, code);
    READ_FLAG(code);
    if (code != 0) {
        return FALSE;
    } else {
        /* @hack redundant: an empty else */
    }
    /* the first macroblock's address increment, which must be 1: a lone 1 */
    READ_FLAG(code);
    if (code == 0) {
        return FALSE;
    }
    PEEK_BITS(LOOKAHEAD_BITS, code);
    switch (code) {
    case BACKWARD_ZERO:
    case BACKWARD_ZERO + 1:
        /* @hack spelling: SKIP_BITS written out, the shift of current between
         * the loads, where SKIP_BITS(BACKWARD_BITS, WORD_BITS) would do */
        bit += BACKWARD_BITS;
        if (bit >= WORD_BITS) {
            current = lookahead;
            bit -= WORD_BITS;
            lookahead = (Sint8)*cursor;
            cursor++;
            lookahead <<= 8;
            lookahead |= *cursor;
            cursor++;
            lookahead <<= 8;
            lookahead |= *cursor;
            cursor++;
            current <<= bit;
            lookahead <<= 8;
            lookahead |= *cursor++;
        } else {
            current <<= BACKWARD_BITS;
        }
        break;
    case FORWARD_ZERO:
        SKIP_BITS(FORWARD_BITS, WORD_BITS);
        break;
    default:
        return FALSE;
    }
    /* the increment from the first macroblock to the last. each escape before
     * its code adds ESCAPE_INCREMENT */
    macroblocks--;
    do {
        PEEK_BITS(ESCAPE_BITS, code);
        if (code == MBA_ESCAPE) {
            SKIP_BITS(ESCAPE_BITS, WORD_BITS);
        } else {
            break;
        }
    } while ((macroblocks -= ESCAPE_INCREMENT) > ESCAPE_INCREMENT);
    /* @hack redundant: a goto to the label right below it */
    goto check_range;
check_range:
    if (macroblocks <= 0 || macroblocks > ESCAPE_INCREMENT) {
        return FALSE;
    }
    entry = increment_codes[macroblocks];
    width = MBA_BITS(entry);
    GET_BITS(width, code);
    if (code != MBA_CODE(entry)) {
        return FALSE;
    }
    PEEK_BITS(LOOKAHEAD_BITS, code);
    switch (code) {
    case BACKWARD_ZERO:
    case BACKWARD_ZERO + 1:
        SKIP_BITS(BACKWARD_BITS, WORD_BITS);
        break;
    case FORWARD_ZERO:
        /* @hack spelling: SKIP_BITS written out, the next word read by index,
         * where SKIP_BITS(FORWARD_BITS, WORD_BITS) would do */
        bit += FORWARD_BITS;
        if (bit >= WORD_BITS) {
            bit -= WORD_BITS;
            current = lookahead << bit;
            lookahead = (Sint8)cursor[0];
            lookahead = (lookahead << 8) | cursor[1];
            lookahead = (lookahead << 8) | cursor[2];
            lookahead = (lookahead << 8) | cursor[3];
            cursor += WORD_BYTES;
        } else {
            current <<= FORWARD_BITS;
        }
        break;
    default:
        return FALSE;
    }
    /* the bytes read up to the last bit */
    /* @hack type: end a byte count and then an address, as a Sint32, where a
     * pointer to the byte past the last bit would do */
    end = (bit + BYTE_BITS - 1) >> BYTE_SHIFT;
    cursor -= AHEAD_BYTES;
    end += (Sint32)cursor;
    used = end - (Sint32)data;
    return used > size ? FALSE : TRUE;
}
