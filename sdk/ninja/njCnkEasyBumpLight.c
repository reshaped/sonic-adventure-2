/* the easy drawer's bump light: a bump-mapped vertex's offset color, and its
 * base color */
#include <nj_cnk.h>
#include <nj_global.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports this function, though no header of the SDK declares
 * it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set _nj_cnk_easy_bump_lt_color_ to an opaque color. its red byte is 255
 * times r, held to 0..255, its fraction dropped. its green and blue bytes
 * take g and b alike */
void njCnkSetEasyBumpLightColor(Float r, Float g, Float b);
/* NOLINTEND(misc-use-internal-linkage) */

#define SIGN_BIT 0x80000000
#define MAGNITUDE 0x7fffffff
/* the bits of the float 2^-95. where v's x and z both lie below it in
 * magnitude, v's heading comes from their signs alone */
#define TINY_BITS 0x10000000
/* a y just above the xz plane. it stands in for v's y where that y's sign
 * bit is set */
#define SMALL_Y 1e-5F
/* (ATAN_A - ATAN_B * a) * a approximates the angle whose tangent is a, for an
 * a from 0 to 1. the angle is in 256ths of a turn */
#define ATAN_A 43.562813F
#define ATAN_B 11.562812F
#define QUARTER_TURN 64.0F
#define BYTE_MAX 255.0F

/* OCTANT_Z_MAJOR is set in an octant's index where |z| is above |x|.
 * OCTANT_X_NEGATIVE is set where x's sign bit is, and OCTANT_Z_NEGATIVE where
 * z's is. X_SIGN_SHIFT and Z_SIGN_SHIFT take a sign bit down to those two */
#define OCTANT_Z_MAJOR 1
#define OCTANT_X_NEGATIVE 2
#define OCTANT_Z_NEGATIVE 4
#define X_SIGN_SHIFT 30
#define Z_SIGN_SHIFT 29
/* where each octant's heading counts from, in 256ths of a turn */
static const Float octant_base[8] = {0.0F,   64.0F,   128.0F,  -64.0F,
                                     256.0F, -192.0F, -128.0F, 192.0F};

/* the offset color's heading byte is its lowest, and its cosine byte lies
 * COSINE_SHIFT bits up. TOP_BYTE sets its top byte to 0x80 */
#define BYTE_MASK 0xff
#define COSINE_SHIFT 8
#define TOP_BYTE (0x80U << 24)
/* a packed ARGB word's alpha byte at 255, and where its red and green bytes
 * lie */
#define OPAQUE 0xff000000
#define RED_SHIFT 16
#define GREEN_SHIFT 8

/* the bits of v's float i */
#define BITS(i) (((Sint32 *)v)[i])
/* @hack spelling: v's components read as ((Float *)v)[i], where v->x, v->y
 * and v->z would do */
#define COMPONENT(i) (((Float *)v)[i])

Uint32 njCnkEasyBumpLight(NJS_VECTOR *v)
{
    /* @hack redundant: register on major, a, y, xz_squared, length_squared
     * and cosine_byte */
    register Float major;
    register Float a;
    register Float y;
    register Float xz_squared;
    register Float length_squared;
    register Float cosine_byte;
    Float angle;
    /* @hack redundant: register on octant */
    register Sint32 octant;
    Uint32 color;

    color = 0;
    octant = 0;
    xz_squared = (COMPONENT(2) * COMPONENT(2)) + (COMPONENT(0) * COMPONENT(0));
    if (((BITS(2) | BITS(0)) & MAGNITUDE) < TINY_BITS) {
        major = 1.0F;
        a = 0.0F;
    } else if ((BITS(0) & MAGNITUDE) >= (BITS(2) & MAGNITUDE)) {
        major = COMPONENT(0);
        a = COMPONENT(2);
    } else {
        a = COMPONENT(0);
        major = COMPONENT(2);
        octant = OCTANT_Z_MAJOR;
    }
    if (BITS(1) & SIGN_BIT) {
        y = SMALL_Y;
    } else {
        y = COMPONENT(1);
    }
    length_squared = xz_squared + (y * y);
    a = _builtin_fabsf(a / major);
    octant |= ((BITS(2) >> Z_SIGN_SHIFT) & OCTANT_Z_NEGATIVE) |
              ((BITS(0) >> X_SIGN_SHIFT) & OCTANT_X_NEGATIVE);
    angle = (ATAN_A - (a * ATAN_B)) * a;
    a = _builtin_fabsf(angle - octant_base[octant]);
    cosine_byte = _builtin_sqrtf(xz_squared / length_squared) * BYTE_MAX;
    a -= QUARTER_TURN;
    if (a < 0.0F) {
        /* @bug logic: adds 255 to a heading below 0, where a turn is 256. a
         * heading from 192 to 255 comes out one less (FADD FR7,FR5 at
         * 0x8c15d738) */
        a += BYTE_MAX;
    }
    if (a > BYTE_MAX) {
        color |= BYTE_MASK;
    } else {
        /* @hack redundant: & BYTE_MASK on a byte that the Uint8 cast already
         * holds */
        color |= (Uint8)(Sint32)a & BYTE_MASK;
    }
    if (cosine_byte > BYTE_MAX) {
        color |= BYTE_MASK << COSINE_SHIFT;
    } else {
        /* @hack redundant: & BYTE_MASK on a byte that the Uint8 cast already
         * holds */
        color |= ((Uint8)(Sint32)cosine_byte & BYTE_MASK) << COSINE_SHIFT;
    }
    color |= TOP_BYTE;
    return color;
}

void njCnkSetEasyBumpLightColor(Float r, Float g, Float b)
{
    r *= BYTE_MAX;
    g *= BYTE_MAX;
    b *= BYTE_MAX;
    if (r > BYTE_MAX) {
        r = BYTE_MAX;
    } else if (r < 0.0F) {
        r = 0.0F;
    }
    if (g > BYTE_MAX) {
        g = BYTE_MAX;
    } else if (g < 0.0F) {
        g = 0.0F;
    }
    if (b > BYTE_MAX) {
        b = BYTE_MAX;
    } else if (b < 0.0F) {
        b = 0.0F;
    }
    _nj_cnk_easy_bump_lt_color_ = OPAQUE | ((Uint8)(Sint32)r << RED_SHIFT) |
                                  ((Uint8)(Sint32)g << GREEN_SHIFT) |
                                  (Uint8)(Sint32)b;
}
