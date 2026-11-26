/* the build, step and free of a garden Chao's body, and the tuning of its
 * springs */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/movement.h"
#ifdef NONMATCHING
/* the matrix-room tests and body_pose.h's frames, by which this side bounds a
 * body's matrix stack */
#include "chao/body_matrix.h"
#include "chao/body_pose.h"
#endif
#include "chao/face.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/model.h"
#include "chao/parts.h"
#include "chao/perception.h"
#include "chao/work.h"
#include "chao/world.h"
#include "draw/chunk.h"
#include "system/task.h"

#define STRIP_COUNT_MASK ((1 << NJD_UFO_SHIFT) - 1)
/* a tiny chunk, or a material or strip chunk's type and size */
#define CHUNK_HEAD_HALFWORDS 2
/* a material chunk's color: blue and green, then red and alpha */
#define COLOR_HALFWORDS 2
/* a strip vertex: its index, then its u and v */
#define UV_HALFWORDS 2
#define VERTEX_HALFWORDS (1 + UV_HALFWORDS)
/* the vertex list's halfword that counts its first chunk's vertices. it is the
 * high half of the chunk's second 32-bit word, and the vertices follow it */
#define COUNT_HALFWORD 3
/* the room copy_node leaves after a copied polygon list for its shiny pass.
 * each strip chunk takes STRIP_ROOM halfwords more than its strips take in
 * the pass. the pass takes SHINY_ROOM halfwords more */
#define STRIP_ROOM 3
#define SHINY_ROOM 23
/* the models chao_form_models holds for each type, and the index of a type's
 * first */
#define FORM_MODELS 6
#define FORM_FIRST(type) (((type) - CHAO_CHILD) * FORM_MODELS)
/* the model each node's offset is measured from. it is the child's swim model,
 * third in struct chao_morph_targets' order */
#define OFFSET_MODEL (FORM_FIRST(CHAO_CHILD) + 2)
/* the body's ARGB color, as the build writes it to shape->color */
#define BODY_COLOR 0xff66bbff
#define FREE_MEMBER(member)                                                    \
    {                                                                          \
        if (member) {                                                          \
            syFree(member);                                                    \
            (member) = NULL;                                                   \
        }                                                                      \
    }

/* the shiny pass's chunks: an additive material of ARGB color SHINY_DIFFUSE,
 * texture SHINY_TEXTURE, and one strip chunk */
#define SHINY_MATERIAL (NJD_CM_D | NJD_FBS_SA | NJD_FBD_ONE)
#define SHINY_DIFFUSE 0x50c8c8c8
#define SHINY_TEXTURE_FLAGS                                                    \
    (NJD_CT_TID | NJD_FFL_U | NJD_FFL_V | NJD_FCL_U | NJD_FDA_100)
#define SHINY_TEXTURE 9
#define SHINY_STRIP                                                            \
    (NJD_CS | NJD_FST_IL | NJD_FST_IS | NJD_FST_UA | NJD_FST_ENV)

/* the chao_body_springs entry for copy_node's next mesh */
static Sint32 spring_index;
/* the nodes prepare_vertices has prepared since the build began */
static Sint16 node_count;
static const NJS_POINT3 origin = {0.0F, 0.0F, 0.0F};

static int index_node(NJS_CNK_OBJECT *node, NJS_CNK_OBJECT **table, int index)
{
    table[index] = node;
    ++index;
    if (node->child) {
        index = index_node(node->child, table, index);
    }
    if (node->sibling) {
        index = index_node(node->sibling, table, index);
    }
    return index;
}
static void index_tree(NJS_CNK_OBJECT *root, NJS_CNK_OBJECT **table)
{
    int index = 0;
    table[index] = root;
    ++index;
    if (root->child) {
        index = index_node(root->child, table, index);
    }
    if (root->sibling) {
        /* @hack redundant: the index the last traversal returns, which
         * nothing reads */
        index = index_node(root->sibling, table, index);
    }
}

