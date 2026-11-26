/* the lights the game hands to Ninja's chunk and direct drawers */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/light.h"
#include "system/game_state.h"

/* nothing calls these. none is static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set one part of the light at index, which must be below LIGHTS, unchecked */
void light_set_dir(int index, Float x, Float y, Float z);
void light_set_intensity(int index, Float intensity);
void light_set_color(int index, Float red, Float green, Float blue);
/* keep player's light in light_saved, for light_restore to select again.
 * only player's low bit is read */
void light_save(int player);
void light_restore(int player);
/* NOLINTEND(misc-use-internal-linkage) */

struct light lights[LIGHTS];
Sint8 light_camera;
Sint8 light_after_draw;
Sint8 light_selected[GAME_PLAYERS];
Sint8 light_saved[GAME_PLAYERS];

void light_init(void)
{
    light_camera = 0;
    light_after_draw = LIGHT_LAND;
    light_selected[0] = 0;
    light_selected[1] = 0;
    light_saved[0] = 0;
    light_saved[1] = 0;
}

void light_set(int index, const struct light *light)
{
    lights[index] = *light;
}

void light_set_dir(int index, Float x, Float y, Float z)
{
    lights[index].dir.x = x;
    lights[index].dir.y = y;
    lights[index].dir.z = z;
}

void light_set_intensity(int index, Float intensity)
{
    lights[index].intensity = intensity;
}

void light_set_ambient(int index, Float ambient)
{
    lights[index].ambient = ambient;
}

void light_set_color(int index, Float red, Float green, Float blue)
{
    lights[index].r = red;
    lights[index].g = green;
    lights[index].b = blue;
}

void light_select_camera(Sint8 index)
{
    light_camera = index;
}

void light_select_after_draw(Sint8 index)
{
    light_after_draw = index;
}

void light_select(int index, int player)
{
    light_selected[player & 1] = index;
}

/* @hack spelling: light_apply's body written again, where
 * light_apply(light_selected[player & 1]) would do */
void light_apply_player(int player)
{
    struct light *light = &lights[light_selected[player & 1]];
    NJS_VECTOR dir;

    /* @hack spelling: the direction copied member by member, where dir =
     * light->dir would do */
    dir.x = light->dir.x;
    dir.y = light->dir.y;
    dir.z = light->dir.z;
    njCalcVector(NULL, &dir, &dir);
    simple_light_set_direction(dir.x, dir.y, dir.z);
    easy_light_set_direction(dir.x, dir.y, dir.z);
    njCnkSetEasyLightIntensity(light->intensity, light->ambient);
    njCnkSetEasyLightColor(light->r, light->g, light->b);
    njSetDirectLight(dir.x, dir.y, dir.z);
    njSetDirectLightIntensity(light->intensity, light->ambient);
    njSetDirectLightColor(light->r, light->g, light->b);
    njCnkSetSimpleLightIntensity(light->intensity, light->ambient);
    njCnkSetSimpleLightColor(light->r, light->g, light->b);
}

void light_apply(int index)
{
    struct light *light = &lights[index];
    NJS_VECTOR dir;

    /* @hack spelling: the direction copied member by member, where dir =
     * light->dir would do */
    dir.x = light->dir.x;
    dir.y = light->dir.y;
    dir.z = light->dir.z;
    njCalcVector(NULL, &dir, &dir);
    simple_light_set_direction(dir.x, dir.y, dir.z);
    easy_light_set_direction(dir.x, dir.y, dir.z);
    njCnkSetEasyLightIntensity(light->intensity, light->ambient);
    njCnkSetEasyLightColor(light->r, light->g, light->b);
    njSetDirectLight(dir.x, dir.y, dir.z);
    njSetDirectLightIntensity(light->intensity, light->ambient);
    njSetDirectLightColor(light->r, light->g, light->b);
    njCnkSetSimpleLightIntensity(light->intensity, light->ambient);
    njCnkSetSimpleLightColor(light->r, light->g, light->b);
}

void light_save(int player)
{
    /* @hack spelling: player reused for its own low bit, where
     * light_saved[player & 1] = light_selected[player & 1] would do */
    player &= 1;
    light_saved[player] = light_selected[player];
}

void light_restore(int player)
{
    /* @hack spelling: player reused for its own low bit, where
     * light_selected[player & 1] = light_saved[player & 1] would do */
    player &= 1;
    light_selected[player] = light_saved[player];
}
