/* the luma of decoded macroblocks as a twiddled texture. a macroblock holds
 * six 8 by 8 blocks of signed samples. two hold chroma, and then four hold
 * luma in row order. a twiddled texture interleaves the bits of a texel's row
 * and column into its index. the row's bit is the lower of each pair */
#include <sg_xpt.h>
#include <sofdec_yuv.h>
#include <string.h>

#define BLOCK_WIDTH 8
#define BLOCK_PIXELS (BLOCK_WIDTH * BLOCK_WIDTH)
#define CHROMA_BLOCKS 2
#define LUMA_BLOCKS 4
/* the luma blocks across a macroblock. a step of BLOCKS_ACROSS blocks moves
 * down a row of them */
#define BLOCKS_ACROSS 2
#define MACROBLOCK_WIDTH 16
#define MACROBLOCK_PIXELS (MACROBLOCK_WIDTH * MACROBLOCK_WIDTH)
#define MACROBLOCK_BYTES ((CHROMA_BLOCKS + LUMA_BLOCKS) * BLOCK_PIXELS)
#define SAMPLE_VALUES 256
#define HALF_PALETTE (SAMPLE_VALUES / 2)
/* the macroblock columns and rows spread_bits covers */
#define MOST_MACROBLOCKS 64
/* mwYuvConvMblkToTwdl256's picture is PICTURE_WIDTH macroblocks square. it
 * converts the picture in PICTURE_SQUARES squares of 2 by 2 macroblocks */
#define PICTURE_WIDTH 16
#define PICTURE_SQUARES 64

/* the palettes of the luminance and alpha modes. each is indexed from
 * HALF_PALETTE by a signed sample, and gives the sample's texel */
static Sint8 luminance[SAMPLE_VALUES] = {
#include "sofdec/yuv_conv/luminance.inc"
};
static Sint8 alpha3[SAMPLE_VALUES] = {
#include "sofdec/yuv_conv/alpha3.inc"
};
static Sint8 alpha5[SAMPLE_VALUES] = {
#include "sofdec/yuv_conv/alpha5.inc"
};
/* a macroblock's column or row with its bits spread to the even bits */
static Sint16 spread_bits[MOST_MACROBLOCKS] = {
#include "sofdec/yuv_conv/spread_bits.inc"
};
/* the steps in macroblocks between the picture's squares, in twiddled order.
 * a step runs from the last macroblock of one square to the first of the
 * next. the first step starts at the picture's first macroblock */
static Sint32 tile_steps[PICTURE_SQUARES] = {
#include "sofdec/yuv_conv/tile_steps.inc"
};
/* the samples YUV_ALPHA3 and YUV_ALPHA5 write back over the source. each
 * table is indexed from HALF_PALETTE by the signed sample it replaces */
static Sint8 alpha3_luma[SAMPLE_VALUES] = {
#include "sofdec/yuv_conv/alpha3_luma.inc"
};
static Sint8 alpha5_luma[SAMPLE_VALUES] = {
#include "sofdec/yuv_conv/alpha5_luma.inc"
};
static void mwYuvConvMblkToTwdl(Sint8 *dest, Sint8 *src, Sint32 mode);
static void mwYuvConvBlkToTwdlLumi(Sint8 *dest, const Sint8 *src);
static void mwYuvConvBlkToTwdl3Alph(Sint8 *dest, Sint8 *src);
static void mwYuvConvBlkToTwdl5Alph(Sint8 *dest, Sint8 *src);
static void mwYuvConvBlkToTwdl256Alph(Sint8 *dest, const Sint8 *src);

/* copy the caller's src, SAMPLE_VALUES entries, into table through the
 * caller's palette. src is indexed by a sample's byte, so its halves swap
 * places in the table */
/* @hack spelling: the copies in each arm, where copying once after choosing
 * the table would do */
#define SET_PALETTE(table)                                                     \
    {                                                                          \
        palette = (table);                                                     \
        memcpy(palette + HALF_PALETTE, src, HALF_PALETTE);                     \
        memcpy(palette, src + HALF_PALETTE, HALF_PALETTE);                     \
    }
