/* texture animations, which rewrite a chunk model's texture coordinates and
 * textures in its polygon list */
#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

/* the shorts of a vertex's coordinates in uvs: its u and its v */
#define UV_SHORTS 2

#define SCROLL_U_SHIFT 16
#define SCROLL_FRAME_MASK 0x1ff

static Uint32 texture_anim_blend_lengths(NJS_CNK_MODEL *model,
                                         struct texture_anim *anim,
                                         Float place);
static void texture_anim_textures(NJS_CNK_MODEL *model,
                                  struct texture_anim *anim, int key);
static void texture_anim_cycle(NJS_CNK_MODEL *model, struct texture_anim *anim,
                               Uint32 frame);

/* nothing calls these. neither is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void fun_8c060be0(void);
/* write into model's polygon list uvs' coordinates at key, held at the last
 * key. return the key written */
Uint32 texture_anim_key(NJS_CNK_MODEL *model, struct texture_anim *anim,
                        Uint32 key);
/* NOLINTEND(misc-use-internal-linkage) */

void fun_8c060be0(void)
{
}

void texture_anim_apply(NJS_CNK_MODEL *model, struct texture_anim *anim,
                        Uint32 time)
{
    Uint32 frames;
    Uint32 flags = anim->flags;
    Uint32 frame;
    Uint32 step;
    Uint32 keys;
    Float length;
    Float phase;
    int key = -1;
    Uint16 scroll_u;
    Uint16 scroll_v;

#ifdef NONMATCHING
    /* A layout-only descriptor has no frame duration or UV keys. */
    if (anim->frame_time == 0) {
        return;
    }
#endif
    /* @hack spelling: length set from anim->frames ahead of frames, where
     * frames would do */
    length = anim->frames;
    frames = anim->frames;
    frame = time / anim->frame_time;
    if (anim->cycles != NULL) {
        texture_anim_cycle(model, anim, frame);
    }
    if (anim->textures == NULL && anim->uvs == NULL) {
        return;
    }
    keys = anim->uvs != NULL ? anim->uvs[1] : anim->textures[1];
    if (!(flags & TEXTURE_ANIM_SCROLL)) {
        if (flags & TEXTURE_ANIM_BOUNCE) {
            /* @hack spelling: step set and doubled in statements of their
             * own, where frame % (frames * 2) would do */
            step = frames;
            step = step * 2;
            step = frame % step;
            phase = (Float)step / length;
            if (phase > 1.0F) {
                phase = 2.0F - phase;
            }
        } else {
            step = frame % frames;
            phase = (Float)step / length;
        }
        if (flags & TEXTURE_ANIM_STEPPED) {
            length = keys;
            phase *= length;
            key = phase;
            if ((Uint32)key >= keys) {
                /* @hack spelling: key set to keys and stepped back, where
                 * key = keys - 1 would do */
                key = keys;
                key--;
            }
            if (anim->uvs != NULL) {
                if (anim->key_lengths != NULL) {
                    texture_anim_blend_lengths(model, anim, key);
                } else {
                    texture_anim_blend(model, anim, key);
                }
            }
        } else {
            keys--;
            length = keys;
            phase *= length;
            if (anim->uvs != NULL) {
                if (anim->key_lengths != NULL) {
                    key = texture_anim_blend_lengths(model, anim, phase);
                } else {
                    key = texture_anim_blend(model, anim, phase);
                }
            }
        }
        if (anim->textures != NULL) {
            if (key < 0) {
                key = phase;
                if (key > length) {
                    key = length;
                }
            }
            texture_anim_textures(model, anim, key);
        }
    } else {
        /* @hack spelling: frame & SCROLL_FRAME_MASK written in each product,
         * where one local would do */
        scroll_u = (frame & SCROLL_FRAME_MASK) * (frames >> SCROLL_U_SHIFT);
        scroll_v = (frame & SCROLL_FRAME_MASK) * frames;
        if (anim->uvs != NULL) {
            texture_anim_scroll(model, anim, scroll_u, scroll_v);
        }
    }
}

