/* Bobbing end sign. The stage owns the lighting parameters and collision
 * sphere. */

#include <ninja.h>
#include <sg_xpt.h>

#include "shared/sign_end_art.h"
#include "stg43/objects.h"

#include "stg43/sign_end/text.inc"

/* the SET editor's captions. only sign_end_task.h's unreached block names
 * them, so that SHC places them in this source's constants */
#define SIGN_END_EDITOR_ANGLE_X "<- ANGLE X"
#define SIGN_END_EDITOR_ANGLE_Y "<- ANGLE Y"
#define SIGN_END_EDITOR_ANGLE_Z "<- ANGLE Z"
#define SIGN_END_EDITOR_ROT "<- ROT"

static NJS_TEXNAME sign_end_textures[4] = {
#include "stg43/sign_end/sign_end_textures.inc"
};

NJS_TEXLIST sign_end_texlist = {
#include "stg43/sign_end/sign_end_texlist.inc"
};

static Sint16 sign_end_face_polygons[37] = {
#include "stg43/sign_end/sign_end_face_polygons.inc"
};

static Sint32 sign_end_face_vertices[35] = {
#include "stg43/sign_end/sign_end_face_vertices.inc"
};

static NJS_CNK_MODEL sign_end_face_mesh = {
#include "stg43/sign_end/sign_end_face_mesh.inc"
};

static NJS_CNK_OBJECT sign_end_face = {
#include "stg43/sign_end/sign_end_face.inc"
};

static Sint16 sign_end_polygons[425] = {
#include "stg43/sign_end/sign_end_polygons.inc"
};

static Sint32 sign_end_vertices[371] = {
#include "stg43/sign_end/sign_end_vertices.inc"
};

static NJS_CNK_MODEL sign_end_mesh = {
#include "stg43/sign_end/sign_end_mesh.inc"
};

NJS_CNK_OBJECT sign_end_object = {
#include "stg43/sign_end/sign_end_object.inc"
};

NJS_TEXLIST *sign_end_texlists[2] = {
#include "stg43/sign_end/sign_end_texlists.inc"
};

#define COSMIC_WALL_SIGN
#include "shared/orbital/sign_end_task.h" /* IWYU pragma: keep -- the code it compiles here */