static void free_node(struct chao_node *node)
{
    struct chao_mesh *mesh;
    if (node) {
        if (node->object.child) {
            free_node(CHAO_NODE_CHILD(node));
            node->object.child = NULL;
        }
        if (node->object.sibling) {
            free_node(CHAO_NODE_SIBLING(node));
            node->object.sibling = NULL;
        }
        mesh = (struct chao_mesh *)node->object.model;
        if (mesh) {
            FREE_MEMBER(mesh->material.chunk.vlist);
            FREE_MEMBER(mesh->material.chunk.plist);
            FREE_MEMBER(mesh->rest_points);
            FREE_MEMBER(mesh->rest_normals);
            FREE_MEMBER(mesh->springs);
            syFree(mesh);
        }
        if (node->matrix) {
            syFree(node->matrix);
        }
        syFree(node);
    }
}

/* run one, two or three by how many colors the material chunk type sets */
#define BY_COLORS(type, one, two, three)                                       \
    {                                                                          \
        switch (type) {                                                        \
        case NJD_CM_D:                                                         \
        case NJD_CM_A:                                                         \
        case NJD_CM_S:                                                         \
            (one);                                                             \
            break;                                                             \
        case NJD_CM_DA:                                                        \
        case NJD_CM_DS:                                                        \
        case NJD_CM_AS:                                                        \
            (two);                                                             \
            break;                                                             \
        case NJD_CM_DAS:                                                       \
            (three);                                                           \
            break;                                                             \
        }                                                                      \
    }
#define SKIP_INPUT(count) (source += (count))

void chao_append_shiny_pass(Sint16 *polygons, int halfwords)
{
    Sint16 *source = polygons;
    Sint16 *output = polygons + halfwords;
    Sint16 *pass_size;
    Sint16 *strip_count;
    Uint16 chunk_size;
    Uint16 strips;
    Sint16 length;
    Sint16 head;
    if ((Uint32)output & 3) {
        *output++ = NJD_CN;
    }
    *output++ = SHINY_MATERIAL;
    *output++ = COLOR_HALFWORDS;
    *output++ = (Sint16)SHINY_DIFFUSE;
    *output++ = SHINY_DIFFUSE >> 16;
    *output++ = (Sint16)SHINY_TEXTURE_FLAGS;
    *output++ = NJD_FFM_BF | SHINY_TEXTURE;
    *output++ = SHINY_STRIP;
    pass_size = output;
    *output++ = 1;
    *output++ = 0;
    strip_count = pass_size + 1;
    for (;;) {
        for (;;) {
            head = *source;
            if ((head & CHUNK_TYPE_BITS) < NJD_CB_BA) {
                source++;
            } else if ((head & CHUNK_TYPE_BITS) < NJD_TINYOFF) {
                source++;
            } else if ((head & CHUNK_TYPE_BITS) < NJD_MATOFF) {
                SKIP_INPUT(CHUNK_HEAD_HALFWORDS);
            } else if ((head & CHUNK_TYPE_BITS) < NJD_VERTOFF) {
                SKIP_INPUT(CHUNK_HEAD_HALFWORDS);
                BY_COLORS(head & CHUNK_TYPE_BITS, SKIP_INPUT(COLOR_HALFWORDS),
                          SKIP_INPUT(2 * COLOR_HALFWORDS),
                          SKIP_INPUT(3 * COLOR_HALFWORDS));
            } else if ((head & CHUNK_TYPE_BITS) < NJD_VOLOFF) {
            } else if ((head & CHUNK_TYPE_BITS) < NJD_STRIPOFF) {
            } else if ((head & CHUNK_TYPE_BITS) < NJD_CE) {
                break;
            }
        }
        source++;
        chunk_size = *source++;
        strips = *source++ & STRIP_COUNT_MASK;
        /* chunk_size counts the strip count's halfword, then a length for each
         * strip and VERTEX_HALFWORDS for each vertex. the pass keeps each
         * length and each vertex's index */
        *pass_size += ((strips * (VERTEX_HALFWORDS - 1)) + chunk_size - 1) /
                      VERTEX_HALFWORDS;
        *strip_count += strips;
        while (strips--) {
            length = *source++;
            *output++ = length;
            if (length < 0) {
                length = -length;
            }
            while (length--) {
                *output++ = *source++;
                source += UV_HALFWORDS;
            }
        }
        head = *source;
        if ((head & CHUNK_TYPE_BITS) == NJD_CN) {
            source++;
            head = *source;
        }
        if ((head & CHUNK_TYPE_BITS) == NJD_CE) {
            break;
        }
    }
    *output++ = NJD_CE;
    *output = NJD_CE;
}

