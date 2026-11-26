/* ADX_DecodeInfoWav on a WAVE header whole, and on the same header cut two
 * bytes into its data chunk's ID. the whole header reads as 16-bit mono PCM.
 * the cut one is refused, though the bytes past its end complete the ID */
#include <assert.h>
#include <string.h>

#include "adx_bwav.c"

/* RIFF and its size, WAVE, and a fmt chunk of 16 bytes come first */
#define DATA_AT 36
#define HEADER_BYTES (DATA_AT + CHUNK_HEADER_BYTES)
#define DATA_BYTES 0x1000
#define RATE 22050

static Uint8 *put_id(Uint8 *p, const char *id)
{
    memcpy(p, id, CHUNK_ID_BYTES);
    return p + CHUNK_ID_BYTES;
}

static Uint8 *put_le(Uint8 *p, Uint32 value, Sint32 bytes)
{
    Sint32 i;

    for (i = 0; i < bytes; i++) {
        *p++ = value >> (i * 8);
    }
    return p;
}

int main(void)
{
    /* aligned as a ring's chunk is, for the wave_format read in place */
    _Alignas(4) Uint8 header[HEADER_BYTES];
    Uint8 *p = header;
    Sint16 offset;
    Sint8 enc;
    Sint8 bits;
    Sint8 block_bytes;
    Sint8 channels;
    Sint32 rate;
    Sint32 total;
    Sint32 block_samples;
    Sint16 encoding;

    p = put_id(p, "RIFF");
    p = put_le(p, HEADER_BYTES - CHUNK_HEADER_BYTES + DATA_BYTES, 4);
    p = put_id(p, wave_id);
    p = put_id(p, format_id);
    p = put_le(p, sizeof(struct wave_format), 4);
    p = put_le(p, WAVE_FORMAT_PCM, 2);
    p = put_le(p, 1, 2);
    p = put_le(p, RATE, 4);
    p = put_le(p, RATE * 2, 4);
    p = put_le(p, 2, 2);
    p = put_le(p, 16, 2);
    assert(p == header + DATA_AT);
    p = put_id(p, data_id);
    put_le(p, DATA_BYTES, 4);

    assert(ADX_DecodeInfoWav(header, HEADER_BYTES, &offset, &enc, &bits,
                             &block_bytes, &channels, &rate, &total,
                             &block_samples, &encoding) == 0);
    assert(offset == HEADER_BYTES);
    assert(channels == 1 && bits == 16 && rate == RATE);
    assert(total == DATA_BYTES / 2);
    assert(encoding == ADXB_PCM16);
    assert(ADX_DecodeInfoWav(header, DATA_AT + 2, &offset, &enc, &bits,
                             &block_bytes, &channels, &rate, &total,
                             &block_samples, &encoding) == -1);
    return 0;
}
