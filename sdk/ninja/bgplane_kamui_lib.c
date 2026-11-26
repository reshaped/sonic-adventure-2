/* Kamui's background plane: one triangle behind every other polygon. the
 * plane's packet goes in after the ISP parameters when the tile accelerator
 * finishes a frame */
#include <kamui.h>
#include <kamui_internal.h>

/* the ISP word's bits that set which words a vertex holds after x, y and z */
#define ISP_TEXTURE 0x02000000 /* UVs */
#define ISP_OFFSET 0x01000000 /* an offset color */
#define ISP_UV16 0x00400000 /* both UVs packed in one word */
/* the words every vertex holds after x, y and z: its base color */
#define BASE_COLOR_WORDS 1

/* background_skip's fields. the words a vertex holds after x, y and z go in
 * SKIP_MASK from SKIP_SHIFT. SKIP_SHADOW and SKIP_CACHE_BYPASS are bits of
 * their own */
#define SKIP_SHIFT 24
#define SKIP_MASK 7
#define SKIP_SHADOW 0x08000000
#define SKIP_CACHE_BYPASS 0x10000000

/* the plane's ISP, TSP and texture words, then its three vertices */
#define BACKGROUND_PACKET kmiDeviceExtension.background_packet

/* the plane Kamui starts with, which is black and at DEFAULT_DEPTH. its ISP
 * word draws it Gouraud shaded where its depth is greater. its TSP word
 * blends it one and zero, without fog */
#define DEFAULT_ISP 0x80800000
#define DEFAULT_TSP 0x20800000
#define BLACK 0xff000000
#define DEFAULT_DEPTH 0x3727c5ac /* 0.00001F, as a float's bits */

#ifdef NONMATCHING
static KMUINT32 float_bits(KMFLOAT value)
{
    union {
        KMFLOAT value;
        KMUINT32 bits;
    } word;
    word.value = value;
    return word.bits;
}
#define FLOAT_BITS(value) float_bits(value)
#else
/* @hack type: a float's bits read through its address cast to a word
 * pointer, where the union of the NONMATCHING side would do */
#define FLOAT_BITS(value) (*(const KMUINT32 *)&(value))
#endif

static void set_skip(KMINT32 words, KMBOOLEAN shadow, KMBOOLEAN bypass)
{
    kmiDeviceExtension.background_skip = (words & SKIP_MASK) << SKIP_SHIFT;
    if (bypass) {
        kmiDeviceExtension.background_skip |= SKIP_CACHE_BYPASS;
    }
    if (shadow) {
        kmiDeviceExtension.background_skip |= SKIP_SHADOW;
    }
}

/* the words a vertex holds after x, y and z under isp */
static KMINT32 payload_words(KMUINT32 isp)
{
    KMINT32 words = BASE_COLOR_WORDS;
    if (isp & ISP_TEXTURE) {
        words++;
        if (isp & ISP_OFFSET) {
            words++;
        }
        if (!(isp & ISP_UV16)) {
            words++;
        }
    }
    return words;
}

static KMINT32 put_offset(KMUINT32 isp, KMINT32 next, KMUINT32 color)
{
    if (isp & ISP_OFFSET) {
        BACKGROUND_PACKET[next++] = color;
    }
    return next;
}

/* a packed UV word goes in whole under ISP_UV16. otherwise U and V each go in
 * as the high half of a float */
static KMINT32 put_uv16(KMUINT32 isp, KMINT32 next, KMUINT32 uv)
{
    if (isp & ISP_TEXTURE) {
        if (isp & ISP_UV16) {
            BACKGROUND_PACKET[next++] = uv;
        } else {
            BACKGROUND_PACKET[next++] = uv & 0xffff0000;
            BACKGROUND_PACKET[next++] = (KMUINT32)(unsigned short)uv << 16;
        }
    }
    return next;
}

static KMINT32 put_uv32(KMUINT32 isp, KMINT32 next, KMUINT32 u, KMUINT32 v)
{
    if (isp & ISP_TEXTURE) {
        BACKGROUND_PACKET[next++] = u;
        BACKGROUND_PACKET[next++] = v;
    }
    return next;
}

/* the packet Kamui starts with: an untextured triangle whose corners are
 * (2, 260), (150, 2) and (560, 260) */