/* take count halfwords of the source list into the copy */
#define TAKE_INPUT(count)                                                      \
    (words += (count), capacity += (count), cursor += (count))
static struct chao_node *copy_node(NJS_CNK_OBJECT *source, NJS_CNK_OBJECT *base,
                                   struct chao_node *parent)
{
    struct chao_node *node;
    struct chao_mesh *mesh;
    NJS_CNK_MODEL *original;
    Sint32 *vertices;
    Sint32 *vertex;
    Sint16 *polygons;
    Sint16 *cursor;
    Sint16 head;
    Uint16 chunk_size;
    Uint16 strips;
    Uint32 capacity;
    Uint32 words;
    if (!source) {
        return NULL;
    }
    node = syCalloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }
    node->matrix = syCalloc(1, sizeof(*node->matrix));
    njMemCopy(node, source, sizeof(*source));
    /* @bug memory: free_node frees the source tree's nodes and lists, which
     * the copy still points to, where an allocation fails (0x0c517ce6) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. the copy holds none of the
     * source's links until it makes its own. where the heap has no matrix, this
     * side frees the copy and returns null */
    node->object.child = NULL;
    node->object.sibling = NULL;
    node->object.model = NULL;
    if (!node->matrix) {
        free_node(node);
        return NULL;
    }
