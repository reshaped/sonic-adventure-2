/* the cart objects' texture remaps. a remap lists a model's texture chunks,
 * with the addresses of their texture words and UVs. cart_texture_remaps
 * lists the remaps, up to one with a null object */
#ifndef CART_TEXTURE_REMAP_H
#define CART_TEXTURE_REMAP_H
#include <ninja.h>
#include <sg_xpt.h>

/* a strip's UV, and its value as the model holds it */
struct cart_uv_patch {
    NJS_TEX *target;
    NJS_TEX original;
};
/* an NJD_CT_TID chunk. texture is its texture as the model holds it, and
 * texture_word its second halfword. uv holds the UVs of the strips it
 * textures */
struct cart_texture_patch {
    Sint32 texture;
    Uint16 *texture_word;
    Sint32 uv_count;
    struct cart_uv_patch *uv;
};
struct cart_texture_remap {
    NJS_CNK_OBJECT *object;
    Sint32 count;
    struct cart_texture_patch **patches;
};

extern struct cart_texture_remap cart_texture_remaps[];
/* CART_SCROLL_MASK wraps a scroll's offset after 64 of a texture's spans of 256
 * UV units. the wrap thus doesn't show */
#define CART_SCROLL_MASK 0x3fff
/* each call takes the remaps of object in table, up to table's null object.
 * it finds a chunk by the texture the model holds, whatever an earlier call
 * put in its place. each returns how many texture words or UVs it set.
 *
 * cart_replace_texture gives each chunk of original the texture replacement.
 * it keeps the chunk's NJD_FFM_MASK and clears its NJD_FSS.
 * cart_scroll_texture sets each UV of the chunks of texture to its original
 * plus offset. cart_scroll_selected sets only the UVs whose original select
 * takes, and calls select for each UV in table order */
int cart_replace_texture(struct cart_texture_remap *table,
                         NJS_CNK_OBJECT *object, int original, int replacement);
int cart_scroll_texture(struct cart_texture_remap *table,
                        NJS_CNK_OBJECT *object, int texture,
                        const NJS_TEX *offset);
int cart_scroll_selected(struct cart_texture_remap *table,
                         NJS_CNK_OBJECT *object, int texture,
                         const NJS_TEX *offset,
                         Bool (*select)(const NJS_TEX *));
/* the node of root's tree whose child is target, or with sibling set, whose
 * sibling is target. the search takes each node's child before its sibling.
 * null where no node links to target, or where target is root */
NJS_CNK_OBJECT *cart_find_link(Bool sibling, NJS_CNK_OBJECT *root,
                               NJS_CNK_OBJECT *target);
#endif
