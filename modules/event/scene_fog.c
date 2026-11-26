#include <ninja.h>
#include <sg_xpt.h>
#include <stdio.h>

#include "event/scene_fog.h"
#include "shared/fog.h"

Bool event_editing_fog = FALSE;
struct fog_setting event_editor_fog;
static struct fog_setting scene_fogs[EVENT_SCENE_FOGS];

void event_fog_start(void)
{
    int scene;

    fog_clear(&event_editor_fog);
    for (scene = 0; scene < EVENT_SCENE_FOGS; scene++) {
        fog_clear(&scene_fogs[scene]);
    }
    fog_start();
    fog_set(scene_fogs);
    event_editing_fog = FALSE;
}

void event_fog_stop(void)
{
    fog_stop();
}

void event_fog_load(int event, int scenes)
{
    char name[32];
    int scene;

    for (scene = 0; scene < scenes; scene++) {
        /* @bug memory: fog_read writes scene 30's fog file past scene_fogs,
         * since the bound at 0x8c507f2e admits it */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the bound refuses scene 30 */
        if (scene >= EVENT_SCENE_FOGS) {
            return;
        }
#else
        if (scene > EVENT_SCENE_FOGS) {
            return;
        }
#endif
        sprintf(name, "e%04d_%02d.fog", event, scene);
        fog_read(name, &scene_fogs[scene]);
    }
}

void event_fog_select(int scene)
{
    if (event_editing_fog) {
        fog_set(&event_editor_fog);
        njPrint(NJM_LOCATION(4, 12), "FOG EDIT MODE!");
        return;
    }
    /* @bug memory: the fog task reads past scene_fogs for scene 30, since
     * the bound at 0x8c507fa0 admits it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the bound refuses scene 30 and a
     * negative scene */
    if (scene < 0 || scene >= EVENT_SCENE_FOGS) {
        return;
    }
#else
    if (scene > EVENT_SCENE_FOGS) {
        return;
    }
#endif
    fog_set(&scene_fogs[scene]);
}
