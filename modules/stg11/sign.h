/* the sign, SIGNA in Iron Gate's object list. it is the sign of shared/sign.h,
 * drawn only where it may be in view */
#ifndef STG11_SIGN_H
#define STG11_SIGN_H
#include <ninja.h>

struct task;

/* the sign's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *iron_gate_sign_texlists[];

/* the object list's step for SIGNA. it sets task up as sign_init does. the
 * sign draws nothing where a ball of radius 25 about its pos lies wholly
 * behind the camera */
void iron_gate_sign_init(struct task *task);

#endif
