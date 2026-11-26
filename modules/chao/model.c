#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/body.h"
#include "chao/model.h"
#include "chao/work.h"
#include "system/task.h"

/* the bits of a texture chunk's second halfword that hold its index */
#define TEXTURE_INDEX_MASK (NJD_FSS - 1)
/* the bits of a strip chunk's third halfword that hold its count of strips */
#define STRIP_COUNT_MASK ((1 << NJD_UFO_SHIFT) - 1)
/* u and v count 256ths of the texture */
#define UV_RANGE 256.0F

/* the diffuse of each body color, by chao_parameters.color */
static const Uint32 body_palette[] = {
    0xffffffff, 0xff3f3f3f, 0xff7f7f7f, 0xffffffff, 0xff7f7fff, 0xff7fff7f,
    0xffff7f7f, 0xffff0000, 0xff007fff, 0xff00c84c, 0xffff00c8, 0xffffc800,
    0xff00c8ff, 0xffb080ff, 0xffff0000, 0xffff7f7f, 0xffff7f00, 0xffffff00,
    0xff00bb00, 0xff22ff22, 0xff0000ff, 0xff5555ff, 0xffff00ff, 0xffff66ff,
    0xffcc4400, 0xff333333, 0xff888888, 0xffffffff};

/* the nodes whose models take the body's color, and those whose models take
 * a material. each node is an index into the shape's nodes. each list's
 * indexes rise, and NODE_LIST_END ends it */
#define NODE_LIST_END (-1)
static Sint16 colored_nodes[] = {1,  3,  6,  8,  10, 13, 16, 19,
                                 22, 23, 24, 25, 26, 30, 31, NODE_LIST_END};
static Sint16 material_nodes[] = {1,  3,  6,  8,  10, 13,
                                  16, 19, 22, 23, 24, 25,
                                  26, 30, 31, 37, 39, NODE_LIST_END};

/* each jewel's texture and body color, by chao_parameters.texture. a jewel
 * whose color is NO_JEWEL_COLOR leaves the body's own */
#define NO_JEWEL_COLOR 12
/* clang-format off */
static Sint16 jewel_textures[] = {9, 7, 8, 9, 9, 9, 9, 9,
                                  3, 6, 4, 5, 1, 9, 0, 9};
static Sint16 jewel_colors[] = {
    NO_JEWEL_COLOR, 3, 3, 7, 9, 8, 10, 11,
    3, 3, 3, 3, NO_JEWEL_COLOR, NO_JEWEL_COLOR, NO_JEWEL_COLOR, NO_JEWEL_COLOR};
/* clang-format on */

/* step past a material chunk of type. step2, step4 and step6 each step past
 * 2, 4 or 6 halfwords. the chunk's head and size take 2, and each color its
 * type sets takes 2 more */
#define STEP_MATERIAL(type, step2, step4, step6)                               \
    {                                                                          \
        (step2);                                                               \
        switch (type) {                                                        \
        case NJD_CM_D:                                                         \
        case NJD_CM_A:                                                         \
        case NJD_CM_S:                                                         \
            (step2);                                                           \
            break;                                                             \
        case NJD_CM_DA:                                                        \
        case NJD_CM_DS:                                                        \
        case NJD_CM_AS:                                                        \
            (step4);                                                           \
            break;                                                             \
        case NJD_CM_DAS:                                                       \
            (step6);                                                           \
        }                                                                      \
    }

#define STEP_CURSOR(halfwords) (cursor += (halfwords))

/* step past a material or strip chunk, any chunk from NJD_MATOFF up. a strip
 * chunk's size halfword counts the halfwords after it. the end chunk returns
 * from the function. a vertex or volume chunk takes no step, so a list must
 * hold none */
