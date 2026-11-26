/* Green Hill's scenery: the palm, the flowers, the grass, the rock, the
 * shadow and the two totems */
#ifndef STG31_SCENERY_H
#define STG31_SCENERY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the texture lists for texture_load_shared of the scenery, ended by null */
extern NJS_TEXLIST *green_hill_scenery_texlists[];

extern NJS_TEXLIST green_hill_palm_texlist;
extern NJS_TEXLIST green_hill_rock_texlist;
extern NJS_TEXLIST green_hill_tall_totem_texlist;
extern NJS_TEXLIST green_hill_short_totem_texlist;
extern const char green_hill_totem_texture_name[6];

/* compile the palm's model at buffer, as model_builds_compile does, and
 * return buffer past the build. the palms draw the build once a call has
 * compiled it */
Uint8 *green_hill_palm_build(Uint8 *buffer);

/* the object list's steps for YASI, HANA, HANA2, KUSA, ROCK, KAGE, TP1 and
 * TP2. each does nothing where task leaves at its placement's range.
 * otherwise it makes task the object at its work's pos. task needs a work.
 *
 * the shadow draws turned by all of ang, and the rest by ang.y alone. the
 * palm's init clears ang.x and ang.z. the other collision shapes take ang as
 * collision.h says, so those two can turn a flower's shape and not its model.
 *
 * the palm, the flowers, the grass and the totems collide by a record of kind
 * COLLISION_OBJECT. the rock is solid ground in the world, and the shadow
 * doesn't collide. a turning flower of scl.x 1 or more draws
 * green_hill_turning_flower_a, and any other _b */
void green_hill_palm_init(struct task *task);
void green_hill_turning_flower_init(struct task *task);
void green_hill_bobbing_flower_init(struct task *task);
void green_hill_grass_init(struct task *task);
void green_hill_rock_init(struct task *task);
void green_hill_shadow_init(struct task *task);
void green_hill_tall_totem_init(struct task *task);
void green_hill_short_totem_init(struct task *task);

#endif
