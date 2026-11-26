#include <ninja.h>
#include <nj_texlist.h>
#include <sfdfx_mw.h>
#include <sg_mwply.h>
#include <sg_xpt.h>

#include "event/movie.h"
#include "event/scene.h"

#define MOVIE_PALETTE 48
#define PALETTE_SHIFT 21
#define PALETTE_MASK 0x07e00000

Bool event_movie_texture_active;
static struct event_movie_texture movie;

static void texture_parameters(NJS_TEXMEMLIST *entry, NJS_TEXSURFACE *surface)
{
    entry->bank = MOVIE_PALETTE;
    /* @hack spelling: the surface copied a field at a time, where
     * entry->texinfo.texsurface = *surface would do */
    entry->texinfo.texsurface.Type = surface->Type;
    entry->texinfo.texsurface.BitDepth = surface->BitDepth;
    entry->texinfo.texsurface.PixelFormat = surface->PixelFormat;
    entry->texinfo.texsurface.nWidth = surface->nWidth;
    entry->texinfo.texsurface.nHeight = surface->nHeight;
    entry->texinfo.texsurface.TextureSize = surface->TextureSize;
    entry->texinfo.texsurface.fSurfaceFlags = surface->fSurfaceFlags;
    entry->texinfo.texsurface.pSurface = surface->pSurface;
    entry->texinfo.texsurface.pVirtual = surface->pVirtual;
    entry->texinfo.texsurface.pPhysical = surface->pPhysical;
    njSetTextureParam(entry);
    if (surface->fSurfaceFlags & NJD_SURFACEFLAGS_PALETTIZED) {
        entry->texparambuffer = (entry->texparambuffer & ~PALETTE_MASK) |
                                (MOVIE_PALETTE << PALETTE_SHIFT);
    }
}

/* @hack type: surface taken as void * and converted through a local, where an
 * NJS_TEXSURFACE * parameter would do */
static void set_texture_frame(NJS_TEXLIST *textures, int index, void *surface)
{
    NJS_TEXMEMLIST frame;
    NJS_TEXSURFACE *source;
    NJS_TEXMEMLIST *target;
    source = surface;
    texture_parameters(&frame, source);
    target = (NJS_TEXMEMLIST *)textures->textures[index].texaddr;
    target->tspparambuffer = frame.tspparambuffer;
    target->texparambuffer = frame.texparambuffer;
}

void event_movie_transfer_texture(void)
{
    mwPlyExecTexSvr();
}

void event_movie_step_texture(void)
{
    MWS_PLY_MVFRM frame;
    MWE_PLY_STAT status;
    status = event_movie_player->vtbl->GetStat(event_movie_player);
    mwPlyExecTexSvr();
    mwPlyExecDrawSvr();
    if (status == MWE_PLY_STAT_PLAYEND || status == MWE_PLY_STAT_ERROR) {
        event_movie_player->vtbl->Stop(event_movie_player);
        event_movie_texture_active = FALSE;
    } else if (status >= MWE_PLY_STAT_PLAYING) {
        njSetTexture(event_scene_file->texlist);
        if (mwPlyGetMvFrm(event_movie_player, &frame) == 1) {
            set_texture_frame(event_scene_file->texlist, movie.texture,
                              frame.srf);
        }
    }
}

void event_movie_start_texture(struct event_movie_texture *request)
{
    /* @hack spelling: the flag compared with 0, where the flag alone would
     * do for a flag that holds TRUE or FALSE */
    if (event_movie_texture_active > 0) {
        return;
    }
    mwPlySetVideoSw(event_movie_player, FALSE);
    event_movie_texture_active = TRUE;
    movie = *request;
    mwPlySetCompoMode(event_movie_player, MWD_PLY_COMPO_OPEQ);
    event_movie_player->vtbl->StartFname(event_movie_player,
                                         (Sint8 *)request->name);
}
