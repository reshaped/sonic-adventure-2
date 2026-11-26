/* the easy multi drawer's light calls, which set the lights nj_cnk.h lays
 * out */
#include <ninja.h>
#include <nj_light_state.h>
#include <sg_xpt.h>

#ifdef NONMATCHING
/* this side reads the mode word by its own name, the plain form the hack tag
 * below gives. GCC needn't place _nj_cnk_easy_multilight_buffer_mdst_ right
 * after the buffer, so a read past the buffer's end needn't reach it */
#define LIGHT_MODE (&_nj_cnk_easy_multilight_buffer_mdst_.modes)
#else
/* @hack spelling: the mode word read past _nj_cnk_easy_multilight_buffer_'s
 * end, where _nj_cnk_easy_multilight_buffer_mdst_'s first word would do */
#define LIGHT_MODE                                                             \
    ((Uint32 *)(_nj_cnk_easy_multilight_buffer_ + MULTILIGHT_FLOATS))
#endif
/* light 1's bits in the mode word, which nj_cnk.h describes. light n's lie
 * LIGHT_SHIFT(n) bits up */
#define LIGHT_ON 1
#define LIGHT_POINT 2
#define LIGHT_LAST 4
#define LIGHT_BITS 3
#define LIGHT_SHIFT(n) (LIGHT_BITS * ((n) - 1))
/* light n's vector or point is the VECTOR_FLOATS floats from
 * _nj_cnk_easy_multilight_buffer_ + VECTOR_FLOATS(n - 1) */
#define VECTOR_FLOATS 3
/* light n is the LIGHT_FLOATS floats from
 * _nj_cnk_easy_multilight_buffer_dst_ + LIGHT_FLOATS(n - 1). its float
 * LIGHT_RANGE is its far range's square, and the next its near range's. its
 * color's r, g and b start at its float LIGHT_COLOR */
#define LIGHT_FLOATS 8
#define LIGHT_RANGE 3
#define LIGHT_COLOR 5
/* VECTOR_LIGHT turns light 1 on with its point bit clear, so that it shines
 * along its vector */
#define VECTOR_LIGHT LIGHT_ON
#define POINT_LIGHT(n) ((Uint32)(LIGHT_ON | LIGHT_POINT) << LIGHT_SHIFT(n))
#define LAST_LIGHT(n) ((Uint32)LIGHT_LAST << LIGHT_SHIFT(n))
#define LIGHT_SWITCH(n) ((Uint32)LIGHT_ON << LIGHT_SHIFT(n))

void njCnkSetEasyMultiLight(Int num)
{
    Uint32 modes[] = {
        LAST_LIGHT(1),
        VECTOR_LIGHT | LAST_LIGHT(1),
        VECTOR_LIGHT | POINT_LIGHT(2) | LAST_LIGHT(2),
        VECTOR_LIGHT | POINT_LIGHT(2) | POINT_LIGHT(3) | LAST_LIGHT(3),
        VECTOR_LIGHT | POINT_LIGHT(2) | POINT_LIGHT(3) | POINT_LIGHT(4) |
            LAST_LIGHT(4),
        VECTOR_LIGHT | POINT_LIGHT(2) | POINT_LIGHT(3) | POINT_LIGHT(4) |
            POINT_LIGHT(5) | LAST_LIGHT(5),
        VECTOR_LIGHT | POINT_LIGHT(2) | POINT_LIGHT(3) | POINT_LIGHT(4) |
            POINT_LIGHT(5) | POINT_LIGHT(6) | LAST_LIGHT(6),
    };
    /* @hack spelling: the mode word stored through mode, where a store
     * through LIGHT_MODE would do */
    Uint32 *mode = LIGHT_MODE;

    *mode = modes[num];
}

void njCnkSetEasyMultiLightSwitch(Int light, Int flag)
{
    Uint32 bits[] = {
        LIGHT_SWITCH(1) | LIGHT_SWITCH(2) | LIGHT_SWITCH(3) | LIGHT_SWITCH(4) |
            LIGHT_SWITCH(5) | LIGHT_SWITCH(6),
        LIGHT_SWITCH(1),
        LIGHT_SWITCH(2),
        LIGHT_SWITCH(3),
        LIGHT_SWITCH(4),
        LIGHT_SWITCH(5),
        LIGHT_SWITCH(6),
    };

    if (flag) {
        *LIGHT_MODE |= bits[light];
    } else {
        *LIGHT_MODE &= ~bits[light];
    }
}

