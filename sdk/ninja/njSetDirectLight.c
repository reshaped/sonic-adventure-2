#include <ninja.h>
#include <nj_direct_draw.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* the near plane's z. njDirectClip reads it from near_z. the direct drawers
 * and their near-clip loops load a 1 of their own */
#define NEAR_Z 1.0F

struct direct_lighting direct_light;
struct direct_frustum direct_clip;

void njSetDirectClip(void)
{
    struct direct_frustum *clip = &direct_clip;

    clip->far_z = _nj_far_clip_;
    clip->near_z = NEAR_Z;
    clip->scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    clip->scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    clip->right = _nj_2d_clip_[1].x - _nj_screen_.cx;
    clip->left = _nj_2d_clip_[0].x - _nj_screen_.cx;
    clip->bottom = _nj_2d_clip_[1].y - _nj_screen_.cy;
    clip->top = _nj_2d_clip_[0].y - _nj_screen_.cy;
}

void njSetDirectLight(Float x, Float y, Float z)
{
    struct direct_lighting *light = &direct_light;

    light->direction.x = x;
    light->direction.y = y;
    light->direction.z = z;
    light->scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    light->scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    light->center_x = _nj_screen_.cx;
    light->center_y = _nj_screen_.cy;
    light->environment_scale = 0.5F;
}

void njSetDirectLightIntensity(Float intensity, Float ambient)
{
    struct direct_lighting *light = &direct_light;

    light->negative_intensity = -intensity;
    light->ambient_bias = ambient - 1.0F;
}

void njSetDirectLightColor(Float red, Float green, Float blue)
{
    struct direct_lighting *light = &direct_light;

    light->red = red;
    light->green = green;
    light->blue = blue;
}
