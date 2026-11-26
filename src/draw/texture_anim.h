/* texture animations, which rewrite a chunk model's texture coordinates and
 * textures in its polygon list */
#ifndef DRAW_TEXTURE_ANIM_H
#define DRAW_TEXTURE_ANIM_H
#include <ninja.h>
#include <sg_xpt.h>

/* the bits of texture_anim's flags */
#define TEXTURE_ANIM_BOUNCE 0x1 /* play forward, then back */
#define TEXTURE_ANIM_SCROLL 0x2 /* scroll uvs' first key, not play the keys */
/* show one key at a time, not a blend, where key_lengths is null */
#define TEXTURE_ANIM_STEPPED 0x4
/* changes nothing, since texture_anim_scroll's one test of it never holds */
#define TEXTURE_ANIM_UNK_8 0x8

/* a chunk model's texture animation. uvs, textures and cycles are lists of
 * shorts, each null for none, laid out as below. an index is a short's index
 * in the model's polygon list.
 *
 *   uvs       entries, keys, then each entry: an index, vertices, and for
 *             each key a u and a v for each vertex
 *   textures  entries, keys, then each entry: an index, and a texture for
 *             each key
 *   cycles    cycles, then each cycle: an index, a first texture, and
 *             textures
 *
 * a uvs entry's index is the short before its vertices. a vertex is three
 * shorts in the polygon list, and the animation writes the last two, its u
 * and v. a textures entry's texture goes at its index, as does a cycle's */
struct texture_anim {
    Uint32 flags;
    /* the frames of a play. under TEXTURE_ANIM_SCROLL, its high half is u's
     * step each frame, and its low half v's */
    Uint32 frames;
    Uint32 frame_time; /* the length of a frame in texture_anim_apply's time */
    /* null, or the length of the span from each of uvs' keys to the next. a
     * blend's place then counts in these lengths rather than in keys. it
     * still runs from 0 to the last key's number */
    Float *key_lengths;
    Sint16 *uvs;
    Sint16 *textures;
    Sint16 *cycles;
    /* the last scroll written, for texture_anim_scroll to skip a repeat of
     * it. the test that would do so never holds, so nothing writes them */
    NJS_CNK_MODEL *model;
    Sint16 u;
    Sint16 v;
};

/* write into model's polygon list what anim shows at time. the animation's
 * frame is time / frame_time. The port leaves the model unchanged if the
 * descriptor has no frame duration. A list plays only
 * where it isn't null. uvs must hold two keys or more, or one under
 * TEXTURE_ANIM_SCROLL. frames must not be 0 outside TEXTURE_ANIM_SCROLL, nor a
 * cycle's count of textures.
 *
 * cycles play under any flags. each writes its first texture plus the frame
 * modulo its count of textures. under TEXTURE_ANIM_BOUNCE each climbs to its
 * last texture and back down, showing each end for two frames.
 *
 * under TEXTURE_ANIM_SCROLL, texture_anim_scroll writes uvs' first key. it
 * scrolls u by frames' u step times the frame's low 9 bits. it scrolls v by its
 * v step times the same. textures doesn't play.
 *
 * otherwise a play lasts frames frames. a play's phase is the share of its
 * frames shown so far, from 0 toward 1. under TEXTURE_ANIM_BOUNCE a second play
 * takes the phase back down to 0. keys is uvs' count of keys, or textures'
 * where uvs is null.
 *
 * under TEXTURE_ANIM_STEPPED, the key is phase * keys, truncated and held at
 * the last key. uvs' coordinates are that key's. otherwise texture_anim_blend
 * writes uvs' coordinates at the place phase * (keys - 1). the key is then the
 * first of the two keys blended, or without uvs, the place truncated. textures
 * writes each entry's texture at the key, held at its own last key.
 *
 * with key_lengths, the coordinates come from a blend whose place counts in the
 * lengths. a stepped key's number is such a place too. stepped key k thus shows
 * key k's own coordinates only where the first k lengths sum to k */
void texture_anim_apply(NJS_CNK_MODEL *model, struct texture_anim *anim,
                        Uint32 time);
/* write into model's polygon list uvs' coordinates at place. place is a key's
 * number with a fraction, held from 0 to the last key. each coordinate blends
 * its values at the two keys about place by place's fraction. the blend is
 * truncated toward 0. uvs must hold two keys or more, and key_lengths is not
 * read. return the first of the two keys, which is never the last key */
Uint32 texture_anim_blend(NJS_CNK_MODEL *model, struct texture_anim *anim,
                          Float place);
/* write into model's polygon list the coordinates of uvs' first key, each u
 * moved by scroll_u and each v by scroll_v */
void texture_anim_scroll(NJS_CNK_MODEL *model, struct texture_anim *anim,
                         Sint16 scroll_u, Sint16 scroll_v);
/* a wrap for texture_anim_scroll's scrolls, in the polygon list's uv units.
 * under NJD_CS_UVN, whose u and v count 256ths of a texture, it is two
 * textures across, so a scroll that wraps there shows no jump */
#define SCROLL_WRAP 0x200

#endif