void njCnkSetEasyMultiAmbient(Float ar, Float ag, Float ab)
{
    /* @hack spelling: the color stored as ambient steps along it, where
     * indexed stores would do */
#ifdef NONMATCHING
    /* this side stores the color by its own name, the plain form the hack tag
     * above gives. GCC needn't place _nj_cnk_easy_multilight_buffer_mdst_
     * right after the buffer, so an index past the buffer's end needn't reach
     * it */
    Float *ambient = _nj_cnk_easy_multilight_buffer_mdst_.ambient;
#else
    Float *ambient = &_nj_cnk_easy_multilight_buffer_[MULTILIGHT_AMBIENT];
#endif

    *ambient++ = ar;
    *ambient++ = ag;
    *ambient = ab;
}

void njCnkSetEasyMultiLightColor(Int light, Float lr, Float lg, Float lb)
{
    /* @hack spelling: the light's offset added to color after, and its
     * floats stored as color steps along them, where indexed stores would
     * do */
    Float *color = &_nj_cnk_easy_multilight_buffer_dst_[LIGHT_COLOR];

    color += (light - 1) * LIGHT_FLOATS;
    *color++ = lr;
    *color++ = lg;
    *color = lb;
}

void njCnkSetEasyMultiLightVectorEx(Int light, Float vx, Float vy, Float vz)
{
    Float *v;
    Uint32 *mode;
    Uint32 bit;

    /* @hack spelling: v set to the buffer before light steps down, where
     * &_nj_cnk_easy_multilight_buffer_[(light - 1) * VECTOR_FLOATS] would
     * do */
    v = _nj_cnk_easy_multilight_buffer_;
    light--;
    v += light * VECTOR_FLOATS;
    *v++ = vx;
    *v++ = vy;
    *v = vz;
    /* @hack spelling: the point bit cleared a step at a time, where
     * *LIGHT_MODE &= ~((Uint32)LIGHT_POINT << LIGHT_SHIFT(light)) would do */
    bit = LIGHT_POINT;
    bit <<= light * LIGHT_BITS;
    bit = ~bit;
    mode = LIGHT_MODE;
    bit &= *mode;
    *mode = bit;
}

void njCnkSetEasyMultiLightVector(Float vx, Float vy, Float vz)
{
    njCnkSetEasyMultiLightVectorEx(1, vx, vy, vz);
}

void njCnkSetEasyMultiLightPoint(Int light, Float px, Float py, Float pz)
{
    Float *p;
    Uint32 *mode;
    Uint32 bit;

    /* @hack spelling: p set to the buffer before light steps down, where
     * &_nj_cnk_easy_multilight_buffer_[(light - 1) * VECTOR_FLOATS] would
     * do */
    p = _nj_cnk_easy_multilight_buffer_;
    light--;
    p += light * VECTOR_FLOATS;
    *p++ = px;
    *p++ = py;
    *p = pz;
    /* @hack spelling: the point bit set a step at a time, where
     * *LIGHT_MODE |= (Uint32)LIGHT_POINT << LIGHT_SHIFT(light) would do */
    bit = LIGHT_POINT;
    bit <<= light * LIGHT_BITS;
    mode = LIGHT_MODE;
    bit |= *mode;
    *mode = bit;
}

void njCnkSetEasyMultiLightRange(Int light, Float nrange, Float frange)
{
    /* @hack spelling: the light's offset added to range after, and the
     * squares stored as range steps along them, where indexed stores would
     * do */
    Float *range = &_nj_cnk_easy_multilight_buffer_dst_[LIGHT_RANGE];

    frange *= frange;
    nrange *= nrange;
    range += (light - 1) * LIGHT_FLOATS;
    *range++ = frange;
    *range = nrange;
}

void njCnkSetEasyMultiLightMatrices(void)
{
    Float *out = _nj_cnk_easy_multilight_buffer_dst_;
    Float *in = _nj_cnk_easy_multilight_buffer_;
    Uint32 mode = *LIGHT_MODE;

    /* @hack spelling: mode shifted a bit at a time, each bit tested at bit
     * 0, where tests of LIGHT_POINT and LIGHT_LAST and one shift by
     * LIGHT_BITS would do */
    for (;;) {
        if (mode & LIGHT_ON) {
            mode >>= 1;
            if (mode & 1) {
                njCalcPoint(NULL, (NJS_POINT3 *)in, (NJS_POINT3 *)out);
            } else {
                njCalcVector(NULL, (NJS_VECTOR *)in, (NJS_VECTOR *)out);
            }
        } else {
            mode >>= 1;
        }
        mode >>= 1;
        if (mode & 1) {
            break;
        }
        mode >>= 1;
        in += VECTOR_FLOATS;
        out += LIGHT_FLOATS;
    }
}