void mwYuvSetPltTbl(const Sint8 *src, Sint32 mode)
{
    Sint8 *palette;
    if (mode == YUV_ALPHA3) {
        SET_PALETTE(alpha3);
    } else if (mode == YUV_ALPHA5) {
        SET_PALETTE(alpha5);
    } else {
        SET_PALETTE(luminance);
    }
}
void mwYuvGetPltTbl(Sint8 *dest, Sint32 mode)
{
    Sint8 *palette;
    if (mode == YUV_ALPHA3) {
        palette = alpha3;
    } else if (mode == YUV_ALPHA5) {
        palette = alpha5;
    } else {
        palette = luminance;
    }
    memcpy(dest + HALF_PALETTE, palette, HALF_PALETTE);
    memcpy(dest, palette + HALF_PALETTE, HALF_PALETTE);
}
void mwYuvConvMblkToTwdlRctngl(Sint8 *dest, Sint8 *src, Sint32 pitch, Sint32 x,
                               Sint32 y, Sint32 width, Sint32 height,
                               Sint32 mode)
{
    Sint8 *block;
    Sint8 *output;
    Sint32 column;
    Sint32 first_column;
    Sint32 row;
    Sint32 first_row;
    Sint32 end_column;
    Sint32 end_row;
    Sint32 stride;
    Sint32 line;
    Sint32 twiddled_x;
    Sint32 twiddled_y;
    /* @hack spelling: spread_bits read through a local, where indexing it
     * would do */
    Sint16 *spread = spread_bits;
    end_column = (x + width) / MACROBLOCK_WIDTH;
    end_row = (y + height) / MACROBLOCK_WIDTH;
    stride = pitch / MACROBLOCK_WIDTH;
    first_column = x / MACROBLOCK_WIDTH;
    first_row = y / MACROBLOCK_WIDTH;
    for (row = first_row; row < end_row; row++) {
        line = stride * row;
        twiddled_y = spread[row];
        for (column = first_column; column < end_column; column++) {
            block = src + ((column + line) * MACROBLOCK_BYTES);
            twiddled_x = spread[column];
            output =
                dest + (((twiddled_x << 1) | twiddled_y) * MACROBLOCK_PIXELS);
            mwYuvConvMblkToTwdl(output, block, mode);
        }
    }
}
void mwYuvConvMblkToTwdlRctngl2(Sint8 *dest, Sint8 *src, Sint32 pitch, Sint32 x,
                                Sint32 y, Sint32 width, Sint32 height,
                                Sint32 mode)
{
    Sint8 *block;
    Sint8 *output;
    Sint32 column;
    Sint32 first_column;
    Sint32 row;
    Sint32 first_row;
    Sint32 end_column;
    Sint32 end_row;
    Sint32 stride;
    Sint32 line;
    end_column = (x + width) / MACROBLOCK_WIDTH;
    end_row = (y + height) / MACROBLOCK_WIDTH;
    stride = pitch / MACROBLOCK_WIDTH;
    first_column = x / MACROBLOCK_WIDTH;
    first_row = y / MACROBLOCK_WIDTH;
    for (row = first_row; row < end_row; row++) {
        line = stride * row;
        for (column = first_column; column < end_column; column++) {
            block = src + ((column + line) * MACROBLOCK_BYTES);
            output = dest + (mwYuvCombLct(column, row) * MACROBLOCK_PIXELS);
            mwYuvConvMblkToTwdl(output, block, mode);
        }
    }
}
void mwYuvConvMblkToTwdl256(Sint8 *dest, Sint8 *src, Sint32 mode)
{
    Sint32 i;
    for (i = 0; i < PICTURE_SQUARES; i++) {
        src += tile_steps[i] * MACROBLOCK_BYTES;
        mwYuvConvMblkToTwdl(dest, src, mode);
        src += PICTURE_WIDTH * MACROBLOCK_BYTES;
        dest += MACROBLOCK_PIXELS;
        mwYuvConvMblkToTwdl(dest, src, mode);
        src -= (PICTURE_WIDTH - 1) * MACROBLOCK_BYTES;
        dest += MACROBLOCK_PIXELS;
        mwYuvConvMblkToTwdl(dest, src, mode);
        src += PICTURE_WIDTH * MACROBLOCK_BYTES;
        dest += MACROBLOCK_PIXELS;
        mwYuvConvMblkToTwdl(dest, src, mode);
        dest += MACROBLOCK_PIXELS;
    }
}
/* convert the luma blocks of the caller's macroblock at src into dest's
 * texels. twiddled order takes them top left, bottom left, top right, then
 * bottom right. it steps the caller's src and dest */
#define CONVERT_QUADRANTS(convert)                                             \
    {                                                                          \
        src += CHROMA_BLOCKS * BLOCK_PIXELS;                                   \
        convert(dest, src);                                                    \
        src += BLOCKS_ACROSS * BLOCK_PIXELS;                                   \
        dest += BLOCK_PIXELS;                                                  \
        convert(dest, src);                                                    \
        src -= BLOCK_PIXELS;                                                   \
        dest += BLOCK_PIXELS;                                                  \
        convert(dest, src);                                                    \
        src += BLOCKS_ACROSS * BLOCK_PIXELS;                                   \
        dest += BLOCK_PIXELS;                                                  \
        convert(dest, src);                                                    \
    }
