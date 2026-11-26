#ifndef STG30_SIGN_TRICK_H
#define STG30_SIGN_TRICK_H
#include <ninja.h>

#include "draw/texture_anim.h"

struct task;

extern NJS_CNK_OBJECT trick_sign_object;
extern struct texture_anim trick_sign_anim;
extern NJS_TEXLIST *trick_sign_texlists[];

/* SIGNTRICK, sign_end_e.c's sign with Final Rush's trick sign model. its
 * texture scrolls 8 along u a step */
void trick_sign_init(struct task *task);

#endif
