/* the guard eyes. an attack calls down up to GUARD_EYES of them around player
 * 0. each aims at player 0, fires a beam that hurts players, and fades */
#ifndef STG26_GUARD_EYE_H
#define STG26_GUARD_EYE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the eye's and the beam's texture lists, ended by null */
extern NJS_TEXLIST *guard_eye_texlists[];

#define GUARD_EYES 4

/* the pos of the eye in each slot, as its last step left it */
extern NJS_POINT3 guard_eye_positions[GUARD_EYES];
/* how many eyes are alive. guard_eye_create counts each eye up, and its
 * destructor counts it down */
extern Sint32 guard_eye_count;
/* how many places guard_eye_find_places left free that no eye has taken */
extern Sint32 guard_eye_free_count;

extern NJS_TEXLIST guard_eye_texlist;
extern NJS_TEXLIST guard_eye_beam_texlist;

/* compile the eye's and the beam's models at buffer, and return the byte
 * after them. once both have compiled, an eye that isn't fading draws the
 * compiled body. while objects_fog_off isn't 0, the call turns the fog off
 * for the builds and on after them */
Uint8 *guard_eye_build(Uint8 *buffer);

/* list the places around player 0 that an attack's eyes may take, and set
 * guard_eye_free_count to their count. a place is free where no surface lies
 * between it and player 0. it must also lie 25 or more from the current
 * camera. guard is unread */
void guard_eye_find_places(struct task *guard);

/* start an eye in slot, from 0 to GUARD_EYES - 1. it takes a random place off
 * guard_eye_find_places' list. the call starts no eye where the heap can't
 * give it a task. guard_eye_free_count must be 1 or more, unchecked. the
 * heap must have room for the eye's state and its collision record,
 * unchecked */
void guard_eye_create(int slot);

#endif
