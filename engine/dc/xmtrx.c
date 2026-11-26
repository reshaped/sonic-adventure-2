/* The Dreamcast batch point kernel and its host C model. */
#include "xmtrx.h"

#include <stdint.h>

#ifdef DREAMCAST
#else
#include "fpu.h"
#endif

/* a point's three floats, in bytes. the asm adds them to end as #12 */
#define POINT_BYTES 12

/* FV0 and FV4 take the points by turns. each half pass loads a point and issues
 * its FTRV. it then stores the point before, whose FTRV the half pass before
 * issued, 13 instructions earlier. DT counts the points left to load. where
 * none is left, a half pass branches to the stores of the point whose FTRV it
 * issued.
 *
 * the stores fill a point's floats backwards by pre-decrement, so end starts
 * POINT_BYTES past out. after a point's loads, skip takes in from the end of
 * its floats to the next point. after a point's stores, step takes end from
 * its first float to POINT_BYTES past the next point's */
void zam_xmtrx_transform_points(const float *in, uint32_t in_stride, float *out,
                                uint32_t out_stride, uint32_t count)
{
#ifdef DREAMCAST
    float *end = out;
    uint32_t skip = in_stride - POINT_BYTES;
    uint32_t step = out_stride + POINT_BYTES;

    __asm__ volatile("tst %[count],%[count]\n\t"
                     "bt 9f\n\t"
                     "add #12,%[end]\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr1\n\t"
                     "fmov.s @%[in]+,fr2\n\t"
                     "fldi1 fr3\n\t"
                     "ftrv xmtrx,fv0\n\t"
                     "dt %[count]\n\t"
                     "bt/s 7f\n\t"
                     "add %[skip],%[in]\n"
                     "1:\n\t"
                     "fmov.s @%[in]+,fr4\n\t"
                     "fmov.s @%[in]+,fr5\n\t"
                     "fmov.s @%[in]+,fr6\n\t"
                     "fldi1 fr7\n\t"
                     "ftrv xmtrx,fv4\n\t"
                     "add %[skip],%[in]\n\t"
                     "fmov.s fr2,@-%[end]\n\t"
                     "fmov.s fr1,@-%[end]\n\t"
                     "fmov.s fr0,@-%[end]\n\t"
                     "dt %[count]\n\t"
                     "bt/s 8f\n\t"
                     "add %[step],%[end]\n\t"
                     "fmov.s @%[in]+,fr0\n\t"
                     "fmov.s @%[in]+,fr1\n\t"
                     "fmov.s @%[in]+,fr2\n\t"
                     "fldi1 fr3\n\t"
                     "ftrv xmtrx,fv0\n\t"
                     "add %[skip],%[in]\n\t"
                     "fmov.s fr6,@-%[end]\n\t"
                     "fmov.s fr5,@-%[end]\n\t"
                     "fmov.s fr4,@-%[end]\n\t"
                     "dt %[count]\n\t"
                     "bf/s 1b\n\t"
                     "add %[step],%[end]\n"
                     "7:\n\t"
                     "fmov.s fr2,@-%[end]\n\t"
                     "fmov.s fr1,@-%[end]\n\t"
                     "bra 9f\n\t"
                     "fmov.s fr0,@-%[end]\n"
                     "8:\n\t"
                     "fmov.s fr6,@-%[end]\n\t"
                     "fmov.s fr5,@-%[end]\n\t"
                     "fmov.s fr4,@-%[end]\n"
                     "9:"
                     : [in] "+&r"(in), [end] "+&r"(end), [count] "+&r"(count)
                     : [skip] "r"(skip), [step] "r"(step)
                     : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                       "t", "memory");
#else
    while (count != 0) {
        float point[4] = {in[0], in[1], in[2], 1.0F};

        dc_ftrv(dc_xmtrx, point);
        out[0] = point[0];
        out[1] = point[1];
        out[2] = point[2];
        if (--count == 0) {
            break;
        }
        in = (const float *)((const char *)in + in_stride);
        out = (float *)((char *)out + out_stride);
    }
#endif
}
