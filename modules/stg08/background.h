/* Security Hall's land, fogs and background task. the task shows the land of
 * the floor the camera is on. it takes security_alt_fog while its submode
 * holds SECURITY_ALT_FOG, which FOGSW sets */
#ifndef STG08_BACKGROUND_H
#define STG08_BACKGROUND_H
#include <sg_xpt.h>

#include "collision/world.h"
#include "shared/fog.h"

struct task;

extern struct task *security_background;
extern struct fog_setting security_fog;
extern struct fog_setting security_alt_fog;
extern struct land_table security_land;
extern const char security_land_filenames[301];
extern NJS_TEXLIST security_hall_terrain_textures;
extern Float security_floor_height;
/* a land entry with this flag shows on either floor */
#define SECURITY_BOTH_FLOORS 0x200
#define SECURITY_ALT_FOG 1

void security_background_step(struct task *task);
void security_background_draw(struct task *task);
void security_background_destroy(struct task *task);

#endif
