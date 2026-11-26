#include <ninja.h>
#include <sg_xpt.h>

/* the SDK declares idata without const, though the call only reads it */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
void njOverhauserSpline(Float *idata, Float *odata, NJS_SPLINE *attr,
                        Float frame)
{
    Float square = frame * frame;
    Float cube = frame * square;
    Float w0 = ((2.0F * square) - frame - cube) * 0.5F;
    Float w1 = (1.5F * cube) - (2.5F * square) + 1.0F;
    Float w2 = (0.5F * frame) - (1.5F * cube) + (2.0F * square);
    Float w3 = (cube - square) * 0.5F;

    /* odata[0]'s sum adds w0's term last, as the image's code does. a float
     * sum's order shows in its result */
    odata[0] =
        (w1 * idata[3]) + (w2 * idata[6]) + (w3 * idata[9]) + (w0 * idata[0]);
    odata[1] =
        (w0 * idata[1]) + (w1 * idata[4]) + (w2 * idata[7]) + (w3 * idata[10]);
    odata[2] =
        (w0 * idata[2]) + (w1 * idata[5]) + (w2 * idata[8]) + (w3 * idata[11]);
}
