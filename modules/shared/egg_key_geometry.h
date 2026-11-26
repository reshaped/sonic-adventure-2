/* Native geometry shared by the Death Chamber and Egg Quarters key archives. */
#ifndef SHARED_EGG_KEY_GEOMETRY_H
#define SHARED_EGG_KEY_GEOMETRY_H
#include <sg_xpt.h>

struct egg_key_vertices {
    Sint32 head[2];
    struct {
        NJS_POINT3 position;
        NJS_VECTOR normal;
        Uint32 color;
    } vertices[229];
    Sint32 end;
};

#endif
