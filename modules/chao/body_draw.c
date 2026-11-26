/* a garden Chao's draw: its body with what it wears and holds, then its
 * shadows. the farther the Chao is, the fewer shadows it draws */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/palette.h"
#include "chao/body.h"
#ifdef NONMATCHING
/* chao_body_has_matrix_room, by which this side's draw tests the matrix
 * stack */
#include "chao/body_matrix.h"
#endif
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/model.h"
#include "chao/movement.h"
#include "chao/parts_art.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "draw/lines.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define MODIFIER_SHADOW (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
/* chao_eyelid_angle's angle for an open eye, whose lid isn't drawn */
#define LID_OPEN (-QUARTER_TURN)
/* the distance draw_shadows takes for a Chao with no garden entry */
#define UNTRACKED_DISTANCE 300.0F
/* how far above the Chao's position the hands' shadows and the icon's lie
 * under CHAO_SHAPE_GROUND_SHADOW */
#define HAND_SHADOW_LIFT 0.01F
#define ICON_SHADOW_LIFT 0.001F
/* the body leans by body_angle about a point PIVOT_HEIGHT above its position. a
 * held Chao draws HELD_DROP lower */
#define PIVOT_HEIGHT 1.0F
#define HELD_DROP 1.0F

extern NJS_CNK_OBJECT chao_hat_1, chao_hat_3;
extern NJS_CNK_OBJECT *chao_costume_models[];

static Sint16 drawing_node;
static Bool easy_draw;

#define BODY_TEXTURE(shape)                                                    \
    (((shape)->flags & CHAO_SHAPE_JEWEL) ? &chao_env_texlist                   \
                                         : &chao_body_texlist)
/* the nodes a hat hides: the eyes, the lids, the mouth and the head's other
 * features */
#define HAT_HIDES_NODE(n)                                                      \
    ((n) == CHAO_NODE_EYE_0 || (n) == CHAO_NODE_EYE_1 ||                       \
     (n) == CHAO_NODE_LID_0 || (n) == CHAO_NODE_LID_1 || (n) == 23 ||          \
     (n) == 25 || (n) == 24 || (n) == 26 || (n) == 30 || (n) == 31 ||          \
     (n) == CHAO_NODE_MOUTH || (n) == 29)
/* the nodes a costume hides: the feet, the tail and the wings */
#define COSTUME_HIDES_NODE(n)                                                  \
    ((n) == 37 || (n) == 39 || (n) == 6 || (n) == 13 || (n) == 8)
#define DRAW_MESH(mesh)                                                        \
    {                                                                          \
        if (easy_draw) {                                                       \
            njCnkEasyDrawModel(mesh);                                          \
        } else {                                                               \
            njCnkSimpleDrawModel(mesh);                                        \
        }                                                                      \
    }

