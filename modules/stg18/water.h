/* Dry Lagoon's water surfaces, each showing the step's frame of
 * lagoon_water_texlist */
#ifndef STG18_WATER_H
#define STG18_WATER_H
#include <ninja.h>
#include <sg_xpt.h>

/* the surfaces' textures. kd_dl_mizu2_001 to 015 lie at 0 to 14, and
 * kd_dl_mizu_001 to 014 at 15 to 28. tx_mine001 lies at 29. the PVM
 * stg18_suimenanim fills the list, as dry_lagoon_pvms says */
extern NJS_TEXLIST lagoon_water_texlist;
/* kd_dl_mizu_001 to 014: the LAGOON_WATER_FRAMES textures of
 * lagoon_water_texlist from LAGOON_WATER_FIRST_FRAME */
#define LAGOON_WATER_FRAMES 14
#define LAGOON_WATER_FIRST_FRAME 15
/* LAGOON_WATER_TID_AT indexes the halfword of a water model's polygon list
 * that holds its texture's index. it is the second halfword of the
 * NJD_CT_TID chunk, which follows a material chunk of LAGOON_WATER_MATERIAL
 * halfwords. lagoon_water_draw_around's model has no material chunk */
#define LAGOON_WATER_MATERIAL 6
#define LAGOON_WATER_TID_AT (LAGOON_WATER_MATERIAL + 1)

/* draw the water's surfaces. a surface draws where camera 0's
 * land_block_mask holds any of its land blocks, whichever camera draws. each
 * stands at its own offset from the current matrix, which the call leaves as
 * it was. lagoon_water_draw's surfaces show kd_dl_mizu_001 to 014 in turn, a
 * frame a step. lagoon_water_draw_under's show kd_dl_mizu2_001 to 015. once a
 * surface draws, lagoon_water_texlist is the current texture list */
void lagoon_water_draw(void);
void lagoon_water_draw_under(void);

/* draw a flat surface of water centered on (x, y, z). it reaches half_x
 * either side in x, and half_z in z. a ring of six of its vertices lies about
 * point, and another vertex lies at point. the water inside the ring thins
 * toward point. the ring's radius is 25, less 0.5 for each unit point lies
 * above or below y. it is never below 0. the ring and the vertex at point stay
 * within the surface's edges. a null point draws nothing.
 *
 * the surface shows kd_dl_mizu_001 to 014, as lagoon_water_draw's do. the
 * call leaves the current matrix as it was, and lagoon_water_texlist the
 * current texture list */
void lagoon_water_draw_around(Float x, Float y, Float z, Float half_x,
                              Float half_z, NJS_POINT3 *point);

#endif
