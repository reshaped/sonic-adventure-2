/* the chase light, RBLK in Radical Highway's object list: a lamp of three
 * lights. they come on one at a time, in turn */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "stg14/chase_light.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#include "stg14/chase_light/text.inc"

/* chase_light_object's model is the lamp's body. its child and the child's
 * siblings are the lights. LIT is the index of the lit one */
#define LIGHTS 3
#define LIT(work) ((work)->submode)
/* a step that finds the work's tick past WAIT_TICKS lights the next light. it
 * sets tick back to 0, so each light stays lit WAIT_TICKS + 2 steps */
#define WAIT_TICKS 30
/* nothing draws where a ball of the body's radius plus DRAW_MARGIN about pos
 * lies wholly behind the camera. the body draws only while the ball lies less
 * than LAMP_DEPTH ahead. the lit light's model draws only less than
 * LIGHT_DEPTH ahead */
#define DRAW_MARGIN 3.0F
#define LAMP_DEPTH 400.0F
#define LIGHT_DEPTH 200.0F
/* the glow turns a full turn for each GLOW_TURN_DISTANCE of the lamp's
 * distance from the camera. that distance counts the lamp's depth at
 * DEPTH_PART of itself. so the glow turns as the lamp crosses the view, and
 * hardly as it nears */
#define GLOW_TURN_DISTANCE 120.0F
#define DEPTH_PART 0.001F
#define GLOW_TEXTURE 0
/* the depth particle_draw_quad takes: the glow draws GLOW_DEPTH nearer the
 * camera than it lies */
#define GLOW_DEPTH 5.0F

/* the lamp's collision box, centered BOX_RISE above pos */
#define BOX_RISE 9.0F
#define BOX_HALF_X 10.0F
#define BOX_HALF_Y 9.0F
#define BOX_HALF_Z 50.0F
#define CHASE_LIGHT_SHAPES 1

/* The three lamps along the light fixture and the separate glow texture. */

static NJS_TEXNAME chase_light_textures[5] = {
#include "stg14/chase_light/chase_light_textures.inc"
};

static NJS_TEXLIST chase_light_texlist = {
#include "stg14/chase_light/chase_light_texlist.inc"
};

static Sint16 chase_light_lamp_3_polygons[97] = {
#include "stg14/chase_light/chase_light_lamp_3_polygons.inc"
};

static Sint32 chase_light_lamp_3_vertices[39] = {
#include "stg14/chase_light/chase_light_lamp_3_vertices.inc"
};

static NJS_CNK_MODEL chase_light_lamp_3_mesh = {
#include "stg14/chase_light/chase_light_lamp_3_mesh.inc"
};

static NJS_CNK_OBJECT chase_light_lamp_3 = {
#include "stg14/chase_light/chase_light_lamp_3.inc"
};

static Sint16 chase_light_lamp_2_polygons[97] = {
#include "stg14/chase_light/chase_light_lamp_2_polygons.inc"
};

static Sint32 chase_light_lamp_2_vertices[39] = {
#include "stg14/chase_light/chase_light_lamp_2_vertices.inc"
};

static NJS_CNK_MODEL chase_light_lamp_2_mesh = {
#include "stg14/chase_light/chase_light_lamp_2_mesh.inc"
};

static NJS_CNK_OBJECT chase_light_lamp_2 = {
#include "stg14/chase_light/chase_light_lamp_2.inc"
};

static Sint16 chase_light_lamp_1_polygons[97] = {
#include "stg14/chase_light/chase_light_lamp_1_polygons.inc"
};

static Sint32 chase_light_lamp_1_vertices[39] = {
#include "stg14/chase_light/chase_light_lamp_1_vertices.inc"
};

static NJS_CNK_MODEL chase_light_lamp_1_mesh = {
#include "stg14/chase_light/chase_light_lamp_1_mesh.inc"
};