Uint32 texture_anim_key(NJS_CNK_MODEL *model, struct texture_anim *anim,
                        Uint32 key)
{
    Sint16 *data = anim->uvs;
    int count = *data++;
    int keys = *data++;
    int i;
    int size;
    Sint16 *dst;
    Sint16 *src;
    Sint16 *end;

    if (key >= (Uint32)keys) {
        key = keys - 1;
    }
    for (i = 0; i < count; i++) {
        dst = &model->plist[*data++ + 1];
        size = *data++ * UV_SHORTS;
        src = data + (key * size);
        end = src + size - UV_SHORTS;
        data += keys * size;
        while (end > src) {
            prefetch(src + UV_SHORTS);
            dst++;
            *dst++ = *src++;
            *dst++ = *src++;
            dst++;
            *dst++ = *src++;
            *dst++ = *src++;
        }
        end += UV_SHORTS;
        while (end > src) {
            dst++;
            *dst++ = *src++;
            *dst++ = *src++;
        }
    }
    return key;
}

Uint32 texture_anim_blend(NJS_CNK_MODEL *model, struct texture_anim *anim,
                          Float place)
{
    Sint16 *data = anim->uvs;
    int count = *data++;
    int keys = *data++;
    int i;
    int size;
    Uint32 key;
    Float rest;
    Float fraction;
    /* @hack redundant: register on dst, src, next and end */
    register Sint16 *dst;
    register Sint16 *src;
    register Sint16 *next;
    register Sint16 *end;

    if (place < 0.0F) {
        place = 0.0F;
    }
    if (place > keys - 1) {
        place = keys - 1;
    }
    key = place;
    if (key >= (Uint32)(keys - 1)) {
        key = keys - 2;
    }
    fraction = place - key;
    rest = 1.0F - fraction;
    for (i = 0; i < count; i++) {
        dst = &model->plist[*data++ + 1];
        size = *data++ * UV_SHORTS;
        src = data + (key * size);
        end = src + size;
        next = end;
        end -= UV_SHORTS;
        data += keys * size;
        while (end > src) {
            prefetch(src + UV_SHORTS);
            dst++;
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            dst++;
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            *dst++ = (*src++ * rest) + (*next++ * fraction);
        }
        end += UV_SHORTS;
        while (end > src) {
            dst++;
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            *dst++ = (*src++ * rest) + (*next++ * fraction);
        }
    }
    return key;
}

/* texture_anim_blend, with place counted in key_lengths: the span from key
 * j to the next is key_lengths[j] long. place is held from 0 to keys - 1
 * first, as texture_anim_blend holds it */
static Uint32 texture_anim_blend_lengths(NJS_CNK_MODEL *model,
                                         struct texture_anim *anim, Float place)
{
    Sint16 *data = anim->uvs;
    Float *lengths = anim->key_lengths;
    int count = *data++;
    int keys = *data++;
    int i;
    int j;
    int size;
    Uint32 key;
    Float sum;
    Float length;
    Float rest;
    /* @hack redundant: register on fraction, dst, src, next and end */
    register Float fraction;
    register Sint16 *dst;
    register Sint16 *src;
    register Sint16 *next;
    register Sint16 *end;

    if (place < 0.0F) {
        place = 0.0F;
    }
    if (place > keys - 1) {
        place = keys - 1;
    }
    sum = 0.0F;
    for (j = 0; j < keys - 1; j++) {
        length = *lengths++;
        sum += length;
        if (sum > place) {
            /* @hack spelling: the key and its fraction in steps of their
             * own, where sum = j + (1.0F - ((sum - place) / length)) would
             * do */
            fraction = sum - place;
            fraction /= length;
            sum = j;
            fraction = 1.0F - fraction;
            sum += fraction;
            break;
        }
    }
    key = sum;
    if (key >= (Uint32)(keys - 1)) {
        key = keys - 2;
        fraction = 1.0F;
        rest = 0.0F;
    } else {
        fraction = sum - key;
        rest = 1.0F - fraction;
    }
    for (i = 0; i < count; i++) {
        dst = &model->plist[*data++ + 1];
        size = *data++ * UV_SHORTS;
        src = data + (key * size);
        end = src + size;
        next = end;
        end -= UV_SHORTS;
        data += keys * size;
        while (end > src) {
            prefetch(src + UV_SHORTS);
            dst++;
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            dst++;
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            *dst++ = (*src++ * rest) + (*next++ * fraction);
        }
        end += UV_SHORTS;
        while (end > src) {
            dst++;
            *dst++ = (*src++ * rest) + (*next++ * fraction);
            *dst++ = (*src++ * rest) + (*next++ * fraction);
        }
    }
    return key;
}

