/* the messenger's model nodes, the tuning its step reads and the builds its
 * draw uses */
#ifndef SHARED_MESSENGER_ART_H
#define SHARED_MESSENGER_ART_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/trail.h"
#include "shared/common_object.h"

struct messenger_texture_filenames {
    char sikake_34_64[16];
    char sikake_35_32[16];
    char sikake_41_32[16];
};

extern const struct messenger_texture_filenames messenger_texture_filenames;

/* the texture list of every part */
extern NJS_TEXLIST messenger_texlist;
/* @hack type: the body, face lights, winding key, arms and feet each declared
 * as an array of one, where the node alone would do */
extern NJS_CNK_OBJECT messenger_body[];
extern NJS_CNK_OBJECT messenger_propeller;
extern NJS_CNK_OBJECT messenger_face_lights[];
extern NJS_CNK_OBJECT messenger_winding_key[];
extern NJS_CNK_OBJECT messenger_head;
extern NJS_CNK_OBJECT messenger_left_arm[];
extern NJS_CNK_OBJECT messenger_right_arm[];
extern NJS_CNK_OBJECT messenger_right_foot[];
extern NJS_CNK_OBJECT messenger_left_foot[];
/* a burst is a run of steps in which a steering messenger adds
 * messenger_accel to its speed. a step that brings it less than
 * messenger_stall nearer its goal may start one */
extern Float messenger_accel;
extern Float messenger_stall;
/* what each steering step multiplies speed by */
extern Float messenger_drag;
/* what a step adds to twirl while the messenger rises, talks or flies */
extern Sint32 messenger_twirl_rate;
/* what a step adds to twirl while the messenger is carried */
extern Sint32 messenger_twirl_held_rate;
/* what a step adds to rotor while the messenger idles or appears */
extern Sint32 messenger_rotor_idle_rate;
/* what a step adds to rotor while the messenger rises, talks or flies */
extern Sint32 messenger_rotor_rate;
/* what a step adds to scale while the messenger appears or flies */
extern Float messenger_grow_rate;
/* what an idle step takes from scale once the messenger's timer is 0 */
extern Float messenger_shrink_rate;
/* what a pick-up or a chat adds to swing_speed. a carried messenger's random
 * kicks scale it */
extern Float messenger_swing_kick;
/* what each idle step multiplies swing_speed by */
extern Float messenger_swing_damping;
extern struct trail_style messenger_trail_style;
/* TRUE once messenger_build_models has compiled all nine builds */
extern Sint32 messenger_built;
/* the face lights built in white, or null. messenger_build_models builds them
 * once all nine builds compile. where it is null, the draw colors the face
 * lights white as it draws them */
extern NJS_DIRECT_HEAD *messenger_white_face_build;
/* each part's build. the draw uses all but the head's and the face lights' */
extern struct model_build messenger_builds[9];
/* the index of each part's build in messenger_builds */
enum messenger_build_index {
    MESSENGER_BUILD_BODY,
    MESSENGER_BUILD_PROPELLER,
    MESSENGER_BUILD_FACE_LIGHTS,
    MESSENGER_BUILD_WINDING_KEY,
    MESSENGER_BUILD_HEAD,
    MESSENGER_BUILD_LEFT_ARM,
    MESSENGER_BUILD_RIGHT_ARM,
    MESSENGER_BUILD_RIGHT_FOOT,
    MESSENGER_BUILD_LEFT_FOOT,
    MESSENGER_BUILDS
};

#endif