static NJS_CNK_OBJECT chase_light_lamp_1 = {
#include "stg14/chase_light/chase_light_lamp_1.inc"
};

static Sint16 chase_light_polygons[951] = {
#include "stg14/chase_light/chase_light_polygons.inc"
};

static Sint32 chase_light_vertices[923] = {
#include "stg14/chase_light/chase_light_vertices.inc"
};

static NJS_CNK_MODEL chase_light_mesh = {
#include "stg14/chase_light/chase_light_mesh.inc"
};

static NJS_CNK_OBJECT chase_light_object = {
#include "stg14/chase_light/chase_light_object.inc"
};

static NJS_TEXNAME chase_light_glow_textures[1] = {
#include "stg14/chase_light/chase_light_glow_textures.inc"
};

static NJS_TEXLIST chase_light_glow_texlist = {
#include "stg14/chase_light/chase_light_glow_texlist.inc"
};

NJS_TEXLIST *chase_light_texlists[3] = {
#include "stg14/chase_light/chase_light_texlists.inc"
};
static struct collision_shape chase_light_shape = {
#include "stg14/chase_light/chase_light_shape.inc"
};
/* the lamp's build, or null while chase_light_build has made none */
static NJS_DIRECT_HEAD *chase_light_built = NULL;
static Float glow_size = 6.0F; /* half the glow's width and height */
static Uint32 glow_color = 0x80ffffff;
/* where the glow draws, in the lit light's frame */
static NJS_POINT3 glow_pos = {
#include "stg14/chase_light/glow_pos.inc"
};

static void chase_light_destroy(struct task *task);
static void chase_light_draw(struct task *task);
static void chase_light_step(struct task *task);

Uint8 *chase_light_build(Uint8 *buf)
{
    njSetTexture(&chase_light_texlist);
    chase_light_built = object_build(&buf, &chase_light_object);
    return buf;
}

void chase_light_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its string, which SHC
     * still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->step = chase_light_step;
    task->draw = chase_light_draw;
    task->destructor = chase_light_destroy;
    collision_init(task, &chase_light_shape, CHASE_LIGHT_SHAPES,
                   COLLISION_OBJECT);
}

static void chase_light_destroy(struct task *task)
{
}

static void chase_light_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = &chase_light_object;
    Float depth;
    Angle turn;
    NJS_POINT3 view;
    int i;

    if (!sphere_in_front(&work->pos, object->model->r + DRAW_MARGIN, &depth)) {
        return;
    }
    njCalcPoint(NULL, &work->pos, &view);
    view.z *= DEPTH_PART;
    turn = njScalor(&view) * (FULL_TURN / GLOW_TURN_DISTANCE);
    njSetTexture(&chase_light_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    if (depth < LAMP_DEPTH) {
        if (chase_light_built != NULL) {
            njDirectDrawObject(chase_light_built);
        } else {
            njCnkEasyDrawModel(object->model);
        }
    }
    object = object->child;
    njPushMatrixEx();
    njSetTexture(&chase_light_texlist);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    for (i = 0; i < LIGHTS && object != NULL; i++) {
        if (LIT(work) == i) {
            njTranslateEx((NJS_VECTOR *)object->pos);
            njSetTexture(&chase_light_glow_texlist);
            particle_draw_quad(&glow_pos, GLOW_TEXTURE, QUARTER_TURN, turn,
                               glow_size, glow_size, glow_color,
                               PARTICLE_BLEND_ADD, GLOW_DEPTH, 0.0F);
            if (depth < LIGHT_DEPTH) {
                njSetTexture(&chase_light_texlist);
                njCnkEasyDrawModel(object->model);
            }
            break;
        }
        object = object->sibling;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
    njPopMatrix(2);
}

static void chase_light_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        if (work->tick++ > WAIT_TICKS) {
            work->tick = 0;
            LIT(work) = (LIT(work) + 1) % LIGHTS;
        }
    }
}