void texture_anim_scroll(NJS_CNK_MODEL *model, struct texture_anim *anim,
                         Sint16 scroll_u, Sint16 scroll_v)
{
    Sint16 *data = anim->uvs;
    int count = *data++;
    int keys = *data++;
    int i;
    int size;
    /* @hack redundant: register on dst, src, end, du and dv */
    register Sint16 *dst;
    register Sint16 *src;
    register Sint16 *end;
    register Sint16 du;
    register Sint16 dv;

    /* @bug logic: the test never holds, so the scroll is written every call:
     * !anim->flags is 0 or 1, and & TEXTURE_ANIM_UNK_8 leaves 0 of either
     * (MOVT at 0x8c0611f0, TST #8 at 0x8c0611f4) */
#ifdef NONMATCHING
    /* this side keeps the tag's result. the parentheses mark the ! as meant,
     * which -Wlogical-not-parentheses asks for */
    if ((!anim->flags) & TEXTURE_ANIM_UNK_8) {
#else
    if (!anim->flags & /* NOLINT(clang-diagnostic-logical-not-parentheses) */
        TEXTURE_ANIM_UNK_8) {
#endif
        if (scroll_u == anim->u && scroll_v == anim->v &&
            model == anim->model) {
            return;
        }
        anim->u = scroll_u;
        anim->v = scroll_v;
        anim->model = model;
    }
    /* @hack spelling: du and dv, copies of scroll_u and scroll_v, where the
     * parameters would do */
    du = scroll_u;
    dv = scroll_v;
    for (i = 0; i < count; i++) {
        dst = &model->plist[*data++ + 1];
        size = *data++ * UV_SHORTS;
        src = data;
        end = src + size - UV_SHORTS;
        data += keys * size;
        while (end > src) {
            prefetch(src + UV_SHORTS);
            dst++;
            *dst++ = *src++ + du;
            *dst++ = *src++ + dv;
            dst++;
            *dst++ = *src++ + du;
            *dst++ = *src++ + dv;
        }
        end += UV_SHORTS;
        while (end > src) {
            dst++;
            *dst++ = *src++ + du;
            *dst++ = *src++ + dv;
        }
    }
}

/* write into model's polygon list each entry's texture at key, held at the
 * last key */
static void texture_anim_textures(NJS_CNK_MODEL *model,
                                  struct texture_anim *anim, int key)
{
    Sint16 *plist = model->plist;
    Sint16 *data = anim->textures;
    int count = *data++;
    int keys = *data++;
    /* @hack redundant: register on i, at, index and texture */
    register int i;
    register int at;
    register int index;
    register Sint16 texture;

    if (key >= keys) {
        key = keys - 1;
    }
    key++;
    for (i = 0; i < count; i++) {
        at = i * (keys + 1);
        /* @hack spelling: texture and index, locals for the entry's texture
         * and its index, where plist[data[at]] = data[at + key] would do */
        texture = data[at + key];
        index = data[at];
        plist[index] = texture;
    }
}

/* write into model's polygon list each cycle's texture at frame */
static void texture_anim_cycle(NJS_CNK_MODEL *model, struct texture_anim *anim,
                               Uint32 frame)
{
    Sint16 *data = anim->cycles;
    int cycles = *data++;
    Sint16 *plist = model->plist;
    /* @hack spelling: flags read after plist, where first would do */
    /* @hack redundant: register on flags, i, index, texture, textures and
     * step */
    register Uint32 flags = anim->flags;
    register int i;
    register int index;
    register Sint16 texture;
    register Sint16 textures;
    register Sint16 step;

    for (i = 0; i < cycles; i++) {
        index = *data++;
        texture = *data++;
        if (flags & TEXTURE_ANIM_BOUNCE) {
            textures = *data++;
            step = frame % (textures * 2);
            if (step >= textures) {
                step = (textures * 2) - step - 1;
            }
            texture += step;
        } else {
            texture += frame % *data++;
        }
        plist[index] = texture;
    }
}
