/* the models and texture lists of CAMV and CAMH, the vertical and horizontal
 * patrol cameras. the two lists share the six texture names below */
#ifndef SHARED_PATROL_CAMERA_TEX_H
#define SHARED_PATROL_CAMERA_TEX_H
#include <ninja.h>

extern NJS_CNK_OBJECT vertical_camera_object[1];
extern NJS_CNK_OBJECT horizontal_camera_object[1];
extern NJS_TEXLIST vertical_camera_texlist;
extern NJS_TEXLIST horizontal_camera_texlist;

/* The shared PVM names each occupy eleven characters and their terminator. */
enum { PATROL_CAMERA_TEXTURE_NAME_SIZE = 12 };

extern const char camera_miu32_ee001[PATROL_CAMERA_TEXTURE_NAME_SIZE];
extern const char camera_miu64_cg004[PATROL_CAMERA_TEXTURE_NAME_SIZE];
extern const char camera_miu64_cg008[PATROL_CAMERA_TEXTURE_NAME_SIZE];
extern const char camera_miu64_cg010[PATROL_CAMERA_TEXTURE_NAME_SIZE];
extern const char camera_miu64_cg020[PATROL_CAMERA_TEXTURE_NAME_SIZE];
extern const char camera_miu64_ee001[PATROL_CAMERA_TEXTURE_NAME_SIZE];

#endif
