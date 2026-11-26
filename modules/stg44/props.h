#ifndef STG44_PROPS_H
#define STG44_PROPS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the carriages' and the platform's texture lists for texture_load_shared,
 * each ended by null */
extern NJS_TEXLIST *mad_space_carriage_textures[];
extern NJS_TEXLIST *mad_space_platform_textures[];
/* the inits of the two carriages and the platform, which the carriage manager
 * creates and starts. it sets each task's unk_3c to the path the task follows.
 * each init creates a trailer behind its task. where the work's tick isn't 0,
 * the trailer carries the emerald whose id is tick - 1 */
void mad_space_carriage_init(struct task *task);
void mad_space_platform_init(struct task *task);

/* compile SL's three models at buffer, and return the first byte past them. SL
 * draws the compiled models only where all three compiled */
Uint8 *mad_space_sl_build(Uint8 *buffer);

#endif