#define SKIP_MATERIAL_OR_STRIP(type, cursor, length)                           \
    {                                                                          \
        if ((type) < NJD_VERTOFF) {                                            \
            STEP_MATERIAL((type), STEP_CURSOR(2), STEP_CURSOR(4),              \
                          STEP_CURSOR(6));                                     \
        } else if ((type) < NJD_VOLOFF) {                                      \
        } else if ((type) < NJD_STRIPOFF) {                                    \
        } else if ((type) >= NJD_CE) {                                         \
            return;                                                            \
        } else {                                                               \
            (cursor)++;                                                        \
            (length) = *(cursor);                                              \
            (cursor) += (Uint16)((length) + 1);                                \
        }                                                                      \
    }

/* node's polygon list must open with a material chunk whose first color is
 * its diffuse */
static void set_diffuse(NJS_CNK_OBJECT *node, Uint32 color)
{
    Sint16 *material;

    if (node && node->model) {
        material = node->model->plist;
        if (material) {
            material += 2;
            *(Uint32 *)material = color;
        }
    }
}

void chao_set_body_color(struct task *chao, Sint16 color)
{
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    struct chao_shape_work *shape = &work->shape;
    NJS_CNK_OBJECT **node = shape->nodes;
    struct chao_parameters *parameters = &work->profile.parameters;
    const Sint16 *list = colored_nodes;
    Uint32 diffuse = body_palette[color];
    int index;

    parameters->color = color;
    for (index = 0; index < CHAO_BODY_NODES; index++, node++) {
        if (*list == NODE_LIST_END) {
            break;
        }
        if (*list == index) {
            set_diffuse(*node, diffuse);
            list++;
        }
    }
}

void garden_set_model_texture(NJS_CNK_OBJECT *node, int texture)
{
    if (!node || !node->model) {
        return;
    }
    garden_model_set_texture((struct garden_model *)node->model, texture);
}

void garden_model_set_texture(struct garden_model *model, int texture)
{
    Sint16 *cursor;
    int saved = 0;
    Uint8 type;
    Sint16 texture_id;
    Sint16 value;

    if (!model) {
        return;
    }
    cursor = model->chunk.plist;
    if (!cursor) {
        return;
    }
    texture_id = texture & TEXTURE_INDEX_MASK;
    for (;;) {
        type = *cursor;
        /* @hack redundant: null and bits chunks tested apart, where one test
         * below NJD_TINYOFF would do */
        if (type < NJD_CB_BA) {
            cursor++;
        } else if (type < NJD_TINYOFF) {
            cursor++;
        } else if (type < NJD_MATOFF) {
            if (type == NJD_CT_TID) {
                cursor++;
                value = *cursor;
                /* @hack spelling: the slot's bound and its contents tested in
                 * nested ifs, where one test with && would do */
                if (saved < GARDEN_SAVED_TEXTURES) {
                    if (model->saved_textures[saved] < 0) {
                        model->saved_textures[saved] =
                            value & TEXTURE_INDEX_MASK;
                        saved++;
                    }
                }
                value &= NJD_FFM_MASK;
                value |= texture_id;
                *cursor = value;
                cursor++;
            } else {
                cursor += 2;
            }
        } else {
            SKIP_MATERIAL_OR_STRIP(type, cursor, value);
        }
    }
}

#ifdef NONMATCHING
/* this side's GARDEN_ANIMATE_TEXTURE, as model.h says */
void garden_animate_texture(NJS_CNK_MODEL *mesh, int texture)
{
    struct garden_model material = {0};

    if (mesh) {
        material.chunk.plist = mesh->plist;
        garden_model_set_texture(&material, texture);
    }
}
#endif

static void restore_textures(struct garden_model *model);

static void restore_node_textures(NJS_CNK_OBJECT *node)
{
    if (!node || !node->model) {
        return;
    }
    restore_textures((struct garden_model *)node->model);
}