KMSTATUS kmiInitializeBackGroundPlane(void)
{
    BACKGROUND_PACKET[0] = DEFAULT_ISP;
    BACKGROUND_PACKET[1] = DEFAULT_TSP;
    BACKGROUND_PACKET[2] = 0;
    BACKGROUND_PACKET[3] = 0x40000000;
    BACKGROUND_PACKET[4] = 0x43820000;
    BACKGROUND_PACKET[5] = DEFAULT_DEPTH;
    BACKGROUND_PACKET[6] = BLACK;
    BACKGROUND_PACKET[7] = 0x43160000;
    BACKGROUND_PACKET[8] = 0x40000000;
    BACKGROUND_PACKET[9] = DEFAULT_DEPTH;
    BACKGROUND_PACKET[10] = BLACK;
    BACKGROUND_PACKET[11] = 0x440c0000;
    BACKGROUND_PACKET[12] = 0x43820000;
    BACKGROUND_PACKET[13] = DEFAULT_DEPTH;
    BACKGROUND_PACKET[14] = BLACK;
    kmiDeviceExtension.background_depth = BACKGROUND_PACKET[5];
    kmiDeviceExtension.background_skip = BASE_COLOR_WORDS << SKIP_SHIFT;
    return KMSTATUS_SUCCESS;
}

#define BEGIN_TRIANGLE(type)                                                   \
    const type *corner[3];                                                     \
    KMUINT32 isp;                                                              \
    KMUINT32 tsp;                                                              \
    KMUINT32 texture;                                                          \
    KMUINT32 next = 0;                                                         \
    corner[0] = vertices[0];                                                   \
    corner[1] = vertices[1];                                                   \
    corner[2] = vertices[2];                                                   \
    isp = kmiDeviceExtension.background_isp;                                   \
    tsp = kmiDeviceExtension.background_tsp;                                   \
    texture = kmiDeviceExtension.background_texture;                           \
    BACKGROUND_PACKET[next++] = isp;                                           \
    BACKGROUND_PACKET[next++] = tsp;                                           \
    BACKGROUND_PACKET[next++] = texture
#define END_TRIANGLE()                                                         \
    kmiDeviceExtension.background_size = next * sizeof BACKGROUND_PACKET[0];   \
    set_skip(payload_words(isp), 0, 0);                                        \
    kmiDeviceExtension.background_depth = FLOAT_BITS(corner[0]->fZ)

#define PUT_POSITION(i)                                                        \
    BACKGROUND_PACKET[next++] = FLOAT_BITS(corner[i]->fX);                     \
    BACKGROUND_PACKET[next++] = FLOAT_BITS(corner[i]->fY);                     \
    BACKGROUND_PACKET[next++] = FLOAT_BITS(corner[i]->fZ)
#define PUT_UV32(i)                                                            \
    next = put_uv32(isp, next, FLOAT_BITS(corner[i]->fU),                      \
                    FLOAT_BITS(corner[i]->fV))
#define PUT_UV32_0M(i)                                                         \
    next = put_uv32(isp, next, FLOAT_BITS(corner[i]->fU0m),                    \
                    FLOAT_BITS(corner[i]->fV0m))
#define PUT_UV16(i) next = put_uv16(isp, next, corner[i]->dwUV)
#define PUT_UV16_0M(i) next = put_uv16(isp, next, corner[i]->dwUV0m)
#define PUT_BASE(value) BACKGROUND_PACKET[next++] = (value)
#define PUT_OFFSET(value) next = put_offset(isp, next, (value))
/* a color of floats from 0 to 1, as a packed word */
#define ARGB(a, r, g, b)                                                       \
    ((KMUINT32)(KMINT32)((a) * 255.0F) << 24 | (KMINT32)((r) * 255.0F) << 16 | \
     (KMINT32)((g) * 255.0F) << 8 | (KMINT32)((b) * 255.0F))
#define INTENSITY(v) ARGB((v), (v), (v), (v))
#define BASE_ARGB(i)                                                           \
    ARGB(corner[i]->fBaseAlpha, corner[i]->fBaseRed, corner[i]->fBaseGreen,    \
         corner[i]->fBaseBlue)
