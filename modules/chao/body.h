/* a garden Chao's body, which is a writable copy of its form's model tree.
 * the Chao morphs, poses and draws it, and its springs move it */
#ifndef CHAO_BODY_H
#define CHAO_BODY_H
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/model.h"
#include "draw/chunk.h"
#include "system/angle.h"

extern NJS_CNK_OBJECT chao_hat_2;

struct task;

/* a body's nodes, which the code indexes in depth-first order */
#define CHAO_BODY_NODES 40
/* the nodes the code singles out. eye 0's nodes are those of chao_shape_work's
 * eyes[0]. a hand or a foot takes its node's number, as chao_shape_work's
 * hand_4 and foot_6 do. an anchor's matrix places what chao_shape_work keeps
 * of it */
enum chao_body_node {
    CHAO_NODE_TRUNK = 1,
    CHAO_NODE_HAND_4 = 4,
    CHAO_NODE_FOOT_6 = 6,
    CHAO_NODE_HAND_11 = 11,
    CHAO_NODE_FOOT_13 = 13,
    CHAO_NODE_HEAD_ANCHOR = 15,
    CHAO_NODE_HEAD = 16,
    CHAO_NODE_EYE_ANCHOR_0 = 17,
    CHAO_NODE_EYE_0 = 18,
    CHAO_NODE_LID_0 = 19,
    CHAO_NODE_EYE_ANCHOR_1 = 20,
    CHAO_NODE_EYE_1 = 21,
    CHAO_NODE_LID_1 = 22,
    CHAO_NODE_MOUTH = 27,
    CHAO_NODE_MOUTH_ANCHOR = 28,
    CHAO_NODE_ICON_ANCHOR = 33,
    CHAO_NODE_MEDAL = 35
};
#define CHAO_NODE_CHILD(node) ((struct chao_node *)(node)->object.child)
#define CHAO_NODE_SIBLING(node) ((struct chao_node *)(node)->object.sibling)

/* what the morph blends at each node, by chao_morph_flags. MORPH_MESH blends
 * the node's vertices and color. MORPH_JOINT blends its position and angles,
 * though a child's morph blends those at every node. no code tests MORPH_EYE */
#define MORPH_MESH 1
#define MORPH_JOINT 2
#define MORPH_EYE 4
extern Uint16 chao_morph_flags[CHAO_BODY_NODES];

/* a vertex's spring: its velocity, and the weight, gain and damping its
 * node's struct chao_spring gives it */
struct chao_vertex_spring {
    NJS_VECTOR velocity;
    Float weight, gain, damping;
};

/* the spring of a node with a vertex list. no code reads unk_10 or unk_14 */
struct chao_spring {
    /* a vertex's gain and damping, and what each unit of its weight adds */
    Float gain, gain_per_weight;
    Float damping, damping_per_weight;
    Float unk_10, unk_14;
    /* a vertex's weight within start of center, and what each unit of
     * distance past start adds */
    Float start, weight, weight_per_unit;
    NJS_POINT3 center;
};

/* a node's model as the body owns it. its vertex list opens with a chunk of
 * vertex_count struct chunk_vertex_vn, at vertices. rest_points and
 * rest_normals hold the morph's shape, which the springs pull the vertices
 * back to */
struct chao_mesh {
    struct garden_model material;
    Sint32 vertex_count;
    struct chunk_vertex_vn *vertices;
    NJS_POINT3 *rest_points;
    NJS_VECTOR *rest_normals;
    struct chao_vertex_spring *springs;
};

/* the byte offset of a body mesh's diffuse color in its polygon list. the
 * list opens with a material chunk, whose type and size halfwords come first.
 * the color's bytes are blue, green, red and alpha */
#define CHAO_DIFFUSE_OFFSET 4

/* the node tables of a form's six models, each in depth-first order. base is
 * the form before it evolves, and grown the form evolved with no leaning. the
 * other four are grown leaning wholly to swim, fly, run or power.
 * chao_parameters' fly_swim and run_power hold the leanings */
struct chao_morph_targets {
    NJS_CNK_OBJECT *base[CHAO_BODY_NODES], *grown[CHAO_BODY_NODES];
    NJS_CNK_OBJECT *swim[CHAO_BODY_NODES], *fly[CHAO_BODY_NODES];
    NJS_CNK_OBJECT *run[CHAO_BODY_NODES], *power[CHAO_BODY_NODES];
};

/* an object a Chao holds at a hand. its scale grows to 1 by CHAO_PROP_FADE a
 * step while target_scale is not 0. otherwise the scale shrinks to 0, and the
 * object then becomes null */