static void restore_textures(struct garden_model *model)
{
    Sint16 *cursor;
    int saved = 0;
    Uint8 type;
    Sint16 value;

    if (!model) {
        return;
    }
    cursor = model->chunk.plist;
    if (!cursor) {
        return;
    }
    for (;;) {
        type = *cursor;
        /* @hack redundant: null and bits chunks tested apart, where one test
         * below NJD_TINYOFF would do */
        if (type < NJD_CB_BA) {
            cursor++;
        } else if (type < NJD_TINYOFF) {
            cursor++;
        } else if (type < NJD_MATOFF) {
            if (type == NJD_CT_TID) {
                cursor++;
                value = *cursor;
                /* @hack spelling: the slot's bound and its contents tested in
                 * nested ifs, where one test with && would do */
                if (saved < GARDEN_SAVED_TEXTURES) {
                    if (model->saved_textures[saved] >= 0) {
                        value &= NJD_FFM_MASK;
                        value |=
                            model->saved_textures[saved] & TEXTURE_INDEX_MASK;
                        saved++;
                    }
                }
                *cursor = value;
                cursor++;
            } else {
                cursor += 2;
            }
        } else {
            SKIP_MATERIAL_OR_STRIP(type, cursor, value);
        }
    }
}

static void set_textures(struct garden_model *model, const Sint16 *textures,
                         int count);

void garden_set_model_textures(NJS_CNK_OBJECT *node, const Sint16 *textures,
                               int count)
{
    if (!node || !node->model) {
        return;
    }
    set_textures((struct garden_model *)node->model, textures, count);
}

static void set_textures(struct garden_model *model, const Sint16 *textures,
                         int count)
{
    Sint16 *cursor;
    Uint8 type;
    Sint16 value;

    if (!model) {
        return;
    }
    cursor = model->chunk.plist;
    if (!cursor) {
        return;
    }
    for (;;) {
        type = *cursor;
        /* @hack redundant: null and bits chunks tested apart, where one test
         * below NJD_TINYOFF would do */
        if (type < NJD_CB_BA) {
            cursor++;
        } else if (type < NJD_TINYOFF) {
            cursor++;
        } else if (type < NJD_MATOFF) {
            if (type == NJD_CT_TID) {
                cursor++;
                value = *cursor;
                value &= NJD_FFM_MASK;
                value |= *textures & TEXTURE_INDEX_MASK;
                textures++;
                *cursor = value;
                count--;
                if (count == 0) {
                    return;
                }
                cursor++;
            } else {
                cursor += 2;
            }
        } else {
            SKIP_MATERIAL_OR_STRIP(type, cursor, value);
        }
    }
}

static void set_nth_texture(struct garden_model *model, Uint16 index,
                            int texture);

void garden_set_nth_texture(NJS_CNK_OBJECT *node, Uint16 index, int texture)
{
    if (!node || !node->model) {
        return;
    }
    set_nth_texture((struct garden_model *)node->model, index, texture);
}

static void set_nth_texture(struct garden_model *model, Uint16 index,
                            int texture)
{
    Sint16 *cursor;
    Uint8 type;
    Sint16 value;
    Sint16 texture_id;

    if (!model) {
        return;
    }
    cursor = model->chunk.plist;
    if (!cursor) {
        return;
    }
    texture_id = texture & TEXTURE_INDEX_MASK;
    for (;;) {
        type = *cursor;
        /* @hack redundant: null and bits chunks tested apart, where one test
         * below NJD_TINYOFF would do */
        if (type < NJD_CB_BA) {
            cursor++;
        } else if (type < NJD_TINYOFF) {
            cursor++;
        } else if (type < NJD_MATOFF) {
            if (type == NJD_CT_TID) {
                if (index == 0) {
                    cursor++;
                    value = *cursor;
                    value &= NJD_FFM_MASK;
                    value |= texture_id;
                    *cursor = value;
                    return;
                }
                /* @bug logic: skipping a texture chunk moves the cursor one
                 * halfword at 0x0c510492, where the chunk takes two. the walk
                 * at 0x0c510462 then reads the chunk's second halfword as the
                 * next chunk's head */
                index--;
                cursor++;
            } else {
                cursor += 2;
            }
        } else {
            SKIP_MATERIAL_OR_STRIP(type, cursor, value);
        }
    }
}

