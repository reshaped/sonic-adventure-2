/* The public texture setter updates the Kamui context only on selection. */
#include <assert.h>
#include <kamui.h>
#include <nj_km.h>
#include <ninja.h>
#include <stdint.h>

NJS_TEXLIST *_nj_current_texlist;
Uint32 _nj_current_texture;
Uint32 _nj_current_texture_bank;
Sint32 _nj_texglobal_color_;
static KMVERTEXCONTEXT context;
KMVERTEXCONTEXT *_nj_current_ctx_ = &context;
static unsigned int updates;
/* The records the two names' texaddr words point to. They lie in the
 * program's data, which a position-dependent ELF link places below 4 GB. */
static NJS_TEXMEMLIST first;
static NJS_TEXMEMLIST next;

KMSTATUS kmSetVertexRenderState(PKMVERTEXCONTEXT state)
{
    assert(state == &context);
    updates++;
    return KMSTATUS_SUCCESS;
}

/* Returns the texaddr word that points to record. The word has the SH-4's
 * 32 bits, so record's address must fit in them. */
static Uint32 texaddr(const NJS_TEXMEMLIST *record)
{
    uintptr_t address = (uintptr_t)record;

    assert((Uint32)address == address);
    return (Uint32)address;
}

static void expect_previous_material(void)
{
    assert(_nj_current_texture == 7);
    assert(_nj_current_texture_bank == 2);
    assert(_nj_texglobal_color_ == 3);
    assert(context.TSPPARAMBUFFER == 0x12340015);
    assert(context.TexturePARAMBUFFER == 0x11111111);
    assert(updates == 1);
}

int main(void)
{
    NJS_TEXNAME slots[2] = {{0}};
    NJS_TEXLIST list = {slots, 2};
    NJS_TEXLIST empty = {NULL, 0};

    first.globalIndex = 7;
    first.bank = 2;
    first.texinfo.texsurface.Type = 3UL << TEXSURFACE_FORMAT_SHIFT;
    first.tspparambuffer = 0x15;
    first.texparambuffer = 0x11111111;
    slots[0].texaddr = texaddr(&first);
    context.TSPPARAMBUFFER = 0x12340000;
    assert(njSetTextureNum(0) == -1);
    assert(updates == 0);
    assert(njSetTexture(&list) == 1);
    assert(njSetTextureNum(0) == 1);
    expect_previous_material();

    assert(njSetTexture(&empty) == 1);
    assert(njSetTextureNum(0) == -1);
    expect_previous_material();
    empty.nbTexture = 2;
    assert(njSetTextureNum(1) == -1);
    expect_previous_material();
    assert(njSetTexture(&list) == 1);
    assert(njSetTextureNum(2) == -1);
    assert(njSetTextureNum(1) == -1);
    expect_previous_material();

    next.globalIndex = 7;
    next.bank = 3;
    next.texinfo.texsurface.Type = 5UL << TEXSURFACE_FORMAT_SHIFT;
    next.tspparambuffer = 0x2a;
    next.texparambuffer = 0x22222222;
    slots[1].texaddr = texaddr(&next);
    assert(njSetTextureNum(1) == 1);
    assert(_nj_current_texture == 7);
    assert(_nj_current_texture_bank == 3);
    assert(_nj_texglobal_color_ == 5);
    assert(context.TSPPARAMBUFFER == 0x1234002a);
    assert(context.TexturePARAMBUFFER == 0x22222222);
    assert(updates == 2);
    assert(njSetTextureNum(1) == 1);
    assert(updates == 2);
    return 0;
}
