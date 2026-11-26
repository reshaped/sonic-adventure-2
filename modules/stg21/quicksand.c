/* Sand Ocean's seven pieces of quicksand. every piece shows the same one of
 * ocean_quicksand_texlist's textures. the pieces move on to the next every
 * two steps, and each piece's own texture_anim moves its UVs */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg21/background_art.h"
#include "stg21/quicksand.h"
#include "system/game_state.h"

/* a piece's texture is set by the 16-bit word QUICKSAND_TID of its polygon
 * list. the draw keeps that word's filter mode, its NJD_FFM_MASK bits, and
 * replaces the rest with a texture index. the index is the frame,
 * game_state.steps >> QUICKSAND_FRAME_SHIFT, modulo QUICKSAND_TEXTURES.
 * QUICKSAND_TEXTURES counts ocean_quicksand_texlist's textures */
#define QUICKSAND_FRAME_SHIFT 1
#define QUICKSAND_TID 7
#define QUICKSAND_TEXTURES 32
#define QUICKSAND_LIFT 0.1F
/* the places of the pieces drawn away from the origin, in x and z. a piece's
 * name gives its z */
#define SAND_2200_X 450.0F
#define SAND_2200_Z (-2200.0F)
#define SAND_3970_X 2585.0F
#define SAND_3970_Z (-3970.0F)
#define SAND_6830_X 2895.0F
#define SAND_6830_Z (-6830.0F)
#define SAND_4250_X 3450.0F
#define SAND_4250_Z (-4250.0F)
#define SAND_3500_X 1000.0F
#define SAND_3500_Z (-3500.0F)

/* set object's texture to the frame's, through the caller's texture and
 * word */
#define SET_SAND_TEXTURE(object)                                               \
    texture = &(object)->model->plist[QUICKSAND_TID];                          \
    word = (*texture & NJD_FFM_MASK) |                                         \
           ((game_state.steps >> QUICKSAND_FRAME_SHIFT) &                      \
            (QUICKSAND_TEXTURES - 1));                                         \
    *texture = word
/* move object's UVs by anim to the frame's, and draw it */
#define DRAW_SAND(object, anim)                                                \
    texture_anim_apply((object)->model, &(anim),                               \
                       game_state.steps >> QUICKSAND_FRAME_SHIFT);             \
    njCnkEasyDrawModel((object)->model)

void ocean_quicksand_draw(void)
{
    Sint16 *texture;
    /* @hack spelling: word, a copy of the new texture word on its way to
     * *texture, where the store alone would do */
    Sint16 word;

    njSetTexture(&ocean_quicksand_texlist);
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_1 ||
        game_state.land_block_mask[0] == OCEAN_BLOCK_2) {
        SET_SAND_TEXTURE(ocean_sand_start);
        njPushMatrixEx();
        njTranslate(NULL, 0.0F, QUICKSAND_LIFT, 0.0F);
        DRAW_SAND(ocean_sand_start, ocean_sand_start_anim);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_3) {
        SET_SAND_TEXTURE(ocean_sand_2200);
        njPushMatrixEx();
        njTranslate(NULL, SAND_2200_X, QUICKSAND_LIFT, SAND_2200_Z);
        DRAW_SAND(ocean_sand_2200, ocean_sand_2200_anim);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_5) {
        SET_SAND_TEXTURE(ocean_sand_3970);
        njPushMatrixEx();
        njTranslate(NULL, SAND_3970_X, QUICKSAND_LIFT, SAND_3970_Z);
        DRAW_SAND(ocean_sand_3970, ocean_sand_3970_anim);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_8) {
        SET_SAND_TEXTURE(ocean_sand_6830);
        njPushMatrixEx();
        njTranslate(NULL, SAND_6830_X, QUICKSAND_LIFT, SAND_6830_Z);
        DRAW_SAND(ocean_sand_6830, ocean_sand_6830_anim);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_5 ||
        game_state.land_block_mask[0] == OCEAN_BLOCK_6 ||
        game_state.land_block_mask[0] == OCEAN_BLOCK_7) {
        SET_SAND_TEXTURE(ocean_sand_4250);
        njPushMatrixEx();
        njTranslate(NULL, SAND_4250_X, QUICKSAND_LIFT, SAND_4250_Z);
        DRAW_SAND(ocean_sand_4250, ocean_sand_4250_anim);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_3 ||
        game_state.land_block_mask[0] == OCEAN_BLOCK_4) {
        SET_SAND_TEXTURE(ocean_sand_3500);
        njPushMatrixEx();
        njTranslate(NULL, SAND_3500_X, QUICKSAND_LIFT, SAND_3500_Z);
        DRAW_SAND(ocean_sand_3500, ocean_sand_3500_anim);
        njPopMatrixEx();
    }
    if (game_state.land_block_mask[0] == OCEAN_BLOCK_6 ||
        game_state.land_block_mask[0] == OCEAN_BLOCK_7 ||
        game_state.land_block_mask[0] == OCEAN_BLOCK_8) {
        SET_SAND_TEXTURE(ocean_sand_end);
        njPushMatrixEx();
        DRAW_SAND(ocean_sand_end, ocean_sand_end_anim);
        njPopMatrixEx();
    }
    /* @hack redundant: reads of game_state.steps and of camera 0's mask,
     * which nothing uses */
    (void)game_state.steps;
    (void)game_state.land_block_mask[0];
}
