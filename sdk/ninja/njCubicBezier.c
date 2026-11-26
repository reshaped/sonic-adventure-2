#include <ninja.h>
#include <sg_xpt.h>

/* idata isn't const, as the SDK declares it */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
void njCubicBezierSpline(Float *idata, Float *odata, NJS_SPLINE *attr,
                         Float frame)
{
    /* @hack spelling: t, a local for frame, where frame read in place would
     * do */
    Float t = frame;
    Float u = 1.0F - t;
    Float w0 = u * u * u;
    Float w1 = t * u * u * 3.0F;
    Float w2 = t * t * u * 3.0F;
    Float w3 = t * t * t;

    /* odata[0]'s sum adds w0's term last, as the image's code does. a float
     * sum's order shows in its result */
    odata[0] =
        (w1 * idata[3]) + (w2 * idata[6]) + (w3 * idata[9]) + (w0 * idata[0]);
    odata[1] =
        (w0 * idata[1]) + (w1 * idata[4]) + (w2 * idata[7]) + (w3 * idata[10]);
    odata[2] =
        (w0 * idata[2]) + (w1 * idata[5]) + (w2 * idata[8]) + (w3 * idata[11]);
}
