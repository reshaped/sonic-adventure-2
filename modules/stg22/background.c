/* the background task's draw */
#include <ninja.h>
#include <nj_direct.h> /* IWYU pragma: keep -- completes NJS_DIRECT_HEAD */
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/color.h"
#include "effect/particle.h"
#include "player/player.h"
#include "shared/vertex_colors.h"
#include "stg22/backdrop.h"
#include "stg22/background_textures.h"
#include "stg22/glow.h"
#include "stg22/stage_data.h"
#include "stg22/surfaces.h"
#include "system/angle.h"
#include "system/cache.h"
#ifdef NONMATCHING
/* the declarations of WORK_AREA and game_state, which this side's plain forms
 * below name */
#include "system/areas.h"
#include "system/game_state.h"
#endif
#include "system/task.h"
#include "system/util.h"

#ifdef NONMATCHING
/* the plain form the hack tag names */
#define SHOW_VOLUMES game_state.debug_display
#define SCENE_FRAME game_state.steps
#define FIRST_BLOCKS game_state.land_blocks[0]
#define SECOND_BLOCKS game_state.land_blocks[1]
#else
/* @hack type: gadget_draw_state, a record laid over game_state that names
 * only the four fields the draw reads, where game_state's own would do */
struct gadget_draw_state {
    Uint8 before_debug_display[3];
    Sint8 debug_display;
    Uint8 before_frame[0x70];
    Uint32 frame;
    Uint8 before_blocks[0x24];
    Sint32 first_blocks;
    Sint32 second_blocks;
};
extern struct gadget_draw_state gadget_draw_state;
#define SHOW_VOLUMES gadget_draw_state.debug_display
#define SCENE_FRAME gadget_draw_state.frame
#define FIRST_BLOCKS gadget_draw_state.first_blocks
#define SECOND_BLOCKS gadget_draw_state.second_blocks
#endif

#define STRIPS 4
#define SIGNS 22
#define SKY_BLOCKS 0x36c30
/* a model draws while the view z of its center lies between its kind's _NEAR
 * and _FAR. the scrolling land and the moving strips draw nearer than
 * SCENERY_FAR. a land model's near bound is minus its radius, and a strip's is
 * minus its length */
#define SCENERY_FAR 2000.0F
#define PULSE_NEAR (-50.0F)
#define PULSE_FAR 800.0F
#define SIGN_NEAR (-1000.0F)
#define SIGN_FAR 1500.0F
/* the scrolling land and the signs each show a texture of their cycle for
 * FRAME_STEPS steps. the land's cycle takes SURFACE_CYCLE frames. it climbs
 * through textures 0 to SURFACE_TEXTURES - 1 of gadget_surface_texlist, and
 * back down to 1 */
#define FRAME_STEPS 2
#define SURFACE_TEXTURES 5
#define SURFACE_CYCLE 9
/* a moving strip is a row of models STRIP_SPACING apart, whose length is the
 * strip's plus STRIP_OVERHANG. the row moves STRIP_SPACING along the strip in
 * STRIP_STEPS steps. it moves one way where the strip's yaw is 0, and the
 * other way otherwise */
#define STRIP_SPACING 200.0F
#define STRIP_OVERHANG 400.0F
#define STRIP_STEPS 300
/* a pulse has PULSE_LAMPS lamps. its light moves from the last to the first,
 * one lamp every LAMP_STEPS steps. a lamp brightens over its first
 * FADE_STEPS / 2 steps lit. the lamp lit before it dims over the first
 * FADE_STEPS */
#define PULSE_LAMPS 4
#define LAMP_STEPS 38
#define FADE_STEPS 10
#define LAMP_DARK 0xff000000
#define LAMP_LIT 0xffffffff
/* a sign's texture id is TID_BITS of halfword SIGN_TID of its panel's polygon
 * list. its cycle shows each of SIGN_TEXTURES textures in turn */
#define SIGN_TEXTURES 13
#define SIGN_TID 7
#define TID_BITS 0x3fff
/* the glow is a fan of GLOW_SEGMENTS triangles about its center. its rim lies
 * gadget_glow_reach from the center, squashed by GLOW_SQUASH in x. the glow's
 * strength is the center's alpha, out of MOST_ALPHA. the rim multiplies the
 * center's color by RIM_COLOR */
#define GLOW_SEGMENTS 32
#define GLOW_VERTICES ((GLOW_SEGMENTS * 3) + 1)
#define GLOW_SQUASH 0.9F
#define MOST_ALPHA 255
#define WHITE_RGB 0xffffff
#define RIM_COLOR 0x10ffffff
/* the glow takes texture 1 of gadget_glow_texlist inside this box, and texture
 * 0 elsewhere */
