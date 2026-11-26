/* the train, which Mad Space's carriage manager makes. its engine makes a
 * trailer behind itself, and the two run round a path. the engine's
 * headlights fade with the way they face. a lamp runs back and forth along the
 * engine. stg44/carriage_path_impl.h gives the cars their init and destructor,
 * and stg44/carriage_step_impl.h their step */
#include "stg44/carriage/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "effect/particle.h"
#include "shared/common_object.h"
#include "stg44/scenery/carriage_chunks.h"
#include "stg44/scenery_shapes.h"
#include "system/angle.h"
#include "stg44/props.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define CARRIAGE_INIT mad_space_carriage_init
#define LEAD_COLLISION mad_space_engine_collision
#define TRAILER_COLLISION mad_space_trailer_collision
#define CARRIAGE_ANGLES current_matrix_angles_xyz
#define LOOK_AHEAD 30.0F
#define HEAD_START 80.0F
#define NEAR_RANGE 250.0F
#define PIECE_HEIGHT 35.0F
#define PIECE_OFFSET (-25.0F)
/* the lamp stands at one of LIGHT_PLACES places, LIGHT_SPACING apart along z.
 * it moves to the next each LIGHT_PERIOD steps, out to the last and back. its
 * places repeat every LIGHT_PHASES moves. a copy at the place it left fades out
 * over the first half of the period */
#define LIGHT_PERIOD 15
#define LIGHT_SPACING 11.5F
#define LIGHT_PLACES 5
#define LIGHT_PHASES (2 * (LIGHT_PLACES - 1))
/* a headlight's flare turns a full turn for each ROLL_DISTANCE from the
 * camera to the headlight. that distance counts the headlight's depth
 * DEPTH_WEIGHT as much as its other offsets. the flare draws FLARE_DEPTH
 * nearer than the headlight */
#define ROLL_DISTANCE 120.0F
#define DEPTH_WEIGHT 0.001F
#define FLARE_DEPTH 10.0F
/* a flare's color is WHITE_RGB with an alpha. the alpha is OPAQUE_ALPHA times
 * the z of the headlight's direction in the view. it is held from 0 to
 * OPAQUE_ALPHA */
#define OPAQUE_ALPHA 255
#define WHITE_RGB 0x00ffffff

static NJS_TEXNAME mad_space_engine_names[11] = {
#include "stg44/carriage/mad_space_engine_names.inc"
};

NJS_TEXLIST mad_space_engine_texlist = {
#include "stg44/carriage/mad_space_engine_texlist.inc"
};

static Sint16 mad_space_engine_body_1_polygons[39] = {
#include "stg44/carriage/mad_space_engine_body_1_polygons.inc"
};

static Sint32 mad_space_engine_body_1_vertices[35] = {
#include "stg44/carriage/mad_space_engine_body_1_vertices.inc"
};

static NJS_CNK_MODEL mad_space_engine_body_1_model = {
#include "stg44/carriage/mad_space_engine_body_1_model.inc"
};

static NJS_CNK_OBJECT mad_space_engine_body_1 = {
#include "stg44/carriage/mad_space_engine_body_1.inc"
};

static Sint16 mad_space_engine_body_polygons[1289] = {
#include "stg44/carriage/mad_space_engine_body_polygons.inc"
};

static Sint32 mad_space_engine_body_vertices[987] = {
#include "stg44/carriage/mad_space_engine_body_vertices.inc"
};

static NJS_CNK_MODEL mad_space_engine_body_model = {
#include "stg44/carriage/mad_space_engine_body_model.inc"
};

NJS_CNK_OBJECT mad_space_engine_body[1] = {
#include "stg44/carriage/mad_space_engine_body.inc"
};

static NJS_TEXNAME mad_space_trailer_names[8] = {
#include "stg44/carriage/mad_space_trailer_names.inc"
};

NJS_TEXLIST mad_space_trailer_texlist = {
#include "stg44/carriage/mad_space_trailer_texlist.inc"
};

static Sint16 mad_space_trailer_body_polygons[615] = {
#include "stg44/carriage/mad_space_trailer_body_polygons.inc"
};

static Sint32 mad_space_trailer_body_vertices[495] = {
#include "stg44/carriage/mad_space_trailer_body_vertices.inc"
};

