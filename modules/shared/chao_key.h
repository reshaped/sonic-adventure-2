/* the Chao key a Chao box leaves. it falls and bounces where it is made, and
 * flies off once a player touches it at rest */
#ifndef SHARED_CHAO_KEY_H
#define SHARED_CHAO_KEY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the key's textures, sikake_36_32, sikake_40_64 and sikake_42_32 */
extern NJS_TEXLIST chao_key_texlist;
/* the key's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *chao_key_texlists[];

/* TRUE while a key's flight runs, or while game_state.chao_key is 1. a
 * flight sets game_state.chao_key to 1 near its target, and again as its
 * task is destroyed */
Bool chao_key_taken(void);
/* make a key at pos in task level 2, moving up at speed. return its task,
 * or null where task_create fails. the key falls and bounces on the
 * height floor_y until it rests there. a player's touch at rest ends the key
 * with a sparkle. the touch starts the key's flight, unless chao_key_taken
 * returns TRUE. the key also ends where placement_try_leave returns TRUE for
 * it */
struct task *chao_key_create(NJS_POINT3 *pos, Float floor_y, Float speed);

#endif