#endif
    node->rest_position.x = source->pos[0];
    node->rest_position.y = source->pos[1];
    node->rest_position.z = source->pos[2];
    node->offset.x = source->pos[0] - base->pos[0];
    node->offset.y = source->pos[1] - base->pos[1];
    node->offset.z = source->pos[2] - base->pos[2];
    if (source->model) {
        original = source->model;
        mesh = syCalloc(1, sizeof(*mesh));
        if (!mesh) {
            free_node(node);
            return NULL;
        }
        /* @hack spelling: source->model, where original would do */
        njMemCopy(mesh, source->model, sizeof(*original));
        node->object.model = &mesh->material.chunk;
#ifdef NONMATCHING
        /* this side starts the mesh's lists at null until the copy makes its
         * own. so free_node frees no list of the source's */
        mesh->material.chunk.vlist = NULL;
        mesh->material.chunk.plist = NULL;
#endif
        mesh->material.saved_textures[0] = -1;
        mesh->material.saved_textures[1] = -1;
        mesh->material.saved_textures[2] = -1;
        mesh->material.saved_textures[3] = -1;
        mesh->material.chunk.r = 0.0F;
        if (original->vlist) {
            vertex = original->vlist;
            for (words = 1; *vertex++ != NJD_CE; words++) {
            }
            vertex = original->vlist;
            vertices = syMalloc(words * sizeof(*vertices));
            if (!vertices) {
                free_node(node);
                return NULL;
            }
            njMemCopy4(vertices, vertex, words);
            mesh->material.chunk.vlist = vertices;
            node->spring = &chao_body_springs[spring_index++];
        }
        if (original->plist) {
            cursor = original->plist;
            words = 0;
            capacity = 0;
            for (;;) {
                head = *cursor;
                if ((Uint8)head < NJD_CB_BA) {
                    words++;
                    capacity++;
                    cursor++;
                } else if ((Uint8)head < NJD_TINYOFF) {
                    words++;
                    capacity++;
                    cursor++;
                } else if ((Uint8)head < NJD_MATOFF) {
                    TAKE_INPUT(CHUNK_HEAD_HALFWORDS);
                } else if ((Uint8)head < NJD_VERTOFF) {
                    TAKE_INPUT(CHUNK_HEAD_HALFWORDS);
                    BY_COLORS((Uint8)head, TAKE_INPUT(COLOR_HALFWORDS),
                              TAKE_INPUT(2 * COLOR_HALFWORDS),
                              TAKE_INPUT(3 * COLOR_HALFWORDS));
                } else if ((Uint8)head < NJD_VOLOFF) {
                } else if ((Uint8)head < NJD_STRIPOFF) {
                } else if ((Uint8)head < NJD_CE) {
                    cursor++;
                    chunk_size = *cursor++;
                    strips = *cursor & STRIP_COUNT_MASK;
                    cursor += chunk_size;
                    words += chunk_size + CHUNK_HEAD_HALFWORDS;
                    capacity += chunk_size + CHUNK_HEAD_HALFWORDS;
                    capacity +=
                        (((strips * (VERTEX_HALFWORDS - 1)) + chunk_size - 1) /
                         VERTEX_HALFWORDS) +
                        STRIP_ROOM;
                } else {
                    break;
                }
            }
            cursor = original->plist;
            words++;
            capacity += SHINY_ROOM;
            polygons = syCalloc(capacity, sizeof(*polygons));
            if (!polygons) {
                free_node(node);
                return NULL;
            }
            njMemCopy2(polygons, cursor, words);
            mesh->material.chunk.plist = polygons;
            mesh->material.polygon_halfwords = words;
        } else {
            mesh->material.chunk.plist = NULL;
        }
    }
    node->parent = parent;
    if (source->child) {
        node->object.child =
            (NJS_CNK_OBJECT *)copy_node(source->child, base->child, node);
#ifdef NONMATCHING
        /* this side frees the node and returns null where the child's copy
         * fails */
        if (!node->object.child) {
            free_node(node);
            return NULL;
        }
#endif
    }
    if (source->sibling) {
        node->object.sibling =
            (NJS_CNK_OBJECT *)copy_node(source->sibling, base->sibling, parent);
#ifdef NONMATCHING
        /* this side frees the node and returns null where the sibling's copy
         * fails */
        if (!node->object.sibling) {
            free_node(node);
            return NULL;
        }
#endif
    }
    return node;
}

/* the weight of a vertex at distance from its spring's center. it is the
 * spring's weight, and weight_per_unit more for each unit of distance past
 * start. a hanging vertex passes its node's parent, and as joint any meshless
 * node between the two. it starts from the parent's weight at the length of the
 * node's position. with a joint, the length is the joint's. each unit past
 * start then adds the parent's weight_per_unit */
/* @hack redundant: register on node and weight, where plain parameters and
 * locals would do */
static Float spring_weight(register struct chao_node *node,
                           struct chao_node *parent, struct chao_node *joint,
                           Float distance)
{
    Float start;
    Float result;
    register Float weight;
    Float rate;
    if (parent) {
        rate = parent->spring->weight_per_unit;
        /* @bug uninitialized: with a parent, the weight subtracts start,
         * which that path never sets (0x0c517f76) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: with a parent, start takes
         * the node's spring's start */
        start = node->spring->start;
#endif
        if (joint) {
            weight = spring_weight(parent, NULL, NULL,
                                   njScalor((NJS_VECTOR *)joint->object.pos));
        } else {
            weight = spring_weight(parent, NULL, NULL,
                                   njScalor((NJS_VECTOR *)node->object.pos));
        }
    } else {
        start = node->spring->start;
        weight = node->spring->weight;
        rate = node->spring->weight_per_unit;
    }
    if (node->spring->start > distance) {
        result = weight;
    } else {
        result = weight + (rate * (distance - start));
    }
    return result;
}

