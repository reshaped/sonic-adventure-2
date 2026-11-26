#ifndef SHARED_GUN_MECH_BOMB_ART_H
#define SHARED_GUN_MECH_BOMB_ART_H

#include "collision/collision.h"
#include "draw/texture_anim.h"

/* the tuning of the beam's bomb, and of the torus and pillar it starts at its
 * full size */
struct bomb_tuning {
    Float size; /* the scale the bomb grows to, its full size */
    Float fade; /* what its alpha loses each step as it fades */
    Float grow; /* what its scale gains each step until it reaches size */
    Float unk_0c;
    Float torus_fade;
    /* what the bomb's scale gains each step from its full size on, and the
     * torus's from its start */
    Float swell;
    Float pillar_fade;
    Float pillar_rise; /* how far the pillar rises each step */
    Float unk_20[2];
};

extern struct texture_anim mech_boss_beam_bomb_anim;
extern struct bomb_tuning mech_boss_beam_bomb_tuning;
extern struct collision_shape mech_boss_beam_bomb_shape;

#endif
