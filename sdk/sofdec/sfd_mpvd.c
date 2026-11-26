/* whether a sequence header start code begins a true header. a true header's
 * aspect ratio code, frame rate code, bit rate and marker bit are each
 * nonzero. a header that ends before those fields counts as true */
#include <sfd_mpvd.h>
#include <sg_xpt.h>
#include <sofdec.h>

/* a sequence header's fields, by their bytes' offsets from its start code.
 * the byte at RATE_CODES_AT holds the aspect ratio code, then the frame rate
 * code, CODE_BITS each. the BIT_RATE_SPAN bytes from BIT_RATE_AT hold the
 * 18-bit bit rate from their top bit, then the marker bit */
#define RATE_CODES_AT 7
#define CODE_BITS 4
#define CODE_MASK 15
#define BIT_RATE_AT 8
#define BIT_RATE_SPAN 3
#define BIT_RATE_SHIFT 6
#define MARKER_SHIFT 5

static Sint32 sfmpvd_ReadParPr(const Uint8 *data, Sint32 size, Sint32 *aspect,
                               Sint32 *rate);
static Sint32 sfmpvd_ReadBrMb(const Uint8 *data, Sint32 size, Sint32 *bit_rate,
                              Sint32 *marker);

Bool SFMPVD_IsShc(Uint8 *data, Sint32 size)
{
    Sint32 bit_rate;
    Sint32 marker;
    Sint32 aspect;
    Sint32 rate;

    if (MPV_CheckDelim(data) != MPV_DELIM_SEQUENCE) {
        return FALSE;
    }
    if (sfmpvd_ReadParPr(data, size, &aspect, &rate) != 0) {
        return TRUE;
    }
    if (aspect == 0) {
        return FALSE;
    }
    if (rate == 0) {
        return FALSE;
    }
    if (sfmpvd_ReadBrMb(data, size, &bit_rate, &marker) != 0) {
        return TRUE;
    }
    if (bit_rate == 0) {
        return FALSE;
    }
    if (marker == 0) {
        return FALSE;
    }
    return TRUE;
}

static Sint32 sfmpvd_ReadParPr(const Uint8 *data, Sint32 size, Sint32 *aspect,
                               Sint32 *rate)
{
    Uint32 fields;

    if (size < RATE_CODES_AT + 1) {
        return -1;
    }
    /* @hack spelling: data stepped on to the byte, where data[RATE_CODES_AT]
     * would do */
    data += RATE_CODES_AT;
    fields = *data;
    *aspect = (fields >> CODE_BITS) & CODE_MASK;
    *rate = fields & CODE_MASK;
    return 0;
}

static Sint32 sfmpvd_ReadBrMb(const Uint8 *data, Sint32 size, Sint32 *bit_rate,
                              Sint32 *marker)
{
    const Uint8 *cursor;
    Uint32 fields;

    cursor = data;
    if (size < BIT_RATE_AT + BIT_RATE_SPAN) {
        return -1;
    }
    /* @hack spelling: cursor stepped along the three bytes, where indexing
     * data would do */
    cursor += BIT_RATE_AT;
    fields = *cursor++;
    fields <<= 8;
    fields |= *cursor++;
    fields <<= 8;
    fields |= *cursor;
    /* @bug logic: the mask 0x2ffff drops bit 16 of the 18-bit bit rate, so
     * SFMPVD_IsShc returns FALSE for a header whose rate is 0x10000
     * (0x8c54eeee) */
    *bit_rate = (fields >> BIT_RATE_SHIFT) & 0x2ffff;
    *marker = (fields >> MARKER_SHIFT) & 1;
    return 0;
}
