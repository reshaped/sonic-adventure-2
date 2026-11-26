/* the sea of Metal Harbor, Weapons Bed, Weapons Bed's two-player deck and the
 * first walker duel. up to SEA_WAVES waves ring out across it */
#ifndef SHARED_SEA_H
#define SHARED_SEA_H
#include <ninja.h>
#include <sg_xpt.h>

/* the most waves the sea holds at once */
#define SEA_WAVES 5

/* a wave: a ring that spreads from its center, one unit each 0.07 steps. a
 * vertex's age in the wave is the wave's age less the steps the ring took to
 * reach the vertex. while that age is at least 0 and below life, the
 * vertex's swing in the wave is
 *
 *   sin(rate * age) * (1 - age / life) / (1 + distance / 200)
 *
 * distance is the vertex's from the center. the wave sets the vertex's height
 * to pos.z, the wave's height, times the swing, but no higher than
 * sea_wave_height_max. the last wave in the array to set a vertex's height
 * wins. a vertex no wave sets keeps the model's height */
struct sea_wave {
    /* pos.x and pos.y are the center's x and z, and pos.z the wave's height */
    NJS_POINT3 pos;
    Angle rate; /* the swing's turn a step */
    Sint32 age; /* in steps, from sea_wave_add's 0 */
    Sint32 life; /* in steps */
};

extern NJS_TEXLIST sea_texlist;

/* the sea's model, with its place in the stage. each stage's source defines
 * it */
/* @hack type: sea_object an array of one, where the object would do */
extern NJS_CNK_OBJECT sea_object[];

/* how many waves are in use */
extern Sint32 sea_wave_count;
/* the highest a wave sets a vertex. it starts at 800, and a stage may change
 * it */
extern Float sea_wave_height_max;

/* age each wave a step, and move its center by dx in x and dy in z. a wave
 * goes once every vertex within 6500 of its center is past its life. the
 * waves after it move up the array */
void sea_waves_step(Float dx, Float dy);
/* start a wave, with age 0, and return it for the caller to set the rest. it
 * returns null where SEA_WAVES waves are in use */
struct sea_wave *sea_wave_add(void);
/* draw sea_object's model at the current matrix, with its vertices copied to
 * WORK_AREA, over what was there. the model's vertex list must be one
 * NJD_CV_D8 chunk, then its end. the chunk must hold a square grid of
 * vertices, row by row.
 *
 * the copy takes the waves' heights. its colors are the model's, scrolled a
 * row each 50 steps. they blend toward white at alpha 0xd0. the blend is half
 * the sum of the waves' swings at the vertex, from 0 up to 1. the model's
 * texture steps through 20 frames, one each 2 steps */
void sea_draw(void);

#endif
