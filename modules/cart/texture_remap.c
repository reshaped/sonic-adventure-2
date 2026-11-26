/* the calls that retexture and scroll the cart objects' models through their
 * remaps, and cart_find_link */
#include <ninja.h>
#include <sg_xpt.h>
#include "cart/texture_remap.h"

int cart_replace_texture(struct cart_texture_remap *table,
                         NJS_CNK_OBJECT *object, int original, int replacement)
{
    int i;
    int changed = 0;
    while (table->object) {
        if (table->object == object) {
            for (i = 0; i < table->count; i++) {
                if (table->patches[i]->texture == original) {
                    *table->patches[i]->texture_word =
                        (*table->patches[i]->texture_word & NJD_FFM_MASK) +
                        replacement;
                    changed++;
                }
            }
        }
        table++;
    }
    return changed;
}

int cart_scroll_texture(struct cart_texture_remap *table,
                        NJS_CNK_OBJECT *object, int texture,
                        const NJS_TEX *offset)
{
    /* @hack spelling: remap, a copy of table that steps in its place, where
     * table stepped itself would do */
    struct cart_texture_remap *remap = table;
    int i;
    int j;
    int changed = 0;
    struct cart_uv_patch *uv;
    while (remap->object) {
        if (remap->object == object) {
            for (i = 0; i < remap->count; i++) {
                if (remap->patches[i]->texture == texture) {
                    uv = remap->patches[i]->uv;
                    for (j = 0; j < remap->patches[i]->uv_count; j++) {
                        uv[j].target->u = uv[j].original.u + offset->u;
                        uv[j].target->v = uv[j].original.v + offset->v;
                        changed++;
                    }
                }
            }
        }
        remap++;
    }
    return changed;
}
int cart_scroll_selected(struct cart_texture_remap *table,
                         NJS_CNK_OBJECT *object, int texture,
                         const NJS_TEX *offset, Bool (*select)(const NJS_TEX *))
{
    /* @hack spelling: remap, a copy of table that steps in its place, where
     * table stepped itself would do */
    struct cart_texture_remap *remap = table;
    int i;
    int j;
    int changed = 0;
    struct cart_uv_patch *uv;
    while (remap->object) {
        if (remap->object == object) {
            for (i = 0; i < remap->count; i++) {
                if (remap->patches[i]->texture == texture) {
                    uv = remap->patches[i]->uv;
                    for (j = 0; j < remap->patches[i]->uv_count; j++) {
                        if (select(&uv[j].original)) {
                            uv[j].target->u = uv[j].original.u + offset->u;
                            uv[j].target->v = uv[j].original.v + offset->v;
                            changed++;
                        }
                    }
                }
            }
        }
        remap++;
    }
    return changed;
}

static NJS_CNK_OBJECT *find_link(Bool sibling, NJS_CNK_OBJECT *root,
                                 NJS_CNK_OBJECT *target);

NJS_CNK_OBJECT *cart_find_link(Bool sibling, NJS_CNK_OBJECT *root,
                               NJS_CNK_OBJECT *target)
{
    if (root == target) {
        return NULL;
    }
    return find_link(sibling, root, target);
}
static NJS_CNK_OBJECT *find_link(Bool sibling, NJS_CNK_OBJECT *root,
                                 NJS_CNK_OBJECT *target)
{
    NJS_CNK_OBJECT *found;
    if (sibling) {
        if (root->sibling == target) {
            return root;
        }
    } else {
        if (root->child == target) {
            return root;
        }
    }
    if (root->child) {
        if ((found = find_link(sibling, root->child, target))) {
            return found;
        }
    }
    if (root->sibling) {
        if ((found = find_link(sibling, root->sibling, target))) {
            return found;
        }
    }
    return NULL;
}
