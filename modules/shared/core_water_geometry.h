/* the water's draws on Rouge's and Knuckles's routes through Cannon's Core,
 * and MOTION_FRAME for the routes' rotors. stg37/water_scenery.c includes
 * them, and so does stg38/water_scenery.c, which defines CORE_KNUCKLES_WATER
 * first. a draw given blocks draws only where camera 0's land_block_mask has
 * a bit of blocks.
 *
 * a sheet draw shows one cell of the model's texture, and moves on a cell
 * every rate steps. it draws a copy of the model's strips in WORK_AREA. each
 * texture coordinate of the copy is snapped by core_snap_uv and moved into
 * the cell. the model's plist is set back after the draw. a texture draw sets
 * each model's texture id to texture, which the model keeps */
#include <ninja.h>
#include <sg_xpt.h>

#include "system/areas.h"
#include "system/game_state.h"

/* the plist words ahead of the first strip's length, which a copy keeps */
#define STRIP_HEADER_WORDS 11
/* the texture coordinates of the cell shown at game_state.steps, for a cell
 * every rate steps */
#define SHEET_COLUMNS 4
#define SHEET_CELLS 16
#define CELL_SIZE 64
#define CELL_U(rate) ((game_state.steps / (rate) % SHEET_COLUMNS) * CELL_SIZE)
#define CELL_V(rate)                                                           \
    ((game_state.steps / (rate) % SHEET_CELLS / SHEET_COLUMNS) * CELL_SIZE)
/* the halfword of a water model's polygon list that holds its texture's
 * index */
#define WATER_TID 7
/* @hack spelling: the bits a texture draw keeps as ~0x3fff, where
 * NJD_FFM_MASK, the filter mode, would do in the 16-bit word */
#define TEXTURE_FLAGS (~0x3fff)
/* @hack spelling: a macro of bare statements, where a do-while around them
 * would do */
#define PLACE_CHILD(child)                                                     \
    njTranslate(NULL, (child)->pos[0], (child)->pos[1], (child)->pos[2]);      \
    njRotateXYZ(NULL, (child)->ang[0], (child)->ang[1], (child)->ang[2])
/* motion's frame at this step. the frame moves on 1 / slowdown a step. it
 * starts over at 0 after the motion's last frame */
#define MOTION_FRAME(motion, slowdown)                                         \
    ((game_state.steps %                                                       \
      (((motion)->nbFrame * (slowdown)) - ((slowdown) - 1))) *                 \
     (1.0F / (slowdown)))

#ifdef CORE_KNUCKLES_WATER
/* @hack type: camera 0's land_block_mask read as game_state_land_block_mask_0,
 * where game_state.land_block_mask[0] would do */
#define BLOCK_MASK game_state_land_block_mask_0
#else
/* @hack type: camera 0's land_block_mask read through a cast of the array's
 * address, where land_block_mask[0] would do */
#define BLOCK_MASK (*(Sint32 *)&game_state.land_block_mask)
#endif

void core_draw_at_children(NJS_CNK_MODEL *model, NJS_CNK_OBJECT *root,
                           Uint32 blocks)
{
    NJS_CNK_OBJECT *child;
    if (BLOCK_MASK & blocks) {
        for (child = root->child; child; child = child->sibling) {
            njPushMatrixEx();
            PLACE_CHILD(child);
            njCnkEasyDrawModel(model);
            njPopMatrixEx();
        }
    }
}

void core_draw_child_models(NJS_CNK_OBJECT *root, Uint32 blocks)
{
    NJS_CNK_OBJECT *child;
    if (BLOCK_MASK & blocks) {
        for (child = root->child; child; child = child->sibling) {
            njPushMatrixEx();
            PLACE_CHILD(child);
            njCnkEasyDrawModel(child->model);
            njPopMatrixEx();
        }
    }
}

/* a coordinate of 0 or more snaps to the UV_GRID line at or below it. one
 * within UV_SLACK below the next line snaps to that line instead. C's
 * division takes a negative coordinate toward 0 */
#define UV_GRID 16
#define UV_SLACK 2
#pragma inline(core_snap_uv)
int core_snap_uv(int coordinate)
{
    return ((coordinate + UV_SLACK) / UV_GRID) * UV_GRID;
}

/* @hack spelling: macros of bare statements, where a do-while around each
 * would do */
#define COPY_STRIP_HEADER()                                                    \
    for (i = 0; i < STRIP_HEADER_WORDS; i++) {                                 \
        *dst++ = *src++;                                                       \
    }
#define COPY_VERTEX()                                                          \
    *dst++ = *src++;                                                           \
    *dst++ = core_snap_uv(*src++) + du;                                        \
    *dst++ = core_snap_uv(*src++) + dv