/* spring_weight for the mouth, which takes its parent's start, weight and
 * weight_per_unit as they are */
static Float mouth_weight(struct chao_node *node, struct chao_node *parent,
                          struct chao_node *joint, Float distance)
{
    Float start;
    Float weight;
    Float rate;
    if (parent) {
        start = parent->spring->start;
        weight = parent->spring->weight;
        rate = parent->spring->weight_per_unit;
    } else {
        start = node->spring->start;
        weight = node->spring->weight;
        rate = node->spring->weight_per_unit;
    }
    if (!(node->spring->start > distance)) {
        weight += rate * (distance - start);
    }
    return weight;
}

static Bool hanging_node(int index)
{
    Sint32 *slot;
    for (slot = chao_hanging_nodes; *slot != CHAO_HANGING_NODE_END; slot++) {
        if (*slot == index) {
            return TRUE;
        }
    }
    return FALSE;
}

#define TUNE_VERTEX()                                                          \
    {                                                                          \
        state->gain = gain + (gain_per_weight * state->weight);                \
        state->gain *= 0.75F;                                                  \
        state->damping = damping + (damping_per_weight * state->weight);       \
        if (state->damping > 0.999F) {                                         \
            state->damping = 0.999F;                                           \
        }                                                                      \
        state->damping = 1.0F - ((1.0F - state->damping) * 0.6F);              \
    }

static void tune_springs(struct task *chao)
{
    struct chao_shape_work *shape;
    NJS_CNK_OBJECT **slot;
    struct chao_node *node;
    struct chao_node *parent;
    struct chao_node *joint;
    struct chao_mesh *mesh;
    struct chunk_vertex_vn *vertex;
    struct chunk_vertex_vn *end;
    NJS_POINT3 *point;
    struct chao_vertex_spring *state;
    struct chao_spring *spring;
    NJS_POINT3 *center;
    NJS_POINT3 zero;
    NJS_POINT3 transformed;
    Float gain;
    Float gain_per_weight;
    Float damping;
    Float damping_per_weight;
    Float distance;
    int i;
    Sint32 count;
#ifdef NONMATCHING
    /* this side tunes no springs while Ninja's matrix stack has no room for the
     * step's push */
    if (!chao_matrix_slot_available()) {
        return;
    }
#endif
    shape = &CHAO_WORK(chao)->shape;
    slot = shape->nodes;
    /* @hack spelling: i set before the loop, where the for would set it */
    i = 0;
    for (; i < CHAO_BODY_NODES; i++, slot++) {
        node = (struct chao_node *)*slot;
        mesh = (struct chao_mesh *)node->object.model;
        if (mesh) {
            point = mesh->rest_points;
            state = mesh->springs;
            count = mesh->vertex_count;
            vertex = mesh->vertices;
            end = vertex + count;
            spring = node->spring;
            gain = spring->gain;
            gain_per_weight = spring->gain_per_weight;
            damping = spring->damping;
            damping_per_weight = spring->damping_per_weight;
            /* a hanging node weighs each vertex by its distance, in its
             * parent's space, from the parent's spring center. a parent
             * without a mesh is a joint, whose own parent serves */
            if (hanging_node(i) && node->parent) {
                parent = node->parent;
                joint = NULL;
                if (!parent->object.model) {
                    joint = parent;
                    parent = parent->parent;
                }
                if (parent->spring) {
                    center = &parent->spring->center;
                } else {
                    center = NULL;
                }
                for (; vertex < end; vertex++, point++, state++) {
                    /* @hack redundant: a copy of origin, which nothing reads */
                    zero = origin;
                    (void)zero;
                    njPushMatrix(&_nj_unit_matrix_);
                    if (joint) {
                        njTranslateEx((NJS_VECTOR *)joint->object.pos);
                        njRotateXYZ(NULL, joint->object.ang[0],
                                    joint->object.ang[1], joint->object.ang[2]);
                        if (!(joint->object.evalflags & NJD_EVAL_UNIT_SCL)) {
                            njScale(NULL, joint->object.scl[0],
                                    joint->object.scl[1], joint->object.scl[2]);
                        }
                    }
                    njTranslateEx((NJS_VECTOR *)node->object.pos);
                    njRotateXYZ(NULL, node->object.ang[0], node->object.ang[1],
                                node->object.ang[2]);
                    njCalcPoint(NULL, point, &transformed);
                    njPopMatrixEx();
                    if (center) {
                        distance = njDistanceP2P(center, &transformed);
                    } else {
                        distance = njScalor(&transformed);
                    }
                    if (i == CHAO_NODE_MOUTH) {
                        state->weight =
                            mouth_weight(node, parent, joint, distance);
                    } else {
                        state->weight =
                            spring_weight(node, parent, joint, distance);
                    }
                    TUNE_VERTEX();
                }
            } else {
                center = &node->spring->center;
                for (; vertex < end; vertex++, point++, state++) {
                    distance = njDistanceP2P(center, point);
                    state->weight = spring_weight(node, NULL, NULL, distance);
                    TUNE_VERTEX();
                }
            }
            /* @hack redundant: an empty test for the eye anchors */
        } else if (i == CHAO_NODE_EYE_ANCHOR_0 || i == CHAO_NODE_EYE_ANCHOR_1) {
        }
    }
}

