/* the vertex compilers. each transforms a vertex chunk's vertices into the
 * vertex buffer that the strip compilers index */
#include <nj_direct.h>
#include <ninja.h>
#include <sg_xpt.h>

#define WHITE 0xffffffff
/* the floats of a vertex's point, and of its normal */
#define VECTOR_FLOATS 3

void njCnkDirectVertexVN(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    NJS_DIRECT_COMPILE_LIGHT *light;
    Float intensity;
    Float r;
    Float g;
    Float b;
    Float a;

    do {
        njCalcPoint(NULL, (NJS_POINT3 *)vlist, (NJS_POINT3 *)&buf->px);
        vlist += VECTOR_FLOATS;
        njCalcVector(NULL, (NJS_VECTOR *)vlist, (NJS_VECTOR *)&buf->nx);
        njUnitVector((NJS_VECTOR *)&buf->nx);
        vlist += VECTOR_FLOATS;
        if (_nj_direct_compile_mode_ == DIRECT_COMPILE_LIGHT) {
            light = _nj_direct_compile_light_;
            r = light->r;
            g = light->g;
            b = light->b;
            a = 1.0F;
            /* the light travels along its (x, y, z). so a normal that faces
             * the light has a negative dot product with it */
            intensity = (buf->nx * light->x) + (buf->ny * light->y) +
                        (buf->nz * light->z);
            /* @hack spelling: intensity times -inten, plus ambient, in two
             * statements, where one expression, ambient - (inten * intensity),
             * would do */
            intensity *= -light->inten;
            intensity += light->ambient;
            if (intensity < light->ambient) {
                intensity = light->ambient;
            }
            if (intensity > 1.0F) {
                intensity = 1.0F;
            }
            intensity *= COLOR_MAX;
            if (r < 0.0F) {
                r = 0.0F;
            }
            if (r > 1.0F) {
                r = 1.0F;
            }
            if (g < 0.0F) {
                g = 0.0F;
            }
            if (g > 1.0F) {
                g = 1.0F;
            }
            if (b < 0.0F) {
                b = 0.0F;
            }
            if (b > 1.0F) {
                b = 1.0F;
            }
            /* @hack redundant: the clamps on a, which is always 1 */
            if (a < 0.0F) {
                a = 0.0F;
            }
            if (a > 1.0F) {
                a = 1.0F;
            }
            r *= intensity;
            g *= intensity;
            b *= intensity;
            a *= COLOR_MAX;
            /* @hack spelling: the bytes ORed blue first, where alpha first
             * would do */
            buf->col = CHANNEL_BYTE(b) | (CHANNEL_BYTE(g) << 8) |
                       (CHANNEL_BYTE(r) << 16) | (CHANNEL_BYTE(a) << 24);
        } else {
            buf->col = WHITE;
        }
        buf++;
    } while (--num);
}

void njCnkDirectVertexV(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    do {
        njCalcPoint(NULL, (NJS_POINT3 *)vlist, (NJS_POINT3 *)&buf->px);
        vlist += VECTOR_FLOATS;
        buf->nx = 0.0F;
        buf->ny = 0.0F;
        buf->nz = 1.0F;
        buf->col = WHITE;
        buf++;
    } while (--num);
}

void njCnkDirectVertexVD8(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    do {
        njCalcPoint(NULL, (NJS_POINT3 *)vlist, (NJS_POINT3 *)&buf->px);
        vlist += VECTOR_FLOATS;
        buf->nx = 0.0F;
        buf->ny = 0.0F;
        buf->nz = 1.0F;
        /* @hack type: the color read through vlist, a float pointer, where
         * a record of a point and a color would do */
        buf->col = *(Uint32 *)vlist++;
        buf++;
    } while (--num);
}

void njCnkDirectVertexVND8(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    do {
        njCalcPoint(NULL, (NJS_POINT3 *)vlist, (NJS_POINT3 *)&buf->px);
        vlist += VECTOR_FLOATS;
        njCalcVector(NULL, (NJS_VECTOR *)vlist, (NJS_VECTOR *)&buf->nx);
        njUnitVector((NJS_VECTOR *)&buf->nx);
        vlist += VECTOR_FLOATS;
        /* @hack type: the color read through vlist, a float pointer, where
         * a record of a point, a normal and a color would do */
        buf->col = *(Uint32 *)vlist++;
        buf++;
    } while (--num);
}