void core_draw_shifted_model(NJS_CNK_MODEL *model, int du, int dv)
{
    Sint16 *src;
    Sint16 *dst;
    Sint16 *saved;
    Sint16 count;
    int i;
    src = saved = model->plist;
    dst = (Sint16 *)WORK_AREA;
    COPY_STRIP_HEADER();
    for (count = *dst++ = *src++; count != NJD_CE; count = *dst++ = *src++) {
        for (i = 0; i < njAbs(count); i++) {
            COPY_VERTEX();
        }
    }
    model->plist = (Sint16 *)WORK_AREA;
    njCnkEasyDrawModel(model);
    model->plist = saved;
}

void core_draw_shifted_object(NJS_CNK_OBJECT *object, int du, int dv)
{
    NJS_CNK_MODEL *model;
    Sint16 *src;
    Sint16 *dst;
    Sint16 *saved;
    Sint16 count;
    int i;
    model = object->model;
    src = saved = model->plist;
    dst = (Sint16 *)WORK_AREA;
    COPY_STRIP_HEADER();
    for (count = *dst++ = *src++; count != NJD_CE; count = *dst++ = *src++) {
        for (i = 0; i < njAbs(count); i++) {
            COPY_VERTEX();
        }
    }
    model->plist = (Sint16 *)WORK_AREA;
    njCnkEasyDrawObject(object);
    model->plist = saved;
}

void core_draw_sheet_at_children(NJS_CNK_MODEL *model, NJS_CNK_OBJECT *root,
                                 Uint32 rate, Uint32 blocks)
{
    NJS_CNK_OBJECT *child;
    Sint16 *src;
    Sint16 *dst;
    Sint16 *saved;
    Sint16 count;
    int du;
    int dv;
    int i;
    if (!(game_state.land_block_mask[0] & blocks)) {
        return;
    }
    child = root->child;
    du = CELL_U(rate);
    dv = CELL_V(rate);
    src = saved = model->plist;
    dst = (Sint16 *)WORK_AREA;
    COPY_STRIP_HEADER();
    for (count = *dst++ = *src++; count != NJD_CE; count = *dst++ = *src++) {
        for (i = 0; i < njAbs(count); i++) {
            COPY_VERTEX();
        }
    }
    model->plist = (Sint16 *)WORK_AREA;
    for (; child; child = child->sibling) {
        njPushMatrixEx();
        PLACE_CHILD(child);
        njCnkEasyDrawModel(model);
        njPopMatrixEx();
    }
    model->plist = saved;
}

void core_draw_sheet_children(NJS_CNK_OBJECT *root, Uint32 rate, Uint32 blocks)
{
    NJS_CNK_OBJECT *child;
    int du;
    int dv;
    if (game_state.land_block_mask[0] & blocks) {
        child = root->child;
        du = CELL_U(rate);
        dv = CELL_V(rate);
        for (; child; child = child->sibling) {
            njPushMatrixEx();
            PLACE_CHILD(child);
            core_draw_shifted_model(child->model, du, dv);
            njPopMatrixEx();
        }
    }
}

void core_draw_sheet_object(NJS_CNK_OBJECT *object, Uint32 rate, Uint32 blocks)
{
    int du;
    int dv;
    if (game_state.land_block_mask[0] & blocks) {
        du = CELL_U(rate);
        dv = CELL_V(rate);
        njPushMatrixEx();
        core_draw_shifted_object(object, du, dv);
        njPopMatrixEx();
    }
}

void core_draw_texture_children(NJS_CNK_OBJECT *root, int texture,
                                Uint32 blocks)
{
    NJS_CNK_OBJECT *child;
    int word;
    if (BLOCK_MASK & blocks) {
        for (child = root->child; child; child = child->sibling) {
            word = (child->model->plist[WATER_TID] & TEXTURE_FLAGS) | texture;
            child->model->plist[WATER_TID] = word;
            njPushMatrixEx();
            PLACE_CHILD(child);
            njCnkEasyDrawModel(child->model);
            njPopMatrixEx();
        }
    }
}
#ifndef CORE_KNUCKLES_WATER
void core_draw_texture_object(NJS_CNK_OBJECT *object, int texture,
                              Uint32 blocks)
{
    Sint16 *tiny;
    int word;
    if (BLOCK_MASK & blocks) {
        /* @hack spelling: the word reached through tiny, where
         * plist[WATER_TID] would do */
        tiny = &object->model->plist[WATER_TID];
        word = *tiny & TEXTURE_FLAGS;
        *tiny = texture | word;
        njPushMatrixEx();
        PLACE_CHILD(object);
        njCnkEasyDrawModel(object->model);
        njPopMatrixEx();
    }
}
#endif
