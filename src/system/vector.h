/* vector arithmetic: sums, differences, a scale, a dot product and a
 * distance */
#ifndef SYSTEM_VECTOR_H
#define SYSTEM_VECTOR_H
#include <ninja.h>
#include <sg_xpt.h>

/* out = first + second. out may be first or second */
void vector_sum(const NJS_VECTOR *first, const NJS_VECTOR *second,
                NJS_VECTOR *out);
/* out = first - second. out may be first or second */
void vector_diff(const NJS_VECTOR *first, const NJS_VECTOR *second,
                 NJS_VECTOR *out);
/* out = first - second. out may be first but not second, since out takes
 * first before second is read */
void vector_subtract(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *out);
/* out = first + second. out may be first but not second, as for
 * vector_subtract */
void vector_add(NJS_VECTOR *first, NJS_VECTOR *second, NJS_VECTOR *out);
/* out = in times scale. out may be in */
void vector_scale(NJS_VECTOR *in, NJS_VECTOR *out, Float scale);
/* dot(first - from, second - from) */
Float vector_dot_from(const NJS_POINT3 *from, const NJS_POINT3 *first,
                      const NJS_POINT3 *second);
/* the distance between from and to */
Float vector_distance(const NJS_POINT3 *from, const NJS_POINT3 *to);

#endif