#define IN_SECOND_TEXTURE_BOX(point)                                           \
    ((point)->x > -3960.0F && (point)->x < -3700.0F && (point)->y > -17.0F &&  \
     (point)->y < 587.0F && (point)->z > -1485.0F && (point)->z < -1225.0F)

struct moving_strip {
    NJS_POINT3 start;
    NJS_POINT3 end;
    Angle yaw;
};
/* each draw compiles the two pulses into WORK_AREA, the second PULSE_B_OFFSET
 * bytes past the first */
#define PULSE_B_OFFSET 0x10000
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define PULSE_A_BUFFER ((NJS_DIRECT_HEAD *)WORK_AREA)
#define PULSE_B_BUFFER ((NJS_DIRECT_HEAD *)(WORK_AREA + PULSE_B_OFFSET))
#else
/* @hack type: the pulses' builds as two symbols at WORK_AREA and
 * PULSE_B_OFFSET past it, where WORK_AREA would do */
extern NJS_DIRECT_HEAD gadget_light_build_a;
extern NJS_DIRECT_HEAD gadget_light_build_b;
#define PULSE_A_BUFFER (&gadget_light_build_a)
#define PULSE_B_BUFFER (&gadget_light_build_b)
#endif

static Uint32 glow_center_color = 0xffffffff;
/* a color no draw takes. the rim takes RIM_COLOR */
static Uint32 glow_edge_color = 0x0080ffff;
static Float glow_uv_scale = 0.2F;
static struct moving_strip moving_strips[STRIPS] = {
#include "stg22/background/moving_strips.inc"
};
static NJS_POINT3 debug_origin = {0.0F, 0.0F, 0.0F};
static NJS_VECTOR debug_box_scale = {1.0F, 1.0F, 1.0F};
static NJS_CNK_OBJECT *sign_panels[SIGNS] = {
#include "stg22/background/sign_panels.inc"
};
static Sint32 sign_blocks[SIGNS] = {
#include "stg22/background/sign_blocks.inc"
};

static Float glow_uv(Float coordinate)
{
    /* @hack spelling: coordinate scaled in place and then returned, where the
     * product returned would do */
    coordinate *= glow_uv_scale;
    return coordinate;
}

