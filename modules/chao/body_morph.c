/* the morph, which shapes a Chao's body from its form's six models, as struct
 * chao_morph_targets names them. run_power picks the power model, or where
 * negative the run model. fly_swim picks the fly model, or where negative the
 * swim model. the grown model moves toward each pick by the size of its
 * leaning. the two results mix in proportion to those sizes. evolution then
 * moves the base model toward the mix. a child's alignment then moves that
 * neutral result toward the hero child's by its size. a negative alignment
 * moves it toward the dark child's */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/palette.h"
#include "chao/body.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/model.h"
#include "chao/movement.h"
#include "chao/work.h"
#include "draw/chunk.h"
#include "system/angle.h"
#include "system/task.h"

/* the morph's cursors: destination in shape->nodes, and the others in the
 * models' node tables. all step a node at a time together. run_power and
 * fly_swim walk the picked models' tables */
static NJS_CNK_OBJECT **destination;
static NJS_CNK_OBJECT **base;
static NJS_CNK_OBJECT **grown;
static NJS_CNK_OBJECT **run_power;
static NJS_CNK_OBJECT **fly_swim;
static NJS_CNK_OBJECT **hero_base;
static NJS_CNK_OBJECT **hero_grown;
static NJS_CNK_OBJECT **hero_run_power;
static NJS_CNK_OBJECT **hero_fly_swim;
static NJS_CNK_OBJECT **dark_base;
static NJS_CNK_OBJECT **dark_grown;
static NJS_CNK_OBJECT **dark_run_power;
static NJS_CNK_OBJECT **dark_fly_swim;
static Float run_power_weight;
static Float run_remainder;
static Float run_share;
static Float fly_swim_weight;
static Float fly_remainder;
static Float fly_share;
static Float evolution;
static Float base_share;
static Float color_run;
static Float color_run_remainder;
static Float color_run_share;
static Float color_fly;
static Float color_fly_remainder;
static Float color_fly_share;
static Float color_evolution;
static Float color_base_share;
/* a child moves toward the dark child's shape, not the hero child's */
static Bool use_dark;

/* a weight of 0 becomes MIN_WEIGHT, so that the shares never divide by 0 */
#define MIN_WEIGHT 1.0e-6F
/* a child's alignment of 0 becomes MIN_ALIGNMENT */
#define MIN_ALIGNMENT 1.0e-7F
/* a color's blue, green, red and alpha bytes */
#define CHANNELS 4

#define VERTICES(slot)                                                         \
    ((struct chunk_vertex_vn *)((*(slot))->model->vlist + CHUNK_HEAD_WORDS))
#define POLYGONS(slot) ((Uint8 *)(*(slot))->model->plist)
#define POSITION(slot) ((NJS_POINT3 *)(*(slot))->pos)
#define ANGLES(slot) ((struct angles *)(*(slot))->ang)
/* one coordinate of the leanings' mix, and of a color's. evolution then moves
 * original toward the mix */
#define LEANING_MIX(adult, running, flying)                                    \
    ((run_share *                                                              \
      (((adult) * run_remainder) + ((running) * run_power_weight))) +          \
     (fly_share * (((adult) * fly_remainder) + ((flying) * fly_swim_weight))))
#define LEANING_MIX_COLOR(adult, running, flying)                              \
    ((color_run_share * (((Float)(adult) * color_run_remainder) +              \
                         ((Float)(running) * color_run))) +                    \
     (color_fly_share * (((Float)(adult) * color_fly_remainder) +              \
                         ((Float)(flying) * color_fly))))
#define EVOLVE(original, mixed)                                                \
    (((original) * base_share) + ((mixed) * evolution))
#define EVOLVE_COLOR(original, mixed)                                          \
    (((Float)(original) * color_base_share) + ((mixed) * color_evolution))