static void mwYuvConvMblkToTwdl(Sint8 *dest, Sint8 *src, Sint32 mode)
{
    if (mode == YUV_ALPHA3) {
        CONVERT_QUADRANTS(mwYuvConvBlkToTwdl3Alph);
    } else if (mode == YUV_ALPHA5) {
        CONVERT_QUADRANTS(mwYuvConvBlkToTwdl5Alph);
    } else if (mode == YUV_ALPHA256) {
        CONVERT_QUADRANTS(mwYuvConvBlkToTwdl256Alph);
    } else {
        CONVERT_QUADRANTS(mwYuvConvBlkToTwdlLumi);
    }
}

/* write a block's texels in twiddled order, backward from the last, through
 * the caller's src and dest. SQUARE_2 and SQUARE_4 each write the square
 * whose bottom right sample src is at. they write its quadrants bottom right,
 * top right, bottom left, then top left. src ends each quadrant at its top left
 * sample. from there it steps to the next quadrant's bottom right. it ends
 * the whole square at its top left sample. SQUARE_8 takes src at the
 * block's first sample and dest at its first texel */
#define SQUARE_2(write)                                                        \
    do {                                                                       \
        write;                                                                 \
        TO_UPPER(2);                                                           \
        write;                                                                 \
        TO_LOWER_LEFT(2);                                                      \
        write;                                                                 \
        TO_UPPER(2);                                                           \
        write;                                                                 \
    } while (0)
#define SQUARE_4(write)                                                        \
    do {                                                                       \
        SQUARE_2(write);                                                       \
        TO_UPPER(4);                                                           \
        SQUARE_2(write);                                                       \
        TO_LOWER_LEFT(4);                                                      \
        SQUARE_2(write);                                                       \
        TO_UPPER(4);                                                           \
        SQUARE_2(write);                                                       \
    } while (0)
#define SQUARE_8(write)                                                        \
    do {                                                                       \
        src += BLOCK_PIXELS - 1;                                               \
        dest += BLOCK_PIXELS;                                                  \
        SQUARE_4(write);                                                       \
        TO_UPPER(8);                                                           \
        SQUARE_4(write);                                                       \
        TO_LOWER_LEFT(8);                                                      \
        SQUARE_4(write);                                                       \
        TO_UPPER(8);                                                           \
        SQUARE_4(write);                                                       \
    } while (0)
/* src's steps within a square width samples wide. each runs from the top
 * left sample of the quadrant just written to the next's bottom right.
 * TO_UPPER goes up to the quadrant above. TO_LOWER_LEFT goes from the top
 * right quadrant down to the bottom left */
#define TO_UPPER(width) (src -= BLOCK_WIDTH - (((width) / 2) - 1))
#define TO_LOWER_LEFT(width) (src += (((width) - 1) * BLOCK_WIDTH) - 1)
#define PALETTE_PIXEL() (*--dest = palette[*src])
#define ALPHA_PIXEL() (PALETTE_PIXEL(), *src = rewrite[*src])
#define COPY_PIXEL() (*--dest = *src)

static void mwYuvConvBlkToTwdlLumi(Sint8 *dest, const Sint8 *src)
{
    /* @hack spelling: palette stepped to the table's middle in a statement
     * of its own, where adding HALF_PALETTE in its initializer would do */
    const Sint8 *palette = luminance;
    palette += HALF_PALETTE;
    SQUARE_8(PALETTE_PIXEL());
}
#define ALPHA_BLOCK(name, palette_table, rewrite_table)                        \
    static void name(Sint8 *dest, Sint8 *src)                                  \
    {                                                                          \
        /* @hack spelling: palette and rewrite stepped to their tables'        \
         * middles in statements of their own, where adding HALF_PALETTE in    \
         * their initializers would do */                                      \
        const Sint8 *palette = (palette_table);                                \
        const Sint8 *rewrite = (rewrite_table);                                \
        palette += HALF_PALETTE;                                               \
        rewrite += HALF_PALETTE;                                               \
        SQUARE_8(ALPHA_PIXEL());                                               \
    }
ALPHA_BLOCK(mwYuvConvBlkToTwdl3Alph, alpha3, alpha3_luma)
ALPHA_BLOCK(mwYuvConvBlkToTwdl5Alph, alpha5, alpha5_luma)
static void mwYuvConvBlkToTwdl256Alph(Sint8 *dest, const Sint8 *src)
{
    SQUARE_8(COPY_PIXEL());
}
