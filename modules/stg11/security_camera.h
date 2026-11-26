/* the security camera, CAM in Iron Gate's object list. it is the camera of
 * shared/security_camera.h, compiled again for this stage */
#ifndef STG11_SECURITY_CAMERA_H
#define STG11_SECURITY_CAMERA_H
#include <ninja.h>

struct task;

/* the camera's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *iron_gate_security_camera_texlists[];

/* the object list's step for CAM. it sets task up as security_camera_init
 * does. the step its head falls in still registers the camera's record */
void iron_gate_security_camera_init(struct task *task);

#endif
