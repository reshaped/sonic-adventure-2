/* the macros that the Neutral, Hero and Dark Gardens share. they load and
 * release the gardens' textures, set their lights and view, and play their
 * ambient sounds */
#ifndef SHARED_GARDEN_SETUP_H
#define SHARED_GARDEN_SETUP_H
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/stack.h"
#include "chao/egg.h"
#include "chao/garden_scene.h"
#include "chao/messages.h"
#include "chao/model.h"
#include "chao/toy_art.h"
#include "chao/walk_screen.h"
#include "collision/world.h"
#include "draw/light.h"
#include "object/set.h"
#include "shared/object_light.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/loader.h"
#include "system/texture.h"
#include "system/util.h"

extern struct texture_pvm garden_pvms[];
extern NJS_TEXLIST **garden_texture_groups[];

#define GARDEN_LIGHTS                                                          \
    {{{0.1F, -0.9F, 0.3F}, 1.0F, 0.6F, 1.0F, 1.0F, 1.0F},                      \
     {{0.3F, 0.1F, 1.0F}, 0.5F, 0.0F, 0.4F, 0.6F, 0.5F}}

/* @hack spelling: a garden's textures, lights and view set by these macros in
 * place, where functions taking each garden's own names would do */
#define GARDEN_LOAD_TEXTURES(area_name, area)                                  \
    do {                                                                       \
        texture_load_pvm("AL_MINIMAL_TEX", &garden_animal_texlist);            \
        texture_load_pvm("AL_OBJECT", &egg_texlist);                           \
        texture_load_pvm("AL_TOY", &garden_toy_texlist);                       \
        texture_load_pvm((area_name), (area));                                 \
        texture_load_shared(garden_pvms, garden_texture_groups);               \
    } while (0)

#define GARDEN_SET_LIGHTING(area_lights)                                       \
    do {                                                                       \
        njCnkSetAmbient(0.3F, 0.1F, 0.1F);                                     \
        easy_light_set_direction(0.0F, 1.0F, 0.0F);                            \
        njCnkSetEasyLightIntensity(1.0F, 0.2F);                                \
        njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);                              \
        light_set(0, &(area_lights)[0]);                                       \
        light_set(1, &(area_lights)[1]);                                       \
        light_set(2, &(area_lights)[0]);                                       \
        light_set(3, &(area_lights)[0]);                                       \
        light_select(0, 0);                                                    \
        light_select(0, 1);                                                    \
        light_select_camera(0);                                                \
        light_select_after_draw(LIGHT_LAND);                                   \
        light_apply_player(0);                                                 \
        lights_load("stg50_light.bin");                                        \
    } while (0)

/* the angle of view of the gardens and the lobby */
#define GARDEN_VIEW_ANGLE 0x24fa

#define GARDEN_START_VIEW(music)                                               \
    do {                                                                       \
        music_fade_in(0);                                                      \
        music_play_name((music));                                              \
        njSetBackColor(0xff3333ff, 0x003333ff, 0x00bbbbff);                    \
        camera_set_view_angle(0, GARDEN_VIEW_ANGLE, 0);                        \
        garden_camera_start();                                                 \
        garden_camera_create();                                                \
    } while (0)

#define GARDEN_RELEASE_RESOURCES(layout, area)                                 \
    do {                                                                       \
        garden_messages_free();                                                \
        texture_release_shared(garden_pvms, garden_texture_groups);            \
        njReleaseTexture(&garden_animal_texlist);                              \
        njReleaseTexture(&egg_texlist);                                        \
        njReleaseTexture(&garden_toy_texlist);                                 \
        njReleaseTexture((area));                                              \
        walk_release_textures();                                               \
        set_end();                                                             \
        if ((layout)) {                                                        \
            loader_free((layout));                                             \
            (layout) = NULL;                                                   \
        }                                                                      \
    } while (0)

/* @hack spelling: the ambient sound's test and play as a macro, where a
 * function would do */
#define GARDEN_AMBIENT_SOUND(frame, interval, chance, id)                      \
    do {                                                                       \
        if ((frame) % (interval) == 0 && (rand() * RAND_SCALE) < (chance)) {   \
            sound_play((id), NULL, 0, 0);                                      \
        }                                                                      \
    } while (0)

#endif