static void draw_node(struct task *chao, struct chao_node *node)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_shape_work *shape = &work->shape;
    struct chao_parameters *parameters = &work->profile.parameters;
    NJS_CNK_MODEL *mesh = node->object.model;
    Sint32 (*bind)(NJS_TEXLIST *);
    NJS_TEXLIST *textures;
    Uint8 *color;
    struct chao_face *face;
    Angle closure;
    Angle slope;
    Angle lid_angle;
    struct angles *angles;
    Float scale;
    Float alpha;
    Float red;
    Float green;
    Float blue;

    njPushMatrixEx();
    chao_transform_node(node);
    /* @hack spelling: njSetTexture called through a pointer, where direct
     * calls would do */
    bind = njSetTexture;
    if (node->prop.object) {
        scale = node->prop.scale;
        bind(node->prop.textures);
        njPushMatrixEx();
        if (node->placed) {
            angles = &node->prop_angles;
            njTranslateEx(&node->prop_position);
            njRotateXYZ(NULL, angles->x, angles->y, angles->z);
        }
        njScale(NULL, scale, scale, scale);
        njCnkSimpleDrawObject(node->prop.object);
        njPopMatrixEx();
    }
    if (drawing_node == CHAO_NODE_HEAD && parameters->hat) {
        bind(&chao_body_texlist);
        /* @hack spelling: the hat read again through the work, where
         * parameters->hat would do */
        switch (CHAO_WORK(chao)->profile.parameters.hat) {
        case 1:
            njCnkEasyDrawObject(&chao_hat_1);
            break;
        case 2:
            njCnkEasyDrawObject(&chao_hat_2);
            break;
        case 3:
        default:
            njCnkEasyDrawObject(&chao_hat_3);
            break;
        }
    } else if (drawing_node == CHAO_NODE_TRUNK && parameters->costume &&
               parameters->type < CHAO_CHAOS) {
        bind(&chao_body_texlist);
        chao_bind_palette(chao, work->base.tick);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        color = (Uint8 *)mesh->plist + CHAO_DIFFUSE_OFFSET;
        blue = color[0] * (1.0F / 255.0F);
        green = color[1] * (1.0F / 255.0F);
        red = color[2] * (1.0F / 255.0F);
        alpha = color[3] * (1.0F / 255.0F);
        constant_material_set(alpha, red, green, blue);
        njCnkSimpleDrawObject(chao_costume_models[parameters->type]);
        draw_control_clear(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
#ifdef NONMATCHING
        /* this side draws no medal past chao_medal_models, nor one without a
         * model, which the matching side draws through */
    } else if (drawing_node == CHAO_NODE_MEDAL && parameters->medal > 0 &&
               parameters->medal <
                   sizeof(chao_medal_models) / sizeof(chao_medal_models[0]) &&
               chao_medal_models[parameters->medal] != NULL) {
#else
    } else if (drawing_node == CHAO_NODE_MEDAL && parameters->medal > 0) {
#endif
        bind(&chao_jewel_texlist);
        njCnkSimpleDrawObject(chao_medal_models[parameters->medal]);
    } else if (parameters->hat && HAT_HIDES_NODE(drawing_node)) {
    } else if (parameters->costume && parameters->type < CHAO_CHAOS &&
               COSTUME_HIDES_NODE(drawing_node)) {
    } else if (node->animal_part && parameters->type < CHAO_CHAOS) {
        bind(&chao_mini_parts_texlist);
        njCnkEasyDrawObject(node->animal_part);
    } else if (mesh) {
        slope = chao_eyelid_slope(chao);
        closure = chao_eyelid_angle(chao);
        if (drawing_node == CHAO_NODE_LID_0) {
            textures = BODY_TEXTURE(shape);
            bind(textures);
            chao_bind_palette(chao, work->base.tick);
            njRotateZ(NULL, slope);
            njRotateX(NULL, closure);
        } else if (drawing_node == CHAO_NODE_LID_1) {
            textures = BODY_TEXTURE(shape);
            bind(textures);
            chao_bind_palette(chao, work->base.tick);
            njRotateZ(NULL, -slope);
            njRotateX(NULL, closure);
        } else if (drawing_node == CHAO_NODE_MOUTH) {
            bind(&chao_mouth_texlist);
        } else if (drawing_node == CHAO_NODE_EYE_0 ||
                   drawing_node == CHAO_NODE_EYE_1) {
            bind(&chao_eye_texlist);
            /* @hack spelling: face set in each arm, where once before them
             * would do */
            if (drawing_node == CHAO_NODE_EYE_0) {
                face = &CHAO_WORK(chao)->face;
                njTranslate(NULL, face->eye_x, face->eye_y, 0.0F);
            } else {
                face = &CHAO_WORK(chao)->face;
                njTranslate(NULL, -face->eye_x, face->eye_y, 0.0F);
            }
        } else {
            textures = BODY_TEXTURE(shape);
            bind(textures);
            chao_bind_palette(chao, work->base.tick);
        }
        if (drawing_node == CHAO_NODE_LID_0 ||
            drawing_node == CHAO_NODE_LID_1) {
            lid_angle = chao_eyelid_angle(chao);
            if (lid_angle != LID_OPEN) {
                DRAW_MESH(mesh);
            }
        } else {
            DRAW_MESH(mesh);
        }
    }
    ++drawing_node;
    if (node->object.child) {
        draw_node(chao, CHAO_NODE_CHILD(node));
    }
    njPopMatrixEx();
    if (node->object.sibling) {
        draw_node(chao, CHAO_NODE_SIBLING(node));
    }
}