void gadget_background_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;
    int i;
    int count;
    Sint32 mask;
    struct land_entry *entry;
    NJS_CNK_OBJECT *object;

    if (SHOW_VOLUMES) {
        struct gadget_kill_volume *volume = gadget_kill_volumes;
        njPushMatrixEx();
        for (i = 0; (Uint32)i < sizeof(gadget_kill_volumes) /
                                    sizeof(gadget_kill_volumes[0]);
             ++i, ++volume) {
            njUnitMatrix(NULL);
            njTranslateEx((NJS_VECTOR *)volume->object->pos);
            njRotateEx(volume->object->ang, 0);
            njScale(NULL, volume->object->scl[0], 1.0F, volume->object->scl[2]);
            debug_draw_model(&debug_origin, NULL, &debug_box_scale,
                             DEBUG_MODEL_BOX);
        }
        njPopMatrixEx();
    }
    if (!camera_current_index) {
        mask = FIRST_BLOCKS;
    } else {
        mask = SECOND_BLOCKS;
    }
    count = gadget_scrolling_land.count;
    entry = gadget_scrolling_land.entries;
    njPushMatrixEx();
    {
        NJS_TEXLIST textures;
        int frame;
        textures = gadget_surface_texlist;
        frame = (SCENE_FRAME / FRAME_STEPS) % SURFACE_CYCLE;
        if (frame >= SURFACE_TEXTURES) {
            frame = SURFACE_CYCLE - frame;
        }
        textures.textures += frame;
        textures.nbTexture = 1;
        njSetTexture(&textures);
        njFogDisable();
        while (count--) {
            if (!entry->blocks || (entry->blocks & mask)) {
                njCalcPoint(NULL, &entry->center, &position);
                if (position.z < SCENERY_FAR && position.z > -entry->radius) {
                    object = (NJS_CNK_OBJECT *)entry->object;
                    njTranslateEx((NJS_VECTOR *)object->pos);
                    njRotateEx(object->ang, 0);
                    njCnkEasyDrawModel(object->model);
                    matrix_load_parent();
                }
            }
            ++entry;
        }
        njFogEnable();
    }
    njPopMatrixEx();
    {
        Float phase = (SCENE_FRAME % STRIP_STEPS) * (1.0F / STRIP_STEPS);
        NJS_POINT3 view;
        Float length;
        Float offset;
        Float distance;
        Float direction;
        njPushMatrixEx();
        njSetTexture(&gadget_strip_texlist);
        for (i = 0;
             (Uint32)i < sizeof(moving_strips) / sizeof(moving_strips[0]);
             ++i) {
            position.x =
                (moving_strips[i].start.x + moving_strips[i].end.x) * 0.5F;
            position.y =
                (moving_strips[i].start.y + moving_strips[i].end.y) * 0.5F;
            position.z =
                (moving_strips[i].start.z + moving_strips[i].end.z) * 0.5F;
            if (moving_strips[i].yaw) {
                direction = 1.0F;
            } else {
                direction = -1.0F;
            }
            length =
                njDistanceP2P(&moving_strips[i].end, &moving_strips[i].start) +
                STRIP_OVERHANG;
            distance = 0.0F;
            offset = (((STRIP_SPACING * 0.5F) - (phase * STRIP_SPACING)) *
                      direction) -
                     (length * 0.5F);
            njCalcPoint(NULL, &position, &view);
            if (view.z > -length && view.z < SCENERY_FAR) {
                njTranslateEx(&position);
                njRotateY(NULL, moving_strips[i].yaw);
                njPushMatrixEx();
                while (distance < length) {

#ifdef DREAMCAST
#ifdef NONMATCHING
#else
                    /* @hack redundant: three stores to position.x, and tests
                     * of four functions' addresses with empty bodies, in a
                     * block no path reaches */
                    if (0) {
                        position.x = -1.0F;
                        position.x = 0.0F;
                        position.x = 400.0F;
                        if (njPushMatrixEx) {
                        }
                        if (njTranslateEx) {
                        }
                        if (njPopMatrixEx) {
                        }
                        if (matrix_load_parent) {
                        }
                    }
#endif
#endif

                    njTranslate(NULL, offset, 0.0F, 0.0F);
                    njCnkEasyDrawModel(gadget_strip_object->model);
                    matrix_load_parent();
                    offset += STRIP_SPACING;
                    distance += STRIP_SPACING;
                }
                njPopMatrixEx();

#ifdef DREAMCAST
#ifdef NONMATCHING
#else
                /* @hack redundant: tests of njPushMatrixEx's and
                 * njDistanceP2P's addresses, twice each with empty bodies, in a
                 * block no path reaches */
                if (0) {
                    if (njPushMatrixEx) {
                    }
                    if (njPushMatrixEx) {
                    }
                    if (njDistanceP2P) {
                    }
                    if (njDistanceP2P) {
                    }
                }
#endif
#endif

                matrix_load_parent();
            }
        }
        njPopMatrixEx();
    }
    {
        NJS_DIRECT_HEAD *compiled_a = PULSE_A_BUFFER;
        NJS_DIRECT_HEAD *compiled_b = PULSE_B_BUFFER;
        int frame = SCENE_FRAME % (PULSE_LAMPS * LAMP_STEPS);
        int active = PULSE_LAMPS - 1 - (frame / LAMP_STEPS);
        NJS_CNK_OBJECT *lamp_a;
        NJS_CNK_OBJECT *lamp_b;
        Float intensity;
        Uint32 color;
        frame = frame % LAMP_STEPS;
        lamp_a = &gadget_pulse_a;
        lamp_b = &gadget_pulse_b;
        for (i = 0; i < PULSE_LAMPS; ++i) {
            if (i == active) {
                if (frame < FADE_STEPS) {
                    intensity = frame * (1.0F / FADE_STEPS);
                    intensity *= 2.0F;
                    if (intensity > 1.0F) {
                        intensity = 1.0F;
                    }
                } else {
                    intensity = 1.0F;
                }
            } else if (i == (active + 1) % PULSE_LAMPS) {
                if (frame < FADE_STEPS) {
                    intensity = (FADE_STEPS - frame) * (1.0F / FADE_STEPS);
                } else {
                    intensity = 0.0F;
                }
            } else {
                intensity = 0.0F;
            }
            color = color_blend(LAMP_DARK, LAMP_LIT, intensity);
            vlist_set_color(lamp_a->model->vlist, color);
            vlist_set_color(lamp_b->model->vlist, color);
            lamp_a = lamp_a->sibling;
            lamp_b = lamp_b->sibling;
        }
        njSetTexture(&gadget_pulse_texlist);
        njCnkDirectObjectCompile(&gadget_pulse_a, compiled_a);
        njCnkDirectObjectCompile(&gadget_pulse_b, compiled_b);
        count = crazy_gadget_land.count;
        entry = crazy_gadget_land.entries;
        /* @bug logic: under camera 1 the pulses take camera 0's blocks. both
         * arms take FIRST_BLOCKS, read at 0x8c501ecc. the scrolling land
         * above takes SECOND_BLOCKS under camera 1, at 0x8c501918 */
        if (!camera_current_index) {
            mask = FIRST_BLOCKS;
        } else {
            mask = FIRST_BLOCKS;
        }
        njPushMatrixEx();
        while (count--) {
            if ((entry->flags & LAND_DRAWN) &&
                (!entry->blocks || (entry->blocks & mask))) {
                object = (NJS_CNK_OBJECT *)entry->object;
#define DRAW_PULSE(compiled)                                                   \
    njCalcPoint(NULL, &entry->center, &position);                              \
    if (position.z > PULSE_NEAR && position.z < PULSE_FAR) {                   \
        njTranslateEx((NJS_VECTOR *)object->pos);                              \
        njRotateEx(object->ang, 0);                                            \
        njDirectDrawObject(compiled);                                          \
        matrix_load_parent();                                                  \
    }
                if (object->model == &gadget_pulse_marker_a) {
                    DRAW_PULSE(compiled_a)
                } else if (object->model == &gadget_pulse_marker_b) {
                    DRAW_PULSE(compiled_b)
                }
#undef DRAW_PULSE
            }
            ++entry;
        }
        njPopMatrixEx();
    }
    {
        int frame = (SCENE_FRAME / FRAME_STEPS) % SIGN_TEXTURES;
        NJS_CNK_OBJECT **panels;
        Sint32 *blocks;
        njFogDisable();
        njSetTexture(&gadget_background_sign_texlist);
        njPushMatrixEx();
        /* @hack spelling: panels and blocks stepped by assignments, where ++
         * would do */
        for (i = 0, panels = sign_panels, blocks = sign_blocks;
             (Uint32)i < sizeof(sign_panels) / sizeof(sign_panels[0]);
             ++i, panels = panels + 1, blocks = blocks + 1) {
            if (FIRST_BLOCKS & *blocks) {
                NJS_CNK_OBJECT *panel = *panels;
                Sint16 *texture_id = &panel->model->plist[SIGN_TID];
                int texture = *texture_id;
                texture = texture & ~TID_BITS;
                texture = texture | frame;
                *texture_id = texture;
                njTranslateEx((NJS_VECTOR *)panel->pos);
                njRotateEx(panel->ang, 0);
                njCalcPoint(NULL, &panel->model->center, &position);
                if (position.z > SIGN_NEAR && position.z < SIGN_FAR) {
                    njCnkEasyDrawModel(panel->model);
                }
                matrix_load_parent();
            }
        }
        njPopMatrixEx();

#ifdef DREAMCAST
#ifdef NONMATCHING
#else
        /* @hack redundant: tests of blocks and panels with empty bodies, in a
         * block no path reaches */
        if (0) {
            if (blocks) {
            }
            if (blocks) {
            }
            if (panels) {
            }
            if (panels) {
            }
            if (panels) {
            }
        }
#endif
#endif

        njFogEnable();
    }
    if (GADGET_GLOW(task) && player_work[0]) {
        /* @hack spelling: point, a pointer to position, where position by
         * name would do */
        NJS_POINT3 *point;
        struct surface_side *surface;
        Float center_u;
        Float center_v;
        int alpha;
        int texture;
        Uint32 center_color;
        Uint32 edge_color;
        NJS_TEXTURE_VTX vertices[GLOW_VERTICES];
        NJS_TEXTURE_VTX *vertex;
        Angle angle;
        Angle pitch;
        Angle roll;
        Float sine;
        Float cosine;
        int index;
        point = &position;
        *point = GADGET_GLOW(task)->pos;
        surface = &GADGET_GLOW(task)->sides[0];
        switch (surface->found) {
        case SURFACE_NEG_X:
        case SURFACE_POS_X:
            point->x = surface->pos;
            center_u = point->y;
            center_v = point->z;
            break;
        case SURFACE_NEG_Y:
        case SURFACE_POS_Y:
            point->y = surface->pos;
            center_u = point->x;
            center_v = point->z;
            break;
        case SURFACE_NEG_Z:
        case SURFACE_POS_Z:
            point->z = surface->pos;
            center_u = point->x;
            center_v = point->y;
            break;
#ifdef NONMATCHING
        default:
            /* this side's default jumps past the reads of center_u and
             * center_v. every path that reads them then sets them. gravity.c's
             * step keeps a SURFACE_ side in found, so no path comes here */
            goto draw_sky;
#endif
        }
        alpha = (int)(work->scl.x * MOST_ALPHA);
        if (alpha > MOST_ALPHA) {
            alpha = MOST_ALPHA;
        }
        if (alpha < 0) {
            alpha = 0;
        }
        center_color = ((Uint32)alpha << ALPHA_SHIFT) | WHITE_RGB;
        /* @hack redundant: a store of RIM_COLOR to edge_color in each arm,
         * where one before the test would do */
        if (IN_SECOND_TEXTURE_BOX(point)) {
            edge_color = RIM_COLOR;
            texture = 1;
        } else {
            edge_color = RIM_COLOR;
            texture = 0;
        }
        pitch = surface->ang_x;
        roll = surface->ang_z;
        angle = 0;
        for (i = 0; i < GLOW_SEGMENTS; ++i) {
            index = i * 3;
            vertex = &vertices[index];
            vertex->x = 0.0F;
            vertex->y = 0.0F;
            vertex->z = 0.0F;
            vertex->col = color_multiply(glow_center_color, center_color);

#ifdef DREAMCAST
#ifdef NONMATCHING
#else
            /* @hack redundant: tests of center_v, angle and vertex with empty
             * bodies, in a block no path reaches */
            if (0) {
                if (center_v) {
                }
                if (angle) {
                }
                if (vertex) {
                }
            }
#endif
#endif

            vertices[index].u = glow_uv(center_u);
            vertices[index].v = glow_uv(center_v);
#define RIM_VERTEX()                                                           \
    sine = njSin(angle);                                                       \
    cosine = njCos(angle);                                                     \
    sine *= GLOW_SQUASH;                                                       \
    ++index;                                                                   \
    vertices[index].x = gadget_glow_reach * sine;                              \
    vertices[index].y = 0.0F;                                                  \
    vertices[index].z = gadget_glow_reach * cosine;                            \
    vertices[index].col = color_multiply(edge_color, center_color);            \
    vertices[index].u = glow_uv(center_u + vertices[index].x);                 \
    vertices[index].v = glow_uv(center_v + vertices[index].z)
            RIM_VERTEX();
            angle = angle + (FULL_TURN / GLOW_SEGMENTS);

#ifdef DREAMCAST
#ifdef NONMATCHING
#else
            /* @hack redundant: a test of pitch and three of njSin's address,
             * each with an empty body, in a block no path reaches */
            if (0) {
                if (pitch) {
                }
                if (njSin) {
                }
                if (njSin) {
                }
                if (njSin) {
                }
            }
#endif
#endif

            RIM_VERTEX();
#undef RIM_VERTEX
        }
        vertex = &vertices[GLOW_VERTICES - 1];
        vertex->x = 0.0F;
        vertex->y = 0.0F;
        vertex->z = 0.0F;
        vertex->col = color_multiply(glow_center_color, center_color);
        vertices[GLOW_VERTICES - 1].u = glow_uv(center_u);
        vertices[GLOW_VERTICES - 1].v = glow_uv(center_v);
        njSetTexture(&gadget_glow_texlist);
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        draw_control_set(DRAW_CONTROL_SHADOW);
        njPushMatrixEx();
        njTranslate(NULL, point->x + world_gravity.x,
                    point->y + world_gravity.y, point->z + world_gravity.z);
        njRotateZ(NULL, roll);
        njRotateX(NULL, pitch);
        njTextureClampMode(NJD_TEXTURECLAMP_NOCLAMP);
        njTextureFlipMode(NJD_TEXTUREFLIP_FLIP_UV);
        njSetTextureNum(texture);
        njDrawTexture3DEx(vertices, GLOW_VERTICES, TRUE);
        njTextureFlipMode(NJD_TEXTUREFLIP_NOFLIP);
        njTextureClampMode(NJD_TEXTURECLAMP_CLAMP_UV);
        njPopMatrixEx();
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    }
#ifdef NONMATCHING
    /* the default above jumps here */
draw_sky:
#endif
    if (FIRST_BLOCKS & SKY_BLOCKS) {
        njFogDisable();
        njSetTexture(&gadget_sky_texlist);
        njPushMatrixEx();
        njTranslateEx(&camera_views[camera_current_index]->pos);
        njCnkEasyDrawObject(&gadget_sky_object);
        njPopMatrixEx();
        njFogEnable();
    }
}

/* visibility.c's code starts at the next 32-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
