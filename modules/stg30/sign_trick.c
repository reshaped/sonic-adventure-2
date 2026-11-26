/* Final Rush draws the trick sign with its own model and texture scroll. */
#include <ninja.h>
#include <sg_xpt.h>
#include "stg30/sign_trick.h"
#include "stg30/sign_textures.h"
#define SIGN_TRICK

NJS_TEXLIST *trick_sign_texlists[2] = {
#include "stg30/sign_trick/trick_sign_texlists.inc"
};

/* the scroll's speeds along u and v, in texture coordinates a step */
static Sint32 scroll_u_speed = 8;
static Sint32 scroll_v_speed = 0;

#define SIGN_TEXLIST trick_sign_texlist
#define SIGN_OBJECT trick_sign_object
#define SIGN_ANIM trick_sign_anim
#define SIGN_INIT trick_sign_init
#include "shared/orbital/sign_scroll_task.h" /* IWYU pragma: keep -- the code it compiles here */