static Bool prepare_vertices(struct chao_node *node)
{
    struct chao_mesh *mesh = (struct chao_mesh *)node->object.model;
    struct chunk_vertex_vn *vertex;
    struct chunk_vertex_vn *end;
    NJS_POINT3 *points;
    NJS_VECTOR *normals;
    struct chao_vertex_spring *springs;
    Sint16 *cursor;
    int count;
    if (mesh) {
        cursor = (Sint16 *)mesh->material.chunk.vlist + COUNT_HALFWORD;
        count = *cursor++;
        /* @bug memory: the spring step overflows chao_spring_points for a
         * mesh of more than CHAO_SPRING_POINTS vertices (0x0c50219c) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. it refuses a mesh of a
         * negative count, or of more than CHAO_SPRING_POINTS vertices */
        if (count < 0 || count > CHAO_SPRING_POINTS) {
            return FALSE;
        }
#endif
        vertex = (struct chunk_vertex_vn *)cursor;
        mesh->vertex_count = count;
        mesh->vertices = vertex;
        points = syCalloc(count, sizeof(*points));
        if (!points) {
#ifdef NONMATCHING
            /* this side returns FALSE and leaves the node to the caller, which
             * frees the whole tree. the matching side frees the node while its
             * parent points to it */
            return FALSE;
#else
            free_node(node);
            return FALSE;
#endif
        }
        normals = syCalloc(count, sizeof(*normals));
        if (!normals) {
#ifdef NONMATCHING
            /* this side frees points and returns FALSE. it leaves the node to
             * the caller, as at the first failure */
            syFree(points);
#else
            free_node(node);
#endif
            return FALSE;
        }
        springs = syCalloc(count, sizeof(*springs));
        if (!springs) {
            syFree(points);
            syFree(normals);
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side. it leaves
             * the node to the caller, which frees the whole tree */
            return FALSE;
#else
            /* @bug memory: free_node frees the node while its parent still
             * points to it, where an allocation fails. chao_build_body then
             * morphs the tree through it (0x0c518356) */
            free_node(node);
            return FALSE;
#endif
        }
        mesh->rest_points = points;
        mesh->rest_normals = normals;
        mesh->springs = springs;
        for (end = vertex + count; vertex < end;
             vertex++, normals++, points++, springs++) {
            springs->velocity.x = 0.0F;
            springs->velocity.y = 0.0F;
            springs->velocity.z = 0.0F;
            *points = vertex->point;
            *normals = vertex->normal;
            /* @hack redundant: each rest point scaled by 1 */
            points->x *= 1.0F;
            points->y *= 1.0F;
            points->z *= 1.0F;
        }
    }
    ++node_count;
    if (node->object.child) {
#ifdef NONMATCHING
        /* this side passes a failure in the child's tree up */
        if (!prepare_vertices(CHAO_NODE_CHILD(node))) {
            return FALSE;
        }
#else
        prepare_vertices(CHAO_NODE_CHILD(node));
#endif
    }
    if (node->object.sibling) {
#ifdef NONMATCHING
        /* this side passes a failure in the sibling's tree up */
        if (!prepare_vertices(CHAO_NODE_SIBLING(node))) {
            return FALSE;
        }
#else
        prepare_vertices(CHAO_NODE_SIBLING(node));
#endif
    }
    return TRUE;
}