#ifdef NONMATCHING
/* the plain forms of the matching side's hacks. the shadow's model is named by
 * its address, and the tuning's field by its name */
#define BODY_SHADOW (&garden_shadow_model)
#define TUNING(field) (chao_tuning.field)
#else
/* @hack type: the shadow's model as a one-model array, where its address
 * would do */
struct shadow_mesh {
    NJS_CNK_MODEL model[1];
};
#define BODY_SHADOW (((struct shadow_mesh *)&garden_shadow_model)->model)
/* @hack spelling: a member read at its byte offset from chao_tuning, where
 * chao_tuning.field would do */
#define TUNING(field)                                                          \
    (*(Float *)((Uint8 *)&chao_tuning + (Uint32) &                             \
                ((struct chao_tuning *)0)->field))
#endif

#define SHADOW_AT(position, yaw, x, y, z)                                      \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njTranslateEx(position);                                               \
        njRotateY(NULL, yaw);                                                  \
        njScale(NULL, x, y, z);                                                \
        njCnkModDrawModel(BODY_SHADOW);                                        \
        njPopMatrixEx();                                                       \
    }
#define HAND_SHADOW(point)                                                     \
    {                                                                          \
        njPushMatrixEx();                                                      \
        if (shape->flags & CHAO_SHAPE_GROUND_SHADOW) {                         \
            (point).y = work->base.pos.y + HAND_SHADOW_LIFT;                   \
        }                                                                      \
        njTranslateEx(&(point));                                               \
        njRotateY(NULL, work->base.ang.y);                                     \
        njScale(NULL, 0.65F, 0.6F, 0.65F);                                     \
        njCnkModDrawModel(BODY_SHADOW);                                        \
        njPopMatrixEx();                                                       \
    }
static void draw_shadows(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_shape_work *shape = &work->shape;
    struct garden_entry *entry = chao->force.p;
    Float x;
    Float y;
    Float z;
    struct task_work *base = chao->work;
    struct task *actor;
    Float distance;
    NJS_POINT3 *icon;
    if (entry) {
        distance = entry->distance;
    } else {
        distance = UNTRACKED_DISTANCE;
    }
    if (chao_current_area() == CHAO_AREA_RACE) {
        SHADOW_AT(&shape->head_position, base->ang.y, 1.6F, 0.7F, 1.5F);
        /* @hack spelling: the body's shadow drawn out, its scale held in z, y
         * and x, where SHADOW_AT would do */
        njPushMatrixEx();
        njTranslateEx(&shape->body_position);
        njRotateY(NULL, base->ang.y);
        z = 1.4F;
        y = 0.7F;
        x = 1.4F;
        njScale(NULL, x, y, z);
        njCnkModDrawModel(BODY_SHADOW);
        njPopMatrixEx();
        return;
    }
    /* @hack spelling: a copy of chao, where chao would do */
    actor = chao;
    if (TUNING(body_shadow_distance) > distance) {
        SHADOW_AT(&shape->head_position, work->base.ang.y, 1.6F, 0.7F, 1.5F);
        if (TUNING(object_shadow_distance) > distance) {
            HAND_SHADOW(shape->hand_4);
            HAND_SHADOW(shape->hand_11);
            if (TUNING(full_shadow_distance) > distance) {
                SHADOW_AT(&shape->body_position, work->base.ang.y, 1.4F, 0.7F,
                          1.4F);
                if (!CHAO_WORK(actor)->profile.parameters.costume) {
                    SHADOW_AT(&shape->foot_6, work->base.ang.y, 0.6F, 0.6F,
                              0.9F);
                    SHADOW_AT(&shape->foot_13, work->base.ang.y, 0.6F, 0.6F,
                              0.9F);
                }
                njPushMatrixEx();
                icon = &CHAO_WORK(actor)->icon.lower.position;
                if (shape->flags & CHAO_SHAPE_GROUND_SHADOW) {
                    njTranslate(NULL, icon->x,
                                work->base.pos.y + ICON_SHADOW_LIFT, icon->z);
                } else {
                    njTranslateEx(icon);
                }
                njRotateY(NULL, work->base.ang.y);
                njScale(NULL, 0.5F, 0.7F, 0.5F);
                njCnkModDrawModel(BODY_SHADOW);
                njPopMatrixEx();
            }
        }
    }
}

