#include <ninja.h>
#include <sg_xpt.h>

/* idata isn't const, as the SDK declares it */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
void njCardinalSpline(Float *idata, Float *odata, NJS_SPLINE *attr, Float frame)
{
    /* @hack spelling: t, a local for frame, where frame read in place would
     * do */
    Float t = frame;
    /* @hack spelling: tension, a local for attr's first float, where
     * attr->fparam[0] read in place would do */
    Float tension = attr->fparam[0];
    Float s = (1.0F - tension) * 0.5F;
    Float square = t * t;
    Float cube = t * square;
    Float w0 = ((2.0F * square) - t - cube) * s;
    Float w1 = ((2.0F - s) * cube) + ((s - 3.0F) * square) + 1.0F;
    Float w2 = ((s - 2.0F) * cube) + ((3.0F - (2.0F * s)) * square) + (s * t);
    Float w3 = (cube - square) * s;

    /* odata[0]'s sum adds w0's term last, as the image's code does. a float
     * sum's order shows in its result */
    odata[0] =
        (w1 * idata[3]) + (w2 * idata[6]) + (w3 * idata[9]) + (w0 * idata[0]);
    odata[1] =
        (w0 * idata[1]) + (w1 * idata[4]) + (w2 * idata[7]) + (w3 * idata[10]);
    odata[2] =
        (w0 * idata[2]) + (w1 * idata[5]) + (w2 * idata[8]) + (w3 * idata[11]);
}