#define OFFSET_ARGB(i)                                                         \
    ARGB(corner[i]->fOffsetAlpha, corner[i]->fOffsetRed,                       \
         corner[i]->fOffsetGreen, corner[i]->fOffsetBlue)

/* the library exports each writer, so none is static */
/* @hack type: each writer returns KMINT32, where KMSTATUS, the type of the
 * statuses it returns, would do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
KMINT32 kmiProcessVertex_00(void *vertices[3]);
KMINT32 kmiProcessVertex_01(void *vertices[3]);
KMINT32 kmiProcessVertex_02(void *vertices[3]);
KMINT32 kmiProcessVertex_03(void *vertices[3]);
KMINT32 kmiProcessVertex_04(void *vertices[3]);
KMINT32 kmiProcessVertex_05(void *vertices[3]);
KMINT32 kmiProcessVertex_06(void *vertices[3]);
KMINT32 kmiProcessVertex_07(void *vertices[3]);
KMINT32 kmiProcessVertex_08(void *vertices[3]);
KMINT32 kmiProcessVertex_09(void *vertices[3]);
KMINT32 kmiProcessVertex_10(void *vertices[3]);
KMINT32 kmiProcessVertex_11(void *vertices[3]);
KMINT32 kmiProcessVertex_12(void *vertices[3]);
KMINT32 kmiProcessVertex_13(void *vertices[3]);
KMINT32 kmiProcessVertex_14(void *vertices[3]);
KMINT32 kmiProcessVertex_15(void *vertices[3]);
KMINT32 kmiProcessVertex_16(void *vertices[3]);
KMINT32 kmiProcessVertex_17(void *vertices[3]);
/* NOLINTEND(misc-use-internal-linkage) */

