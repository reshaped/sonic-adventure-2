/* the sign, SIGNA in Prison Lane's object list, whose lit panel pulses.
 * stg11/sign.h is the same object compiled a second time */
#ifndef SHARED_SIGN_H
#define SHARED_SIGN_H
#include <ninja.h>

struct task;

/* the sign's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *sign_texlists[];

/* the object list's step for SIGNA. task needs a work, whose pos places the
 * sign and whose ang turns it */
void sign_init(struct task *task);

#endif
