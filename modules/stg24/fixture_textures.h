/* the null-ended texture lists of Eternal Engine's objects, which
 * engine_object_groups holds, and the names of STEP's six paths */
#ifndef STG24_FIXTURE_TEXTURES_H
#define STG24_FIXTURE_TEXTURES_H

#include <ninja.h>

extern NJS_TEXLIST *engine_step_texlists[];
extern NJS_TEXLIST *engine_hatch_texlists[];
extern NJS_TEXLIST *engine_reactor_texlists[];
extern NJS_TEXLIST *engine_kazobj_texlists[];
extern NJS_TEXLIST *engine_lamp_texlists[];
extern NJS_TEXLIST *engine_ditch_texlists[];
extern NJS_TEXLIST *engine_machine_texlists[];
extern const char *engine_step_path_names[6];

#endif
