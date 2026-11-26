/* FUN, PANELAREA, REDLAMP, WIND, WIND2 and FOGSW in Security Hall's object
 * list */
#ifndef STG08_SECURITY_PROPS_H
#define STG08_SECURITY_PROPS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern NJS_TEXLIST *security_fan_textures[];
extern NJS_TEXLIST *security_panel_textures[];
extern NJS_TEXLIST *security_red_lamp_textures[];

/* build the fan's model at buffer, and return the first free byte past it */
Uint8 *security_fan_build(Uint8 *buffer);
void security_fan_init(struct task *task);
void security_panel_init(struct task *task);
void security_red_lamp_step(struct task *task);
/* WIND blows within a fixed width and depth, anywhere below its top. WIND2
 * blows within a box whose half sizes are scl's */
void security_wind_init(struct task *task);
void security_wind2_init(struct task *task);
void security_fog_switch_init(struct task *task);

#endif