/* the morph's tables in shape->member. INDEX_NEXT takes the next model into
 * one table, and INDEX_FORM takes a type's six from models[first] */
#define INDEX_NEXT(member, field)                                              \
    index_tree(models[index++], shape->member->field)
#define INDEX_FORM(member, first)                                              \
    {                                                                          \
        index_tree(models[(first)], shape->member->base);                      \
        index_tree(models[(first) + 1], shape->member->grown);                 \
        index_tree(models[(first) + 2], shape->member->swim);                  \
        index_tree(models[(first) + 3], shape->member->fly);                   \
        index_tree(models[(first) + 4], shape->member->run);                   \
        index_tree(models[(first) + 5], shape->member->power);                 \
    }

Bool chao_build_body(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_shape_work *shape = &work->shape;
    NJS_CNK_OBJECT **models;
    int index;
#ifdef NONMATCHING
    /* this side builds no body for a type outside CHAO_CHILD to
     * CHAO_DARK_CHAOS, which indexes past chao_form_models */
    if (work->profile.parameters.type < CHAO_CHILD ||
        work->profile.parameters.type > CHAO_DARK_CHAOS) {
        return FALSE;
    }
#endif
    spring_index = 0;
    node_count = 0;
    index = FORM_FIRST(work->profile.parameters.type);
    models = chao_form_models;
#ifdef NONMATCHING
    /* this side builds no body whose tree Ninja's matrix stack has no room
     * for */
    if (!chao_body_has_matrix_room(models[index], CHAO_BUILD_FRAMES)) {
        return FALSE;
    }
#endif
    shape->root = copy_node(models[index], models[OFFSET_MODEL], NULL);
    if (!shape->root) {
        return FALSE;
    }
    index_tree(&shape->root->object, shape->nodes);
    shape->form = syCalloc(1, sizeof(struct chao_morph_targets));
    if (!shape->form) {
        return FALSE;
    }
    /* @hack spelling: the form's own six taken through index++, where
     * INDEX_FORM(form, index) would do */
    INDEX_NEXT(form, base);
    INDEX_NEXT(form, grown);
    INDEX_NEXT(form, swim);
    INDEX_NEXT(form, fly);
    INDEX_NEXT(form, run);
    index_tree(models[index], shape->form->power);
    /* only a child's morph reads the hero and dark children's tables */
    shape->hero = syCalloc(1, sizeof(struct chao_morph_targets));
    if (!shape->hero) {
        return FALSE;
    }
    INDEX_FORM(hero, FORM_FIRST(CHAO_HERO_TYPE(CHAO_CHILD)));
    shape->dark = syCalloc(1, sizeof(struct chao_morph_targets));
    if (!shape->dark) {
        return FALSE;
    }
    INDEX_FORM(dark, FORM_FIRST(CHAO_DARK_TYPE(CHAO_CHILD)));
#ifdef NONMATCHING
    /* this side returns FALSE where prepare_vertices fails, and leaves the tree
     * to the caller */
    if (!prepare_vertices(shape->root)) {
        return FALSE;
    }
#else
    prepare_vertices(shape->root);
#endif
    chao_morph_body(chao);
    tune_springs(chao);
    chao_build_node_matrices(chao);
    shape->color = BODY_COLOR;
    chao_apply_body_material(chao);
    return TRUE;
}