struct chao_hand_prop {
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *textures;
    Float scale, target_scale;
};

/* how far a prop's scale moves each step as it fades in or out */
#define CHAO_PROP_FADE 0.1F

/* a node of a Chao's body. object comes first, so each of the tree's links
 * points to a chao_node. CHAO_NODE_CHILD and CHAO_NODE_SIBLING follow the links
 * as such. no code writes weight, gain, damping, rigid or swing. each holds the
 * 0 the build's allocation leaves */
struct chao_node {
    NJS_CNK_OBJECT object;
    struct chao_node *parent; /* null for the root */
    /* the node's position less the same node's in the child's swim model. a
     * pose adds it to each position key */
    NJS_VECTOR offset;
    Uint8 unk_44[12];
    NJS_POINT3 rest_position; /* the node's position in its form's model */
    NJS_VECTOR velocity;
    Uint8 unk_68[0x80 - 0x68];
    Float weight, gain, damping;
    Bool rigid, swing;
    NJS_CNK_OBJECT *animal_part; /* drawn in place of the node's mesh */
    /* an object drawn at the node. it fades as a struct chao_hand_prop does, in
     * while fade_in is set and out while it is clear */
    struct {
        NJS_CNK_OBJECT *object;
        NJS_TEXLIST *textures;
        Float scale;
        Bool fade_in;
    } prop;
    /* prop_position and prop_angles place the prop where placed is set */
    Bool placed;
    NJS_VECTOR prop_position;
    struct angles prop_angles;
    /* the node's unposed transform from chao_build_node_matrices. while
     * CHAO_BODY_SPRINGS and CHAO_BODY_IN_VIEW are set, each spring step stores
     * a mesh node's transform here */
    NJS_MATRIX *matrix;
    struct chao_spring *spring; /* for a node with a vertex list */
};

/* a spring for each node with a vertex list, in depth-first order */
extern struct chao_spring chao_body_springs[];
/* six models for each type from CHAO_CHILD on, in the order of struct
 * chao_morph_targets */
extern NJS_CNK_OBJECT *chao_form_models[];
#define CHAO_HANGING_NODE_END (-1)
/* the nodes whose vertices weigh by their parent's spring, ended by
 * CHAO_HANGING_NODE_END */
extern Sint32 chao_hanging_nodes[];

/* set each node's matrix to its unposed transform at the Chao's position and
 * yaw */
void chao_build_node_matrices(struct task *chao);
/* pose the tree by the Chao's motion, read the anchors chao_shape_work keeps,
 * and run the springs */
void chao_move_body_tree(struct task *chao);
/* polygons is a body mesh's polygon list of halfwords halfwords, NJD_CE the
 * last. each vertex of its strips holds its index, then u and v. write after
 * the list a shiny pass, which draws each strip again, translucent and
 * environment-mapped. the list's NJD_CE hides the pass until a caller makes
 * it NJD_CN. copy_node leaves room for the pass. no code calls it */
void chao_append_shiny_pass(Sint16 *polygons, int halfwords);
/* morph the body by the Chao's type, its leanings fly_swim and run_power, its
 * evolution and its alignment. it updates the palette, the icon's color and
 * the material, and clears CHAO_SHAPE_RESHAPE */
void chao_morph_body(struct task *chao);
/* rebuild the body as form, and reset the face, icon, animal parts and
 * perception. form must be CHAO_CHILD or an adult or Chaos form, or the call
 * returns FALSE and changes nothing */
Bool chao_change_form(struct task *chao, Uint32 form);
/* point chao_pose at the Chao's motion, and while it links, at the next */
void chao_select_body_motion(struct task *chao);
/* multiply the matrix by the node's pose from chao_pose, and step chao_pose
 * to the next node. call it for each node in depth-first order */
void chao_transform_node(struct chao_node *node);
/* draw the body, its props, its icon and its shadows. it sets
 * CHAO_BODY_IN_VIEW while the body's box is in view, and clears it otherwise.
 * it draws only while CHAO_BODY_IN_VIEW and CHAO_VISIBLE are both set */
void chao_draw_body(struct task *chao);
/* build the body of the profile's type, from CHAO_CHILD to CHAO_DARK_CHAOS,
 * and morph it. FALSE where an allocation fails */
Bool chao_build_body(struct task *chao);
/* step the body a frame. it morphs the body again where CHAO_SHAPE_RESHAPE is
 * set. it sets CHAO_SHAPE_GROUND_SHADOW while the Chao touches a ground-shadow
 * field. the step fades the hand props, then poses and springs the tree */
void chao_step_body(struct task *chao);
/* free the body chao_build_body built */
void chao_free_body(struct task *chao);

#endif
