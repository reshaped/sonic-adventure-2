/* HATA and HATA2 in Sand Ocean's and Hidden Base's object lists: a flag and a
 * tilted flag */
#ifndef STG21_FLAG_H
#define STG21_FLAG_H
#include <ninja.h>

struct task;

/* the cloth's frames, which a flag shows one a step, in a loop */
#define FLAG_FRAMES 90
/* the frame a flag shows at game_state.steps 0, from 0 to FLAG_FRAMES - 1.
 * its init draws it at random */
#define FLAG_PHASE(task) ((task)->unk_3c.l)

extern NJS_CNK_MODEL *ocean_flag_models[FLAG_FRAMES];
/* the flags' texture lists for texture_load_shared, ended by null. in Sand
 * Ocean each frame of cloth has its own, and in Hidden Base one serves every
 * frame. the support's list comes after the cloth's, at FLAG_FRAMES in Sand
 * Ocean and at 1 in Hidden Base */
extern NJS_TEXLIST *ocean_flag_texlists[];
extern NJS_TEXLIST flag_support_texlist;
extern NJS_CNK_MODEL flag_support_model;
extern NJS_TEXLIST flag_shadow_support_texlist;
extern NJS_TEXLIST flag_bright_support_texlist;
extern NJS_CNK_MODEL flag_shadow_support;
extern NJS_CNK_MODEL flag_bright_support;
extern NJS_CNK_OBJECT flag_support_root;
extern NJS_CNK_OBJECT flag_shadow_support_root;
extern NJS_CNK_OBJECT flag_bright_support_root;
/* Hidden Base's one texture list for every frame of cloth */
extern NJS_TEXLIST hidden_base_flag_cloth;
#ifdef HIDDEN_BASE_FLAG
#define FLAG_CLOTH_TEXLIST(frame) (&hidden_base_flag_cloth)
#else
#define FLAG_CLOTH_TEXLIST(frame) (ocean_flag_texlists[frame])
#endif

/* the tilted flag's supports' texture lists for texture_load_shared, the
 * shadow's and then the bright one's, ended by null */
extern NJS_TEXLIST *tilted_flag_texlists[];

/* the object list's steps for HATA, ocean_flag_init, and for HATA2,
 * tilted_flag_init. each does nothing where task leaves at its placement's
 * range. otherwise it gives task the flag's step, draw and destructor.
 *
 * task needs a work, whose pos places the flag and whose ang.y turns it. each
 * later step tests the range as the first did. scl.x + 1 scales the flag.
 * HATA2's cloth is turned 70 degrees about x. the whole part of |scl.z| picks
 * its support. an even one picks the shadow support, and an odd one the
 * bright support */
void ocean_flag_init(struct task *task);
void tilted_flag_init(struct task *task);

#endif
