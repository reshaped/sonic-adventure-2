/* the call through which the chunk drawers take a material chunk */
#include <ninja.h>
#include <nj_cnk_set_material.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* a material chunk's type, less NJD_MATOFF, has a bit for each color the
 * chunk holds. the colors follow the chunk's first word in this order */
#define MAT_DIFFUSE 1
#define MAT_AMBIENT 2
#define MAT_SPECULAR 4

/* where a packed color's bytes lie, as shifts */
#define ALPHA_SHIFT 24
#define RED_SHIFT 16
#define GREEN_SHIFT 8
#define BLUE_SHIFT 0
/* a packed color's byte at shift */
#define COLOR_BYTE(color, shift) (((color) >> (shift)) & 0xff)
/* 1 over 255, to six digits: a color's byte times this is its float */
#define COLOR_SCALE 0.00392157F

void njCnkSetMaterial(Uint32 head, Uint32 *colors, Float *diffuse,
                      Float *ambient, Float *specular)
{
    Uint32 held = (Uint8)head - NJD_MATOFF;
    /* @hack redundant: register on color */
    register Uint32 color;
    /* @hack type: material, a float pointer stepped through
     * _nj_constant_material_'s a, r, g and b, where reads of the four by name
     * would do */
    Float *material;

    /* @hack spelling: diffuse stepped through its four floats, where stores
     * to diffuse[0] to diffuse[3] would do */
    if (held & MAT_DIFFUSE) {
        color = *colors++;
        if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CONSTANT_MATERIAL) {
            material = &_nj_constant_material_.a;
            *diffuse++ = *material++;
            *diffuse++ = *material++;
            *diffuse++ = *material++;
            *diffuse = *material;
        } else if (diffuse != NULL) {
            if (_nj_control_3d_flag_ & NJD_CONTROL_3D_OFFSET_MATERIAL) {
                material = &_nj_constant_material_.a;
                *diffuse++ =
                    ((Float)COLOR_BYTE(color, ALPHA_SHIFT) * COLOR_SCALE) +
                    *material++;
                *diffuse++ =
                    ((Float)COLOR_BYTE(color, RED_SHIFT) * COLOR_SCALE) +
                    *material++;
                *diffuse++ =
                    ((Float)COLOR_BYTE(color, GREEN_SHIFT) * COLOR_SCALE) +
                    *material++;
                *diffuse =
                    ((Float)COLOR_BYTE(color, BLUE_SHIFT) * COLOR_SCALE) +
                    *material;
            } else {
                *diffuse++ =
                    (Float)COLOR_BYTE(color, ALPHA_SHIFT) * COLOR_SCALE;
                *diffuse++ = (Float)COLOR_BYTE(color, RED_SHIFT) * COLOR_SCALE;
                *diffuse++ =
                    (Float)COLOR_BYTE(color, GREEN_SHIFT) * COLOR_SCALE;
                *diffuse = (Float)COLOR_BYTE(color, BLUE_SHIFT) * COLOR_SCALE;
            }
        }
    }
    if (held & MAT_AMBIENT) {
        color = *colors++;
        if (ambient != NULL) {
            /* @hack spelling: ambient stepped through its four floats, where
             * stores to ambient[0] to ambient[3] would do */
            *ambient++ = (Float)COLOR_BYTE(color, ALPHA_SHIFT) * COLOR_SCALE;
            *ambient++ = (Float)COLOR_BYTE(color, RED_SHIFT) * COLOR_SCALE;
            *ambient++ = (Float)COLOR_BYTE(color, GREEN_SHIFT) * COLOR_SCALE;
            *ambient = (Float)COLOR_BYTE(color, BLUE_SHIFT) * COLOR_SCALE;
        }
    }
    if (held & MAT_SPECULAR) {
        color = *colors;
        if (specular != NULL) {
            /* @hack spelling: specular stepped through its four floats, where
             * stores to specular[0] to specular[3] would do */
            *specular++ = (Float)COLOR_BYTE(color, ALPHA_SHIFT) * COLOR_SCALE;
            *specular++ = (Float)COLOR_BYTE(color, RED_SHIFT) * COLOR_SCALE;
            *specular++ = (Float)COLOR_BYTE(color, GREEN_SHIFT) * COLOR_SCALE;
            *specular = (Float)COLOR_BYTE(color, BLUE_SHIFT) * COLOR_SCALE;
        }
    }
    njSetkmCnkBlendMode(head);
}
