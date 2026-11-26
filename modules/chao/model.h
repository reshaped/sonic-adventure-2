/* the textures and materials of the garden's chunk models, and copies of
 * chunk objects whose UVs scroll */
#ifndef CHAO_MODEL_H
#define CHAO_MODEL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the model of a Chao body's node. saved_textures keeps the indexes the
 * first texture change replaced, in order. a negative index marks an empty
 * slot. polygon_halfwords is chunk.plist's length, its end chunk included */
#define GARDEN_SAVED_TEXTURES 4
struct garden_model {
    NJS_CNK_MODEL chunk;
    Sint16 saved_textures[GARDEN_SAVED_TEXTURES];
    Sint32 polygon_halfwords;
};

/* a copy of source whose top model's UVs lie u and v past source's. u and v
 * count 256ths of the texture. each garden_uv_step moves u and v by du and dv,
 * and keeps them from 0 to 256 */
struct garden_uv_scroll {
    NJS_CNK_OBJECT *source, *copy;
    Float u, v, du, dv;
};

extern NJS_TEXLIST garden_animal_texlist, chao_body_texlist;
extern NJS_TEXLIST chao_eye_texlist, chao_mouth_texlist, chao_env_texlist;
extern NJS_TEXLIST chao_mini_parts_texlist, chao_jewel_texlist;
/* set each texture chunk of the model to texture's index, keeping the
 * chunk's NJD_FFM_MASK bits. where saved_textures' first slot is empty, the
 * indexes the call replaces fill its slots. garden_set_model_texture takes the
 * model of node, and changes nothing where node or its model is null */
void garden_set_model_texture(NJS_CNK_OBJECT *node, int texture);
void garden_model_set_texture(struct garden_model *model, int texture);
/* set each texture chunk of a scenery mesh, a plain NJS_CNK_MODEL, to
 * texture's index */
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side by
 * garden_animate_texture. that function passes the mesh's polygon list in a
 * garden_model of its own. the model's saved_textures, all 0, keep nothing */
void garden_animate_texture(NJS_CNK_MODEL *mesh, int texture);
#define GARDEN_ANIMATE_TEXTURE garden_animate_texture
#else
/* @bug memory: garden_model_set_texture reads saved_textures past the end
 * of a plain mesh, at 0x0c510130. where a slot there reads negative, it
 * writes the slot. a UV scroll's copy is such a mesh, which
 * garden_clone_object allocates at 0x0c510880 */
#define GARDEN_ANIMATE_TEXTURE(mesh, texture)                                  \
    garden_model_set_texture((struct garden_model *)(mesh), (texture))
#endif
/* set node's first count texture chunks to the indexes in textures, in
 * order, and save none. count must be 1 or more */
void garden_set_model_textures(NJS_CNK_OBJECT *node, const Sint16 *textures,
                               int count);
/* set node's texture chunk at index, counted from 0, to texture's index. a
 * shipped bug reads the second halfword of each texture chunk it skips as the
 * next chunk's head. a call past index 0 can then set the wrong chunk */
void garden_set_nth_texture(NJS_CNK_OBJECT *node, Uint16 index, int texture);
/* set the Chao's color to color, from 0 to 27. the color becomes the diffuse
 * of the nodes that take it */
void chao_set_body_color(struct task *chao, Sint16 color);
/* a jewel's texture, mapped as the environment, on the body's nodes that
 * take a material. the clear puts back the textures saved_textures keeps.
 * setting a jewel clears the shiny material */
void chao_set_jewel_texture(struct task *chao, int texture);
void chao_clear_jewel_texture(struct task *chao);
/* the shiny material on the body's nodes that take a material. Ninja then
 * draws each such node's list past its end, into the pass
 * chao_append_shiny_pass writes there. setting it clears the jewel */
void chao_set_shiny_material(struct task *chao);
void chao_clear_shiny_material(struct task *chao);
/* give the body its profile's color, where that is above 0. then give it
 * the shiny material where shiny is 1. otherwise a texture above 0 gives it
 * that jewel. a jewel with a color of its own sets that color by
 * chao_set_body_color. that call writes the profile's color too */
void chao_apply_body_material(struct task *chao);
/* copy source, its children and its siblings, each with its model and the
 * model's vertex and polygon lists. return the copy, or NULL for a null
 * source. a node that can't be allocated is left out, with the nodes that
 * hang from it. no other allocation is checked. a copy's model has an r of
 * 0. garden_free_object frees a copy */
NJS_CNK_OBJECT *garden_clone_object(NJS_CNK_OBJECT *source);
void garden_free_object(NJS_CNK_OBJECT *node);
/* make a scroll of a copy of object, which garden_uv_destroy frees. its
 * allocation is unchecked. garden_uv_step requires the strips of object's
 * model to give each vertex a u and a v. they may hold no user halfwords, as
 * NJD_CS_UVN's hold none */
struct garden_uv_scroll *garden_uv_create(NJS_CNK_OBJECT *object, Float du,
                                          Float dv);
void garden_uv_destroy(struct garden_uv_scroll *scroll);
void garden_uv_step(struct garden_uv_scroll *scroll);

#endif
