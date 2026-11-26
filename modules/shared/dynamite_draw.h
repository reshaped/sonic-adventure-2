/* dynamite_draw, the charge's draw, for each source that compiles a charge.
 * the includer defines dynamite_built, dynamite_builds, flare_pos and
 * flare_half_side. under HIDDEN_BASE_CHARGE and SAND_OCEAN_CHARGE, the flare
 * draws only at a depth of flare_distance or less */
#ifndef SHARED_DYNAMITE_DRAW_H
#define SHARED_DYNAMITE_DRAW_H

#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "shared/common_object.h"
#include "shared/dynamite.h"
#include "system/util.h"

/* a blown charge's body draws red, its green and blue falling by FADE_STEP a
 * step from 1. its alpha stays FADE_ALPHA above them */
#define FADE_STEP (1.0F / DYNAMITE_FADE_STEPS)
#define FADE_ALPHA 0.5F
/* the bits of ang.x that turn the charge about x */
#define TURN_X 0xff00
/* dynamite_blink_model blinks by blink_level(BLINK_PERIOD, BLINK_REST,
 * BLINK_FADE), from BLINK_DARK at 0 to BLINK_LIGHT at 1 */
#define BLINK_PERIOD 18
#define BLINK_REST 6
#define BLINK_FADE 5
#define BLINK_DARK 0xff202020
#define BLINK_LIGHT 0xffffffff
/* the flare is a square laid on the screen. it blinks by blink_level with the
 * FLARE_ numbers, from FLARE_RED to FLARE_LIGHT */
#define FLARE_PERIOD 22
#define FLARE_REST 8
#define FLARE_FADE 5
#define FLARE_RED 0xffff0000
#define FLARE_LIGHT 0xffffffff

static void dynamite_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: object, a local for &dynamite_object set after the
     * first call, where dynamite_object.model read in place would do */
    NJS_CNK_OBJECT *object;
    /* @hack type: color an int, where the Uint32 that color_blend returns
     * would do */
    int color;
    Float fade;
    /* @hack spelling: pos, a copy of flare_pos, where flare_pos read in
     * place would do */
    NJS_POINT3 pos;
    NJS_POINT3 screen;
    /* @hack spelling: half_side, a copy of flare_half_side, where
     * flare_half_side read in place would do */
    Float half_side;
    NJS_QUAD_TEXTURE quad;

#ifndef HIDDEN_BASE_CHARGE
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ID(ANG X)";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
#endif
    njSetTexture(&dynamite_texlist);
    object = &dynamite_object;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, (work->ang.x & TURN_X));
    if (work->tick) {
        /* a blown charge draws in the constant material, translucent and
         * unlit, whatever its strips' own flags say */
        draw_control_save();
        draw_attr_save();
        fade = 1.0F - (work->tick * FADE_STEP);
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IL);
        constant_material_set(fade + FADE_ALPHA, 1.0F, fade, fade);
        njCnkSimpleDrawModel(object->model);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    } else if (dynamite_built) {
        njDirectDrawObject(dynamite_builds[0].built);
    } else {
        njCnkEasyDrawModel(object->model);
    }
    njFogDisable();
    njSetTexture(&dynamite_blink_texlist);
    color = color_blend(BLINK_DARK, BLINK_LIGHT,
                        blink_level(BLINK_PERIOD, BLINK_REST, BLINK_FADE));
    model_draw_tinted(&dynamite_blink_model, color);
    color = color_blend(FLARE_RED, FLARE_LIGHT,
                        blink_level(FLARE_PERIOD, FLARE_REST, FLARE_FADE));
    pos = flare_pos;
    half_side = flare_half_side;
#if defined(HIDDEN_BASE_CHARGE) || defined(SAND_OCEAN_CHARGE)
#define PROJECT_FLARE()                                                        \
    (screen_project(&pos, &screen) && 1.0F / screen.z <= flare_distance)
#else
#define PROJECT_FLARE() screen_project(&pos, &screen)
#endif
    if (PROJECT_FLARE()) {
        quad.x1 = screen.x - half_side;
        quad.y1 = screen.y - half_side;
        quad.x2 = screen.x + half_side;
        quad.y2 = screen.y + half_side;
        quad.u1 = 0.0F;
        quad.v1 = 0.0F;
        quad.u2 = 1.0F;
        quad.v2 = 1.0F;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&dynamite_flare_texlist);
        njQuadTextureStart(TRUE);
        njSetQuadTexture(0, color);
        njDrawQuadTexture(&quad, screen.z);
        njQuadTextureEnd();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njPopMatrixEx();
    njFogEnable();
}

#endif
