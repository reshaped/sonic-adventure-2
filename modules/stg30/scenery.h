#ifndef STG30_SCENERY_H
#define STG30_SCENERY_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

enum scenery_motion { SCENERY_SCROLL, SCENERY_PULSE };
struct scenery_strip {
    Sint32 motion;
    NJS_CNK_OBJECT *object;
    struct texture_anim *anim;
    Sint32 v_speed;
    Sint32 u_speed;
};
#define SCENERY_STRIPS 57
extern struct scenery_strip scenery_strips[SCENERY_STRIPS];

#endif
