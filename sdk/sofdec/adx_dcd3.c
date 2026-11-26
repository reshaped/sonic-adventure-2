/* CRI ADX's decoders that predict in floats. a block of ADX data is a
 * big-endian halfword, its header, then 16 bytes. each byte codes two
 * samples. a byte's high 4 bits code the first, and its low 4 bits the
 * second */
#include <adx_dcd.h>
#include <adx_dcd3.h>
#include <adx_qtbl.h>
#include <sg_xpt.h>

#define BLK_SMPL 32
/* the bytes of a block's header, which its samples follow */
#define BLK_HDR_LEN 2
/* a header with HDR_END set ends the data. HDR_SCALE holds the block's scale
 * less 1 */
#define HDR_END 0x8000
#define HDR_SCALE 0x1fff
/* the halfword w with its two bytes swapped. bits past the low 16 remain,
 * and the Sint16 that takes the result drops them */
#define SWAP16(w) ((((w) >> 8) & 0xff) | (((w) << 8) & ~0xff))

Sint32 ADX_DecodeMonoFloat(Sint8 *ibuf, Sint32 nblk, Sint16 *obuf, Sint16 *dly,
                           Sint16 k0, Sint16 k1)
{
    /* @hack spelling: the locals declared in this order, where the order of
     * their first uses would do */
    /* @hack spelling: in, a local that holds ibuf as bytes, where ibuf read
     * through casts would do */
    Uint8 *in;
    Sint32 blk;
    Sint16 header;
    Sint32 j;
    /* @hack redundant: register on code, sample and the floats, where plain
     * locals would do */
    register Sint32 code;
    /* @hack spelling: sample, a local for x converted, where (Sint32)x in
     * the shift would do */
    register Sint32 sample;
    register float c0;
    register float c1;
    register float newer;
    register float older;
    register float scale;
    /* @hack spelling: step, a local for the table's entry, where the entry
     * read in the sum would do */
    register float step;
    register float x;

    in = (Uint8 *)ibuf;
    newer = (float)(dly[0] << ADX_FRAC_BITS);
    older = (float)(dly[1] << ADX_FRAC_BITS);
    c0 = (float)(k0 << (ADX_FRAC_BITS - ADX_COEF_BITS)) / ADX_SAMPLE_UNIT;
    c1 = (float)(k1 << (ADX_FRAC_BITS - ADX_COEF_BITS)) / ADX_SAMPLE_UNIT;

    for (blk = 0; blk < nblk; blk++) {
        header = SWAP16(*(Sint16 *)in);
        if (header & HDR_END) {
            return blk;
        }
        scale = (float)((header & HDR_SCALE) + 1);
        in += BLK_HDR_LEN;
        for (j = 0; j < BLK_SMPL; j += 2) {
            code = *in++;

            step = AdxQtblFloat0[code];
            x = (c0 * newer) + (c1 * older) + (scale * step);
            older = newer;
            newer = x;
            sample = (Sint32)x;
            *obuf++ = sample >> ADX_FRAC_BITS;

            step = AdxQtblFloat1[code];
            x = (c0 * newer) + (c1 * older) + (scale * step);
            older = newer;
            newer = x;
            sample = (Sint32)x;
            *obuf++ = sample >> ADX_FRAC_BITS;
        }
    }
    dly[0] = (Sint32)newer >> ADX_FRAC_BITS;
    dly[1] = (Sint32)older >> ADX_FRAC_BITS;
    return nblk;
}

Sint32 ADX_DecodeSteFloat(Sint8 *ibuf, Sint32 nblk, Sint16 *obufl, Sint16 *dlyl,
                          Sint16 *obufr, Sint16 *dlyr, Sint16 k0, Sint16 k1)
{
    /* @hack spelling: the locals declared in this order, where the order of
     * their first uses would do */
    Sint32 blk;
    /* @hack spelling: left_blk and right_blk, counters of the indexes of
     * each pair's left and right block, where blk * 2 and blk * 2 + 1 would
     * do */
    Sint32 right_blk;
    Sint32 left_blk;
    /* @hack spelling: step, a local for the table's entry, where the entry
     * read in the sum would do */
    float step;
    /* @hack spelling: in, a local that holds ibuf as bytes, where ibuf read
     * through casts would do */
    Uint8 *in;
    float x;
    float newer_l;
    float c1;
    float scale;
    float newer_r;
    float older_l;
    Sint32 code;
    float c0;
    Sint16 header;
    Sint32 j;
    float older_r;
    /* @hack redundant: register on sample, where a plain local would do */
    /* @hack spelling: sample, a local for x converted, where (Sint32)x in
     * the shift would do */
    register Sint32 sample;

    in = (Uint8 *)ibuf;
    newer_l = (float)(dlyl[0] << ADX_FRAC_BITS);
    older_l = (float)(dlyl[1] << ADX_FRAC_BITS);
    newer_r = (float)(dlyr[0] << ADX_FRAC_BITS);
    older_r = (float)(dlyr[1] << ADX_FRAC_BITS);
    c0 = (float)(k0 << (ADX_FRAC_BITS - ADX_COEF_BITS)) / ADX_SAMPLE_UNIT;
    c1 = (float)(k1 << (ADX_FRAC_BITS - ADX_COEF_BITS)) / ADX_SAMPLE_UNIT;

    /* @hack spelling: right_blk = right_blk + 2, where right_blk += 2 would
     * do */
    for (blk = 0, right_blk = 1, left_blk = 0; blk < nblk / 2;
         blk++, right_blk = right_blk + 2, left_blk += 2) {
        header = SWAP16(*(Sint16 *)in);
        if (header & HDR_END) {
            return left_blk;
        }
        scale = (float)((header & HDR_SCALE) + 1);
        in += BLK_HDR_LEN;
        for (j = 0; j < BLK_SMPL; j += 2) {
            code = *in++;

            step = AdxQtblFloat0[code];
            x = (c0 * newer_l) + (c1 * older_l) + (scale * step);
            older_l = newer_l;
            newer_l = x;
            sample = (Sint32)x;
            *obufl++ = sample >> ADX_FRAC_BITS;

            step = AdxQtblFloat1[code];
            x = (c0 * newer_l) + (c1 * older_l) + (scale * step);
            older_l = newer_l;
            newer_l = x;
            sample = (Sint32)x;
            *obufl++ = sample >> ADX_FRAC_BITS;
        }

        header = SWAP16(*(Sint16 *)in);
        if (header & HDR_END) {
            return right_blk;
        }
        scale = (float)((header & HDR_SCALE) + 1);
        in += BLK_HDR_LEN;
        for (j = 0; j < BLK_SMPL; j += 2) {
            code = *in++;

            step = AdxQtblFloat0[code];
            x = (c0 * newer_r) + (c1 * older_r) + (scale * step);
            older_r = newer_r;
            newer_r = x;
            sample = (Sint32)x;
            *obufr++ = sample >> ADX_FRAC_BITS;

            step = AdxQtblFloat1[code];
            x = (c0 * newer_r) + (c1 * older_r) + (scale * step);
            older_r = newer_r;
            newer_r = x;
            sample = (Sint32)x;
            *obufr++ = sample >> ADX_FRAC_BITS;
        }
    }
    dlyl[0] = (Sint32)newer_l >> ADX_FRAC_BITS;
    dlyl[1] = (Sint32)older_l >> ADX_FRAC_BITS;
    dlyr[0] = (Sint32)newer_r >> ADX_FRAC_BITS;
    dlyr[1] = (Sint32)older_r >> ADX_FRAC_BITS;
    return nblk;
}