static void set_environment(struct garden_model *model, int enabled);

static void set_node_environment(NJS_CNK_OBJECT *node, int enabled)
{
    if (!node || !node->model) {
        return;
    }
    set_environment((struct garden_model *)node->model, enabled);
}

static void set_environment(struct garden_model *model, int enabled)
{
    Sint16 *cursor;
    Uint8 type;
    Sint16 length;

    if (!model) {
        return;
    }
    cursor = model->chunk.plist;
    if (!cursor) {
        return;
    }
    for (;;) {
        type = *cursor;
        /* @hack redundant: null and bits chunks tested apart, where one test
         * below NJD_TINYOFF would do */
        if (type < NJD_CB_BA) {
            cursor++;
        } else if (type < NJD_TINYOFF) {
            cursor++;
        } else if (type < NJD_MATOFF) {
            cursor += 2;
        } else if (type < NJD_VERTOFF) {
            STEP_MATERIAL(type, STEP_CURSOR(2), STEP_CURSOR(4), STEP_CURSOR(6));
        } else if (type < NJD_VOLOFF) {
        } else if (type < NJD_STRIPOFF) {
        } else if (type < NJD_CE) {
            if (enabled > 0) {
                *cursor |= NJD_FST_ENV;
            } else {
                *cursor &= ~NJD_FST_ENV;
            }
            cursor++;
            length = *cursor;
            cursor += (Uint16)(length + 1);
        } else {
            return;
        }
    }
}

void chao_set_jewel_texture(struct task *chao, int texture)
{
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    struct chao_shape_work *shape = &work->shape;
    NJS_CNK_OBJECT **node = shape->nodes;
    const Sint16 *list = material_nodes;
    int index;

    if (shape->flags & CHAO_SHAPE_SHINY) {
        chao_clear_shiny_material(chao);
    }
    shape->flags |= CHAO_SHAPE_JEWEL;
    for (index = 0; index < CHAO_BODY_NODES; index++, node++) {
        if (*list == NODE_LIST_END) {
            break;
        }
        if (*list == index) {
            set_node_environment(*node, 1);
            garden_set_model_texture(*node, texture);
            list++;
        }
    }
}

void chao_clear_jewel_texture(struct task *chao)
{
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    struct chao_shape_work *shape = &work->shape;
    NJS_CNK_OBJECT **node = shape->nodes;
    const Sint16 *list = material_nodes;
    int index;

    shape->flags &= ~CHAO_SHAPE_JEWEL;
    for (index = 0; index < CHAO_BODY_NODES; index++, node++) {
        if (*list == NODE_LIST_END) {
            break;
        }
        if (*list == index) {
            set_node_environment(*node, 0);
            restore_node_textures(*node);
            list++;
        }
    }
}

/* the list's last halfword, at polygon_halfwords - 1, is its end chunk. a
 * null chunk in its place has Ninja draw on past it, into the pass
 * chao_append_shiny_pass writes there */
static void enable_shiny_pass(struct garden_model *model)
{
    Sint16 *end = model->chunk.plist;

    if (end) {
        end += model->polygon_halfwords - 1;
        *end = NJD_CN;
    }
}

static void enable_node_shine(NJS_CNK_OBJECT *node)
{
    if (!node || !node->model) {
        return;
    }
    enable_shiny_pass((struct garden_model *)node->model);
}

static void disable_shiny_pass(struct garden_model *model)
{
    Sint16 *end = model->chunk.plist;

    /* @bug memory: writes through a null plist, which enable_shiny_pass at
     * 0x0c510700 tests for and this function at 0x0c51072e doesn't */
    end += model->polygon_halfwords - 1;
    *end = NJD_CE;
}

static void disable_node_shine(NJS_CNK_OBJECT *node)
{
    if (!node || !node->model) {
        return;
    }
    disable_shiny_pass((struct garden_model *)node->model);
}