#define STEP_HAND_PROP(prop)                                                   \
    {                                                                          \
        if ((prop).object) {                                                   \
            if ((prop).target_scale) {                                         \
                (prop).scale += step;                                          \
                if ((prop).scale > 1.0F) {                                     \
                    (prop).scale = 1.0F;                                       \
                }                                                              \
            } else {                                                           \
                (prop).scale -= step;                                          \
                if ((prop).scale < 0.0F) {                                     \
                    (prop).scale = 0.0F;                                       \
                    (prop).object = NULL;                                      \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }

void chao_step_body(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_shape_work *shape = &work->shape;
    Float step;
    if (shape->flags & CHAO_SHAPE_RESHAPE) {
        chao_morph_body(chao);
        tune_springs(chao);
    }
    if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, GARDEN_SHADE_HIT)) {
        shape->flags |= CHAO_SHAPE_GROUND_SHADOW;
    } else {
        shape->flags &= ~CHAO_SHAPE_GROUND_SHADOW;
    }
    /* @hack spelling: CHAO_PROP_FADE held in step, where the constant would
     * do */
    step = CHAO_PROP_FADE;
    STEP_HAND_PROP(shape->prop_4);
    STEP_HAND_PROP(shape->prop_11);
    chao_move_body_tree(chao);
}

#ifdef NONMATCHING
/* this side frees a body's three sets of morph targets and its tree, and clears
 * the root. so chao_change_form can drop a body that failed to build */
static void release_body(struct chao_shape_work *shape)
{
    FREE_MEMBER(shape->form);
    FREE_MEMBER(shape->hero);
    FREE_MEMBER(shape->dark);
    free_node(shape->root);
    shape->root = NULL;
}
#endif

void chao_free_body(struct task *chao)
{
    struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
#ifdef NONMATCHING
    /* this side frees the body by release_body, which also clears the root */
    release_body(shape);
#else
    if (shape->form) {
        syFree(shape->form);
    }
    if (shape->hero) {
        syFree(shape->hero);
    }
    if (shape->dark) {
        syFree(shape->dark);
    }
    free_node(shape->root);
#endif
}

Bool chao_change_form(struct task *chao, Uint32 form)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
#ifdef NONMATCHING
    /* the body this side keeps while a new one builds */
    struct chao_shape_work *shape = &CHAO_WORK(chao)->shape;
    struct chao_shape_work previous_shape;
    Uint8 previous_type;
#endif
    if (form >= CHAO_DARK_CHAOS + 1) {
        return FALSE;
    }
    switch (form) {
    case CHAO_NONE:
    case CHAO_EGG:
    case CHAO_HERO_TYPE(CHAO_CHILD):
    case CHAO_DARK_TYPE(CHAO_CHILD):
        return FALSE;
    }
#ifdef NONMATCHING
    /* the old body stays until the new one is built, and comes back if the
     * build fails */
    previous_shape = *shape;
    previous_type = parameters->type;
    shape->root = NULL;
    shape->form = NULL;
    shape->hero = NULL;
    shape->dark = NULL;
    parameters->type = form;
    if (!chao_build_body(chao)) {
        release_body(shape);
        *shape = previous_shape;
        parameters->type = previous_type;
        return FALSE;
    }
    release_body(&previous_shape);
#else
    chao_free_body(chao);
    parameters->type = form;
    chao_build_body(chao);
#endif
    chao_face_init(chao);
    chao_icon_init(chao);
    chao_restore_animal_parts(chao);
    chao_perception_init(chao);
    return TRUE;
}
