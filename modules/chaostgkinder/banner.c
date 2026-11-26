#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/services.h"
#include "chaostgkinder/service_menu.h"
#include "system/game_state.h"
#include "system/text.h"
#include "system/texture.h"

static const char texture_name[] = "al_stg_kinder_ad_tex";
/* the strips' textures are STRIPS_TEXELS texels high. each
 * kinder_banner_frame is a row of them, STRIP_HEIGHT texels high */
#define STRIPS_TEXELS 256.0F
#define UV(texel) ((texel) * (1.0F / STRIPS_TEXELS))
#define STRIP_HEIGHT 32
/* the frame's texture of kinder_service_textures, and the strips'. each
 * strips texture holds one language's titles and the other's captions */
#define FRAME_TEXTURE 0
#define FIRST_STRIPS 2
#define SECOND_STRIPS 3
#define BACK_DEPTH 0.3F
#define FRONT_DEPTH 0.301F
static const NJS_QUAD_TEXTURE backdrop = {
#include "chaostgkinder/banner/backdrop.inc"
};
static const NJS_QUAD_TEXTURE left_cap = {
#include "chaostgkinder/banner/left_cap.inc"
};
static const NJS_QUAD_TEXTURE right_cap = {
#include "chaostgkinder/banner/right_cap.inc"
};
static const NJS_QUAD_TEXTURE emblem = {
#include "chaostgkinder/banner/emblem.inc"
};

#define TEXTURES 7
static NJS_TEXNAME textures[TEXTURES];
NJS_TEXLIST kinder_service_textures = {textures, TEXTURES};
static NJS_QUAD_TEXTURE japanese_title = {
#include "chaostgkinder/banner/japanese_title.inc"
};
static NJS_QUAD_TEXTURE japanese_caption = {
#include "chaostgkinder/banner/japanese_caption.inc"
};
static NJS_QUAD_TEXTURE english_title = {
#include "chaostgkinder/banner/english_title.inc"
};
static NJS_QUAD_TEXTURE english_caption = {
#include "chaostgkinder/banner/english_caption.inc"
};
static Sint32 frame;
static Uint32 tint;

void kinder_banner_load(void)
{
    texture_load_pvm(texture_name, &kinder_service_textures);
}

void kinder_banner_release(void)
{
    njReleaseTexture(&kinder_service_textures);
}

void kinder_banner_select(Sint32 selection, Uint32 color)
{
    frame = selection;
    tint = color;
}

/* @hack spelling: a macro, where a function would do */
/* @hack spelling: the v2 store and the draw in one statement, where a
 * statement of each would do */
#define DRAW_STRIP(quad, texture)                                              \
    do {                                                                       \
        njSetQuadTexture((texture), 0xffffffff);                               \
        (quad).v1 = UV(frame * STRIP_HEIGHT);                                  \
        (quad).v2 = UV((frame * STRIP_HEIGHT) + STRIP_HEIGHT),                 \
        njDrawQuadTexture(&(quad), FRONT_DEPTH);                               \
    } while (0)

void kinder_banner_draw(void)
{
    njSetTexture(&kinder_service_textures);
    njQuadTextureStart(FALSE);
    njSetQuadTexture(FRAME_TEXTURE, tint);
    njDrawQuadTexture((NJS_QUAD_TEXTURE *)&backdrop, BACK_DEPTH);
    njQuadTextureEnd();
    njQuadTextureStart(TRUE);
    njSetQuadTexture(FRAME_TEXTURE, tint);
    njDrawQuadTexture((NJS_QUAD_TEXTURE *)&left_cap, BACK_DEPTH);
    njDrawQuadTexture((NJS_QUAD_TEXTURE *)&right_cap, BACK_DEPTH);
    njSetQuadTextureColor(0xffffffff);
    njDrawQuadTexture((NJS_QUAD_TEXTURE *)&emblem, FRONT_DEPTH);
    if (game_state.text_language == TEXT_JAPANESE) {
        DRAW_STRIP(japanese_title, FIRST_STRIPS);
        DRAW_STRIP(japanese_caption, SECOND_STRIPS);
    } else {
        DRAW_STRIP(english_title, SECOND_STRIPS);
        DRAW_STRIP(english_caption, FIRST_STRIPS);
    }
    njQuadTextureEnd();
}