#define MIX_VERTEX(member, grown_point, run_vertex, fly_vertex)                \
    {                                                                          \
        x = LEANING_MIX((grown_point)->x, (run_vertex)->member.x,              \
                        (fly_vertex)->member.x);                               \
        y = LEANING_MIX((grown_point)->y, (run_vertex)->member.y,              \
                        (fly_vertex)->member.y);                               \
        z = LEANING_MIX((grown_point)->z, (run_vertex)->member.z,              \
                        (fly_vertex)->member.z);                               \
    }
#define MIX_POINT(adult, running, flying)                                      \
    {                                                                          \
        x = LEANING_MIX((adult)->x, (running)->x, (flying)->x);                \
        y = LEANING_MIX((adult)->y, (running)->y, (flying)->y);                \
        z = LEANING_MIX((adult)->z, (running)->z, (flying)->z);                \
    }
#define NEXT_NODE()                                                            \
    {                                                                          \
        destination++;                                                         \
        base++;                                                                \
        grown++;                                                               \
        run_power++;                                                           \
        fly_swim++;                                                            \
    }

static void morph_adult(struct chao_work *work)
{
    Uint16 *flags;
    Uint16 *end;
    int flag;
    struct chao_mesh *mesh;
    flags = &chao_morph_flags[0];
    end = &chao_morph_flags[CHAO_BODY_NODES];
    while (flags < end) {
        flag = *flags++;
        mesh = (struct chao_mesh *)(*destination)->model;
        if (mesh && (flag & MORPH_MESH)) {
            Uint16 count;
            NJS_POINT3 *rest;
            struct chunk_vertex_vn *vertex;
            struct chunk_vertex_vn *original;
            struct chunk_vertex_vn *adult;
            struct chunk_vertex_vn *running;
            struct chunk_vertex_vn *flying;
            Uint8 *diffuse;
            Uint8 *base_color;
            Uint8 *grown_color;
            Uint8 *run_color;
            Uint8 *fly_color;
            Float x;
            Float y;
            Float z;
            int i;
            count = mesh->vertex_count;
            rest = mesh->rest_points;
            /* @hack redundant: a test of count that never runs */
            if (0) {
                if (count) {
                }
            }
            vertex = mesh->vertices;
            original = VERTICES(base);
            adult = VERTICES(grown);
            running = VERTICES(run_power);
            /* @hack spelling: VERTICES(fly_swim) held in a block's local,
             * where flying could take it */
            {
                struct chunk_vertex_vn *start = VERTICES(fly_swim);
                flying = start;
            }
            for (i = 0; i < count; i++) {
                /* @hack spelling: the grown point and normal held in locals,
                 * where MIX_VERTEX could take their addresses */
                NJS_POINT3 *position = &adult->point;
                NJS_VECTOR *normal = &adult->normal;
                MIX_VERTEX(point, position, running, flying);
                rest->x = EVOLVE(original->point.x, x);
                rest->y = EVOLVE(original->point.y, y);
                rest->z = EVOLVE(original->point.z, z);
                MIX_VERTEX(normal, normal, running, flying);
                vertex->normal.x = EVOLVE(original->normal.x, x);
                vertex->normal.y = EVOLVE(original->normal.y, y);
                vertex->normal.z = EVOLVE(original->normal.z, z);
                /* @bug logic: an adult's morphed normals go to its vertices,
                 * not to rest_normals, so write_morph_tree puts the built
                 * normals back over them (0x0c50a5ea) */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: the morphed normals
                 * go to rest_normals too */
                mesh->rest_normals[i] = vertex->normal;
#endif
                rest++;
                vertex++;
                original++;
                adult++;
                running++;
                flying++;
            }
            /* @hack spelling: the lists held in blocks' locals and stepped to
             * their colors apart, where morph_child's POLYGONS(slot) +
             * CHAO_DIFFUSE_OFFSET would do */
            {
                Uint8 *header = POLYGONS(destination);
                diffuse = header;
            }
            {
                Uint8 *header = POLYGONS(base);
                base_color = header;
            }
            grown_color = POLYGONS(grown);
            run_color = POLYGONS(run_power);
            fly_color = POLYGONS(fly_swim);
            diffuse += CHAO_DIFFUSE_OFFSET;
            base_color = base_color + CHAO_DIFFUSE_OFFSET;
            grown_color = grown_color + CHAO_DIFFUSE_OFFSET;
            run_color += CHAO_DIFFUSE_OFFSET;
            fly_color += CHAO_DIFFUSE_OFFSET;
            for (i = 0; i < CHANNELS; i++) {
                *diffuse =
                    LEANING_MIX_COLOR(*grown_color, *run_color, *fly_color);
                *diffuse = EVOLVE_COLOR(*base_color, (Float)*diffuse);
                diffuse++;
                base_color++;
                grown_color++;
                run_color++;
                fly_color++;
            }
        }
        if (flag & MORPH_JOINT) {
            NJS_POINT3 *point;
            NJS_POINT3 *original;
            NJS_POINT3 *adult;
            NJS_POINT3 *running;
            NJS_POINT3 *flying;
            struct angles *angle;
            struct angles *base_angle;
            struct angles *grown_angle;
            struct angles *run_angle;
            struct angles *fly_angle;
            Float x;
            Float y;
            Float z;
            point = POSITION(destination);
            /* @hack spelling: POSITION(base) held in a block's local, where
             * original could take it */
            {
                NJS_POINT3 *start = POSITION(base);
                original = start;
            }
            adult = POSITION(grown);
            running = POSITION(run_power);
            flying = POSITION(fly_swim);
            x = LEANING_MIX(adult->x, running->x, flying->x);
            /* @hack redundant: a read of adult->y, which nothing uses */
            y = ((void)adult->y, LEANING_MIX(adult->y, running->y, flying->y));
            z = LEANING_MIX(adult->z, running->z, flying->z);
            point->x = EVOLVE(original->x, x);
            point->y = EVOLVE(original->y, y);
            point->z = EVOLVE(original->z, z);
            angle = ANGLES(destination);
            base_angle = ANGLES(base);
            grown_angle = ANGLES(grown);
            run_angle = ANGLES(run_power);
            fly_angle = ANGLES(fly_swim);
            x = LEANING_MIX((Float)grown_angle->x, (Float)run_angle->x,
                            (Float)fly_angle->x);
            y = LEANING_MIX((Float)grown_angle->y, (Float)run_angle->y,
                            (Float)fly_angle->y);
            z = LEANING_MIX((Float)grown_angle->z, (Float)run_angle->z,
                            (Float)fly_angle->z);
            angle->x = (Angle)EVOLVE((Float)base_angle->x, x);
            angle->y = (Angle)EVOLVE((Float)base_angle->y, y);
            angle->z = (Angle)EVOLVE((Float)base_angle->z, z);
        }
        NEXT_NODE();
    }
}