void chao_set_shiny_material(struct task *chao)
{
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    struct chao_shape_work *shape = &work->shape;
    NJS_CNK_OBJECT **node = shape->nodes;
    const Sint16 *list = material_nodes;
    int index;

    if (shape->flags & CHAO_SHAPE_JEWEL) {
        chao_clear_jewel_texture(chao);
    }
    shape->flags |= CHAO_SHAPE_SHINY;
    for (index = 0; index < CHAO_BODY_NODES; index++, node++) {
        if (*list == NODE_LIST_END) {
            break;
        }
        if (*list == index) {
            enable_node_shine(*node);
            list++;
        }
    }
}

void chao_clear_shiny_material(struct task *chao)
{
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    struct chao_shape_work *shape = &work->shape;
    NJS_CNK_OBJECT **node = shape->nodes;
    const Sint16 *list = material_nodes;
    int index;

    shape->flags &= ~CHAO_SHAPE_SHINY;
    for (index = 0; index < CHAO_BODY_NODES; index++, node++) {
        if (*list == NODE_LIST_END) {
            break;
        }
        if (*list == index) {
            disable_node_shine(*node);
            list++;
        }
    }
}

#define COUNT_HALFWORDS(halfwords) (count += (halfwords), cursor += (halfwords))

NJS_CNK_OBJECT *garden_clone_object(NJS_CNK_OBJECT *source)
{
    NJS_CNK_OBJECT *node;
    NJS_CNK_MODEL *original;
    NJS_CNK_MODEL *model;
    Sint32 *vertex;
    Sint32 *vertices;
    Sint16 *cursor;
    Sint16 *polygons;
    int count;
    Uint8 type;
    Sint16 length;

    if (!source) {
        return NULL;
    }
    node = syCalloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }
    njMemCopy(node, source, sizeof(*node));
    if (source->model) {
        original = source->model;
        model = syCalloc(1, sizeof(*model));
        njMemCopy(model, source->model, sizeof(*model));
        model->r = 0.0F;
        node->model = model;
        if (original->vlist) {
            vertex = original->vlist;
            for (count = 1; *vertex++ != NJD_CE; count++) {
            }
            vertex = original->vlist;
            vertices = syMalloc(count * sizeof(*vertices));
            njMemCopy4(vertices, vertex, count);
            model->vlist = vertices;
        }
        if (original->plist) {
            cursor = original->plist;
            count = 0;
            for (;;) {
                type = *cursor;
                if (type < NJD_CB_BA) {
                    /* @hack spelling: count + 1 assigned for the null chunk,
                     * where count++ would do */
                    count = count + 1;
                    cursor++;
                } else if (type < NJD_TINYOFF) {
                    count++;
                    cursor++;
                } else if (type < NJD_MATOFF) {
                    COUNT_HALFWORDS(2);
                } else if (type < NJD_VERTOFF) {
                    STEP_MATERIAL(type, COUNT_HALFWORDS(2), COUNT_HALFWORDS(4),
                                  COUNT_HALFWORDS(6));
                } else if (type < NJD_VOLOFF) {
                } else if (type < NJD_STRIPOFF) {
                } else if (type >= NJD_CE) {
                    break;
                } else {
                    cursor++;
                    length = *cursor;
                    cursor++;
                    cursor += (Uint16)length;
                    count += (Uint16)length + 2;
                }
            }
            count++;
            cursor = original->plist;
            polygons = syCalloc(count, sizeof(*polygons));
            njMemCopy2(polygons, cursor, count);
            model->plist = polygons;
        } else {
            model->plist = NULL;
        }
    }
    if (source->child) {
        node->child = garden_clone_object(source->child);
    }
    if (source->sibling) {
        node->sibling = garden_clone_object(source->sibling);
    }
    return node;
}