static NJS_CNK_MODEL mad_space_trailer_body_model = {
#include "stg44/carriage/mad_space_trailer_body_model.inc"
};

NJS_CNK_OBJECT mad_space_trailer_body[1] = {
#include "stg44/carriage/mad_space_trailer_body.inc"
};

static NJS_MATERIAL mad_space_engine_collision_materials[1] = {
#include "stg44/carriage/mad_space_engine_collision_materials.inc"
};

static Sint16 mad_space_engine_collision_faces[95] = {
#include "stg44/carriage/mad_space_engine_collision_faces.inc"
};

static NJS_MESHSET mad_space_engine_collision_meshsets[1] = {
#include "stg44/carriage/mad_space_engine_collision_meshsets.inc"
};

static NJS_POINT3 mad_space_engine_collision_points[30] = {
#include "stg44/carriage/mad_space_engine_collision_points.inc"
};

static NJS_VECTOR mad_space_engine_collision_normals[30] = {
#include "stg44/carriage/mad_space_engine_collision_normals.inc"
};

static NJS_MODEL mad_space_engine_collision_model = {
#include "stg44/carriage/mad_space_engine_collision_model.inc"
};

NJS_OBJECT mad_space_engine_collision = {
#include "stg44/carriage/mad_space_engine_collision.inc"
};

static NJS_MATERIAL mad_space_trailer_collision_materials[1] = {
#include "stg44/carriage/mad_space_trailer_collision_materials.inc"
};

static Sint16 mad_space_trailer_collision_faces[21] = {
#include "stg44/carriage/mad_space_trailer_collision_faces.inc"
};

static NJS_MESHSET mad_space_trailer_collision_meshsets[1] = {
#include "stg44/carriage/mad_space_trailer_collision_meshsets.inc"
};

static NJS_POINT3 mad_space_trailer_collision_points[8] = {
#include "stg44/carriage/mad_space_trailer_collision_points.inc"
};

static NJS_VECTOR mad_space_trailer_collision_normals[8] = {
#include "stg44/carriage/mad_space_trailer_collision_normals.inc"
};

static NJS_MODEL mad_space_trailer_collision_model = {
#include "stg44/carriage/mad_space_trailer_collision_model.inc"
};

NJS_OBJECT mad_space_trailer_collision = {
#include "stg44/carriage/mad_space_trailer_collision.inc"
};

static Sint16 mad_space_engine_shadow_polygons[113] = {
#include "stg44/carriage/mad_space_engine_shadow_polygons.inc"
};

static struct mad_space_engine_shadow_vertex_list
    mad_space_engine_shadow_vertices = {
#include "stg44/carriage/mad_space_engine_shadow_vertices.inc"
};

static NJS_CNK_MODEL mad_space_engine_shadow_model = {
#include "stg44/carriage/mad_space_engine_shadow_model.inc"
};

NJS_CNK_OBJECT mad_space_engine_shadow = {
#include "stg44/carriage/mad_space_engine_shadow.inc"
};

static Sint16 mad_space_trailer_shadow_polygons[89] = {
#include "stg44/carriage/mad_space_trailer_shadow_polygons.inc"
};

static struct mad_space_trailer_shadow_vertex_list
    mad_space_trailer_shadow_vertices = {
#include "stg44/carriage/mad_space_trailer_shadow_vertices.inc"
};

static NJS_CNK_MODEL mad_space_trailer_shadow_model = {
#include "stg44/carriage/mad_space_trailer_shadow_model.inc"
};

NJS_CNK_OBJECT mad_space_trailer_shadow = {
#include "stg44/carriage/mad_space_trailer_shadow.inc"
};

static Float travel_speed = 1.2F;
static NJS_TEXNAME headlight_names[1] = {
#include "stg44/carriage/headlight_names.inc"
};
static NJS_TEXLIST headlight_textures = {
#include "stg44/carriage/headlight_textures.inc"
};
NJS_TEXLIST *mad_space_carriage_textures[4] = {
#include "stg44/carriage/mad_space_carriage_textures.inc"
};

