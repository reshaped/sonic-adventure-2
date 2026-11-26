/* a garden's Chao egg */
#ifndef CHAO_EGG_H
#define CHAO_EGG_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
struct chao_gene;
struct chao_profile;
struct chao_save;

/* where an egg comes from: a save, a mating, a cocoon or the transfer
 * machine. a laid egg starts small and grows. a reborn egg collides only
 * after its cocoon's hold and fade and 35 frames more. a transferred egg
 * collides after 60 frames */
enum chao_egg_origin { EGG_RESIDENT, EGG_LAID, EGG_REBORN, EGG_TRANSFERRED };

/* an egg's state, at its task's extra */
struct egg_state {
    enum chao_egg_origin origin; /* the origin it was made with */
    Bool moving; /* held, it was last seen moving rather than still */
    /* the shakes it needs yet. each start and each stop counts one */
    Sint32 shakes_left;
    Sint32 unk_0c;
    Float frame; /* the frame of the motion it plays */
    Sint32 rest_frames; /* left at rest before it cracks, shaken or not */
    /* size grows to 1, and wobble springs to 1 at wobble_speed. the egg's
     * width is size times wobble, and its height size times 2 - wobble */
    Float size, wobble, wobble_speed;
    /* frames before it collides, which also waits for size to pass 0.8 */
    Sint32 collision_delay;
};

extern NJS_TEXLIST egg_texlist;
extern NJS_CNK_OBJECT egg_shell_top[1], egg_shell_bottom[1];
extern NJS_CNK_OBJECT egg_open_model[1], egg_whole_model[1];
extern NJS_MOTION egg_shake_motion[1], egg_hatch_motion_206,
    egg_hatch_motion_205;

/* an egg at position, which rolls until it comes to rest. it takes a copy of
 * gene, or a new gene where gene is null. it takes a copy of a non-null
 * profile, whose type becomes CHAO_EGG.
 *
 * in a garden, CHAO_AREA_NEUTRAL to CHAO_AREA_DARK, the profile's garden
 * becomes the area. elsewhere it becomes save's garden, where that is a
 * garden, and save must not be null there. a non-null save becomes the egg's
 * save record, and takes the egg's profile and gene at once */
struct task *chao_create_egg_with_profile(struct chao_gene *gene,
                                          struct chao_profile *profile,
                                          NJS_POINT3 *position,
                                          enum chao_egg_origin origin,
                                          struct chao_save *save);
/* chao_create_egg_with_profile with a null profile */
struct task *chao_create_egg(struct chao_gene *gene, NJS_POINT3 *position,
                             enum chao_egg_origin origin,
                             struct chao_save *save);

#endif
