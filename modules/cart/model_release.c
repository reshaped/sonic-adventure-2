/* the release of the karts' textures, which cart_load_model_textures loads */
#include <ninja.h>

#include "cart/course.h"

void cart_release_model_textures(void)
{
    switch (cart_setup.course_mode) {
    case CART_TAILS:
        njReleaseTexture(&cart_tails_textures);
        break;
    case CART_ROUGE:
        njReleaseTexture(&cart_rouge_textures);
        break;
    default:
        njReleaseTexture(&cart_race_textures);
        break;
    }
}