KMINT32 kmiProcessVertex_00(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX0);
    PUT_POSITION(0);
    PUT_BASE(corner[0]->dwBaseRGB);
    PUT_POSITION(1);
    PUT_BASE(corner[1]->dwBaseRGB);
    PUT_POSITION(2);
    PUT_BASE(corner[2]->dwBaseRGB);
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_01(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX1);
    PUT_POSITION(0);
    PUT_BASE(BASE_ARGB(0));
    PUT_POSITION(1);
    PUT_BASE(BASE_ARGB(1));
    PUT_POSITION(2);
    PUT_BASE(BASE_ARGB(2));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_02(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX2);
    PUT_POSITION(0);
    PUT_BASE(INTENSITY(corner[0]->fBaseIntensity));
    PUT_POSITION(1);
    PUT_BASE(INTENSITY(corner[1]->fBaseIntensity));
    PUT_POSITION(2);
    PUT_BASE(INTENSITY(corner[2]->fBaseIntensity));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_03(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX3);
    PUT_POSITION(0);
    PUT_UV32(0);
    PUT_BASE(corner[0]->uBaseRGB.dwPacked);
    PUT_OFFSET(corner[0]->uOffsetRGB.dwPacked);
    PUT_POSITION(1);
    PUT_UV32(1);
    PUT_BASE(corner[1]->uBaseRGB.dwPacked);
    PUT_OFFSET(corner[1]->uOffsetRGB.dwPacked);
    PUT_POSITION(2);
    PUT_UV32(2);
    PUT_BASE(corner[2]->uBaseRGB.dwPacked);
    PUT_OFFSET(corner[2]->uOffsetRGB.dwPacked);
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_04(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX4);
    PUT_POSITION(0);
    PUT_UV16(0);
    PUT_BASE(corner[0]->uBaseRGB.dwPacked);
    PUT_OFFSET(corner[0]->uOffsetRGB.dwPacked);
    PUT_POSITION(1);
    PUT_UV16(1);
    PUT_BASE(corner[1]->uBaseRGB.dwPacked);
    /* @bug logic: the second and third corners take the first corner's
     * offset color, not their own (the loads at 0x8c149d18 and 0x8c149d64) */
    PUT_OFFSET(corner[0]->uOffsetRGB.dwPacked);
    PUT_POSITION(2);
    PUT_UV16(2);
    PUT_BASE(corner[2]->uBaseRGB.dwPacked);
    PUT_OFFSET(corner[0]->uOffsetRGB.dwPacked);
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_05(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX5);
    PUT_POSITION(0);
    PUT_UV32(0);
    PUT_BASE(BASE_ARGB(0));
    PUT_OFFSET(OFFSET_ARGB(0));
    PUT_POSITION(1);
    PUT_UV32(1);
    PUT_BASE(BASE_ARGB(1));
    PUT_OFFSET(OFFSET_ARGB(1));
    PUT_POSITION(2);
    PUT_UV32(2);
    PUT_BASE(BASE_ARGB(2));
    PUT_OFFSET(OFFSET_ARGB(2));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_06(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX6);
    PUT_POSITION(0);
    PUT_UV16(0);
    PUT_BASE(BASE_ARGB(0));
    PUT_OFFSET(OFFSET_ARGB(0));
    PUT_POSITION(1);
    /* @bug logic: the second and third corners take the first corner's UVs,
     * not their own (the loads at 0x8c14a1f6 and 0x8c14a2b2) */
    PUT_UV16(0);
    PUT_BASE(BASE_ARGB(1));
    PUT_OFFSET(OFFSET_ARGB(1));
    PUT_POSITION(2);
    PUT_UV16(0);
    PUT_BASE(BASE_ARGB(2));
    PUT_OFFSET(OFFSET_ARGB(2));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_07(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX7);
    PUT_POSITION(0);
    PUT_UV32(0);
    PUT_BASE(INTENSITY(corner[0]->fBaseIntensity));
    PUT_OFFSET(INTENSITY(corner[0]->fOffsetIntensity));
    /* @bug logic: the first corner's offset intensity goes in a second time,
     * whether or not isp has ISP_OFFSET, so that corner holds a word more than
     * background_skip counts (the store at 0x8c14a4ac) */
    PUT_BASE(INTENSITY(corner[0]->fOffsetIntensity));
    PUT_POSITION(1);
    PUT_UV32(1);
    PUT_BASE(INTENSITY(corner[1]->fBaseIntensity));
    PUT_OFFSET(INTENSITY(corner[1]->fOffsetIntensity));
    PUT_POSITION(2);
    PUT_UV32(2);
    PUT_BASE(INTENSITY(corner[2]->fBaseIntensity));
    PUT_OFFSET(INTENSITY(corner[2]->fOffsetIntensity));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_08(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX8);
    PUT_POSITION(0);
    PUT_UV16(0);
    PUT_BASE(INTENSITY(corner[0]->fBaseIntensity));
    PUT_OFFSET(INTENSITY(corner[0]->fOffsetIntensity));
    PUT_POSITION(1);
    /* @bug logic: the second and third corners take the first corner's UVs,
     * not their own (the loads at 0x8c14a744 and 0x8c14a7ce) */
    PUT_UV16(0);
    PUT_BASE(INTENSITY(corner[1]->fBaseIntensity));
    PUT_OFFSET(INTENSITY(corner[1]->fOffsetIntensity));
    PUT_POSITION(2);
    PUT_UV16(0);
    PUT_BASE(INTENSITY(corner[2]->fBaseIntensity));
    PUT_OFFSET(INTENSITY(corner[2]->fOffsetIntensity));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_09(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX9);
    PUT_POSITION(0);
    PUT_BASE(corner[0]->uBaseRGB0m.dwPacked);
    PUT_POSITION(1);
    PUT_BASE(corner[1]->uBaseRGB0m.dwPacked);
    PUT_POSITION(2);
    PUT_BASE(corner[2]->uBaseRGB0m.dwPacked);
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_10(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX10);
    PUT_POSITION(0);
    PUT_BASE(INTENSITY(corner[0]->fBaseIntensity0m));
    PUT_POSITION(1);
    PUT_BASE(INTENSITY(corner[1]->fBaseIntensity0m));
    PUT_POSITION(2);
    PUT_BASE(INTENSITY(corner[2]->fBaseIntensity0m));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_11(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX11);
    PUT_POSITION(0);
    PUT_UV32_0M(0);
    PUT_BASE(corner[0]->uBaseRGB0m.dwPacked);
    PUT_OFFSET(corner[0]->uOffsetRGB0m.dwPacked);
    PUT_POSITION(1);
    PUT_UV32_0M(1);
    PUT_BASE(corner[1]->uBaseRGB0m.dwPacked);
    PUT_OFFSET(corner[1]->uOffsetRGB0m.dwPacked);
    PUT_POSITION(2);
    PUT_UV32_0M(2);
    PUT_BASE(corner[2]->uBaseRGB0m.dwPacked);
    PUT_OFFSET(corner[2]->uOffsetRGB0m.dwPacked);
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_12(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX12);
    PUT_POSITION(0);
    PUT_UV16_0M(0);
    PUT_BASE(corner[0]->uBaseRGB0m.dwPacked);
    PUT_OFFSET(corner[0]->uOffsetRGB0m.dwPacked);
    PUT_POSITION(1);
    /* @bug logic: the second corner takes the first corner's UVs, not its
     * own (the load at 0x8c14adc2) */
    PUT_UV16_0M(0);
    PUT_BASE(corner[1]->uBaseRGB0m.dwPacked);
    PUT_OFFSET(corner[1]->uOffsetRGB0m.dwPacked);
    PUT_POSITION(2);
    PUT_UV16_0M(2);
    PUT_BASE(corner[2]->uBaseRGB0m.dwPacked);
    PUT_OFFSET(corner[2]->uOffsetRGB0m.dwPacked);
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_13(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX13);
    PUT_POSITION(0);
    PUT_UV32_0M(0);
    PUT_BASE(INTENSITY(corner[0]->fBaseIntensity0m));
    PUT_OFFSET(INTENSITY(corner[0]->fOffsetIntensity0m));
    PUT_POSITION(1);
    PUT_UV32_0M(1);
    PUT_BASE(INTENSITY(corner[1]->fBaseIntensity0m));
    PUT_OFFSET(INTENSITY(corner[1]->fOffsetIntensity0m));
    PUT_POSITION(2);
    PUT_UV32_0M(2);
    PUT_BASE(INTENSITY(corner[2]->fBaseIntensity0m));
    PUT_OFFSET(INTENSITY(corner[2]->fOffsetIntensity0m));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_14(void *vertices[3])
{
    BEGIN_TRIANGLE(KMVERTEX14);
    PUT_POSITION(0);
    PUT_UV16_0M(0);
    PUT_BASE(INTENSITY(corner[0]->fBaseIntensity0m));
    PUT_OFFSET(INTENSITY(corner[0]->fOffsetIntensity0m));
    PUT_POSITION(1);
    PUT_UV16_0M(1);
    PUT_BASE(INTENSITY(corner[1]->fBaseIntensity0m));
    PUT_OFFSET(INTENSITY(corner[1]->fOffsetIntensity0m));
    PUT_POSITION(2);
    PUT_UV16_0M(2);
    PUT_BASE(INTENSITY(corner[2]->fBaseIntensity0m));
    PUT_OFFSET(INTENSITY(corner[2]->fOffsetIntensity0m));
    END_TRIANGLE();
    return KMSTATUS_SUCCESS;
}

KMINT32 kmiProcessVertex_15(void *vertices[3])
{
    return KMSTATUS_INVALID_VERTEX_TYPE;
}

KMINT32 kmiProcessVertex_16(void *vertices[3])
{
    return KMSTATUS_INVALID_VERTEX_TYPE;
}

KMINT32 kmiProcessVertex_17(void *vertices[3])
{
    return KMSTATUS_INVALID_VERTEX_TYPE;
}

/* each vertex format's writer. formats 15 to 17, a sprite's and a modifier
 * volume's, make no plane */
KMI_BACKGROUND_WRITER kmiBackGroundPlaneFuncs[] = {
    kmiProcessVertex_00, kmiProcessVertex_01, kmiProcessVertex_02,
    kmiProcessVertex_03, kmiProcessVertex_04, kmiProcessVertex_05,
    kmiProcessVertex_06, kmiProcessVertex_07, kmiProcessVertex_08,
    kmiProcessVertex_09, kmiProcessVertex_10, kmiProcessVertex_11,
    kmiProcessVertex_12, kmiProcessVertex_13, kmiProcessVertex_14,
    kmiProcessVertex_15, kmiProcessVertex_16, kmiProcessVertex_17};
