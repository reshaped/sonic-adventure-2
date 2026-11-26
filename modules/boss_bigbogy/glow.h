/* a glow: a chunk model's alpha, rising and falling between two bounds. the
 * alpha is the material's that opens the model's polygon list */
#ifndef BOSS_BIGBOGY_GLOW_H
#define BOSS_BIGBOGY_GLOW_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;
/* a glow's settings. the alpha rises from minimum to maximum and falls back
 * each period steps. as the glow ends it sets the alpha to restored */
struct bigbogy_glow {
    Uint8 minimum;
    Uint8 maximum;
    Uint8 restored;
    Uint16 period;
    NJS_CNK_OBJECT *object; /* the node whose model's alpha glows */
};

/* make a glow of *spec as parent's child. a null spec takes a default:
 * bigbogy_glow_object's alpha, from 50 to 180 and back each 180 steps. one
 * glow lives at a time. the call returns null while one does, for a null
 * parent or object, and where task_create fails. period must not be 0. where
 * object's model has a polygon list, its first chunk must be a material with
 * a diffuse color */
struct task *bigbogy_glow_create(struct task *parent,
                                 const struct bigbogy_glow *spec);
#endif
