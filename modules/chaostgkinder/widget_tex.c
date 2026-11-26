#include <ninja.h>
#include "chaostgkinder/widget.h"
#include "system/texture.h"

#define WIDGET_TEXTURES 2

static NJS_TEXNAME textures[WIDGET_TEXTURES];
NJS_TEXLIST kinder_widget_texlist = {textures, WIDGET_TEXTURES};

void kinder_widget_textures_load(void)
{
    texture_load_pvm("net_ad_tex", &kinder_widget_texlist);
}

void kinder_widget_textures_release(void)
{
    njReleaseTexture(&kinder_widget_texlist);
}