void garden_free_object(NJS_CNK_OBJECT *node)
{
    NJS_CNK_MODEL *model;

    if (!node) {
        return;
    }
    if (node->child) {
        garden_free_object(node->child);
        node->child = NULL;
    }
    if (node->sibling) {
        garden_free_object(node->sibling);
        node->sibling = NULL;
    }
    model = node->model;
    if (model) {
        if (model->vlist) {
            syFree(model->vlist);
            model->vlist = NULL;
        }
        if (model->plist) {
            syFree(model->plist);
            model->plist = NULL;
        }
        syFree(model);
    }
    syFree(node);
}

struct garden_uv_scroll *garden_uv_create(NJS_CNK_OBJECT *object, Float du,
                                          Float dv)
{
    struct garden_uv_scroll *scroll = syCalloc(1, sizeof(*scroll));

    scroll->source = object;
    scroll->copy = garden_clone_object(object);
    scroll->du = du;
    scroll->dv = dv;
    return scroll;
}

void garden_uv_destroy(struct garden_uv_scroll *scroll)
{
    garden_free_object(scroll->copy);
    syFree(scroll);
}

#define STEP_UV_LISTS(halfwords) (source += (halfwords), copy += (halfwords))

void garden_uv_step(struct garden_uv_scroll *scroll)
{
    NJS_CNK_MODEL *original = scroll->source->model;
    NJS_CNK_MODEL *shifted = scroll->copy->model;
    Sint16 *source;
    Sint16 *copy;
    Uint8 type;
    Uint16 strips;
    Sint16 vertices;

    scroll->u += scroll->du;
    scroll->v += scroll->dv;
    while (scroll->u > UV_RANGE) {
        scroll->u -= UV_RANGE;
    }
    while (scroll->v > UV_RANGE) {
        scroll->v -= UV_RANGE;
    }
    while (scroll->u < 0.0F) {
        scroll->u += UV_RANGE;
    }
    while (scroll->v < 0.0F) {
        scroll->v += UV_RANGE;
    }
    if (!original->plist) {
        return;
    }
    source = original->plist;
    copy = shifted->plist;
    for (;;) {
        type = *source;
        /* @hack redundant: null and bits chunks tested apart, where one test
         * below NJD_TINYOFF would do */
        if (type < NJD_CB_BA) {
            source++;
            copy++;
        } else if (type < NJD_TINYOFF) {
            source++;
            copy++;
        } else if (type < NJD_MATOFF) {
            STEP_UV_LISTS(2);
        } else if (type < NJD_VERTOFF) {
            STEP_MATERIAL(type, STEP_UV_LISTS(2), STEP_UV_LISTS(4),
                          STEP_UV_LISTS(6));
        } else if (type < NJD_VOLOFF) {
        } else if (type < NJD_STRIPOFF) {
        } else if (type >= NJD_CE) {
            return;
        } else {
            source += 2;
            strips = *source++ & STRIP_COUNT_MASK;
            copy += 3;
            while (strips-- != 0) {
                vertices = *source++;
                copy++;
                if (vertices < 0) {
                    vertices = -vertices;
                }
                while (vertices-- != 0) {
                    source++;
                    copy++;
                    *copy++ = *source++ + scroll->u;
                    *copy++ = *source++ + scroll->v;
                }
            }
        }
    }
}

void chao_apply_body_material(struct task *chao)
{
    struct task_work *base = chao->work;
    struct chao_parameters *parameters =
        &((struct chao_work *)base)->profile.parameters;
    Sint16 color;
    Sint16 texture;

    if (parameters->color > 0) {
        chao_set_body_color(chao, parameters->color);
    }
    if (parameters->shiny == 1) {
        chao_set_shiny_material(chao);
    } else if (parameters->texture > 0) {
        color = jewel_colors[parameters->texture];
        texture = jewel_textures[parameters->texture];
        if (color != NO_JEWEL_COLOR) {
            chao_set_body_color(chao, color);
        }
        chao_set_jewel_texture(chao, texture);
    }
}