#define HAND_PROP(prop, point)                                                 \
    {                                                                          \
        if ((prop).object) {                                                   \
            scale = (prop).scale;                                              \
            njSetTexture((prop).textures);                                     \
            njPushMatrixEx();                                                  \
            njTranslateEx(&(point));                                           \
            njRotateY(NULL, chao->work->ang.y);                                \
            njScale(NULL, scale, scale, scale);                                \
            njCnkSimpleDrawObject((prop).object);                              \
            njPopMatrixEx();                                                   \
        }                                                                      \
    }

void chao_draw_body(struct task *chao)
{
    struct task_work *base = chao->work;
    struct chao_work *work = (struct chao_work *)base;
    struct chao_shape_work *shape = &work->shape;
    struct chao_parameters *parameters;
    Float scale;
    Float pivot;
#ifdef NONMATCHING
    /* this side draws no body where Ninja's matrix stack can't hold its tree
     * and the draw's frames */
    if (!chao_body_has_matrix_room((NJS_CNK_OBJECT *)shape->root,
                                   CHAO_DRAW_FRAMES)) {
        return;
    }
#endif
    if (!garden_visible_box(chao, 5.2F, 4.5F, 2.9F)) {
        CHAO_WORK(chao)->flags &= ~CHAO_BODY_IN_VIEW;
        return;
    }
    CHAO_WORK(chao)->flags |= CHAO_BODY_IN_VIEW;
    if (!(CHAO_WORK(chao)->flags & CHAO_VISIBLE)) {
        return;
    }
    parameters = &work->profile.parameters;
    /* only a hero adult with neither a texture nor a color draws its meshes
     * by njCnkEasyDrawModel */
    easy_draw = FALSE;
    switch (parameters->type) {
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
        if (parameters->texture == 0) {
            easy_draw = TRUE;
        }
        break;
    }
    if (parameters->color != 0) {
        easy_draw = FALSE;
    }
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR |
                       NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
    draw_attr_save();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    drawing_node = 0;
    chao_select_body_motion(chao);
    njSetTexture(&chao_body_texlist);
    chao_bind_palette(chao, base->tick);
    if (shape->flags & CHAO_SHAPE_GROUND_SHADOW) {
        draw_control_set(MODIFIER_SHADOW);
    }
    njPushMatrixEx();
    if (base->flags & WORK_HELD_BY_PLAYER) {
        njTranslate(NULL, 0.0F, -HELD_DROP, 0.0F);
    }
    njTranslateV(NULL, &base->pos);
    njRotateZ(NULL, base->ang.z);
    njRotateX(NULL, base->ang.x);
    njRotateY(NULL, base->ang.y);
    /* @hack spelling: the pivot's height held in pivot, where PIVOT_HEIGHT
     * would do */
    pivot = PIVOT_HEIGHT;
    njTranslate(NULL, 0.0F, -PIVOT_HEIGHT, 0.0F);
    njRotateX(NULL, work->body_angle);
    njTranslate(NULL, 0.0F, pivot, 0.0F);
    draw_node(chao, work->shape.root);
    njPopMatrixEx();
    HAND_PROP(shape->prop_4, shape->hand_4);
    HAND_PROP(shape->prop_11, shape->hand_11);
    chao_draw_icon(chao);
    draw_shadows(chao);
    draw_control_clear(MODIFIER_SHADOW);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}
