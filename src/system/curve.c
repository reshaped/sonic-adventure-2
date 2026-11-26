/* curves through keyed records: a record of any layout, blended field by
 * field from the four keys around a place on a closed curve */
#include <mathf.h>
#include <sg_xpt.h>

#include "system/curve.h"

/* CURVE_THROUGH and CURVE_B_SPLINE */
#define CURVE_KINDS 2
/* the steps curve_weights holds from one key to the next */
#define STEPS 100
/* the keys a blend takes */
#define BLEND_KEYS 4

/* the weights of four keys in a row, by kind of curve and by step. the steps
 * run from the second key to the third. curve_evaluate reads a kind's first
 * STEPS rows. curve_weights_init's float steps also fill row STEPS, and
 * nothing writes the last row */
static Float curve_weights[CURVE_KINDS][STEPS + 2][BLEND_KEYS];

/* the row for t, from 0 up to 1. curve_weights_init's float steps leave
 * some t just short of their row, and the 0.01 lifts them into it */
#define STEP(t) ((int)(((t) * STEPS) + 0.01F))
/* each kind's row of weights at t */
#define THROUGH(t) curve_weights[CURVE_THROUGH][STEP(t)]
#define B_SPLINE(t) curve_weights[CURVE_B_SPLINE][STEP(t)]
#define SIXTH (1.0F / 6.0F)

/* field i of key n */
#define KEY(type, n)                                                           \
    (*(type *)((char *)curve->keys + ((n) * size) + offsets[i]))
/* field i, blended from the keys a, b, c and d */
#define BLEND(type, a, b, c, d)                                                \
    ((weights[0] * KEY(type, a)) + (weights[1] * KEY(type, b)) +               \
     (weights[2] * KEY(type, c)) + (weights[3] * KEY(type, d)))
/* set field i of out from the four keys around key. they are key itself, the
 * one before it and the two after. a key past an end wraps to the other end */
#define SET_FIELD(type)                                                        \
    if (key == 0) {                                                            \
        *(type *)(out + offsets[i]) = BLEND(type, curve->count - 1, 0, 1, 2);  \
    } else if (key == curve->count - 2) {                                      \
        *(type *)(out + offsets[i]) = BLEND(type, key - 1, key, key + 1, 0);   \
    } else if (key == curve->count - 1) {                                      \
        *(type *)(out + offsets[i]) = BLEND(type, key - 1, key, 0, 1);         \
    } else {                                                                   \
        *(type *)(out + offsets[i]) =                                          \
            BLEND(type, key - 1, key, key + 1, key + 2);                       \
    }

/* set offsets to the offset of each field format spells. the entry after
 * the last takes the record's size. it returns the number of fields */
static int curve_layout(char *format, int *offsets)
{
    int i;
    int offset;
    int align;

    for (i = 0, offset = 0, align = 1; *format; format++, i++) {
        switch (*format) {
        case 'c':
        case 'C':
            offsets[i] = offset;
            offset += 1;
            break;
        case 's':
        case 'S':
            offset += offset % 2;
            offsets[i] = offset;
            offset += 2;
            if (align < 2) {
                align = 2;
            }
            break;
        case 'l':
        case 'L':
        case 'I':
        case 'i':
        case 'F':
        case 'f':
        case 'A':
        case 'a':
            if (offset % 4) {
                offset += 4 - (offset % 4);
            }
            offsets[i] = offset;
            offset += 4;
            if (align < 4) {
                align = 4;
            }
            break;
        default:
            /* stop the program: the misaligned store faults */
#ifdef NONMATCHING
            /* GCC writes a store to address 1 as byte stores, which don't
             * fault, so its build spins instead */
            for (;;) {
            }
#else
            *(int *)1 = 0;
#endif
            break;
        }
    }
    if (offset % align) {
        offset = ((offset / align) + 1) * align;
    }
    offsets[i] = offset;
    return i;
}

void curve_weights_init(void)
{
    /* the way from one key to the next, from 0 up to 1 */
    Float t; /* NOLINT(readability-identifier-length) */
    Float up[4]; /* up[n] is t to the n */
    Float down[4]; /* down[n] is (1 - t) to the n */

    /* @hack spelling: each store indexes the table again, where a pointer
     * to the row, set once a step, would do */
    /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
    for (t = 0.0F; t < 1.0F; t += 1.0F / STEPS) {
        THROUGH(t)[0] = -((t - 1.0F) * (t - 2.0F) * t * SIXTH);
        THROUGH(t)[1] = (t + 1.0F) * (t - 1.0F) * (t - 2.0F) * 0.5F;
        THROUGH(t)[2] = -(t + 1.0F) * (t - 2.0F) * t * 0.5F;
        THROUGH(t)[3] = (t + 1.0F) * (t - 1.0F) * t * SIXTH;
    }
    /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
    for (t = 0.0F; t < 1.0F; t += 1.0F / STEPS) {
        up[2] = t * t;
        up[3] = up[2] * t;
        down[1] = 1.0F - t;
        down[2] = (1.0F - t) * (1.0F - t);
        down[3] = down[1] * down[2];
        B_SPLINE(t)[0] = down[3] * SIXTH;
        B_SPLINE(t)[1] = (up[3] * 0.5F) - up[2] + (2.0F / 3.0F);
        B_SPLINE(t)[2] = (up[2] * 0.5F) - (up[3] * 0.5F) + (t * 0.5F) + SIXTH;
        B_SPLINE(t)[3] = up[3] * SIXTH;
    }
}

void curve_evaluate(struct curve *curve, Float place)
{
    /* @hack redundant: the locals spare and unused, and every local in this
     * order, which sets its slot on the stack */
    int offsets[CURVE_FIELDS + 1];
    int size;
    int key;
    int i;
    Float *weights;
    int spare;
    int unused; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    int count;
    char *format;
    char *out;

    unused = 0;
    count = curve_layout(curve->format, offsets);
    size = offsets[count];
    key = (int)floorf(place);
    weights = curve_weights[curve->kind][(int)((place - key) * STEPS)];
    format = curve->format;
    out = curve->out;
    for (i = 0; i < count; format++, i++) {
        switch (*format) {
        case 'c':
            SET_FIELD(Uint8)
            break;
        case 'C':
            SET_FIELD(Sint8)
            break;
        case 's':
            SET_FIELD(Uint16)
            break;
        case 'S':
            SET_FIELD(Sint16)
            break;
        case 'l':
            SET_FIELD(Uint32)
            break;
        /* @hack spelling: each of these letters jumps to the code of the
         * one it shares, where case labels on one statement would do */
        case 'L':
            goto long_field;
        case 'A':
            goto long_field;
        case 'a':
        long_field:
            SET_FIELD(Sint32)
            break;
        case 'I':
            goto int_field;
        case 'i':
            goto int_field;
        int_field:
            SET_FIELD(Int)
            break;
        case 'F':
            goto float_field;
        case 'f':
            goto float_field;
        float_field:
            SET_FIELD(Float)
            break;
        }
    }
}
