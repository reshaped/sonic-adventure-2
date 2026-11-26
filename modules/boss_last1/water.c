#include <ninja.h>
#include <sg_xpt.h>
#include "boss_last1/arena.h"
#include "boss_last1/stage.h"
#include "boss_last1/terrain.h"
#include "system/game_state.h"

/* the halfword of the water's polygon list that holds its texture's index.
 * it follows the material chunk and the texture chunk's head */
#define WATER_TID 7
/* biolizard_water_texlist's textures, a power of two */
#define WATER_FRAMES 16

void biolizard_water_draw(void)
{
    NJS_CNK_OBJECT *water = &biolizard_water_object;
    Sint16 *texture = water->model->plist;
    int id;
    /* @hack spelling: the halfword stepped to, read, masked and set again
     * through texture and id, where one assignment to plist[WATER_TID] would
     * do */
    texture += WATER_TID;
    id = *texture;
    id &= NJD_FFM_BF;
    id |= game_state.steps & (WATER_FRAMES - 1);
    *texture = id;
    njSetTexture(&biolizard_water_texlist);
    njPushMatrixEx();
    /* @hack redundant: a translate by 0, which changes nothing */
    njTranslate(NULL, 0, 0, 0);
    njCnkEasyDrawObject(water);
    njPopMatrixEx();
}
