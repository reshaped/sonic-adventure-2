/* the platform, STAGE in Meteor Herd's object list: a floor drawn as one of
 * two models */
#ifndef STG32_PLATFORM_H
#define STG32_PLATFORM_H
#include <ninja.h>

struct task;

/* the texture lists of the platform's two models, which name the same
 * textures */
extern NJS_TEXLIST meteor_platform_texlist;
extern NJS_TEXLIST meteor_platform_alt_texlist;
/* the two lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *meteor_platform_textures[];
/* the object list's step for STAGE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the platform's step, draw and
 * destructor. task needs a work, whose pos places the platform and whose ang.y
 * turns it. a scl.x of 1 or more draws the alternate model, and less the plain
 * one. both models collide alike.
 *
 * while a player lies within 300 of pos, task has two children, which
 * solid_object_create_flags makes. one is a solid object of
 * meteor_platform_collision. the other is a solid object of
 * meteor_platform_dusty_collision, whose node goes into the world with
 * WORLD_DUSTY */
void meteor_platform_init(struct task *task);

#endif