/* a child's shape is its neutral result moved toward its alignment's result
 * by the alignment's size. the macros keep the two results in neutral_x to _z
 * and other_x to _z */
#define EVOLVE_VECTOR(out, original)                                           \
    {                                                                          \
        out##_x = EVOLVE((original).x, x);                                     \
        out##_y = EVOLVE((original).y, y);                                     \
        out##_z = EVOLVE((original).z, z);                                     \
    }
#define ALIGN(neutral, other)                                                  \
    (((neutral) * neutral_share) + ((other) * alignment))
#define STORE_ALIGNED(point)                                                   \
    {                                                                          \
        (point)->x = ALIGN(neutral_x, other_x);                                \
        (point)->y = ALIGN(neutral_y, other_y);                                \
        (point)->z = ALIGN(neutral_z, other_z);                                \
    }
#define SELECT_VERTICES(group)                                                 \
    {                                                                          \
        other_base = VERTICES(group##_base);                                   \
        other_grown = VERTICES(group##_grown);                                 \
        other_run = VERTICES(group##_run_power);                               \
        other_fly = VERTICES(group##_fly_swim);                                \
    }
#define SELECT_COLORS(group)                                                   \
    {                                                                          \
        other_base_color = POLYGONS(group##_base) + CHAO_DIFFUSE_OFFSET;       \
        other_grown_color = POLYGONS(group##_grown) + CHAO_DIFFUSE_OFFSET;     \
        other_run_color = POLYGONS(group##_run_power) + CHAO_DIFFUSE_OFFSET;   \
        other_fly_color = POLYGONS(group##_fly_swim) + CHAO_DIFFUSE_OFFSET;    \
    }
#define SELECT_POSITIONS(group)                                                \
    {                                                                          \
        other_base_pos = POSITION(group##_base);                               \
        selected_grown = (NJS_POINT3 *)&(*group##_grown)->pos[0];              \
        other_run_pos = POSITION(group##_run_power);                           \
        selected_fly = POSITION(group##_fly_swim);                             \
    }
/* @hack spelling: an angle table taken as &ang[0] and as ang, where ang
 * would do for each */
#define SELECT_ANGLES(group)                                                   \
    {                                                                          \
        other_base_angle = &(*group##_base)->ang[0];                           \
        other_grown_angle = &(*group##_grown)->ang[0];                         \
        other_run_angle = &(*group##_run_power)->ang[0];                       \
        other_fly_angle = (*group##_fly_swim)->ang;                            \
    }
#define MIX_ROTATION(axis)                                                     \
    ((Angle)((run_share * (Float)run_rotation.v[axis]) +                       \
             (fly_share * (Float)fly_rotation.v[axis])))
#define BLEND_ROTATION(out, from, adult, running, flying)                      \
    {                                                                          \
        garden_blend_angles((adult), (running), run_power_weight,              \
                            (Angle *)&run_rotation);                           \
        garden_blend_angles((adult), (flying), fly_swim_weight,                \
                            (Angle *)&fly_rotation);                           \
        (out).v[0] = MIX_ROTATION(0);                                          \
        (out).v[1] = MIX_ROTATION(1);                                          \
        (out).v[2] = MIX_ROTATION(2);                                          \
        garden_blend_angles((from), (Angle *)&(out), evolution,                \
                            (Angle *)&(out));                                  \
    }
static void morph_child(struct chao_work *work)
{
    Uint16 *flags;
    Uint16 *end;
    int flag;
    struct chao_mesh *mesh;
    struct chao_parameters *parameters;
    Float alignment;
    Float neutral_share;
    /* @hack redundant: a test of work that never runs */
    if (0) {
        if (work) {
        }
    }
    parameters = &work->profile.parameters;
    alignment = parameters->alignment;
    if (alignment >= 0.0F) {
        use_dark = FALSE;
    } else {
        alignment *= -1.0F;
        use_dark = TRUE;
    }
    /* @bug uninitialized: node 0's joint blend reads neutral_share before a
     * mesh's blend sets it, since chao_morph_flags[0] has no MORPH_MESH
     * (0x0c509358) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: neutral_share starts at the
     * blend's share before the first mesh */
    neutral_share = 1.0F - alignment;
#endif
    flags = &chao_morph_flags[0];
    end = &chao_morph_flags[CHAO_BODY_NODES];
    while (flags < end) {
        flag = *flags++;
        mesh = (struct chao_mesh *)(*destination)->model;
        if (mesh && (flag & MORPH_MESH)) {
            Uint16 count;
            NJS_POINT3 *rest;
            NJS_VECTOR *normal;
            struct chunk_vertex_vn *original;
            struct chunk_vertex_vn *adult;
            struct chunk_vertex_vn *running;
            struct chunk_vertex_vn *flying;
            struct chunk_vertex_vn *other_base;
            struct chunk_vertex_vn *other_grown;
            struct chunk_vertex_vn *other_run;
            struct chunk_vertex_vn *other_fly;
            Uint8 *diffuse;
            Uint8 *base_color;
            Uint8 *grown_color;
            Uint8 *run_color;
            Uint8 *fly_color;
            Uint8 *other_base_color;
            Uint8 *other_grown_color;
            Uint8 *other_run_color;
            Uint8 *other_fly_color;
            Float x;
            Float y;
            Float z;
            Float neutral_x;
            Float neutral_y;
            Float neutral_z;
            Float other_x;
            Float other_y;
            Float other_z;
            int i;
            count = mesh->vertex_count;
            rest = mesh->rest_points;
            normal = mesh->rest_normals;
            original = VERTICES(base);
            adult = VERTICES(grown);
            running = VERTICES(run_power);
            flying = VERTICES(fly_swim);
            if (!use_dark) {
                SELECT_VERTICES(hero);
            } else {
                SELECT_VERTICES(dark);
            }
            if (alignment == 0.0F) {
                alignment = MIN_ALIGNMENT;
            }
            neutral_share = 1.0F - alignment;
            for (i = 0; i < count; i++) {
                /* @hack spelling: the grown points and normals held in
                 * grown_point and grown_normal, where MIX_VERTEX could take
                 * their addresses */
                NJS_POINT3 *grown_point = &adult->point;
                NJS_VECTOR *grown_normal;
                MIX_VERTEX(point, grown_point, running, flying);
                EVOLVE_VECTOR(neutral, original->point);
                grown_point = &other_grown->point;
                MIX_VERTEX(point, grown_point, other_run, other_fly);
                EVOLVE_VECTOR(other, other_base->point);
                STORE_ALIGNED(rest);
                grown_normal = &adult->normal;
                MIX_VERTEX(normal, grown_normal, running, flying);
                EVOLVE_VECTOR(neutral, original->normal);
                grown_normal = &other_grown->normal;
                MIX_VERTEX(normal, grown_normal, other_run, other_fly);
                EVOLVE_VECTOR(other, other_base->normal);
                STORE_ALIGNED(normal);
                rest++;
                normal++;
                original++;
                adult++;
                running++;
                flying++;
                other_base++;
                other_grown++;
                other_run++;
                other_fly++;
            }
            diffuse = POLYGONS(destination) + CHAO_DIFFUSE_OFFSET;
            base_color = POLYGONS(base) + CHAO_DIFFUSE_OFFSET;
            grown_color = POLYGONS(grown) + CHAO_DIFFUSE_OFFSET;
            run_color = POLYGONS(run_power) + CHAO_DIFFUSE_OFFSET;
            fly_color = POLYGONS(fly_swim) + CHAO_DIFFUSE_OFFSET;
            if (!use_dark) {
                SELECT_COLORS(hero);
            } else {
                SELECT_COLORS(dark);
            }
            for (i = 0; i < CHANNELS; i++) {
                x = LEANING_MIX_COLOR(*grown_color, *run_color, *fly_color);
                neutral_x = EVOLVE_COLOR(*base_color, x);
                x = LEANING_MIX_COLOR(*other_grown_color, *other_run_color,
                                      *other_fly_color);
                other_x = EVOLVE_COLOR(*other_base_color, x);
                *diffuse = ALIGN(neutral_x, other_x);
                diffuse++;
                base_color++;
                grown_color++;
                run_color++;
                fly_color++;
                other_base_color++;
                other_grown_color++;
                other_run_color++;
                other_fly_color++;
            }
        }
        {
            NJS_POINT3 *point;
            NJS_POINT3 *original;
            NJS_POINT3 *adult;
            NJS_POINT3 *running;
            NJS_POINT3 *flying;
            NJS_POINT3 *other_base_pos;
            NJS_POINT3 *other_grown_pos;
            NJS_POINT3 *other_run_pos;
            NJS_POINT3 *other_fly_pos;
            Angle *angle;
            Angle *base_angle;
            Angle *grown_angle;
            Angle *run_angle;
            Angle *fly_angle;
            Angle *other_base_angle;
            Angle *other_grown_angle;
            Angle *other_run_angle;
            Angle *other_fly_angle;
            Angle *first_angle;
            NJS_POINT3 *selected_grown;
            NJS_POINT3 *selected_fly;
            struct morph_rotation {
                Angle v[3];
            };
            struct morph_rotation run_rotation;
            struct morph_rotation fly_rotation;
            struct morph_rotation neutral_rotation;
            struct morph_rotation other_rotation;
            Float x;
            Float y;
            Float z;
            Float neutral_x;
            Float neutral_y;
            Float neutral_z;
            Float other_x;
            Float other_y;
            Float other_z;
            point = POSITION(destination);
            /* @hack spelling: the positions as &pos[0], where POSITION would
             * do */
            original = (NJS_POINT3 *)&(*base)->pos[0];
            adult = (NJS_POINT3 *)&(*grown)->pos[0];
            running = (NJS_POINT3 *)&(*run_power)->pos[0];
            flying = (NJS_POINT3 *)&(*fly_swim)->pos[0];
            if (!use_dark) {
                SELECT_POSITIONS(hero);
            } else {
                SELECT_POSITIONS(dark);
            }
            /* @hack spelling: the other alignment's grown and fly positions
             * held in selected_grown and selected_fly, where other_grown_pos
             * and other_fly_pos could take them */
            other_fly_pos = selected_fly;
            /* @hack redundant: two tests of selected_grown that never run */
            if (0) {
                if (selected_grown) {
                }
                if (selected_grown) {
                }
            }
            other_grown_pos = selected_grown;
            MIX_POINT(adult, running, flying);
            EVOLVE_VECTOR(neutral, *original);
            MIX_POINT(other_grown_pos, other_run_pos, other_fly_pos);
            EVOLVE_VECTOR(other, *other_base_pos);
            STORE_ALIGNED(point);
            angle = (*destination)->ang;
            /* @hack spelling: base's angles held in first_angle, where
             * base_angle could take them */
            first_angle = (*base)->ang;
            base_angle = first_angle;
            grown_angle = (*grown)->ang;
            run_angle = (*run_power)->ang;
            fly_angle = (*fly_swim)->ang;
            if (!use_dark) {
                SELECT_ANGLES(hero);
            } else {
                SELECT_ANGLES(dark);
            }
            BLEND_ROTATION(neutral_rotation, base_angle, grown_angle, run_angle,
                           fly_angle);
            BLEND_ROTATION(other_rotation, other_base_angle, other_grown_angle,
                           other_run_angle, other_fly_angle);
            garden_blend_angles((Angle *)&neutral_rotation,
                                (Angle *)&other_rotation, alignment, angle);
        }
        NEXT_NODE();
        hero_base++;
        hero_grown++;
        hero_run_power++;
        hero_fly_swim++;
        dark_base++;
        dark_grown++;
        dark_run_power++;
        dark_fly_swim++;
    }
}

static void write_morph_tree(struct task *chao, struct chao_node *node);

/* set a color's weight to value, held between -1 and 1 */
#define CLAMP_WEIGHT(weight, value)                                            \
    if ((value) > 1.0F) {                                                      \
        (weight) = 1.0F;                                                       \
    } else if ((value) < -1.0F) {                                              \
        (weight) = -1.0F;                                                      \
    } else {                                                                   \
        (weight) = (value);                                                    \
    }

void chao_morph_body(struct task *chao)
{
    struct chao_work *work;
    struct chao_parameters *parameters;
    struct chao_shape_work *shape;
    work = (struct chao_work *)chao->work;
    shape = &work->shape;
    parameters = &work->profile.parameters;
    run_power_weight = parameters->run_power;
    fly_swim_weight = parameters->fly_swim;
    destination = shape->nodes;
    switch (parameters->type) {
    case CHAO_CHILD:
        base = shape->form->base;
        grown = shape->form->grown;
        hero_base = shape->hero->base;
        hero_grown = shape->hero->grown;
        dark_base = shape->dark->base;
        dark_grown = shape->dark->grown;
        if (run_power_weight >= 0.0F) {
            run_power = shape->form->power;
            hero_run_power = shape->hero->power;
            dark_run_power = shape->dark->power;
        } else {
            run_power = shape->form->run;
            hero_run_power = shape->hero->run;
            dark_run_power = shape->dark->run;
            run_power_weight *= -1.0F;
        }
        if (fly_swim_weight >= 0.0F) {
            fly_swim = shape->form->fly;
            hero_fly_swim = shape->hero->fly;
            dark_fly_swim = shape->dark->fly;
        } else {
            fly_swim = shape->form->swim;
            hero_fly_swim = shape->hero->swim;
            dark_fly_swim = shape->dark->swim;
            fly_swim_weight *= -1.0F;
        }
        break;
    default:
        base = shape->form->base;
        grown = shape->form->grown;
        if (run_power_weight >= 0.0F) {
            run_power = shape->form->power;
        } else {
            run_power = shape->form->run;
            run_power_weight *= -1.0F;
        }
        if (fly_swim_weight >= 0.0F) {
            fly_swim = shape->form->fly;
        } else {
            fly_swim = shape->form->swim;
            fly_swim_weight *= -1.0F;
        }
        break;
    }
    evolution = parameters->evolution;
    if (run_power_weight == 0.0F) {
        run_power_weight = MIN_WEIGHT;
    }
    if (fly_swim_weight == 0.0F) {
        fly_swim_weight = MIN_WEIGHT;
    }
    run_remainder = 1.0F - run_power_weight;
    fly_remainder = 1.0F - fly_swim_weight;
    base_share = 1.0F - evolution;
    run_share = fly_share = 1.0F / (run_power_weight + fly_swim_weight);
    run_share *= run_power_weight;
    fly_share *= fly_swim_weight;
    CLAMP_WEIGHT(color_run, run_power_weight);
    CLAMP_WEIGHT(color_fly, fly_swim_weight);
    CLAMP_WEIGHT(color_evolution, evolution);
    color_run_remainder = 1.0F - color_run;
    color_fly_remainder = 1.0F - color_fly;
    color_base_share = 1.0F - color_evolution;
    color_run_share = color_fly_share =
        1.0F / (run_power_weight + fly_swim_weight);
    color_run_share *= color_run;
    color_fly_share *= color_fly;
    switch (parameters->type) {
    case CHAO_CHILD:
        chao_update_palette(parameters, chao->work->tick);
        chao_update_icon_color(chao);
        morph_child(work);
        break;
    default:
        chao_update_palette(parameters, chao->work->tick);
        chao_update_icon_color(chao);
        morph_adult(work);
        break;
    }
    write_morph_tree(chao, shape->root);
    chao_apply_body_material(chao);
    shape->flags &= ~CHAO_SHAPE_RESHAPE;
}

static void write_morph_tree(struct task *chao, struct chao_node *node)
{
    struct chao_mesh *mesh;
    /* @hack redundant: register on the walk's cursors, where plain locals would
     * do */
    register struct chunk_vertex_vn *vertex;
    register struct chunk_vertex_vn *end;
    register NJS_POINT3 *point;
    register NJS_VECTOR *normal;
    mesh = (struct chao_mesh *)node->object.model;
    if (mesh) {
        vertex = mesh->vertices;
        end = vertex + mesh->vertex_count;
        point = mesh->rest_points;
        normal = mesh->rest_normals;
        for (; vertex < end; vertex++) {
            vertex->point = *point;
            vertex->normal = *normal;
            point++;
            normal++;
        }
    }
    if (node->object.child) {
        write_morph_tree(chao, CHAO_NODE_CHILD(node));
    }
    if (node->object.sibling) {
        write_morph_tree(chao, CHAO_NODE_SIBLING(node));
    }
}
