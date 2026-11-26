#include <assert.h>
#include "draw/texture_anim.h"

int main(void)
{
    Sint16 keys[] = {1, 1, 0, 1, 32, -12};
    Sint16 polygons[] = {17, 23, 5, 7};
    NJS_CNK_MODEL model = {0};
    struct texture_anim animation = {0};

    model.plist = polygons;
    animation.uvs = keys;
    texture_anim_apply(&model, &animation, 10);
    assert(polygons[0] == 17 && polygons[1] == 23);
    assert(polygons[2] == 5 && polygons[3] == 7);

    animation.flags = TEXTURE_ANIM_SCROLL;
    animation.frames = (2U << 16) | 3;
    animation.frame_time = 2;
    texture_anim_apply(&model, &animation, 10);
    assert(polygons[0] == 17 && polygons[1] == 23);
    assert(polygons[2] == 42 && polygons[3] == 3);
    return 0;
}
