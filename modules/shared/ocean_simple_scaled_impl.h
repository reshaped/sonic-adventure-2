/* ocean_model_draw_simple_scaled of ocean_util.h. with HIDDEN_BASE_UTIL,
 * ocean_util.c includes it before ocean_model_draw_scaled, and otherwise
 * after ocean_model_draw_scaled_v, as the images order the three functions */
#ifndef SHARED_OCEAN_SIMPLE_SCALED_IMPL_H
#define SHARED_OCEAN_SIMPLE_SCALED_IMPL_H

void ocean_model_draw_simple_scaled(NJS_CNK_MODEL *model, Float scale)
{
    /* @hack spelling: radius, a pointer to model->r, where model->r read and
     * written in place would do */
    Float *radius = &model->r;
    Float saved = *radius;

    *radius *= scale;
    njScale(NULL, scale, scale, scale);
    njCnkSimpleDrawModel(model);
    *radius = saved;
}

#endif
