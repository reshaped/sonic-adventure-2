#include <ninja.h>

#include "shared/sign/text.inc"

/* SHC retains the SET editor captions in its constant pool. */
#ifdef NONMATCHING
/* this side marks the captions used, so that GCC keeps them as SHC does. GCC
 * drops a static datum that nothing reads */
__attribute__((used)) static const char sign_editor_rot_x[] = SIGN_EDITOR_ROT_X;
__attribute__((used)) static const char sign_editor_rot_y[] = SIGN_EDITOR_ROT_Y;
__attribute__((used)) static const char sign_editor_rot_z[] = SIGN_EDITOR_ROT_Z;
#endif

#include "shared/prison_machinery/sign_task.h"

#include "shared/prison_machinery/sign_art.h"

NJS_TEXLIST *sign_texlists[2] = {
#include "shared/sign/sign_texlists.inc"
};