static struct collision_shape sphere = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0, {0, 0, 0}, 35, 0, 0,
    0, {0, 0, 0},
};
static Float headlight_size = {
#include "stg44/carriage/headlight_size.inc"
};
static NJS_POINT3 headlight_right = {
#include "stg44/carriage/headlight_right.inc"
};
static NJS_POINT3 headlight_left = {
#include "stg44/carriage/headlight_left.inc"
};

#include "stg44/carriage_path_impl.h"

#pragma inline(headlight_opacity)
static int headlight_opacity(int value)
{
    if (value < 0) {
        value = 0;
    }
    if (value > OPAQUE_ALPHA) {
        value = OPAQUE_ALPHA;
    }
    return value;
}

/* @hack redundant: depth, an output that headlight_draw never reads */
static void headlight_color(NJS_VECTOR *direction, Float *depth, Uint32 *color)
{
    NJS_VECTOR view;
    Float facing;
    Uint32 alpha;

    njCalcVector(NULL, direction, &view);
    facing = view.z;
    /* @hack spelling: *depth stored in both branches, where one store after
     * the if would do */
    if (facing < 0.0F) {
        facing = 0.0F;
        *depth = facing;
    } else {
        *depth = facing;
    }
    alpha = (Uint32)headlight_opacity(facing * (Float)OPAQUE_ALPHA);
    alpha <<= 24;
    *color = alpha + WHITE_RGB;
}

static void headlight_draw(NJS_POINT3 *position)
{
    Uint32 color;
    Float depth;
    Angle turn;
    NJS_POINT3 view;
    NJS_VECTOR facing;

    njCalcPoint(NULL, position, &view);
    view.z *= DEPTH_WEIGHT;
    turn = njScalor(&view) * (ANGLE_PER_TURN / ROLL_DISTANCE);
    facing.x = 0.0F;
    facing.y = 0.0F;
    facing.z = -1.0F;
    njSetTexture(&headlight_textures);
    headlight_color(&facing, &depth, &color);
    particle_draw_quad(position, 0, QUARTER_TURN, turn, headlight_size,
                       headlight_size, color, PARTICLE_BLEND_ADD, FLARE_DEPTH,
                       0.0F);
}

static void carriage_draw(struct task *task)
{
    struct task_work *work;
    NJS_CNK_OBJECT *light;
    Uint32 steps;
    /* @hack spelling: period, a copy of LIGHT_PERIOD, where LIGHT_PERIOD
     * itself would do */
    Uint32 period;
    int frame;
    int from;
    int to;
    Float fade;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGSPD Y";
    }
    work = task->work;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (work->submode) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkModDrawObject(&mad_space_trailer_shadow);
        njSetTexture(&mad_space_trailer_texlist);
        njTranslateEx((NJS_VECTOR *)mad_space_trailer_body->pos);
        njCnkEasyDrawModel(mad_space_trailer_body->model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    } else {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njFogDisable();
        headlight_draw(&headlight_right);
        headlight_draw(&headlight_left);
        njFogEnable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njCnkModDrawObject(&mad_space_engine_shadow);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njSetTexture(&mad_space_engine_texlist);
        njCnkEasyDrawModel(mad_space_engine_body->model);
        draw_control_clear(DRAW_CONTROL_SHADOW);
        /* @hack spelling: the assignment to steps inside the remainder, where
         * a statement of its own would do */
        period = LIGHT_PERIOD;
        frame = (steps = game_state.steps) % period;
        from = (steps / period) % LIGHT_PHASES;
        to = (from + 1) % LIGHT_PHASES;
        if (from >= LIGHT_PLACES) {
            from = LIGHT_PHASES - from;
        }
        if (to >= LIGHT_PLACES) {
            to = LIGHT_PHASES - to;
        }
        light = mad_space_engine_body->child;
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, 0.0F, to * LIGHT_SPACING);
        njCnkEasyDrawModel(light->model);
        njPopMatrixEx();
        fade = (0.5F - (frame * (1.0F / LIGHT_PERIOD))) * 2.0F;
        if (fade < 0.0F) {
            fade = 0.0F;
        }
        njTranslate(NULL, 0.0F, 0.0F, from * LIGHT_SPACING);
        model_draw_tinted(light->model,
                          color_blend(COLOR_CLEAR, COLOR_NONE, fade));
    }
    njPopMatrixEx();
}
#include "stg44/carriage_step_impl.h" /* IWYU pragma: keep -- the code it compiles here */
