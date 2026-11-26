/* the call through which the chunk drawers take a material chunk */
#ifndef NJ_CNK_SET_MATERIAL_H
#define NJ_CNK_SET_MATERIAL_H
#include <sg_xpt.h>

/* take a material chunk's colors and its blend. head is the chunk's first
 * word, and colors points to the packed ARGB words after it. the chunk's type
 * says which of its diffuse, ambient and specular colors it holds. each color
 * it holds goes to diffuse, ambient or specular as four floats, in NJS_ARGB's
 * order. each float is its byte times 0.00392157, 1 over 255 to six digits. a
 * color the chunk lacks, or whose pointer is null, leaves its floats as they
 * were.
 *
 * while NJD_CONTROL_3D_CONSTANT_MATERIAL is set, the chunk's diffuse color
 * gives diffuse _nj_constant_material_ instead, and diffuse must not be null.
 * while NJD_CONTROL_3D_OFFSET_MATERIAL is set and
 * NJD_CONTROL_3D_CONSTANT_MATERIAL clear, diffuse takes the color plus
 * _nj_constant_material_.
 *
 * the call then passes head to njSetkmCnkBlendMode */
void njCnkSetMaterial(Uint32 head, Uint32 *colors, Float *diffuse,
                      Float *ambient, Float *specular);
/* the index of each channel among a color's four floats */
#define ARGB_A 0
#define ARGB_R 1
#define ARGB_G 2
#define ARGB_B 3
/* where the ambient color starts in an array of floats that holds the diffuse
 * color, then the ambient, as the multi drawers' material does */
#define MATERIAL_AMBIENT 4

#endif
