#include <ninja.h>

#include "draw/texture_anim.h"
#include "stg23/background.h"
#include "stg23/sand_surfaces.h"
#include "system/angle.h"
#include "system/game_state.h"

/* BgClip sets camera 0's mask to one block alone, so == tests it as
 * hidden_base.c's & does */
#define IN_BLOCK(block) (game_state.land_block_mask[0] == (block))
/* a surface's texture is set by the 16-bit word QUICKSAND_TID of its polygon
 * list. the draw keeps that word's filter mode, its NJD_FFM_MASK bits, and
 * replaces the rest with a texture index. the index is the frame,
 * game_state.steps >> FRAME_SHIFT, modulo TEXTURES. TEXTURES counts
 * base_quicksand_texlist's textures */
#define FRAME_SHIFT 1
#define QUICKSAND_TID 7
#define TEXTURES 32
#define FRAME ((game_state.steps >> FRAME_SHIFT) & (TEXTURES - 1))
#define TEXTURE(object) ((object)->model->plist[QUICKSAND_TID])
/* the start and the low surface lie SAND_LIFT above the ground */
#define SAND_LIFT 0.1F
/* move object's UVs by anim to frame's, and draw it */
#define DRAW_SURFACE(object, anim, frame)                                      \
    texture_anim_apply((object)->model, &(anim), (frame));                     \
    njCnkEasyDrawModel((object)->model)

void base_quicksand_draw(void)
{
    NJS_CNK_OBJECT *object;
    /* @hack type: texture an int, where the Sint16 of the word it holds would
     * do */
    int texture;

    njSetTexture(&base_quicksand_texlist);
    if (IN_BLOCK(BASE_BLOCK_10)) {
        /* @hack spelling: object set inside TEXTURE's argument, where a
         * statement of its own would do */
        texture = TEXTURE(object = &base_sand_upper);
        texture = (texture & NJD_FFM_MASK) | FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -3830.0F, 900.0F, -5310.0F);
        DRAW_SURFACE(object, base_sand_upper_anim,
                     -game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_10) || IN_BLOCK(BASE_BLOCK_11)) {
        object = &base_sand_end;
        texture = TEXTURE(object);
        texture = (texture & NJD_FFM_MASK) | FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -3580.0F, 900.0F, -5785.0F);
        DRAW_SURFACE(object, base_sand_end_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    object = &base_sand_fall;
    if (IN_BLOCK(BASE_BLOCK_4) || IN_BLOCK(BASE_BLOCK_5) ||
        IN_BLOCK(BASE_BLOCK_6)) {
        /* @hack spelling: the mask taken in the read and the frame or'd in
         * apart, where texture = (texture & NJD_FFM_MASK) | FRAME would do */
        texture = TEXTURE(object) & NJD_FFM_MASK;
        texture |= FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -4095.0F, 220.0F, -3085.0F);
        DRAW_SURFACE(object, base_sand_fall_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_7) || IN_BLOCK(BASE_BLOCK_8) ||
        IN_BLOCK(BASE_BLOCK_9)) {
        /* @hack spelling: the mask and the frame in statements of their own,
         * where texture = (texture & NJD_FFM_MASK) | FRAME would do */
        texture = TEXTURE(object);
        texture &= NJD_FFM_MASK;
        texture |= FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -4085.0F, 580.0F, -4745.0F);
        njRotateY(NULL, -QUARTER_TURN);
        DRAW_SURFACE(object, base_sand_fall_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_10) || IN_BLOCK(BASE_BLOCK_11) ||
        IN_BLOCK(BASE_BLOCK_12)) {
        texture = TEXTURE(object);
        texture = (texture & NJD_FFM_MASK) | FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -3025.0F, 940.0F, -5785.0F);
        njRotateY(NULL, HALF_TURN);
        DRAW_SURFACE(object, base_sand_fall_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_13)) {
        /* @hack spelling: the mask and the frame in statements of their own,
         * where texture = (texture & NJD_FFM_MASK) | FRAME would do */
        texture = TEXTURE(object);
        texture &= NJD_FFM_MASK;
        texture |= FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -3625.0F, 580.0F, -4640.0F);
        njRotateY(NULL, -QUARTER_TURN);
        DRAW_SURFACE(object, base_sand_fall_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
        njPushMatrixEx();
        njTranslate(NULL, -3625.0F, 580.0F, -4940.0F);
        njRotateY(NULL, -QUARTER_TURN);
        DRAW_SURFACE(object, base_sand_fall_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_1)) {
        /* @hack spelling: object set inside TEXTURE's argument, where a
         * statement of its own would do */
        texture = TEXTURE(object = &base_sand_start);
        texture = (texture & NJD_FFM_MASK) | FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, SAND_LIFT, -1000.0F);
        DRAW_SURFACE(object, base_sand_start_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
    if (IN_BLOCK(BASE_BLOCK_2) || IN_BLOCK(BASE_BLOCK_3) ||
        IN_BLOCK(BASE_BLOCK_4) || IN_BLOCK(BASE_BLOCK_5)) {
        /* @hack spelling: object set inside TEXTURE's argument, where a
         * statement of its own would do */
        texture = TEXTURE(object = &base_sand_low);
        texture = (texture & NJD_FFM_MASK) | FRAME;
        TEXTURE(object) = texture;
        njPushMatrixEx();
        njTranslate(NULL, -2000.0F, SAND_LIFT, -1500.0F);
        DRAW_SURFACE(object, base_sand_low_anim,
                     game_state.steps >> FRAME_SHIFT);
        njPopMatrixEx();
    }
}
